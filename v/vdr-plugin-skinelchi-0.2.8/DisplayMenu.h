/*
 * DisplayMenu.h: Menu display handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __DISPLAYMENU_H
#define __DISPLAYMENU_H

#include <vdr/skins.h>

#include "common.h"
#include "scroll.h"
#include "config.h"

//for epgimages
//#ifdef SKINELCHI_HAVE_IMAGEMAGICK
#define MAXBITMAPS 10
class cEpgImage : public cThread
{
private:
   virtual void Action(void);
   u_int32_t eventid;
   int x, y; //, imagenumber;
   int maxcolors, maxBitmap, currentBitmap, lastBitmap;
   u_int32_t currenteventid;
   cBitmap *bmpEpgImages[MAXBITMAPS];
   cMutex mtxmaxBitmap; // protect maxBitamp and bmpEpgImages
   cMutex mtxeventid;   // protect eventID

   cCondWait condWait;
public:
   cEpgImage(int maxColors);
   ~cEpgImage();
   void Stop(void);
   cBitmap *GetNextBitmap(void);
   void ReleaseNextBitmap(void);
   bool PutEventID(u_int32_t EventID);
};
//#endif


class cSkinElchiDisplayMenu : public cSkinDisplayMenu
{
private:
   cOsd *osd;
   int CurAreaNdx;
   int epgimagesize;
   cEpgImage *epgimageThread;
   int epgimagecolors;
   char *buttons[4];
   char *title;
   cString title2;
   void DrawScrollbar(int Total, int Offset, int Shown, int Top, int Height, bool CanScrollUp, bool CanScrollDown);
   void SetTextScrollbar(void);
   void DrawTitle(void);
   bool showMessage, showVolume;
   cTimeMs volumeTimer;
   cTimeMs epgimageTimer;
   int volumechange;
   int x0, x1, x2, /*x3,*/ x4, x5, x6, x7, x8;
   int y0, y1, y2, y3, y4, y5, y6, y7, y8, y9;
   int lh, menuTop, menuHeight, currentIndex, area;
   cBitmap *bmNew, *bmRec, *bmArrowTurn, *bmClock, *bmClocksml, *bmVPS;
   static cBitmap bmANew, bmARec, bmAArrowTurn, bmAClock, bmAClocksml, bmAHD, bmAVPS;
   static cBitmap bmBNew, bmBRec, bmBArrowTurn, bmBClock, bmBClocksml, bmBHD, bmBVPS;
   static cBitmap bmCNew, bmCRec, bmCArrowTurn, bmCClock, bmCClocksml, bmCHD, bmCVPS;
   char *date;
   bool changed;
   int lastDiskUsageState;
   bool clear_pending;
   cCharScroll *CurrentItemScroll;
   cCharScroll *TitleScroll;
   void Clear2(void);
   enum areatype {none, items, scrollable, epg };
   areatype CurAreaType;
   void Init(areatype ReqAreaType);
   tOSDsize OSDsize;
   char *lastCurrentText;
   tColor lastClrDlgFg;
   tColor lastClrDlgBg;

   u_int32_t eventID;
   int timercheck;
   bool curEventHasImage;
   timeval lasttime;

public:
   cSkinElchiDisplayMenu(void);
   virtual ~cSkinElchiDisplayMenu();
   virtual void Scroll(bool Up, bool Page);
   virtual int MaxItems(void);
   virtual void Clear(void);
   virtual void SetTitle(const char *Title);
   virtual void SetButtons(const char *Red, const char *Green = NULL, const char *Yellow = NULL, const char *Blue = NULL);
   virtual void SetMessage(eMessageType Type, const char *Text);
   virtual void SetItem(const char *Text, int Index, bool Current, bool Selectable);
   virtual void SetScrollbar(int Total, int Offset);
   virtual void SetEvent(const cEvent *Event);
   virtual void SetRecording(const cRecording *Recording);
   virtual void SetText(const char *Text, bool FixedFont);
   virtual int GetTextAreaWidth(void) const;
   virtual const cFont *GetTextAreaFont(bool FixedFont) const;
   virtual void Flush(void);
};

#endif //__DISPLAYMENU_H
