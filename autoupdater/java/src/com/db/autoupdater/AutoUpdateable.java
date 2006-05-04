/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.autoupdater;

/**
 * Any class that implements this interface can be auto-updated by an
 * AutoUpdater. 
 * 
 * @author Dave Longley
 */
public interface AutoUpdateable
{
   /**
    * Executes this auto-updateable application with the given parameters.
    * 
    * This method should load all of the necessary jar files to execute
    * this application.
    * 
    * @param args the arguments to execute this auto-updateable application
    *             with.
    */
   public void execute(String[] args);
   
   /**
    * Receives and processes arguments once this auto-updateable application
    * is already running.
    * 
    * @param args the arguments to process.
    */
   public void processArguments(String[] args);

   /**
    * Shuts down this auto-updateable application.
    */
   public void shutdown();
   
   /**
    * Returns true if this application should be restarted, false if not.
    * 
    * @return true if this application should be restarted, false if not.
    */
   public boolean shouldRestart();
}
