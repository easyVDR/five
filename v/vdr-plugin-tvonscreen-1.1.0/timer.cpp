/*
 * timer.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: timer.c,v 1.3 2006/06/18 13:59:36 schmitzj Exp $
 *
 */

#include "magazine.h"

tvOcMenuEvent::tvOcMenuEvent(const class cEvent *EventInfo) : cMenuEvent(EventInfo,false)
{
    helpLine();
}

void tvOcMenuEvent::printMsg(const char *s)
{
    SetStatus(s);
}
void tvOcMenuEvent::helpLine(bool full)
{
    SetHelp(tr("Record"), tr("Search"), full?tr("AutoTimer"):NULL, NULL);
}
