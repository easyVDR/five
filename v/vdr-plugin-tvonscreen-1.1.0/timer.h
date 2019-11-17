/*
 * timer.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: timer.h,v 1.4 2006/06/18 13:59:36 schmitzj Exp $
 *
 */

#ifndef TIMER_H
#define TIMER_H

#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <time.h>
#include "gfxtools.h"
#include "config.h"

class tvOcMenuEvent : public cMenuEvent
{
public:
    tvOcMenuEvent(const class cEvent *EventInfo);
    void printMsg(const char *);
    void helpLine(bool full=true);
};


#endif
