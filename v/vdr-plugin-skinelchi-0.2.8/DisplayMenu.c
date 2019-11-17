/*
 * DisplayMenu.c: Menu display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <ctype.h>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

//for epgimages
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
#include "bitmap.h"
#else
#warning WARNING: YOU WILL NOT BE ABLE TO USE EPG-IMAGES
#warning uncomment "SKINELCHI_HAVE_IMAGEMAGICK = 1" in the "Makefile" if you want EPG images
#endif

//#define DEBUG
//#define DEBUG2
#include <vdr/videodir.h>
#include "common.h"
#include "DisplayMenu.h"
#include "vdrstatus.h"

//#include <sys/time.h>  // for timing debugging

#include "symbols/576/new.xpm"
#include "symbols/576/arrowturn.xpm"
#include "symbols/576/rec.xpm"
#include "symbols/576/clock.xpm"
#include "symbols/576/clocksml.xpm"
#include "symbols/576/vpssml.xpm"

#include "symbols/720/Bnew.xpm"
#include "symbols/720/Barrowturn.xpm"
#include "symbols/720/Brec.xpm"
#include "symbols/720/Bclock.xpm"
#include "symbols/720/Bclocksml.xpm"
#include "symbols/720/Bvpssml.xpm"

#include "symbols/1080/Cnew.xpm"
#include "symbols/1080/Carrowturn.xpm"
#include "symbols/1080/Crec.xpm"
#include "symbols/1080/Cclock.xpm"
#include "symbols/1080/Cclocksml.xpm"
#include "symbols/1080/Cvpssml.xpm"

cBitmap cSkinElchiDisplayMenu::bmANew(new_xpm);
cBitmap cSkinElchiDisplayMenu::bmAArrowTurn(arrowturn_xpm);
cBitmap cSkinElchiDisplayMenu::bmARec(rec_xpm);
cBitmap cSkinElchiDisplayMenu::bmAClock(clock_xpm);
cBitmap cSkinElchiDisplayMenu::bmAClocksml(clocksml_xpm);
cBitmap cSkinElchiDisplayMenu::bmAVPS(vpssml_xpm);

cBitmap cSkinElchiDisplayMenu::bmBNew(Bnew_xpm);
cBitmap cSkinElchiDisplayMenu::bmBArrowTurn(Barrowturn_xpm);
cBitmap cSkinElchiDisplayMenu::bmBRec(Brec_xpm);
cBitmap cSkinElchiDisplayMenu::bmBClock(Bclock_xpm);
cBitmap cSkinElchiDisplayMenu::bmBClocksml(Bclocksml_xpm);
cBitmap cSkinElchiDisplayMenu::bmBVPS(Bvpssml_xpm);

cBitmap cSkinElchiDisplayMenu::bmCNew(Cnew_xpm);
cBitmap cSkinElchiDisplayMenu::bmCArrowTurn(Carrowturn_xpm);
cBitmap cSkinElchiDisplayMenu::bmCRec(Crec_xpm);
cBitmap cSkinElchiDisplayMenu::bmCClock(Cclock_xpm);
cBitmap cSkinElchiDisplayMenu::bmCClocksml(Cclocksml_xpm);
cBitmap cSkinElchiDisplayMenu::bmCVPS(Bvpssml_xpm);

extern cSkinElchiStatus *ElchiStatus;

/* Possible values of the stream content descriptor according to ETSI EN 300 468 */
enum stream_content
{
	sc_reserved       = 0x00,
	sc_video_MPEG2    = 0x01,
	sc_audio_MP2      = 0x02, // MPEG 1 Layer 2 audio
	sc_subtitle       = 0x03,
	sc_audio_AC3      = 0x04,
	sc_video_H264_AVC = 0x05,
	sc_audio_HEAAC    = 0x06,
};

//class cSkinDisplayMenu : public cSkinDisplay {
       ///< This class implements the general purpose menu display, which is
       ///< used throughout the program to display information and let the
       ///< user interact with items.
       ///< A menu consists of the following fields, each of which is explicitly
       ///< set by calls to the member functions below:
       ///< - Title: a single line of text, indicating what this menu displays
       ///< - Color buttons: the red, green, yellow and blue buttons, used for
       ///<   various functions
       ///< - Message: a one line message, indicating a Status, Info, Warning,
       ///<   or Error condition
       ///< - Central area: the main central area of the menu, used to display
       ///<   one of the following:
       ///<   - Items: a list of single line items, of which the user may be
       ///<     able to select one (non-scrollable)
       ///<   - Event: the full information about one EPG event (*cEvent passed => scrollable)
       ///<   - Text: a multi line, scrollable text (scrollable)
       ///<   - Recording: a multi line, scrollable text (*cRecording passed => scrollable)

cSkinElchiDisplayMenu::cSkinElchiDisplayMenu(void)
{
   DSYSLOG("skinelchi: DisplayMenu::cSkinElchiDisplayMenu()")
   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);
   lasttime.tv_sec = 0;
   showMessage = false;
   showVolume = false;
   const cFont *font = cFont::GetFont(fontOsd);
   volumechange = ElchiStatus->GetVolumeChange();
   lastDiskUsageState = -1;
   timercheck = ElchiConfig.showTimer;
   lastClrDlgFg = ElchiConfig.clrDlgFg;
   lastClrDlgBg = ElchiConfig.clrDlgBg;
   buttons[0] = NULL;
   buttons[1] = NULL;
   buttons[2] = NULL;
   buttons[3] = NULL;
   lastCurrentText = NULL;
   title = NULL;
   epgimageThread = NULL;

   osd = NULL;
   CurAreaType = none;
   CurAreaNdx = -1;
   if ((timercheck & 2) && !cPluginManager::CallFirstService(EPGSEARCH_CONFLICTINFO))
      timercheck &= ~2;
   date = NULL;
   lh = font->Height();

   OSDsize.left   = cOsd::OsdLeft();
   OSDsize.top    = cOsd::OsdTop();
   OSDsize.width  = cOsd::OsdWidth();
   OSDsize.height = cOsd::OsdHeight();

   int OSDHeight = 576;
   int OSDWidth = 720;
   double OSDAspect = 4.0/3.0;
#if VDRVERSNUM >= 10708   
   cDevice::PrimaryDevice()->GetOsdSize(OSDWidth, OSDHeight, OSDAspect);
#endif
  
   DSYSLOG("skinelchi: OSDsize Menu %dx%d left=%d/%d top=%d/%d width=%d/%d heigth=%d/%d",
           OSDWidth, OSDHeight, OSDsize.left, Setup.OSDLeft, OSDsize.top, Setup.OSDTop, OSDsize.width, Setup.OSDWidth, OSDsize.height, Setup.OSDHeight);

   if (OSDHeight >= 1080) {
      bmNew       = &bmCNew;
      bmRec       = &bmCRec;
      bmArrowTurn = &bmCArrowTurn;
      bmClock     = &bmCClock;
      bmClocksml  = &bmCClocksml;
      bmVPS       = &bmCVPS;
   } else if (OSDHeight >= 720) {
      bmNew       = &bmBNew;
      bmRec       = &bmBRec;
      bmArrowTurn = &bmBArrowTurn;
      bmClock     = &bmBClock;
      bmClocksml  = &bmBClocksml;
      bmVPS       = &bmBVPS;
   } else {  // <  720 incl. 576
      bmNew       = &bmANew;
      bmRec       = &bmARec;
      bmArrowTurn = &bmAArrowTurn;
      bmClock     = &bmAClock;
      bmClocksml  = &bmAClocksml;
      bmVPS       = &bmAVPS;
   }
   
    x0 = 0;
    int lhh = lh / 2;
    x1 = x0 + lhh;
    x8 = (x0 + OSDsize.width) & ~0x07;
    //x6 = (x8 - lh + lhh) & ~0x07;
    x6 = (x8 - 8) & ~0x07;

    //x2 = x0 + x8 - x6;
    x2 = x1; //0 + (OSDsize.width & ~0x07) - (((OSDsize.width & ~0x07) - lh + lh/2) & ~0x07);

    x5 = x6 - lhh;
    x7 = x8 - lhh;
    int roundness = x2 - x0;

    y0 = 0;
    y1 = y0 + roundness;
    y2 = y0 + lh;
    y3 = y2 + lh;
    y8 = y0 + OSDsize.height;
    y7 = y8 - roundness;
    y6 = y8 - lh;
    y5 = y6 - lh;
    menuHeight = y5 - y3;
    menuTop = y3;

#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   epgimagesize = ElchiConfig.EpgImageSize;
   switch (epgimagesize) {
      case 1: x4 = x6 - 120; y4 = y3 + 90; break;
      case 3: x4 = x6 - 120; y4 = y3 + 120; break;
      default: x4 = x6 - 180; y4 = y3 + 144;
   }
   x4 &= ~0x07;
#else
   epgimagesize = 0;
   x4 = x6 - 720 / 4; // 180
   y4 = y3 + 576 / 4; // 144
#endif

   //TODO kein MessageScroll ??

   TitleScroll = NULL;
   CurrentItemScroll = NULL;

   osd = NULL;
   clear_pending = true;

   //gettimeofday(&tp2, NULL);
   //DSYSLOG2("skinelchi: DisplayMenu::cSkinElchiDisplayMenu(): %4.3f ms",
   //   (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0)
}


void cSkinElchiDisplayMenu::Init(areatype ReqAreaType)
{  // create areas (and display menu BG ?)
   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);
   DSYSLOG("skinelchi: DisplayMenu::Init(%d,%d)", CurAreaType, ReqAreaType)
   if (!clear_pending)
      return;

   if (CurAreaType != ReqAreaType)
   {
      DELETENULL(CurrentItemScroll); currentIndex = -1;
      DELETENULL(TitleScroll);

      DELETENULL(osd);
      osd = cOsdProvider::NewOsd(OSDsize.left, OSDsize.top);

      cBitmap *elchiPaletteBmp = new cBitmap(1,1,8);
      for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++)
         elchiPaletteBmp->SetColor(i, ElchiConfig.ThemeColors[i]);

      tArea Area_1_8[] = { { x0, y0, x8 - 1, y8 - 1, 8} };

      tArea Area_7_epg8[] = { { x0, y0, x8 - 1, y2 - 1, 2 },  // titelzeile
                              { x0, y2, x8 - 1, y3 - 1, 2 },  // statuszeile
                              { x0, y3, x4 - 1, y4 - 1, 2 },  // left of image
                              { x4, y3, x6 - 1, y4 - 1, 8 },  // image
                              { x0, y4, x6 - 1, y5 - 1, 2 },  // underneath image
                              { x6, y3, x8 - 1, y5 - 1, 2 },  // scrollbar
                              { x0, y5, x8 - 1, y8 - 1, 4 },};// message zeile und buttons


      tArea Area_7_epg4[] = { { x0, y0, x8 - 1, y2 - 1, 2 },  // titelzeile
                              { x0, y2, x8 - 1, y3 - 1, 2 },  // statuszeile
                              { x0, y3, x4 - 1, y4 - 1, 2 },  // left of image
                              { x4, y3, x6 - 1, y4 - 1, 4 },  // image
                              { x0, y4, x6 - 1, y5 - 1, 2 },  // underneath image
                              { x6, y3, x8 - 1, y5 - 1, 2 },  // scrollbar
                              { x0, y5, x8 - 1, y8 - 1, 4 }, }; // message zeile und buttons

      tArea Area_4_items[] = { { x0, y0, x8 - 1, y2 - 1, 2 },  // titelzeile
                               { x0, y2, x8 - 1, y3 - 1, 2 },  // statuszeile
                               { x0, y3, x8 - 1, y5 - 1, 2 },  // central area
                               { x0, y5, x8 - 1, y8 - 1, 4 }, }; // message zeile und buttons

      tArea Area_5_scrollable[] = { { x0, y0, x8 - 1, y2 - 1, 2 },  // titelzeile
                                    { x0, y2, x8 - 1, y3 - 1, 2 },  // statuszeile
                                    { x0, y3, x6 - 1, y5 - 1, 2 },  // central area ohne scrollbar
                                    { x6, y3, x8 - 1, y5 - 1, 2 },  // scrollbar
                                    { x0, y5, x8 - 1, y8 - 1, 4 },};// message zeile und buttons

      //   AreaNdx            0         1            2            3             4
      tArea *AllAreas[] = {Area_1_8, Area_7_epg8, Area_7_epg4, Area_4_items, Area_5_scrollable};
      int AllAreasCnt[] = {1, 7, 7, 4, 5};  //sizeof(Area_1_8) / sizeof(tArea)

      int AreaNdx = -2;

      // create areas
      if ((ElchiConfig.trySingle8BppArea  || OSDsize.height > 576 ) && (osd->CanHandleAreas(AllAreas[0], AllAreasCnt[0]) == oeOk)) {
         osd->SetAreas(Area_1_8, sizeof(Area_1_8) / sizeof(tArea));
         AreaNdx = 0;
         epgimagecolors = 256 - MAX_ELCHI_THEME_COLORS;
         DSYSLOG("skinelchi: setting Area_1_8Bit (%s:%d)",__FILE__,__LINE__)
         osd->GetBitmap(0)->Reset();
         for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++)
            osd->GetBitmap(0)->SetColor(i, ElchiConfig.ThemeColors[i]);
      }
      else {
         epgimagesize = 0;
         switch (ReqAreaType) {
            case items: // Area_4_items
               if (osd->CanHandleAreas(AllAreas[3], AllAreasCnt[3]) == oeOk) {
                  AreaNdx = 3;
                  DSYSLOG("skinelchi: items: Area_4_items (%s:%d)",__FILE__,__LINE__)
                  osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
               }
               currentIndex = -1;
               break;
            case scrollable: // Area_5_scrollable
               if (osd->CanHandleAreas(AllAreas[4], AllAreasCnt[4]) == oeOk) {
                  AreaNdx = 4;
                  osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
                  DSYSLOG("skinelchi: scrollable: Area_5_scrollable (%s:%d)",__FILE__,__LINE__)
                  osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
               }
               break;
            case epg:
               if (!ElchiConfig.EpgImageSize) { // without EPG images
                  //epgimagesize = 0;
                  if (osd->CanHandleAreas(AllAreas[4], AllAreasCnt[4]) == oeOk) {
                     AreaNdx = 4; // Area_5_scrollable
                     DSYSLOG("skinelchi: epg1: OSD Area_5_scrollable (%s:%d)",__FILE__,__LINE__)
                     osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
                  }
               }
               else { // with EPG images
                  /* 8Bit Mode with multiple areas currently not supported by VDR or driver
                  if (osd->CanHandleAreas(AllAreas[1], AllAreasCnt[1]) == oeOk) {
                     AreaNdx = 1;
                     epgimagecolors = 256 - MAX_ELCHI_THEME_COLORS; // Area_7_epg8
                     epgimagesize = ElchiConfig.EpgImageSize;
                     DSYSLOG("skinelchi: epg2: Area_7_epg8 (%s:%d)",__FILE__,__LINE__)
                     osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
                     }
                  } else
                     */
                     if (osd->CanHandleAreas(AllAreas[2], AllAreasCnt[2]) == oeOk) {
                        AreaNdx = 2;
                        epgimagecolors = 15; // Area_7_epg4
                        epgimagesize = ElchiConfig.EpgImageSize;
                        DSYSLOG("skinelchi: epg3: Area_7_epg4 (%s:%d)",__FILE__,__LINE__)
                        osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
                     } else { // image not possible, try without
                        DSYSLOG("skinelchi: EPG image not possible, trying without (%s:%d)",__FILE__,__LINE__)
                        epgimagesize = 0;
                        if (osd->CanHandleAreas(AllAreas[4], AllAreasCnt[4]) == oeOk) {
                           AreaNdx = 4;  // Area_5_scrollable
                           DSYSLOG("skinelchi: epg4: OSD Area_5_scrollable (%s:%d)",__FILE__,__LINE__)
                           osd->SetAreas(AllAreas[AreaNdx], AllAreasCnt[AreaNdx]);
                        }
                     }
               }
               break;
            case none:
               esyslog("skinelchi: Areatype none has been given? (%s:%d)",__FILE__,__LINE__);
               break;
         }
      }

      //isyslog("skinelchi: AreaNdx=%d CurAreaNdx=%d" , AreaNdx, CurAreaNdx);

      if (AreaNdx == -2 ) { // fallback to MINOSD
         esyslog("skinelchi: 2B implemented: fall back to MINOSD (%s:%d)",__FILE__,__LINE__);
      }
      else
      {
         tColor tbg = Theme.Color(clrMenuTitleBg);
         tColor bg = Theme.Color(clrBackground);
         //upper left rounding
         osd->DrawRectangle(x0, y0, x2 - 1, y1 - 1, clrTransparent);
         osd->DrawEllipse(x0, y0,x2 - 1, y1 - 1, tbg, 2);
         osd->DrawRectangle(x0, y1, x2 - 1, y2 - 1, tbg);
         //upper right rounding
         osd->DrawRectangle(x6, y0, x8 - 1, y1 - 1, clrTransparent);
         osd->DrawEllipse(x6, y0, x8 - 1, y1  - 1, tbg, 1);
         osd->DrawRectangle(x6, y1, x8 - 1, y2 - 1, tbg);
         //title area
         osd->DrawRectangle(x2, y0, x6 - 1, y2 - 1, tbg);

         // message line
         osd->DrawRectangle(x0, y5, x8 - 1, y6 - 1, bg);

         changed = true;
         const cFont *font = cFont::GetFont(fontOsd);

         int x = x6 - font->Width(*DayDateTime()) - font->Width("  ");
         TitleScroll = new cCharScroll(x2, y0+(font->Height()-cFont::GetFont(fontSml)->Height())/2, x - x2, cFont::GetFont(fontSml)->Height(), osd,
                           Theme.Color(clrMenuTitleFg), Theme.Color(clrMenuTitleBg));
         //isyslog("skinelchi: titlescroll=%llX", (long long int)TitleScroll);
      }
      CurAreaType = ReqAreaType;
      CurAreaNdx = AreaNdx;
   } // end CurArea != ReqArea

   TitleScroll->SetText(NULL);
   DSYSLOG2("skinelchi: call1 DisplayMenu::Clear2()")
   Clear2();
   DSYSLOG2("skinelchi: call1 DisplayMenu::SetTitle()")
   SetTitle(title);
   DSYSLOG2("skinelchi: call1 DisplayMenu::SetButtons()")
   SetButtons(buttons[0], buttons[1], buttons[2], buttons[3]);

   //gettimeofday(&tp2, NULL);
   //DSYSLOG("skinelchi: DisplayMenu::Init() end: %4.3f ms",
   //   (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0)
   DSYSLOG("skinelchi: DisplayMenu::Init() end");
}

cSkinElchiDisplayMenu::~cSkinElchiDisplayMenu()
{
   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);
   DSYSLOG2("skinelchi: DisplayMenu::~cSkinElchiDisplayMenu()")

#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   if (epgimageThread) {
      epgimageThread->Stop();
      DELETENULL(epgimageThread);
   }
#endif
   free(lastCurrentText); lastCurrentText = NULL;
   free(date); date = NULL;
   if (osd) {
      DELETENULL(osd);
   }
   //gettimeofday(&tp2, NULL);
   //DSYSLOG2("skinelchi: DisplayMenu::~cSkinElchiDisplayMenu(): %4.3f ms",
   //   (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0)
}

void cSkinElchiDisplayMenu::DrawScrollbar(int Total, int Offset, int Shown, int Top, int Height, bool CanScrollUp, bool CanScrollDown)
{
   //isyslog("skinelchi: DrawScrollbar: t=%2d o=%2d s=%2d t=%2d h=%2d, %08X %08X", Total, Offset, Shown, Top, Height, Theme.Color(clrMenuScrollbarTotal), Theme.Color(clrMenuScrollbarShown));
   if (Total > 0 && Total > Shown) {
      changed = true;
      int st = Top;
      int sb = st + Height - 1;
      int tt = st + (sb - st) * Offset / Total;
      int tb = st + (sb - st) * (Offset + Shown) / Total;
      ///osd->DrawRectangle(x6, st, x8 - 1, sb, Theme.Color(clrMenuScrollbarTotal));
      ///osd->DrawRectangle(x6, tt, x8 - 1, tb, Theme.Color(clrMenuScrollbarShown));
      ///osd->DrawRectangle(x8-6, st, x8 - 1, sb, Theme.Color(clrMenuScrollbarTotal));
      ///osd->DrawRectangle(x8-6, tt, x8 - 1, tb, Theme.Color(clrMenuScrollbarShown));
      ///osd->DrawRectangle(x6, st, x8 - 7, sb, Theme.Color(clrBackground)); // Theme.Color(clrMenuItemNonSelectable));
      osd->DrawRectangle(x6, st, x6 + 4, sb, Theme.Color(clrMenuScrollbarTotal));
      osd->DrawRectangle(x6, tt, x6 + 4, tb, Theme.Color(clrMenuScrollbarShown));
      //osd->DrawRectangle(x6, st-2, x6 + 1, st + 2, Theme.Color(clrBackground)); // Theme.Color(clrMenuItemNonSelectable));
   }
}


void cSkinElchiDisplayMenu::SetTextScrollbar(void)
{
   if (textScroller.CanScroll()) {
      DrawScrollbar(textScroller.Total(), textScroller.Offset(), textScroller.Shown(),
         textScroller.Top(), textScroller.Height(),
         textScroller.CanScrollUp(), textScroller.CanScrollDown());
   }
}

void cSkinElchiDisplayMenu::Scroll(bool Up, bool Page)
{  ///< If this menu contains a text area that can be scrolled, this function
   ///< will be called to actually scroll the text. Up indicates whether the
   ///< text shall be scrolled up or down, and Page is true if it shall be
   ///< scrolled by a full page, rather than a single line. An object of the
   ///< cTextScroller class can be used to implement the scrolling text area.
   DSYSLOG2("skinelchi: DisplayMenu::Scroll(%s%s)", Up ? "'Up'" : "'Down'", Page ? "'Page'" : "'Line'")
   cSkinDisplayMenu::Scroll(Up, Page);
   SetTextScrollbar();
}

int cSkinElchiDisplayMenu::MaxItems(void)
{  ///< Returns the maximum number of items the menu can display.
   DSYSLOG("skinelchi: DisplayMenu::MaxItems()")
   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();
   return (menuHeight / lh);
}

void cSkinElchiDisplayMenu::Clear(void)
{  ///< Clears the entire central area of the menu.
   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);
   DSYSLOG("skinelchi: DisplayMenu::Clear()")
   clear_pending = true;

   free(lastCurrentText); lastCurrentText = NULL;

   if(CurrentItemScroll) {
      DELETENULL(CurrentItemScroll);
      currentIndex = -1;
   }

   //gettimeofday(&tp2, NULL);
   //DSYSLOG2("skinelchi: DisplayMenu::Clear(): %4.3f ms",
   //   (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0)
}

void cSkinElchiDisplayMenu::Clear2(void)
{
   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);
   DSYSLOG("skinelchi: DisplayMenu::Clear2()")
   free(date); date = NULL;

   clear_pending = false;
   changed = true;

   textScroller.Reset();
   tColor bg = Theme.Color(clrBackground);

   osd->DrawRectangle(x0, y2, x8 - 1, y5 - 1, bg);

   if (timercheck) {
      cTimer *nexttimer = Timers.GetNextActiveTimer();
      if (nexttimer) {
         const cFont *font = cFont::GetFont(fontOsd);
         Epgsearch_lastconflictinfo_v1_0 conflictinfo;
         conflictinfo.nextConflict = 0;
         conflictinfo.relevantConflicts = 0;
         conflictinfo.totalConflicts = 0;

         if ((timercheck & 2) && cPluginManager::CallFirstService(EPGSEARCH_CONFLICTINFO, &conflictinfo)) {
            if (conflictinfo.nextConflict) {
               DSYSLOG2("skinelchi: timer conflict found: %s", *DayDateTime(conflictinfo.nextConflict));
               char conflict[100];
               snprintf(conflict, 100, "%s %s", tr(" timer conflicts! "), *DayDateTime(conflictinfo.nextConflict));
               osd->DrawText(x2, y2, conflict, Theme.Color(clrMessageWarningFg), Theme.Color(clrMessageWarningBg), font, x6 - x2, y2 - y0, taRight);
            }
         }
         if (timercheck & 1 && !conflictinfo.nextConflict) {
            char s[100];
            snprintf(s, 100, "%s: %s %d %02d:%02d %s - %s", tr("Next recording"), *WeekDayName(nexttimer->StartTime()), nexttimer->GetMDay(nexttimer->Day()), nexttimer->Start() / 100, nexttimer->Start() % 100, nexttimer->Channel()->Name(), nexttimer->File());
            osd->DrawText(x2, y2, s, Theme.Color(clrMenuItemNonSelectable), bg, font);
         }

      }
   }

   //gettimeofday(&tp2, NULL);
   //DSYSLOG2("skinelchi: DisplayMenu::Clear2(): %4.3f ms",
   //   (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0)
}

void cSkinElchiDisplayMenu::DrawTitle(void)
{
   DSYSLOG("skinelchi: DisplayMenu::DrawTitle (%s)", *title2)
#if VDRVERSNUM >= 10728
   if ((MenuCategory() == mcMain) || (MenuCategory() == mcRecording) || (MenuCategory() == mcTimer)) {
      ostringstream titleline;
      titleline.imbue(std::locale(""));

      //titleline << *title2 << " - " << trVDR("Disk") << " " << trVDR("free") << ": " << fixed << setprecision(1) << cVideoDiskUsage::FreeMB()/1024.0 << " GB ("
      //          << (100 - cVideoDiskUsage::UsedPercent()) << "%)  -  "
      //          << fixed << cVideoDiskUsage::FreeMinutes()/60 << ":" << setw(2) << setfill('0') << cVideoDiskUsage::FreeMinutes()%60;
      titleline << *title2 << " - " << trVDR("Disk") << " " << cVideoDiskUsage::UsedPercent() << "% (" << fixed << setprecision(1) << cVideoDiskUsage::FreeMB()/1024.0 << " GB - "
                << fixed << cVideoDiskUsage::FreeMinutes()/60 << ":" << setw(2) << setfill('0') << cVideoDiskUsage::FreeMinutes()%60 << " " << trVDR("free") << ")";
                
      TitleScroll->SetText(titleline.str().c_str(), cFont::GetFont(fontSml));
   }
   else
      TitleScroll->SetText(*title2, cFont::GetFont(fontSml));
#endif
   changed = true;
}

void cSkinElchiDisplayMenu::SetTitle(const char *Title)
{  ///< Sets the title of this menu to Title.
   DSYSLOG("skinelchi: DisplayMenu::SetTitle(%s) clp=%d", Title, clear_pending)
   if (clear_pending) {
      free (title);
      title = strdup (Title);
      return;
   }

#if VDRVERSNUM >= 10728
   title2 = Title;
   DrawTitle();
#else
   TitleScroll->SetText(Title, cFont::GetFont(fontSml));
#endif
   changed = true;
}

void cSkinElchiDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue)
{  ///< Sets the color buttons to the given strings. If any of the values is
   ///< NULL, any previous text must be removed from the related button.
   DSYSLOG("skinelchi: DisplayMenu::SetButtons(\"%s\",\"%s\",\"%s\",\"%s\") clp=%d", Red, Green, Yellow, Blue, clear_pending)

   if (clear_pending) {
      free(buttons[0]); if (Red) buttons[0] = strdup(Red); else buttons[0] = NULL;
      free(buttons[1]); if (Green) buttons[1] = strdup(Green); else buttons[1] = NULL;
      free(buttons[2]); if (Yellow) buttons[2] = strdup(Yellow); else buttons[2] = NULL;
      free(buttons[3]); if (Blue) buttons[3] = strdup(Blue); else buttons[3] = NULL;
      return;
   }
   changed = true;
   const cFont *smallfont = cFont::GetFont(fontSml);
   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();
   int w = x8 - x0;
   int t0 = x0;
   int t1 = x2;
   int t2 = t0 + w / 4;
   int t3 = t0 + w / 2;
   int t4 = x8 - w / 4;
   int t5 = x6;
   int t6 = x8;

   tColor redbg = Theme.Color(clrButtonRedBg);
   tColor bluebg = Theme.Color(clrButtonBlueBg);
   osd->DrawRectangle(t0, y6, t1 - 1, y7 - 1, Red ? redbg : Theme.Color(clrBackground));
   osd->DrawRectangle(t0, y7, t1 - 1, y8 - 1, clrTransparent);
   osd->DrawEllipse(t0, y7, t1 - 1, y8 - 1, Red ? redbg : Theme.Color(clrBackground), 3);
   osd->DrawText(t1, y6, Red,    Theme.Color(clrButtonRedFg), Red ? redbg : Theme.Color(clrBackground), smallfont, t2 - t1, 0, taCenter);
   osd->DrawRectangle(t1, y8 - (font->Height() - smallfont->Height()), t2 - 1, y8 - 1, Red ? redbg : Theme.Color(clrBackground));
   
   osd->DrawText(t2, y6, Green,  Theme.Color(clrButtonGreenFg), Green ? Theme.Color(clrButtonGreenBg) : Theme.Color(clrBackground), smallfont, t3 - t2, 0, taCenter);
   osd->DrawRectangle(t2, y8 - (font->Height() - smallfont->Height()), t3 - 1, y8 - 1, Green ? Theme.Color(clrButtonGreenBg) : Theme.Color(clrBackground));
   
   osd->DrawText(t3, y6, Yellow, Theme.Color(clrButtonYellowFg), Yellow ? Theme.Color(clrButtonYellowBg) : Theme.Color(clrBackground), smallfont, t4 - t3, 0, taCenter);
   osd->DrawRectangle(t3, y8 - (font->Height() - smallfont->Height()), t4 - 1, y8 - 1, Yellow ? Theme.Color(clrButtonYellowBg) : Theme.Color(clrBackground));
   
   osd->DrawText(t4, y6, Blue,   Theme.Color(clrButtonBlueFg), Blue ? bluebg : Theme.Color(clrBackground), smallfont, t5 - t4, 0, taCenter);
   osd->DrawRectangle(t4, y8 - (font->Height() - smallfont->Height()), t6 - 1, y8 - 1, Blue ? bluebg : Theme.Color(clrBackground));
   osd->DrawRectangle(t5, y6, t6 - 1, y7 - 1, Blue ? bluebg : Theme.Color(clrBackground));
   osd->DrawRectangle(t5, y7, t6 - 1, y8 - 1, clrTransparent);
   osd->DrawEllipse(t5, y7, t6 - 1, y8 - 1, Blue ? bluebg : Theme.Color(clrBackground), 4);
}

void cSkinElchiDisplayMenu::SetMessage(eMessageType Type, const char *Text)
{  ///< Sets a one line message Text, with the given Type. Type can be used
   ///< to determine, e.g., the colors for displaying the Text.
   ///< If Text is NULL, any previously displayed message must be removed, and
   ///< any previous contents overwritten by the message must be restored.
   if (clear_pending && (Text || showMessage)) {
      isyslog("skinelchi: assuming item display");
      Init(scrollable);
   }

   if (Text) {
      DSYSLOG("skinelchi: DisplayMenu::SetMessage(%d,\"%s\")", int(Type), Text)
      changed = true;
      showMessage = true;
      const cFont *font = cFont::GetFont(fontOsd);
      lh = font->Height();
      int lhh = lh / 2;
      tColor bg = Theme.Color(clrBackground);
      osd->DrawRectangle(x0, y5, x2 - 1, y6 - 1, bg);
      tColor msb = Theme.Color(clrMessageStatusBg + 2 * Type);
      osd->DrawEllipse(x0, y5, x0 + lhh - 1, y6 - 1, msb, 7);
      osd->DrawText(x0 + lhh, y5, Text, Theme.Color(clrMessageStatusFg + 2 * Type), msb, font, x6 - x0 - lhh, 0, taCenter);
      osd->DrawRectangle(x6, y5, x8 - lhh - 1, y6 - 1, msb);
      osd->DrawRectangle(x8 - lhh, y5, x8 - 1, y6 - 1, bg);
      osd->DrawEllipse(x8 - lhh, y5, x8 - 1, y6 - 1, msb, 5);
   }
   else {
      if (showMessage) {
         DSYSLOG("skinelchi: DisplayMenu::SetMessage(%d,\"%s\")", int(Type), Text?Text:"NULL")
         changed = true;
         showMessage = false;
         tColor bg = Theme.Color(clrBackground);
         osd->DrawRectangle(x0, y5, x6 - 1, y6 - 1, bg);
         osd->DrawRectangle(x6, y5, x8 - 1, y6 - 1, bg);
      }
      else
         DSYSLOG("skinelchi: skip DisplayMenu::SetMessage(%d,\"%s\")", int(Type), Text)
   }
}

void cSkinElchiDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable)
{  ///< Sets the item at the given Index to Text. Index is between 0 and the
   ///< value returned by MaxItems(), minus one. Text may contain tab characters ('\t'),
   ///< which shall be used to separate the text into several columns, according to the
   ///< values set by a prior call to SetTabs(). If Current is true, this item shall
   ///< be drawn in a way indicating to the user that it is the currently selected
   ///< one. Selectable can be used to display items differently that can't be
   ///< selected by the user.
   ///< Whenever the current status is moved from one item to another,
   ///< this function will be first called for the old current item
   ///< with Current set to false, and then for the new current item
   ///< with Current set to true.
   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);
   DSYSLOG2("skinelchi: DisplayMenu::SetItem(\"%s\",%d,%s,%s) clp=%d", Text, Index, Current ? "'Current'" : "'nonCurrent'", Selectable ? "'Selectable'" : "'nonSelectable'", clear_pending)

   if (clear_pending) {
      Init(scrollable);
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuItemSelectable));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuItemCurrentBg));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuItemCurrentFg));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuItemNonSelectable));
      osd->DrawPixel(x0, y3, Theme.Color(clrBackground));
   }

   int xa = x5;
   int xb = x6;

   tColor ColorFg, ColorBg;
   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();
   int y = menuTop + Index * lh;

   if (Current) {
      // ignore identical calls
      if ((Index == currentIndex) && lastCurrentText && Text && !strcmp(lastCurrentText, Text)) {
         //DSYSLOG2("skinelchi: skipping superfluous SetCurrentItem");
         return;
      }
      free(lastCurrentText);
      lastCurrentText = strdup(Text);

      ColorFg = Theme.Color(clrMenuItemCurrentFg);
      ColorBg = Theme.Color(clrMenuItemCurrentBg);
      if (Index != currentIndex) {
         currentIndex = Index;
         osd->DrawEllipse(x0, y, x1 - 1, y + lh - 1, ColorBg, 7);
         osd->DrawEllipse(xa, y, xb - 1, y + lh - 1, ColorBg, 5);
         CurrentItemScroll = new cCharScroll(x1, y, xa - x1, font->Height(), osd, ColorFg, ColorBg);
      }
   }
   else {
      ColorFg = Theme.Color(Selectable ? clrMenuItemSelectable : clrMenuItemNonSelectable);
      ColorBg = Theme.Color(clrBackground);
      if (currentIndex == Index) { // reset previus "current index"
         osd->DrawRectangle(x0, y, x1 - 1, y + lh - 1, ColorBg);
         osd->DrawRectangle(xa, y, xb - 1, y + lh - 1, ColorBg);
         DELETENULL(CurrentItemScroll);
         currentIndex = -1;
         free(lastCurrentText);
         lastCurrentText = NULL;
      }
   }

   const char *s;
   int xOff = x1;
   for (int i = 0; i < MaxTabs; i++) {
      s = GetTabbedText(Text, i);
      if (s) {  // symbol and progress bar detection, partly borrowed from skinenigma 0.0.5
         char buffer[9];
         bool istimer = false;
         bool isnewrecording = false;
         bool isprogressbar = false;
         bool hasEventtimer = false;
         bool haspartEventtimer = false;
         bool isRecording = false;
         bool hasVPS = false;
         bool isRunning = false;

         if (ElchiConfig.showIcons) {
            // check if timer info symbols: " !#>"
            if (i == 0 && strlen(s) == 1 && strchr(" !#>", s[0])) {
               istimer = true; // update status
            }
            else
            // check if new recording: "10:10*", "1:10*", "01.01.06*"
               if (  (strlen(s) == 6 && s[5] == '*' && s[2] == ':' && isdigit(*s)
                      && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4)))
                  || (strlen(s) == 5 && s[4] == '*' && s[1] == ':' && isdigit(*s)
                      && isdigit(*(s + 2)) && isdigit(*(s + 3)))
                  || (strlen(s) == 9 && s[8] == '*' && s[5] == '.' && s[2] == '.'
                      && isdigit(*s) && isdigit(*(s + 1)) && isdigit(*(s + 3))
                      && isdigit(*(s + 4)) && isdigit(*(s + 6)) && isdigit(*(s + 7)))) {
                  isnewrecording = true;  // update status
                  strncpy(buffer, s, strlen(s));   // make a copy
                  buffer[strlen(s) - 1] = '\0';  // remove the '*' character
               }
               else
                  if ( (strlen(s) == 3) && ( i == 2 || i == 3 || i == 4)) {
                     if (s[0] == 'R') isRecording = true;
                     if (s[0] == 'T') hasEventtimer = true;
                     if (s[0] == 't') haspartEventtimer = true;
                     if (s[1] == 'V') hasVPS = true;
                     if (s[2] == '*') isRunning = true;
                  }
                  else
                     if ( (strlen(s) == 4) && ( i == 3 )) { //epgsearch What's on now default
                        if (s[1] == 'R') isRecording = true;
                        if (s[1] == 'T') hasEventtimer = true;
                        if (s[1] == 't') haspartEventtimer = true;
                        if (s[2] == 'V') hasVPS = true;
                        if (s[3] == '*') isRunning = true;
                     }
          }

         int current = 0, total = 0;
         // check if progress bar: "[|||||||   ]"
         if (!istimer && !isnewrecording && ElchiConfig.GraphicalProgressbar &&
            (strlen(s) > 5 && s[0] == '[' && s[strlen(s) - 1] == ']')) {
            const char *p = s + 1;
            isprogressbar = true; // update status
            for (; *p != ']'; ++p) {  // check if progressbar characters
               if (*p == ' ' || *p == '|') { // update counters
                  ++total;
                  if (*p == '|')
                     ++current;
               } else {  // wrong character detected; not a progressbar
                  isprogressbar = false;
                  break;
               }
            }
         }
         xOff = x1 + Tab(i);

         if (istimer) {
            // timer menu
            osd->DrawRectangle(xOff, y, xa, y + lh - 1, ColorBg);
            switch(s[0]) {
            case '!':
               osd->DrawBitmap(xOff, y + (lh - bmArrowTurn->Height()) / 2, *bmArrowTurn, ColorFg, ColorBg);
               break;
            case '#':
               osd->DrawBitmap(xOff, y + (lh - bmRec->Height()) / 2, *bmRec, ColorFg, ColorBg);
               break;
            case '>':
               osd->DrawBitmap(xOff, y + (lh - bmClock->Height()) / 2, *bmClock, ColorFg, ColorBg);
               break;
            case ' ':
            default:
               break;
            }
         } else if (isRecording || hasEventtimer || haspartEventtimer || hasVPS || isRunning) {
            // program schedule menu
            if (isRecording)
               osd->DrawBitmap(xOff, y + (lh - bmRec->Height()) / 2, *bmRec, ColorFg, ColorBg);
            else {
               if (hasEventtimer)
                  osd->DrawBitmap(xOff, y + (lh - bmClock->Height()) / 2, *bmClock, ColorFg, ColorBg);
               if (haspartEventtimer)
                  osd->DrawBitmap(xOff + (bmClock->Height() - bmClocksml->Height()) / 2, y + (lh - bmClocksml->Height()) / 2, *bmClocksml, ColorFg, ColorBg);
            }
            xOff += bmClock->Width(); // clock is wider than rec

            if (hasVPS)
               osd->DrawBitmap(xOff, y + (lh - bmVPS->Height()) / 2, *bmVPS, ColorFg, ColorBg);
            xOff += bmVPS->Width();

            if (isRunning)
               osd->DrawText(xOff, y, "*", ColorFg, ColorBg, font, xa - xOff);
            
         } else if (isnewrecording) {
            // recordings menu
            osd->DrawText(xOff, y, buffer, ColorFg, ColorBg, font, x5 - xOff);
            // draw symbol "new" centered
            int gap = std::max(0, (Tab(i+1)-Tab(i)- font->Width(buffer) - bmNew->Width()) / 2);
            osd->DrawBitmap(xOff + font->Width(buffer) + gap, y + (lh - bmNew->Height()) / 2, *bmNew, ColorFg, ColorBg);
         } else if (isprogressbar) {
            //isyslog("skinelchi: progressbar: %d t=%d c=%d %3d%%, %d(%s)", Tab(i+1)-Tab(i), total, current, current*100/total, strlen(s), s);
            // define x coordinates of progressbar
            int px0 = xOff;
            int px1;
            if (strlen(s) < 11)
               px1 = (Tab(i + 1)?Tab(i+1):xa);
            else
               px1 = std::min(xOff + font->Width(s) - font->Height()/2, xa);

            int px = px0 + std::max((int)((float) current * (float) (px1 - px0) / (float) total), 1);
            //isyslog("skinelchi: progressbar: %d-%d (%d) %d(%d)", px0, px1, px1-px0, px, px-px0);
            // define y coordinates of progressbar
            int py0 = y + 6;
            int py1 = y + lh - 6;
            osd->DrawRectangle(px0,     py0,      px,  py1,     ColorFg);
            osd->DrawRectangle(px + 1,  py0,      px1, py0 + 1, ColorFg);
            osd->DrawRectangle(px + 1,  py1 - 1,  px1, py1,     ColorFg);
            osd->DrawRectangle(px1 - 1, py0,      px1, py1,     ColorFg);
         } else
            if (Current) {
               CurrentItemScroll->SetText(s, font, xOff - x1); // xOff - x0
            }
            else {
               osd->DrawText(xOff, y, s, ColorFg, ColorBg, font, xa - xOff);
            }
      }
      if (!Tab(i + 1))
         break;
   }

   SetEditableWidth(xa - xOff);
   changed = true;
   //gettimeofday(&tp2, NULL);
   //double msec = (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0;
   //if (msec > 0.5) isyslog("skinelchi: DisplayMenu::cSkinElchiSetItem(): %4.3f ms", msec);
}

void cSkinElchiDisplayMenu::SetScrollbar(int Total, int Offset)
{
   DrawScrollbar(Total, Offset, MaxItems(), menuTop, MaxItems() * lh, Offset > 0, Offset + MaxItems() < Total);
}

void cSkinElchiDisplayMenu::SetEvent(const cEvent *Event)
{  ///< Sets the Event that shall be displayed, using the entire central area
   ///< of the menu. The Event's 'description' shall be displayed using a
   ///< cTextScroller, and the Scroll() function will be called to drive scrolling
   ///< that text if necessary.
   if (!Event) {
      DSYSLOG("skinelchi: skip DisplayMenu::SetEvent()")
      return;
   }

   DSYSLOG("skinelchi: DisplayMenu::SetEvent(%d) clp=%d", Event->EventID(), clear_pending)
   if (clear_pending) {
      Init(epg);
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventTitle));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventShortText));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventDescription));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventTime));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventVpsFg));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventVpsBg));
      osd->DrawPixel(x0, y3, Theme.Color(clrBackground));
   }

   changed = true;
   tColor bg = Theme.Color(clrBackground);
   tColor me = Theme.Color(clrMenuEventTitle);
   const cFont *font = cFont::GetFont(fontOsd);
   const cFont *smallfont = cFont::GetFont(fontSml);
   int slh = smallfont->Height();
   int textwidth = x6 - x2; // max. width

#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   if (epgimagesize) {  // != 0, that is: is wanted and can be displayed
      textwidth = x4 - x2 - (x2 - x0);

      if (!epgimageThread)
         epgimageThread = new cEpgImage(epgimagecolors);

      // start conversion via PutEventID
      curEventHasImage = epgimageThread->PutEventID(Event->EventID() /*&7*/);
      if (curEventHasImage) {
         int x64 = x6 - x4;
         int y4432 = y4 - (y4 - y3) / 2;
         osd->DrawText(x4, y4432 - lh, tr("Picture"), me, bg, smallfont, x64, 0, taCenter);
         osd->DrawText(x4, y4432, tr("loading"), me, bg, smallfont, x64, 0, taCenter);
         epgimageTimer.Set(); // force timeout
      }
      else {
         int x64 = x6 - x4;
         osd->DrawText(x4, y3, tr("No picture"), me, bg, smallfont, x64, 0, taCenter);
         osd->DrawText(x4, y3 + lh, tr("available"), me, bg, smallfont, x64, 0, taCenter);
      }
   }
#endif

   font = cFont::GetFont(fontOsd);
   smallfont = cFont::GetFont(fontSml);
   lh = font->Height();

   int y = y3;
   cTextScroller ts;

   // date, start time - end time, left of image
   char t[32];
   snprintf(t, sizeof(t), "%s  %s - %s", *Event->GetDateString(), *Event->GetTimeString(), *Event->GetEndTimeString());
   ts.Set(osd, x2, y, textwidth, y5 - y, t, font, Theme.Color(clrMenuEventTime), clrTransparent);
   y += ts.Height();

   // VPS time
   if (Event->Vps() && Event->Vps() != Event->StartTime()) {
      cString s = cString::sprintf(" VPS: %s", *Event->GetVpsString());
      osd->DrawText(x2, y, s, Theme.Color(clrMenuEventVpsFg), Theme.Color(clrMenuEventVpsBg), smallfont);
   }

#if VDRVERSNUM >= 10711
   // Parentalrating
   if (ElchiConfig.showEPGDetails == 2 && Event->ParentalRating()) {
      cString buffer = cString::sprintf(" %s ", *Event->GetParentalRatingString());
      int w = font->Width(buffer);
      osd->DrawText(x2 + textwidth - w - lh/2, y, buffer, Theme.Color(clrMenuEventVpsFg), Theme.Color(clrMenuEventVpsBg), smallfont, w);
   }
#endif
   y += lh;

   // Title
   ts.Set(osd, x2, y, textwidth, y5 - y, Event->Title(), font, me, clrTransparent);
   y += ts.Height();

   // ShortText
   if (!isempty(Event->ShortText())) {
      char *s = NULL;
      s = strdup(Event->ShortText());
      ts.Set(osd, x2, y, textwidth, y5 - y, s, smallfont, Theme.Color(clrMenuEventShortText), clrTransparent);
      free(s);
      y += ts.Height();
   }

#if VDRVERSNUM >= 10711
   // Content
   if (ElchiConfig.showEPGDetails == 2) {
      ostringstream content;
      bool firstContent = true;
      for (int i = 0; Event->Contents(i); i++) {
         if (!isempty(Event->ContentToString(Event->Contents(i)))) { // skip empty (user defined) content
            if (!firstContent)
               content << ", ";
            content << Event->ContentToString(Event->Contents(i));
            firstContent = false;
         }
      }
      if (content.str().length() > 0) {
         ts.Set(osd, x2, y, textwidth, y5 - y, content.str().c_str(), smallfont, Theme.Color(clrMenuEventShortText), Theme.Color(clrBackground));
         y += ts.Height();
      }
   }
#endif

   // Description
   ostringstream text;
   if (!isempty(Event->Description())) {
      y += slh/2;
      if (epgimagesize && (y4 + slh) > y) // mindestens unterhalb image beginnen
         y = y4 + slh;

      text << Event->Description() << endl;
   }

   if (ElchiConfig.showEPGDetails > 0) {
      const cComponents *Components = Event->Components();
      if (Components) {
         ostringstream audio;
         bool firstAudio = true;
         const char *audio_type = NULL;
         ostringstream subtitle;
         bool firstSubtitle = true;
         for (int i = 0; i < Components->NumComponents(); i++) {
            const tComponent *p = Components->Component(i);
            switch (p->stream) {
               case sc_video_MPEG2:
                  if (p->description)
                     text << endl << tr("Video") << ": " <<  p->description << " (MPEG2)";
                  else
                     text << endl << tr("Video") << ": MPEG2";
                  break;
               case sc_video_H264_AVC:
                  if (p->description)
                     text << endl << tr("Video") << ": " <<  p->description << " (H.264)";
                  else
                     text << endl << tr("Video") << ": H.264";
                  break;

               case sc_audio_MP2:
               case sc_audio_AC3:
               case sc_audio_HEAAC:
                  if (firstAudio)
                     firstAudio = false;
                  else
                     audio << ", ";

                  switch (p->stream) {
                     case sc_audio_MP2:
                        // workaround for wrongfully used stream type X 02 05 for AC3
                        if (p->type == 5)
                           audio_type = "AC3";
                        else
                           audio_type = "MP2";
                        break;
                     case sc_audio_AC3:
                        audio_type = "AC3"; break;
                     case sc_audio_HEAAC:
                        audio_type = "HEAAC"; break;
                  }
                  if (p->description)
                     audio << p->description << " (" << audio_type << ", " << p->language << ")";
                  else
                     audio << p->language << " (" << audio_type << ")";

                  break;
               case sc_subtitle:
                  if (firstSubtitle)
                     firstSubtitle = false;
                  else
                     subtitle << ", ";

                  if (p->description)
                     subtitle << p->description << " (" << p->language << ")";
                  else
                     subtitle << p->language;

                  break;
            }
         }
         if (audio.str().length() > 0)
            text << endl << tr("Audio") << ": "<< audio.str();
         if (subtitle.str().length() > 0)
            text << endl << tr("Subtitle") << ": "<< subtitle.str();
      }
   }

   if ( text.str().length() > 0 ) {
      textScroller.Set(osd, x2, y, x6 - x2, y5 - y, text.str().c_str(), smallfont, Theme.Color(clrMenuEventDescription), bg);
      SetTextScrollbar();
   }
}

string xml_substring(string source, const char* str_start, const char* str_end)
// returns the string between start and end or an empty string if not found
{
   size_t start = source.find(str_start);
   size_t end   = source.find(str_end);

   if (string::npos != start && string::npos != end) {
      return (source.substr(start + strlen(str_start), end - start - strlen(str_start)));
   }

   return string();
}

void cSkinElchiDisplayMenu::SetRecording(const cRecording *Recording)
{  ///< Sets the Recording that shall be displayed, using the entire central area
   ///< of the menu. The Recording's 'description' shall be displayed using a
   ///< cTextScroller, and the Scroll() function will be called to drive scrolling
   ///< that text if necessary.
   DSYSLOG("skinelchi: DisplayMenu::SetRecording(...) clp=%d", clear_pending)
   if (clear_pending) {
      Init(scrollable);
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventTime));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventTitle));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventDescription));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventShortText));
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuEventDescription));
      osd->DrawPixel(x0, y3, Theme.Color(clrBackground));
   }

   if (!Recording)
      return;
   const cRecordingInfo *Info = Recording->Info();
   const cFont *font = cFont::GetFont(fontOsd);
   const cFont *smallfont = cFont::GetFont(fontSml);
   tColor bg = Theme.Color(clrBackground);
   int y = y3;
   int x = x6 - x2;
   lh = font->Height();
   cTextScroller ts; // cTextScroller missbraucht für 3x cTextWrapper
   char t[32];
#if VDRVERSNUM >= 10721
   snprintf(t, sizeof(t), "%s  %s", *DateString(Recording->Start()), *TimeString(Recording->Start()));
#else
   snprintf(t, sizeof(t), "%s  %s", *DateString(Recording->start), *TimeString(Recording->start));
#endif
   ts.Set(osd, x2, y, x, y5 - y, t, font, Theme.Color(clrMenuEventTime), bg);

   y += ts.Height();

#if VDRVERSNUM >= 10711
   // Parentalrating
   if ((ElchiConfig.showRecDetails == 2) && Info->GetEvent()->ParentalRating()) {
      cString buffer = cString::sprintf(" %s ", *Info->GetEvent()->GetParentalRatingString());
      osd->DrawText(x2, y, buffer, Theme.Color(clrMenuEventVpsFg), Theme.Color(clrMenuEventVpsBg), smallfont);
   }
#endif
   y += lh;

   const char *Title = Info->Title();
   if (isempty(Title))
      Title = Recording->Name();
   ts.Set(osd, x2, y, x, y5 - y, Title, font, Theme.Color(clrMenuEventTitle), bg);
   y += ts.Height();

   if (!isempty(Info->ShortText())) {
      smallfont = cFont::GetFont(fontSml);
      ts.Set(osd, x2, y, x, y5 - y, Info->ShortText(), smallfont, Theme.Color(clrMenuEventShortText), bg);
      y += ts.Height();
   }

#if VDRVERSNUM >= 10711
   // Content
   if (ElchiConfig.showRecDetails == 2)
   {
      ostringstream content;
      bool firstContent = true;
      for (int i = 0; Info->GetEvent()->Contents(i); i++) {
         if (!isempty(Info->GetEvent()->ContentToString(Info->GetEvent()->Contents(i)))) { // skip empty (user defined) content
            if (!firstContent)
               content << ", ";
            content << Info->GetEvent()->ContentToString(Info->GetEvent()->Contents(i));
            firstContent = false;
         }
      }    
      if (content.str().length() > 0) {
         ts.Set(osd, x2, y, x, y5 - y, content.str().c_str(), smallfont, Theme.Color(clrMenuEventShortText), Theme.Color(clrBackground));
         y += ts.Height();
      }
   }
#endif
   y += lh;

   ostringstream text;
   text.imbue(std::locale(""));

   if (!isempty(Info->Description()))
      text << Info->Description() << endl << endl;

   if (ElchiConfig.showRecDetails > 0)
   {
      cChannel *channel = Channels.GetByChannelID(((cRecordingInfo *)Info)->ChannelID());
      if (channel)
         text << trVDR("Channel") << ": " << channel->Number() << " - " << channel->Name() << endl;

      cMarks marks;

#if VDRVERSNUM >= 10703
      bool hasMarks = marks.Load(Recording->FileName(), Recording->FramesPerSecond(), Recording->IsPesRecording()) && marks.Count();

      cIndexFile *index = new cIndexFile(Recording->FileName(), false, Recording->IsPesRecording());
#else
      bool hasMarks = marks.Load(Recording->FileName()) && marks.Count();

      cIndexFile *index = new cIndexFile(Recording->FileName(), false);
#endif
      int lastIndex = 0;

      int cuttedLength = 0;
      long cutinframe = 0;
      unsigned long long recsize = 0;
      unsigned long long recsizecutted = 0;
      unsigned long long cutinoffset = 0;
      unsigned long long filesize[100000];
      filesize[0] = 0;

      int i = 0;
      int imax = 999;
      struct stat filebuf;
      cString filename;
      int rc = 0;

      do {
#if VDRVERSNUM >= 10703
         if (Recording->IsPesRecording())
            filename = cString::sprintf("%s/%03d.vdr", Recording->FileName(), ++i);
         else {
            filename = cString::sprintf("%s/%05d.ts", Recording->FileName(), ++i);
            imax = 99999;
         }
#else
         filename = cString::sprintf("%s/%03d.vdr", Recording->FileName(), ++i);
#endif
         rc=stat(filename, &filebuf);
         if (rc == 0)
            filesize[i] = filesize[i-1] + filebuf.st_size;
         else
            if (ENOENT != errno) {
               esyslog ("skinelchi: error determining file size of \"%s\" %d (%s)", (const char *)filename, errno, strerror(errno));
               recsize = 0;
            }
      } while (i <= imax && !rc);
      recsize = filesize[i-1];

      if (hasMarks && index) {
#if VDRVERSNUM >= 10703
         uint16_t FileNumber;
         off_t FileOffset;
#else
         uchar FileNumber;
         int FileOffset;
#endif

         bool cutin = true;
         cMark *mark = marks.First();
         while (mark) {
#if VDRVERSNUM >= 10721
            long position = mark->Position();
#else
            long position = mark->position;
#endif
            index->Get(position, &FileNumber, &FileOffset);
            if (cutin) {
               cutinframe = position;
               cutin = false;
               cutinoffset = filesize[FileNumber-1] + FileOffset;
            } else {
               cuttedLength += position - cutinframe;
               cutin = true;
               recsizecutted += filesize[FileNumber-1] + FileOffset - cutinoffset;
            }
            cMark *nextmark = marks.Next(mark);
            mark = nextmark;
         }
         if (!cutin) {
            cuttedLength += index->Last() - cutinframe;
            index->Get(index->Last() - 1, &FileNumber, &FileOffset);
            recsizecutted += filesize[FileNumber-1] + FileOffset - cutinoffset;
         }
      }

      if (index) {
         lastIndex = index->Last();

#if VDRVERSNUM >= 10703
         text << tr("Length") << ": " << *IndexToHMSF(lastIndex, false, Recording->FramesPerSecond());
#else
         text << tr("Length") << ": " << *IndexToHMSF(lastIndex);
#endif
         if (hasMarks)
#if VDRVERSNUM >= 10703
            text << " (" << tr("cutted") << ": " << *IndexToHMSF(cuttedLength, false, Recording->FramesPerSecond()) << ")";
#else
            text << " (" << tr("cutted") << ": " << *IndexToHMSF(cuttedLength) << ")";
#endif
         text << endl;
      }
      delete index;

      if (recsize > MEGABYTE(1023))
         text << tr("Size") << ": " << fixed << setprecision(2) << (float)recsize / MEGABYTE(1024) << " GB";
      else
         text << tr("Size") << ": " << recsize / MEGABYTE(1) << " MB";
      if (hasMarks)
         if (recsize > MEGABYTE(1023))
            text << " (" <<  tr("cutted") << ": " << fixed << setprecision(2) <<  (float)recsizecutted/MEGABYTE(1024) << " GB)";
         else
            text << " (" << tr("cutted") << ": " <<  recsizecutted/MEGABYTE(1) << " MB)";

#if VDRVERSNUM >= 10721
      text << endl << trVDR("Priority") << ": " << Recording->Priority() << ", " << trVDR("Lifetime") << ": " << Recording->Lifetime() << endl;
#else
      text << endl << trVDR("Priority") << ": " << Recording->priority << ", " << trVDR("Lifetime") << ": " << Recording->lifetime << endl;
#endif

      if (lastIndex) {
#if VDRVERSNUM >= 10703
         text << tr("format") << ": " << (Recording->IsPesRecording() ? "PES" : "TS") << ", " << tr("bit rate") << ": ~ " << fixed << setprecision (2) << (float)recsize/lastIndex*Recording->FramesPerSecond()*8/MEGABYTE(1) << " MBit/s (Video + Audio)";
#else
         text << tr("bit rate") << ": ~ " << fixed << setprecision (2) << (float)recsize/lastIndex*FRAMESPERSEC*8/MEGABYTE(1) << " MBit/s (Video + Audio)";
#endif
   }

      const cComponents *Components = Info->Components();
      if (Components) {
         ostringstream audio;
         bool firstAudio = true;
         const char *audio_type = NULL;
         ostringstream subtitle;
         bool firstSubtitle = true;
         for (int i = 0; i < Components->NumComponents(); i++) {
            const tComponent *p = Components->Component(i);

            switch (p->stream) {
               case sc_video_MPEG2:
                  text << endl << tr("Video") << ": " <<  p->description << " (MPEG2)";
                  break;
               case sc_video_H264_AVC:
                  text << endl << tr("Video") << ": " <<  p->description << " (H.264)";
                  break;

               case sc_audio_MP2:
               case sc_audio_AC3:
               case sc_audio_HEAAC:
                  if (firstAudio)
                     firstAudio = false;
                  else
                     audio << ", ";

                  switch (p->stream) {
                     case sc_audio_MP2:
                        // workaround for wrongfully used stream type X 02 05 for AC3
                        if (p->type == 5)
                           audio_type = "AC3";
                        else
                           audio_type = "MP2";
                        break;
                     case sc_audio_AC3:
                        audio_type = "AC3"; break;
                     case sc_audio_HEAAC:
                        audio_type = "HEAAC"; break;
                  }
                  if (p->description)
                     audio << p->description << " (" << audio_type << ", " << p->language << ")";
                  else
                     audio << p->language << " (" << audio_type << ")";

                  break;
               case sc_subtitle:
                  if (firstSubtitle)
                     firstSubtitle = false;
                  else
                     subtitle << ", ";

                  if (p->description)
                     subtitle << p->description << " (" << p->language << ")";
                  else
                     subtitle << p->language;

                  break;
            }
         }
         if (audio.str().length() > 0)
            text << endl << tr("Audio") << ": "<< audio.str();
         if (subtitle.str().length() > 0)
            text << endl << tr("Subtitle") << ": "<< subtitle.str();
      }

      if (Info->Aux()) {
         string str_epgsearch = xml_substring(Info->Aux(), "<epgsearch>", "</epgsearch>");
         string channel, searchtimer, pattern;

         if (!str_epgsearch.empty()) {
            channel = xml_substring(str_epgsearch, "<channel>", "</channel>");
            searchtimer = xml_substring(str_epgsearch, "<searchtimer>", "</searchtimer>");
            if (searchtimer.empty())
               searchtimer = xml_substring(str_epgsearch, "<Search timer>", "</Search timer>");
         }

         string str_vdradmin = xml_substring(Info->Aux(), "<vdradmin-am>", "</vdradmin-am>");
         if (!str_vdradmin.empty()) {
            pattern = xml_substring(str_vdradmin, "<pattern>", "</pattern>");
         }

         if ((!channel.empty() && !searchtimer.empty()) || !pattern.empty())  {
            text << endl << endl << tr("additional information") << ":" << endl;
            if (!channel.empty() && !searchtimer.empty()) {
               text << "EPGsearch: " << tr("channel") << ": " << channel << ", " << tr("search pattern") << ": " << searchtimer;
            }
            if (!pattern.empty()) {
               text << "VDRadmin-AM: " << tr("search pattern") << ": " << pattern;
            }
         }
      }
   }

   if (text.str().length() > 0 ) {
      textScroller.Set(osd, x2, y, x6 - x2, y5 - y, text.str().c_str(), smallfont, Theme.Color(clrMenuEventDescription), bg);
      SetTextScrollbar();
   }
   changed = true;
}

void cSkinElchiDisplayMenu::SetText(const char *Text, bool FixedFont)
{  ///< Sets the Text that shall be displayed, using the entire central area
   ///< of the menu. The Text shall be displayed using a cTextScroller, and
   ///< the Scroll() function will be called to drive scrolling that text if
   ///< necessary.
   DSYSLOG("skinelchi: DisplayMenu::SetText(%s,%s) clp=%d", Text, FixedFont ? "'FixedFont'" : "'nonFixedFont'", clear_pending)
   if (clear_pending) {
      Init(scrollable);
      osd->DrawPixel(x0, y3, Theme.Color(clrMenuText));
      osd->DrawPixel(x0, y3, Theme.Color(clrBackground));
   }
   changed = true;
   textScroller.Set(osd, x2, menuTop, GetTextAreaWidth(), menuHeight, Text, GetTextAreaFont(FixedFont), Theme.Color(clrMenuText), Theme.Color(clrBackground));
   SetTextScrollbar();
}

int cSkinElchiDisplayMenu::GetTextAreaWidth(void) const
{  ///< Returns the width in pixel of the area which is used to display text
   ///< with SetText(). The width of the area is the width of the central area
   ///< minus the width of any possibly displayed scroll-bar or other decoration.
   ///< The default implementation returns 0. Therefore a caller of this method
   ///< must be prepared to receive 0 if the plugin doesn't implement this method.

   return x6 - x2;
}

const cFont *cSkinElchiDisplayMenu::GetTextAreaFont(bool FixedFont) const
{  ///< Returns a pointer to the font which is used to display text with SetText().
   ///< The parameter FixedFont has the same meaning as in SetText(). The default
   ///< implementation returns NULL. Therefore a caller of this method must be
   ///< prepared to receive NULL if the plugin doesn't implement this method.
   ///< The returned pointer is valid a long as the instance of cSkinDisplayMenu
   ///< exists.

   return cFont::GetFont(FixedFont ? fontFix : fontOsd);
}

void cSkinElchiDisplayMenu::Flush(void)
{  ///< Actually draws the OSD display to the output device.
   //DSYSLOG2("skinelchi: DisplayMenu::Flush()")

   if (clear_pending) {
      if (!osd) { // init OSD if not done yet
         DSYSLOG("skinelchi: forced Area to scrollable")
         Init(scrollable);
      }
      Clear2(); // for empty screens
   }

   //timeval tp1, tp2;
   //gettimeofday(&tp1, NULL);

   ///if (lasttime.tv_sec) isyslog("skinelchi Menu: FlushDelta=%.2f msec", (((long long)tp1.tv_sec * 1000000 + tp1.tv_usec) - ((long long)lasttime.tv_sec * 1000000 + lasttime.tv_usec)) / 1000.0);
   //lasttime=tp1;

   const cFont *font = cFont::GetFont(fontOsd);
   lh = font->Height();

#if APIVERSNUM >= 10728
   if (cVideoDiskUsage::HasChanged(lastDiskUsageState))
      DrawTitle();
#endif
   
   cString newdate = DayDateTime();
   if (!date || strcmp(date, newdate)) {
      changed = true;
      free (date);
      date = strdup(newdate);
      osd->DrawText(x6 - font->Width(date), y0, date, Theme.Color(clrMenuDate), Theme.Color(clrMenuTitleBg), font);
   }

#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   if (epgimagesize && curEventHasImage && epgimageThread) {
      if (epgimageTimer.TimedOut()) {
         cBitmap *bitmap = epgimageThread->GetNextBitmap();
         if (bitmap) {
            if (CurAreaNdx == 0) {
               osd->GetBitmap(0)->Reset();
               for (int i = 0; i < MAX_ELCHI_THEME_COLORS; i++)
                  osd->GetBitmap(0)->SetColor(i, ElchiConfig.ThemeColors[i]);
            }
            osd->DrawRectangle(x4, y3, x6 - 1, y4 - 1, Theme.Color(clrBackground));
            osd->DrawBitmap(x6 - bitmap->Width(), y3, *bitmap);
            epgimageThread->ReleaseNextBitmap();
            changed = true;
            epgimageTimer.Set(ElchiConfig.EpgImageDisplayTime * 1000);
         }
      }
   }
#endif

   if (CurrentItemScroll) changed |= CurrentItemScroll->DrawIfNeeded();
   if (TitleScroll) changed |= TitleScroll->DrawIfNeeded();

   if (!showMessage) {
      if (ElchiStatus->GetVolumeChange() != volumechange) {
         volumechange = ElchiStatus->GetVolumeChange();  //TODO change und volume zu einem Wert zusammen legen
                                                   // if volume != Vdrstatus.volume()
         int w = x6;
         int y = y5 + 4;
         int h = y6 - 5;
         int p = ((w - x2) * ElchiStatus->GetVolume()) / 255;
         int xp = x2 + p;
         osd->DrawRectangle(x2, y, xp - 1, h, Theme.Color(clrVolumeBarUpper));
         osd->DrawRectangle(xp, y, x6 - 1, h, Theme.Color(clrVolumeBarLower));
         showVolume = true;
         volumeTimer.Set(1000);
         changed = true;
      }
      else {
         if (showVolume && volumeTimer.TimedOut()) {
            tColor bg = Theme.Color(clrBackground);
            osd->DrawRectangle(x2, y5 + 4, x6 - 1, y6 - 5, bg);
            showVolume = false;
            changed = true;
         }
      }
   }

   if (ElchiConfig.clrdlgActive) {
      DSYSLOG2("skinelchi: colorchanging - DisplayMenu::Flush()")
      lastClrDlgFg = ElchiConfig.clrDlgFg;
      lastClrDlgBg = ElchiConfig.clrDlgBg;

      osd->SaveRegion(x0, y5, x8 - 1, y8 - 1);
      osd->DrawText(x0, y5, "Test Message",
         lastClrDlgFg, lastClrDlgBg, font, x8 - x0, y8 - y5, taCenter);
      osd->Flush();
      osd->RestoreRegion();
   }
   else {
      if (changed) {
         //DSYSLOG2("skinelchi: DisplayMenu::Flush()")
         //timeval tp1, tp2;
         //gettimeofday(&tp1, NULL);
         osd->Flush();
         //gettimeofday(&tp2, NULL);
         //DSYSLOG2("skinelchi: DisplayMenu::osd->Flush() %4.3f ms",
         //   (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0)
         changed = false;
      }
   }
   //gettimeofday(&tp2, NULL);
   //double msec = (((long long)tp2.tv_sec * 1000000 + tp2.tv_usec) - ((long long)tp1.tv_sec * 1000000 + tp1.tv_usec)) / 1000.0;
   //if (msec > 0.5) DSYSLOG2("skinelchi: DisplayMenu::Flush(): %4.3f ms", msec)
}



// ================================== class cEpgImage ===================================================
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
cEpgImage::cEpgImage(int maxColors)
:cThread("skinelchi-cEpgImage")
{
   DSYSLOG("skinelchi: DisplayMenu - cEpgImage::cEpgImage(maxColors=%d)", maxColors)
   maxcolors = maxColors;
   eventid = 0;

   switch (ElchiConfig.EpgImageSize) {
      case 1: x = 120; y = 90; break;
      case 3: x = 120; y = 120; break;
      default: x = 180; y = 144;
   }

   maxBitmap = 0;
   currenteventid = 0;
   currentBitmap = -1;
   lastBitmap = -1;
   Start();
}

cEpgImage::~cEpgImage()
{
   DSYSLOG("skinelchi: DisplayMenu - cEpgImage::~cEpgImage()")
   Stop();
}

void cEpgImage::Stop()
{
   if (Running()) {
      Cancel(-1);
      condWait.Signal();
   }
}


bool cEpgImage::PutEventID(u_int32_t EventID) {
   DSYSLOG("skinelchi: DisplayMenu - cEpgImage::PutEventID(%d)", EventID)

   struct stat statbuf;
   cString filename = cString::sprintf("%s/%d.png", ElchiConfig.GetEpgImageDir(), EventID);
   int rc = stat(filename, &statbuf);

   if (rc == 0) { // start thread (conversion) only if image available
      mtxeventid.Lock();
      eventid = EventID;
      mtxeventid.Unlock();
      condWait.Signal();
   }

   currentBitmap = -1;
   lastBitmap = -1;

   return (rc == 0); // return if img was found
}


cBitmap *cEpgImage::GetNextBitmap()
{
   mtxeventid.Lock();
   u_int32_t tmpeventid = eventid;
   mtxeventid.Unlock();

   mtxmaxBitmap.Lock();
   if ((maxBitmap == 0) ||
       (maxBitmap == 1 && currentBitmap == 0) ||
       (tmpeventid != currenteventid)) {
      mtxmaxBitmap.Unlock();
      return NULL;  // not yet ready
   }
   currentBitmap++;
   if (currentBitmap >= maxBitmap)
      currentBitmap = 0;

   DSYSLOG("skinelchi: GetNextBitmap (%d_%d) %08X", tmpeventid, currentBitmap, (long long)bmpEpgImages[currentBitmap]);
   return bmpEpgImages[currentBitmap];
}


void cEpgImage::ReleaseNextBitmap()
{
   mtxmaxBitmap.Unlock();
}


void cEpgImage::Action(void) {
   DSYSLOG("skinelchi: DisplayMenu - cEpgImage::Action()")
   cOSDImageBitmap image;
   u_int32_t tmpeventid = 0;
   bool convert = false;
   while (Running()) {
      // get actual event id (might be the same as before)
      mtxeventid.Lock();
      tmpeventid = eventid;
      mtxeventid.Unlock();

      if (tmpeventid != currenteventid) {
         // delete previous bitmaps
         mtxmaxBitmap.Lock();
         currenteventid = tmpeventid;
         for (int i = 0; i<maxBitmap; i++)
            delete bmpEpgImages[i];
         maxBitmap = 0;
         mtxmaxBitmap.Unlock();
         convert = true;
      }

      if (convert) {
         // new eventid or new images
         struct stat filebuf;
         cString imagefilename = cString::sprintf("%s/%d%s%s.%s", ElchiConfig.GetEpgImageDir(), currenteventid,
                  maxBitmap ? "_" : "", maxBitmap ? *itoa(maxBitmap) : "", "png");
         convert = (stat(imagefilename, &filebuf) == 0) && (maxBitmap <= MAXBITMAPS);
         if (convert) {
            DSYSLOG2("skinelchi: Loading EpgImage: %s", *imagefilename)
            if (!image.LoadZoomed(imagefilename, 0, 0, 0, 0)) {
               esyslog("skinelchi: ERROR loading EPG image '%s'", (const char *)imagefilename);
               convert = false;
            }
            else {
               bmpEpgImages[maxBitmap] = new cBitmap(8,8,1);
               image.Render(*bmpEpgImages[maxBitmap], x, y, maxcolors, 0);
               DSYSLOG("skinelchi: epgimage created %d_%d", currenteventid, maxBitmap)
               mtxmaxBitmap.Lock();
               maxBitmap++;
               mtxmaxBitmap.Unlock();
            }
         }
      }
      condWait.Wait(250);
   }

   // delete bmp images
   mtxmaxBitmap.Lock();
   for (int i = 0; i<maxBitmap; i++)
      delete bmpEpgImages[i];
   maxBitmap = 0;
   currenteventid = -1;
   mtxmaxBitmap.Unlock();

   DSYSLOG2("skinelchi: DisplayMenu - cEpgImage-thread end")
}

#endif
