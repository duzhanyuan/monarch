/*
 * Copyright (c) 2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/validation/Max.h"

using namespace db::rt;
using namespace db::validation;

Max::Max(int size, const char* errorMessage) :
   Validator(errorMessage),
   mSize(size)
{
}

Max::~Max()
{
}

bool Max::isValid(
   DynamicObject& obj,
   ValidatorContext* context)
{
   bool rval = (obj->length() <= mSize);
   
   if(!rval)
   {
      DynamicObject detail = context->addError("db.validation.MaxError");
      if(mErrorMessage)
      {
         detail["message"] = mErrorMessage;
      }
      detail["expectedMax"] = mSize;
   }
   
   return rval;
}
