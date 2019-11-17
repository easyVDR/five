/*
 * span.c: SPectrum ANalyzer plugin core
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

#ifndef ___SPAN_C
#define ___SPAN_C

/*
 * span.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "span.h"
#include "menu.h"

static const char *VERSION        = "0.0.7";
static const char *DESCRIPTION    =  trNOOP("Spectrum Analyzer for music-data");
static const char *MAINMENUENTRY  =  trNOOP("Spectrum Analyzer");

// --- cPluginSpan ----------------------------------------------------------

cPluginSpan::cPluginSpan(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  sa = NULL;
}

cPluginSpan::~cPluginSpan()
{
  delete sa;
}

const char *cPluginSpan::Version(void)
{
    return VERSION;
}

const char *cPluginSpan::Description(void)
{
    return tr(DESCRIPTION);
}

const char *cPluginSpan::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginSpan::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginSpan::Start(void)
{
  // Start any background activities the plugin shall perform.
  sa = new cSpectrumAnalyzer;
  return true;
}

void cPluginSpan::Housekeeping(void)
{
	// Perform any cleanup or other regular tasks.
}

const char *cPluginSpan::MainMenuEntry(void)
{
  return SpanSetup.HideMainMenu ? 0 : tr(MAINMENUENTRY);
}

cOsdObject *cPluginSpan::MainMenuAction(void)
{
	// Perform the action when selected from the main VDR menu.
	return new cSpanOsdMenu;
//	return NULL;
}

cMenuSetupPage *cPluginSpan::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupSpan;
}

bool cPluginSpan::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if      (!strcasecmp(Name, "Activated"))			SpanSetup.Activated			= atoi(Value);
  else if (!strcasecmp(Name, "VisualizationDelay")) SpanSetup.VisualizationDelay= atoi(Value);
  else if (!strcasecmp(Name, "UseLogarithmic"))		SpanSetup.UseLogarithmic	= atoi(Value);
  else if (!strcasecmp(Name, "HideMainMenu"))		SpanSetup.HideMainMenu		= atoi(Value);
  else if (!strcasecmp(Name, "PureData"))			SpanSetup.PureData			= atoi(Value);
  else
     return false;
  return true;
}

bool cPluginSpan::Service(const char *Id, void *Data)
{
	// no, we do not want to "waste" CPU-time or provoke a segfault ;-)
	if ( !SpanSetup.Activated || sa == NULL )
		return false;
	if (strcmp(Id, SPAN_SET_PCM_DATA_ID) == 0)
	{
		
//		if ( sa->requestTime.TimedOut() ) // why working if there are no clients requesting our data?!
//		{
////			return false;
//		}
//		else
		
		if (Data == NULL)
		{
			return true;
		}
		else if (Data != NULL)
		{
			sa->putPcmData( ((Span_SetPcmData_1_0*)Data)->data,
							((Span_SetPcmData_1_0*)Data)->length,
							((Span_SetPcmData_1_0*)Data)->index,
							((Span_SetPcmData_1_0*)Data)->bufferSize,
							((Span_SetPcmData_1_0*)Data)->bigEndian);
			return true;
		}
	}
	else if (strcmp(Id, SPAN_SET_PLAYINDEX_ID) == 0)
	{
		if (Data == NULL)
		{
			return true;
		}
		else return sa->setPlayindex( ((Span_SetPlayindex_1_0*)Data)->index );
	}
	else if (strcmp(Id, SPAN_GET_BAR_HEIGHTS_ID) == 0) {
		if ( sa == NULL )
			return false;
		sa->requestTime.Set(DATA_TIMEOUT); // clients are requesting -> accept pcm16-data

		// In case there is a replay which does not provide pcm16-data:
		// There was no input data for some time: Do not provide the "out-of-date" or "zeroed" results
		// and delete all clients that have been created at the start of the replay, due to !sa->timedOut()
		if ( sa->timedOut() )
		{
			sa->deleteClients();
			return false;
		}
		if (Data == NULL)
		{
			return true;
		}
		if (Data != NULL)
		{
			int found = -1;
			int i = 0;
			
			while ( i < sa->clients.Count() )
			{
				if ( (sa->clients.Get(i))->timedOut() )
				{
					sa->clients.Del( sa->clients.Get(i), true );
					dsyslog("[span]: deleted client %d (%s()(%s,%d))", i, __func__, __FILE__, __LINE__ );
					i--;
				}
				else if ( 0 == (sa->clients.Get(i))->Compare( ((Span_GetBarHeights_v1_0*)(Data))->name) )
				{
					found = i;
					client = sa->clients.Get(i);
				}
				i++;
			}

			if ( found < 0 )
			{
				client = new cSaClient( ((Span_GetBarHeights_v1_0*)Data)->name,
											((Span_GetBarHeights_v1_0*)Data)->bands,
											((Span_GetBarHeights_v1_0*)Data)->falloff );
				sa->clients.Add( client );
				dsyslog("[span]: added client %s (%s()(%s,%d))", ((Span_GetBarHeights_v1_0*)Data)->name, __func__, __FILE__, __LINE__ );
			}
			if ( (int)((Span_GetBarHeights_v1_0*)Data)->bands != client->bands )
			{
				sa->clients.Del( client, true );
				sa->clients.Add( new cSaClient( ((Span_GetBarHeights_v1_0*)Data)->name,
											((Span_GetBarHeights_v1_0*)Data)->bands,
											((Span_GetBarHeights_v1_0*)Data)->falloff )
								);
				client = sa->clients.Get(i-1);
				dsyslog("[span]: readded client %s (%s()(%s,%d))", ((Span_GetBarHeights_v1_0*)Data)->name, __func__, __FILE__, __LINE__ );				
			}
			else if ( (int)((Span_GetBarHeights_v1_0*)Data)->falloff != client->getFalloff() )
			{
				client->setFalloff( ((Span_GetBarHeights_v1_0*)Data)->falloff );
			}
//			dsyslog("[span]: %s (%s()(%s,%d))", ((Span_GetBarHeights_v1_0*)Data)->name, __func__, __FILE__, __LINE__ );
			sa->getAllData(	(int)(((Span_GetBarHeights_v1_0*)Data)->bands),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->barHeights),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->barHeightsLeftChannel),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->barHeightsRightChannel),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->volumeLeftChannel),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->volumeRightChannel),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->volumeBothChannels),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->barPeaksBothChannels),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->barPeaksLeftChannel),
							(int*)(((Span_GetBarHeights_v1_0*)Data)->barPeaksRightChannel),
							client
					);
			
			return true;
		}
	}
	return false;
}

VDRPLUGINCREATOR(cPluginSpan); // Don't touch this!

#endif //___SPAN_C
