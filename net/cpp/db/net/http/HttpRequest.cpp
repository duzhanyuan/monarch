/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/net/http/HttpRequest.h"
#include "db/net/http/HttpConnection.h"
#include "db/net/http/HttpResponse.h"

using namespace db::io;
using namespace db::net;
using namespace db::net::http;

HttpRequest::HttpRequest(HttpConnection* hc) : WebRequest(hc)
{
}

HttpRequest::~HttpRequest()
{
}

WebResponse* HttpRequest::createResponse()
{
   return new HttpResponse(this);
}

IOException* HttpRequest::sendHeader()
{
   return getConnection()->sendHeader(getHeader());
}

IOException* HttpRequest::receiveHeader()
{
   return getConnection()->receiveHeader(getHeader());
}

IOException* HttpRequest::sendBody(InputStream* is, HttpTrailer* trailer)
{
   return getConnection()->sendBody(getHeader(), is, trailer);
}

OutputStream* HttpRequest::getBodyOutputStream(HttpTrailer* trailer)
{
   return getConnection()->getBodyOutputStream(getHeader(), trailer);
}

IOException* HttpRequest::receiveBody(OutputStream* os, HttpTrailer* trailer)
{
   return getConnection()->receiveBody(getHeader(), os, trailer);
}

HttpRequestHeader* HttpRequest::getHeader()
{
   return &mHeader;
}

HttpConnection* HttpRequest::getConnection()
{
   return (HttpConnection*)mConnection;
}
