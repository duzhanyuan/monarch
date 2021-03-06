/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#include "monarch/crypto/DigitalEnvelope.h"

#include "monarch/crypto/PrivateKey.h"
#include "monarch/crypto/PublicKey.h"
#include "monarch/crypto/SymmetricKeyFactory.h"
#include "monarch/rt/Exception.h"

#include <openssl/err.h>

#include <cstring>

using namespace std;
using namespace monarch::crypto;
using namespace monarch::io;
using namespace monarch::rt;

DigitalEnvelope::DigitalEnvelope() : AbstractBlockCipher(true)
{
}

DigitalEnvelope::~DigitalEnvelope()
{
}

bool DigitalEnvelope::startSealing(
   const char* algorithm, PublicKeyRef& publicKey, SymmetricKey* symmetricKey)
{
   // store and use just a single public key
   mKey = publicKey;
   PublicKey* pkey = &(*publicKey);
   return startSealing(algorithm, &pkey, &symmetricKey, 1);
}

bool DigitalEnvelope::startSealing(
   const char* algorithm, PublicKey** publicKeys,
   SymmetricKey** symmetricKeys, int keys)
{
   bool rval = false;

   // enable encryption mode
   mEncryptMode = true;

   // reset input and output bytes
   mInputBytes = mOutputBytes = 0;

   // get the cipher function
   mCipherFunction = getCipherFunction(algorithm);
   if(mCipherFunction != NULL)
   {
      // create symmetric key buffers for each public key
      EVP_PKEY* pKeys[keys];
      unsigned char* eKeys[keys];
      int eKeyLengths[keys];
      for(int i = 0; i < keys; ++i)
      {
         pKeys[i] = publicKeys[i]->getPKEY();
         eKeys[i] = (unsigned char*)malloc(publicKeys[i]->getOutputSize());
      }

      // create iv buffer
      unsigned int ivLength = EVP_CIPHER_iv_length(mCipherFunction);
      unsigned char* iv = (ivLength == 0) ?
         NULL : (unsigned char*)malloc(ivLength);

      // initialize sealing the envelope
      if(EVP_SealInit(
         &mCipherContext, mCipherFunction,
         eKeys, eKeyLengths, iv, pKeys, keys) == 1)
      {
         // initialization successful
         rval = true;

         // set the encrypted symmetric key data
         for(int i = 0; i < keys; ++i)
         {
            // copy iv
            char* ivCopy = NULL;
            if(ivLength > 0)
            {
               ivCopy = (char*)malloc(ivLength);
               memcpy(ivCopy, iv, ivLength);
            }

            // assign encrypted symmetric key
            symmetricKeys[i]->setAlgorithm(algorithm);
            symmetricKeys[i]->assignData(
               (char*)eKeys[i], eKeyLengths[i], ivCopy, ivLength, true);
         }

         if(iv != NULL)
         {
            // delete iv buffer (freeing the other buffers is up
            // to the SymmetricKeys)
            free(iv);
         }
      }
      else
      {
         // delete all allocated key data
         for(int i = 0; i < keys; ++i)
         {
            free(eKeys[i]);
         }

         if(iv != NULL)
         {
            // delete iv buffer (freeing the other buffers is up to the
            // SymmetricKeys)
            free(iv);
         }

         ExceptionRef e = new Exception(
            "Could not start opening envelope.",
            "monarch.crypto.DigitalEnvelope.SealError");
         e->getDetails()["error"] = ERR_error_string(ERR_get_error(), NULL);
         Exception::set(e);
      }
   }

   return rval;
}

bool DigitalEnvelope::startOpening(
   PrivateKeyRef& privateKey, SymmetricKey* symmetricKey)
{
   bool rval = false;

   // store key
   mKey = privateKey;

   // disable encryption mode
   mEncryptMode = false;

   // reset input and output bytes
   mInputBytes = mOutputBytes = 0;

   // get the cipher function
   mCipherFunction = getCipherFunction(symmetricKey->getAlgorithm());
   if(mCipherFunction != NULL)
   {
      // get the symmetric key data
      char* eKey;
      unsigned int eKeyLength;
      char* iv;
      unsigned int ivLength;
      symmetricKey->getData(&eKey, eKeyLength, &iv, ivLength);

      // initialize opening the envelope
      if(EVP_OpenInit(
         &mCipherContext, mCipherFunction,
         (unsigned char*)eKey, eKeyLength, (unsigned char*)iv,
         mKey->getPKEY()) == 1)
      {
         rval = true;
      }
      else
      {
         ExceptionRef e = new Exception(
            "Could not start opening envelope.",
            "monarch.crypto.DigitalEnvelope.OpenError");
         e->getDetails()["error"] = ERR_error_string(ERR_get_error(), NULL);
         Exception::set(e);
      }
   }

   return rval;
}

bool DigitalEnvelope::update(
   const char* in, int inLength, char* out, int& outLength)
{
   bool rval = false;

   // only proceed if the cipher function has been set
   if(mCipherFunction != NULL)
   {
      if(isEncryptEnabled())
      {
         // seal more data
         if(EVP_SealUpdate(
            &mCipherContext, (unsigned char*)out, &outLength,
            (unsigned char*)in, inLength) == 1)
         {
            rval = true;
         }
         else
         {
            ExceptionRef e = new Exception(
               "Could not seal envelope data.",
               "monarch.crypto.DigitalEnvelope.SealError");
            e->getDetails()["error"] = ERR_error_string(ERR_get_error(), NULL);
            Exception::set(e);
         }
      }
      else
      {
         // open more data
         if(EVP_OpenUpdate(
            &mCipherContext, (unsigned char*)out, &outLength,
            (unsigned char*)in, inLength) == 1)
         {
            rval = true;
         }
         else
         {
            ExceptionRef e = new Exception(
               "Could not open envelope data.",
               "monarch.crypto.DigitalEnvelope.OpenError");
            e->getDetails()["error"] = ERR_error_string(ERR_get_error(), NULL);
            Exception::set(e);
         }
      }

      if(rval)
      {
         // update input and output bytes
         mInputBytes += inLength;
         mOutputBytes += outLength;
      }
   }
   else
   {
      ExceptionRef e = new Exception(
         "Cannot update envelope; envelope not started.");
      Exception::set(e);
   }

   return rval;
}

bool DigitalEnvelope::finish(char* out, int& length)
{
   bool rval = false;

   // only proceed if the cipher function has been set
   if(mCipherFunction != NULL)
   {
      if(isEncryptEnabled())
      {
         // finish sealing
         if(EVP_SealFinal(&mCipherContext, (unsigned char*)out, &length) == 1)
         {
            rval = true;
         }
         else
         {
            ExceptionRef e = new Exception(
               "Could not finish sealing envelope.",
               "monarch.crypto.DigitalEnvelope.SealError");
            e->getDetails()["error"] = ERR_error_string(ERR_get_error(), NULL);
            Exception::set(e);
         }
      }
      else
      {
         // finish opening
         if(EVP_OpenFinal(&mCipherContext, (unsigned char*)out, &length) == 1)
         {
            rval = true;
         }
         else
         {
            ExceptionRef e = new Exception(
               "Could not finish opening envelope.",
               "monarch.crypto.DigitalEnvelope.OpenError");
            e->getDetails()["error"] = ERR_error_string(ERR_get_error(), NULL);
            Exception::set(e);
         }
      }

      if(rval)
      {
         // update output bytes
         mOutputBytes += length;
      }
   }
   else
   {
      ExceptionRef e = new Exception(
         "Cannot finish envelope; envelope not started.",
         "monarch.crypto.DigitalEnvelope.MethodCallOutOfOrder");
      Exception::set(e);
   }

   return rval;
}

uint64_t DigitalEnvelope::getTotalInput()
{
   return mInputBytes;
}

uint64_t DigitalEnvelope::getTotalOutput()
{
   return mOutputBytes;
}
