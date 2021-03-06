/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_data_AbstractDataFormatInspector_H
#define monarch_data_AbstractDataFormatInspector_H

#include "monarch/data/DataFormatInspector.h"
#include "monarch/data/DataInspector.h"

namespace monarch
{
namespace data
{

/**
 * An AbstractDataFormatInspector inspects data by checking to see if its
 * format is recognized or not. Extending classes provide the actual
 * implementation details for detected particular data formats.
 *
 * @author Dave Longley
 */
class AbstractDataFormatInspector : public DataFormatInspector
{
protected:
   /**
    * A flag that indicates whether or not this inspector has inspected
    * enough data to determine if it recognizes its format.
    */
   bool mDataSatisfied;

   /**
    * A flag that indicates whether or not this inspector recognizes the format
    * of the inspected data.
    */
   bool mFormatRecognized;

   /**
    * True if this inspector should continue to inspect data after successfully
    * recognizing the data format, false if not.
    */
   bool mInspectAfterSatisfied;

   /**
    * The total number of bytes inspected so far.
    */
   uint64_t mBytesInspected;

   /**
    * Stores the current number of bytes that the specific inspection
    * implementation can skip over.
    */
   uint64_t mSkipBytes;

   /**
    * Called when enough data has been inspected for this inspector to
    * determine whether or not the format of the data is recognized.
    *
    * @param recognized true if the data format is recognized, false if not.
    */
   virtual void setFormatRecognized(bool recognized);

   /**
    * Sets the current number of bytes that the inspection algorithm can skip
    * over. This is the number of bytes that will pass through this inspector
    * without calling detectFormat().
    *
    * @param count the current number of bytes that the inspection algorithm
    *              can skip over.
    */
   virtual void setSkipBytes(int64_t count);

public:
   /**
    * Creates a new AbstractDataFormatInspector.
    */
   AbstractDataFormatInspector();

   /**
    * Destructs this AbstractDataFormatInspector.
    */
   virtual ~AbstractDataFormatInspector();

   /**
    * Inspects the data in the passed buffer for some implementation
    * specific attributes. This method returns the number of bytes that
    * were successfully inspected such that the passed buffer can safely
    * clear that number of bytes.
    *
    * An inspector should treat calls to this method as if the data in
    * the passed buffer is consecutive in nature (i.e. read from a stream).
    *
    * This method may return a number of bytes that is actually greater
    * than the number passed in, if the DataInspector can determine it does
    * not need to inspect them directly.
    *
    * @param b the buffer with data to inspect.
    * @param length the number of bytes in the buffer.
    *
    * @return the number of bytes inspected by this DataInspector and that
    *         should not be passed to it again.
    */
   virtual int inspectData(const char* b, int length);

   /**
    * Returns whether or not this inspector is "data-satisfied." The inspector
    * is data-satisfied when it has inspected enough data to determine whether
    * or not the format of the data is recognized.
    *
    * @return true if this inspector has inspected enough data to determine
    *         if the format is or is not recognized, false if not.
    */
   virtual bool isDataSatisfied();

   /**
    * Returns whether or not this inspector has recognized the format of
    * the data it has been inspecting.
    *
    * @return true if the data format is recognized, false if not.
    */
   virtual bool isFormatRecognized();

   /**
    * Sets whether or not this inspector should keep inspecting data after
    * successfully recognizing the data format, false if not.
    *
    * @param inspect true to keep inspecting, false not to.
    */
   virtual void setKeepInspecting(bool inspect);

   /**
    * Gets whether or not this inspector should keep inspecting data after
    * successfully recognizing the data format, false if not.
    *
    * @return true to keep inspecting, false not to.
    */
   virtual bool keepInspecting();

   /**
    * Gets the total number of bytes inspected so far.
    *
    * @return the number of bytes inspected so far.
    */
   virtual uint64_t getBytesInspected();

   /**
    * Inspects the data in the passed buffer and tries to detect its
    * format. The number of bytes that were inspected is returned so that
    * they can be safely cleared from the passed buffer.
    *
    * Subsequent calls to this method should be treated as if the data
    * in the passed buffer is consecutive in nature (i.e. read from a stream).
    *
    * Once this inspector has determined that the inspected data is in
    * a known or unknown format, this inspector may opt to stop inspecting
    * data.
    *
    * @param b the buffer with data to inspect.
    * @param length the maximum number of bytes to inspect.
    *
    * @return the number of bytes that were inspected in the passed buffer.
    */
   virtual int detectFormat(const char* b, int length) = 0;
};

} // end namespace data
} // end namespace monarch
#endif
