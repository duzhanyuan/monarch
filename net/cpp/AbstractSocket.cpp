/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "AbstractSocket.h"
#include "SocketDefinitions.h"
#include "InterruptedException.h"
#include "PeekInputStream.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"
#include "Thread.h"

using namespace db::io;
using namespace db::net;
using namespace db::rt;

AbstractSocket::AbstractSocket()
{
   // file descriptor is invalid at this point
   mFileDescriptor = -1;
   
   // not bound, listening, or connected
   mBound = false;
   mListening = false;
   mConnected = false;
   
   // input/output uninitialized
   mInputStream = NULL;
   mOutputStream = NULL;
   
   // no receive or send timeouts (socket will block)
   mReceiveTimeout = 0;
   mSendTimeout = 0;
   
   // default backlog is 50
   mBacklog = 50;
}

AbstractSocket::~AbstractSocket()
{
   // close socket
   close();
}

void AbstractSocket::create(int domain, int type, int protocol)
throw(SocketException)
{
   int fd = socket(domain, type, protocol);
   if(fd < 0)
   {
      throw SocketException("Could not create Socket!", strerror(errno));
   }
   
   // set reuse address flag
   // disables "address already in use" errors by reclaiming ports that
   // are waiting to be cleaned up
   int reuse = 1;
   int error = setsockopt(
      fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
   if(error < 0)
   {
      // close socket
      close();
      
      // throw exception
      throw SocketException("Could not create Socket!", strerror(errno));
   }
   
   mFileDescriptor = fd;
}

void AbstractSocket::select(bool read, unsigned long long timeout)
throw(SocketException)
{
   // get the current thread
   Thread* thread = Thread::currentThread();
   if(thread != NULL && thread->isInterrupted())
   {
      if(read)
      {
         throw InterruptedException("Socket read interrupted!");
      }
      else
      {
         throw InterruptedException("Socket write interrupted!");
      }
   }
   
   // create a file descriptor set to select on
   fd_set fds;
   FD_ZERO(&fds);
   
   // add file descriptor to set
   FD_SET((unsigned int)mFileDescriptor, &fds);
   
   // "n" parameter is the highest numbered descriptor plus 1
   int n = mFileDescriptor + 1;
   
   // create timeout
   struct timeval* tv = NULL;
   struct timeval to;
   if(timeout > 0)
   {
      // set timeout (1 millisecond is 1000 microseconds) 
      to.tv_sec = timeout / 1000LL;
      to.tv_usec = (timeout % 1000LL) * 1000LL;
      tv = &to;
   }
   
   int error;
   if(read)
   {
      // wait for data to arrive for reading or for an exception
      error = ::select(n, &fds, NULL, &fds, tv);
   }
   else
   {
      // wait for writability or for an exception
      error = ::select(n, NULL, &fds, &fds, tv);
   }
   
   if(error < 0)
   {
      if(errno == EINTR)
      {
         if(read)
         {
            // throw interrupted exception
            throw InterruptedException(
               "Socket read interrupted!", strerror(errno));
         }
         else
         {
            // throw interrupted exception
            throw InterruptedException(
               "Socket write interrupted!", strerror(errno));
         }
      }
      
      if(read)
      {
         // error occurred, get string message
         throw SocketException("Could not read from Socket!", strerror(errno));
      }
      else
      {
         // error occurred, get string message
         throw SocketException("Could not write to Socket!", strerror(errno));
      }
   }
   else if(error == 0)
   {
      if(read)
      {
         // read timeout occurred
         throw SocketTimeoutException(
            "Socket read timed out!", strerror(errno));
      }
      else
      {
         // write timeout occurred
         throw SocketTimeoutException(
            "Socket write timed out!", strerror(errno));
      }
   }
}

void AbstractSocket::initializeInput() throw(SocketException)
{
   if(mInputStream == NULL)
   {
      // create input stream
      mInputStream = new PeekInputStream(new SocketInputStream(this), true);
   }
}

void AbstractSocket::initializeOutput() throw(SocketException)
{
   if(mOutputStream == NULL)
   {
      // create output stream
      mOutputStream = new SocketOutputStream(this);
   }
}

void AbstractSocket::shutdownInput() throw(SocketException)
{
   // delete input stream
   if(mInputStream == NULL)
   {
      delete mInputStream;
   }
}

void AbstractSocket::shutdownOutput() throw(SocketException)
{
   // delete output stream
   if(mOutputStream == NULL)
   {
      delete mOutputStream;
   }
}

void AbstractSocket::bind(SocketAddress* address) throw(SocketException)
{
   // acquire file descriptor
   acquireFileDescriptor(address->getProtocol());
   
   // populate address structure
   unsigned int size = 130;
   char addr[size];
   address->toSockAddr((sockaddr*)&addr, size);
   
   // bind
   int error = ::bind(mFileDescriptor, (sockaddr*)&addr, size);
   if(error < 0)
   {
      throw new SocketException("Could not bind Socket!", strerror(errno));
   }
   
   // initialize input and output
   initializeInput();
   initializeOutput();
   
   // now bound
   mBound = true;
}

void AbstractSocket::listen(unsigned int backlog) throw(SocketException)
{
   if(!isBound())
   {
      // throw exception
      throw SocketException("Cannot listen on unbound Socket!"); 
   }
   
   // set backlog
   mBacklog = backlog;
   
   // listen
   int error = ::listen(mFileDescriptor, backlog);
   if(error < 0)
   {
      throw SocketException("Could not listen on Socket!", strerror(errno));
   }
   
   // now listening
   mListening = true;
}

Socket* AbstractSocket::accept(unsigned int timeout) throw(SocketException)
{
   if(!isListening())
   {
      throw SocketException("Cannot accept with a non-listening Socket!");
   }
   
   // wait for a connection
   select(true, timeout * 1000LL);
   
   // accept a connection
   int fd = ::accept(mFileDescriptor, NULL, NULL);
   if(fd < 0)
   {
      throw SocketException("Could not accept connection!", strerror(errno));
   }
   
   // create a connected Socket
   return createConnectedSocket(fd);
}

void AbstractSocket::connect(SocketAddress* address, unsigned int timeout)
throw(SocketException)
{
   // acquire file descriptor
   acquireFileDescriptor(address->getProtocol());
   
   // populate address structure
   unsigned int size = 130;
   char addr[size];
   address->toSockAddr((sockaddr*)&addr, size);
   
   // make socket non-blocking temporarily
   fcntl(mFileDescriptor, F_SETFL, O_NONBLOCK);
   
   // connect
   int error = ::connect(mFileDescriptor, (sockaddr*)addr, size);
   
   if(error < 0)
   {
      try
      {
         // wait until the connection can be written to
         select(false, timeout * 1000LL);
      }
      catch(SocketTimeoutException &e)
      {
         // restore socket to blocking
         fcntl(mFileDescriptor, F_SETFL, 0);
         
         // throw exception
         throw SocketTimeoutException(
            "Socket connection timed out!", e.getCode());
      }
      
      // get the last error on the socket
      int lastError;
      socklen_t lastErrorLength = sizeof(lastError);
      getsockopt(
         mFileDescriptor, SOL_SOCKET, SO_ERROR,
         (char*)&lastError, &lastErrorLength);
      if(lastError != 0)
      {
         // restore socket to blocking
         fcntl(mFileDescriptor, F_SETFL, 0);
         
         // throw exception
         throw SocketException(
            "Could not connect Socket! Connection refused.",
            strerror(lastError));
      }
   }
   
   // restore socket to blocking
   fcntl(mFileDescriptor, F_SETFL, 0);
   
   // initialize input and output
   initializeInput();
   initializeOutput();
   
   // now connected and bound
   mBound = true;
   mConnected = true;
}

void AbstractSocket::send(const char* b, unsigned int length) throw(IOException)
{
   if(!isBound())
   {
      throw SocketException("Cannot write to unbound Socket!");
   }
   
   // send all data (send can fail to send all bytes in one go because the
   // socket send buffer was full)
   unsigned int offset = 0;
   while(length > 0)
   {
      // wait for socket to become writable
      select(false, getSendTimeout());
      
      // send some data
      int bytes = ::send(mFileDescriptor, b + offset, length, 0);
      if(bytes < 0)
      {
         throw SocketException("Could not write to Socket!", strerror(errno));
      }
      else if(bytes > 0)
      {
         offset += bytes;
         length -= bytes;
      }
   }
}

int AbstractSocket::receive(char* b, unsigned int length) throw(IOException)
{
   int rval = -1;
   
   if(!isBound())
   {
      throw SocketException("Cannot read from unbound Socket!");
   }
   
   // wait for data to become available
   select(true, getReceiveTimeout());
   
   // receive some data
   rval = ::recv(mFileDescriptor, b, length, 0);
   if(rval < -1)
   {
      throw SocketException("Could not read from Socket!", strerror(errno));
   }
   else if(rval == 0)
   {
      // socket is closed now
      rval = -1;
   }
   
   return rval;
}

void AbstractSocket::close()
{
   if(mFileDescriptor != -1)
   {
      // shutdown input and output
      shutdownInput();
      shutdownOutput();
      
      // close the socket
      ::close(mFileDescriptor);
      
      // file descriptor is invalid again
      mFileDescriptor = -1;
      
      // not bound, listening, or connected
      mBound = false;
      mListening = false;
      mConnected = false;
   }
}

bool AbstractSocket::isBound()
{
   return mBound;
}

bool AbstractSocket::isListening()
{
   return mListening;
}

bool AbstractSocket::isConnected()
{
   return mConnected;
}

void AbstractSocket::getLocalAddress(SocketAddress* address)
throw(SocketException)
{
   if(!isBound())
   {
      throw SocketException("Cannot get local address for an unbound Socket!");
   }
   
   // get address structure
   socklen_t size = 130;
   char addr[size];
   
   // get local information
   int error = getsockname(mFileDescriptor, (sockaddr*)&addr, &size);
   if(error < 0)
   {
      throw SocketException(
         "Could not get Socket local address!", strerror(errno));
   }
   
   // convert socket address
   address->fromSockAddr((sockaddr*)&addr, size);
}

void AbstractSocket::getRemoteAddress(SocketAddress* address)
throw(SocketException)
{
   if(!isConnected())
   {
      throw SocketException(
         "Cannot get local address for an unconnected Socket!");
   }
   
   // get address structure
   socklen_t size = 130;
   char addr[size];
   
   // get remote information
   int error = getpeername(mFileDescriptor, (sockaddr*)&addr, &size);
   if(error < 0)
   {
      throw SocketException(
         "Could not get Socket remote address!", strerror(errno));
   }
   
   // convert socket address
   address->fromSockAddr((sockaddr*)&addr, size);
}

InputStream* AbstractSocket::getInputStream()
{
   return mInputStream;
}

OutputStream* AbstractSocket::getOutputStream()
{
   return mOutputStream;
}

void AbstractSocket::setSendTimeout(unsigned long long timeout)
{
   mSendTimeout = timeout;
}

unsigned long long AbstractSocket::getSendTimeout()
{
   return mSendTimeout;
}

void AbstractSocket::setReceiveTimeout(unsigned long long timeout)
{
   mReceiveTimeout = timeout;
}

unsigned long long AbstractSocket::getReceiveTimeout()
{
   return mReceiveTimeout;
}

unsigned int AbstractSocket::getBacklog()
{
   return mBacklog;
}

int AbstractSocket::getFileDescriptor()
{
   return mFileDescriptor;
}
