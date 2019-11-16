/*
 * dm140vfd plugin for VDR (C++)
 *
 * (C) 2010 Andreas Brachold <vdr07 AT deltab de>
 *
 * This dm140vfd plugin is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 3 of the License.
 *
 * See the files README and COPYING for details.
 *
 */

#ifndef __DM140VFD_H___
#define __DM140VFD_H___

#include <vdr/plugin.h>

#include "vfd.h"
#include "watch.h"
#include "status.h"

class cPluginDm140VFD : public cPlugin {
private:
  cVFDStatusMonitor *statusMonitor;
  cVFDWatch         m_dev;
  bool               m_bSuspend;
  char*              m_szIconHelpPage;
protected:
  bool resume();
  bool suspend();

  const char* SVDRPCommandOn(const char *Option, int &ReplyCode);
  const char* SVDRPCommandOff(const char *Option, int &ReplyCode);
  const char* SVDRPCommandIcon(const char *Option, int &ReplyCode);

public:
  cPluginDm140VFD(void);
  virtual ~cPluginDm140VFD();
  virtual const char *Version(void);
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);

};

#endif
