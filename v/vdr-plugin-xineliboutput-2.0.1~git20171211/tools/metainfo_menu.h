/*
 * metainfo_menu.h: Media file info menu
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef __XINELIB_INFO_MENU_H_
#define __XINELIB_INFO_MENU_H_

//
// cMetainfoMenu
//

#include <vdr/osdbase.h>

class cMetainfoMenu : public cOsdMenu
{
  protected:

    cString m_Filename;

  public:

    cMetainfoMenu(cString Filename);
    virtual ~cMetainfoMenu();

    virtual void Display(void);

    // cOsdMenu
    virtual eOSState ProcessKey(eKeys Key);

};

#endif // __XINELIB_INFO_MENU_H_
