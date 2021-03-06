/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_modest_Module_H
#define monarch_modest_Module_H

#include "monarch/modest/ModuleInterface.h"
#include "monarch/rt/Exception.h"

namespace monarch
{
namespace modest
{

// forward declare Kernel
class Kernel;

/**
 * A ModuleId contains a name and version for a Module. The name and version
 * are stored as const char*'s and are assumed to point to static memory.
 *
 * @author Dave Longley
 */
struct ModuleId
{
   /**
    * The unique name of this Module.
    */
   const char* name;

   /**
    * The version (major.minor) of this Module.
    */
   const char* version;

   /**
    * Creates a ModuleId with the specified name and version.
    *
    * @param name the name for the ModuleId.
    * @param version the version (major.minor) for the ModuleId.
    */
   ModuleId(const char* name = "", const char* version = "")
   {
      this->name = name;
      this->version = version;
   }

   /**
    * Compares two ModuleIds for equality.
    *
    * Two ModuleIds are equal if the names are the same and the
    * versions are the same or at least one of the versions is NULL.
    *
    * @param id the ModuleId to compare to.
    *
    * @return true if the id1 == id2, false if not.
    */
   bool operator==(const ModuleId& id) const
   {
      return
         strcmp(name, id.name) == 0 &&
         ((version == NULL || id.version == NULL) ||
          strcmp(version, id.version) == 0);
   }
};

/**
 * A Module is any extension to Modest. It can be loaded into an instance of
 * Modest run its available Operations. It can also create and provide new
 * Operations for other Modules to run.
 *
 * @author Dave Longley
 */
class Module
{
public:
   /**
    * Creates a new Module.
    */
   Module() {};

   /**
    * Destructs this Module.
    */
   virtual ~Module() {};

   /**
    * Gets the ID of this Module.
    *
    * @return the ID of this Module.
    */
   virtual const ModuleId& getId() = 0;

   /**
    * Initializes this Module with the modest Kernel once it has been
    * loaded.
    *
    * @param k the the modest Kernel to initialize with.
    *
    * @return true if the module initialized, false if not (with
    *         an Exception set).
    */
   virtual bool initialize(Kernel* k) = 0;

   /**
    * Cleans up this Module just prior to its unloading.
    *
    * @param k the modest Kernel that is unloading this Module.
    */
   virtual void cleanup(Kernel* k) = 0;

   /**
    * Gets the interface for this Module. The returned object should be
    * cast to the appropriate extended ModuleInterface class for this Module.
    *
    * @return the interface that provides access to this Module's functionality.
    */
   virtual ModuleInterface* getInterface() = 0;
};

} // end namespace modest
} // end namespace monarch

// prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

monarch::modest::Module* createModestModule();
void freeModestModule(monarch::modest::Module* m);

typedef monarch::modest::Module* (*CreateModestModuleFn)();
typedef void (*FreeModestModuleFn)(monarch::modest::Module*);

#ifdef __cplusplus
}
#endif

#endif
