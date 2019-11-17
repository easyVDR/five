#include "viewelementsdisplaymenu.h"
#include "../config.h"
#include <vdr/videodir.h>
#include "../extensions/helpers.h"
#include "../extensions/globaltimers.h"
#include "../services/epgtimer.h"
#include <sys/sysinfo.h>
#include <fstream>
#include <iostream>

/******************************************************************
* cVeDmHeader
******************************************************************/
cVeDmHeader::cVeDmHeader(void) {
    changed = false;
    title = NULL;
    channelName = NULL;
    channelNumber = 0;
    channelId = NULL;
    epgSearchFav = false;
}

cVeDmHeader::~cVeDmHeader(void) {
    free(title);
    free(channelName);
    free(channelId);
}

void cVeDmHeader::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eDMHeaderST::title);
    tokenContainer->DefineStringToken("{vdrversion}", (int)eDMHeaderST::vdrversion);
    tokenContainer->DefineStringToken("{icon}", (int)eDMHeaderST::icon);
    tokenContainer->DefineStringToken("{freetime}", (int)eDMHeaderST::freetime);
    tokenContainer->DefineStringToken("{vdrusagestring}", (int)eDMHeaderST::vdrusagestring);
    tokenContainer->DefineStringToken("{channelname}", (int)eDMHeaderST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eDMHeaderST::channelid);
    tokenContainer->DefineIntToken("{hasicon}", (int)eDMHeaderIT::hasicon);
    tokenContainer->DefineIntToken("{freepercent}", (int)eDMHeaderIT::freepercent);
    tokenContainer->DefineIntToken("{usedpercent}", (int)eDMHeaderIT::usedpercent);
    tokenContainer->DefineIntToken("{freegb}", (int)eDMHeaderIT::freegb);
    tokenContainer->DefineIntToken("{discalert}", (int)eDMHeaderIT::discalert);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eDMHeaderIT::channelnumber);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eDMHeaderIT::channellogoexists);
    tokenContainer->DefineIntToken("{whatson}", (int)eDMHeaderIT::whatson);
    tokenContainer->DefineIntToken("{whatsonnow}", (int)eDMHeaderIT::whatsonnow);
    tokenContainer->DefineIntToken("{whatsonnext}", (int)eDMHeaderIT::whatsonnext);
    tokenContainer->DefineIntToken("{whatsonfavorites}", (int)eDMHeaderIT::whatsonfavorites);
    InheritTokenContainer();
}

void cVeDmHeader::SetTitle(const char *title) { 
    if (this->title && !strcmp(this->title, title))
        return;
    free(this->title);
    this->title = strdup(title); 
    changed = true;
}

void cVeDmHeader::SetChannel(const cChannel *channel) {
    ClearChannel();
    if (!channel)
        return;
    channelNumber = channel->Number();
    channelName = strdup(channel->Name());
    channelId = strdup(*(channel->GetChannelID().ToString()));
}

void cVeDmHeader::ClearChannel(void) {
    channelNumber = 0;
    free(channelName);
    channelName = NULL;
    free(channelId);
    channelId = NULL;
}

void cVeDmHeader::Set(eMenuCategory menuCat) {
    if (!changed)
        return;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDMHeaderST::title, title);
    tokenContainer->AddStringToken((int)eDMHeaderST::vdrversion, VDRVERSION);

    tokenContainer->AddIntToken((int)eDMHeaderIT::whatson, (menuCat == mcSchedule && !epgSearchFav) ? true : false);
    tokenContainer->AddIntToken((int)eDMHeaderIT::whatsonnow, menuCat == mcScheduleNow ? true : false);
    tokenContainer->AddIntToken((int)eDMHeaderIT::whatsonnext, menuCat == mcScheduleNext ? true : false);
    tokenContainer->AddIntToken((int)eDMHeaderIT::whatsonfavorites, epgSearchFav);

    //check for standard menu entries
    bool hasIcon = false;

    if (title) {
        string icon = imgCache->GetIconName(title, menuCat);
        if (imgCache->MenuIconExists(icon))
            hasIcon = true;
        tokenContainer->AddStringToken((int)eDMHeaderST::icon, icon.c_str());
        tokenContainer->AddIntToken((int)eDMHeaderIT::hasicon, hasIcon);
    }

    //Disc Usage
    tokenContainer->AddStringToken((int)eDMHeaderST::vdrusagestring, *cVideoDiskUsage::String());
    tokenContainer->AddStringToken((int)eDMHeaderST::freetime, *cString::sprintf("%02d:%02d", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60));
    int discUsage = cVideoDiskUsage::UsedPercent();
    bool discAlert = (discUsage > 95) ? true : false;
    int freeGB = cVideoDiskUsage::FreeMB() / 1024;
    tokenContainer->AddIntToken((int)eDMHeaderIT::usedpercent, discUsage);
    tokenContainer->AddIntToken((int)eDMHeaderIT::freepercent, 100-discUsage);
    tokenContainer->AddIntToken((int)eDMHeaderIT::discalert, discAlert);
    tokenContainer->AddIntToken((int)eDMHeaderIT::freegb, freeGB);

    if (channelName && channelId) {
        tokenContainer->AddStringToken((int)eDMHeaderST::channelname, channelName);
        tokenContainer->AddStringToken((int)eDMHeaderST::channelid, channelId);
        tokenContainer->AddIntToken((int)eDMHeaderIT::channelnumber, channelNumber);
        tokenContainer->AddIntToken((int)eDMHeaderIT::channellogoexists, imgCache->LogoExists(channelId));
    }
    changed = false;
    SetDirty();
}

/******************************************************************
* cVeDmSortmode
******************************************************************/
cVeDmSortmode::cVeDmSortmode(void) {
    sortMode = msmUnknown;
    lastSortMode = msmUnknown;
}

cVeDmSortmode::~cVeDmSortmode(void) {
}

void cVeDmSortmode::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{sortnumber}", (int)eDMSortmodeIT::sortnumber);
    tokenContainer->DefineIntToken("{sortname}", (int)eDMSortmodeIT::sortname);
    tokenContainer->DefineIntToken("{sorttime}", (int)eDMSortmodeIT::sorttime);
    tokenContainer->DefineIntToken("{sortprovider}", (int)eDMSortmodeIT::sortprovider);
    InheritTokenContainer();
}

void cVeDmSortmode::Set(eMenuSortMode sortMode) {
    this->sortMode = sortMode;
}

bool cVeDmSortmode::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    if (sortMode == lastSortMode) {
        return false;
    }
    lastSortMode = sortMode;

    bool sortNumber   = (sortMode == msmNumber)   ? true : false;
    bool sortName     = (sortMode == msmName)     ? true : false;
    bool sortTime     = (sortMode == msmTime)     ? true : false;
    bool sortProvider = (sortMode == msmProvider) ? true : false;
    
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMSortmodeIT::sortnumber, sortNumber);
    tokenContainer->AddIntToken((int)eDMSortmodeIT::sortname, sortName);
    tokenContainer->AddIntToken((int)eDMSortmodeIT::sorttime, sortTime);
    tokenContainer->AddIntToken((int)eDMSortmodeIT::sortprovider, sortProvider);
    SetDirty();
    return true;
}
/******************************************************************
* cVeDmColorbuttons
******************************************************************/
cVeDmColorbuttons::cVeDmColorbuttons(void) {
    changed = false;
    red = NULL;
    green = NULL;
    yellow = NULL;
    blue = NULL;
}

cVeDmColorbuttons::~cVeDmColorbuttons(void) {
    free(red);
    free(yellow);
    free(green);
    free(blue);
}

void cVeDmColorbuttons::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{red}", (int)eDMColorbuttonsST::red);
    tokenContainer->DefineStringToken("{green}", (int)eDMColorbuttonsST::green);
    tokenContainer->DefineStringToken("{yellow}", (int)eDMColorbuttonsST::yellow);
    tokenContainer->DefineStringToken("{blue}", (int)eDMColorbuttonsST::blue);
    tokenContainer->DefineIntToken("{red1}", (int)eDMColorbuttonsIT::red1);
    tokenContainer->DefineIntToken("{red2}", (int)eDMColorbuttonsIT::red2);
    tokenContainer->DefineIntToken("{red3}", (int)eDMColorbuttonsIT::red3);
    tokenContainer->DefineIntToken("{red4}", (int)eDMColorbuttonsIT::red4);
    tokenContainer->DefineIntToken("{green1}", (int)eDMColorbuttonsIT::green1);
    tokenContainer->DefineIntToken("{green2}", (int)eDMColorbuttonsIT::green2);
    tokenContainer->DefineIntToken("{green3}", (int)eDMColorbuttonsIT::green3);
    tokenContainer->DefineIntToken("{green4}", (int)eDMColorbuttonsIT::green4);
    tokenContainer->DefineIntToken("{yellow1}", (int)eDMColorbuttonsIT::yellow1);
    tokenContainer->DefineIntToken("{yellow2}", (int)eDMColorbuttonsIT::yellow2);
    tokenContainer->DefineIntToken("{yellow3}", (int)eDMColorbuttonsIT::yellow3);
    tokenContainer->DefineIntToken("{yellow4}", (int)eDMColorbuttonsIT::yellow4);
    tokenContainer->DefineIntToken("{blue1}", (int)eDMColorbuttonsIT::blue1);
    tokenContainer->DefineIntToken("{blue2}", (int)eDMColorbuttonsIT::blue2);
    tokenContainer->DefineIntToken("{blue3}", (int)eDMColorbuttonsIT::blue3);
    tokenContainer->DefineIntToken("{blue4}", (int)eDMColorbuttonsIT::blue4);
    InheritTokenContainer();
}

void cVeDmColorbuttons::SetButtons(const char *red, const char *green, const char *yellow, const char *blue) {
    free(this->red);
    free(this->yellow);
    free(this->green);
    free(this->blue);
    this->red = NULL;
    this->green = NULL;
    this->yellow = NULL;
    this->blue = NULL;
    if (red) this->red = strdup(red);
    if (green) this->green = strdup(green);
    if (yellow) this->yellow = strdup(yellow);
    if (blue) this->blue = strdup(blue);
    changed = true;
}

bool cVeDmColorbuttons::Parse(bool forced) {
    if (!cViewElement::Parse(forced) || !changed)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDMColorbuttonsST::red, red);
    tokenContainer->AddStringToken((int)eDMColorbuttonsST::green, green);
    tokenContainer->AddStringToken((int)eDMColorbuttonsST::yellow, yellow);
    tokenContainer->AddStringToken((int)eDMColorbuttonsST::blue, blue);

    int colorKeys[4] = { Setup.ColorKey0, Setup.ColorKey1, Setup.ColorKey2, Setup.ColorKey3 };

    for (int button = 1; button < 5; button++) {
        bool isRed = false;
        bool isGreen = false;
        bool isYellow = false;
        bool isBlue = false;
        switch (colorKeys[button-1]) {
            case 0:
                isRed = true;
                break;
            case 1:
                isGreen = true;
                break;
            case 2:
                isYellow = true;
                break;
            case 3:
                isBlue = true;
                break;
            default:
                break;
        }
        int tokRed = button - 1;
        int tokGreen = 3 + button;
        int tokYellow = 7 + button;
        int tokBlue = 11 + button;
        tokenContainer->AddIntToken(tokRed, isRed);
        tokenContainer->AddIntToken(tokGreen, isGreen);
        tokenContainer->AddIntToken(tokYellow, isYellow);
        tokenContainer->AddIntToken(tokBlue, isBlue);
    }
    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDmScrollbar
******************************************************************/
cVeDmScrollbar::cVeDmScrollbar(void) {
}

cVeDmScrollbar::~cVeDmScrollbar(void) {
}

void cVeDmScrollbar::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{height}", (int)eDMScrollbarIT::height);
    tokenContainer->DefineIntToken("{offset}", (int)eDMScrollbarIT::offset);
    tokenContainer->DefineIntToken("{hasprev}", (int)eDMScrollbarIT::hasprev);
    tokenContainer->DefineIntToken("{hasnext}", (int)eDMScrollbarIT::hasnext);
    InheritTokenContainer();
}

void cVeDmScrollbar::SetList(int numDisplayed, int offset, int numMax) {
    if (numDisplayed < 1)
        return;
    
    int barHeight = 0;
    if (numDisplayed < numMax)
        barHeight = 1000;
    else
        barHeight = (double)numMax * 1000 / (double)numDisplayed;
    
    int barOffset = (double)offset * 1000 / (double)numDisplayed;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMScrollbarIT::height, barHeight);
    tokenContainer->AddIntToken((int)eDMScrollbarIT::offset, barOffset);
    tokenContainer->AddIntToken((int)eDMScrollbarIT::hasprev, (offset == 0) ? 0 : 1);
    tokenContainer->AddIntToken((int)eDMScrollbarIT::hasnext, (offset + numMax >= numDisplayed) ? 0 : 1);
    SetDirty();
}

void cVeDmScrollbar::SetDetail(int height, int offset, bool end) {
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMScrollbarIT::height, height);
    tokenContainer->AddIntToken((int)eDMScrollbarIT::offset, offset);
    tokenContainer->AddIntToken((int)eDMScrollbarIT::hasprev, (offset == 0) ? 0 : 1);
    tokenContainer->AddIntToken((int)eDMScrollbarIT::hasnext, (end) ? 0 : 1);
    SetDirty();
}

/******************************************************************
* cVeDmTimers
******************************************************************/
cVeDmTimers::cVeDmTimers(void) {
    timerIndex = -1;
}

cVeDmTimers::~cVeDmTimers(void) {
}

void cVeDmTimers::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineLoopToken("{timers[title]}", (int)eDMTimersLT::title);
    tokenContainer->DefineLoopToken("{timers[datetime]}", (int)eDMTimersLT::datetime);
    tokenContainer->DefineLoopToken("{timers[recording]}", (int)eDMTimersLT::recording);
    tokenContainer->DefineLoopToken("{timers[channelname]}", (int)eDMTimersLT::channelname);
    tokenContainer->DefineLoopToken("{timers[channelnumber]}", (int)eDMTimersLT::channelnumber);
    tokenContainer->DefineLoopToken("{timers[channelid]}", (int)eDMTimersLT::channelid);
    tokenContainer->DefineLoopToken("{timers[channellogoexists]}", (int)eDMTimersLT::channellogoexists);
    tokenContainer->DefineLoopToken("{timers[isremotetimer]}", (int)eDMTimersLT::isremotetimer);
    tokenContainer->DefineLoopToken("{timers[remotehost]}", (int)eDMTimersLT::remotehost);
    tokenContainer->DefineIntToken("{numtimers}", (int)eDMTimersIT::numtimers);
    tokenContainer->DefineIntToken("{numtimerconflicts}", (int)eDMTimersIT::numtimerconflicts);
    tokenContainer->DefineIntToken("{timer1exists}", (int)eDMTimersIT::timer1exists);
    tokenContainer->DefineIntToken("{timer2exists}", (int)eDMTimersIT::timer2exists);
    tokenContainer->DefineIntToken("{timer3exists}", (int)eDMTimersIT::timer3exists);
    tokenContainer->DefineIntToken("{timer4exists}", (int)eDMTimersIT::timer4exists);
    tokenContainer->DefineIntToken("{timer5exists}", (int)eDMTimersIT::timer5exists);
    tokenContainer->DefineIntToken("{timer6exists}", (int)eDMTimersIT::timer6exists);
    tokenContainer->DefineIntToken("{timer7exists}", (int)eDMTimersIT::timer7exists);
    tokenContainer->DefineIntToken("{timer8exists}", (int)eDMTimersIT::timer8exists);
    tokenContainer->DefineIntToken("{timer9exists}", (int)eDMTimersIT::timer9exists);
    tokenContainer->DefineIntToken("{timer10exists}", (int)eDMTimersIT::timer10exists);
    tokenContainer->DefineIntToken("{timer11exists}", (int)eDMTimersIT::timer11exists);
    tokenContainer->DefineIntToken("{timer12exists}", (int)eDMTimersIT::timer12exists);
    tokenContainer->DefineIntToken("{timer13exists}", (int)eDMTimersIT::timer13exists);
    tokenContainer->DefineIntToken("{timer14exists}", (int)eDMTimersIT::timer14exists);
    tokenContainer->DefineIntToken("{timer15exists}", (int)eDMTimersIT::timer15exists);
    timerIndex = tokenContainer->LoopIndex("timers");
    InheritTokenContainer();
}

bool cVeDmTimers::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    tokenContainer->Clear();
    
    cGlobalTimers globalTimers;
    globalTimers.LoadTimers();
    globalTimers.SortTimers();
    globalTimers.MarkLocalTimers();

    int numTimers = globalTimers.Size();
    tokenContainer->AddIntToken((int)eDMTimersIT::numtimers, numTimers);
    tokenContainer->AddIntToken((int)eDMTimersIT::numtimerconflicts, globalTimers.NumTimerConfilicts());
    for (int i=0; i<15; i++) {
        if (i < numTimers) {
            tokenContainer->AddIntToken(i+2, true);
        } else {
            tokenContainer->AddIntToken(i+2, false);
        }
    }

    vector<int> timerInfo;
    timerInfo.push_back(numTimers);
    tokenContainer->CreateLoopTokenContainer(&timerInfo);

    for (int i = 0; i < numTimers; i++) {
        if (i >=15)
            break;
        const cTimer *Timer = globalTimers[i];
        tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::isremotetimer, globalTimers.IsRemoteTimer(i) ? "1" : "0");
        tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::remotehost, globalTimers.RemoteHost(i));
        const cEvent *event = Timer->Event();
        if (event) {
            tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::title, event->Title());
        } else {
            const char *File = Setup.FoldersInTimerMenu ? NULL : strrchr(Timer->File(), FOLDERDELIMCHAR);
            if (File && strcmp(File + 1, TIMERMACRO_TITLE) && strcmp(File + 1, TIMERMACRO_EPISODE))
                File++;
            else
                File = Timer->File();
            tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::title, File);
        }
        const cChannel *channel = Timer->Channel();
        if (channel) {
            tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::channelname, channel->Name());
            tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::channelnumber, *cString::sprintf("%d", channel->Number()));
            cString channelID = channel->GetChannelID().ToString();
            tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::channelid, *channelID);
            tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::channellogoexists, imgCache->LogoExists(*channelID) ? "1" : "0");
        }
        bool isRecording = Timer->Recording();
        if (!isRecording) {
            if (cEpgTimer_Interface_V1* epgTimer = dynamic_cast<cEpgTimer_Interface_V1*>((cTimer*)Timer)) {
                if (epgTimer->State() == 'R')
                    isRecording = true;
            }
        }
        tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::recording, isRecording ? "1" : "0");

        cString timerDate("");
        if (isRecording) {
            timerDate = cString::sprintf("-%s", *TimeString(Timer->StopTime()));
        } else {
            time_t Now = time(NULL);
            cString Today = WeekDayName(Now);
            cString Time = TimeString(Timer->StartTime());
            cString Day = WeekDayName(Timer->StartTime());
            if (Timer->StartTime() > Now + 6 * SECSINDAY) {
                time_t ttm = Timer->StartTime();
                struct tm * timerTime = localtime(&ttm);
                timerDate = cString::sprintf("%02d.%02d %s", timerTime->tm_mday, timerTime->tm_mon + 1, *Time);
            } else if (strcmp(Day, Today) != 0)
                timerDate = cString::sprintf("%s %s", *Day, *Time);
            else
                timerDate = Time;
            if (Timer->Flags() & tfVps)
                timerDate = cString::sprintf("VPS %s", *timerDate);
        }
        tokenContainer->AddLoopToken(timerIndex, i, (int)eDMTimersLT::datetime, *timerDate);
    }

    SetDirty();
    return true;
}

/******************************************************************
* cVeDmCurrentschedule
******************************************************************/
cVeDmCurrentschedule::cVeDmCurrentschedule(void) {
    rec = NULL;
}

cVeDmCurrentschedule::~cVeDmCurrentschedule(void) {
}

void cVeDmCurrentschedule::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eDMCurrentscheduleST::title);
    tokenContainer->DefineStringToken("{subtitle}", (int)eDMCurrentscheduleST::subtitle);
    tokenContainer->DefineStringToken("{start}", (int)eDMCurrentscheduleST::start);
    tokenContainer->DefineStringToken("{stop}", (int)eDMCurrentscheduleST::stop);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eDMCurrentscheduleST::durationminutes);
    tokenContainer->DefineStringToken("{mediapath}", (int)eScraperPosterBannerST::mediapath);
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperPosterBannerST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eScraperPosterBannerST::bannerpath);
    tokenContainer->DefineIntToken("{islivetv}", (int)eDMCurrentscheduleIT::islivetv);
    tokenContainer->DefineIntToken("{duration}", (int)eDMCurrentscheduleIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eDMCurrentscheduleIT::durationhours);
    tokenContainer->DefineIntToken("{minutes}", (int)eDMCurrentscheduleIT::minutes);
    tokenContainer->DefineIntToken("{elapsed}", (int)eDMCurrentscheduleIT::elapsed);
    tokenContainer->DefineIntToken("{remaining}", (int)eDMCurrentscheduleIT::remaining);
    tokenContainer->DefineIntToken("{mediawidth}", (int)eScraperPosterBannerIT::mediawidth);
    tokenContainer->DefineIntToken("{mediaheight}", (int)eScraperPosterBannerIT::mediaheight);
    tokenContainer->DefineIntToken("{isbanner}", (int)eScraperPosterBannerIT::isbanner);
    tokenContainer->DefineIntToken("{hasposter}", (int)eScraperPosterBannerIT::hasposter);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperPosterBannerIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperPosterBannerIT::posterheight);
    tokenContainer->DefineIntToken("{hasbanner}", (int)eScraperPosterBannerIT::hasbanner);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eScraperPosterBannerIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eScraperPosterBannerIT::bannerheight);
    InheritTokenContainer();
}

void cVeDmCurrentschedule::SetRecording(const char *currentRec) { 
    rec = currentRec; 
}

bool cVeDmCurrentschedule::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    cDevice *device = cDevice::PrimaryDevice();
    const cChannel *channel = NULL;
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
#else
    cChannels* channels = &Channels;
#endif

    if (!device->Replaying() || device->Transferring()) {
        channel = channels->GetByNumber(device->CurrentChannel());
    }
    if (channel) {
        ParseFromChannel(channel);
    } else {
        if (!rec)
            return true;
        const cRecording *recording = new cRecording(rec);
        if (recording) {
            ParseFromRecording(recording);
            delete recording;
        }
    }
    SetDirty();
    return true;
}

void cVeDmCurrentschedule::ParseFromChannel(const cChannel *channel) {
    const cEvent *event = NULL;

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_SCHEDULES_READ;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif

    if (schedules)
        if (const cSchedule *Schedule = schedules->GetSchedule(channel))
            event = Schedule->GetPresentEvent();
    if (!event)
        return;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::islivetv, 1);
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::title, event->Title());
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::subtitle, event->ShortText());
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::start, *event->GetTimeString());
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::stop, *event->GetEndTimeString());
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::duration, event->Duration() / 60);
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::durationhours, event->Duration() / 3600);
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::durationminutes, *cString::sprintf("%.2d", (event->Duration() / 60)%60));
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::elapsed, (int)round((time(NULL) - event->StartTime())/60));
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::remaining, (int)round((event->EndTime() - time(NULL))/60));
    SetPosterBanner(tokenContainer, event, NULL);
}

void cVeDmCurrentschedule::ParseFromRecording(const cRecording *recording) {
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::islivetv, 0);
    string recFullName = recording->Name() ? recording->Name() : "";
    string recName = "";
    string recFolder = "";
    RecName(recFullName, recName, recFolder);
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::title, recName.c_str());
    const cRecordingInfo *info = recording->Info();
    if (info)
        tokenContainer->AddStringToken((int)eDMCurrentscheduleST::subtitle, info->ShortText());
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::duration, recording->LengthInSeconds() / 60);
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::durationhours, recording->LengthInSeconds() / 3600);
    tokenContainer->AddStringToken((int)eDMCurrentscheduleST::durationminutes, *cString::sprintf("%.2d", (recording->LengthInSeconds() / 60)%60));
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::elapsed, 0);
    tokenContainer->AddIntToken((int)eDMCurrentscheduleIT::remaining, 0);
    SetPosterBanner(tokenContainer, NULL, recording);
}

void cVeDmCurrentschedule::RecName(string &path, string &name, string &folder) {
    size_t delim = path.find_last_of('~');
    if (delim == string::npos) {
        name = path;
        if (name.find('%') == 0) {
            name = name.substr(1);
        }
        return;
    }
    name = path.substr(delim+1);
    if (name.find('%') == 0) {
        name = name.substr(1);
    }
    folder = path.substr(0, delim);
    size_t delim2 = folder.find_last_of('~');
    if (delim2 == string::npos) {
        return;
    }
    folder = folder.substr(delim2+1);
}
/******************************************************************
* cVeDmDiscusage
******************************************************************/
cVeDmDiscusage::cVeDmDiscusage(void) {
}

cVeDmDiscusage::~cVeDmDiscusage(void) {
}

void cVeDmDiscusage::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{freetime}", (int)eDMDiscusageST::freetime);
    tokenContainer->DefineStringToken("{vdrusagestring}", (int)eDMDiscusageST::vdrusagestring);
    tokenContainer->DefineIntToken("{freepercent}", (int)eDMDiscusageIT::freepercent);
    tokenContainer->DefineIntToken("{usedpercent}", (int)eDMDiscusageIT::usedpercent);
    tokenContainer->DefineIntToken("{freegb}", (int)eDMDiscusageIT::freegb);
    tokenContainer->DefineIntToken("{discalert}", (int)eDMDiscusageIT::discalert);
    InheritTokenContainer();
}

bool cVeDmDiscusage::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDMDiscusageST::vdrusagestring, *cVideoDiskUsage::String());
    tokenContainer->AddStringToken((int)eDMDiscusageST::freetime, *cString::sprintf("%02d:%02d", cVideoDiskUsage::FreeMinutes() / 60, cVideoDiskUsage::FreeMinutes() % 60));
    int discUsage = cVideoDiskUsage::UsedPercent();
    bool discAlert = (discUsage > 95) ? true : false;
    int freeGB = cVideoDiskUsage::FreeMB() / 1024;
    tokenContainer->AddIntToken((int)eDMDiscusageIT::usedpercent, discUsage);
    tokenContainer->AddIntToken((int)eDMDiscusageIT::freepercent, 100-discUsage);
    tokenContainer->AddIntToken((int)eDMDiscusageIT::discalert, discAlert);
    tokenContainer->AddIntToken((int)eDMDiscusageIT::freegb, freeGB);
    SetDirty();
    return true;
}

/******************************************************************
* cVeDmSystemload
******************************************************************/
cVeDmSystemload::cVeDmSystemload(void) {
    lastSystemLoad = -1.0f;
}

cVeDmSystemload::~cVeDmSystemload(void) {
}

void cVeDmSystemload::Close(void) {
    Reset();
    cViewElement::Close();
}

void cVeDmSystemload::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{load}", (int)eDMSystemloadST::load);
    tokenContainer->DefineIntToken("{loadhand}", (int)eDMSystemloadIT::loadhand);
    InheritTokenContainer();
}

bool cVeDmSystemload::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    double systemLoad;
    if (getloadavg(&systemLoad, 1) <= 0)
        return false;
    if (lastSystemLoad == systemLoad && !forced)
        return false;

    lastSystemLoad = systemLoad;

    string load = *cString::sprintf("%.2f", systemLoad);
    int loadHand = systemLoad * 1000;
    int loadHandValue = 0;
    if (loadHand > 2500)
        loadHandValue = 2525;
    else {
        int loadHandDec = loadHand - (loadHand / 100) * 100;
        if (loadHandDec <= 12)
            loadHandDec = 0;
        else if (loadHandDec <= 37)
            loadHandDec = 25;
        else if (loadHandDec <= 62)
            loadHandDec = 50;
        else if (loadHandDec <= 87)
            loadHandDec = 75;
        else
            loadHandDec = 0;
        loadHandValue = loadHand / 100 * 100 + loadHandDec;
    }
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDMSystemloadST::load, *cString::sprintf("%.2f", systemLoad));
    tokenContainer->AddIntToken((int)eDMSystemloadIT::loadhand, loadHandValue);
    SetDirty();
    return true;
}

/******************************************************************
* cVeDmSystemmemory
******************************************************************/
cVeDmSystemmemory::cVeDmSystemmemory(void) {
    lastMemUsage = -1;
}

cVeDmSystemmemory::~cVeDmSystemmemory(void) {
}

void cVeDmSystemmemory::Close(void) {
    Reset();
    cViewElement::Close();
}

void cVeDmSystemmemory::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{totalmem}", (int)eDMSystemmemoryIT::totalmem);
    tokenContainer->DefineIntToken("{usedmem}", (int)eDMSystemmemoryIT::usedmem);
    tokenContainer->DefineIntToken("{usedmempercent}", (int)eDMSystemmemoryIT::usedmempercent);
    InheritTokenContainer();
}

bool cVeDmSystemmemory::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long totalMem = memInfo.totalram;
    totalMem += memInfo.totalswap;
    totalMem *= memInfo.mem_unit;
    int totalMemMB = totalMem / 1024 / 1024;
    long long usedMem = memInfo.totalram - memInfo.freeram;
    usedMem += memInfo.totalswap - memInfo.freeswap;
    usedMem *= memInfo.mem_unit;
    int usedMemMB = usedMem / 1024 / 1024;

    if (lastMemUsage == usedMemMB) {
        return false;
    }
    lastMemUsage = usedMemMB;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMSystemmemoryIT::totalmem, totalMemMB);
    tokenContainer->AddIntToken((int)eDMSystemmemoryIT::usedmem, usedMemMB);
    if (totalMemMB > 0)
        tokenContainer->AddIntToken((int)eDMSystemmemoryIT::usedmempercent, usedMemMB * 100 / totalMemMB);
    SetDirty();
    return true;
}

/******************************************************************
* cVeDmTemperatures
******************************************************************/
cVeDmTemperatures::cVeDmTemperatures(void) {
    Reset();
}

cVeDmTemperatures::~cVeDmTemperatures(void) {
}

void cVeDmTemperatures::Close(void) {
    Reset();
    cViewElement::Close();
}

void cVeDmTemperatures::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{cputemp}", (int)eDMTemperaturesIT::cputemp);
    tokenContainer->DefineIntToken("{gputemp}", (int)eDMTemperaturesIT::gputemp);
    InheritTokenContainer();
}

bool cVeDmTemperatures::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    cString execCommand = cString::sprintf("cd \"%s/\"; \"%s/temperatures\"", SCRIPTFOLDER, SCRIPTFOLDER);
    int ok = system(*execCommand);
    if (ok) {}

    string tempCPU, tempGPU;
    int cpu, gpu;

    cString itemFilename = cString::sprintf("%s/cpu", SCRIPTOUTPUTPATH );
    ifstream file(*itemFilename, ifstream::in);
    if( file.is_open() ) {
        std::getline(file, tempCPU);
        if (tempCPU.size() > 2) {
            cpu = atoi(tempCPU.substr(0,2).c_str());
        } else
            cpu = 0;
        file.close();
    } else {
        cpu = 0;
    }

    itemFilename = cString::sprintf("%s/gpu", SCRIPTOUTPUTPATH );
    ifstream file2(*itemFilename, ifstream::in);
    if( file2.is_open() ) {
        std::getline(file2, tempGPU);
        if (tempGPU.size() > 2) {
            gpu = atoi(tempGPU.substr(0,2).c_str());
        } else
            gpu = 0;
        file2.close();
    } else {
        gpu = 0;
    }

    if ((lastCpuTemp == cpu) && (lastGpuTemp == gpu))
        return false;
    lastCpuTemp = cpu;
    lastGpuTemp = gpu;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDMTemperaturesIT::cputemp, cpu);
    tokenContainer->AddIntToken((int)eDMTemperaturesIT::gputemp, gpu);
    SetDirty();
    return true;
}
/******************************************************************
* cVeDmVdrstatistics
******************************************************************/
cVeDmVdrstatistics::cVeDmVdrstatistics(void) {
    Reset();
}

cVeDmVdrstatistics::~cVeDmVdrstatistics(void) {
}

void cVeDmVdrstatistics::Close(void) {
    Reset();
    cViewElement::Close();
}

void cVeDmVdrstatistics::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{vdrcpu}", (int)eDMVdrstatisticsST::vdrcpu);
    tokenContainer->DefineStringToken("{vdrmem}", (int)eDMVdrstatisticsST::vdrmem);
    InheritTokenContainer();
}

bool cVeDmVdrstatistics::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    cString execCommand = cString::sprintf("cd \"%s/\"; \"%s/vdrstats\"", SCRIPTFOLDER, SCRIPTFOLDER);
    int ok = system(*execCommand);
    if (ok) {}

    string vdrCPU = "";
    string vdrMEM = "";

    cString itemFilename = cString::sprintf("%s/vdrcpu", SCRIPTOUTPUTPATH );
    ifstream file(*itemFilename, ifstream::in);
    if( file.is_open() ) {
        std::getline(file, vdrCPU);
        file.close();
    }

    itemFilename = cString::sprintf("%s/vdrmem", SCRIPTOUTPUTPATH );
    ifstream file2(*itemFilename, ifstream::in);
    if( file2.is_open() ) {
        std::getline(file2, vdrMEM);
        file2.close();
    }

    if (vdrCPU.size() == 0 || vdrMEM.size() == 0)
        return false;

    if (!lastVdrCPU.compare(vdrCPU) && !lastVdrMEM.compare(vdrMEM)) {
        return false;
    }
    lastVdrCPU = vdrCPU;
    lastVdrMEM = vdrMEM;

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDMVdrstatisticsST::vdrcpu, vdrCPU.c_str());
    tokenContainer->AddStringToken((int)eDMVdrstatisticsST::vdrmem, vdrMEM.c_str());
    SetDirty();
    return true;
}
/******************************************************************
* cVeDmLastrecordings
******************************************************************/
cVeDmLastrecordings::cVeDmLastrecordings(void) {
    recIndex = -1;
}

cVeDmLastrecordings::~cVeDmLastrecordings(void) {
}

void cVeDmLastrecordings::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{numtimers}", (int)eDMLastrecordingsIT::numtimers);
    tokenContainer->DefineLoopToken("{recordings[name]}", (int)eDMLastrecordingsLT::name);
    tokenContainer->DefineLoopToken("{recordings[seriesname]}", (int)eDMLastrecordingsLT::seriesname);
    tokenContainer->DefineLoopToken("{recordings[date]}", (int)eDMLastrecordingsLT::date);
    tokenContainer->DefineLoopToken("{recordings[time]}", (int)eDMLastrecordingsLT::time);
    tokenContainer->DefineLoopToken("{recordings[duration]}", (int)eDMLastrecordingsLT::duration);
    tokenContainer->DefineLoopToken("{recordings[hasposter]}", (int)eDMLastrecordingsLT::hasposter);
    tokenContainer->DefineLoopToken("{recordings[posterpath]}", (int)eDMLastrecordingsLT::posterpath);
    tokenContainer->DefineLoopToken("{recordings[posterwidth]}", (int)eDMLastrecordingsLT::posterwidth);
    tokenContainer->DefineLoopToken("{recordings[posterheight]}", (int)eDMLastrecordingsLT::posterheight);
    recIndex = tokenContainer->LoopIndex("recordings");
    InheritTokenContainer();
}

bool cVeDmLastrecordings::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    tokenContainer->Clear();

    cGlobalTimers globalTimers;
    globalTimers.LoadTimers();
    
    //set number of timers so that it is possible to adapt this viewelement accordingly
    tokenContainer->AddIntToken((int)eDMLastrecordingsIT::numtimers, globalTimers.Size());

    list<const cRecording*> orderedRecs;

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_RECORDINGS_READ;
    const cRecordings* recordings = Recordings;
#else
    const cRecordings* recordings = &Recordings;
#endif

    for (const cRecording *recording = recordings->First(); recording; recording = recordings->Next(recording)) {
        if (orderedRecs.size() == 0) {
            orderedRecs.push_back(recording);
            continue;
        }
        bool inserted = false;
        for (list<const cRecording*>::iterator it = orderedRecs.begin(); it != orderedRecs.end(); it++) {
            const cRecording *orderedRec = *it;
            if (recording->Start() >= orderedRec->Start()) {
                orderedRecs.insert(it, recording);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            orderedRecs.push_back(recording);
        }
    }

    int MAX_RECORDINGS = 10;
    int availableRecordings = orderedRecs.size();
    int numRecordings = min(MAX_RECORDINGS, availableRecordings);
    if (!numRecordings)
        return true;

    vector<int> loopInfo;
    loopInfo.push_back(numRecordings);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);

    int i = 0;
    for (list<const cRecording*>::iterator it = orderedRecs.begin(); it != orderedRecs.end(); it++) {
        const cRecording *recording = *it;
#if APIVERSNUM >= 20101
        if (recording->IsInUse()) {
            continue;
        }
#endif
        string recName = "";
        string recSeriesName = "";
        const cRecordingInfo *recInfo = recording->Info();
        if (recInfo) {
            recName = recInfo->Title() ? recInfo->Title() : "";
            if (recInfo->Title() && recInfo->ShortText()) {
                stringstream ss;
                ss << recInfo->Title() << " - " << recInfo->ShortText();
                recSeriesName = ss.str();
            }
        } else {
            string recPath = recording->Name() ? recording->Name() : "";
            RecName(recPath, recName);
        }
        if (recName.size() == 0)
            recName = recording->Name() ? recording->Name() : "";
        cString recDuration = cString::sprintf("%d", recording->LengthInSeconds()/60);
        string posterPath = "";
        int posterWidth = 0;
        int posterHeight = 0;
        bool hasPoster = false;
        RecPoster(recording, posterWidth, posterHeight, posterPath, hasPoster);

        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::name, recName.c_str());
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::seriesname, recSeriesName.c_str());
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::date, *ShortDateString(recording->Start()));
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::time, *TimeString(recording->Start()));
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::duration, *recDuration);
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::hasposter, hasPoster ? "1" : "0");
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::posterpath, posterPath.c_str());
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::posterwidth, *cString::sprintf("%d", posterWidth));
        tokenContainer->AddLoopToken(recIndex, i, (int)eDMLastrecordingsLT::posterheight, *cString::sprintf("%d", posterHeight));

        i++;
        if (i == MAX_RECORDINGS)
            break;
    }

    SetDirty();
    return true;
}

void cVeDmLastrecordings::RecName(string &path, string &name) {
    size_t delim = path.find_last_of('~');
    if (delim == string::npos) {
        name = path;
        if (name.find('%') == 0) {
            name = name.substr(1);
        }
        return;
    }
    name = path.substr(delim+1);
    if (name.find('%') == 0) {
        name = name.substr(1);
    }
}
/******************************************************************
* cVeDmDetailheaderEpg
******************************************************************/
cVeDmDetailheaderEpg::cVeDmDetailheaderEpg(void) {
    event = NULL;
}

cVeDmDetailheaderEpg::~cVeDmDetailheaderEpg(void) {
}

void cVeDmDetailheaderEpg::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperHeaderST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eScraperHeaderST::bannerpath);
    tokenContainer->DefineStringToken("{title}", (int)eDmDetailedHeaderEpgST::title);
    tokenContainer->DefineStringToken("{shorttext}", (int)eDmDetailedHeaderEpgST::shorttext);
    tokenContainer->DefineStringToken("{start}", (int)eDmDetailedHeaderEpgST::start);
    tokenContainer->DefineStringToken("{stop}", (int)eDmDetailedHeaderEpgST::stop);
    tokenContainer->DefineStringToken("{day}", (int)eDmDetailedHeaderEpgST::day);
    tokenContainer->DefineStringToken("{date}", (int)eDmDetailedHeaderEpgST::date);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eDmDetailedHeaderEpgST::durationminutes);
    tokenContainer->DefineStringToken("{vps}", (int)eDmDetailedHeaderEpgST::vps);
    tokenContainer->DefineStringToken("{channelname}", (int)eDmDetailedHeaderEpgST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eDmDetailedHeaderEpgST::channelid);
    tokenContainer->DefineStringToken("{epgpicpath}", (int)eDmDetailedHeaderEpgST::epgpicpath);
    tokenContainer->DefineIntToken("{ismovie}", (int)eScraperHeaderIT::ismovie);
    tokenContainer->DefineIntToken("{isseries}", (int)eScraperHeaderIT::isseries);
    tokenContainer->DefineIntToken("{posteravailable}", (int)eScraperHeaderIT::posteravailable);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperHeaderIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperHeaderIT::posterheight);
    tokenContainer->DefineIntToken("{banneravailable}", (int)eScraperHeaderIT::banneravailable);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eScraperHeaderIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eScraperHeaderIT::bannerheight);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eDmDetailedHeaderEpgIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eDmDetailedHeaderEpgIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eDmDetailedHeaderEpgIT::year);
    tokenContainer->DefineIntToken("{running}", (int)eDmDetailedHeaderEpgIT::running);
    tokenContainer->DefineIntToken("{elapsed}", (int)eDmDetailedHeaderEpgIT::elapsed);
    tokenContainer->DefineIntToken("{duration}", (int)eDmDetailedHeaderEpgIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eDmDetailedHeaderEpgIT::durationhours);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eDmDetailedHeaderEpgIT::channelnumber);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eDmDetailedHeaderEpgIT::channellogoexists);
    tokenContainer->DefineIntToken("{epgpicavailable}", (int)eDmDetailedHeaderEpgIT::epgpicavailable);
    InheritTokenContainer();
}

void cVeDmDetailheaderEpg::SetEvent(const cEvent *event) {
    this->event = event;
}

bool cVeDmDetailheaderEpg::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!event)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::title, event->Title());
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::shorttext, event->ShortText());
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::start, *(event->GetTimeString()));
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::stop, *(event->GetEndTimeString()));
    time_t startTime = event->StartTime();
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::day, *WeekDayName(startTime));
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::date, *ShortDateString(startTime));
    struct tm * sStartTime = localtime(&startTime);
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::year, sStartTime->tm_year + 1900);
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::daynumeric, sStartTime->tm_mday);
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::month, sStartTime->tm_mon+1);
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
#else
    cChannels* channels = &Channels;
#endif
    const cChannel *channel = channels->GetByChannelID(event->ChannelID());
    if (channel) {
        tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::channelname, channel->Name());
        tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::channelnumber, channel->Number());
    }
    cString channelID = event->ChannelID().ToString();
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::channelid, *channelID);
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::channellogoexists, imgCache->LogoExists(*channelID));

    bool isRunning = false;
    time_t now = time(NULL);
    if ((now >= event->StartTime()) && (now <= event->EndTime()))
        isRunning = true;
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::running, isRunning);
    if (isRunning) {
        tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::elapsed, (now - event->StartTime())/60);
    } else {
        tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::elapsed, 0);
    }
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::duration, event->Duration() / 60);
    tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::durationhours, event->Duration() / 3600);
    tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::durationminutes, *cString::sprintf("%.2d", (event->Duration() / 60)%60));
    if (event->Vps())
        tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::vps, *event->GetVpsString());

    cString epgImageName = cString::sprintf("%d", event->EventID());
    bool epgPicAvailable = FileExists(*config.epgImagePath, *epgImageName, "jpg");
    if (epgPicAvailable) {
        tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::epgpicavailable, true);
        tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::epgpicpath, *cString::sprintf("%s%s.jpg", *config.epgImagePath, *epgImageName));
    } else {
        epgImageName = cString::sprintf("%d_0", event->EventID());
        epgPicAvailable = FileExists(*config.epgImagePath, *epgImageName, "jpg");
        if (epgPicAvailable) {
            tokenContainer->AddIntToken((int)eDmDetailedHeaderEpgIT::epgpicavailable, true);
            tokenContainer->AddStringToken((int)eDmDetailedHeaderEpgST::epgpicpath, *cString::sprintf("%s%s.jpg", *config.epgImagePath, *epgImageName));
        }
    }
    bool scrapInfoAvailable = LoadFullScrapInfo(event, NULL);
    if (scrapInfoAvailable) {
        SetHeaderScrapInfo(tokenContainer);
    }

    SetDirty();
    return true;
}

/******************************************************************
* cVeDmDetailheaderRec
******************************************************************/
cVeDmDetailheaderRec::cVeDmDetailheaderRec(void) {
}

cVeDmDetailheaderRec::~cVeDmDetailheaderRec(void) {
}

void cVeDmDetailheaderRec::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperHeaderST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eScraperHeaderST::bannerpath);
    tokenContainer->DefineStringToken("{name}", (int)eDmDetailedHeaderRecST::name);
    tokenContainer->DefineStringToken("{epgname}", (int)eDmDetailedHeaderRecST::epgname);
    tokenContainer->DefineStringToken("{shorttext}", (int)eDmDetailedHeaderRecST::shorttext);
    tokenContainer->DefineStringToken("{date}", (int)eDmDetailedHeaderRecST::date);
    tokenContainer->DefineStringToken("{time}", (int)eDmDetailedHeaderRecST::time);
    tokenContainer->DefineStringToken("{recimgpath}", (int)eDmDetailedHeaderRecST::recimgpath);
    tokenContainer->DefineStringToken("{recchannelname}", (int)eDmDetailedHeaderRecST::recchannelname);
    tokenContainer->DefineStringToken("{recchannelid}", (int)eDmDetailedHeaderRecST::recchannelid);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eDmDetailedHeaderRecST::durationminutes);
    tokenContainer->DefineStringToken("{durationeventminutes}", (int)eDmDetailedHeaderRecST::durationeventminutes);
    tokenContainer->DefineIntToken("{ismovie}", (int)eScraperHeaderIT::ismovie);
    tokenContainer->DefineIntToken("{isseries}", (int)eScraperHeaderIT::isseries);
    tokenContainer->DefineIntToken("{posteravailable}", (int)eScraperHeaderIT::posteravailable);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperHeaderIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperHeaderIT::posterheight);
    tokenContainer->DefineIntToken("{banneravailable}", (int)eScraperHeaderIT::banneravailable);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eScraperHeaderIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eScraperHeaderIT::bannerheight);
    tokenContainer->DefineIntToken("{daynumeric}", (int)eDmDetailedHeaderRecIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eDmDetailedHeaderRecIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eDmDetailedHeaderRecIT::year);
    tokenContainer->DefineIntToken("{duration}", (int)eDmDetailedHeaderRecIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eDmDetailedHeaderRecIT::durationhours);
    tokenContainer->DefineIntToken("{durationevent}", (int)eDmDetailedHeaderRecIT::durationevent);
    tokenContainer->DefineIntToken("{durationeventhours}", (int)eDmDetailedHeaderRecIT::durationeventhours);
    tokenContainer->DefineIntToken("{recimgavailable}", (int)eDmDetailedHeaderRecIT::recimgavailable);
    tokenContainer->DefineIntToken("{recchannelnumber}", (int)eDmDetailedHeaderRecIT::recchannelnumber);
    InheritTokenContainer();
}

void cVeDmDetailheaderRec::SetRecording(const cRecording *recording) {
    this->recording = recording;
}

bool cVeDmDetailheaderRec::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!recording)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::name, recording->Name());

    const cRecordingInfo *info = recording->Info();
    if (info) {
        tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::epgname, info->Title());
        tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::shorttext, info->ShortText());
        const cEvent *event = info->GetEvent();
        if (event) {
            cString recDate = event->GetDateString();
            cString recTime = event->GetTimeString();
            if (strstr(*recDate, "1970")) {
                time_t start = recording->Start();
                recDate = DateString(start);
                recTime = TimeString(start);
            }
            int duration = event->Duration() / 60;
            int recDuration = recording->LengthInSeconds();
            recDuration = (recDuration > 0) ? (recDuration / 60) : 0;
            tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::date, *recDate);
            tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::time, *recTime);
            time_t startTime = event->StartTime();
            struct tm * sStartTime = localtime(&startTime);
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::year, sStartTime->tm_year + 1900);
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::daynumeric, sStartTime->tm_mday);
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::month, sStartTime->tm_mon+1);
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::duration, recDuration);
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::durationhours, recDuration / 60);
            tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::durationminutes, *cString::sprintf("%.2d", recDuration%60));
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::durationevent, duration);
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::durationeventhours, duration / 60);
            tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::durationeventminutes, *cString::sprintf("%.2d", duration%60));
        }
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_CHANNELS_READ;
        const cChannels* channels = Channels;
#else
        cChannels* channels = &Channels;
#endif
        const cChannel *channel = channels->GetByChannelID(info->ChannelID());
        if (channel) {
            tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::recchannelname, channel->Name());
            tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::recchannelid, *channel->GetChannelID().ToString());
            tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::recchannelnumber, channel->Number());
        }
    }
    string recImage = "";
    string path = recording->FileName() ? recording->FileName() : "";
    string extension =  ".jpg";
    if (FirstFileInFolder(path, extension, recImage)) {
        tokenContainer->AddIntToken((int)eDmDetailedHeaderRecIT::recimgavailable, 1);
        tokenContainer->AddStringToken((int)eDmDetailedHeaderRecST::recimgpath, *cString::sprintf("%s/%s", path.c_str(), recImage.c_str()));
    }
    bool scrapInfoAvailable = LoadFullScrapInfo(NULL, recording);
    if (scrapInfoAvailable) {
        SetHeaderScrapInfo(tokenContainer);
    }
    SetDirty();
    return true;
}

/******************************************************************
* cVeDmDetailheaderPlugin
******************************************************************/
cVeDmDetailheaderPlugin::cVeDmDetailheaderPlugin(void) {
    plugId = -1;
    plugMenuId = -1;
}

cVeDmDetailheaderPlugin::~cVeDmDetailheaderPlugin(void) {
}

void cVeDmDetailheaderPlugin::SetTokenContainer(void) {
    if (plugId == -1 || plugMenuId == -1)
        return;
    skindesignerapi::cTokenContainer *tkPlugMenu = plgManager->GetTokenContainer(plugId, plugMenuId);
    tokenContainer = new skindesignerapi::cTokenContainer(*tkPlugMenu);
    InheritTokenContainer();
}

void cVeDmDetailheaderPlugin::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
}

bool cVeDmDetailheaderPlugin::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    SetDirty();
    return true;
}
/******************************************************************
* cVeDmTablabels
******************************************************************/
cVeDmTablabels::cVeDmTablabels(void) {
    tabIndex = -1;
    activeTab = -1;
}

cVeDmTablabels::~cVeDmTablabels(void) {
}

void cVeDmTablabels::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineLoopToken("{tabs[title]}", (int)eDmTabsLT::title);
    tokenContainer->DefineLoopToken("{tabs[current]}", (int)eDmTabsLT::current);
    tokenContainer->DefineStringToken("{currenttab}", (int)eDmTabsST::currenttab);
    tokenContainer->DefineStringToken("{prevtab}", (int)eDmTabsST::prevtab);
    tokenContainer->DefineStringToken("{nexttab}", (int)eDmTabsST::nexttab);
    tabIndex = tokenContainer->LoopIndex("tabs");
    InheritTokenContainer();
}

void cVeDmTablabels::SetTabs(vector<const char*> &newTabs) { 
    tabs.clear();
    for (vector<const char*>::iterator it = newTabs.begin(); it != newTabs.end(); it++) {
        tabs.push_back(*it);
    }
    SetDirty();
}

bool cVeDmTablabels::Parse(bool forced) {
    tokenContainer->Clear();
    int numTabs = tabs.size();
    vector<int> loopInfo;
    loopInfo.push_back(numTabs);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
    for (int i = 0; i < numTabs; i++) {
        if (i == activeTab) {
            tokenContainer->AddLoopToken(tabIndex, i, (int)eDmTabsLT::current, "1");

        } else {
            tokenContainer->AddLoopToken(tabIndex, i, (int)eDmTabsLT::current, "0");
        }
        tokenContainer->AddLoopToken(tabIndex, i, (int)eDmTabsLT::title, tabs[i]);
    }

    if (activeTab >= 0 && activeTab < numTabs) {
        tokenContainer->AddStringToken((int)eDmTabsST::currenttab, tabs[activeTab]);
        tokenContainer->AddStringToken((int)eDmTabsST::prevtab, (activeTab == 0)         ? tabs[numTabs-1] : tabs[activeTab-1]);
        tokenContainer->AddStringToken((int)eDmTabsST::nexttab, (activeTab == numTabs-1) ? tabs[0]         : tabs[activeTab+1]);
    }
    return true;
}



