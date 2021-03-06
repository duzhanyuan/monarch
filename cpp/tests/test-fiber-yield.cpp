/*
 * Copyright (c) 2009-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/test/Test.h"
#include "monarch/test/TestModule.h"
#include "monarch/fiber/FiberScheduler.h"
#include "monarch/modest/Kernel.h"
#include "monarch/util/Timer.h"

#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace std;
using namespace monarch::config;
using namespace monarch::fiber;
using namespace monarch::modest;
using namespace monarch::rt;
using namespace monarch::test;
using namespace monarch::util;

namespace mo_test_fiber_yield
{

class TestFiber : public Fiber
{
public:
   int count;

public:
   TestFiber(int n)
   {
      count = n;
   };
   virtual ~TestFiber() {};

   virtual void run()
   {
      //printf("Running test fiber '%d'\n", getId());

      int i = 0;
      for(; i < count; ++i)
      {
         //printf("Test fiber '%d' yielding...\n", getId());
         yield();
         //printf("Test fiber '%d' continuing.\n", getId());
      }

      //printf("Test fiber '%d' done with '%d' iterations.\n", getId(), i);
   }
};

static void runFiberYieldTest(TestRunner& tr)
{
   tr.group("Fiber Yield");

   tr.test("10 yielding fibers/10 iterations");
   {
      Kernel k;
      k.getEngine()->start();

      FiberScheduler fs;

      // queue up some fibers prior to starting
      for(int i = 0; i < 10; ++i)
      {
         fs.addFiber(new TestFiber(10));
      }

      uint64_t startTime = Timer::startTiming();
      fs.start(&k, 1);

      fs.waitForLastFiberExit(true);
      printf("time=%g secs... ", Timer::getSeconds(startTime));

      k.getEngine()->stop();
   }
   tr.passIfNoException();

   tr.ungroup();
}

static bool run(TestRunner& tr)
{
   if(tr.isTestEnabled("fiber-yield"))
   {
      runFiberYieldTest(tr);
   }
   return true;
}

} // end namespace

MO_TEST_MODULE_FN(
   "monarch.tests.fiber-yield.test", "1.0", mo_test_fiber_yield::run)
