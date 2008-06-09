/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */

#include "db/logging/Category.h"

#include <cstdlib>
#include <cstring>

using namespace db::logging;
 
// DO NOT INITIALIZE THESE VARIABLES!
// These are not initialized on purpose due to initialization code issues.
Category* DB_DEFAULT_CAT;
Category* DB_ALL_CAT;

Category::Category(const char* id, const char* name, const char* description) :
   mId(NULL),
   mName(NULL),
   mDescription(NULL)
{
   setId(id);
   setName(name);
   setDescription(description);
}
   
Category::~Category()
{
   Category::setId(NULL);
   Category::setName(NULL);
   Category::setDescription(NULL);
}

void Category::initialize()
{
   DB_DEFAULT_CAT = new Category(
      "DB_DEFAULT",
      "Default",
      "Default category for general use");
   DB_ALL_CAT = new Category(
      NULL,
      NULL,
      "Pseudo-category that matches ALL other categories");
}

void Category::cleanup()
{
   delete DB_DEFAULT_CAT;
   DB_DEFAULT_CAT = NULL;
   
   delete DB_ALL_CAT;
   DB_ALL_CAT = NULL;
}

void Category::setId(const char* id)
{
   if(mId != NULL)
   {
      free(mId);
   }
   mId = (id != NULL ? strdup(id) : NULL);
}

const char* Category::getId()
{
   return mId;
}

void Category::setName(const char* name)
{
   if(mName != NULL)
   {
      free(mName);
   }
   mName = (name != NULL ? strdup(name) : NULL);
}

const char* Category::getName()
{
   return mName != NULL ? mName : "<?>";
}

void Category::setDescription(const char* description)
{
   if(mDescription != NULL)
   {
      free(mDescription);
   }
   mDescription = (description != NULL ? strdup(description) : NULL);
}

const char* Category::getDescription()
{
   return mDescription;
}
