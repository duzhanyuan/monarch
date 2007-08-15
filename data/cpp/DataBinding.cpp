/*
 * Copyright (c) 2007 Digital Bazaar, Inc.  All rights reserved.
 */
#include "DataBinding.h"

using namespace std;
using namespace db::data;

DataBinding::DataBinding(void* obj)
{
   mObject = obj;
   mCurrentDataName = NULL;
}

DataBinding::~DataBinding()
{
   // clean up all data mappings
   for(map<DataName*, DataMapping*, DataNameComparator>::iterator i =
       mDataMappings.begin(); i != mDataMappings.end(); i++)
   {
      // free data name
      freeDataName(i->first);
   }
   
   // clean up all data bindings
   for(map<DataName*, DataBinding*, DataNameComparator>::iterator i =
       mDataBindings.begin(); i != mDataBindings.end(); i++)
   {
      // free data name
      freeDataName(i->first);
   }
   
   // free current data name
   freeDataName(mCurrentDataName);
}

bool DataBinding::DataNameComparator::operator()(
   DataName* dn1, DataName* dn2) const
{
   bool rval = false;
   
   if(dn1->ns == NULL && dn2->ns != NULL)
   {
      rval = true;
   }
   else if(dn1->ns != NULL && dn2->ns != NULL)
   {
      // compare namespaces
      int i = strcmp(dn1->ns, dn2->ns);
      if(i > 0)
      {
         rval = true;
      }
      else if(i == 0)
      {
         // compare names
         rval = strcmp(dn1->name, dn2->name) < 0;
      }
   }
   else if(dn1->ns == NULL && dn2->ns == NULL)
   {
      // compare names
      rval = strcmp(dn1->name, dn2->name) < 0;
   }
   
   return rval;
}

DataName* DataBinding::createDataName(const char* ns, const char* name)
{
   DataName* dn = new DataName();
   
   if(ns != NULL)
   {
      dn->ns = new char[strlen(ns) + 1];
      strcpy(dn->ns, ns);
   }
   else
   {
      dn->ns = NULL;
   }
   
   dn->name = new char[strlen(name) + 1];
   strcpy(dn->name, name);
   
   return dn;
}

void DataBinding::freeDataName(DataName* dn)
{
   if(dn != NULL)
   {
      // delete name space if exists
      if(dn->ns != NULL)
      {
         delete [] dn->ns;
      }
      
      // delete name
      delete [] dn->name;
      
      // delete data name
      delete dn;
   }
}

void DataBinding::addDataMapping(
   const char* ns, const char* name, DataMapping* dm)
{
   // create data name
   DataName* dn = createDataName(ns, name);
   
   // set data mapping
   mDataMappings[dn] = dm;
   
   // add data name to order
   mDataNameOrder.push_back(dn);
}

void DataBinding::addDataBinding(
   const char* ns, const char* name, DataBinding* db)
{
   // create data name
   DataName* dn = createDataName(ns, name);
   
   // set data binding
   mDataBindings[dn] = db;
   
   // add data name to order
   mDataNameOrder.push_back(dn);
}

DataBinding* DataBinding::startData(
   const char* charEncoding, const char* ns, const char* name) 
{
   DataBinding* rval = NULL;
   
   // create data name
   DataName* dn = createDataName(ns, name);
   
   // get data binding
   rval = getDataBinding(dn);
   if(rval != NULL)
   {
      // get data mapping
      DataMapping* dm = getDataMapping(dn);
      if(dm != NULL)
      {
         // create new child object for the data binding
         rval->mObject = dm->createChild(mObject);
      }
   }
   else
   {
      // use self for binding, no child to create
      rval = this;
   }
   
   // free old data name and set new one
   freeDataName(rval->mCurrentDataName);
   rval->mCurrentDataName = dn;
   
   return rval;
}

void DataBinding::appendData(
   const char* charEncoding, const char* data, unsigned int length)
{
   // get data mapping
   DataMapping* dm = getDataMapping(mCurrentDataName);
   if(dm != NULL)
   {
      // append data
      dm->appendData(mObject, data, length);
   }
}

void DataBinding::endData(
   const char* charEncoding, const char* ns, const char* name, DataBinding* db)
{
   // add child object if not using self as binding
   if(this != db)
   {
      // get data mapping
      DataMapping* dm = getDataMapping(db->mCurrentDataName);
      if(dm != NULL)
      {
         // add child object
         dm->addChild(mObject, db->mObject);
      }
   }
}

void DataBinding::setData(
   const char* charEncoding, const char* ns, const char* name,
   const char* data, unsigned int length)
{
   // create data name for look up
   DataName dn;
   dn.ns = (char*)ns;
   dn.name = (char*)name;
   
   // get data mapping
   DataMapping* dm = getDataMapping(&dn);
   if(dm != NULL)
   {
      // set data
      dm->setData(mObject, data, length);
   }
}

DataMapping* DataBinding::getDataMapping(DataName* dn)
{
   DataMapping* rval = NULL;
   
   // find data mapping
   map<DataName*, DataMapping*, DataNameComparator>::iterator i =
      mDataMappings.find(dn);
   if(i != mDataMappings.end())
   {
      rval = i->second;
   }
   
   return rval;
}

DataBinding* DataBinding::getDataBinding(DataName* dn)
{
   DataBinding* rval = NULL;
   
   // find data binding
   map<DataName*, DataBinding*, DataNameComparator>::iterator i =
      mDataBindings.find(dn);
   if(i != mDataBindings.end())
   {
      rval = i->second;
   }
   
   return rval;
}

list<DataName*>& DataBinding::getDataNames()
{
   return mDataNameOrder;
}
