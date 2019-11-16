/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef __MENU_COMMANDS_H
#define __MENU_COMMANDS_H

#include "image.h"
#include "commands.h"
#include <vdr/menuitems.h>
#include <vdr/interface.h>

class cImageMenuCommands
: public cOsdMenu 
{
  cImageCommands *m_pCmds;
  char *m_szFileName;
  bool m_bImageChanged;
  char *m_szTitle;
protected:
  eOSState Execute(void);
public:
  cImageMenuCommands(const char *szTitle, cImageCommands *Cmds, const char *szFileName);
  virtual ~cImageMenuCommands();
  virtual eOSState ProcessKey(eKeys Key);
  bool HasImageChanged() const { return m_bImageChanged; } 
};
  

class cImageMenuResult : public cOsdMenu 
{
	const char *m_szText;
protected:
	virtual void Display(void);
public:
    cImageMenuResult(const char *Title, const char *Text);
    virtual eOSState ProcessKey(eKeys Key);
};


#endif //__MENU_COMMANDS_H
