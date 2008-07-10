/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/modest/ModuleLibrary.h"

using namespace std;
using namespace db::modest;
using namespace db::rt;

ModuleLibrary::ModuleLibrary(Kernel* k)
{
   mKernel = k;
}

ModuleLibrary::~ModuleLibrary()
{
   // unload all modules
   ModuleLibrary::unloadAllModules();
}

Module* ModuleLibrary::findModule(const ModuleId* id)
{
   Module* rval = NULL;
   
   // find module
   ModuleMap::iterator i = mModules.find(id);
   if(i != mModules.end())
   {
      rval = i->second->module;
   }
   
   return rval;
}

Module* ModuleLibrary::findModule(const char* name)
{
   // find module
   ModuleId id(name);
   return findModule(&id);
}

Module* ModuleLibrary::loadModule(const char* filename)
{
   Module* rval = NULL;
   
   mLoadLock.lock();
   {
      // try to load module
      ModuleInfo* mi = mLoader.loadModule(filename);
      if(mi != NULL)
      {
         // ensure the module isn't already loaded
         if(findModule(&mi->module->getId()) == NULL)
         {
            // initialize the module
            if(mi->module->initialize(mKernel))
            {
               // add Module to the map and list
               mModules[&mi->module->getId()] = mi;
               mLoadOrder.push_back(&mi->module->getId());
               rval = mi->module;
            }
            else
            {
               // could not initialize module, so unload it
               ExceptionRef e = Exception::getLast();
               int length = 
                  120 + strlen(filename) + 
                  strlen(mi->module->getId().name) +
                  strlen(mi->module->getId().version) +
                  strlen(e->getMessage()) +
                  strlen(e->getType());
               char temp[length];
               snprintf(temp, length,
                  "Could not initialize module '%s' "
                  "named '%s', version '%s',cause=%s:%s:%i",
                  filename,
                  mi->module->getId().name,
                  mi->module->getId().version,
                  e->getMessage(),
                  e->getType(),
                  e->getCode());
               ExceptionRef ex = new Exception(
                  temp, "db.modest.ModuleInitializationError");
               ex->setCause(e);
               Exception::setLast(ex, false);
               mLoader.unloadModule(mi);
            }
         }
         else
         {
            // module is already loaded, set exception and unload it
            int length = 
               100 + strlen(filename) + 
               strlen(mi->module->getId().name) +
               strlen(mi->module->getId().version);
            char temp[length];
            snprintf(temp, length,
               "Could not load module '%s'. Module "
               "named '%s' with version '%s' is already loaded.",
               filename,
               mi->module->getId().name,
               mi->module->getId().version);
            ExceptionRef e = new Exception(temp, "db.modest.DuplicateModule");
            Exception::setLast(e, false);
            mLoader.unloadModule(mi);
         }
      }
   }
   mLoadLock.unlock();
   
   return rval;
}

void ModuleLibrary::unloadModule(const ModuleId* id)
{
   mLoadLock.lock();
   {
      // find module
      ModuleMap::iterator i = mModules.find(id);
      if(i != mModules.end())
      {
         // get module
         ModuleInfo* mi = i->second;
         
         // erase module from map and list
         mModules.erase(i);
         for(ModuleList::iterator li = mLoadOrder.begin();
             li != mLoadOrder.end(); li++)
         {
            if(**li == *id)
            {
               mLoadOrder.erase(li);
               break;
            }
         }
         
         // clean up and unload module
         mi->module->cleanup(mKernel);
         mLoader.unloadModule(mi);
      }
   }
   mLoadLock.unlock();
}

void ModuleLibrary::unloadAllModules()
{
   mLoadLock.lock();
   {
      // clean up and free every module
      while(!mLoadOrder.empty())
      {
         // find ModuleInfo
         ModuleMap::iterator i = mModules.find(mLoadOrder.back());
         ModuleInfo* mi = i->second;
         
         // remove module from map and list
         mModules.erase(i);
         mLoadOrder.pop_back();
         
         // clean up and unload module
         mi->module->cleanup(mKernel);
         mLoader.unloadModule(mi);
      }
   }
   mLoadLock.unlock();
}

Module* ModuleLibrary::getModule(const ModuleId* id)
{
   Module* rval = NULL;
   
   mLoadLock.lock();
   {
      // find Module
      rval = findModule(id);
   }
   mLoadLock.unlock();
   
   return rval;
}

const ModuleId* ModuleLibrary::getModuleId(const char* name)
{
   const ModuleId* rval = NULL;
   
   mLoadLock.lock();
   {
      // find Module
      Module* m = findModule(name);
      if(m != NULL)
      {
         rval = &m->getId();
      }
   }
   mLoadLock.unlock();
   
   return rval;
}

ModuleInterface* ModuleLibrary::getModuleInterface(const ModuleId* id)
{
   ModuleInterface* rval = NULL;
   
   mLoadLock.lock();
   {
      // find Module
      Module* m = findModule(id);
      if(m != NULL)
      {
         rval = m->getInterface();
      }
   }
   mLoadLock.unlock();
   
   return rval;
}
