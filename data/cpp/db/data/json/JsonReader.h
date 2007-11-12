/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_data_json_JsonReader_H
#define db_data_json_JsonReader_H

#include "db/util/DynamicObject.h"
#include "db/io/InputStream.h"

#include <vector>

namespace db
{
namespace data
{
namespace json
{

/**
 * Input classes.
 */
typedef enum JsonInputClass {
   /* Character classes */
   C_WS,    /* whitespace */
   C_SP,    /* space */
   C_BO,    /* { - begin object */
   C_EO,    /* } - end object */
   C_BA,    /* [ - begin array */
   C_EA,    /* ] - end array */
   C_NS,    /* : - name seperator */
   C_VS,    /* , - value seperator */
   C_DQ,    /* " - double quote */
   C_BS,    /* \ - backslash */
   C_FS,    /* / - forward slash */
   C_NZ,    /* 0 - number */
   C_NN,    /* 1-9 - number */
   C_ND,    /* . - decimal */
   C_NP,    /* + - plus */
   C_NM,    /* - - minus */
   C_LA,    /* a */
   C_LB,    /* b */
   C_LC,    /* c */
   C_LD,    /* d */
   C_LE,    /* e */
   C_LF,    /* f */
   C_LL,    /* l */
   C_LN,    /* n */
   C_LR,    /* r */
   C_LS,    /* s */
   C_LT,    /* t */
   C_LU,    /* u */
   C_UH,    /* ABCDF */
   C_UE,    /* E */
   C_CH,    /* other valid characters */
   C_DO,    /* DynamicObjects on stack */
   C___,    /* invalid class */
   /* Meta */
   C_COUNT  /* count of classes */
};

/**
 * Parser states and actions.
 */
typedef enum JsonState {
   /* State codes */
   /* CODE /. state, [expect, ...] ./ */
   J_, /* start of JSON parsing, ['{', '['] */
   _J, /* end of JSON parsing, [] */
   O_, /* '{' start object, [key, '}'] */
   O2, /* got ',', [key] */
   OK, /* got key, [':'] */
   OC, /* got key+colon, [value] */
   OV, /* got key:value, [',', '}'] */
   A_, /* '[' start array, [value, ']'] */
   A2, /* got ',', [value] */
   AV, /* got value, [',', ']'] */
   S_, /* '"' start string, [char, '\', '"'] */
   SC, /* got character, [char, '\', '"'] */
   E_, /* '\' start escape, [code, 'u'] */
   U_, /* '\u' start unicode escape, [hex] */
   U1, /* '\ux', [hex] */
   U2, /* '\uxx', [hex] */
   U3, /* '\uxxx', [hex] */
   T_, /* 't' start true, ['r'] */
   TR, /* 'tr', ['u'] */
   TU, /* 'tru', ['e'] */
   F_, /* 'f' start false, ['a'] */
   FA, /* 'fa', ['l'] */
   FL, /* 'fal', ['s'] */
   FS, /* 'fals', ['e'] */
   N_, /* 'n' start null, ['u'] */
   NU, /* 'nu', ['l']  */
   NL, /* 'nul', ['l'] */
   MI, /* '-' start - zero/int/frac */
   Z_, /* '0' start zero/frac */
   Z2, /* '0' cont zero/frac */
   I_, /* '1-9' start int */
   I2, /* '0-9' cont int */
   FR, /* fraction */
   F2, /* cont fraction */
   EE, /* exponent e E */
   ES, /* exponent -/+ */
   EV, /* exponent value */
   S_COUNT,
   /* Action codes */
   _W, /* Whitespace done */
   _O, /* Object done */
   _A, /* Array done */
   _E, /* Escape done */
   _U, /* Unicode done */
   _S, /* String done */
   _T, /* true done */
   _F, /* false done */
   _N, /* null done */
   _I, /* Integer done */
   _D, /* Float done */
   S_ACTION_COUNT,
   __  /* Error */
};
   
/**
 * A JsonReader provides an interface for deserializing objects from
 * JSON (JavaScript Object Notation) (RFC 4627).
 * 
 * @author David I. Lehn
 */
class JsonReader
{
protected:
   /**
    * Map of first 128 ASCII characters to their JsonInputClass.  Used to reduce
    * size of state table.
    */
   static JsonInputClass sAsciiToClass[128];
   
   /**
    * State table.  Used to find next state or action from current state and
    * next input class.
    */
   static JsonState sStateTable[S_COUNT][C_COUNT];
   
   /**
    * True if this JSON parser has started, false if not.
    */
   bool mStarted;
   
   /**
    * Current state.
    */
   JsonState mState;
   
   /**
    * A stack of parse states.
    */
   std::vector<JsonState> mStateStack;
   
   /**
    * Temporary string built up as input is parsed.  Interpreted as needed.
    */
   std::string mString;

   /**
    * A stack of DynamicObjects.
    */
   std::vector<db::util::DynamicObject> mDynoStack;
   
   /**
    * The read size in bytes.
    */
   static unsigned int READ_SIZE;
   
   /**
    * Current buffer.
    */
   char* mBuffer;
   
   /**
    * Line number for debugging (count of '\n's, starting at 1).
    */
   int mLineNumber;

   /**
    * Flag if root Object or Array was found.
    */
   bool mValid;
   
   /**
    * Process a buffer of characters
    * 
    * @param c string buffer
    * @param count size of c
    */
   db::io::IOException* process(const char* c, int count);
   
   /**
    * Process one input object.  For most classes the parameter c is used
    * to pass the character object.  For C_DO the DynamicObjects to
    * process are at the back of mDynoStack.
    * 
    * @param ic the type of input to process
    * @param c the character to process (if needed)
    */
   db::io::IOException* processNext(JsonInputClass ic, char c = '\0');
   
public:
   /**
    * Creates a new JsonReader.
    */
   JsonReader();
   
   /**
    * Destructs this JsonReader.
    */
   virtual ~JsonReader();
   
   /**
    * Starts deserializing an object from JSON. This JsonReader can be re-used
    * by calling start() with the same or a new object.  Calling start() before
    * a previous deserialization has finished will abort the previous state.
    * 
    * Using a non-empty target object can be used to merge in new values.  This
    * is only defined for similar object types (ie, merging an array into a map
    * will overwrite the map).
    * 
    * @param dyno the DynamicObject for the object to deserialize.
    */
   virtual void start(db::util::DynamicObject dyno);
   
   /**
    * This method reads JSON from the passed InputStream until the end of
    * the stream, blocking if necessary.
    * 
    * The start() method must be called at least once before calling read(). As
    * the JSON is read, the DynamicObject provided in start() is used to
    * deserialize an object.
    * 
    * This method may be called multiple times if the input stream needs to
    * be populated in between calls or if multiple input streams are used.
    * 
    * The object is built incrementally and on error will be partially built.
    * 
    * finish() should be called after the read is complete in order to check
    * that a top level object is complete.
    * 
    * @param dyno the DynamicObject for the object to deserialize.
    * @param is the InputStream to read the JSON from.
    * 
    * @return an IOException if one occurred, NULL if not.
    */
   virtual db::io::IOException* read(db::io::InputStream* is);

   /**
    * Finishes deserializing an object from JSON. This method shoudl be called
    * to complete deserialization and verify valid JSON was found.
    * 
    * @return an IOException is one occurred, NULL if not.
    */
   virtual db::io::IOException* finish();
};

} // end namespace json
} // end namespace data
} // end namespace db
#endif
