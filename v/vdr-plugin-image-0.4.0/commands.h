/*
 * Image plugin to VDR (C++)
 *
 * (c) 2004-2011 Andreas Brachold <vdr07 at deltab.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

// --- cImageCommands ---------------------------------------------------------

#ifndef __COMMANDS_H
#define __COMMANDS_H

#include <vdr/tools.h>

class cImageCommand
: public cListObject 
{
  char *m_szTitle;
  char *m_szCommand;
  bool m_bConfirm;
  static char *m_szLastResult;
public:
  cImageCommand(void);
  virtual ~ cImageCommand();

  bool Parse(const char *s);

  const char *Title(void) const  { return m_szTitle;  } 
  bool Confirm(void) const       { return m_bConfirm; }
  const char *Command(void) const{ return m_szCommand;}

  const char *Execute(const char *szFileName = NULL);
};

class cImageCommands
: public cList < cImageCommand > 
{
  char *m_szFileName;
  bool m_bAllowComments;
protected:
  void Clear(void);
public:
  cImageCommands(void);
  virtual ~ cImageCommands();

  const char *FileName(void) const { return m_szFileName; }
  bool Load(const char *szFileName = NULL, bool bAllowComments = true, bool bMustExist = false);
};

#endif				//__COMMANDS_H
