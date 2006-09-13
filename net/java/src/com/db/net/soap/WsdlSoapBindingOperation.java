/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.net.soap;

import com.db.logging.Logger;
import com.db.logging.LoggerManager;
import com.db.net.wsdl.Wsdl;
import com.db.net.wsdl.WsdlMessage;
import com.db.net.wsdl.WsdlPortTypeOperation;
import com.db.xml.AbstractXmlSerializer;
import com.db.xml.XmlElement;

/**
 * A WSDL SOAP Binding Operation.
 * 
 * A WSDL SOAP Binding Operation defines the message encoding and 
 * transmission protocol for a WSDL Port Type operation. It indicates
 * that messages will be encoded in a SOAP envelope and sent over HTTP. 
 * 
 * @author Dave Longley
 */
public class WsdlSoapBindingOperation extends AbstractXmlSerializer
{
   /**
    * The corresponding port type operation.
    */
   protected WsdlPortTypeOperation mPortTypeOperation;
   
   /**
    * The optional soap action associated with this operation. 
    */
   protected String mSoapAction;
   
   /**
    * The SOAP encoding namespace.
    */
   public static final String SOAP_ENCODING_NAMESPACE_URI =
      "http://schemas.xmlsoap.org/soap/encoding/";   
   
   /**
    * Creates a new Wsdl Soap Binding Operation with the given
    * associated port type operation.
    * 
    * @param operation the associated port type operation.
    */
   public WsdlSoapBindingOperation(WsdlPortTypeOperation operation)
   {
      // store port type operation
      setPortTypeOperation(operation);
      
      // set blank soap action
      setSoapAction("");
   }
   
   /**
    * Adds the SOAP body element child to the passed XmlElement.
    * 
    * @param element the XmlElement to add the SOAP body element to.
    */
   protected void addSoapBodyToXmlElement(XmlElement element)
   {
      // create the soap body element
      XmlElement soapBodyElement = new XmlElement(
         "body", "soap", Wsdl.WSDL_SOAP_NAMESPACE_URI);
      soapBodyElement.addAttribute(
         "encodingStyle", SOAP_ENCODING_NAMESPACE_URI);
      soapBodyElement.addAttribute("use", "encoded");
      
      // add the soap body element as a child
      element.addChild(soapBodyElement);
   }
   
   /**
    * Sets the port type operation associated with this operation.
    * 
    * @param operation the port type operation associated with this
    *                  operation.
    */
   public void setPortTypeOperation(WsdlPortTypeOperation operation)
   {
      mPortTypeOperation = operation;
   }
   
   /**
    * Sets the port type operation associated with this operation.
    * 
    * @return the port type operation associated with this operation.
    */
   public WsdlPortTypeOperation getPortTypeOperation()
   {
      return mPortTypeOperation;
   }
   
   /**
    * Gets the name of this operation.
    * 
    * @return the name of this operation.
    */
   public String getName()
   {
      return getPortTypeOperation().getName();
   }
   
   /**
    * Sets the soap action associated with this operation.
    * 
    * @param action the soap action associated with this operation.
    */
   public void setSoapAction(String action)
   {
      mSoapAction = action;
   }
   
   /**
    * Gets the soap action associated with this operation.
    * 
    * @return the soap action associated with this operation.
    */
   public String getSoapAction()
   {
      return mSoapAction;
   }
   
   /**
    * Gets the WsdlMessage for a SOAP request to a web service.
    * 
    * @return the WsdlMessage for a SOAP request to a web service.
    */
   public WsdlMessage getRequestMessage()
   {
      return getPortTypeOperation().getRequestMessage();
   }
   
   /**
    * Gets the WsdlMessage for a SOAP response from a web service.
    * 
    * @return the WsdlMessage for a SOAP response from a web service.
    */
   public WsdlMessage getResponseMessage()
   {
      return getPortTypeOperation().getResponseMessage();
   }
   
   /**
    * Returns the root tag name for this serializer.
    * 
    * @return the root tag name for this serializer.
    */
   public String getRootTag()   
   {
      return "operation";
   }
   
   /**
    * Creates an XmlElement from this object.
    *
    * @param parent the parent XmlElement for the XmlElement being created
    *               (can be null). 
    * 
    * @return the XmlElement that represents this object.
    */
   public XmlElement convertToXmlElement(XmlElement parent)
   {
      // create xml element
      XmlElement element = new XmlElement(getRootTag());
      element.setParent(parent);
      
      // add attributes
      element.addAttribute("name", getName());
      
      // create and add soap operation element
      XmlElement soapOperationElement = new XmlElement(
         "operation", "soap", Wsdl.WSDL_SOAP_NAMESPACE_URI);
      soapOperationElement.addAttribute("soapAction", getSoapAction());
      element.addChild(soapOperationElement);
      
      // create input and output elements
      XmlElement inputElement = new XmlElement("input");
      addSoapBodyToXmlElement(inputElement);
      
      XmlElement outputElement = new XmlElement("output");
      addSoapBodyToXmlElement(outputElement);
      
      if(getPortTypeOperation().usesOnlyInputMessage())
      {
         // add input element as a child
         element.addChild(inputElement);
      }
      else if(getPortTypeOperation().usesOnlyOutputMessage())
      {
         // add output element as a child
         element.addChild(outputElement);
      }
      else
      {
         if(getPortTypeOperation().isInputFirst())
         {
            // add input element as a child
            element.addChild(inputElement);

            // add output element as a child
            element.addChild(outputElement);
         }
         else
         {
            // add output element as a child
            element.addChild(outputElement);

            // add input element as a child
            element.addChild(inputElement);
         }
      }
      
      // return element
      return element;      
   }
   
   /**
    * Converts this object from an XmlElement.
    *
    * @param element the XmlElement to convert from.
    * 
    * @return true if successful, false otherwise.
    */
   public boolean convertFromXmlElement(XmlElement element)   
   {
      boolean rval = false;
      
      // clear soap action
      setSoapAction("");
      
      if(element.getName().equals(getRootTag()))
      {
         // get the soap namespace
         String soapNs = element.findNamespace(Wsdl.WSDL_SOAP_NAMESPACE_URI);
         
         // get the soap operation child
         XmlElement soapOperationChild = element.getFirstChild(
            "operation", soapNs); 
         if(soapOperationChild != null)
         {
            // set soap action
            setSoapAction(soapOperationChild.getAttributeValue("soapAction"));
         
            // FUTURE CODE: current implementation assumes soap encoding
            // parameters -- we'll want to parse these out in the future
         }
         
         rval = true;
      }
      
      return rval;      
   }
   
   /**
    * Gets the logger.
    * 
    * @return the logger.
    */
   public Logger getLogger()
   {
      return LoggerManager.getLogger("dbnet");
   }   
}
