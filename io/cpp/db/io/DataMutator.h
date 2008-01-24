/*
 * Copyright (c) 2007-2008 Digital Bazaar, Inc.  All rights reserved.
 */
#ifndef db_io_DataMutator_H
#define db_io_DataMutator_H

#include "db/io/ByteBuffer.h"
#include "db/io/DataMutationAlgorithm.h"

namespace db
{
namespace io
{

/**
 * A DataMutator mutates data that is stored in a ByteBuffer and writes it to
 * another ByteBuffer from which it can be retrieved.
 * 
 * @author Dave Longley
 */
class DataMutator : protected DataMutationAlgorithm
{
protected:
   /**
    * The source ByteBuffer with unmutated data.
    */
   ByteBuffer* mSource;
   
   /**
    * The destination ByteBuffer with mutated data.
    */
   ByteBuffer* mDestination;
   
   /**
    * The algorithm used to mutate data.
    */
   DataMutationAlgorithm* mAlgorithm;
   
   /**
    * Stores the algorithm exit code (2 or 3) or 0 if the algorithm is not
    * completed.
    */
   int mAlgorithmExitCode;
   
   /**
    * A ByteBuffer used as a wrapper for source bytes to improve performance
    * when input data doesn't need to be cached.
    */
   ByteBuffer mInputWrapper;
   
   /**
    * Gets data out of the source ByteBuffer, mutates it in some implementation
    * specific fashion, and then puts it in the destination ByteBuffer.
    * 
    * If the mutation algorithm requires more data in the source buffer to
    * execute its next step then this method must return 0.
    * 
    * If the mutation algorithm had enough data to execute its next step,
    * regardless of whether or not it wrote data to the destination buffer,
    * this method must return 1.
    * 
    * If the mutation algorithm completed and any remaining source data must
    * be passed on to the destination buffer, this method must return 2.
    * 
    * If the mutation algorithm completed and any remaining source data must
    * be cleared (and *not* passed on to the destination buffer), this method
    * must return 3.
    * 
    * If an exception occurs, this method must return -1.
    * 
    * Note: The destination buffer may be resized to accommodate any mutated
    * bytes.
    * 
    * @param src the source ByteBuffer with bytes to mutate.
    * @param dest the destination ByteBuffer to write the mutated bytes to.
    * @param finish true if there will be no more source data and the
    *               mutation algorithm should finish, false if there is
    *               more data.
    * 
    * @return 0 if more data is needed, 1 if there was enough data, 2 if 
    *         the algorithm is complete and any remaining source data should
    *         be passed on, 3 if the algorithm is complete and any remaining
    *         source data should be truncated, -1 if an exception occurred.
    */
   virtual int mutateData(ByteBuffer* src, ByteBuffer* dest, bool finish);
   
public:
   /**
    * Creates a new DataMutator.
    * 
    * @param src the source ByteBuffer to read from.
    * @param dest the destination ByteBuffer to write to.
    */
   DataMutator(ByteBuffer* src, ByteBuffer* dest);
   
   /**
    * Destructs this DataMutator.
    */
   virtual ~DataMutator();
   
   /**
    * Sets the data mutation algorithm for this mutator.
    * 
    * @param algorithm the DataMutationAlgorithm to use.
    */
   virtual void setAlgorithm(DataMutationAlgorithm* algorithm);
   
   /**
    * Runs a mutation algorithm on data read from the passed input stream
    * until some mutated bytes are available.
    * 
    * This method will block until this DataMutator has mutated data that can
    * be retrieved or until the end of the input stream has been reached and
    * there is no more mutated data available.
    * 
    * @param is the input stream to read from.
    * 
    * @return the number of mutated bytes, or 0 if the end of the stream has
    *         been reached and no more mutated data is available, or -1 if an
    *         exception occurred.
    */
   virtual int mutate(InputStream* is);
   
   /**
    * Runs a mutation algorithm on the passed data and returns the number of
    * mutated bytes. If the data cannot be mutated, it will be stored in an
    * internal buffer which will be appended to with the next mutate() call.
    * 
    * Keep in mind that that buffer will be expanded to accommodate any number
    * of bytes that cannot be mutated.
    * 
    * This method is non-blocking.
    * 
    * @param b the bytes to mutate.
    * @param length the number of bytes to mutate (0 to end the mutation).
    * 
    * @return the number of mutated bytes, which may be 0, or -1 if an
    *         exception occurred.
    */
   virtual int mutate(const char* b, int length);
   
   /**
    * Runs a mutation algorithm on data read from the passed input stream
    * and skips the mutated bytes that are generated.
    * 
    * This method will block until this DataMutator has skipped the some
    * mutated data (it will try to skip the requested amount) or until the
    * end of the input stream has been reached and there is no more mutated
    * data to skip.
    * 
    * @param is the input stream to read from.
    * @param count the number of mutated bytes to skip. 
    * 
    * @return the number of skipped mutated bytes.
    */
   virtual long long skipMutatedBytes(InputStream* is, long long count);
   
   /**
    * Gets data out of this mutator and puts it into the passed buffer. The
    * amount of data may be less than the requested amount if this mutator
    * does not have enough data.
    * 
    * @param b the buffer to put the retrieved data into.
    * @param length the maximum number of bytes to get.
    * 
    * @return the actual number of bytes retrieved, which may be 0 if this
    *         mutator is empty.
    */
   virtual int get(char* b, int length);
   
   /**
    * Returns true if this mutator has data that can be obtained, false if
    * not.
    * 
    * @return true if this mutator has data that can be obtained, false if
    *         not.
    */
   virtual bool hasData();
   
   /**
    * Sets the source ByteBuffer for this DataMutator.
    * 
    * @param src the source ByteBuffer to read from.
    */
   virtual void setSource(ByteBuffer* src);
   
   /**
    * Gets direct access to the source ByteBuffer for this DataMutator.
    * 
    * @return this DataMutator's source ByteBuffer.
    */
   virtual ByteBuffer* getSource();
   
   /**
    * Sets the destination ByteBuffer for this DataMutator.
    * 
    * @param src the source ByteBuffer to read from.
    */
   virtual void setDestination(ByteBuffer* dest);
   
   /**
    * Gets direct access to the destination ByteBuffer for this DataMutator.
    * 
    * @return this DataMutator's destination ByteBuffer.
    */
   virtual ByteBuffer* getDestination();
};

} // end namespace io
} // end namespace db
#endif
