/*
 * Copyright (c) 2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/test/TestLoader.h"

#include "monarch/kernel/MicroKernel.h"
#include "monarch/rt/Exception.h"
#include "monarch/test/Test.h"
#include "monarch/test/Testable.h"

#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace monarch::app;
using namespace monarch::config;
using namespace monarch::kernel;
using namespace monarch::modest;
using namespace monarch::rt;
using namespace monarch::test;

#define TEST_LOADER_NAME "monarch.test.TestLoader"

TestLoader::TestLoader()
{
}

TestLoader::~TestLoader()
{
}

bool TestLoader::initConfigs(Config& defaults)
{
   // defaults
   Config& c = defaults[ConfigManager::MERGE][TEST_LOADER_NAME];
   c["level"] = TestRunner::Names;
   c["continueAfterException"] = false;
   c["listTests"] = false;
   c["tests"]->setType(Array);
   c["modules"]->setType(Array);
   return true;
}

DynamicObject TestLoader::getCommandLineSpec(Config& cfg)
{
   // initialize config
   Config& c = cfg[ConfigManager::MERGE][TEST_LOADER_NAME];
   c["tests"]->setType(Array);
   c["modules"]->setType(Array);

   DynamicObject spec;
   spec["help"] =
"Test options:\n"
"  -l, --level LEVEL   Adjust test output level to LEVEL. (default: 3)\n"
"                         0: No output.\n"
"                         1: Final results.\n"
"                         2: Progress (.=success, W=warning, F=failure).\n"
"                         3: Test names and PASS/WARNING/FAIL status.\n"
"                         4: Same as 3, plus test time.\n"
"                      All levels have exit status of 0 on success.\n"
"      --continue-after-exception\n"
"                      Continue after failure. (default: false).\n"
"      --list-tests    List all named tests.\n"
"  -t, --test TEST     Add TEST to list of enabled tests. (default: unit).\n"
"      --test-module MODULE\n"
"                      Add MODULE to list of enabled test modules.\n"
"                      (default: all testable modules).\n"
"\n";

   DynamicObject opt;

   opt = spec["options"]->append();
   opt["short"] = "-l";
   opt["long"] = "--level";
   opt["arg"]["root"] = c;
   opt["arg"]["path"] = "level";

   opt = spec["options"]->append();
   opt["long"] = "--continue-after-exception";
   opt["setTrue"]["root"] = c;
   opt["setTrue"]["path"] = "continueAfterException";

   opt = spec["options"]->append();
   opt["long"] = "--list-tests";
   opt["setTrue"]["root"] = c;
   opt["setTrue"]["path"] = "listTests";

   opt = spec["options"]->append();
   opt["short"] = "-t";
   opt["long"] = "--test";
   opt["append"] = c["tests"];
   opt["argError"] = "No test specified.";

   opt = spec["options"]->append();
   opt["long"] = "--test-module";
   opt["append"] = c["modules"];
   opt["argError"] = "No module specified.";

   return spec;
}

bool TestLoader::run(App* app)
{
   bool rval = true;

   Config cfg = app->getConfig()[TEST_LOADER_NAME];
   bool cont = cfg["continueAfterException"]->getBoolean();
   uint32_t cfgLevel = cfg["level"]->getUInt32();
   TestRunner::OutputLevel level;

   switch(cfgLevel)
   {
      case 0: level = TestRunner::None; break;
      case 1: level = TestRunner::Final; break;
      case 2: level = TestRunner::Progress; break;
      case 3: level = TestRunner::Names; break;
      default: level = TestRunner::Times; break;
   }

   TestRunner tr(app, cont, level);
   tr.setListTests(cfg["listTests"]->getBoolean());

   tr.group(NULL);

   // Special case "unit" testing: if any tests specified then disable "unit"
   // tests unless "unit" is specifically enabled.
   bool usingCustomTests = false;
   bool hasDefaultTest = false;

   // setup enabled tests
   {
      DynamicObjectIterator i = cfg["tests"].getIterator();
      while(i->hasNext())
      {
         const char* name = i->next()->getString();
         tr.enableTest(name);
         usingCustomTests = true;
         hasDefaultTest |= (strcmp(name, TestRunner::DEFAULT) == 0);
      }
   }

   // list of specified modules under test
   // if empty then test all modules
   DynamicObject customModules;
   customModules->setType(Map);

   // setup enabled modules
   {
      DynamicObjectIterator i = cfg["modules"].getIterator();
      while(i->hasNext())
      {
         customModules[i->next()->getString()] = true;
      }
   }

   // disable default test if using custom tests and default not specified
   if(usingCustomTests && !hasDefaultTest)
   {
      tr.enableTest(TestRunner::DEFAULT, false);
   }

   // load all monarch.test.TestModules and run them
   {
      ConfigManager* cm = app->getConfigManager();
      MicroKernel* k = app->getKernel();
      MicroKernel::ModuleApiList tests;
      k->getModuleApisByType("monarch.test.TestModule", tests);
      for(MicroKernel::ModuleApiList::iterator i = tests.begin();
         rval && i != tests.end(); ++i)
      {
         Module* m = dynamic_cast<Module*>(*i);
         Testable* f = dynamic_cast<Testable*>(*i);
         const char* name = m->getId().name;
         // only run test if no modules listed or current module listed
         if((customModules->length() == 0) ||
            (customModules->hasMember(name) &&
            customModules[name]->getBoolean()))
         {
            cm->saveState();
            tr.group(name);
            rval = f->run(tr);
            tr.ungroup();
            cm->restoreState();
         }
      }
   }

   assertNoExceptionSet();

   tr.ungroup();
   tr.done();

   return rval;
}
