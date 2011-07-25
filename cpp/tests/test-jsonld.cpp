/*
 * Copyright (c) 2007-2011 Digital Bazaar, Inc. All rights reserved.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __STDC_CONSTANT_MACROS

#include <cstdio>

#include "monarch/test/Test.h"
#include "monarch/test/TestModule.h"
#include "monarch/data/json/JsonLd.h"
#include "monarch/data/json/JsonReader.h"
#include "monarch/data/json/JsonWriter.h"
#include "monarch/logging/Logging.h"
#include "monarch/io/File.h"
#include "monarch/io/FileInputStream.h"
#include "monarch/io/FileList.h"
#include "monarch/util/StringTools.h"
#include "monarch/validation/Validation.h"

using namespace std;
using namespace monarch::test;
using namespace monarch::data::json;
using namespace monarch::io;
using namespace monarch::rt;
using namespace monarch::util;
namespace v = monarch::validation;

namespace mo_test_jsonld
{

// hack just so code compiles even w/o test suite support
#ifndef HAVE_JSON_LD_TEST_SUITE
#define JSON_LD_TEST_SUITE_DIR ""
#endif

static void _readFile(const char* root, const char* name, DynamicObject& data)
{
   string fullPath;
   fullPath.assign(File::join(root, name));

   File f(fullPath.c_str());
   if(!f->exists() || !f->isFile())
   {
      ExceptionRef e = new Exception("Invalid test file.");
      e->getDetails()["name"] = name;
      Exception::set(e);
   }
   assertNoExceptionSet();
   FileInputStream is(f);
   JsonReader r(false);
   r.start(data);
   bool success = r.read(&is) && r.finish();
   if(!success)
   {
      ExceptionRef e = new Exception("Failure reading test file.");
      e->getDetails()["name"] = name;
      Exception::push(e);
   }
   assertNoExceptionSet();
   is.close();
}

static void _runJsonLdTestSuiteTest(
   TestRunner& tr, const char* root, DynamicObject& test)
{
   tr.test(test["name"]);

   DynamicObject& type = test["type"];

   // read input
   DynamicObject input;
   _readFile(root, test["input"], input);

   // read expected output
   DynamicObject expect;
   _readFile(root, test["expect"], expect);

   DynamicObject output;

   if(type == "normalize")
   {
      assertNoException(
         JsonLd::normalize(input, output));
   }
   else if(type == "expand")
   {
      // expand
      assertNoException(
         JsonLd::removeContext(input, output));
   }
   else if(type == "compact")
   {
      // sanity check
      v::ValidatorRef tv = new v::Map(
         "context", new v::Type(String),
         NULL);
      assertNoException(
         tv->isValid(test));

      // read context
      DynamicObject context;
      _readFile(root, test["context"], context);

      // compact
      assertNoException(
         JsonLd::changeContext(context, input, output));
   }
   else if(type == "frame")
   {
      // sanity check
      v::ValidatorRef tv = new v::Map(
         "frame", new v::Type(String),
         NULL);
      assertNoException(
         tv->isValid(test));

      // read frame
      DynamicObject frame;
      _readFile(root, test["frame"], frame);

      // reframe
      assertNoException(
         JsonLd::frame(input, frame, output));
   }

   assertNamedDynoCmp("expect", expect, "output", output);

   tr.pass();
}

static void runJsonLdTestSuite(TestRunner& tr)
{
   tr.group("JSON-LD");

   File dir(JSON_LD_TEST_SUITE_DIR);
   if(!dir->exists())
   {
      tr.test("JSON-LD Test Suite");
      tr.fail(JSON_LD_TEST_SUITE_DIR " not found");
   }
   else if(!dir->isDirectory())
   {
      tr.test("JSON-LD Test Suite");
      tr.fail(JSON_LD_TEST_SUITE_DIR " is not a directory");
   }
   else
   {
      FileList list;
      dir->listFiles(list);
      IteratorRef<File> i = list->getIterator();
      while(i->hasNext())
      {
         File& f = i->next();
         if(f->isFile())
         {
            const char* ext = f->getExtension();
            if(strcmp(ext, ".test") == 0)
            {
               //string grp = StringTools::format("%s", f->getBaseName());
               //tr.group(grp.c_str());

               // read test file
               DynamicObject tests;
               FileInputStream is(f);
               JsonReader r;
               r.start(tests);
               assertNoException(
                  r.read(&is) && r.finish());
               is.close();

               // sanity check
               v::ValidatorRef tv = new v::Map(
                  "group", new v::Type(String),
                  "tests", new v::Each(new v::Map(
                     "name", new v::Type(String),
                     "input", new v::Type(String),
                     "expect", new v::Type(String),
                     NULL)),
                  NULL);
               assertNoException(
                  tv->isValid(tests));

               tr.group(tests["group"]);

               // process each test
               DynamicObjectIterator i = tests["tests"].getIterator();
               while(i->hasNext())
               {
                  _runJsonLdTestSuiteTest(tr, dir->getPath(), i->next());
               }

               tr.ungroup();
            }
         }
      }
   }

   tr.ungroup();
}

static bool run(TestRunner& tr)
{
   if(tr.isDefaultEnabled() || tr.isTestEnabled("json-ld"))
   {
#ifdef HAVE_JSON_LD_TEST_SUITE
      runJsonLdTestSuite(tr);
#endif
   }
   return true;
}

} // end namespace

MO_TEST_MODULE_FN("monarch.tests.jsonld.test", "1.0", mo_test_jsonld::run)
