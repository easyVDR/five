/*
 * search.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: search.h,v 1.2 2006/06/18 13:59:36 schmitzj Exp $
 *
 */

#ifndef SEARCH_H
#define SEARCH_H

#include <vdr/plugin.h>
#include <time.h>
#include "gfxtools.h"
#include "config.h"

class EventItem : public cOsdItem
{
    const class cEvent *myev;

public:
    EventItem(const class cEvent *ev);

    const class cEvent *Event(void)
    {
        return myev;
    };
};

// -----------------------------------------------------------------------------

class cSearchMenu : public cOsdMenu
{
    cSchedulesLock _schedulesLock;

    const class cEvent *myev;

    int search(const cSchedule *s,const class cEvent *ev);

public:
    cSearchMenu(const class cEvent *EventInfo);
    const class cEvent *currentSelected(void);

    void searchIn(const cSchedule** schedArray,int schedArrayNum);
};

#endif
