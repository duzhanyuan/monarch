/*
 * Copyright (c) 2003-2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.common;

import com.db.logging.LoggerManager;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.net.URLDecoder;

/**
 * This class provides the ability to encode and decode using
 * URL encoding.
 * 
 * @author Dave Longley
 */
public class URLCoder
{
   /**
    * The singleton instance of the URLCoder.
    */
   protected static URLCoder mInstance = new URLCoder();
   
   /**
    * Creates the URLCoder.
    */
   protected URLCoder()
   {
   }

   /**
    * Gets the singleton instance of the URLCoder.
    * 
    * @return the singleton instance of the URLCoder.
    */
   public static URLCoder getInstance()
   {
      return mInstance;
   }

   /**
    * URL encodes a string.
    * 
    * @param string the string to URL encode.
    * 
    * @return the URL encoded string.
    */
   public static String encode(String string)
   {
      String encoded = "";
      
      if(string != null && !string.equals(""))
      {
         try
         {
            // encode the string
            encoded = URLEncoder.encode(string, "UTF-8");
         }
         catch(UnsupportedEncodingException uee)
         {
            LoggerManager.error("dbcommon", "UTF-8 encoding not supported.");
            LoggerManager.debug("dbcommon", LoggerManager.getStackTrace(uee));
         }
      }

      return encoded;
   }
   
   /**
    * URL decodes a string.
    * 
    * @param string the string to URL decode.
    * 
    * @return the URL decoded string.
    */
   public static String decode(String string)
   {
      String decoded = "";
      
      if(string != null && !string.equals(""))
      {
         try
         {
            // decode the string
            decoded = URLDecoder.decode(string, "UTF-8");
         }
         catch(UnsupportedEncodingException uee)
         {
            LoggerManager.error("dbcommon", "UTF-8 encoding not supported.");
            LoggerManager.debug("dbcommon", LoggerManager.getStackTrace(uee));
         }
      }

      return decoded;
   }
}
