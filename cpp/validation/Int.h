/*
 * Copyright (c) 2008-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_validation_Int_H
#define monarch_validation_Int_H

#include "monarch/validation/Validator.h"

namespace monarch
{
namespace validation
{

/**
 * Validates an object is an integer.  Optionally can specify minimum and
 * maximum values.
 *
 * @author David I. Lehn
 */
class Int : public Validator
{
public:
   /**
    * General integer types.
    */
   enum IntegerType
   {
      // < 0
      Negative,
      // <= 0
      NonPositive,
      // = 0
      Zero,
      // >= 0
      NonNegative,
      // > 0
      Positive
   };

protected:
   /* minimum */
   uint64_t mMin;

   /* is minimum negative? */
   bool mMinNegative;

   /* maximum */
   uint64_t mMax;

   /* is maximum negative? */
   bool mMaxNegative;

   /**
    * Set the min/max values.
    *
    * @param min minimum value
    * @param minNegative negative minimum value flag
    * @param max maximum value
    * @param maxNegative negative maximum value flag
    */
   void setMinMax(
      uint64_t min,
      bool minNegative,
      uint64_t max,
      bool maxNegative);

public:
   /**
    * Creates a new validator with specified minimum and maximum.  Restricted
    * to +/- uint64_t max.
    *
    * @param min minimum value
    * @param minNegative negative minimum value flag
    * @param max maximum value
    * @param maxNegative negative maximum value flag
    * @param errorMessage custom error message
    */
   Int(uint64_t min, bool minNegative, uint64_t max, bool maxNegative,
      const char* errorMessage = NULL);

   /**
    * Creates a new validator with specified minimum and maximum.  Restricted
    * to int64_t range.
    *
    * @param min minimum value
    * @param max maximum value
    * @param errorMessage custom error message
    */
   Int(int64_t min, int64_t max, const char* errorMessage = NULL);

   /**
    * Creates a new validator for an specific IntegerType.
    *
    * @param positive true to check >= 0, false for < 0.
    * @param errorMessage custom error message
    */
   Int(IntegerType type, const char* errorMessage = NULL);

   /**
    * Creates a new validator with min/max ranges for the specifed integer type.
    *
    * @param type type of integer (UInt32, Int32, UInt64, Int64)
    * @param errorMessage custom error message
    */
   Int(monarch::rt::DynamicObjectType type, const char* errorMessage = NULL);

   /**
    * Creates a new validator.
    *
    * @param errorMessage custom error message
    */
   Int(const char* errorMessage = NULL);

   /**
    * Destructs this validator.
    */
   virtual ~Int();

   /**
    * Checks if an object is valid.
    *
    * @param obj the object to validate.
    * @param context context to use during validation.
    *
    * @return true if obj is valid, false and exception set otherwise.
    */
   virtual bool isValid(
      monarch::rt::DynamicObject& obj,
      ValidatorContext* context);
   using Validator::isValid;
};

} // end namespace validation
} // end namespace monarch
#endif
