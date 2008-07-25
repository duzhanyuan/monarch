/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#include <iostream>
#include <sstream>

#include "db/test/Test.h"
#include "db/test/Tester.h"
#include "db/test/TestRunner.h"
#include "db/io/ByteArrayInputStream.h"
#include "db/mail/SmtpClient.h"
#include "db/mail/MailTemplateParser.h"
#include "db/mail/MailSpool.h"
#include "db/net/Url.h"

using namespace std;
using namespace db::io;
using namespace db::net;
using namespace db::test;
using namespace db::rt;

void runSmtpClientTest(TestRunner& tr)
{
   tr.test("SmtpClient");
   
   // set url of mail server
   Url url("smtp://localhost:25");
   
   // set mail
   db::mail::Mail mail;
   mail.setSender("testuser@bitmunk.com");
   mail.addTo("support@bitmunk.com");
   mail.addCc("support@bitmunk.com");
   mail.setSubject("This is an autogenerated unit test email");
   mail.setBody("This is the test body");
   
   // send mail
   db::mail::SmtpClient c;
   c.sendMail(&url, &mail);
   
   tr.passIfNoException();
}

void runMailTemplateParser(TestRunner& tr)
{
   tr.test("MailTemplateParser");
   
   // create mail template
   const char* tpl =
      "Subject: This is an autogenerated unit test email\r\n"
      "From: testuser@bitmunk.com\r\n"
      "To: support@bitmunk.com\r\n"
      "Cc: support@bitmunk.com\r\n"
      "Bcc: $bccAddress1\r\n"
      "\r\n"
      "This is the test body. I want \\$10.00.\n"
      "I used a variable: \\$bccAddress1 with the value of "
      "'$bccAddress1'.\n"
      "Slash before variable \\\\$bccAddress1.\n"
      "2 slashes before variable \\\\\\\\$bccAddress1.\n"
      "Slash before escaped variable \\\\\\$bccAddress1.\n"
      "2 slashes before escaped variable \\\\\\\\\\$bccAddress1.\n"
      "$eggs$bacon$ham$sausage.";
   
   // create template parser
   db::mail::MailTemplateParser parser;
   
   // create input stream
   ByteArrayInputStream bais(tpl, strlen(tpl));
   
   // create variables
   DynamicObject vars;
   vars["bccAddress1"] = "support@bitmunk.com";
   vars["eggs"] = "This is a ";
   //vars["bacon"] -- no bacon
   vars["ham"] = "number ";
   vars["sausage"] = 5;
   
   // parse mail
   db::mail::Mail mail;
   parser.parse(&mail, vars, &bais);
   
   const char* expect =
      "This is the test body. I want $10.00.\r\n"
      "I used a variable: $bccAddress1 with the value of "
      "'support@bitmunk.com'.\r\n"
      "Slash before variable \\support@bitmunk.com.\r\n"
      "2 slashes before variable \\\\support@bitmunk.com.\r\n"
      "Slash before escaped variable \\$bccAddress1.\r\n"
      "2 slashes before escaped variable \\\\$bccAddress1.\r\n"
      "This is a number 5.\r\n";
   
   // get mail message
   db::mail::Message msg = mail.getMessage();
   
   // assert body parsed properly
   const char* body = msg["body"]->getString();
   assertStrCmp(body, expect);
   
   // create template from mail
   string generatedTemplate = mail.toTemplate();
   
   const char* genExpect =
      "CC: support@bitmunk.com\r\n"
      "From: testuser@bitmunk.com\r\n"
      "Subject: This is an autogenerated unit test email\r\n"
      "To: support@bitmunk.com\r\n"
      "\r\n"
      "This is the test body. I want \\$10.00.\r\n"
      "I used a variable: \\$bccAddress1 with the value of 'support@bitmunk.com'.\r\n"
      "Slash before variable \\\\support@bitmunk.com.\r\n"
      "2 slashes before variable \\\\\\\\support@bitmunk.com.\r\n"
      "Slash before escaped variable \\\\\\$bccAddress1.\r\n"
      "2 slashes before escaped variable \\\\\\\\\\$bccAddress1.\r\n"
      "This is a number 5.\r\n";
   
   assertStrCmp(generatedTemplate.c_str(), genExpect);
   //cout << "Generated template=\n" << generatedTemplate << std::endl;
   
//   // print out mail message
//   cout << "\nHeaders=\n";
//   DynamicObjectIterator i = msg["headers"].getIterator();
//   while(i->hasNext())
//   {
//      DynamicObject header = i->next();
//      DynamicObjectIterator doi = header.getIterator();
//      while(doi->hasNext())
//      {
//         cout << i->getName() << ": " << doi->next()->getString() << endl;
//      }
//   }
//   
//   cout << "Expect=\n" << expect << endl;
//   cout << "Body=\n" << msg["body"]->getString() << endl;
   
//   // set url of mail server
//   Url url("smtp://localhost:25");
//   
//   // send mail
//   db::mail::SmtpClient c;
//   c.sendMail(&url, &mail);
   
   tr.passIfNoException();
}

void mailSpoolTest(TestRunner& tr)
{
   tr.test("MailSpool");
   
   // create mail template
   const char* tpl =
      "Subject: This is an autogenerated unit test email\r\n"
      "From: testuser@bitmunk.com\r\n"
      "To: support@bitmunk.com\r\n"
      "Cc: support@bitmunk.com\r\n"
      "Bcc: $bccAddress1\r\n"
      "\r\n"
      "This is the test body. I want \\$10.00.\n"
      "I used a variable: \\$bccAddress1 with the value of "
      "'$bccAddress1'.\n"
      "Slash before variable \\\\$bccAddress1.\n"
      "2 slashes before variable \\\\\\\\$bccAddress1.\n"
      "Slash before escaped variable \\\\\\$bccAddress1.\n"
      "2 slashes before escaped variable \\\\\\\\\\$bccAddress1.\n"
      "$eggs$bacon$ham$sausage.";
   
   // create template parser
   db::mail::MailTemplateParser parser;
   
   // create input stream
   ByteArrayInputStream bais(tpl, strlen(tpl));
   
   // create variables
   DynamicObject vars;
   vars["bccAddress1"] = "support@bitmunk.com";
   vars["eggs"] = "This is a ";
   //vars["bacon"] -- no bacon
   vars["ham"] = "number ";
   vars["sausage"] = 5;
   
   // parse mail
   db::mail::Mail mail;
   parser.parse(&mail, vars, &bais);
   
   // get template
   string tpl1 = mail.toTemplate();
   
   // clean up old spool files
   File file("/tmp/bmtestspool");
   File idxFile("/tmp/bmtestspool.idx");
   idxFile->remove();
   file->remove();
   
   // create mail spool
   db::mail::MailSpool spool;
   spool.setFile(file);
   assertNoException();
   
   // spool mail
   spool.spool(&mail);
   assertNoException();
   
   // spool mail
   spool.spool(&mail);
   assertNoException();
   
   // spool mail
   spool.spool(&mail);
   assertNoException();
   
   // get mail
   db::mail::Mail m2;
   spool.getFirst(&m2);
   assertNoException();
   
   // assert templates are equal
   string tpl2 = m2.toTemplate();
   assertStrCmp(tpl1.c_str(), tpl2.c_str());
   
   // unwind
   spool.unwind();
   spool.unwind();
   assertNoException();
   
   // get mail
   db::mail::Mail m3;
   spool.getFirst(&m3);
   assertNoException();
   
   // assert templates are equal
   string tpl3 = m3.toTemplate();
   assertStrCmp(tpl1.c_str(), tpl3.c_str());
   
   // unwind
   spool.unwind();
   
   // get first mail (should exception, spool is empty)
   spool.getFirst(&m3);
   assertException();
   Exception::clearLast();
   
   tr.passIfNoException();
}

class DbMailTester : public db::test::Tester
{
public:
   DbMailTester()
   {
      setName("mail");
   }

   /**
    * Run automatic unit tests.
    */
   virtual int runAutomaticTests(TestRunner& tr)
   {
      runMailTemplateParser(tr);
      mailSpoolTest(tr);
      return 0;
   }

   /**
    * Runs interactive unit tests.
    */
   virtual int runInteractiveTests(TestRunner& tr)
   {
      runSmtpClientTest(tr);
      return 0;
   }
};

#ifndef DB_TEST_NO_MAIN
DB_TEST_MAIN(DbMailTester)
#endif
