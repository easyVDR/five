#include "channelcolumn.h"
#include "dummygrid.h"

cChannelColumn::cChannelColumn(int num, const cChannel *channel, cMyTime *myTime) {
    this->channel = channel;
    this->num = num;
    this->myTime = myTime;
    hasTimer = channel->HasTimer();
    hasSwitchTimer = SwitchTimers.ChannelInSwitchList(channel);
    schedulesLock = new cSchedulesLock(false, 100);
    header = NULL;
}

cChannelColumn::~cChannelColumn(void) {
    if (header)
        delete header;
    grids.Clear();
    delete schedulesLock;
}

void cChannelColumn::clearGrids() {
    grids.Clear();
}

void cChannelColumn::createHeader() {
    header = new cHeaderGrid();
    header->createBackground(num);
    header->drawChannel(channel);
}

void cChannelColumn::drawHeader() {
    header->setPosition(num);
}

bool cChannelColumn::readGrids() {
    schedules = cSchedules::Schedules(*schedulesLock);
    const cSchedule *Schedule = NULL;
    Schedule = schedules->GetSchedule(channel);
    if (!Schedule) {
        addDummyGrid(myTime->GetStart(), myTime->GetEnd(), NULL, false);
        return true;
    }
    bool eventFound = false;
    bool dummyAtStart = false;
    const cEvent *startEvent = Schedule->GetEventAround(myTime->GetStart());
    if (startEvent != NULL) {
        eventFound = true;
    } else {
        for (int i=1; i<6; i++) {
            startEvent = Schedule->GetEventAround(myTime->GetStart()+i*5*60);
            if (startEvent) {
                eventFound = true;
                dummyAtStart = true;
                break;
            }
        }
    } 
    if (eventFound) {
        bool col = true;
        if (dummyAtStart) {
            addDummyGrid(myTime->GetStart(), startEvent->StartTime(), NULL, col);
            col = !col;
        }
        bool dummyNeeded = true;
        bool toFarInFuture = false;
        time_t endLast = myTime->GetStart();
        const cEvent *event = startEvent;
        const cEvent *eventLast = NULL;
        for (; event; event = Schedule->Events()->Next(event)) {
            if (endLast < event->StartTime()) {
                //gap, dummy needed
                time_t endTime = event->StartTime();
                if (endTime > myTime->GetEnd()) {
                    endTime = myTime->GetEnd();
                    toFarInFuture = true;
                }
                addDummyGrid(endLast, endTime, NULL, col);
                col = !col;
            }
            if (toFarInFuture) {
                break;
            }
            addEpgGrid(event, NULL, col);
            col = !col;
            endLast = event->EndTime();
            if (event->EndTime() > myTime->GetEnd()) {
                dummyNeeded = false;
                break;
            }
            eventLast = event;
        }
        if (dummyNeeded) {
            addDummyGrid(eventLast->EndTime(), myTime->GetEnd(), NULL, col);
        }
        return true;
    } else {
        addDummyGrid(myTime->GetStart(), myTime->GetEnd(), NULL, false);
        return true;
    }
    return false;
}

void cChannelColumn::drawGrids() {
    for (cGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
        grid->SetViewportHeight();
        grid->PositionPixmap();
        grid->Draw();
    }
}

int cChannelColumn::getX() {
    return geoManager.timeLineWidth + num*geoManager.colWidth;
}

int cChannelColumn::getY() {
    return geoManager.statusHeaderHeight +  geoManager.timeLineHeight + num*geoManager.rowHeight;
}

cGrid * cChannelColumn::getActive() {
    cMyTime t;
    t.Now();
    for (cGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
        if (grid->Match(t.Get()))
            return grid;
    }
    return grids.First();
}

cGrid * cChannelColumn::getNext(cGrid *activeGrid) {
    if (activeGrid == NULL)
        return NULL;
    cGrid *next = grids.Next(activeGrid);
    if (next)
        return next;
    return NULL;
}

cGrid * cChannelColumn::getPrev(cGrid *activeGrid) {
    if (activeGrid == NULL)
        return NULL;
    cGrid *prev = grids.Prev(activeGrid);
    if (prev)
        return prev;
    return NULL;
}

cGrid * cChannelColumn::getNeighbor(cGrid *activeGrid) {
    if (!activeGrid)
        return NULL;
    cGrid *neighbor = NULL;
    int overlap = 0;
    int overlapNew = 0;
    cGrid *grid = NULL;
    grid = grids.First();
    if (grid) {
        for (; grid; grid = grids.Next(grid)) {
            if ( (grid->StartTime() == activeGrid->StartTime()) ) {
                neighbor = grid;
                break;
            }
            overlapNew = activeGrid->calcOverlap(grid);
            if (overlapNew > overlap) {
                neighbor = grid;
                overlap = overlapNew;
            }
        }
    }
    if (!neighbor)
        neighbor = grids.First();
    return neighbor;
}

bool cChannelColumn::isFirst(cGrid *grid) {
    if (grid == grids.First())
        return true;
    return false;
}

void cChannelColumn::AddNewGridsAtStart() {
    cGrid *firstGrid = NULL;
    firstGrid = grids.First();
    if (firstGrid == NULL)
        return;
    //if first event is long enough, nothing to do.
    if (firstGrid->StartTime() <= myTime->GetStart()) {
        return;
    }
    //if not, i have to add new ones to the list
    schedules = cSchedules::Schedules(*schedulesLock);
    const cSchedule *Schedule = NULL;
    Schedule = schedules->GetSchedule(channel);
    if (!Schedule) {
        if (firstGrid->isDummy()) {
            firstGrid->SetStartTime(myTime->GetStart());
            firstGrid->SetEndTime(myTime->GetEnd());
        }
        return;
    }
    bool col = !(firstGrid->IsColor1());
    bool dummyNeeded = true;
    for (const cEvent *event = Schedule->GetEventAround(firstGrid->StartTime()-60); event; event = Schedule->Events()->Prev(event)) {
        if (!event)
            break;
        if (event->EndTime() < myTime->GetStart()) {
            break;
        }
        cGrid *grid = addEpgGrid(event, firstGrid, col);
        col = !col;
        firstGrid = grid;
        if (event->StartTime() <= myTime->GetStart()) {
            dummyNeeded = false;
            break;
        }
    }
    if (dummyNeeded) {
        firstGrid = grids.First();
        if (firstGrid->isDummy()) {
            firstGrid->SetStartTime(myTime->GetStart());
            if (firstGrid->EndTime() >= myTime->GetEnd())
                firstGrid->SetEndTime(myTime->GetEnd());
        } else {
            addDummyGrid(myTime->GetStart(), firstGrid->StartTime(), firstGrid, col);
        }
    }
}

void cChannelColumn::AddNewGridsAtEnd() {
    cGrid *lastGrid = NULL;
    lastGrid = grids.Last();
    if (lastGrid == NULL)
        return;
    //if last event is long enough, nothing to do.
    if (lastGrid->EndTime() >= myTime->GetEnd()) {
        return;
    }
    //if not, i have to add new ones to the list
    schedules = cSchedules::Schedules(*schedulesLock);
    const cSchedule *Schedule = NULL;
    Schedule = schedules->GetSchedule(channel);
    if (!Schedule) {
        if (lastGrid->isDummy()) {
            lastGrid->SetStartTime(myTime->GetStart());
            lastGrid->SetEndTime(myTime->GetEnd());
        }
        return;
    }
    bool col = !(lastGrid->IsColor1());
    bool dummyNeeded = true;
    for (const cEvent *event = Schedule->GetEventAround(lastGrid->EndTime()+60); event; event = Schedule->Events()->Next(event)) {
        if (!event)
            break;
        if (event->StartTime() > myTime->GetEnd()) {
            break;
        }
        addEpgGrid(event, NULL, col);
        col = !col;
        if (event->EndTime() > myTime->GetEnd()) {
            dummyNeeded = false;
            break;
        }
    }
    if (dummyNeeded) {
        lastGrid = grids.Last();
        if (lastGrid->isDummy()) {
            lastGrid->SetEndTime(myTime->GetEnd());
            if (lastGrid->StartTime() <= myTime->GetStart())
                lastGrid->SetStartTime(myTime->GetStart());
        } else {
            addDummyGrid(lastGrid->EndTime(), myTime->GetEnd(), NULL, col);
        }
    }
}

void cChannelColumn::ClearOutdatedStart() {
    cGrid *firstGrid = NULL;
    while (true) {
        firstGrid = grids.First();
        if (!firstGrid)
            break;
        if (firstGrid->EndTime() <= myTime->GetStart()) {
            grids.Del(firstGrid);
            firstGrid = NULL;
        } else {
            if (firstGrid->isDummy()) {
                firstGrid->SetStartTime(myTime->GetStart());
                cGrid *next = getNext(firstGrid);
                if (next) {
                    firstGrid->SetEndTime(next->StartTime());
                } else {
                    firstGrid->SetEndTime(myTime->GetEnd());
                }
            }
            break;
        }
    }
}

void cChannelColumn::ClearOutdatedEnd() {
    cGrid *lastGrid = NULL;
    while (true) {
        lastGrid = grids.Last();    
        if (!lastGrid)
            break;
        if (lastGrid->StartTime() >= myTime->GetEnd()) {
            grids.Del(lastGrid);
            lastGrid = NULL;
        } else {
            if (lastGrid->isDummy()) {
                lastGrid->SetEndTime(myTime->GetEnd());
                cGrid *prev = getPrev(lastGrid);
                if (prev) {
                    lastGrid->SetStartTime(prev->EndTime());
                } else {
                    lastGrid->SetStartTime(myTime->GetStart());
                }
            }
            break;
        }
    }
}

cGrid *cChannelColumn::addEpgGrid(const cEvent *event, cGrid *firstGrid, bool color) {
    cGrid *grid = new cEpgGrid(this, event);
    grid->setText();
    grid->SetColor(color);
    if (!firstGrid)
        grids.Add(grid);
    else
        grids.Ins(grid, firstGrid);
    return grid;
}

cGrid *cChannelColumn::addDummyGrid(time_t start, time_t end, cGrid *firstGrid, bool color) {
    cGrid *dummy = new cDummyGrid(this, start, end);
    dummy->setText();
    dummy->SetColor(color);
    if (!firstGrid)
        grids.Add(dummy);
    else
        grids.Ins(dummy, firstGrid);
    return dummy;
}

void cChannelColumn::SetTimers() {
    hasTimer = channel->HasTimer();
    hasSwitchTimer = SwitchTimers.ChannelInSwitchList(channel);
    for (cGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
        bool gridHadTimer = grid->HasTimer();
        grid->SetTimer();
        if (gridHadTimer != grid->HasTimer())
            grid->SetDirty();
        bool gridHadSwitchTimer = grid->HasSwitchTimer();
        grid->SetSwitchTimer();
        if (gridHadSwitchTimer != grid->HasSwitchTimer())
            grid->SetDirty();
        grid->Draw();
    }
}

void cChannelColumn::dumpGrids() {
    esyslog("tvguide: ------Channel %s %d: %d entires ---------", channel->Name(), num, grids.Count());
    int i=1;
    for (cGrid *grid = grids.First(); grid; grid = grids.Next(grid)) {
        esyslog("tvguide: grid %d: start: %s, stop: %s", i, *cMyTime::printTime(grid->StartTime()), *cMyTime::printTime(grid->EndTime()));
        i++;
    }
}