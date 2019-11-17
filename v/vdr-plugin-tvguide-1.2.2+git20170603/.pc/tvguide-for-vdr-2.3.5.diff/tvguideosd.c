#include <stdlib.h>
#include <vector>

#include "config.h"
#include "services/epgsearch.h"
#include "services/remotetimers.h"

#include "tools.h"
#include "setup.h"

#include "tvguideosd.h"

cTvGuideOsd::cTvGuideOsd(void) {
    detailView = NULL;
    detailViewActive = false;
    activeGrid = NULL;
    timeLine = NULL;
    recMenuManager = NULL;
    channelJumper = NULL;
}

cTvGuideOsd::~cTvGuideOsd() {
    delete myTime;
    columns.Clear();
    if (tvguideConfig.displayStatusHeader) {
        delete statusHeader;
    }
    if (detailView)
        delete detailView;
    delete timeLine;
    delete channelGroups;
    delete footer;
    delete recMenuManager;
    if (channelJumper)
        delete channelJumper;
    osdManager.deleteOsd();
}

void cTvGuideOsd::Show(void) {
    int start = cTimeMs::Now();
    bool ok = false;
    ok = osdManager.setOsd();
    if (ok) {
        bool themeChanged = tvguideConfig.LoadTheme();
        tvguideConfig.SetStyle();
        tvguideConfig.setDynamicValues();
        bool geoChanged = geoManager.SetGeometry(cOsd::OsdWidth(), cOsd::OsdHeight());
        if (themeChanged || geoChanged) {
            fontManager.DeleteFonts();
            fontManager.SetFonts();
            imgCache.Clear();
            imgCache.CreateCache();
        }
        osdManager.setBackground();
        myTime = new cMyTime();
        myTime->Now();
        SwitchTimers.Load(AddDirectory(cPlugin::ConfigDirectory("epgsearch"), "epgsearchswitchtimers.conf"));
        recMenuManager = new cRecMenuManager();
        pRemoteTimers = cPluginManager::CallFirstService("RemoteTimers::RefreshTimers-v1.0", NULL);
        if (pRemoteTimers) {
            isyslog("tvguide: remotetimers-plugin is available");
        }
        if (tvguideConfig.useRemoteTimers && pRemoteTimers) {
            cString errorMsg;
            if (!pRemoteTimers->Service("RemoteTimers::RefreshTimers-v1.0", &errorMsg)) {
                esyslog("tvguide: %s", *errorMsg);
            }
        }
        drawOsd();
    }
    esyslog("tvguide: Rendering took %d ms", int(cTimeMs::Now()-start));
}

void cTvGuideOsd::drawOsd() {
    cPixmap::Lock();
    cChannel *startChannel = Channels.GetByNumber(cDevice::CurrentChannel());
    int numBack = tvguideConfig.numGrids / 2;
    int offset = 0;
    const cChannel *newStartChannel = startChannel;
    for (; newStartChannel ; newStartChannel = Channels.Prev(newStartChannel)) {
        if (newStartChannel && !newStartChannel->GroupSep()) {
            offset++;
        }
        if (offset == numBack)
            break;
    }
    if (!newStartChannel)
        newStartChannel = Channels.First();
    offset--;
    if (offset < 0)
        offset = 0;
    
    if (tvguideConfig.displayStatusHeader) {
        statusHeader = new cStatusHeader();
        statusHeader->Draw();
        statusHeader->ScaleVideo();
    }
    timeLine = new cTimeLine(myTime);
    timeLine->drawDateViewer();
    timeLine->drawTimeline();
    timeLine->drawClock();
    channelGroups = new cChannelGroups();
    channelGroups->ReadChannelGroups();
    footer = new cFooter(channelGroups);
    recMenuManager->SetFooter(footer);
    footer->drawRedButton();
    if (tvguideConfig.channelJumpMode == eNumJump) {
        footer->drawGreenButton();
        footer->drawYellowButton();
    }
    footer->drawBlueButton(false);
    osdManager.flush();
    readChannels(newStartChannel);
    drawGridsChannelJump(offset);
    osdManager.flush();
    cPixmap::Unlock();
}

void cTvGuideOsd::readChannels(const cChannel *channelStart) {
    int i=0;
    bool foundEnough = false;
    columns.Clear();
    if (!channelStart)
        return;
    for (const cChannel *channel = channelStart; channel; channel = Channels.Next(channel)) {
        if (!channel->GroupSep()) {
            if (channelGroups->IsInLastGroup(channel)) {
                break;
            }
            cChannelColumn *column = new cChannelColumn(i, channel, myTime);
            if (column->readGrids()) {
                columns.Add(column);
                i++;
            } else {
                delete column;
            }
        }
        if (i == tvguideConfig.numGrids) {
            foundEnough = true;
            break;
        }
    }
    if (!foundEnough) {
        int numCurrent = columns.Count();
        int numBack = tvguideConfig.numGrids - numCurrent;
        int newChannelNumber = columns.First()->getChannel()->Number() - numBack;
        const cChannel *newStart = Channels.GetByNumber(newChannelNumber);
        readChannels(newStart);
    }
}

void cTvGuideOsd::drawGridsChannelJump(int offset) {
    if (columns.Count() == 0)
        return;
    activeGrid = columns.Get(offset)->getActive();
    if (activeGrid)
        activeGrid->SetActive();
    if (tvguideConfig.displayStatusHeader) {
        statusHeader->DrawInfoText(activeGrid);
    }
    if (activeGrid && (tvguideConfig.channelJumpMode == eGroupJump)) {
        footer->UpdateGroupButtons(activeGrid->column->getChannel());
    }
    if (tvguideConfig.displayChannelGroups) {
        channelGroups->DrawChannelGroups(columns.First()->getChannel(), columns.Last()->getChannel());
    }
    for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
        column->createHeader();
        column->drawGrids();
    }
}

void cTvGuideOsd::drawGridsTimeJump() {
    if (columns.Count() == 0)
        return;
    cChannelColumn *colActive = NULL;
    if (activeGrid) {
        colActive = activeGrid->column;
    } else {
        colActive = columns.First();
    }
    for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
        column->clearGrids();
        column->readGrids();
        column->drawGrids();
    }
    activeGrid = colActive->getActive();
    if (activeGrid) {
        activeGrid->SetActive();
        activeGrid->Draw();
        if (tvguideConfig.displayStatusHeader) {
            statusHeader->DrawInfoText(activeGrid);
        }
    }
}

void cTvGuideOsd::setNextActiveGrid(cGrid *next) {
    if (!next || !activeGrid) {
        return;
    }
    activeGrid->SetInActive();
    activeGrid->Draw(); 
    activeGrid = next;
    activeGrid->SetActive();
    activeGrid->Draw();
    if (tvguideConfig.displayStatusHeader) {
        statusHeader->DrawInfoText(activeGrid);
    }
}

void cTvGuideOsd::channelForward() {
    cChannelColumn *colRight = columns.Next(activeGrid->column);
    bool colAdded = false;
    if (!colRight) {
        const cChannel *channelRight = activeGrid->column->getChannel();
        while (channelRight = Channels.Next(channelRight)) {
            if (!channelRight->GroupSep()) {
                if (channelGroups->IsInLastGroup(channelRight)) {
                    break;
                }
                colRight = new cChannelColumn(tvguideConfig.numGrids - 1, channelRight, myTime);
                if (colRight->readGrids()) {
                    break;
                } else {
                    delete colRight;
                    colRight = NULL;
                }
            }
        }
        if (colRight) {
            colAdded = true;
            if (columns.Count() == tvguideConfig.numGrids) {
                cChannelColumn *cFirst = columns.First();
                columns.Del(cFirst);
            }
            for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
                column->SetNum(column->GetNum() - 1);
                column->drawHeader();
                column->drawGrids();
            }
            columns.Add(colRight);
            colRight->createHeader();
            colRight->drawGrids();
        }
    }
    if (colRight) {
        cGrid *right = colRight->getNeighbor(activeGrid);
        if (right) {
            setNextActiveGrid(right);
        }
    }
    if (tvguideConfig.displayChannelGroups && colAdded) {
        channelGroups->DrawChannelGroups(columns.First()->getChannel(), columns.Last()->getChannel());
    }
    if (activeGrid && (tvguideConfig.channelJumpMode == eGroupJump)) {
        footer->UpdateGroupButtons(activeGrid->column->getChannel());
    }
    osdManager.flush();
}

void cTvGuideOsd::channelBack() {
    cChannelColumn *colLeft = columns.Prev(activeGrid->column);
    bool colAdded = false;
    if (!colLeft) {
        const cChannel *channelLeft = activeGrid->column->getChannel();
        while (channelLeft = Channels.Prev(channelLeft)) {
            if (!channelLeft->GroupSep()) {
                colLeft = new cChannelColumn(0, channelLeft, myTime);
                if (colLeft->readGrids()) {
                    break;
                } else {
                    delete colLeft;
                    colLeft = NULL;
                }
            }
        }
        if (colLeft) {
            colAdded = true;
            if (columns.Count() == tvguideConfig.numGrids) {
                cChannelColumn *cLast = columns.Last();
                columns.Del(cLast);
            }
            for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
                column->SetNum(column->GetNum() + 1);
                column->drawHeader();
                column->drawGrids();
            }
            columns.Ins(colLeft, columns.First());
            colLeft->createHeader();
            colLeft->drawGrids();
        }
    }

    if (colLeft) {
        cGrid *left = colLeft->getNeighbor(activeGrid);
        if (left) {
            setNextActiveGrid(left);
        }
    }
    if (tvguideConfig.displayChannelGroups && colAdded) {
        channelGroups->DrawChannelGroups(columns.First()->getChannel(), columns.Last()->getChannel());
    }

    if (activeGrid && (tvguideConfig.channelJumpMode == eGroupJump)) {
        footer->UpdateGroupButtons(activeGrid->column->getChannel());
    }
    osdManager.flush();
}

void cTvGuideOsd::timeForward() {
    bool actionDone = false;
    if ( (myTime->GetEnd() - activeGrid->EndTime())/60 < 30 ) {
        ScrollForward();
        actionDone = true;
    }
    cGrid *next = activeGrid->column->getNext(activeGrid);
    if (next) {
        if (   (next->EndTime() < myTime->GetEnd())
            || ( (myTime->GetEnd() - next->StartTime())/60 > 30 ) ) {
            setNextActiveGrid(next);
            actionDone = true;
        }
    }
    if (!actionDone) {
        ScrollForward();
    }
    osdManager.flush();
}

void cTvGuideOsd::ScrollForward() {
    myTime->AddStep(tvguideConfig.stepMinutes);
    timeLine->drawDateViewer();
    timeLine->drawClock();
    timeLine->setTimeline();
    for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
        column->AddNewGridsAtEnd();
        column->ClearOutdatedStart();
        column->drawGrids();
    }
}

void cTvGuideOsd::timeBack() {
    bool actionDone = false;
    if ( (activeGrid->StartTime() - myTime->GetStart())/60 < 30 ) {
        ScrollBack();
        actionDone = true;
    }
    cGrid *prev = activeGrid->column->getPrev(activeGrid);
    if (prev) {
        if (   (prev->StartTime() > myTime->GetStart())
            || ( (prev->EndTime() - myTime->GetStart())/60 > 30 )
            || ( prev->isFirst()) ) {
            setNextActiveGrid(prev);
            actionDone = true;
        }
    }
    if (!actionDone) {
        ScrollBack();
    }
    osdManager.flush();
}

void cTvGuideOsd::ScrollBack() {
    bool tooFarInPast = myTime->DelStep(tvguideConfig.stepMinutes);
    if (tooFarInPast)
        return;
    timeLine->drawDateViewer();
    timeLine->drawClock();
    timeLine->setTimeline();
    for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
        column->AddNewGridsAtStart();
        column->ClearOutdatedEnd();
        column->drawGrids();
    }   
}

void cTvGuideOsd::processKeyUp() {
    if (!activeGrid) {
        return;
    }
    if (tvguideConfig.displayMode == eVertical) {
        timeBack();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        channelBack();
    }
}

void cTvGuideOsd::processKeyDown() {
    if (!activeGrid) {
            return;
    }
    if (tvguideConfig.displayMode == eVertical) {
        timeForward();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        channelForward();
    }
}

void cTvGuideOsd::processKeyLeft() {
    if (activeGrid == NULL)
        return;
    if (tvguideConfig.displayMode == eVertical) {
        channelBack();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeBack();
    }
}

void cTvGuideOsd::processKeyRight() {
    if (activeGrid == NULL)
        return;
    if (tvguideConfig.displayMode == eVertical) {
        channelForward();
    } else if (tvguideConfig.displayMode == eHorizontal) {
        timeForward();
    }
}

void cTvGuideOsd::processKeyRed() {
    if  ((activeGrid == NULL) || activeGrid->isDummy())
        return;
    recMenuManager->Start(activeGrid->GetEvent());
}

void cTvGuideOsd::processKeyGreen() {
    if (activeGrid == NULL)
        return;
    
    const cChannel *currentChannel = activeGrid->column->getChannel();
    const cChannel *firstChannel = columns.First()->getChannel();
    int currentCol = activeGrid->column->GetNum();
    const cChannel *prev = NULL;
    
    if (tvguideConfig.channelJumpMode == eGroupJump) {
        int prevNum = channelGroups->GetPrevGroupChannelNumber(currentChannel);
        if (prevNum) {
            prev = Channels.GetByNumber(prevNum);
        }    
    } else if (tvguideConfig.channelJumpMode == eNumJump) {
        int i = tvguideConfig.jumpChannels + 1;
        for (const cChannel *channel = firstChannel; channel; channel = Channels.Prev(channel)) {
            if (!channel->GroupSep()) {
                prev = channel;
                i--;
            }
            if (i == 0)
                break;
        }
    }
    if (prev) {
        readChannels(prev);
        if (columns.Count() > 0) {
            if (tvguideConfig.channelJumpMode == eGroupJump)
                drawGridsChannelJump();
            else
                drawGridsChannelJump(currentCol);
        }
        osdManager.flush();
    }
}

void cTvGuideOsd::processKeyYellow() {
    if (activeGrid == NULL)
        return;
    const cChannel *currentChannel = activeGrid->column->getChannel();
    int currentCol = activeGrid->column->GetNum();
    const cChannel *firstChannel = columns.First()->getChannel();
    const cChannel *next = NULL;
    
    if (tvguideConfig.channelJumpMode == eGroupJump) {
        int nextNum = channelGroups->GetNextGroupChannelNumber(currentChannel);
        if (nextNum) {
            next = Channels.GetByNumber(nextNum);
        }    
    } else if (tvguideConfig.channelJumpMode == eNumJump) {
        int i=0;
        for (const cChannel *channel = firstChannel; channel; channel = Channels.Next(channel)) {
            if (channelGroups->IsInLastGroup(channel)) {
                break;
            }
            if (!channel->GroupSep()) {
                next = channel;
                i++;
            }
            if (i == (tvguideConfig.jumpChannels+1)) {
                break;
            }
        }
    }
    if (next) {
        readChannels(next);
        if (columns.Count() > 0) {
            if (tvguideConfig.channelJumpMode == eGroupJump)
                drawGridsChannelJump();
            else
                drawGridsChannelJump(currentCol);
        }
        osdManager.flush();
    }
}

eOSState cTvGuideOsd::processKeyBlue(bool *alreadyUnlocked) {
    if (tvguideConfig.blueKeyMode == eBlueKeySwitch) {
        return ChannelSwitch(alreadyUnlocked);
    } else if (tvguideConfig.blueKeyMode == eBlueKeyEPG) {
        DetailedEPG();
    } else if (tvguideConfig.blueKeyMode == eBlueKeyFavorites) {
        recMenuManager->StartFavorites();
    }
    return osContinue;
}

eOSState cTvGuideOsd::processKeyOk(bool *alreadyUnlocked) {
    if (tvguideConfig.blueKeyMode == eBlueKeySwitch) {
        DetailedEPG();
    } else if (tvguideConfig.blueKeyMode == eBlueKeyEPG) {
        return ChannelSwitch(alreadyUnlocked);
    } else if (tvguideConfig.blueKeyMode == eBlueKeyFavorites) {
        DetailedEPG();
    }
    return osContinue;
}

eOSState cTvGuideOsd::ChannelSwitch(bool *alreadyUnlocked) {
    if (activeGrid == NULL)
        return osContinue;
    const cChannel *currentChannel = activeGrid->column->getChannel();
    if (currentChannel) {
        cPixmap::Unlock();
        *alreadyUnlocked = true;
        cDevice::PrimaryDevice()->SwitchChannel(currentChannel, true);
        if (tvguideConfig.closeOnSwitch) {
            if (detailView) {
                delete detailView;
                detailView = NULL;
                detailViewActive = false;
            }
            return osEnd;
        }
    }
    return osContinue;
}

void cTvGuideOsd::DetailedEPG() {
    if (!activeGrid->isDummy()) {
        detailViewActive = true;
        detailView = new cDetailView(activeGrid->GetEvent(), footer);
        footer->SetDetailedViewMode();
        osdManager.flush();
        detailView->Start();
        osdManager.flush();
    }
}

void cTvGuideOsd::processNumKey(int numKey) {
    if (tvguideConfig.numkeyMode == 0) {
        //timely jumps with 1,3,4,6,7,9
        TimeJump(numKey);
    } else {
        //jump to specific channel
        ChannelJump(numKey);
    }
}

void cTvGuideOsd::TimeJump(int mode) {
    switch (mode) {
        case 1: {
            bool tooFarInPast = myTime->DelStep(tvguideConfig.bigStepHours*60);
            if (tooFarInPast)
                return;
        }
            break;
        case 3: {
            myTime->AddStep(tvguideConfig.bigStepHours*60);
        }
            break;
        case 4: {
            bool tooFarInPast = myTime->DelStep(tvguideConfig.hugeStepHours*60);
            if (tooFarInPast)
                return;
        }
            break;
        case 6: {
            myTime->AddStep(tvguideConfig.hugeStepHours*60);
        }
            break;
        case 7: {
            cMyTime primeChecker;
            primeChecker.Now();
            time_t prevPrime = primeChecker.getPrevPrimetime(myTime->GetStart());
            if (primeChecker.tooFarInPast(prevPrime))
                return;
            myTime->SetTime(prevPrime);
        }
            break;
        case 9: {
            cMyTime primeChecker;
            time_t nextPrime = primeChecker.getNextPrimetime(myTime->GetStart());
            myTime->SetTime(nextPrime);
        }
            break;
        default:
            return;
    }
    drawGridsTimeJump();
    timeLine->drawDateViewer();
    timeLine->drawClock();
    timeLine->setTimeline();
    osdManager.flush();
}

void cTvGuideOsd::ChannelJump(int num) {
    if (!channelJumper) {
        channelJumper = new cChannelJump(channelGroups);
    }
    channelJumper->Set(num);
    channelJumper->DrawText();
    osdManager.flush();
}

void cTvGuideOsd::CheckTimeout(void) {
    if (!channelJumper)
        return;
    if (channelJumper->TimeOut()) {
        int newChannelNum = channelJumper->GetChannel(); 
        delete channelJumper;
        channelJumper = NULL;
        const cChannel *newChannel = Channels.GetByNumber(newChannelNum);
        if (newChannel) {
            readChannels(newChannel);
            if (columns.Count() > 0) {
                drawGridsChannelJump();
            }
        }
        osdManager.flush();
    }
}

void cTvGuideOsd::SetTimers() {
    for (cChannelColumn *column = columns.First(); column; column = columns.Next(column)) {
        column->SetTimers();
    }
}

eOSState cTvGuideOsd::ProcessKey(eKeys Key) {
    eOSState state = osContinue;
    cPixmap::Lock();
    bool alreadyUnlocked = false;
    if (recMenuManager->isActive()) {
        state = recMenuManager->ProcessKey(Key);
        if (state == osEnd) {
            SetTimers();                
            osdManager.flush();
        }
        state = osContinue;
    } else if (detailViewActive) {
        if ((Key & ~k_Repeat) == kRed) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            processKeyRed();
        } else if ((Key & ~k_Repeat) == kBlue) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            if ((tvguideConfig.blueKeyMode == eBlueKeySwitch) || (tvguideConfig.blueKeyMode == eBlueKeyFavorites)) {
                state = ChannelSwitch(&alreadyUnlocked);
            } else {
                osdManager.flush();
                state = osContinue;
            }
        } else if ((Key & ~k_Repeat) == kOk && (tvguideConfig.blueKeyMode == eBlueKeyEPG)) {
            delete detailView;
            detailView = NULL;
            detailViewActive = false;
            state = ChannelSwitch(&alreadyUnlocked);
        } else {
            state = detailView->ProcessKey(Key);
            if (state == osEnd) {
                delete detailView;
                detailView = NULL;
                detailViewActive = false;
                osdManager.flush();
                state = osContinue;
            }
        }
    } else {
        switch (Key & ~k_Repeat) {
            case kUp:       processKeyUp(); break;
            case kDown:     processKeyDown(); break;
            case kLeft:     processKeyLeft(); break;
            case kRight:    processKeyRight(); break;
            case kRed:      processKeyRed(); break;
            case kGreen:    processKeyGreen(); break;
            case kYellow:   processKeyYellow(); break;
            case kBlue:     state = processKeyBlue(&alreadyUnlocked); break;
            case kOk:       state = processKeyOk(&alreadyUnlocked); break;
            case kBack:     state=osEnd; break;    
            case k0 ... k9: processNumKey(Key - k0); break;
            case kNone:     if (channelJumper) CheckTimeout(); break;
            default:        break;
        }
    }
    if (!alreadyUnlocked) {
        cPixmap::Unlock();
    }
    return state;
}

void cTvGuideOsd::dump() {
    esyslog("tvguide: ------Dumping Content---------");
    activeGrid->debug();
    int i=1;
    for (cChannelColumn *col = columns.First(); col; col = columns.Next(col)) {
        col->dumpGrids();
    }
}
