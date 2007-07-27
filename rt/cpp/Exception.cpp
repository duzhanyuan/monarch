/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "Exception.h"

#include <string>

using namespace db::rt;

Exception::Exception(const char* message, const char* code)
{
   if(message == NULL)
   {
      mMessage = new char[1];
      mMessage[0] = 0;
   }
   else
   {
      mMessage = new char[strlen(message) + 1];
      strcpy(mMessage, message);
   }
   
   if(code == NULL)
   {
      mCode = new char[1];
      mCode[0] = 0;
   }
   else
   {
      mCode = new char[strlen(code) + 1];
      strcpy(mCode, code);
   }
}

Exception::~Exception()
{
   delete mMessage;
   delete mCode;
}

const char* Exception::getMessage()
{
   return mMessage;
}

const char* Exception::getCode()
{
   return mCode;
}
