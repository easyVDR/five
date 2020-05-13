/*
 * epg_item.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <string>
#include <math.h>
#include <vdr/menu.h>
#include <locale.h>
#include <langinfo.h>
#include "epg_item.h"

#define HOURS(x) ((x)/100)
#define MINUTES(x) ((x)%100)

#define SHORTTEXT(EVENT) \
    ((EVENT) && !isempty((EVENT)->ShortText())) ? " ~ ":"", \
((EVENT) && !isempty((EVENT)->ShortText())) ? (EVENT)->ShortText():""

//extern int exitToMainMenu;

// --- Icons ------------------------------------------------------------------
bool Icons::IsUTF8 = false;

void Icons::InitCharSet()
{
    // Taken from VDR's vdr.c
    char *CodeSet = NULL;
    if(setlocale(LC_CTYPE, ""))
        CodeSet = nl_langinfo(CODESET);
    else
    {
        char *LangEnv = getenv("LANG"); // last resort in case locale stuff isn't installed
        if(LangEnv)
        {
            CodeSet = strchr(LangEnv,'.');
            if( CodeSet )
                CodeSet++; // skip the dot
        }
    }

    if( CodeSet && strcasestr(CodeSet,"UTF-8") != 0 )
        IsUTF8 = true;
}


// --- cMenuMyScheduleItem ------------------------------------------------------
    cMenuMyScheduleItem::cMenuMyScheduleItem(const cEvent *Event, cZapHistoryChannel *Channel, int Progress)
:cZapHistoryOsdItem(Channel)
{
    event = Event;
    progress = Progress;
    timerMatch = tmNone;
    Update(true);
}

bool cMenuMyScheduleItem::Update(bool Force)
{
    bool result = false;
    const cChannel* channel = zapChannel->GetChannel();

    eTimerMatch OldTimerMatch = timerMatch;
    cTimer* hasMatch = NULL;

    if (event)
        hasMatch = Timers.GetMatch(event, &timerMatch);

    if (Force || timerMatch != OldTimerMatch) 
    {
        char szChannelpart[20] = "";
        if (channel)
	        snprintf(szChannelpart, 20, "%s\t", channel->Name() );

        char szProgressPart[50] = "";
        if (progress > 0 && channel && event )
        {
            strcpy(szProgressPart, "\t");
            
            if( ZapHistorySetup.ProgressView == 1 ) { // VDRSymbols
                std::string ProgressBar;
                ProgressBar += Icons::ProgressStart();
                int frac = (int)roundf( (float)(time(NULL) - event->StartTime()) / (float)(event->Duration()) * 10.0 );
                frac = min(10,max(0, frac));
                for(int i=0;i < 10;i++)
                {
                    if(i < frac)
                        ProgressBar += Icons::ProgressFilled();
                    else
                        ProgressBar += Icons::ProgressEmpty();
                }
                ProgressBar += Icons::ProgressEnd();
                sprintf(szProgressPart, "%s\t", ProgressBar.c_str());
            } else if( ZapHistorySetup.ProgressView == 0 ) {
                char szProgress[9] = "";
                int frac = (int)roundf( (float)(time(NULL) - event->StartTime()) / (float)(event->Duration()) * 8.0 );
                frac = min(8,max(0, frac));

                for(int i = 0; i < frac; i++)
                    szProgress[i] = (progress == 1 ? '|' : 127);
                szProgress[frac]=0;
                sprintf(szProgressPart, "%c%-8s%c\t", progress==1?'[':128, szProgress, progress==1?']':129);
            } else {
                float frac = (int)roundf( (float)(time(NULL) - event->StartTime()) / (float)(event->Duration()) * 100.0 );
                sprintf(szProgressPart, "%3.0f%%", frac);
            }
        }

        char t = event && hasMatch ? (timerMatch == tmFull) ? 'T' : 't' : ' ';
        char v = event && event->Vps() && (event->Vps() - event->StartTime()) ? 'V' : ' ';
        char r = event && event->IsRunning() ? '*' : ' ';

        char szEventDescr[100] = "";
        snprintf(szEventDescr, 100, "%s%s%s", 
	            event?event->Title():tr("no info"), 
	            SHORTTEXT(event) );

        char *buffer = NULL;
        if (channel) // menu "What's on"
	        asprintf(&buffer, "%s%s\t%s %c%c%c \t%s", 
		            szChannelpart,
		            event?*(event->GetTimeString() ):"", 
		            szProgressPart,
		            t, v, r, 
		            szEventDescr);

        SetText(buffer, false);
        return true;
    }
    return result;
}
