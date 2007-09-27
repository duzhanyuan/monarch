/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "db/data/id3v2/FrameHeader.h"

using namespace std;
using namespace db::data::id3v2;

// initialize static variables
const int FrameHeader::HEADER_SIZE = 10;
const unsigned char FrameHeader::TAG_ALTERED_DISCARD_FRAME_BIT = 0x80;
const unsigned char FrameHeader::FILE_ALTERED_DISCARD_FRAME_BIT = 0x40;
const unsigned char FrameHeader::READ_ONLY_BIT = 0x20;
const unsigned char FrameHeader::COMPRESSION_BIT = 0x80;
const unsigned char FrameHeader::ENCRYPTION_BIT = 0x40;
const unsigned char FrameHeader::GROUPING_BIT = 0x20;

FrameHeader::FrameHeader(const char* id)
{
   mId = new char[5];
   memset(mId + 4, 0, 1);
   strncpy(mId, id, 4);
   
   mDescription = new char[1];
   memset(mDescription, 0, 1);
   
   mFrameSize = 0;
   mTagAlteredDiscardFrame = false;
   mFileAlteredDiscardFrame = false;
   mReadOnly = false;
   mCompressed = false;
   mEncrypted = false;
   mGrouped = false;
}

FrameHeader::~FrameHeader()
{
   delete [] mId;
   delete [] mDescription;
}

void FrameHeader::setFlags1(unsigned char b)
{
   mTagAlteredDiscardFrame = (b & TAG_ALTERED_DISCARD_FRAME_BIT) != 0;
   mFileAlteredDiscardFrame = (b & FILE_ALTERED_DISCARD_FRAME_BIT) != 0;
   mReadOnly = (b & READ_ONLY_BIT) != 0;
}

unsigned char FrameHeader::getFlagByte1()
{
   unsigned char b = 0x00;
   
   if(mTagAlteredDiscardFrame)
   {
      b |= TAG_ALTERED_DISCARD_FRAME_BIT;
   }
   
   if(mFileAlteredDiscardFrame)
   {
      b |= FILE_ALTERED_DISCARD_FRAME_BIT;
   }
   
   if(mReadOnly)
   {
      b |= READ_ONLY_BIT;
   }
   
   return b;
}

void FrameHeader::setFlags2(unsigned char b)
{
   mCompressed = (b & COMPRESSION_BIT) != 0;
   mEncrypted = (b & ENCRYPTION_BIT) != 0;
   mGrouped = (b & GROUPING_BIT) != 0;
}

unsigned char FrameHeader::getFlagByte2()
{
   unsigned char b = 0x00;
   
   if(mCompressed)
   {
      b |= COMPRESSION_BIT;
   }
   
   if(mEncrypted)
   {
      b |= ENCRYPTION_BIT;
   }
   
   if(mGrouped)
   {
      b |= GROUPING_BIT;
   }
   
   return b;
}

void FrameHeader::convertFromBytes(const char* b, int length)
{
   // convert ID
   char id[5];
   strncpy(id, b, 4);
   memset(id + 4, 0, 1);
   setId(id);
   
   // convert frame size
   setFrameSize(convertBytesToInt(b + 4));
   
   // convert flags
   setFlags1(b[8]);
   setFlags2(b[9]);
}

void FrameHeader::convertToBytes(char* b)
{
   // copy ID
   memcpy(b, getId(), 4);
   
   // set size
   convertIntToBytes(getFrameSize(), b + 4);
   
   // set flags
   b[8] = getFlagByte1();
   b[9] = getFlagByte2();
}

void FrameHeader::setId(const char* id)
{
   strncpy(mId, id, 4);
}

const char* FrameHeader::getId()
{
   return mId;
}

void FrameHeader::setDescription(const char* description)
{
   delete [] mDescription;
   mDescription = new char[strlen(description) + 1];
   strcpy(mDescription, description);
}

const char* FrameHeader::getDescription()
{
   return mDescription;
}

void FrameHeader::setFrameSize(int size)
{
   mFrameSize = size;
}

int FrameHeader::getFrameSize()
{
   return mFrameSize;
}

void FrameHeader::setTagAlteredDiscardFrame(bool discard)
{
   mTagAlteredDiscardFrame = discard;
}

bool FrameHeader::getTagAlteredDiscardFrame()
{
   return mTagAlteredDiscardFrame;
}

void FrameHeader::setFileAlteredDiscardFrame(bool discard)
{
   mFileAlteredDiscardFrame = discard;
}

bool FrameHeader::getFileAlteredDiscardFrame()
{
   return mFileAlteredDiscardFrame;
}

void FrameHeader::setReadOnly(bool readOnly)
{
   mReadOnly = readOnly;
}

bool FrameHeader::isReadOnly()
{
   return mReadOnly;
}

void FrameHeader::setCompressed(bool compressed)
{
   mCompressed = compressed;
}

bool FrameHeader::isCompressed()
{
   return mCompressed;
}

void FrameHeader::setEncrypted(bool encrypted)
{
   mEncrypted = encrypted;
}

bool FrameHeader::isEncrypted()
{
   return mEncrypted;
}

void FrameHeader::setGrouped(bool grouped)
{
   mGrouped = grouped;
}

bool FrameHeader::isGrouped()
{
   return mGrouped;
}

string& FrameHeader::toString(string& str)
{
   str.erase();
   
   str.append("[ID3TagFrameHeader]\n");
   str.append("Frame ID=");
   str.append(getId());
   str.append("\nFrame Size=");
   
   char temp[20];
   sprintf(temp, "%i", getFrameSize());
   str.append(1, '\n');
   
   return str;
}

void FrameHeader::convertIntToBytes(int integer, char* b)
{
   for(int i = 0; i < 4; i++)
   {
      b[i] = ((integer >> ((3 - i) * 8)) & 0xFF);
   }
}

int FrameHeader::convertBytesToInt(const char* b)
{
   int rval = 0;
   
   // most significant byte first
   for(int i = 0; i < 4; i++)
   {
      rval |= ((((unsigned char)b[i]) & 0xFF) << ((3 - i) * 8));
   }
   
   return rval;
}
