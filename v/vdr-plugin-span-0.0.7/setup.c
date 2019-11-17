/*
 * setup.c: Configuration
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

#ifndef ___SETUP_C
#define ___SETUP_C

#include "setup.h"

cSpanSetup SpanSetup;

// cMenuSetupSpan #################################################################
cMenuSetupSpan::cMenuSetupSpan(void)
{
	Setup();
}
// ----------------------------------------------------------------------------

void cMenuSetupSpan::Setup(void)
{
  // store the values temporarily (enables "forgetting" them if you leave the setup with "Exit")
  newActivated 		    = SpanSetup.Activated;
  newVisualizationDelay = SpanSetup.VisualizationDelay;
  newUseLogarithmic     = SpanSetup.UseLogarithmic;
  newHideMainMenu 	    = SpanSetup.HideMainMenu;
  if (SpanSetup.UseLogarithmic)
  {
  	newPureData = 0;
  }
  else
  {
	newPureData		= SpanSetup.PureData;
  }

  int current = Current();

  Clear();
  Add(new cMenuEditBoolItem( tr("Setup.Span$Activate spectrum analyzer?"),  &newActivated,          tr("no"),	tr("yes")));
  Add(new cMenuEditIntItem( tr("Setup.Span$Visualization delay (in ms)"),   &newVisualizationDelay, 0,   2000));
  Add(new cMenuEditBoolItem( tr("Setup.Span$Use logarithmic diagram"),      &newUseLogarithmic,      tr("no"),	tr("yes")));
  Add(new cMenuEditBoolItem( tr("Hide mainmenu entry"),                     &newHideMainMenu,        tr("no"),	tr("yes")));
  if ( newUseLogarithmic )
  {
  	newPureData = 0;
  }
  Add(new cMenuEditBoolItem( tr("Setup.Span$Use pure (unequalized) data"),		&newPureData,		tr("no"),	tr("yes")));
  SetCurrent(Get(current));
  Display();
}
// ----------------------------------------------------------------------------

void cMenuSetupSpan::Store(void)
{
  SetupStore("Activated",			SpanSetup.Activated         = newActivated );
  SetupStore("VisualizationDelay",  SpanSetup.VisualizationDelay= newVisualizationDelay );
  SetupStore("UseLogarithmic",		SpanSetup.UseLogarithmic	= newUseLogarithmic );
  SetupStore("HideMainMenu",		SpanSetup.HideMainMenu		= newHideMainMenu );
  if (SpanSetup.UseLogarithmic)
  {
  	newPureData = 0;
  }
  SetupStore("PureData",			SpanSetup.PureData			= newPureData );
}

// cSpanSetup #####################################################################
cSpanSetup::cSpanSetup(void)
{
	Activated           = true;
    VisualizationDelay  = 300; // this should roughly be the delay for FF-Cards
	UseLogarithmic      = false;
	HideMainMenu	    = true;
	PureData		    = false;
}

#endif //___SETUP_C
