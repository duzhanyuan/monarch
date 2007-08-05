/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_net_http_HttpHeader_H
#define db_net_http_HttpHeader_H

#include "Date.h"

#include <map>
#include <string>

namespace db
{
namespace net
{
namespace http
{

typedef struct FieldComparator
{
   /**
    * Compares two null-terminated strings, returning true if the first is
    * less than the second, false if not. The compare is case-insensitive.
    * 
    * @param s1 the first string.
    * @param s2 the second string.
    * 
    * @return true if the s1 < s2, false if not.
    */
   bool operator()(const char* s1, const char* s2) const
   {
      return strcasecmp(s1, s2) < 0;
   }
};

/**
 * An HttpHeader is the header for an HTTP Message. It contains a start-line,
 * and, optionally, a collection of HTTP header fields.
 * 
 * Both http requests and responses use an HttpHeader. A request uses a
 * request-line for its start-line and a response uses a status-line for its
 * start-line.
 * 
 * A multipart http message will use a MIME boundary as the start-line for
 * its HttpHeader.
 * 
 * @author Dave Longley
 */
class HttpHeader
{
protected:
   /**
    * The map containing the header fields.
    */
   std::map<const char*, std::string, FieldComparator> mFields;
   
public:
   /**
    * The standard line break for HTTP, carriage return + line feed.
    */
   static const char CRLF[];
   
   /**
    * Creates a new HttpHeader.
    */
   HttpHeader();
   
   /**
    * Destructs this HttpHeader.
    */
   virtual ~HttpHeader();
   
   /**
    * Parses the start line for this HttpHeader from the passed string.
    * 
    * @param str the string to parse from.
    * @param length the length of the start line (no null character included).
    * 
    * @return true if the start line could be parsed, false if not.
    */
   virtual bool parseStartLine(const char* str, unsigned int length) = 0;
   
   /**
    * Gets the start line for this HttpHeader.
    * 
    * @param line the startLine to populate.
    */
   virtual void getStartLine(std::string& line) = 0;
   
   /**
    * Sets a header field.
    * 
    * @param name the name of the header field to set.
    * @param value the value for the header field.
    */
   virtual void setField(const char* name, long long value);
   
   /**
    * Sets a header field.
    * 
    * @param name the name of the header field to set.
    * @param value the value for the header field.
    */
   virtual void setField(const char* name, const std::string& value);
   
   /**
    * Adds a value to an existing header field. If the header field does not
    * exist, it will be created.
    * 
    * @param name the name of the header field to update.
    * @param value the value to add to the header field.
    */
   virtual void addField(const char* name, const std::string& value);
   
   /**
    * Removes a header field.
    * 
    * @param name the name of the header field to remove.
    */
   virtual void removeField(const char* name);
   
   /**
    * Clears all header fields.
    */
   virtual void clearFields();
   
   /**
    * Gets a header field value.
    * 
    * @param name the name of the header field to get the value of.
    * @param value the value to populate.
    * 
    * @return true if the header field exists, false if not.
    */
   virtual bool getField(const char* name, long long& value);
   
   /**
    * Gets a header field value.
    * 
    * @param name the name of the header field to get the value of.
    * @param value the value to populate.
    * 
    * @return true if the header field exists, false if not.
    */
   virtual bool getField(const char* name, std::string& value);
   
   /**
    * Parses this header from the passed string.
    * 
    * @param str the string to parse from.
    * 
    * @return true if the header could be parsed, false if not.
    */
   virtual bool parse(const std::string& str);
   
   /**
    * Writes this header to a string.
    * 
    * @param str the string to write this header to.
    * 
    * @return the string.
    */
   virtual std::string& toString(std::string& str);
   
   /**
    * Sets the GMT date for this header. A value of NULL will set the date
    * to the current GMT.
    * 
    * @param date the date to use (NULL to use the current date).
    */
   virtual void setDate(db::util::Date* date = NULL);
   
   /** 
    * Gets the GMT date for this header.
    * 
    * @param date the Date to populate.
    * 
    * @return the GMT date for this header, false if no date header exists.
    */
   virtual bool getDate(db::util::Date& date);
   
   /**
    * BiCapitalizes a header field so that its name is normalized as an
    * HTTP header field name. HTTP header field names do not *have* to be
    * BiCapitalized, it's just the standard practice -- and it makes header
    * field names more readable.
    * 
    * @param name the name of the header field to BiCapitalize.
    */
   static void biCapitalize(char* name);
};

} // end namespace http
} // end namespace net
} // end namespace db
#endif
