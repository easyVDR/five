/*
 * menu.c: SpAn's menu
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

#ifndef _MENU__C
#define _MENU__C

#include "menu-item.h"
#include "menu.h"

// cSpanOsdMenu ##################################################################
cSpanOsdMenu::cSpanOsdMenu(): cOsdMenu(tr("Menu.Span$Span: Service providers"))
{
	Clear();
	SetCols(3,30);

	cPlugin *p = NULL;
	int i;
	char buff[128];

	bool isActive;
	bool isRunning;
	
	Add(new cSpanOsdItem(tr("Menu.Span$Providers:"), HEADING));

	i = 0;
	while ( NULL != ( p = cPluginManager::GetPlugin(i++) ) )
	{
		Span_Provider_Check_1_0	spanIsProvider;
		isActive 	= false;
		isRunning 	= false;
		spanIsProvider.isActive = &isActive;
		spanIsProvider.isRunning = &isRunning;
		
		if ( p->Service( SPAN_PROVIDER_CHECK_ID, &spanIsProvider ) )
		{
			sprintf(buff, "%s", p->MainMenuEntry()?p->MainMenuEntry():p->Description());
			sprintf(buff, "%s (%s/%s)", buff, isActive?tr("activated"):tr("deactivated"), isRunning?tr("providing"):tr("idle"));
//			dsyslog( "[span]: provider detected: %s", buff );
			Add(new cSpanOsdItem(buff, PROVIDER, isActive, isRunning ));
		}
	}
	Add(new cOsdItem( "", osUnknown, false));
	Add(new cSpanOsdItem(tr("Menu.Span$Visualizations:"), HEADING));
	i = 0;
	while ( NULL != ( p = cPluginManager::GetPlugin(i++) ) )
	{
		Span_Client_Check_1_0	spanIsClient;
		isActive 	= false;
		isRunning 	= false;
		spanIsClient.isActive = &isActive;
		spanIsClient.isRunning = &isRunning;
		
		if ( p->Service( SPAN_CLIENT_CHECK_ID, &spanIsClient ) )
		{
			sprintf(buff, "%s", p->MainMenuEntry()?p->MainMenuEntry():p->Description());
			sprintf(buff, "%s (%s/%s)", buff, isActive?tr("activated"):tr("deactivated"), isRunning?tr("visualizing"):tr("idle"));
//			dsyslog( "[span]: client detected: %s", buff );
			Add(new cSpanOsdItem(buff, CLIENT, isActive, isRunning ));
		}
	}
}
#endif //_MENU__C
