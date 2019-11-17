#include "channelcolumn.h"
#include "grid.h"

cGrid::cGrid(cChannelColumn *c) {
    this->column = c;
    text = new cTextWrapper();
    dirty = true;
    active = false;
    viewportHeight = 0;
    borderWidth = 10;
}

cGrid::~cGrid(void) {
    delete text;
}

void cGrid::setBackground() {
    if (active) {
        color = theme.Color(clrHighlight);
        colorBlending = theme.Color(clrHighlightBlending);
    } else {
        if (isColor1) {
            color = theme.Color(clrGrid1);
            colorBlending = theme.Color(clrGrid1Blending);
        } else {
            color = theme.Color(clrGrid2);
            colorBlending = theme.Color(clrGrid2Blending);
        }
    }
}

void cGrid::Draw() {
    if (!pixmap) {
        return;
    }
    if (dirty) {
        if (tvguideConfig.style == eStyleGraphical) {
            drawBackgroundGraphical(bgGrid, active);
            drawText();
        } else {
            setBackground();
            drawBackground();
            drawText();
            drawBorder();
        }
        pixmap->SetLayer(1);
        dirty = false;
    }
}

bool cGrid::isFirst(void) {
    if (column->isFirst(this))
        return true;
    return false;
}

bool cGrid::Match(time_t t) {
    if ((StartTime() < t) && (EndTime() > t))
        return true;
    else
        return false;
}

int cGrid::calcOverlap(cGrid *neighbor) {
    int overlap = 0;
    if (intersects(neighbor)) {
        if ((StartTime() <= neighbor->StartTime()) && (EndTime() <= neighbor->EndTime())) {
            overlap = EndTime() - neighbor->StartTime();
        } else if ((StartTime() >= neighbor->StartTime()) && (EndTime() >= neighbor->EndTime())) {
            overlap = neighbor->EndTime() - StartTime();
        } else if ((StartTime() >= neighbor->StartTime()) && (EndTime() <= neighbor->EndTime())) {
            overlap = Duration();
        } else if ((StartTime() <= neighbor->StartTime()) && (EndTime() >= neighbor->EndTime())) {
            overlap = neighbor->EndTime() - neighbor->StartTime();
        }
    }
    return overlap;
}

bool cGrid::intersects(cGrid *neighbor) {
    return ! ( (neighbor->EndTime() <= StartTime()) || (neighbor->StartTime() >= EndTime()) ); 
}
