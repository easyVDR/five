/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 * (C) 2003 Kai Tobias Burwieck <kai@burwieck.net>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef ___MENU_IMAGE_H
#define ___MENU_IMAGE_H

#include <vdr/osdbase.h>
#include <vdr/menuitems.h>

#include "menu.h"

// ----------------------------------------------------------------
class cMenuImageBrowse:public cMenuBrowse {
  private:
    bool sourcing;
    void SetButtons(void);
    eOSState Source(bool second);
  public:
     cMenuImageBrowse(void);
    virtual eOSState ProcessKey(eKeys Key);
};

#endif				//___MENU_IMAGE_H
