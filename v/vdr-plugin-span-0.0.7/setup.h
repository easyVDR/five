/*
 * setup.h: Configuration
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

#ifndef ___SETUP_H
#define ___SETUP_H

#include <getopt.h>
#include <stdlib.h>

#include <vdr/interface.h>
#include <vdr/plugin.h>

#include "defines.h"

// cMenuSetupSpan #################################################################
class cMenuSetupSpan : public cMenuSetupPage {
private:
  int newActivated;
  int newVisualizationDelay;
  int newUseLogarithmic;
  int newHideMainMenu;
  int newPureData;
  
  virtual void Setup(void);
protected:
  virtual void Store(void);
public:
  cMenuSetupSpan(void);
};

// cSpanSetup #####################################################################
class cSpanSetup {
public:
	// Global variables that control the overall behaviour:
	bool Activated;
    int VisualizationDelay;
	bool UseLogarithmic;
	bool HideMainMenu;
	bool PureData;
	
	cSpanSetup(void);
};

extern cSpanSetup SpanSetup;

#endif //___SETUP_H
