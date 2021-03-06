/*
 * Copyright (c) 2009 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_config_ConfigChangeListener_H
#define monarch_config_ConfigChangeListener_H

#include "monarch/config/ConfigManager.h"

namespace monarch
{
namespace config
{

/**
 * A ConfigChangeListener interface is called by a ConfigManager when a
 * configuration change occurs.
 *
 * @author Dave Longley
 */
class ConfigChangeListener
{
public:
   /**
    * Creates a new ConfigChangeListener.
    */
   ConfigChangeListener() {};

   /**
    * Destructs this ConfigChangeListener.
    */
   virtual ~ConfigChangeListener() {};

   /**
    * Called by a ConfigManager when a config is added.
    *
    * @param cm the ConfigManager.
    * @param id the ID of the config.
    */
   virtual void configAdded(
      ConfigManager* cm, ConfigManager::ConfigId id) = 0;

   /**
    * Called by a ConfigManager when a config is changed.
    *
    * @param cm the ConfigManager.
    * @param id the ID of the config.
    * @param diff the diff between the old config and the new one.
    */
   virtual void configChanged(
      ConfigManager* cm, ConfigManager::ConfigId id, Config& diff) = 0;

   /**
    * Called by a ConfigManager when a config is removed.
    *
    * @param cm the ConfigManager.
    * @param id the ID of the config.
    */
   virtual void configRemoved(
      ConfigManager* cm, ConfigManager::ConfigId id) = 0;

   /**
    * Called by a ConfigManager when all configs are cleared.
    *
    * @param cm the ConfigManager.
    */
   virtual void configCleared(
      ConfigManager* cm) = 0;
};

} // end namespace config
} // end namespace monarch
#endif
