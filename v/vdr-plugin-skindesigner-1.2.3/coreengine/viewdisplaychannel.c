#include "viewdisplaychannel.h"
#include "../config.h"

/************************************************************************************
* cViewChannel
************************************************************************************/

cViewChannel::cViewChannel(void) {
    veCustomTokens = NULL;
    ClearVariables();
    viewId = eViewType::DisplayChannel;
    viewName = strdup("displaychannel");
    numViewElements = (int)eVeDisplayChannel::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    SetViewElements();
    veMessage = NULL;
    veChannelInfo = NULL;
    veChannelGroup = NULL;
    veEpgInfo = NULL;
    veProgressBar = NULL;
    veStatusInfo = NULL;
    veScraperContent = NULL;
    veEcmInfo = NULL;
#ifdef USE_ZAPCOCKPIT
    veChannelHints = NULL;
    veChannelDetail = NULL;
    veChannelListDetail = NULL;
    veGroupChannelListDetail = NULL;
    channelList = NULL;
    groupList = NULL;
    groupChannelList = NULL;
#endif
}

cViewChannel::~cViewChannel() {
#ifdef USE_ZAPCOCKPIT
    delete channelList;
    delete groupList;
    delete groupChannelList;
#endif
}

void cViewChannel::SetGlobals(cGlobals *globals) {
    cView::SetGlobals(globals);
#ifdef USE_ZAPCOCKPIT
    if (channelList)
        channelList->SetGlobals(globals);
    if (groupList)
        groupList->SetGlobals(globals);
    if (groupChannelList)
        groupChannelList->SetGlobals(globals);
#endif
}

void cViewChannel::PreCache(void) {
    cView::PreCache();
#ifdef USE_ZAPCOCKPIT
    if (channelList) {
        channelList->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
        channelList->PreCache();
    }
    if (groupList) {
        groupList->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
        groupList->PreCache();
    }
    if (groupChannelList) {
        groupChannelList->SetContainer(attribs->X(), attribs->Y(), attribs->Width(), attribs->Height());
        groupChannelList->PreCache();
    }
#endif
    if (viewElements[(int)eVeDisplayChannel::channellistback])
        viewElements[(int)eVeDisplayChannel::channellistback]->UnsetStartAnim();
    if (viewElements[(int)eVeDisplayChannel::grouplistback])
        viewElements[(int)eVeDisplayChannel::grouplistback]->UnsetStartAnim();
    if (viewElements[(int)eVeDisplayChannel::groupchannellistback])
        viewElements[(int)eVeDisplayChannel::groupchannellistback]->UnsetStartAnim();
    SetViewelementsAnimOut();
}

void cViewChannel::AddChannelViewList(const char *listName, cViewList *viewList) {
#ifdef USE_ZAPCOCKPIT
    if (!strcmp(listName, "channellist"))
        channelList = dynamic_cast<cViewListChannelList*>(viewList);
    else if (!strcmp(listName, "grouplist"))
        groupList = dynamic_cast<cViewListGroupList*>(viewList);
    else if (!strcmp(listName, "groupchannellist"))
        groupChannelList = dynamic_cast<cViewListChannelList*>(viewList);
    else
        esyslog("skindesigner: invalid view list %s in displaychannel", listName);
#endif
}

void cViewChannel::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayChannel::background));
    viewElementNames.insert(pair<string, int>("channelgroup", (int)eVeDisplayChannel::channelgroup));
    viewElementNames.insert(pair<string, int>("channelinfo", (int)eVeDisplayChannel::channelinfo));
    viewElementNames.insert(pair<string, int>("epginfo", (int)eVeDisplayChannel::epginfo));
    viewElementNames.insert(pair<string, int>("progressbar", (int)eVeDisplayChannel::progressbar));
    viewElementNames.insert(pair<string, int>("statusinfo", (int)eVeDisplayChannel::statusinfo));
    viewElementNames.insert(pair<string, int>("audioinfo", (int)eVeDisplayChannel::audioinfo));
    viewElementNames.insert(pair<string, int>("ecminfo", (int)eVeDisplayChannel::ecminfo));
    viewElementNames.insert(pair<string, int>("screenresolution", (int)eVeDisplayChannel::screenresolution));
    viewElementNames.insert(pair<string, int>("signalquality", (int)eVeDisplayChannel::signalquality));
    viewElementNames.insert(pair<string, int>("devices", (int)eVeDisplayChannel::devices));
    viewElementNames.insert(pair<string, int>("currentweather", (int)eVeDisplayChannel::currentweather));
    viewElementNames.insert(pair<string, int>("scrapercontent", (int)eVeDisplayChannel::scrapercontent));
    viewElementNames.insert(pair<string, int>("channelhints", (int)eVeDisplayChannel::channelhints));
    viewElementNames.insert(pair<string, int>("channeldetail", (int)eVeDisplayChannel::channeldetail));
    viewElementNames.insert(pair<string, int>("channellistback", (int)eVeDisplayChannel::channellistback));
    viewElementNames.insert(pair<string, int>("channellistdetail", (int)eVeDisplayChannel::channellistdetail));
    viewElementNames.insert(pair<string, int>("grouplistback", (int)eVeDisplayChannel::grouplistback));
    viewElementNames.insert(pair<string, int>("groupchannellistback", (int)eVeDisplayChannel::groupchannellistback));
    viewElementNames.insert(pair<string, int>("groupchannellistdetail", (int)eVeDisplayChannel::groupchannellistdetail));
    viewElementNames.insert(pair<string, int>("datetime", (int)eVeDisplayChannel::datetime));
    viewElementNames.insert(pair<string, int>("time", (int)eVeDisplayChannel::time));
    viewElementNames.insert(pair<string, int>("message", (int)eVeDisplayChannel::message));
    viewElementNames.insert(pair<string, int>("customtokens", (int)eVeDisplayChannel::customtokens));
}

void cViewChannel::SetViewElementObjects(void) {
    for (int i = 0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        if (dynamic_cast<cVeMessage*>(viewElements[i])) 
        {
            veMessage = dynamic_cast<cVeMessage*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcChannelInfo*>(viewElements[i])) {

            veChannelInfo = dynamic_cast<cVeDcChannelInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcChannelGroup*>(viewElements[i])) 
        {
            veChannelGroup = dynamic_cast<cVeDcChannelGroup*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcEpgInfo*>(viewElements[i])) 
        {
            veEpgInfo = dynamic_cast<cVeDcEpgInfo*>(viewElements[i]);
            veEpgInfo->SetGlobalTimers(&globalTimers);
        }
        else if (dynamic_cast<cVeDcProgressBar*>(viewElements[i])) 
        {
            veProgressBar = dynamic_cast<cVeDcProgressBar*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcStatusInfo*>(viewElements[i])) 
        {
            veStatusInfo = dynamic_cast<cVeDcStatusInfo*>(viewElements[i]);
            veStatusInfo->SetGlobalTimers(&globalTimers);
        }
        else if (dynamic_cast<cVeDcScraperContent*>(viewElements[i])) 
        {
            veScraperContent = dynamic_cast<cVeDcScraperContent*>(viewElements[i]);
        }
#ifdef USE_ZAPCOCKPIT
        else if (dynamic_cast<cVeDcChannelListDetail*>(viewElements[i])) 
        {
            veChannelListDetail = dynamic_cast<cVeDcChannelListDetail*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcGroupChannelListDetail*>(viewElements[i])) 
        {
            veGroupChannelListDetail = dynamic_cast<cVeDcGroupChannelListDetail*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcChannelHints*>(viewElements[i])) 
        {
            veChannelHints = dynamic_cast<cVeDcChannelHints*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDcChannelDetail*>(viewElements[i])) 
        {
            veChannelDetail = dynamic_cast<cVeDcChannelDetail*>(viewElements[i]);
        }
#endif
        else if (dynamic_cast<cVeDcEcmInfo*>(viewElements[i])) 
        {
            veEcmInfo = dynamic_cast<cVeDcEcmInfo*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeCustomTokens*>(viewElements[i])) 
        {
            veCustomTokens = dynamic_cast<cVeCustomTokens*>(viewElements[i]);
        }
        else if (dynamic_cast<cVeDevices*>(viewElements[i])) 
        {
            viewElements[i]->SetDetached();
        }
    }
}

void cViewChannel::ClearVariables(void) {
    cView::ClearVariables();
#ifdef USE_ZAPCOCKPIT
    viewType = dcDefault;
    viewTypeLast = dcDefault;
    initExtended = true;
    displayList = false;
    initList = false;
    channelInput = false;
#endif
    channelChange = false;
    mode = dmDefault;
    if (veCustomTokens)
        veCustomTokens->Reset();
    timersLoaded = false;
    globalTimers.ClearTimers();
}

void cViewChannel::SetChannel(const cChannel *channel, int number) {
    if (!timersLoaded) {
        timersLoaded = true;
        globalTimers.LoadTimers();
    }
    channelChange = true;

    //check if channelgroups have to be cleared
    bool clearChannelGroups = (mode == dmChannelGroups) ? true : false;
    mode = dmDefault;

#ifdef USE_ZAPCOCKPIT
    //check if channel hints have to be cleared
    if (number) {
        channelInput = true;
    } else if (channelInput) {
        channelInput = false;
        if (veChannelHints)
            veChannelHints->Close();
    }
    if (veChannelDetail)
        veChannelDetail->Clear(true);
#endif

    if (veChannelInfo) {
        veChannelInfo->Clear();
        veChannelInfo->Set(channel, number);
    }

    if (channel) {
        if (!channel->GroupSep()) {
            if (clearChannelGroups)
                Clear((int)eVeDisplayChannel::channelgroup);
            if (veStatusInfo)
                veStatusInfo->Set(channel);
            if (veEcmInfo)
                veEcmInfo->Set(channel);
        } else {
            mode = dmChannelGroups;
            ClearBasic(false);
            if (veChannelGroup)
                veChannelGroup->Set(channel);
        }
    }
}

void cViewChannel::SetEvents(const cEvent *present, const cEvent *following) {
    if (!timersLoaded) {
        timersLoaded = true;
        globalTimers.LoadTimers();
    }
    Clear((int)eVeDisplayChannel::epginfo);
    Clear((int)eVeDisplayChannel::progressbar);
    Clear((int)eVeDisplayChannel::scrapercontent);
    if (veProgressBar)
        veProgressBar->Set(present);

    if (!present && !following)
        return;
    
    if (veEpgInfo)
        veEpgInfo->Set(present, following);

    if (veScraperContent)
        veScraperContent->Set(present);
}

void cViewChannel::SetMessage(eMessageType type, const char *text) {
    if (veMessage) {
        if (text)
            veMessage->Set(type, text);
        else
            veMessage->Clear();
    }
}

#ifdef USE_ZAPCOCKPIT

void cViewChannel::SetViewType(eDisplaychannelView viewType) {
    viewTypeLast = this->viewType;
    this->viewType = viewType;
}

int cViewChannel::MaxItems(void) {
    initList = true;
    if (viewType == dcChannelList && channelList)
        return channelList->NumItems();
    else if (viewType == dcGroupsList && groupList)
        return groupList->NumItems();
    else if (viewType == dcGroupsChannelList && groupChannelList)
        return groupChannelList->NumItems();
    return 0;
}

bool cViewChannel::KeyRightOpensChannellist(void) {
    if (channelList) {
        if (channelList->Button() == eButtonType::left)
            return false;
        else if (channelList->Button() == eButtonType::right)
            return true;
    }
    if (groupList) {
        if (groupList->Button() == eButtonType::left)
            return true;
        else if (groupList->Button() == eButtonType::right)
            return false;
    }
    return true;
}

void cViewChannel::SetChannelInfo(const cChannel *channel) {
    if (!channel)
        return;
    if (viewType == dcChannelInfo && veChannelDetail) {
        ClearOnDisplay();
        veChannelDetail->Set(channel);
        Render((int)eVeDisplayChannel::channeldetail);
    } else if (viewType == dcChannelListInfo && veChannelListDetail) {
        veChannelListDetail->Set(channel);
        Render((int)eVeDisplayChannel::channellistdetail);
    } else if (viewType == dcGroupsChannelListInfo && veGroupChannelListDetail) {
        veGroupChannelListDetail->Set(channel);
        Render((int)eVeDisplayChannel::groupchannellistdetail);
    }
}

void cViewChannel::SetChannelList(const cChannel *channel, int index, bool current) {
    displayList = true;
    if (viewType == dcChannelList && channelList) {
        channelList->Set(channel, index, current);
    } else if (viewType == dcGroupsChannelList && groupChannelList) {
        groupChannelList->Set(channel, index, current);
    }
}

void cViewChannel::SetGroupList(const char *group, int numChannels, int index, bool current) {
    displayList = true;
    if (groupList) {
        groupList->Set(group, numChannels, index, current);  
    }
}

void cViewChannel::ClearList(void) {
    if (viewType == dcChannelList && channelList)
        channelList->Clear();
    if (viewType == dcGroupsList && groupList)
        groupList->Clear();
    if (viewType == dcGroupsChannelList && groupChannelList)
        groupChannelList->Clear();
}

void cViewChannel::SetNumChannelHints(int num) {
    if (veChannelHints)
        veChannelHints->SetNumHints(num);
    channelHints = true;
}

void cViewChannel::SetChannelHint(const cChannel *channel) {
    if (veChannelHints)
        veChannelHints->SetHint(channel);
}

#endif //USE_ZAPCOCKPIT

void cViewChannel::Close(void) {
    bool doAnim = true;
#ifdef USE_ZAPCOCKPIT
    if (viewType != dcDefault || viewTypeLast != dcDefault)
        doAnim = false;
#endif
    if (doAnim) {
        animator->Stop();
        animator->Finish();    
    }
    delete animator;
    animator = NULL;
    UnScaleTv();
    ClearVariables();
    for (int i=0; i < numViewElements; i++) {
        if (!viewElements[i])
            continue;
        viewElements[i]->Close();
    }
#ifdef USE_ZAPCOCKPIT
    if (channelList)
        channelList->Close();
    if (groupList)
        groupList->Close();
    if (groupChannelList)
        groupChannelList->Close();
#endif
    sdOsd.Flush();
    sdOsd.DeleteOsd();
}

void cViewChannel::ClearBasic(bool clearBackground) {
    if (clearBackground) {
        Clear((int)eVeDisplayChannel::background, true);
        Clear((int)eVeDisplayChannel::currentweather, true);
    }
    Clear((int)eVeDisplayChannel::channelinfo, true);
    Clear((int)eVeDisplayChannel::epginfo, true);
    Clear((int)eVeDisplayChannel::statusinfo, true);
    Clear((int)eVeDisplayChannel::scrapercontent, true);
    Clear((int)eVeDisplayChannel::progressbar, true);
    Clear((int)eVeDisplayChannel::screenresolution, true);
    Clear((int)eVeDisplayChannel::signalquality, true);
    Clear((int)eVeDisplayChannel::audioinfo, true);
    Clear((int)eVeDisplayChannel::ecminfo, true);
    Clear((int)eVeDisplayChannel::devices, true);
    Clear((int)eVeDisplayChannel::customtokens, true);
}

void cViewChannel::ClearExtended(void) {
#ifdef USE_ZAPCOCKPIT
    //exit from channel list
    if ((viewTypeLast == dcChannelList || viewTypeLast == dcChannelListInfo) && viewType == dcDefault && channelList) {
        channelList->Clear();
        Clear((int)eVeDisplayChannel::channellistback);
        Clear((int)eVeDisplayChannel::channellistdetail);
        SetDirty((int)eVeDisplayChannel::channellistback);
        initExtended = true;
        ShowBasic();
    }
    //exit from channellist info to channel list
    if (viewTypeLast == dcChannelListInfo && viewType == dcChannelList) {
        Clear((int)eVeDisplayChannel::channellistdetail);
        SetDirty((int)eVeDisplayChannel::channellistdetail);
    }
    //exit from group list
    if (viewTypeLast == dcGroupsList && viewType == dcDefault && groupList) {
        groupList->Clear();
        Clear((int)eVeDisplayChannel::grouplistback);
        SetDirty((int)eVeDisplayChannel::grouplistback);
        initExtended = true;
        ShowBasic();
    }
    //exit from group channel list to group list
    if (viewTypeLast == dcGroupsChannelList && viewType == dcGroupsList && groupChannelList) {
        groupChannelList->Clear();
        Clear((int)eVeDisplayChannel::groupchannellistback);
        SetDirty((int)eVeDisplayChannel::groupchannellistback);
    }
    //exit from groupchannellist info to groupchannel list
    if (viewTypeLast == dcGroupsChannelListInfo && viewType == dcGroupsChannelList) {
        Clear((int)eVeDisplayChannel::groupchannellistdetail);
        SetDirty((int)eVeDisplayChannel::groupchannellistdetail);
    }
    //exit from channel list
    if ((viewTypeLast == dcGroupsChannelList || viewTypeLast == dcGroupsChannelListInfo) && viewType == dcDefault && groupList && groupChannelList) {
        groupList->Clear();
        groupChannelList->Clear();
        Clear((int)eVeDisplayChannel::grouplistback);
        Clear((int)eVeDisplayChannel::groupchannellistback);
        Clear((int)eVeDisplayChannel::groupchannellistdetail);
        SetDirty((int)eVeDisplayChannel::groupchannellistback);
        SetDirty((int)eVeDisplayChannel::grouplistback);
        SetDirty((int)eVeDisplayChannel::groupchannellistdetail);
        initExtended = true;
        ShowBasic();
    }
    //exit from channel info to channel or group list
    if (viewTypeLast == dcChannelInfo && (viewType == dcChannelList || viewType == dcGroupsList)) {
        Clear((int)eVeDisplayChannel::channeldetail, true);
    }
    viewTypeLast = dcDefault;
#endif
}

void cViewChannel::ClearOnDisplay(void) {
#ifdef USE_ZAPCOCKPIT
    if (viewType == dcChannelInfo && veChannelDetail) {
        vector<int> clear = veChannelDetail->GetClearOnDisplay();
        for (vector<int>::iterator ve = clear.begin(); ve != clear.end(); ve++) {
            Hide(*ve);
        }
    } else {
        cViewElement *listBack = NULL;
        if (viewType == dcChannelList)
            listBack = viewElements[(int)eVeDisplayChannel::channellistback];
        else if (viewType == dcGroupsList)
            listBack = viewElements[(int)eVeDisplayChannel::grouplistback];
        if (!listBack)
            return;
        if (listBack->DoClearAll()) {
            HideBasic();
            return;
        }
        vector<int> clear = listBack->GetClearOnDisplay();
        for (vector<int>::iterator ve = clear.begin(); ve != clear.end(); ve++) {
            Hide(*ve);
        }
    }
#endif
}

void cViewChannel::DrawBasic(bool initial) {
    if (initial) {
        Render((int)eVeDisplayChannel::background);
        Render((int)eVeDisplayChannel::currentweather);
    }
    Render((int)eVeDisplayChannel::channelinfo);
    Render((int)eVeDisplayChannel::epginfo);
    Render((int)eVeDisplayChannel::statusinfo);
    Render((int)eVeDisplayChannel::scrapercontent);
    Render((int)eVeDisplayChannel::progressbar, channelChange);
    Render((int)eVeDisplayChannel::screenresolution);
    Render((int)eVeDisplayChannel::signalquality);
    Render((int)eVeDisplayChannel::audioinfo);
    Render((int)eVeDisplayChannel::ecminfo);
    Render((int)eVeDisplayChannel::devices);
    Render((int)eVeDisplayChannel::customtokens);
    Render((int)eVeDisplayChannel::message);

#ifdef USE_ZAPCOCKPIT
    if (channelHints) {
        channelHints = false;
        if (veChannelHints) {
            if (veChannelHints->Active())
                veChannelHints->Close();
            veChannelHints->Parse();
            veChannelHints->Render();            
        }
    }
#endif
}

void cViewChannel::HideBasic(void) {
    for (int ve = (int)eVeDisplayChannel::background; ve <= (int)eVeDisplayChannel::ecminfo; ve++) {
        Hide(ve);
    }
}

void cViewChannel::ShowBasic(void) {
    for (int ve = (int)eVeDisplayChannel::background; ve <= (int)eVeDisplayChannel::ecminfo; ve++) {
        Show(ve);
    }
}

void cViewChannel::DrawExtended(void) {
#ifdef USE_ZAPCOCKPIT
    if (!displayList)
        return;
    if (initExtended) {
        ClearOnDisplay();
        initExtended = false;
    }
    if (viewType == dcChannelList && channelList) {
        Render((int)eVeDisplayChannel::channellistback);
        channelList->Draw(mcUndefined);
        if (initList)
            channelList->StartAnimation();
    } else if (viewType == dcGroupsList && groupList) {
        Render((int)eVeDisplayChannel::grouplistback);
        groupList->Draw(mcUndefined);
        if (initList)
            groupList->StartAnimation();
    } else if (viewType == dcGroupsChannelList && groupChannelList) {
        Render((int)eVeDisplayChannel::groupchannellistback);
        groupChannelList->Draw(mcUndefined);
        if (initList)
            groupChannelList->StartAnimation();
    }
    displayList = false;
    initList = false;
#endif
}

void cViewChannel::Flush(void) {
#ifdef USE_ZAPCOCKPIT
    ClearExtended();
#endif

#ifdef USE_ZAPCOCKPIT
    if (viewType < dcChannelList) {
#endif
    //Basic Display Handling
    if (mode == dmDefault) {
        DrawBasic(init);
    } else if (mode == dmChannelGroups) {
        if (init) {
            Render((int)eVeDisplayChannel::background);
            Render((int)eVeDisplayChannel::currentweather);
        }
        Render((int)eVeDisplayChannel::channelgroup);
    }
    Render((int)eVeDisplayChannel::datetime);
    Render((int)eVeDisplayChannel::time);
#ifdef USE_ZAPCOCKPIT
    }
#endif
    channelChange = false;

#ifdef USE_ZAPCOCKPIT
    DrawExtended();
#endif

    cView::Flush();
}
