/*
 * Copyright (c) 2007-2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef db_io_FileOutputStream_H
#define db_io_FileOutputStream_H

#include "db/io/File.h"
#include "db/io/OutputStream.h"

#include <cstdio>

namespace db
{
namespace io
{

/**
 * A FileOutputStream is used to write bytes to a file.
 *
 * @author Dave Longley
 */
class FileOutputStream : public OutputStream
{
public:
   /**
    * An enum for using stdout or stderr.
    */
   enum StdOutput
   {
      StdOut,
      StdErr
   };

protected:
   /**
    * The File to write to.
    */
   File mFile;

   /**
    * Whether or the file should be appended or overwritten.
    */
   bool mAppend;

   /**
    * The file handle to write with.
    */
   FILE* mHandle;

public:
   /**
    * Creates a new FileOutputStream that opens the passed File for writing.
    *
    * @param file the File to write to.
    * @param append true to append to the File if it exists, false to overwrite.
    */
   FileOutputStream(File& file, bool append = false);

   /**
    * Creates a new FileOutputStream that uses either stdout or stderror for
    * writing.
    *
    * @param out either FileOutputStream::StdOut or FileOutputStream::StdErr to
    *            write to.
    */
   FileOutputStream(StdOutput out);

   /**
    * Destructs this FileOutputStream.
    */
   virtual ~FileOutputStream();

   /**
    * Writes some bytes to the stream.
    *
    * @param b the array of bytes to write.
    * @param length the number of bytes to write to the stream.
    *
    * @return true if the write was successful, false if an IO exception
    *         occurred.
    */
   virtual bool write(const char* b, int length);

   /**
    * Forces this stream to flush its output, if any of it was buffered.
    *
    * Default implementation simply returns true.
    *
    * @return true if the write was successful, false if an IO exception
    *         occurred.
    */
   virtual bool flush();

   /**
    * Closes the stream.
    */
   virtual void close();

protected:
   /**
    * Ensures the file is open for writing.
    *
    * @return true if the file is opened for writing, false if it cannot be
    *         opened.
    */
   bool ensureOpen();
};

} // end namespace io
} // end namespace db
#endif
