#include "viewdetail.h"
#include "../config.h"
#include "../extensions/helpers.h"
/******************************************************************
* cViewDetail
******************************************************************/
cViewDetail::cViewDetail(void) {
    activeTab = NULL;
    activeTabIndex = -1;
    numTabs = 0;
    plugId = -1;
    plugMenuId = -1;
}

cViewDetail::~cViewDetail(void) {
}

void cViewDetail::SetActiveTab(void) {
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        if (node->ActiveTab()) {
            activeTab = dynamic_cast<cArea*>(node);
            activeTabIndex = 0;
            return;
        }
    }
}

int cViewDetail::GetWidth(void) {
    cAreaNode *node = areaNodes.First();
    if (!node)
        return 0;
    return node->GetWidth();
}

void cViewDetail::ResetTabs(void) {
    activeTab = NULL;
    activeTabIndex = -1;
}

void cViewDetail::Clear(bool forceClearBackground) {
    cViewElement::Close();    
}

void cViewDetail::Close(void) {
    cViewElement::Close();    
    activeTab = NULL;
    activeTabIndex = -1;
    numTabs = 0;
}

void cViewDetail::Render(void) {
    if (!dirty || blocked)
        return;

    if (attribs->DoDebug())
        Debug();
    
    if (!activeTab)
        SetActiveTab();
    if (!activeTab)
        return;

    activeTab->Clear();
    if (activeTab->Execute()) {
        activeTab->Render();
    }

    dirty = false;
}

void cViewDetail::Scrollbar(int &barheight, int &offset, bool &end) {
    if (!activeTab)
        return;
    cRect tabViewPort = activeTab->ViewPort();
    cRect tabDrawPort = activeTab->DrawPort();
    int totalHeight = tabDrawPort.Height();
    int screenHeight = tabViewPort.Height();
    int y = (-1)*tabDrawPort.Y();
    if (totalHeight == 0) {
        return;
    }
    if (totalHeight <= screenHeight)
        barheight = 1000;
    else {
        barheight = (double)screenHeight / (double) totalHeight * 1000; 
    }
    offset = (double)y / (double) totalHeight * 1000;
    end = true;
}

bool cViewDetail::ScrollUp(bool page) {
    if (!activeTab)
        return false;
    cRect tabDrawPort = activeTab->DrawPort();
    int scrollStep = activeTab->ScrollStep();
    if (page) {
        cRect tabViewPort = activeTab->ViewPort();
        scrollStep = tabViewPort.Height();
    }
    int aktHeight = tabDrawPort.Y();
    if (aktHeight >= 0) {
        return false;
    }
    int newY = aktHeight + scrollStep;
    if (newY > 0)
        newY = 0;
    cPoint dp(0, newY);
    activeTab->SetDrawPort(dp);
    return true;
}

bool cViewDetail::ScrollDown(bool page) {
    if (!activeTab)
        return false;
    cRect tabViewPort = activeTab->ViewPort();
    cRect tabDrawPort = activeTab->DrawPort();
    int scrollStep = activeTab->ScrollStep();
    if (page) {
        scrollStep = tabViewPort.Height();
    }
    int aktHeight = tabDrawPort.Y();
    int totalHeight = tabDrawPort.Height();
    int screenHeight = tabViewPort.Height();

    if (totalHeight - ((-1)*aktHeight) == screenHeight) {
        return false;
    }
    int newY = aktHeight - scrollStep;
    if ((-1)*newY > totalHeight - screenHeight)
        newY = (-1)*(totalHeight - screenHeight);
    cPoint dp(0, newY);
    activeTab->SetDrawPort(dp);
    return true;
}

int cViewDetail::GetTabs(vector<const char*> &tabs) {
    int i=0;
    for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
        if (!node->Execute())
            continue;
        tabs.push_back(node->Name());
        if (node->ActiveTab()) {
            activeTabIndex = i;
        }
        i++;
    }
    numTabs = i;
    return activeTabIndex;
}

void cViewDetail::NextTab(void) {
    cArea *nextActiveTab = NULL;
    for (cAreaNode *node = areaNodes.Next(activeTab); node; node = areaNodes.Next(node)) {
        if (node->Execute()) {
            nextActiveTab = dynamic_cast<cArea*>(node);
            activeTabIndex++;
            break;
        }
    }
    if (!nextActiveTab) {
        nextActiveTab = dynamic_cast<cArea*>(areaNodes.First());
        activeTabIndex = 0;
    }
    activeTab = nextActiveTab;
}

void cViewDetail::PrevTab(void) {
    cArea *prevActiveTab = NULL;
    for (cAreaNode *node = areaNodes.Prev(activeTab); node; node = areaNodes.Prev(node)) {
        if (node->Execute()) {
            prevActiveTab = dynamic_cast<cArea*>(node);
            activeTabIndex--;
            break;
        }
    }
    if (!prevActiveTab) {
        int numActiveTabs = 0;
        for (cAreaNode *node = areaNodes.First(); node; node = areaNodes.Next(node)) {
            if (node->Execute()) {
                numActiveTabs++;
            }
        }
        for (cAreaNode *node = areaNodes.Last(); node; node = areaNodes.Prev(node)) {
            if (node->Execute()) {
                prevActiveTab = dynamic_cast<cArea*>(node);
                activeTabIndex = numActiveTabs-1;
                break;
            }
        }
    }
    activeTab = prevActiveTab;
}

void cViewDetail::SetTransparency(int transparency, bool forceDetached) {
    if (activeTab)
        activeTab->SetTransparency(transparency);
}

/******************************************************************
* cViewDetailEpg
******************************************************************/
cViewDetailEpg::cViewDetailEpg(void) {
    event = NULL;
    rerunsIndex = -1;
    actorsIndex = -1;
}

cViewDetailEpg::~cViewDetailEpg(void) {
}

void cViewDetailEpg::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{title}", (int)eDmDetailedEpgST::title);
    tokenContainer->DefineStringToken("{shorttext}", (int)eDmDetailedEpgST::shorttext);
    tokenContainer->DefineStringToken("{description}", (int)eDmDetailedEpgST::description);
    tokenContainer->DefineStringToken("{start}", (int)eDmDetailedEpgST::start);
    tokenContainer->DefineStringToken("{stop}", (int)eDmDetailedEpgST::stop);
    tokenContainer->DefineStringToken("{day}", (int)eDmDetailedEpgST::day);
    tokenContainer->DefineStringToken("{date}", (int)eDmDetailedEpgST::date);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eDmDetailedEpgST::durationminutes);
    tokenContainer->DefineStringToken("{vps}", (int)eDmDetailedEpgST::vps);
    tokenContainer->DefineStringToken("{channelname}", (int)eDmDetailedEpgST::channelname);
    tokenContainer->DefineStringToken("{channelid}", (int)eDmDetailedEpgST::channelid);
    tokenContainer->DefineStringToken("{epgpic1path}", (int)eDmDetailedEpgST::epgpic1path);
    tokenContainer->DefineStringToken("{epgpic2path}", (int)eDmDetailedEpgST::epgpic2path);
    tokenContainer->DefineStringToken("{epgpic3path}", (int)eDmDetailedEpgST::epgpic3path);
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
    tokenContainer->DefineIntToken("{daynumeric}", (int)eDmDetailedEpgIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eDmDetailedEpgIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eDmDetailedEpgIT::year);
    tokenContainer->DefineIntToken("{running}", (int)eDmDetailedEpgIT::running);
    tokenContainer->DefineIntToken("{elapsed}", (int)eDmDetailedEpgIT::elapsed);
    tokenContainer->DefineIntToken("{duration}", (int)eDmDetailedEpgIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eDmDetailedEpgIT::durationhours);
    tokenContainer->DefineIntToken("{channelnumber}", (int)eDmDetailedEpgIT::channelnumber);
    tokenContainer->DefineIntToken("{channellogoexists}", (int)eDmDetailedEpgIT::channellogoexists);
    tokenContainer->DefineIntToken("{hasreruns}", (int)eDmDetailedEpgIT::hasreruns);
    tokenContainer->DefineIntToken("{epgpic1available}", (int)eDmDetailedEpgIT::epgpic1available);
    tokenContainer->DefineIntToken("{epgpic2available}", (int)eDmDetailedEpgIT::epgpic2available);
    tokenContainer->DefineIntToken("{epgpic3available}", (int)eDmDetailedEpgIT::epgpic3available);
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
    tokenContainer->DefineLoopToken("{reruns[title]}", (int)eRerunsLT::title);
    tokenContainer->DefineLoopToken("{reruns[shorttext]}", (int)eRerunsLT::shorttext);
    tokenContainer->DefineLoopToken("{reruns[date]}", (int)eRerunsLT::date);
    tokenContainer->DefineLoopToken("{reruns[day]}", (int)eRerunsLT::day);
    tokenContainer->DefineLoopToken("{reruns[start]}", (int)eRerunsLT::start);
    tokenContainer->DefineLoopToken("{reruns[stop]}", (int)eRerunsLT::stop);
    tokenContainer->DefineLoopToken("{reruns[channelname]}", (int)eRerunsLT::channelname);
    tokenContainer->DefineLoopToken("{reruns[channelnumber]}", (int)eRerunsLT::channelnumber);
    tokenContainer->DefineLoopToken("{reruns[channelid]}", (int)eRerunsLT::channelid);
    tokenContainer->DefineLoopToken("{reruns[channellogoexists]}", (int)eRerunsLT::channellogoexists);
    tokenContainer->DefineLoopToken("{actors[name]}", (int)eScraperLT::name);
    tokenContainer->DefineLoopToken("{actors[role]}", (int)eScraperLT::role);
    tokenContainer->DefineLoopToken("{actors[thumb]}", (int)eScraperLT::thumb);
    tokenContainer->DefineLoopToken("{actors[thumbwidth]}", (int)eScraperLT::thumbwidth);
    tokenContainer->DefineLoopToken("{actors[thumbheight]}", (int)eScraperLT::thumbheight);
    rerunsIndex = tokenContainer->LoopIndex("reruns");
    actorsIndex = tokenContainer->LoopIndex("actors");
    InheritTokenContainer();
}

bool cViewDetailEpg::Parse(bool forced) {
    if (!cViewElement::Parse())
        return false;
    if (!event)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::title, event->Title());
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::shorttext, event->ShortText());
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::description, event->Description());
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::start, *(event->GetTimeString()));
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::stop, *(event->GetEndTimeString()));
    time_t startTime = event->StartTime();
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::day, *WeekDayName(startTime));
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::date, *ShortDateString(startTime));
    struct tm * sStartTime = localtime(&startTime);
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::year, sStartTime->tm_year + 1900);
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::daynumeric, sStartTime->tm_mday);
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::month, sStartTime->tm_mon+1);
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
    LOCK_CHANNELS_READ;
    const cChannels* channels = Channels;
#else
    cChannels* channels = &Channels;
#endif
    const cChannel *channel = channels->GetByChannelID(event->ChannelID());
    if (channel) {
        tokenContainer->AddStringToken((int)eDmDetailedEpgST::channelname, channel->Name());
        tokenContainer->AddIntToken((int)eDmDetailedEpgIT::channelnumber, channel->Number());
    }
    cString channelID = event->ChannelID().ToString();
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::channelid, *channelID);
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::channellogoexists, imgCache->LogoExists(*channelID));

    bool isRunning = false;
    time_t now = time(NULL);
    if ((now >= event->StartTime()) && (now <= event->EndTime()))
        isRunning = true;
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::running, isRunning);
    if (isRunning) {
        tokenContainer->AddIntToken((int)eDmDetailedEpgIT::elapsed, (now - event->StartTime())/60);
    } else {
        tokenContainer->AddIntToken((int)eDmDetailedEpgIT::elapsed, 0);
    }
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::duration, event->Duration() / 60);
    tokenContainer->AddIntToken((int)eDmDetailedEpgIT::durationhours, event->Duration() / 3600);
    tokenContainer->AddStringToken((int)eDmDetailedEpgST::durationminutes, *cString::sprintf("%.2d", (event->Duration() / 60)%60));
    if (event->Vps())
        tokenContainer->AddStringToken((int)eDmDetailedEpgST::vps, *event->GetVpsString());

    cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *reruns = LoadReruns();
    int numReruns = NumReruns(reruns);
    vector<int> loopInfo;
    //num reruns
    loopInfo.push_back(numReruns);
    //num actors
    bool scrapInfoAvailable = LoadFullScrapInfo(event, NULL);
    int numActors = NumActors();
    loopInfo.push_back(numActors);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
    if (numReruns > 0) {
        tokenContainer->AddIntToken((int)eDmDetailedEpgIT::hasreruns, 1);
        SetReruns(reruns);
    }
    if (scrapInfoAvailable) {
        SetFullScrapInfo(tokenContainer, actorsIndex);
    }
    SetEpgPictures(event->EventID());
    SetDirty();
    return true;
}

cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *cViewDetailEpg::LoadReruns(void) {
    cPlugin *epgSearchPlugin = cPluginManager::GetPlugin("epgsearch");
    if (!epgSearchPlugin)
        return NULL;

    if (isempty(event->Title()))
        return NULL;

    Epgsearch_searchresults_v1_0 data;
    data.query = (char*)event->Title();
    data.mode = 0;
    data.channelNr = 0;
    data.useTitle = true;
    data.useSubTitle = true;
    data.useDescription = false;

    cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *result = NULL;
    if (epgSearchPlugin->Service("Epgsearch-searchresults-v1.0", &data))
        result = data.pResultList;
    return result;
}

int cViewDetailEpg::NumReruns(cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *reruns) {
    if (!reruns || reruns->Count() < 2)
        return 0;

    int maxNumReruns = config.rerunAmount;
    int rerunDistance = config.rerunDistance * 3600;
    int rerunMaxChannel = config.rerunMaxChannel;

    int i = 0;
    for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = reruns->First(); r && i < maxNumReruns; r = reruns->Next(r)) {
        time_t eventStart = event->StartTime();
        time_t rerunStart = r->event->StartTime();
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_CHANNELS_READ;
        const cChannels* channels = Channels;
#else
        cChannels* channels = &Channels;
#endif
        const cChannel *channel = channels->GetByChannelID(r->event->ChannelID(), true, true);
        //check for identical event
        if ((event->ChannelID() == r->event->ChannelID()) && (eventStart == rerunStart))
            continue;
        //check for timely distance
        if (rerunDistance > 0)
            if (rerunStart - eventStart < rerunDistance)
                continue;
        //check for maxchannel
        if (rerunMaxChannel > 0)
            if (channel && channel->Number() > rerunMaxChannel)
                continue;
        i++;
    }
    return i;
}

void cViewDetailEpg::SetReruns(cList<Epgsearch_searchresults_v1_0::cServiceSearchResult> *reruns) {
    if (!reruns || reruns->Count() < 2)
        return;

    int maxNumReruns = config.rerunAmount;
    int rerunDistance = config.rerunDistance * 3600;
    int rerunMaxChannel = config.rerunMaxChannel;

    int i = 0;
    for (Epgsearch_searchresults_v1_0::cServiceSearchResult *r = reruns->First(); r && i < maxNumReruns; r = reruns->Next(r)) {
        time_t eventStart = event->StartTime();
        time_t rerunStart = r->event->StartTime();
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_CHANNELS_READ;
        const cChannels* channels = Channels;
#else
        cChannels* channels = &Channels;
#endif
        const cChannel *channel = channels->GetByChannelID(r->event->ChannelID(), true, true);
        //check for identical event
        if ((event->ChannelID() == r->event->ChannelID()) && (eventStart == rerunStart))
            continue;
        //check for timely distance
        if (rerunDistance > 0)
            if (rerunStart - eventStart < rerunDistance)
                continue;
        //check for maxchannel
        if (rerunMaxChannel > 0)
            if (channel && channel->Number() > rerunMaxChannel)
                continue;
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::title, r->event->Title());
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::shorttext, r->event->ShortText());
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::start, *(r->event->GetTimeString()));
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::stop, *(r->event->GetEndTimeString()));
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::date, *ShortDateString(r->event->StartTime()));
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::day, *WeekDayName(r->event->StartTime()));
        cString channelID = r->event->ChannelID().ToString();
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::channelid, *channelID);
        bool logoExists = imgCache->LogoExists(*channelID);
        tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::channellogoexists, logoExists ? "1" : "0");
        if (channel) {
            cString channelNumber = cString::sprintf("%d", channel->Number());
            tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::channelname, channel->ShortName(true));
            tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::channelnumber, *channelNumber);
        } else {
            tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::channelname, "");
            tokenContainer->AddLoopToken(rerunsIndex, i, (int)eRerunsLT::channelnumber, "");
        }
        i++;
    }
}

void cViewDetailEpg::SetEpgPictures(int eventId) {
    for (int i=0; i<3; i++) {
        cString epgPic = cString::sprintf("%s%d_%d.jpg", *config.epgImagePath, eventId, i);
        bool epgPicAvailable = FileExists(*epgPic);
        int indexAvailable = (int)eDmDetailedEpgIT::epgpic1available + i;
        int indexPath = (int)eDmDetailedEpgST::epgpic1path + i;
        tokenContainer->AddIntToken(indexAvailable, epgPicAvailable);
        tokenContainer->AddStringToken(indexPath, *epgPic);
    }
}

/******************************************************************
* cViewDetailRec
******************************************************************/
cViewDetailRec::cViewDetailRec(void) {
    recording = NULL;
    actorsIndex = -1;
}

cViewDetailRec::~cViewDetailRec(void) {
}

void cViewDetailRec::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{name}", (int)eDmDetailedRecST::name);
    tokenContainer->DefineStringToken("{epgname}", (int)eDmDetailedRecST::epgname);
    tokenContainer->DefineStringToken("{shorttext}", (int)eDmDetailedRecST::shorttext);
    tokenContainer->DefineStringToken("{description}", (int)eDmDetailedRecST::description);
    tokenContainer->DefineStringToken("{day}", (int)eDmDetailedRecST::day);
    tokenContainer->DefineStringToken("{date}", (int)eDmDetailedRecST::date);
    tokenContainer->DefineStringToken("{time}", (int)eDmDetailedRecST::time);
    tokenContainer->DefineStringToken("{durationminutes}", (int)eDmDetailedRecST::durationminutes);
    tokenContainer->DefineStringToken("{durationeventminutes}", (int)eDmDetailedRecST::durationeventminutes);
    tokenContainer->DefineStringToken("{recchannelname}", (int)eDmDetailedRecST::recchannelname);
    tokenContainer->DefineStringToken("{recchannelid}", (int)eDmDetailedRecST::recchannelid);
    tokenContainer->DefineStringToken("{recordingsize}", (int)eDmDetailedRecST::recordingsize);
    tokenContainer->DefineStringToken("{recordingsizecutted}", (int)eDmDetailedRecST::recordingsizecutted);
    tokenContainer->DefineStringToken("{recordinglength}", (int)eDmDetailedRecST::recordinglength);
    tokenContainer->DefineStringToken("{recordinglengthcutted}", (int)eDmDetailedRecST::recordinglengthcutted);
    tokenContainer->DefineStringToken("{recordingbitrate}", (int)eDmDetailedRecST::recordingbitrate);
    tokenContainer->DefineStringToken("{recordingformat}", (int)eDmDetailedRecST::recordingformat);
    tokenContainer->DefineStringToken("{searchtimer}", (int)eDmDetailedRecST::searchtimer);
    tokenContainer->DefineStringToken("{recimg1path}", (int)eDmDetailedRecST::recimg1path);
    tokenContainer->DefineStringToken("{recimg2path}", (int)eDmDetailedRecST::recimg2path);
    tokenContainer->DefineStringToken("{recimg3path}", (int)eDmDetailedRecST::recimg3path);
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
    tokenContainer->DefineIntToken("{daynumeric}", (int)eDmDetailedRecIT::daynumeric);
    tokenContainer->DefineIntToken("{month}", (int)eDmDetailedRecIT::month);
    tokenContainer->DefineIntToken("{year}", (int)eDmDetailedRecIT::year);
    tokenContainer->DefineIntToken("{duration}", (int)eDmDetailedRecIT::duration);
    tokenContainer->DefineIntToken("{durationhours}", (int)eDmDetailedRecIT::durationhours);
    tokenContainer->DefineIntToken("{durationevent}", (int)eDmDetailedRecIT::durationevent);
    tokenContainer->DefineIntToken("{durationeventhours}", (int)eDmDetailedRecIT::durationeventhours);
    tokenContainer->DefineIntToken("{recchannelnumber}", (int)eDmDetailedRecIT::recchannelnumber);
    tokenContainer->DefineIntToken("{cutted}", (int)eDmDetailedRecIT::cutted);
    tokenContainer->DefineIntToken("{recimg1avaialble}", (int)eDmDetailedRecIT::recimg1avaialble);
    tokenContainer->DefineIntToken("{recimg2avaialble}", (int)eDmDetailedRecIT::recimg2avaialble);
    tokenContainer->DefineIntToken("{recimg3avaialble}", (int)eDmDetailedRecIT::recimg3avaialble);
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

bool cViewDetailRec::Parse(bool forced) {
    if (!cViewElement::Parse())
        return false;
    if (!recording)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDmDetailedRecST::name, recording->Name());
    tokenContainer->AddIntToken((int)eDmDetailedRecIT::cutted, recording->IsEdited());

    const cRecordingInfo *info = recording->Info();
    if (info) {
        tokenContainer->AddStringToken((int)eDmDetailedRecST::epgname, info->Title());
        tokenContainer->AddStringToken((int)eDmDetailedRecST::shorttext, info->ShortText());
        tokenContainer->AddStringToken((int)eDmDetailedRecST::description, info->Description());
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
            recDuration = (recDuration>0) ? (recDuration / 60) : 0;
            tokenContainer->AddStringToken((int)eDmDetailedRecST::date, *recDate);
            tokenContainer->AddStringToken((int)eDmDetailedRecST::time, *recTime);
            time_t startTime = event->StartTime();
            struct tm * sStartTime = localtime(&startTime);
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::year, sStartTime->tm_year + 1900);
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::daynumeric, sStartTime->tm_mday);
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::month, sStartTime->tm_mon+1);
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::duration, recDuration);
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::durationhours, recDuration / 60);
            tokenContainer->AddStringToken((int)eDmDetailedRecST::durationminutes, *cString::sprintf("%.2d", recDuration%60));
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::durationevent, duration);
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::durationeventhours, duration / 60);
            tokenContainer->AddStringToken((int)eDmDetailedRecST::durationeventminutes, *cString::sprintf("%.2d", duration%60));
        }
    }
    SetRecInfos();
    SetRecordingImages(recording->FileName());

    vector<int> loopInfo;
    bool scrapInfoAvailable = LoadFullScrapInfo(NULL, recording);
    int numActors = NumActors();
    loopInfo.push_back(numActors);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
    if (scrapInfoAvailable) {
        SetFullScrapInfo(tokenContainer, actorsIndex);
    }

    SetDirty();
    return true;
}

void cViewDetailRec::SetRecInfos(void) {
    unsigned long long nRecSize = -1;
    unsigned long long nFileSize[1000];
    nFileSize[0] = 0;
    int i = 0;
    struct stat filebuf;
    cString filename;
    int rc = 0;
    do {
        if (recording->IsPesRecording())
            filename = cString::sprintf("%s/%03d.vdr", recording->FileName(), ++i);
        else
            filename = cString::sprintf("%s/%05d.ts", recording->FileName(), ++i);
        rc = stat(filename, &filebuf);
        if (rc == 0)
            nFileSize[i] = nFileSize[i-1] + filebuf.st_size;
        else
            if (ENOENT != errno) {
                nRecSize = -1;
            }
    } while (i <= 999 && !rc);
    nRecSize = nFileSize[i-1];

    cMarks marks;
    bool fHasMarks = marks.Load(recording->FileName(), recording->FramesPerSecond(), recording->IsPesRecording()) && marks.Count();
    cIndexFile *index = new cIndexFile(recording->FileName(), false, recording->IsPesRecording());

    int nCutLength = 0;
    long nCutInFrame = 0;
    unsigned long long nRecSizeCut = nRecSize < 0 ? -1 : 0;
    unsigned long long nCutInOffset = 0;

    if (fHasMarks && index) {
        uint16_t FileNumber;
        off_t FileOffset;

        bool fCutIn = true;
        cMark *mark = marks.First();
        while (mark) {
            int pos = mark->Position();
            index->Get(pos, &FileNumber, &FileOffset); //TODO: will disc spin up?
            if (fCutIn) {
                nCutInFrame = pos;
                fCutIn = false;
                if (nRecSize >= 0)
                    nCutInOffset = nFileSize[FileNumber-1] + FileOffset;
            } else {
                nCutLength += pos - nCutInFrame;
                fCutIn = true;
                if (nRecSize >= 0)
                    nRecSizeCut += nFileSize[FileNumber-1] + FileOffset - nCutInOffset;
            }
            cMark *nextmark = marks.Next(mark);
            mark = nextmark;
        }
        if (!fCutIn) {
            nCutLength += index->Last() - nCutInFrame;
            index->Get(index->Last() - 1, &FileNumber, &FileOffset);
            if (nRecSize >= 0)
                nRecSizeCut += nFileSize[FileNumber-1] + FileOffset - nCutInOffset;
        }
    }

    if (nRecSize < 0) {
        if ((nRecSize = ReadSizeVdr(recording->FileName())) < 0) {
            nRecSize = DirSizeMB(recording->FileName());
        }
    }
    if (nRecSize >= 0) {
        cString strRecSize = "";
        cString strRecSizeCut = "";

        if (fHasMarks) {
            if (nRecSize > MEGABYTE(1023)) {
                strRecSize = cString::sprintf("%.2f GB", (float)nRecSize / MEGABYTE(1024));
                strRecSizeCut = cString::sprintf("%.2f GB", (float)nRecSizeCut / MEGABYTE(1024));
            } else {
                strRecSize = cString::sprintf("%lld MB", nRecSize / MEGABYTE(1));
                strRecSizeCut = cString::sprintf("%lld MB", nRecSizeCut / MEGABYTE(1));
            }
        } else {
            if (nRecSize > MEGABYTE(1023)) {
                strRecSize = cString::sprintf("%.2f GB", (float)nRecSize / MEGABYTE(1024));
                strRecSizeCut = strRecSize;
            } else {
                strRecSize = cString::sprintf("%lld MB", nRecSize / MEGABYTE(1));
                strRecSizeCut = strRecSize;
            }
        }
        tokenContainer->AddStringToken((int)eDmDetailedRecST::recordingsize, *strRecSize);
        tokenContainer->AddStringToken((int)eDmDetailedRecST::recordingsizecutted, *strRecSizeCut);
    }

    const cRecordingInfo *info = recording->Info();
    if (info) {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_CHANNELS_READ;
        const cChannels* channels = Channels;
#else
        cChannels* channels = &Channels;
#endif
        const cChannel *channel = channels->GetByChannelID(info->ChannelID());
        if (channel) {
            tokenContainer->AddStringToken((int)eDmDetailedRecST::recchannelname, channel->Name());
            tokenContainer->AddStringToken((int)eDmDetailedRecST::recchannelid, *channel->GetChannelID().ToString());
            tokenContainer->AddIntToken((int)eDmDetailedRecIT::recchannelnumber, channel->Number());
        }        
    }


    if (index) {
        int nLastIndex = index->Last();
        if (nLastIndex) {
            string strLength = *IndexToHMSF(nLastIndex, false, recording->FramesPerSecond());
            string strLengthCutted = "";
            if (fHasMarks) {
                strLengthCutted = *IndexToHMSF(nCutLength, false, recording->FramesPerSecond());
            } else {
                strLengthCutted = strLength;
            }
            string strBitrate = *cString::sprintf("%.2f MBit/s", (float)nRecSize / nLastIndex * recording->FramesPerSecond() * 8 / MEGABYTE(1));
            tokenContainer->AddStringToken((int)eDmDetailedRecST::recordinglength, strLength.c_str());
            tokenContainer->AddStringToken((int)eDmDetailedRecST::recordinglengthcutted, strLengthCutted.c_str());
            tokenContainer->AddStringToken((int)eDmDetailedRecST::recordingbitrate, strBitrate.c_str());
        }
        delete index;
    }
    
    tokenContainer->AddStringToken((int)eDmDetailedRecST::recordingformat, recording->IsPesRecording() ? "PES" : "TS");
    
    bool searchTimerFound = false;
    if (info) {
        const char *aux = NULL;
        aux = info->Aux();
        if (aux) {
            string strAux = aux;
            string auxEpgsearch = StripXmlTag(strAux, "epgsearch");
            if (!auxEpgsearch.empty()) {
                string searchTimer = StripXmlTag(auxEpgsearch, "searchtimer");
                if (!searchTimer.empty()) {
                    tokenContainer->AddStringToken((int)eDmDetailedRecST::searchtimer, searchTimer.c_str());
                    searchTimerFound = true;
                }
            }
        }
    }
    if (!searchTimerFound)
        tokenContainer->AddStringToken((int)eDmDetailedRecST::searchtimer, "n.a.");
}

int cViewDetailRec::ReadSizeVdr(const char *strPath) {
    int dirSize = -1;
    char buffer[20];
    char *strFilename = NULL;
    if (-1 != asprintf(&strFilename, "%s/size.vdr", strPath)) {
        struct stat st;
        if (stat(strFilename, &st) == 0) {
                int fd = open(strFilename, O_RDONLY);
            if (fd >= 0) {
                if (safe_read(fd, &buffer, sizeof(buffer)) >= 0) {
                    dirSize = atoi(buffer);
                }
                close(fd);
            }
        }
        free(strFilename);
    }
    return dirSize;
}

string cViewDetailRec::StripXmlTag(string &Line, const char *Tag) {
        // set the search strings
        stringstream strStart, strStop;
        strStart << "<" << Tag << ">";
        strStop << "</" << Tag << ">";
        // find the strings
        string::size_type locStart = Line.find(strStart.str());
        string::size_type locStop = Line.find(strStop.str());
        if (locStart == string::npos || locStop == string::npos)
                return "";
        // extract relevant text
        int pos = locStart + strStart.str().size();
        int len = locStop - pos;
        return len < 0 ? "" : Line.substr(pos, len);
}

void cViewDetailRec::SetRecordingImages(const char *recPath) {
    if (!recPath) {
        return;
    }
    DIR *dirHandle;
    struct dirent *dirEntry;
    dirHandle = opendir(recPath);
    if (!dirHandle) {
        return;        
    }
    int picsFound = 0;
    int indexStr = (int)eDmDetailedRecST::recimg1path;
    int indexInt = (int)eDmDetailedRecIT::recimg1avaialble;
    while ( 0 != (dirEntry = readdir(dirHandle))) {
        if (endswith(dirEntry->d_name, "jpg")) {
            tokenContainer->AddStringToken(indexStr, *cString::sprintf("%s/%s", recPath, dirEntry->d_name));
            tokenContainer->AddIntToken(indexInt, 1);
            picsFound++;
        }
        if (picsFound == 3) {
            break;
        }
    }
    closedir(dirHandle);
}

/******************************************************************
* cViewDetailText
******************************************************************/
cViewDetailText::cViewDetailText(void) {
    text = NULL;
}

cViewDetailText::~cViewDetailText(void) {
}

void cViewDetailText::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{text}", (int)eDmDetailedTextST::text);
    InheritTokenContainer();    
}

bool cViewDetailText::Parse(bool forced) {
    if (!cViewElement::Parse())
        return false;
    if (!text)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eDmDetailedTextST::text, text);
    SetDirty();
    return true;
}

/******************************************************************
* cViewDetailPlugin
******************************************************************/
cViewDetailPlugin::cViewDetailPlugin(void) {
}

cViewDetailPlugin::~cViewDetailPlugin(void) {
}

void cViewDetailPlugin::SetTokenContainer(void) {
    if (plugId == -1 || plugMenuId == -1)
        return;
    skindesignerapi::cTokenContainer *tkPlugMenu = plgManager->GetTokenContainer(plugId, plugMenuId);
    tokenContainer = new skindesignerapi::cTokenContainer(*tkPlugMenu);
    InheritTokenContainer();
}

void cViewDetailPlugin::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
}

bool cViewDetailPlugin::Parse(bool forced) {
    if (!cViewElement::Parse())
        return false;
    SetDirty();
    return true;
}

/******************************************************************
* cViewDetailAdvancedPlugin
******************************************************************/
cViewDetailAdvancedPlugin::cViewDetailAdvancedPlugin(int viewId, int plugId) {
    this->plugId = plugId;
    plugViewId = viewId;
}

cViewDetailAdvancedPlugin::~cViewDetailAdvancedPlugin(void) {
}

void cViewDetailAdvancedPlugin::SetTokenContainer(void) {
    if (plugId == -1 || plugViewId == -1)
        return;
    skindesignerapi::cTokenContainer *tkDetail = plgManager->GetTokenContainerTab(plugId, plugViewId);
    tokenContainer = new skindesignerapi::cTokenContainer(*tkDetail);
    InheritTokenContainer();
}

void cViewDetailAdvancedPlugin::Set(skindesignerapi::cTokenContainer *tk) {
    tokenContainer->Clear();
    tokenContainer->SetTokens(tk);
}

bool cViewDetailAdvancedPlugin::Parse(bool forced) {
    if (!cViewElement::Parse())
        return false;
    SetDirty();
    return true;
}
