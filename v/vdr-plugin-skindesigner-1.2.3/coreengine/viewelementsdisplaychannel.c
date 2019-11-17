#include "viewelementsdisplaychannel.h"
#include "../config.h"
#include "../extensions/helpers.h"
#include "../services/scraper2vdr.h"
#include "../services/epgtimer.h"

/******************************************************************
* cVeDcChannelInfo
******************************************************************/
cVeDcChannelInfo::cVeDcChannelInfo(void) {
}

cVeDcChannelInfo::~cVeDcChannelInfo(void) {
}

void cVeDcChannelInfo::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{channelnumber}", (int)eDCChannelInfoST::channelnumber);
    tokenContainer->DefineStringToken("{channelname}", (int)eDCChannelInfoST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eDCChannelInfoST::channelid);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eDCChannelInfoIT::channellogoexists);
    tokenContainer->DefineIntToken("{switching}", (int)eDCChannelInfoIT::switching);
    InheritTokenContainer();
}

void cVeDcChannelInfo::Set(const cChannel *c, int number) {
    cString channelNumber("");
    cString channelName("");
    cString channelId("");
    if (c) {
        channelName = c->Name() ? c->Name() : "";
        channelId = c->GetChannelID().ToString();
        if (!c->GroupSep()) {
            channelNumber = cString::sprintf("%d%s", c->Number(), number ? "-" : "");
        }
    } else if (number) {
        channelNumber = cString::sprintf("%d-", number);
    } else {
        channelName = ChannelString(NULL, 0);
    }

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDCChannelInfoST::channelnumber, *channelNumber);
    tokenContainer->AddStringToken((int)eDCChannelInfoST::channelname, *channelName);
    tokenContainer->AddStringToken((int)eDCChannelInfoST::channelid, *channelId);
    bool logoExists = imgCache->LogoExists(*channelId);
    tokenContainer->AddIntToken((int)eDCChannelInfoIT::channellogoexists, logoExists);
    tokenContainer->AddIntToken((int)eDCChannelInfoIT::switching, (number > 0)?true:false);

    SetDirty();
}

/******************************************************************
* cVeDcChannelGroup
******************************************************************/
cVeDcChannelGroup::cVeDcChannelGroup(void) {
}

cVeDcChannelGroup::~cVeDcChannelGroup(void) {
}

void cVeDcChannelGroup::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{group}", (int)eDCChannelGroupST::group);
    tokenContainer->DefineStringToken("{nextgroup}", (int)eDCChannelGroupST::nextgroup);
    tokenContainer->DefineStringToken("{prevgroup}", (int)eDCChannelGroupST::prevgroup);
    tokenContainer->DefineStringToken("{seppath}", (int)eDCChannelGroupST::seppath);
    tokenContainer->DefineIntToken("{prevAvailable}", (int)eDCChannelGroupIT::prevAvailable);
    tokenContainer->DefineIntToken("{nextAvailable}", (int)eDCChannelGroupIT::nextAvailable);
    tokenContainer->DefineIntToken("{sepexists}", (int)eDCChannelGroupIT::sepexists);
    InheritTokenContainer();
}

void cVeDcChannelGroup::Set(const cChannel *c) {
    const char *sep = c->Name(); 
    bool sepExists = imgCache->SeparatorLogoExists(sep);

    const char *prevChannelSep = GetChannelSep(c, true);
    const char *nextChannelSep = GetChannelSep(c, false);

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDCChannelGroupST::group, sep);
    tokenContainer->AddStringToken((int)eDCChannelGroupST::nextgroup, nextChannelSep ? nextChannelSep : "");
    tokenContainer->AddStringToken((int)eDCChannelGroupST::prevgroup, prevChannelSep ? prevChannelSep : "");
    tokenContainer->AddStringToken((int)eDCChannelGroupST::seppath, sepExists ? sep : "");
    tokenContainer->AddIntToken((int)eDCChannelGroupIT::prevAvailable, (prevChannelSep)?true:false);
    tokenContainer->AddIntToken((int)eDCChannelGroupIT::nextAvailable, (nextChannelSep)?true:false);
    tokenContainer->AddIntToken((int)eDCChannelGroupIT::sepexists, sepExists);

    SetDirty();
}

const char *cVeDcChannelGroup::GetChannelSep(const cChannel *c, bool prev) {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
#else
    const cChannels* channels = &Channels;
#endif
    const cChannel *sep = prev ? channels->Prev(c) :
                                 channels->Next(c);

    for (; sep; (prev)?(sep = channels->Prev(sep)):(sep = channels->Next(sep))) {
        if (sep->GroupSep()) {
            return sep->Name();
        }
    }
    return NULL;
}
/******************************************************************
* cVeDcEpgInfo
******************************************************************/
cVeDcEpgInfo::cVeDcEpgInfo(void) {
    globalTimers = NULL; 
}

cVeDcEpgInfo::~cVeDcEpgInfo(void) {
}

void cVeDcEpgInfo::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{currenttitle}", (int)eDCEpgInfoST::currenttitle);
    tokenContainer->DefineStringToken("{currentsubtitle}", (int)eDCEpgInfoST::currentsubtitle);
    tokenContainer->DefineStringToken("{currentstart}", (int)eDCEpgInfoST::currentstart);
    tokenContainer->DefineStringToken("{currentstop}", (int)eDCEpgInfoST::currentstop);
    tokenContainer->DefineStringToken("{nexttitle}", (int)eDCEpgInfoST::nexttitle);
    tokenContainer->DefineStringToken("{nextsubtitle}", (int)eDCEpgInfoST::nextsubtitle);
    tokenContainer->DefineStringToken("{nextstart}", (int)eDCEpgInfoST::nextstart);
    tokenContainer->DefineStringToken("{nextstop}", (int)eDCEpgInfoST::nextstop);
    tokenContainer->DefineIntToken("{currentduration}", (int)eDCEpgInfoIT::currentduration);
    tokenContainer->DefineIntToken("{currentdurationhours}", (int)eDCEpgInfoIT::currentdurationhours);
    tokenContainer->DefineIntToken("{currentdurationminutes}", (int)eDCEpgInfoIT::currentdurationminutes);
    tokenContainer->DefineIntToken("{currentelapsed}", (int)eDCEpgInfoIT::currentelapsed);
    tokenContainer->DefineIntToken("{currentremaining}", (int)eDCEpgInfoIT::currentremaining);
    tokenContainer->DefineIntToken("{nextduration}", (int)eDCEpgInfoIT::nextduration);
    tokenContainer->DefineIntToken("{nextdurationhours}", (int)eDCEpgInfoIT::nextdurationhours);
    tokenContainer->DefineIntToken("{nextdurationminutes}", (int)eDCEpgInfoIT::nextdurationminutes);
    tokenContainer->DefineIntToken("{nextrecording}", (int)eDCEpgInfoIT::nextrecording);
    tokenContainer->DefineIntToken("{currentrecording}", (int)eDCEpgInfoIT::currentrecording);
    tokenContainer->DefineIntToken("{hasVPS}", (int)eDCEpgInfoIT::hasVPS);
    InheritTokenContainer();
}

void cVeDcEpgInfo::Set(const cEvent *p, const cEvent *f) {
    tokenContainer->Clear();
    if (p) {
        tokenContainer->AddStringToken((int)eDCEpgInfoST::currenttitle, p->Title());
        tokenContainer->AddStringToken((int)eDCEpgInfoST::currentsubtitle, p->ShortText());
        tokenContainer->AddStringToken((int)eDCEpgInfoST::currentstart, *p->GetTimeString());
        tokenContainer->AddStringToken((int)eDCEpgInfoST::currentstop, *p->GetEndTimeString());
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::currentduration, p->Duration() / 60);
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::currentdurationhours, p->Duration() / 3600);
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::currentdurationminutes, (p->Duration() / 60) % 60);
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::currentelapsed, (int)round((time(NULL) - p->StartTime())/60));
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::currentremaining, (int)round((p->EndTime() - time(NULL))/60));
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::currentrecording, EventHasTimer(p));
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::hasVPS, (bool)p->Vps());
    }
    if (f) {
        tokenContainer->AddStringToken((int)eDCEpgInfoST::nexttitle, f->Title());
        tokenContainer->AddStringToken((int)eDCEpgInfoST::nextsubtitle, f->ShortText());
        tokenContainer->AddStringToken((int)eDCEpgInfoST::nextstart, *f->GetTimeString());
        tokenContainer->AddStringToken((int)eDCEpgInfoST::nextstop, *f->GetEndTimeString());
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::nextduration, f->Duration() / 60);
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::nextdurationhours, f->Duration() / 3600);
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::nextdurationminutes, (f->Duration() / 60) % 60);
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::nextrecording, EventHasTimer(f));
        tokenContainer->AddIntToken((int)eDCEpgInfoIT::hasVPS, (bool)f->Vps());
    }
    SetDirty();
}

void cVeDcEpgInfo::Close(void) {
    tokenContainer->Clear();
    cViewElement::Close();
}

bool cVeDcEpgInfo::EventHasTimer(const cEvent *e) {
    if (!e) return false;
    bool hasTimer = e->HasTimer();
    for (int i = 0; i < globalTimers->Size() && !hasTimer; i++) 
        if (const cTimer *Timer = globalTimers->At(i)) 
            if (Timer->Channel() && (Timer->Channel()->GetChannelID() == e->ChannelID())) 
                if (const cEvent *timerEvent = Timer->Event())
                    if (e->EventID() == timerEvent->EventID())
                        hasTimer = true;

    return hasTimer;
}

/******************************************************************
* cVeDcProgressBar
******************************************************************/
cVeDcProgressBar::cVeDcProgressBar(void) {
    currentLast = -1;
    startTime = -1;
    duration = -1;
}

cVeDcProgressBar::~cVeDcProgressBar(void) {
}

void cVeDcProgressBar::Close(void) {
    currentLast = -1;
    startTime = -1;
    duration = -1;
    cViewElement::Close();
}

void cVeDcProgressBar::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{start}", (int)eDCProgressBarST::start);
    tokenContainer->DefineStringToken("{stop}", (int)eDCProgressBarST::stop);
    tokenContainer->DefineIntToken("{duration}", (int)eDCProgressBarIT::duration);
    tokenContainer->DefineIntToken("{elapsed}", (int)eDCProgressBarIT::elapsed);
    tokenContainer->DefineIntToken("{remaining}", (int)eDCProgressBarIT::remaining);
    tokenContainer->DefineIntToken("{permashift}", (int)eDCProgressBarIT::permashift);
    tokenContainer->DefineIntToken("{livebuffer}", (int)eDCProgressBarIT::livebuffer);
    InheritTokenContainer();
}

void cVeDcProgressBar::Set(const cEvent *p) {
    if (!p) {
        startTime = -1;
        duration = -1;
        return;
    }
    startTime = p->StartTime();
    duration = p->Duration();

    int current = 0;
    time_t t = time(NULL);
    if (t > startTime)
        current = t - startTime;

    currentLast = current;
    SetDirty();

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDCProgressBarST::start, *p->GetTimeString());
    tokenContainer->AddStringToken((int)eDCProgressBarST::stop, *p->GetEndTimeString());
    tokenContainer->AddIntToken((int)eDCProgressBarIT::duration, duration);
    tokenContainer->AddIntToken((int)eDCProgressBarIT::elapsed, current);
    tokenContainer->AddIntToken((int)eDCProgressBarIT::remaining, duration - current);
    int liveBuffer = GetLiveBuffer();
    if (liveBuffer >= 0) {
        tokenContainer->AddIntToken((int)eDCProgressBarIT::permashift, 1);
        tokenContainer->AddIntToken((int)eDCProgressBarIT::livebuffer, liveBuffer);
    } else {
        tokenContainer->AddIntToken((int)eDCProgressBarIT::permashift, 0);
    }
}

bool cVeDcProgressBar::Parse(bool force) {
    if (!cViewElement::Parse(force))
        return false;
    int current = 0;
    time_t t = time(NULL);
    if (t > startTime)
        current = t - startTime;
    if (!(current > currentLast + 3) && !force && !Dirty())
        return false;
    SetDirty();
    if (duration <= 0) {
        tokenContainer->AddIntToken((int)eDCProgressBarIT::duration, 0);
        tokenContainer->AddIntToken((int)eDCProgressBarIT::elapsed, 0);
        tokenContainer->AddIntToken((int)eDCProgressBarIT::remaining, 0);        
    } else {    
        tokenContainer->AddIntToken((int)eDCProgressBarIT::duration, duration);
        tokenContainer->AddIntToken((int)eDCProgressBarIT::elapsed, current);
        tokenContainer->AddIntToken((int)eDCProgressBarIT::remaining, duration - current);
        int liveBuffer = GetLiveBuffer();
        if (liveBuffer >= 0) {
            tokenContainer->AddIntToken((int)eDCProgressBarIT::permashift, 1);
            tokenContainer->AddIntToken((int)eDCProgressBarIT::livebuffer, liveBuffer);
        } else {
            tokenContainer->AddIntToken((int)eDCProgressBarIT::permashift, 0);
        }
    }
    return true;
}

int cVeDcProgressBar::GetLiveBuffer(void) {
    static cPlugin *pPermashift = cPluginManager::GetPlugin("permashift");
    if (!pPermashift) {
        return -1;
    }
    int buffer = 0;
    if (pPermashift->Service("Permashift-GetUsedBufferSecs-v1", &buffer)) {
        return buffer;
    }
    return -1;
}
/******************************************************************
* cVeDcStatusInfo
******************************************************************/
cVeDcStatusInfo::cVeDcStatusInfo(void) {
}

cVeDcStatusInfo::~cVeDcStatusInfo(void) {
}

void cVeDcStatusInfo::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{isRadio}", (int)eDCStatusInfoIT::isRadio);
    tokenContainer->DefineIntToken("{hasVT}", (int)eDCStatusInfoIT::hasVT);
    tokenContainer->DefineIntToken("{isStereo}", (int)eDCStatusInfoIT::isStereo);
    tokenContainer->DefineIntToken("{isDolby}", (int)eDCStatusInfoIT::isDolby);
    tokenContainer->DefineIntToken("{isEncrypted}", (int)eDCStatusInfoIT::isEncrypted);
    tokenContainer->DefineIntToken("{isRecording}", (int)eDCStatusInfoIT::isRecording);
    tokenContainer->DefineIntToken("{newmails}", (int)eDCStatusInfoIT::newmails);
    InheritTokenContainer();
}

void cVeDcStatusInfo::Set(const cChannel *c) {
    bool isRadio = !c->Vpid() && c->Apid(0);
    bool hasVT = c->Vpid() && c->Tpid();
    bool isStereo = c->Apid(0);
    bool isDolby = c->Dpid(0);
    bool isEncrypted = c->Ca();
    bool isRecording = cRecordControls::Active();

    for (int i = 0; i < globalTimers->Size() && !isRecording; i++) {
        if (const cTimer *Timer = globalTimers->At(i)) {
            if (Timer->Recording()) 
                isRecording = true;
            else if (cEpgTimer_Interface_V1* epgTimer = dynamic_cast<cEpgTimer_Interface_V1*>((cTimer*)Timer)) {
                if (epgTimer->State() == 'R')
                    isRecording = true;
            }
        }
    }

    tokenContainer->AddIntToken((int)eDCStatusInfoIT::isRadio, isRadio);
    tokenContainer->AddIntToken((int)eDCStatusInfoIT::hasVT, hasVT);
    tokenContainer->AddIntToken((int)eDCStatusInfoIT::isStereo, isStereo);
    tokenContainer->AddIntToken((int)eDCStatusInfoIT::isDolby, isDolby);
    tokenContainer->AddIntToken((int)eDCStatusInfoIT::isEncrypted, isEncrypted);
    tokenContainer->AddIntToken((int)eDCStatusInfoIT::isRecording, isRecording);
    tokenContainer->AddIntToken((int)eDCStatusInfoIT::newmails, CheckMails());
    SetDirty();
}

bool cVeDcStatusInfo::CheckMails(void) {
    static cPlugin *pMailbox = cPluginManager::GetPlugin("mailbox");
    if (!pMailbox) {
        return false;
    }
    bool newMail = false;
    if (pMailbox->Service("MailBox-HasNewMail-1.0", &newMail)) {
        return newMail;
    }
    return false;
}

/******************************************************************
* cVeDcAudioInfo
******************************************************************/
cVeDcAudioInfo::cVeDcAudioInfo(void) {
    lastNumAudioTracks = -1;
    lastAudioChannel = -1;
    lastTracDesc = NULL;
    lastTrackLang = NULL;
}

cVeDcAudioInfo::~cVeDcAudioInfo(void) {
}

void cVeDcAudioInfo::Close(void) {
    lastNumAudioTracks = -1;
    lastAudioChannel = -1;
    free(lastTracDesc);
    lastTracDesc = NULL;
    free(lastTrackLang);
    lastTrackLang = NULL;
    cViewElement::Close();
}

void cVeDcAudioInfo::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{trackdesc}", (int)eDCAudioInfoST::trackdesc);
    tokenContainer->DefineStringToken("{tracklang}", (int)eDCAudioInfoST::tracklang);
    tokenContainer->DefineIntToken("{numaudiotracks}", (int)eDCAudioInfoIT::numaudiotracks);
    tokenContainer->DefineIntToken("{audiochannel}", (int)eDCAudioInfoIT::audiochannel);
    InheritTokenContainer();
}

bool cVeDcAudioInfo::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    int numAudioTracks = 0;
    int audioChannel = -1;
    const char *trackDescription = "";
    const char *trackLanguage = "";

    cDevice *device = cDevice::PrimaryDevice();
    if (device) {
        numAudioTracks = device->NumAudioTracks();
        audioChannel = device->GetAudioChannel();
        if (numAudioTracks > 0) {
            const tTrackId *track = device->GetTrack(device->GetCurrentAudioTrack());
            if (track) {
                trackDescription = track->description ? track->description : "";
                trackLanguage = track->language ? track->language : "";
            }
        }
    }
    if (   !Dirty() && !forced && lastNumAudioTracks == numAudioTracks 
           && lastAudioChannel == audioChannel 
           && lastTracDesc && !strcmp(trackDescription, lastTracDesc)
           && lastTrackLang && !strcmp(trackLanguage, lastTrackLang)) {
        return false;
    }

    lastNumAudioTracks = numAudioTracks;
    lastAudioChannel = audioChannel;
    free(lastTracDesc);
    lastTracDesc = strdup(trackDescription);
    free(lastTrackLang);
    lastTrackLang = strdup(trackLanguage);
    SetDirty();

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDCAudioInfoIT::numaudiotracks, numAudioTracks);
    tokenContainer->AddIntToken((int)eDCAudioInfoIT::audiochannel, audioChannel);
    tokenContainer->AddStringToken((int)eDCAudioInfoST::trackdesc, trackDescription);
    tokenContainer->AddStringToken((int)eDCAudioInfoST::tracklang, trackLanguage);
    return true;
}
/******************************************************************
* cVeDcScreenResolution
******************************************************************/
cVeDcScreenResolution::cVeDcScreenResolution(void) {
    lastScreenWidth = -1;
    lastScreenHeight = -1;
    lastAspect = -1.0f;
}

cVeDcScreenResolution::~cVeDcScreenResolution(void) {
}

void cVeDcScreenResolution::Close(void) {
    lastScreenWidth = -1;
    lastScreenHeight = -1;
    lastAspect = -1.0f;
    cViewElement::Close();
}

void cVeDcScreenResolution::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{resolution}", (int)eDCScreenResolutionST::resolution);
    tokenContainer->DefineStringToken("{aspect}", (int)eDCScreenResolutionST::aspect);
    tokenContainer->DefineIntToken("{screenwidth}", (int)eDCScreenResolutionIT::screenwidth);
    tokenContainer->DefineIntToken("{screenheight}", (int)eDCScreenResolutionIT::screenheight);
    tokenContainer->DefineIntToken("{isHD}", (int)eDCScreenResolutionIT::isHD);
    tokenContainer->DefineIntToken("{isWideScreen}", (int)eDCScreenResolutionIT::isWideScreen);
    InheritTokenContainer();
}

bool cVeDcScreenResolution::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    int screenWidth = 0;
    int screenHeight = 0;
    double aspect = 0;

    cDevice::PrimaryDevice()->GetVideoSize(screenWidth, screenHeight, aspect);

    if ((lastScreenWidth == screenWidth) && (lastScreenHeight == screenHeight) && (lastAspect == aspect))
        return false;

    if ((screenWidth == 0) && (screenHeight == 0))
        return false;

    lastScreenWidth = screenWidth;
    lastScreenHeight = screenHeight;
    lastAspect = aspect;
    SetDirty();

    bool isHD = false;
    string resName = GetScreenResolutionString(screenWidth, screenHeight, &isHD);

    bool isWideScreen = false;
    string aspectName = GetScreenAspectString(aspect, &isWideScreen);

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDCScreenResolutionIT::screenwidth, screenWidth);
    tokenContainer->AddIntToken((int)eDCScreenResolutionIT::screenheight, screenHeight);
    tokenContainer->AddIntToken((int)eDCScreenResolutionIT::isHD, isHD);
    tokenContainer->AddIntToken((int)eDCScreenResolutionIT::isWideScreen, isWideScreen);
    tokenContainer->AddStringToken((int)eDCScreenResolutionST::resolution, resName.c_str());
    tokenContainer->AddStringToken((int)eDCScreenResolutionST::aspect, aspectName.c_str());
    return true;
}
/******************************************************************
* cVeDcSignalQuality
******************************************************************/
cVeDcSignalQuality::cVeDcSignalQuality(void) {
    lastSignalDisplay = -1;
    lastSignalStrength = -1;
    lastSignalQuality = -1;
}

cVeDcSignalQuality::~cVeDcSignalQuality(void) {
}

void cVeDcSignalQuality::Close(void) {
    lastSignalDisplay = -1;
    lastSignalStrength = -1;
    lastSignalQuality = -1;
    cViewElement::Close();
}

void cVeDcSignalQuality::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{signalstrength}", (int)eDCSignalQualityIT::signalstrength);
    tokenContainer->DefineIntToken("{signalquality}", (int)eDCSignalQualityIT::signalquality);
    InheritTokenContainer();
}

bool cVeDcSignalQuality::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    time_t now = time(NULL);
    if (now == lastSignalDisplay)
        return false;
    lastSignalDisplay = now;

    int signalStrength = cDevice::ActualDevice()->SignalStrength();
    int signalQuality = cDevice::ActualDevice()->SignalQuality();

    if (signalStrength < 0) signalStrength = 0;
    if (signalQuality < 0) signalQuality = 0;

    if (signalStrength == 0 && signalQuality==0)
        return false;

    if ((lastSignalStrength == signalStrength) && (lastSignalQuality == signalQuality))
        return false;
    SetDirty();
    lastSignalStrength = signalStrength;
    lastSignalQuality = signalQuality;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDCSignalQualityIT::signalstrength, signalStrength);
    tokenContainer->AddIntToken((int)eDCSignalQualityIT::signalquality, signalQuality);
    return true;
}

/******************************************************************
* cVeDcScraperContent
******************************************************************/
cVeDcScraperContent::cVeDcScraperContent(void) {
}

cVeDcScraperContent::~cVeDcScraperContent(void) {
}

void cVeDcScraperContent::Close(void) {
    tokenContainer->Clear();
    cViewElement::Close();
}

void cVeDcScraperContent::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperPosterBannerST::posterpath);
    tokenContainer->DefineStringToken("{bannerpath}", (int)eScraperPosterBannerST::bannerpath);
    tokenContainer->DefineStringToken("{mediapath}", (int)eScraperPosterBannerST::mediapath);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperPosterBannerIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperPosterBannerIT::posterheight);
    tokenContainer->DefineIntToken("{hasposter}", (int)eScraperPosterBannerIT::hasposter);
    tokenContainer->DefineIntToken("{bannerwidth}", (int)eScraperPosterBannerIT::bannerwidth);
    tokenContainer->DefineIntToken("{bannerheight}", (int)eScraperPosterBannerIT::bannerheight);
    tokenContainer->DefineIntToken("{hasbanner}", (int)eScraperPosterBannerIT::hasbanner);
    tokenContainer->DefineIntToken("{mediawidth}", (int)eScraperPosterBannerIT::mediawidth);
    tokenContainer->DefineIntToken("{mediaheight}", (int)eScraperPosterBannerIT::mediaheight);
    tokenContainer->DefineIntToken("{isbanner}", (int)eScraperPosterBannerIT::isbanner);
    InheritTokenContainer();
}

void cVeDcScraperContent::Set(const cEvent *e) {
    tokenContainer->Clear();
    SetPosterBanner(tokenContainer, e, NULL);
    SetDirty();
}

/******************************************************************
* cVeDcScreenResolution
******************************************************************/
cVeDcEcmInfo::cVeDcEcmInfo(void) {
    channelSid = -1;
    lastEcmInfo.hops = -1;
    lastEcmInfo.ecmtime = -1;
    lastEcmInfo.caid = -1;
    lastEcmInfo.pid = -1;
    lastEcmInfo.prid = -1;
}

cVeDcEcmInfo::~cVeDcEcmInfo(void) {
}

void cVeDcEcmInfo::Close(void) {
    channelSid = -1;
    lastEcmInfo.hops = -1;
    lastEcmInfo.ecmtime = -1;
    lastEcmInfo.caid = -1;
    lastEcmInfo.pid = -1;
    lastEcmInfo.prid = -1;
    cViewElement::Close();
}

void cVeDcEcmInfo::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{cardsystem}", (int)eDCEcmInfoST::cardsystem);
    tokenContainer->DefineStringToken("{reader}", (int)eDCEcmInfoST::reader);
    tokenContainer->DefineStringToken("{from}", (int)eDCEcmInfoST::from);
    tokenContainer->DefineStringToken("{protocol}", (int)eDCEcmInfoST::protocol);
    tokenContainer->DefineIntToken("{caid}", (int)eDCEcmInfoIT::caid);
    tokenContainer->DefineIntToken("{pid}", (int)eDCEcmInfoIT::pid);
    tokenContainer->DefineIntToken("{prid}", (int)eDCEcmInfoIT::prid);
    tokenContainer->DefineIntToken("{ecmtime}", (int)eDCEcmInfoIT::ecmtime);
    tokenContainer->DefineIntToken("{hops}", (int)eDCEcmInfoIT::hops);
    InheritTokenContainer();
}

void cVeDcEcmInfo::Set(const cChannel *c) {
    channelSid = c->Sid();
}

bool cVeDcEcmInfo::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    static cPlugin *pDVBApi = cPluginManager::GetPlugin("dvbapi");
    if (!pDVBApi)
        return false;
    if (channelSid < 0)
        return false;

    sDVBAPIEcmInfo ecmInfo;
    ecmInfo.sid = channelSid;

    if (!pDVBApi->Service("GetEcmInfo", &ecmInfo)) {
        return false;
    }

    if (ecmInfo.hops < 0 || ecmInfo.ecmtime <= 0 || ecmInfo.ecmtime > 100000)
        return false;
    if (CompareECMInfos(&ecmInfo))
        return false;
    lastEcmInfo = ecmInfo;
    SetDirty();

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDCEcmInfoIT::caid, ecmInfo.caid);
    tokenContainer->AddIntToken((int)eDCEcmInfoIT::pid, ecmInfo.pid);
    tokenContainer->AddIntToken((int)eDCEcmInfoIT::prid, ecmInfo.prid);
    tokenContainer->AddIntToken((int)eDCEcmInfoIT::ecmtime, ecmInfo.ecmtime);
    tokenContainer->AddIntToken((int)eDCEcmInfoIT::hops, ecmInfo.hops);
    tokenContainer->AddStringToken((int)eDCEcmInfoST::cardsystem, *ecmInfo.cardsystem);
    tokenContainer->AddStringToken((int)eDCEcmInfoST::reader, *ecmInfo.reader);
    tokenContainer->AddStringToken((int)eDCEcmInfoST::from, *ecmInfo.from);
    tokenContainer->AddStringToken((int)eDCEcmInfoST::protocol, *ecmInfo.protocol);
    return true;
}

bool cVeDcEcmInfo::CompareECMInfos(sDVBAPIEcmInfo *ecmInfo) {
    if (ecmInfo->caid != lastEcmInfo.caid)
        return false;
    if (ecmInfo->pid != lastEcmInfo.pid)
        return false;
    if (ecmInfo->prid != lastEcmInfo.prid)
        return false;
    if (ecmInfo->ecmtime != lastEcmInfo.ecmtime)
        return false;
    if (ecmInfo->hops != lastEcmInfo.hops)
        return false;
    return true;
}

/******************************************************************
* cVeDcChannelHints
******************************************************************/
cVeDcChannelHints::cVeDcChannelHints(void) {
    hints = NULL;
    numHints = 0;
    current = 0;
    hintsIndex = -1;
    active = false;
}

cVeDcChannelHints::~cVeDcChannelHints(void) {
    delete[] hints;
}

void cVeDcChannelHints::Close(void) {
    cViewElement::Close();
}

void cVeDcChannelHints::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{numhints}", (int)eDCChannelHintsIT::numhints);
    tokenContainer->DefineLoopToken("{hints[channelnumber]}", (int)eDCChannelHintsLT::channelnumber);
    tokenContainer->DefineLoopToken("{hints[channelname]}", (int)eDCChannelHintsLT::channelname);
    tokenContainer->DefineLoopToken("{hints[channelid]}", (int)eDCChannelHintsLT::channelid);
    tokenContainer->DefineLoopToken("{hints[channellogoexists]}", (int)eDCChannelHintsLT::channellogoexists);
    hintsIndex = tokenContainer->LoopIndex("hints");
    InheritTokenContainer();
}

void cVeDcChannelHints::SetNumHints(int num) {
    delete[] hints;
    numHints = num;
    hints = new const cChannel*[num];
    current = 0;
    active = true;
    SetDirty();
}


void cVeDcChannelHints::SetHint(const cChannel *c) {
    hints[current++] = c;
}

bool cVeDcChannelHints::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!Dirty())
        return false;
    if (!hints)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDCChannelHintsIT::numhints, numHints);
    
    vector<int> loopInfo;
    loopInfo.push_back(numHints);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);

    for (int i=0; i < numHints; i++) {
        if (hints[i]) {
            tokenContainer->AddLoopToken(hintsIndex, i, (int)eDCChannelHintsLT::channelnumber, *cString::sprintf("%d", hints[i]->Number()));
            tokenContainer->AddLoopToken(hintsIndex, i, (int)eDCChannelHintsLT::channelname, hints[i]->Name());
            cString channelID = hints[i]->GetChannelID().ToString();
            tokenContainer->AddLoopToken(hintsIndex, i, (int)eDCChannelHintsLT::channelid, *channelID);
            bool logoExists = imgCache->LogoExists(*channelID);
            tokenContainer->AddLoopToken(hintsIndex, i, (int)eDCChannelHintsLT::channellogoexists, *cString::sprintf("%d", logoExists ? 1 : 0));
        }
    }
    return true;
}

/******************************************************************
* cVeDcChannelDetail
******************************************************************/
cVeDcChannelDetail::cVeDcChannelDetail(void) {
    channel = NULL;
}

cVeDcChannelDetail::~cVeDcChannelDetail(void) {
    channel = NULL;
}

void cVeDcChannelDetail::Close(void) {
    channel = NULL;
    cViewElement::Close();
}

void cVeDcChannelDetail::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{channelname}", (int)eDCChannelDetailST::channelname);
    tokenContainer->DefineStringToken("{currenttitle}", (int)eDCChannelDetailST::currenttitle);
    tokenContainer->DefineStringToken("{currentshorttext}", (int)eDCChannelDetailST::currentshorttext);
    tokenContainer->DefineStringToken("{currentdescription}", (int)eDCChannelDetailST::currentdescription);
    tokenContainer->DefineStringToken("{currentstart}", (int)eDCChannelDetailST::currentstart);
    tokenContainer->DefineStringToken("{currentstop}", (int)eDCChannelDetailST::currentstop);
    tokenContainer->DefineStringToken("{currentdurationminutes}", (int)eDCChannelDetailST::currentdurationminutes);
    tokenContainer->DefineStringToken("{nexttitle}", (int)eDCChannelDetailST::nexttitle);
    tokenContainer->DefineStringToken("{nextshorttext}", (int)eDCChannelDetailST::nextshorttext);
    tokenContainer->DefineStringToken("{nextdescription}", (int)eDCChannelDetailST::nextdescription);
    tokenContainer->DefineStringToken("{nextstart}", (int)eDCChannelDetailST::nextstart);
    tokenContainer->DefineStringToken("{nextstop}", (int)eDCChannelDetailST::nextstop);
    tokenContainer->DefineStringToken("{nextdurationminutes}", (int)eDCChannelDetailST::nextdurationminutes);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eDCChannelDetailIT::channelnumber);
    tokenContainer->DefineIntToken("{currentduration}", (int)eDCChannelDetailIT::currentduration);
    tokenContainer->DefineIntToken("{currentdurationhours}", (int)eDCChannelDetailIT::currentdurationhours);
    tokenContainer->DefineIntToken("{currentelapsed}", (int)eDCChannelDetailIT::currentelapsed);
    tokenContainer->DefineIntToken("{currentremaining}", (int)eDCChannelDetailIT::currentremaining);
    tokenContainer->DefineIntToken("{nextduration}", (int)eDCChannelDetailIT::nextduration);
    tokenContainer->DefineIntToken("{nextdurationhours}", (int)eDCChannelDetailIT::nextdurationhours);
    tokenContainer->DefineStringToken("{movietitle}", (int)eScraperST::movietitle);
    tokenContainer->DefineStringToken("{movieoriginalTitle}", (int)eScraperST::movieoriginalTitle);
    tokenContainer->DefineStringToken("{movietagline}", (int)eScraperST::movietagline);
    tokenContainer->DefineStringToken("{movieoverview}", (int)eScraperST::movieoverview);
    tokenContainer->DefineStringToken("{moviegenres}", (int)eScraperST::moviegenres);
    tokenContainer->DefineStringToken("{moviehomepage}", (int)eScraperST::moviehomepage);
    tokenContainer->DefineStringToken("{moviereleasedate}", (int)eScraperST::moviereleasedate);
    tokenContainer->DefineStringToken("{moviepopularity}", (int)eScraperST::moviepopularity);
    tokenContainer->DefineStringToken("{movievoteaverage}", (int)eScraperST::movievoteaverage);
    tokenContainer->DefineStringToken("{posterpath}", (int)eScraperST::posterpath);
    tokenContainer->DefineStringToken("{fanartpath}", (int)eScraperST::fanartpath);
    tokenContainer->DefineStringToken("{moviecollectionName}", (int)eScraperST::moviecollectionName);
    tokenContainer->DefineStringToken("{collectionposterpath}", (int)eScraperST::collectionposterpath);
    tokenContainer->DefineStringToken("{collectionfanartpath}", (int)eScraperST::collectionfanartpath);
    tokenContainer->DefineStringToken("{seriesname}", (int)eScraperST::seriesname);
    tokenContainer->DefineStringToken("{seriesoverview}", (int)eScraperST::seriesoverview);
    tokenContainer->DefineStringToken("{seriesfirstaired}", (int)eScraperST::seriesfirstaired);
    tokenContainer->DefineStringToken("{seriesnetwork}", (int)eScraperST::seriesnetwork);
    tokenContainer->DefineStringToken("{seriesgenre}", (int)eScraperST::seriesgenre);
    tokenContainer->DefineStringToken("{seriesrating}", (int)eScraperST::seriesrating);
    tokenContainer->DefineStringToken("{seriesstatus}", (int)eScraperST::seriesstatus);
    tokenContainer->DefineStringToken("{episodetitle}", (int)eScraperST::episodetitle);
    tokenContainer->DefineStringToken("{episodefirstaired}", (int)eScraperST::episodefirstaired);
    tokenContainer->DefineStringToken("{episodegueststars}", (int)eScraperST::episodegueststars);
    tokenContainer->DefineStringToken("{episodeoverview}", (int)eScraperST::episodeoverview);
    tokenContainer->DefineStringToken("{episoderating}", (int)eScraperST::episoderating);
    tokenContainer->DefineStringToken("{episodeimagepath}", (int)eScraperST::episodeimagepath);
    tokenContainer->DefineStringToken("{seasonposterpath}", (int)eScraperST::seasonposterpath);
    tokenContainer->DefineStringToken("{seriesposter1path}", (int)eScraperST::seriesposter1path);
    tokenContainer->DefineStringToken("{seriesposter2path}", (int)eScraperST::seriesposter2path);
    tokenContainer->DefineStringToken("{seriesposter3path}", (int)eScraperST::seriesposter3path);
    tokenContainer->DefineStringToken("{seriesfanart1path}", (int)eScraperST::seriesfanart1path);
    tokenContainer->DefineStringToken("{seriesfanart2path}", (int)eScraperST::seriesfanart2path);
    tokenContainer->DefineStringToken("{seriesfanart3path}", (int)eScraperST::seriesfanart3path);
    tokenContainer->DefineStringToken("{seriesbanner1path}", (int)eScraperST::seriesbanner1path);
    tokenContainer->DefineStringToken("{seriesbanner2path}", (int)eScraperST::seriesbanner2path);
    tokenContainer->DefineStringToken("{seriesbanner3path}", (int)eScraperST::seriesbanner3path);
    tokenContainer->DefineIntToken("{ismovie}", (int)eScraperIT::ismovie);
    tokenContainer->DefineIntToken("{moviebudget}", (int)eScraperIT::moviebudget);
    tokenContainer->DefineIntToken("{movierevenue}", (int)eScraperIT::movierevenue);
    tokenContainer->DefineIntToken("{movieadult}", (int)eScraperIT::movieadult);
    tokenContainer->DefineIntToken("{movieruntime}", (int)eScraperIT::movieruntime);
    tokenContainer->DefineIntToken("{isseries}", (int)eScraperIT::isseries);
    tokenContainer->DefineIntToken("{posterwidth}", (int)eScraperIT::posterwidth);
    tokenContainer->DefineIntToken("{posterheight}", (int)eScraperIT::posterheight);
    tokenContainer->DefineIntToken("{fanartwidth}", (int)eScraperIT::fanartwidth);
    tokenContainer->DefineIntToken("{fanartheight}", (int)eScraperIT::fanartheight);
    tokenContainer->DefineIntToken("{movieiscollection}", (int)eScraperIT::movieiscollection);
    tokenContainer->DefineIntToken("{collectionposterwidth}", (int)eScraperIT::collectionposterwidth);
    tokenContainer->DefineIntToken("{collectionposterheight}", (int)eScraperIT::collectionposterheight);
    tokenContainer->DefineIntToken("{collectionfanartwidth}", (int)eScraperIT::collectionfanartwidth);
    tokenContainer->DefineIntToken("{collectionfanartheight}", (int)eScraperIT::collectionfanartheight);
    tokenContainer->DefineIntToken("{epgpicavailable}", (int)eScraperIT::epgpicavailable);
    tokenContainer->DefineIntToken("{episodenumber}", (int)eScraperIT::episodenumber);
    tokenContainer->DefineIntToken("{episodeseason}", (int)eScraperIT::episodeseason);
    tokenContainer->DefineIntToken("{episodeimagewidth}", (int)eScraperIT::episodeimagewidth);
    tokenContainer->DefineIntToken("{episodeimageheight}", (int)eScraperIT::episodeimageheight);
    tokenContainer->DefineIntToken("{seasonposterwidth}", (int)eScraperIT::seasonposterwidth);
    tokenContainer->DefineIntToken("{seasonposterheight}", (int)eScraperIT::seasonposterheight);
    tokenContainer->DefineIntToken("{seriesposter1width}", (int)eScraperIT::seriesposter1width);
    tokenContainer->DefineIntToken("{seriesposter1height}", (int)eScraperIT::seriesposter1height);
    tokenContainer->DefineIntToken("{seriesposter2width}", (int)eScraperIT::seriesposter2width);
    tokenContainer->DefineIntToken("{seriesposter2height}", (int)eScraperIT::seriesposter2height);
    tokenContainer->DefineIntToken("{seriesposter3width}", (int)eScraperIT::seriesposter3width);
    tokenContainer->DefineIntToken("{seriesposter3height}", (int)eScraperIT::seriesposter3height);
    tokenContainer->DefineIntToken("{seriesfanart1width}", (int)eScraperIT::seriesfanart1width);
    tokenContainer->DefineIntToken("{seriesfanart1height}", (int)eScraperIT::seriesfanart1height);
    tokenContainer->DefineIntToken("{seriesfanart2width}", (int)eScraperIT::seriesfanart2width);
    tokenContainer->DefineIntToken("{seriesfanart2height}", (int)eScraperIT::seriesfanart2height);
    tokenContainer->DefineIntToken("{seriesfanart3width}", (int)eScraperIT::seriesfanart3width);
    tokenContainer->DefineIntToken("{seriesfanart3height}", (int)eScraperIT::seriesfanart3height);
    tokenContainer->DefineIntToken("{seriesbanner1width}", (int)eScraperIT::seriesbanner1width);
    tokenContainer->DefineIntToken("{seriesbanner1height}", (int)eScraperIT::seriesbanner1height);
    tokenContainer->DefineIntToken("{seriesbanner2width}", (int)eScraperIT::seriesbanner2width);
    tokenContainer->DefineIntToken("{seriesbanner2height}", (int)eScraperIT::seriesbanner2height);
    tokenContainer->DefineIntToken("{seriesbanner3width}", (int)eScraperIT::seriesbanner3width);
    tokenContainer->DefineIntToken("{seriesbanner3height}", (int)eScraperIT::seriesbanner3height);
    tokenContainer->DefineLoopToken("{actors[name]}", (int)eScraperLT::name);
    tokenContainer->DefineLoopToken("{actors[role]}", (int)eScraperLT::role);
    tokenContainer->DefineLoopToken("{actors[thumb]}", (int)eScraperLT::thumb);
    tokenContainer->DefineLoopToken("{actors[thumbwidth]}", (int)eScraperLT::thumbwidth);
    tokenContainer->DefineLoopToken("{actors[thumbheight]}", (int)eScraperLT::thumbheight);
    actorsIndex = tokenContainer->LoopIndex("actors");
    InheritTokenContainer();
}

void cVeDcChannelDetail::Set(const cChannel *c) {
    channel = c;
}

bool cVeDcChannelDetail::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (!channel)
        return false;

    SetDirty();
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDCChannelDetailIT::channelnumber, channel->Number());
    tokenContainer->AddStringToken((int)eDCChannelDetailST::channelname, channel->Name());

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_SCHEDULES_READ;
    const cSchedules* schedules = Schedules;
#else
    cSchedulesLock schedulesLock;
    const cSchedules* schedules = (cSchedules*)cSchedules::Schedules(schedulesLock);
#endif
    const cSchedule *schedule = schedules->GetSchedule(channel);
    if (schedule) {
        const cEvent *presentEvent = schedule->GetPresentEvent();
        if (presentEvent) {
            tokenContainer->AddStringToken((int)eDCChannelDetailST::currenttitle, presentEvent->Title());
            tokenContainer->AddStringToken((int)eDCChannelDetailST::currentshorttext, presentEvent->ShortText());
            tokenContainer->AddStringToken((int)eDCChannelDetailST::currentdescription, presentEvent->Description());
            tokenContainer->AddStringToken((int)eDCChannelDetailST::currentstart, *presentEvent->GetTimeString());
            tokenContainer->AddStringToken((int)eDCChannelDetailST::currentstop, *presentEvent->GetEndTimeString());
            tokenContainer->AddIntToken((int)eDCChannelDetailIT::currentduration, presentEvent->Duration()/60);
            tokenContainer->AddIntToken((int)eDCChannelDetailIT::currentelapsed, (time(0) - presentEvent->StartTime())/60);
            tokenContainer->AddIntToken((int)eDCChannelDetailIT::currentremaining, presentEvent->Duration()/60 - (time(0) - presentEvent->StartTime())/60);
            tokenContainer->AddIntToken((int)eDCChannelDetailIT::currentdurationhours, presentEvent->Duration() / 3600);
            tokenContainer->AddStringToken((int)eDCChannelDetailST::currentdurationminutes, *cString::sprintf("%.2d", (presentEvent->Duration() / 60)%60));
            vector<int> loopInfo;
            bool scrapInfoAvailable = LoadFullScrapInfo(presentEvent, NULL);
            int numActors = NumActors();
            loopInfo.push_back(numActors);
            tokenContainer->CreateLoopTokenContainer(&loopInfo);
            if (scrapInfoAvailable) {
                SetFullScrapInfo(tokenContainer, actorsIndex);
            }
        }
        const cList<cEvent> *events = schedule->Events();
        if (events && presentEvent) {
            const cEvent *nextEvent = events->Next(presentEvent);
            if (nextEvent) {
                tokenContainer->AddStringToken((int)eDCChannelDetailST::nexttitle, nextEvent->Title());
                tokenContainer->AddStringToken((int)eDCChannelDetailST::nextshorttext, nextEvent->ShortText());
                tokenContainer->AddStringToken((int)eDCChannelDetailST::nextdescription, nextEvent->Description());
                tokenContainer->AddStringToken((int)eDCChannelDetailST::nextstart, *nextEvent->GetTimeString());
                tokenContainer->AddStringToken((int)eDCChannelDetailST::nextstop, *nextEvent->GetEndTimeString());
                tokenContainer->AddIntToken((int)eDCChannelDetailIT::nextduration, nextEvent->Duration() / 60);
                tokenContainer->AddIntToken((int)eDCChannelDetailIT::nextdurationhours, nextEvent->Duration() / 3600);
                tokenContainer->AddStringToken((int)eDCChannelDetailST::nextdurationminutes, *cString::sprintf("%.2d", (nextEvent->Duration() / 60)%60));
            }
        }
    }
    return true;
}
