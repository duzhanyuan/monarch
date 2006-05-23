/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
import com.db.common.*;

public class UTSignableXMLEnvelope
{
   public static void doEnvelopeTest()
   {
      System.out.println("Putting string \"chicken\" into envelope...");

      String xmlText = "";

      System.out.println("Generating keys for signing envelope...");

      KeyManager km = new KeyManager();
      boolean generated = km.generateKeyPair();

      System.out.println("Keys generated: " + generated);

      if(!generated)
      {
         System.exit(1);
      }
      

      StringXMLSerializer sxs = new StringXMLSerializer("chicken");
      SignableXMLEnvelope sxe = new SignableXMLEnvelope(sxs);

      if(sxe.sign(0, km.getPrivateKey()))
      {
         System.out.println("Successfully signed envelope");
      }
      else
      {
         System.err.println("Unable to sign envelope.");
         return;
      }
      
      // put that envelope in another envelope and sign it
      SignableXMLEnvelope sxe2 = new SignableXMLEnvelope(sxe);
      sxe2.sign(0, km.getPrivateKey());
      
      System.out.println("--- Converting envelope in envelope to XML ---");
      System.out.println(sxe2.convertToXML());
      System.out.println("--- Loading envelope in envelope from XML ---");
      String text = sxe2.convertToXML();
      SignableXMLEnvelope sxe3 =
         new SignableXMLEnvelope(new StringXMLSerializer());
      sxe2 = new SignableXMLEnvelope(sxe3, text);
      System.out.println(sxe2.convertToXML());
      System.out.println("--- Verify envelope in envelope test ---");
      System.out.println("Verified: " + sxe2.verify(km.getPublicKeyString()));
      System.out.println("Verified: " + sxe3.verify(km.getPublicKeyString()));

      System.out.println("--- Convert to XML test ---");

      xmlText = sxe.convertToXML();
      System.out.println(xmlText);
      
      System.out.println("--- Load XML test ---");

      // get a new envelope to test
      sxe = new SignableXMLEnvelope(new StringXMLSerializer());

      if(sxe.convertFromXML(xmlText))
      {
         System.out.println("Successfully loaded envelope from XML\n");
      }
      else
      {
         System.out.println("ERROR!! COULD NOT LOAD ENVELOPE");
      }

      try
      {
         System.out.println("--- Convert to XML after XML load test ---");
         System.out.println(sxe.convertToXML());
      }
      catch(NullPointerException e)
      {
         e.printStackTrace();
      }

      System.out.println("--- Verify Test ---");

      if(sxe.verify(km.getPublicKeyString()))
      {
         System.out.println("SUCCESS: envelope verified!");
      }
      else
      {
         System.out.println("FAILURE: envelope not verified!");
      }
      
      System.out.println("--- Envelope in envelope test ---");
      
      SignableXMLEnvelope outerEnvelope = new SignableXMLEnvelope(sxe);
      String outerXml = outerEnvelope.convertToXML();
      
      StringXMLSerializer innerSxs = new StringXMLSerializer();
      SignableXMLEnvelope innerEnvelope =
         new SignableXMLEnvelope(innerSxs);
      SignableXMLEnvelope outerEnvelope2 =
         new SignableXMLEnvelope(innerEnvelope);
      
      outerEnvelope2.convertFromXML(outerXml);
      
      System.out.println("ENVELOPE IN ENVELOPE=\n" +
         outerEnvelope2.convertToXML());
   }
   
   public static void main(String[] args)
   {
      com.db.logging.LoggerManager.setConsoleVerbosity(
         "dbcommon", com.db.logging.Logger.DEBUG_VERBOSITY);
      
      System.out.println("\nTesting SignableXMLEnvelope...");
      System.out.println("----------------------------\n");

      doEnvelopeTest();
   }
}
