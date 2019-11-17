#include "baserender.h"
#include "flat.h"

cFlatBaseRender::cFlatBaseRender(void) {
    font = cFont::CreateFont(Setup.FontOsd, Setup.FontOsdSize );
    fontSml = cFont::CreateFont(Setup.FontSml, Setup.FontSmlSize);
    fontFixed = cFont::CreateFont(Setup.FontFix, Setup.FontFixSize);

    fontHeight = font->Height();
    fontSmlHeight = fontSml->Height();

    topBarTitle = "";
    tobBarTitleExtra1 = ""; 
    tobBarTitleExtra2 = ""; 
    topBarLastDate = "";
    topBarUpdateTitle = false;
    topBarHeight = 0;

    marginOsd = 5;
    marginItem = 10;

    progressBarHeight = 20;
    
    buttonsHeight = 0;
    
    topBarPixmap = NULL;
    buttonsPixmap = NULL;
    messagePixmap = NULL;
    contentPixmap = NULL;
    progressBarPixmap = NULL;
}

cFlatBaseRender::~cFlatBaseRender(void) {
    delete font;
    delete fontSml;
    delete fontFixed;

    if( osd )
    {
        if( topBarPixmap )
            osd->DestroyPixmap(topBarPixmap);
        if( buttonsPixmap )
            osd->DestroyPixmap(buttonsPixmap);
        if( messagePixmap )
            osd->DestroyPixmap(messagePixmap);
        if( contentPixmap )
            osd->DestroyPixmap(contentPixmap);
        if( progressBarPixmap )
            osd->DestroyPixmap(progressBarPixmap);

        delete osd;
    }
}

void cFlatBaseRender::CreateFullOsd(void) {
    CreateOsd(cOsd::OsdLeft() + marginOsd, cOsd::OsdTop() + marginOsd, cOsd::OsdWidth() - marginOsd*2, cOsd::OsdHeight() - marginOsd*2);
}

void cFlatBaseRender::CreateOsd(int left, int top, int width, int height) {
    osdWidth = width;
    osdHeight = height;

    osd = cOsdProvider::NewOsd(left, top);
    if (osd) {
        tArea Area = { 0, 0, width, height,  32 };
        if (osd->SetAreas(&Area, 1) == oeOk) {  
            return;
        }
    }
    return;
}

void cFlatBaseRender::TopBarCreate(void) {
    if( fontHeight > fontSmlHeight*2 )
        topBarHeight = fontHeight;
    else
        topBarHeight = fontSmlHeight * 2;

    topBarPixmap = osd->CreatePixmap(1, cRect(0, 0, osdWidth, topBarHeight));
    topBarPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::TopBarSetTitle(cString title) {
    topBarTitle = title;
    tobBarTitleExtra1 = "";
    tobBarTitleExtra2 = "";
    topBarUpdateTitle = true;
}

void cFlatBaseRender::TopBarSetTitleExtra(cString extra1, cString extra2) {
    tobBarTitleExtra1 = extra1;
    tobBarTitleExtra2 = extra2;
    topBarUpdateTitle = true;
}

// sollte bei jedum "Flush" aufgerufen werden!
void cFlatBaseRender::TopBarUpdate(void) {
    cString curDate = DayDateTime();
    if ( strcmp(curDate, topBarLastDate) || topBarUpdateTitle ) {
        topBarUpdateTitle = false;
        topBarLastDate = curDate;

        int fontTop = (topBarHeight - fontHeight) / 2;
        int fontSmlTop = (topBarHeight - fontSmlHeight*2) / 2;

        topBarPixmap->Fill(Theme.Color(clrTopBarBg));
        topBarPixmap->DrawText(cPoint(marginItem*2, fontTop), topBarTitle, Theme.Color(clrTopBarFont), Theme.Color(clrTopBarBg), font);

        int extra1Width = fontSml->Width(tobBarTitleExtra1);
        int extra2Width = fontSml->Width(tobBarTitleExtra2);
        int extraMaxWidth = max(extra1Width, extra2Width);
        
        int extraLeft = osdWidth/2 - extraMaxWidth/2;
        topBarPixmap->DrawText(cPoint(extraLeft, fontSmlTop), tobBarTitleExtra1, Theme.Color(clrTopBarDateFont), Theme.Color(clrTopBarBg), fontSml, extraMaxWidth);
        topBarPixmap->DrawText(cPoint(extraLeft, fontSmlTop + fontSmlHeight), tobBarTitleExtra2, Theme.Color(clrTopBarDateFont), Theme.Color(clrTopBarBg), fontSml, extraMaxWidth);

        time_t t;
        time(&t);

        cString time = TimeString(t);
        cString time2 = cString::sprintf("%s %s", *time, tr("clock"));

        int timeWidth = font->Width(*time2) + marginItem*2;
        topBarPixmap->DrawText(cPoint(osdWidth - timeWidth, fontTop), time2, Theme.Color(clrTopBarTimeFont), Theme.Color(clrTopBarBg), font);

        cString weekday = WeekDayNameFull(t);
        int weekdayWidth = fontSml->Width(*weekday);

        cString date = ShortDateString(t);
        int dateWidth = fontSml->Width(*date);

        int fullWidth = max(weekdayWidth, dateWidth);

        topBarPixmap->DrawText(cPoint(osdWidth - timeWidth - fullWidth - marginItem*2, fontSmlTop), weekday, Theme.Color(clrTopBarDateFont), Theme.Color(clrTopBarBg), fontSml, fullWidth, 0, taCenter);
        topBarPixmap->DrawText(cPoint(osdWidth - timeWidth - fullWidth - marginItem*2, fontSmlTop + fontSmlHeight), date, Theme.Color(clrTopBarDateFont), Theme.Color(clrTopBarBg), fontSml, fullWidth, 0, taCenter);
    }
}

void cFlatBaseRender::ButtonsCreate(void) {
    buttonsHeight = fontHeight + 30;
    buttonsWidth = osdWidth;

    buttonsPixmap = osd->CreatePixmap(1, cRect(0, osdHeight - buttonsHeight, buttonsWidth, buttonsHeight));
    buttonsPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ButtonsSet(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    int buttonWidth = (buttonsWidth / 4) - marginItem;

    buttonsPixmap->Fill(clrTransparent);

    int x = 0;
    buttonsPixmap->DrawText(cPoint(x, 0), Red, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonRed));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Green, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonGreen));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Yellow, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonYellow));

    x += buttonWidth + marginItem;
    buttonsPixmap->DrawText(cPoint(x, 0), Blue, Theme.Color(clrButtonFont), Theme.Color(clrButtonBg), font, buttonWidth, 0, taCenter);
    buttonsPixmap->DrawRectangle(cRect(x, fontHeight + 10, buttonWidth, 6), Theme.Color(clrButtonBlue));
}

void cFlatBaseRender::MessageCreate(void) {
    messageHeight = fontHeight + marginItem*2;
    int top = osdHeight - osdHeight/5 - messageHeight;
    messagePixmap = osd->CreatePixmap(2, cRect(0, top, osdWidth, messageHeight));
    messagePixmap->Fill(clrTransparent);
}

void cFlatBaseRender::MessageSet(eMessageType Type, const char *Text) {
    tColor col = Theme.Color(clrMessageStatus);
    switch (Type) {
        case mtStatus: 
            col = Theme.Color(clrMessageStatus);
            break;
        case mtInfo:
            col = Theme.Color(clrMessageInfo);
            break;
        case mtWarning:
            col = Theme.Color(clrMessageWarning);
            break;
        case mtError:
            col = Theme.Color(clrMessageError);
            break;
    }
    messagePixmap->Fill(Theme.Color(clrMessageBg));

    messagePixmap->DrawRectangle(cRect( 0, 0, messageHeight, messageHeight), col);
    messagePixmap->DrawRectangle(cRect( osdWidth - messageHeight, 0, messageHeight, messageHeight), col);

    int textWidth = font->Width(Text);
    messagePixmap->DrawText(cPoint((osdWidth - textWidth) / 2, marginItem), Text, Theme.Color(clrMessageFont), Theme.Color(clrMessageBg), font);
}

void cFlatBaseRender::MessageClear(void) {
    messagePixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ContentCreate(int Left, int Top, int Width, int Height) {
    contentHasScrollbar = false;
    contentShown = false;

    contentLeft = Left;
    contentTop = Top;
    contentWidth = Width;
    contentHeight = Height;
    int lines = ContentVisibleLines();
    contentHeight = lines * fontHeight;
}

void cFlatBaseRender::ContentSet(const char *Text, tColor ColorFg, tColor ColorBg) {
    contentWrapper.Set(Text, font, contentWidth - marginItem*2);
    contentColorFg = ColorFg;
    contentColorBg = ColorBg;

    int contentWrapperHeight = (contentWrapper.Lines()+1) * fontHeight;

    if( contentWrapperHeight > contentHeight ) {
        contentDrawPortHeight = contentWrapperHeight;
        contentHasScrollbar = true;
    } else {
        contentDrawPortHeight = contentHeight;
        contentHasScrollbar = false;
    }

    contentPixmap = osd->CreatePixmap(2, cRect(contentLeft, contentTop, contentWidth, contentHeight),
            cRect(0, 0, contentWidth, contentDrawPortHeight));

    contentPixmap->Fill(contentColorBg);

    contentDraw();
    contentShown = true;
}

bool cFlatBaseRender::ContentScrollable(void) {
    return contentHasScrollbar;
}

double cFlatBaseRender::ScrollbarSize(void) {
    return (double)contentHeight / (double)contentDrawPortHeight;
}

int cFlatBaseRender::ContentScrollTotal(void) {
    return contentWrapper.Lines();
}

int cFlatBaseRender::ContentGetHeight(void) {
    return contentHeight;
}

int cFlatBaseRender::ContentScrollOffset(void) {
    double offset;
    if ( ((-1)*contentPixmap->DrawPort().Point().Y() + contentHeight + fontHeight) > contentDrawPortHeight)
        offset = (double)1 - ScrollbarSize();
    else
        offset = (double)((-1)*contentPixmap->DrawPort().Point().Y())/(double)((-1)*contentPixmap->DrawPort().Point().Y() + contentHeight);
    return ContentScrollTotal() * offset;
}

int cFlatBaseRender::ContentVisibleLines(void) {
    return contentHeight / fontHeight;
}

bool cFlatBaseRender::ContentScroll(bool Up, bool Page) {
    int aktHeight = contentPixmap->DrawPort().Point().Y();
    int totalHeight = contentPixmap->DrawPort().Height();
    int screenHeight = contentPixmap->ViewPort().Height();
    int lineHeight = fontHeight;
    bool scrolled = false;
    if (Up) {
        if (Page) {
            int newY = aktHeight + screenHeight;
            if (newY > 0)
                newY = 0;
            contentPixmap->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (aktHeight < 0) {
                contentPixmap->SetDrawPortPoint(cPoint(0, aktHeight + lineHeight));
                scrolled = true;
            }
        }
    } else {
        if (Page) {
            int newY = aktHeight - screenHeight;
            if ((-1)*newY > totalHeight - screenHeight)
                newY = (-1)*(totalHeight - screenHeight);
            contentPixmap->SetDrawPortPoint(cPoint(0, newY));
            scrolled = true;
        } else {
            if (totalHeight - ((-1)*aktHeight + lineHeight) > screenHeight) {
                contentPixmap->SetDrawPortPoint(cPoint(0, aktHeight - lineHeight));
                scrolled = true;
            }
        }
    }
    return scrolled;
}

bool cFlatBaseRender::ContentIsShown(void) {
    return contentShown;
}

void cFlatBaseRender::ContentClear(void) {
    if( contentPixmap )
        contentPixmap->Fill(clrTransparent);
    contentShown = false;
}

void cFlatBaseRender::contentDraw(void) {
    int linesText = contentWrapper.Lines();
    int currentHeight = 0;
    for (int i=0; i < linesText; i++) {
        currentHeight = (i)*fontHeight;
        contentPixmap->DrawText(cPoint(marginItem, currentHeight), contentWrapper.GetLine(i), contentColorFg, contentColorBg, font, contentWidth - marginItem*2);
    }
}

void cFlatBaseRender::ProgressBarDrawInline(cPixmap *Pixmap, int Left, int Top, int Width, int Height, int Current, int Total, tColor ColorFg, tColor ColorBarFg, tColor ColorBg) {
    int Middle = Top + Height/2;
    double percentLeft = ((double)Current) / (double)Total;

    // background
    Pixmap->DrawRectangle(cRect( Left, Top, Width, Height), ColorBg);

    // small line
    Pixmap->DrawRectangle(cRect( Left, Middle-1, Width, 2), ColorFg);

    if (Current > 0)
        Pixmap->DrawRectangle(cRect( Left, Middle - 4, (Width * percentLeft), 8), ColorBarFg);
}

void cFlatBaseRender::ProgressBarMarksCreate(int Left, int Top, int Width, tColor ColorFg, tColor ColorBarFg, tColor ColorBg) {
    progressBarHeight = 40;
    ProgressBarCreate(Left, Top, Width, ColorFg, ColorBarFg, ColorBg);
}

void cFlatBaseRender::ProgressBarCreate(int Left, int Top, int Width, tColor ColorFg, tColor ColorBarFg, tColor ColorBg) {
    progressBarTop = Top;
    progressBarWidth = Width;

    progressBarColorFg = ColorFg;
    progressBarColorBarFg = ColorBarFg;
    progressBarColorBg = ColorBg;
    
    progressBarColorBarCurFg = Theme.Color(clrReplayProgressBarCurFg);
    
    progressBarPixmap = osd->CreatePixmap(2, cRect(Left, Top, Width, progressBarHeight));
    progressBarPixmap->Fill(clrTransparent);
}

void cFlatBaseRender::ProgressBarDraw(int Current, int Total) {
    int top = progressBarHeight / 2;
    double percentLeft = ((double)Current) / (double)Total;

    progressBarPixmap->Fill( progressBarColorBg );

    progressBarPixmap->DrawRectangle(cRect( 0, top-1, progressBarWidth, 2), progressBarColorFg);
    if (Current > 0)
        progressBarPixmap->DrawRectangle(cRect( 0, top - 4, (progressBarWidth * percentLeft), 8), progressBarColorBarFg);
}

int cFlatBaseRender::ProgressBarHeight(void) {
    return progressBarHeight;
}

int cFlatBaseRender::ProgressBarMarksHeight(void) {
    return 40;
}

void cFlatBaseRender::ProgressBarDrawMarks(int Current, int Total, const cMarks *Marks, tColor Color, tColor ColorCurrent) {
    progressBarColorMark = Color;
    progressBarColorMarkCurrent = ColorCurrent;
    int posMark = 0, posMarkLast = 0, posCurrent = 0;
    
    int top = progressBarHeight / 2;
    progressBarPixmap->Fill( progressBarColorBg );

    int sml = progressBarHeight / 10 * 2;
    if( sml <= 4 )
        sml = 4;
    int big = progressBarHeight - sml*2 - 2;
    
    if( !Marks ) {
        progressBarColorFg = progressBarColorBarCurFg;
        progressBarColorBarFg = progressBarColorBarCurFg;

        ProgressBarDraw(Current, Total);
        return;
    }
    if( !Marks->First() ) {
        progressBarColorFg = progressBarColorBarCurFg;
        progressBarColorBarFg = progressBarColorBarCurFg;

        ProgressBarDraw(Current, Total);
        return;
    }

    // the small line
    progressBarPixmap->DrawRectangle(cRect( 0, top - sml/2, progressBarWidth, sml), progressBarColorFg);
        
    bool Start = true;
    for( const cMark *m = Marks->First(); m; m = Marks->Next(m) ) {
        posMark = ProgressBarMarkPos( m->Position(), Total );
        posCurrent = ProgressBarMarkPos( Current, Total );

        ProgressBarDrawMark(posMark, posMarkLast, posCurrent, Start, m->Position() == Current);
        posMarkLast = posMark;
        Start = !Start;
    }
    if( !Start ) {
        progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - big/2, progressBarWidth - posMarkLast, big), progressBarColorBarFg);
        if( posCurrent > posMarkLast )
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - big/2, posCurrent - posMarkLast, big), progressBarColorBarCurFg);
    } else {
        // marker
        progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - sml/2, posCurrent - posMarkLast, sml), progressBarColorBarCurFg);
        progressBarPixmap->DrawRectangle(cRect( posCurrent - big/2, top - big/2, big, big), progressBarColorBarCurFg);

        if( posCurrent > posMarkLast + sml/2 )
            progressBarPixmap->DrawRectangle(cRect( posMarkLast - sml/2, 0, sml, progressBarHeight), progressBarColorMark);
    }
}

int cFlatBaseRender::ProgressBarMarkPos(int P, int Total) {
    return P * progressBarWidth / Total;
}

void cFlatBaseRender::ProgressBarDrawMark(int posMark, int posMarkLast, int posCurrent, bool Start, bool isCurrent)
{
    int top = progressBarHeight / 2;
    int sml = progressBarHeight / 10 * 2;
    if( sml <= 4 )
        sml = 4;
    int big = progressBarHeight - sml*2 - 2;
    
    int mbig = progressBarHeight*2;
    if( progressBarHeight > 15 )
        mbig = progressBarHeight;

    // marker vertical line
    if( posCurrent == posMark )
        progressBarPixmap->DrawRectangle(cRect( posMark - sml, 0, sml*2, progressBarHeight), progressBarColorMarkCurrent);
    else
        progressBarPixmap->DrawRectangle(cRect( posMark - sml/2, 0, sml, progressBarHeight), progressBarColorMark);

    if( Start ) {
        if( posCurrent > posMark )
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - sml/2, posMark - posMarkLast, sml), progressBarColorBarCurFg);
        else {
            // marker
            progressBarPixmap->DrawRectangle(cRect( posCurrent - big/2, top - big/2, big, big), progressBarColorBarCurFg);
            if( posCurrent > posMarkLast )
                progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - sml/2, posCurrent - posMarkLast, sml), progressBarColorBarCurFg);
        }
        // marker top
        if( isCurrent )
            progressBarPixmap->DrawRectangle(cRect( posMark - mbig/2, 0, mbig, sml), progressBarColorMarkCurrent);
        else
            progressBarPixmap->DrawRectangle(cRect( posMark - mbig/2, 0, mbig, sml), progressBarColorMark);
    } else {
        // big line
        if( posCurrent > posMark ) {
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - big/2, posMark - posMarkLast, big), progressBarColorBarCurFg);
            // draw last marker top
            progressBarPixmap->DrawRectangle(cRect( posMarkLast - mbig/2, 0, mbig, marginItem/2), progressBarColorMark);
        } else {
            progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - big/2, posMark - posMarkLast, big), progressBarColorBarFg);
            if( posCurrent > posMarkLast ) {
                progressBarPixmap->DrawRectangle(cRect( posMarkLast, top - big/2, posCurrent - posMarkLast, big), progressBarColorBarCurFg);
                // draw last marker top
                progressBarPixmap->DrawRectangle(cRect( posMarkLast - mbig/2, 0, mbig, marginItem/2), progressBarColorMark);
            }
        }
        // marker bottom
        if( isCurrent )
            progressBarPixmap->DrawRectangle(cRect( posMark - mbig/2, progressBarHeight - sml, mbig, sml), progressBarColorMarkCurrent);
        else
            progressBarPixmap->DrawRectangle(cRect( posMark - mbig/2, progressBarHeight - sml, mbig, sml), progressBarColorMark);
    }
    
    if( posCurrent == posMarkLast )
        progressBarPixmap->DrawRectangle(cRect( posMarkLast - sml, 0, sml*2, progressBarHeight), progressBarColorMarkCurrent);
    else
        progressBarPixmap->DrawRectangle(cRect( posMarkLast - sml/2, 0, sml, progressBarHeight), progressBarColorMark);
    
}
