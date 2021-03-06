/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/crypto/BlockCipher.h"

using namespace monarch::io;
using namespace monarch::crypto;

BlockCipher::BlockCipher()
{
}

BlockCipher::~BlockCipher()
{
}

bool BlockCipher::update(
   const char* in, int inLength, ByteBuffer* out, bool resize)
{
   bool rval = false;

   // allocate space for data
   out->allocateSpace(inLength + getBlockSize(), resize);

   // do update
   int length;
   if(update(in, inLength, out->end(), length))
   {
      // extend buffer length
      out->extend(length);
      rval = true;
   }

   return rval;
}

bool BlockCipher::finish(ByteBuffer* out, bool resize)
{
   bool rval = false;

   // allocate space for data
   out->allocateSpace(getBlockSize(), resize);

   // do finish
   int length;
   if(finish(out->end(), length))
   {
      // extend buffer length
      out->extend(length);
      rval = true;
   }

   return rval;
}
