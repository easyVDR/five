/*
 * search.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: search.c,v 1.4 2006/06/18 13:59:36 schmitzj Exp $
 *
 */

#include "search.h"
#include "magazine.h"

EventItem::EventItem(const class cEvent *ev)
{
    char buf[200];
    const char *txt,*chan;
    cString time1,time2,date;
    cChannel *channel;

    myev=ev;
    channel = Channels.GetByChannelID(ev->ChannelID(), true);
    time1=ev->GetTimeString();
    time2=ev->GetEndTimeString();
    date=ev->GetDateString();
    chan=channel->ShortName(true);
    txt=ev->Title();

    snprintf(buf,sizeof(buf)-1,"%.6s %s - %s %s/%s",(const char *)date,(const char *)time1,(const char *)time2,chan,txt);
    SetText(buf);
}

// -----------------------------------------------------------------------------

cSearchMenu::cSearchMenu(const class cEvent *ev) : cOsdMenu(tr("Search"))
{
    myev=ev;

    const char *title;
    title=ev->Title();

    char txt[200];
    snprintf(txt,sizeof(txt),"%s '%s'...",tr("Search for"),title);
    SetTitle(txt);

    SetStatus(tr("Searching..."));
    Display();
    Skins.Message(mtInfo, tr("Searching..."));
    const cSchedules* Schedules = cSchedules::Schedules(_schedulesLock);

    const cSchedule *s;
    s=Schedules->GetSchedule(ev->ChannelID());

    int cc=search(s,ev);

    snprintf(txt,sizeof(txt),"%s '%s':",tr("Search for"),title);
    SetTitle(txt);
    if (cc>0)
    {
        SetStatus("");
        SetHelp(tr("Record"), tr("Details"), tr("in all"), tr("Back"));
    }
    else
    {
        SetStatus(tr("Nothing found!"));
        SetHelp(NULL, NULL, tr("in all"), tr("Back"));
    }
    Display();
}
const cEvent *cev=NULL;

int cSearchMenu::search(const cSchedule *s,const class cEvent *ev)
{
    const char *title;
    title=ev->Title();

    int cc=0;
    cev=s->GetPresentEvent();
    while (cev)
    {
        if (cev!=ev || tvonscreenCfg.showsearchinitiator)
        {
            const char *ctitle;
            ctitle=cev->Title();
            if (ctitle && title && strstr(ctitle,title)) // || strstr(title,ctitle))
            {
                Add(new EventItem(cev));
                cc++;
            }
        }
        cev=magazine::getNext(s,cev);
    }
    return cc;
}
void cSearchMenu::searchIn(const cSchedule** schedArray,int schedArrayNum)
{
    cChannel* channel;
    const char *title;
    title=myev->Title();

    char txt[200];
    snprintf(txt,sizeof(txt),"%s '%s'...",tr("Search for"),title);
    SetTitle(txt);
    Clear();

    SetStatus(tr("Searching..."));
    Display();
    Skins.Message(mtInfo, tr("Searching..."));
    int cc=0;

    for (int i=0;i<schedArrayNum;i++)
    {
        channel = Channels.GetByChannelID(schedArray[i]->ChannelID(), true);
        snprintf(txt,sizeof(txt),"%s /%-20.20s",tr("Searching..."),channel->ShortName(true));
        Skins.Message(mtInfo, txt);
        cc+=search(schedArray[i],myev);
    }

    snprintf(txt,sizeof(txt),"%s '%s':",tr("Search for"),title);
    SetTitle(txt);
    if (cc>0)
    {
        SetStatus("");
        SetHelp(tr("Record"), tr("Details"), NULL, tr("Back"));
    }
    else
    {
        SetStatus(tr("Nothing found!"));
        SetHelp(NULL, NULL, NULL, tr("Back"));
    }
    Display();
}


const class cEvent *cSearchMenu::currentSelected(void)
{
    EventItem *item = (EventItem *)Get(Current());
    return item ? item->Event() : NULL;
}
