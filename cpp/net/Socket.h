/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_net_Socket_H
#define monarch_net_Socket_H

#include "monarch/io/InputStream.h"
#include "monarch/io/OutputStream.h"
#include "monarch/net/SocketAddress.h"

#include <inttypes.h>

namespace monarch
{
namespace net
{

/**
 * A Socket is an interface for an end point for communication.
 *
 * If an exception occurs during an operation it can be retrieved via
 * getException().
 *
 * @author Dave Longley
 */
class Socket
{
public:
   /**
    * Creates a new Socket.
    */
   Socket() {};

   /**
    * Destructs this Socket.
    */
   virtual ~Socket() {};

   /**
    * Binds this Socket to a SocketAddress.
    *
    * @param address the address to bind to.
    *
    * @return true if bound, false if an exception occurred.
    */
   virtual bool bind(SocketAddress* address) = 0;

   /**
    * Causes this Socket to start listening for incoming connections.
    *
    * @param backlog the number of connections to keep backlogged.
    *
    * @return true if listening, false if an exception occurred.
    */
   virtual bool listen(int backlog = 50) = 0;

   /**
    * Accepts a connection to this Socket. This method will block until a
    * connection is made to this Socket.
    *
    * The returned Socket will wrap the file descriptor that is used to
    * communicated with the connected socket.
    *
    * @param timeout the timeout, in seconds, 0 for no timeout.
    *
    * @return an allocated Socket to use to communicate with the connected
    *         socket or NULL if an error occurs.
    */
   virtual Socket* accept(int timeout) = 0;

   /**
    * Connects this Socket to the given address.
    *
    * @param address the address to connect to.
    * @param timeout the timeout, in seconds, 0 for no timeout.
    *
    * @return true if connected, false if an exception occurred.
    */
   virtual bool connect(SocketAddress* address, int timeout = 30) = 0;

   /**
    * Writes raw data to this Socket. This method will block until all of
    * the data has been written.
    *
    * Note: This method is *not* preferred. Use getOutputStream() to obtain the
    * output stream for this Socket.
    *
    * @param b the array of bytes to write.
    * @param length the number of bytes to write to the stream.
    *
    * @return true if the data was sent, false if an exception occurred.
    */
   virtual bool send(const char* b, int length) = 0;

   /**
    * Reads raw data from this Socket. This method will block until at least
    * one byte can be read or until the end of the stream is reached (the
    * Socket has closed). A value of 0 will be returned if the end of the
    * stream has been reached, otherwise the number of bytes read will be
    * returned.
    *
    * Note: This method is *not* preferred. Use getInputStream() to obtain the
    * input stream for this Socket.
    *
    * @param b the array of bytes to fill.
    * @param length the maximum number of bytes to read into the buffer.
    *
    * @return the number of bytes read from the stream or 0 if the end of the
    *         stream (the Socket has closed) has been reached or -1 if an error
    *         occurred.
    */
   virtual int receive(char* b, int length) = 0;

   /**
    * Closes this Socket. This will be done automatically when the Socket is
    * destructed.
    */
   virtual void close() = 0;

   /**
    * Returns true if this Socket is bound, false if not.
    *
    * @return true if this Socket is bound, false if not.
    */
   virtual bool isBound() = 0;

   /**
    * Returns true if this Socket is listening, false if not.
    *
    * @return true if this Socket is listening, false if not.
    */
   virtual bool isListening() = 0;

   /**
    * Returns true if this Socket is connected, false if not.
    *
    * @return true if this Socket is connected, false if not.
    */
   virtual bool isConnected() = 0;

   /**
    * Gets the local SocketAddress for this Socket.
    *
    * @param address the SocketAddress to populate.
    *
    * @return true if the address was populated, false if an exception occurred.
    */
   virtual bool getLocalAddress(SocketAddress* address) = 0;

   /**
    * Gets the remote SocketAddress for this Socket.
    *
    * @param address the SocketAddress to populate.
    *
    * @return true if the address was populated, false if an exception occurred.
    */
   virtual bool getRemoteAddress(SocketAddress* address) = 0;

   /**
    * Gets the InputStream for reading from this Socket.
    *
    * @return the InputStream for reading from this Socket.
    */
   virtual monarch::io::InputStream* getInputStream() = 0;

   /**
    * Gets the OutputStream for writing to this Socket.
    *
    * @return the OutputStream for writing to this Socket.
    */
   virtual monarch::io::OutputStream* getOutputStream() = 0;

   /**
    * Sets the send timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to send data.
    *
    * @param timeout the send timeout in milliseconds.
    */
   virtual void setSendTimeout(uint32_t timeout) = 0;

   /**
    * Gets the send timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to send data.
    *
    * @return the send timeout in milliseconds.
    */
   virtual uint32_t getSendTimeout() = 0;

   /**
    * Sets the receive timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to receive data.
    *
    * @param timeout the receive timeout in milliseconds.
    */
   virtual void setReceiveTimeout(uint32_t timeout) = 0;

   /**
    * Gets the receive timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to receive data.
    *
    * @return the receive timeout in milliseconds.
    */
   virtual uint32_t getReceiveTimeout() = 0;

   /**
    * Gets the number of Socket connections that can be kept backlogged while
    * listening.
    *
    * @return the number of Socket connections that can be kept backlogged
    *         while listening.
    */
   virtual int getBacklog() = 0;

   /**
    * Gets the file descriptor for this Socket.
    *
    * @return the file descriptor for this Socket.
    */
   virtual int getFileDescriptor() = 0;

   /**
    * Gets the communication domain for this Socket, i.e. IPv4, IPv6.
    *
    * @return the communication domain for this Socket.
    */
   virtual SocketAddress::CommunicationDomain getCommunicationDomain() = 0;

   /**
    * Sets whether or not this Socket should not block when sending. If true,
    * then its send and OutputStream will return Exceptions when they would
    * block. An exception detail of "wouldBlock" will be set to true and
    * the number of bytes actually sent will be set to "written".
    *
    * @param on true to activate non-blocking send, false not to.
    */
   virtual void setSendNonBlocking(bool on) = 0;

   /**
    * Gets whether or not this Socket blocks when sending.
    *
    * @return true if this Socket does not block when sending, false if it
    *         does.
    */
   virtual bool isSendNonBlocking() = 0;

   /**
    * Sets whether or not this Socket should not block when receiving. If true,
    * then its receive and InputStream will return Exceptions when they would
    * block. An exception detail of "wouldBlock" will be set to true.
    *
    * @param on true to activate non-blocking receive, false not to.
    */
   virtual void setReceiveNonBlocking(bool on) = 0;

   /**
    * Gets whether or not this Socket blocks when receiving.
    *
    * @return true if this Socket does not block when receiving, false if it
    *         does.
    */
   virtual bool isReceiveNonBlocking() = 0;
};

} // end namespace net
} // end namespace monarch
#endif
