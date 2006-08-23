/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.gui.wizard;

import javax.swing.BorderFactory;
import javax.swing.JPanel;

/**
 * A wizard page view is the most basic view for a wizard page.
 * 
 * @author Dave Longley
 */
public abstract class WizardPageView extends JPanel
{
   /**
    * The wizard page this view is for.
    */
   protected WizardPage mPage;
   
   /**
    * Creates a new WizardPageView.
    * 
    * @param page the wizard page this view is for.
    */
   public WizardPageView(WizardPage page)
   {
      // store page
      mPage = page;
      
      // add bevelled border by default
      setBorder(BorderFactory.createLoweredBevelBorder());
      
      // set to transparent by default
      setOpaque(false);
   }
   
   /**
    * Gets the wizard page this view is for.
    * 
    * @return the wizard page this view is for.
    */
   public WizardPage getPage()
   {
      return mPage;
   }
}
