/*
 * span.h: SPectrum ANalyzer plugin core
 */

/*
 * Spectrum Analyzer plugin for VDR (C++)
 *
 * (C) 2006 Christian Leuschen <christian.leuschen@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ___SPAN_H
#define ___SPAN_H

#include <getopt.h>
#include <stdlib.h>

#include <vdr/interface.h>
#include <vdr/plugin.h>

#include "defines.h"
#include "setup.h"
#include "client.h"
#include "spectrum_analyzer.h"

class cPluginSpan : public cPlugin {
private:
  const char *option_a;
  bool option_b;
  cSpectrumAnalyzer *sa;
  cSaClient *client;
public:
  cPluginSpan(void);
  virtual ~cPluginSpan();
  virtual const char *Version(void);
  virtual const char *Description(void);
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void);
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  
  virtual bool Service(const char *Id, void *Data = NULL);
  };

#endif //___SPAN_H
