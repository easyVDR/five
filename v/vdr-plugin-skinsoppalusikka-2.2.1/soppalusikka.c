/*
 * skinsoppalusikka.c: The 'soppalusikka' VDR skin
 *
 * $Id: $
 */

#include "common.h"
#include "tools.h"
#include "config.h"
#include "logo.h"
#include "symbol.h"
#include "soppalusikka.h"
#include <math.h>
#include <ctype.h>
#include <vdr/device.h>
#include <vdr/timers.h>
#include <vdr/menu.h>
#include <vdr/font.h>
#include <vdr/osd.h>
#include <vdr/themes.h>
#include <vdr/plugin.h>
#include <vdr/videodir.h>

#define GetSymbol(id)     SoppalusikkaSymbolCache.Get(id)
#define TinyGap           SoppalusikkaSymbolCache.GetGapTiny()
#define SmallGap          SoppalusikkaSymbolCache.GetGapSmall()
#define Gap               SoppalusikkaSymbolCache.GetGapNormal()
#define BigGap            SoppalusikkaSymbolCache.GetGapBig()
#define Roundness         SoppalusikkaSymbolCache.GetRoundness()

#define ChannelLogoWidth  SoppalusikkaLogoCache.GetLogoWidth()
#define ChannelLogoHeight SoppalusikkaLogoCache.GetLogoHeight()

static cTheme ThemeS;

THEME_CLR(ThemeS, clrBackground,             0xC8000066);
THEME_CLR(ThemeS, clrButtonRedFg,            0xFFFFFFFF);
THEME_CLR(ThemeS, clrButtonRedBg,            0xC8CC1111);
THEME_CLR(ThemeS, clrButtonGreenFg,          0xFF000000);
THEME_CLR(ThemeS, clrButtonGreenBg,          0xC822BB22);
THEME_CLR(ThemeS, clrButtonYellowFg,         0xFF000000);
THEME_CLR(ThemeS, clrButtonYellowBg,         0xC8EEEE22);
THEME_CLR(ThemeS, clrButtonBlueFg,           0xFFFFFFFF);
THEME_CLR(ThemeS, clrButtonBlueBg,           0xC82233CC);
THEME_CLR(ThemeS, clrMessageStatusFg,        0xFFFFEE00);
THEME_CLR(ThemeS, clrMessageStatusBg,        0xC8000066);
THEME_CLR(ThemeS, clrMessageInfoFg,          0xFFFFEE00);
THEME_CLR(ThemeS, clrMessageInfoBg,          0xC8000066);
THEME_CLR(ThemeS, clrMessageWarningFg,       0xFFFFEE00);
THEME_CLR(ThemeS, clrMessageWarningBg,       0xC8000066);
THEME_CLR(ThemeS, clrMessageErrorFg,         0xFFFFEE00);
THEME_CLR(ThemeS, clrMessageErrorBg,         0xC8000066);
THEME_CLR(ThemeS, clrVolumePromptFg,         0xFF000000);
THEME_CLR(ThemeS, clrVolumePromptBg,         0xFFFFFFFF);
THEME_CLR(ThemeS, clrVolumePromptBorder,     0xFF000000);
THEME_CLR(ThemeS, clrVolumeBarUpper,         0xC8000066);
THEME_CLR(ThemeS, clrVolumeBarLower,         0xFF33CC33);
THEME_CLR(ThemeS, clrVolumeBarMute,          0xFFFF0000);
THEME_CLR(ThemeS, clrChannelNameFg,          0xFF000000);
THEME_CLR(ThemeS, clrChannelNameBg,          0xFFFFFFFF);
THEME_CLR(ThemeS, clrChannelNumberDateFg,    0xFF000000);
THEME_CLR(ThemeS, clrChannelNumberDateBg,    0xFFFFFFFF);
THEME_CLR(ThemeS, clrChannelEpgTimeFg,       0xFFDDDDDD);
THEME_CLR(ThemeS, clrChannelEpgTimeBg,       0xC8CC1111);
THEME_CLR(ThemeS, clrChannelEpgTitle,        0xFF00FFFF);
THEME_CLR(ThemeS, clrChannelEpgShortText,    0xFFCCBB22);
THEME_CLR(ThemeS, clrChannelEpgDuration,     0xFF7F7F7F);
THEME_CLR(ThemeS, clrChannelTimeBarFg,       0xFF7F7F7F);
THEME_CLR(ThemeS, clrChannelTimeBarBg,       0xFF000000);
THEME_CLR(ThemeS, clrChannelSymbolActive,    0xFFCCBB22);
THEME_CLR(ThemeS, clrChannelSymbolInactive,  0xFF7F7F7F);
THEME_CLR(ThemeS, clrChannelSymbolRecord,    0xFFFF0000);
THEME_CLR(ThemeS, clrMenuTitleFg,            0xFF000000);
THEME_CLR(ThemeS, clrMenuTitleBg,            0xC833AAEE);
THEME_CLR(ThemeS, clrMenuItemCurrentFg,      0xFF000000);
THEME_CLR(ThemeS, clrMenuItemCurrentBg,      0xC833AAEE);
THEME_CLR(ThemeS, clrMenuItemSelectable,     0xFFFFFFFF);
THEME_CLR(ThemeS, clrMenuEventTime,          0xFFFFFFFF);
THEME_CLR(ThemeS, clrMenuEventTitle,         0xFFCCBB22);
THEME_CLR(ThemeS, clrMenuEventShortText,     0xFFCCBB22);
THEME_CLR(ThemeS, clrMenuEventDescription,   0xFF00FFFF);
THEME_CLR(ThemeS, clrMenuEventSymbolFg,      0xFFCCBB22);
THEME_CLR(ThemeS, clrMenuEventSymbolBg,      0xC8000066);
THEME_CLR(ThemeS, clrMenuScrollbarTotal,     0xC8000066);
THEME_CLR(ThemeS, clrMenuScrollbarShown,     0xFFCCBB22);
THEME_CLR(ThemeS, clrMenuText,               0xFFCCBB22);
THEME_CLR(ThemeS, clrReplayTitleFg,          0xFF000000);
THEME_CLR(ThemeS, clrReplayTitleBg,          0xFFFFFFFF);
THEME_CLR(ThemeS, clrReplayTitleBorder,      0xFF000000);
THEME_CLR(ThemeS, clrReplayCurrent,          0xFFFFFFFF);
THEME_CLR(ThemeS, clrReplayTotal,            0xFFFFFFFF);
THEME_CLR(ThemeS, clrReplayModeJump,         0xFFFFFFFF);
THEME_CLR(ThemeS, clrReplaySymbolActive,     0xFFCCBB22);
THEME_CLR(ThemeS, clrReplaySymbolInactive,   0xFF7F7F7F);
THEME_CLR(ThemeS, clrReplayProgressSeen,     0xFF33CC33);
THEME_CLR(ThemeS, clrReplayProgressRest,     0xFFFFFFFF);
THEME_CLR(ThemeS, clrReplayProgressSelected, 0xFFFF0000);
THEME_CLR(ThemeS, clrReplayProgressMark,     0xFF000000);
THEME_CLR(ThemeS, clrReplayProgressCurrent,  0xFFFF0000);

// --- cSkinSoppalusikkaDisplayChannel --------------------------------------------

class cSkinSoppalusikkaDisplayChannel : public cSkinDisplayChannel {
private:
  enum {
    SIGNALUPDATETIMEOUT_MS = 500
  };
  cOsd *osdM;
  bool isLogoM;
  int x0M, x1M;
  int xt0M, xt1M, xt2M, xt3M, xt4M, xt5M, xt6M, xt7M, xt8M, xt9M;
  int xb0M, xb1M, xb2M, xb3M, xb4M, xb5M, xb6M;
  int xs0M, xs1M;
  int y0M, y1M;
  int yt0, yt1M, yt2M, yt3M, yt4M;
  int yb0, yb1M, yb2M, yb3M, yb4M, yb5M;
  int ys0, ys1M;
  cString lastDateM;
  cTimeMs lastSignalUpdateM;
  bool HasChannelTimerRecording(const cChannel *channelP);
  void ResetTopAreaCoordinates(bool isLogoP = false);
  void DrawTopArea(const cChannel *channel = NULL);
  void DrawBottomArea(void);
  void DrawSignalArea(void);
  cString GetChannelName(const cChannel *channelP);
  cString GetChannelNumber(const cChannel *channelP, int numberP);

public:
  cSkinSoppalusikkaDisplayChannel(bool withInfoP);
  virtual ~cSkinSoppalusikkaDisplayChannel();
  virtual void SetChannel(const cChannel *channelP, int numberP);
  virtual void SetEvents(const cEvent *presentP, const cEvent *followingP);
  virtual void SetMessage(eMessageType typeP, const char *textP);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayChannel::cSkinSoppalusikkaDisplayChannel(bool withInfoP)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int lineHeightM = font->Height();
  isLogoM = false;
  lastSignalUpdateM.Set(0);
  // general coordinates
  x0M = 0;
  x1M = cOsd::OsdWidth();
  // top area x-coordinates
  xt9M = x1M - BigGap;
  xt0M = x0M + BigGap;
  xt1M = xt0M;
  xt2M = xt1M + Roundness;
  xt3M = xt2M + font->Width("0000-");
  xt8M = xt9M - Roundness;
  xt7M = xt8M - GetSymbol(SYMBOL_TELETEXT).Width() - GetSymbol(SYMBOL_AUDIO).Width() - GetSymbol(SYMBOL_DOLBY_DIGITAL).Width() - GetSymbol(SYMBOL_ENCRYPTED).Width() - GetSymbol(SYMBOL_RECORDING).Width() - 7 * BigGap;
  if (SoppalusikkaConfig.GetShowVps())
     xt7M -= GetSymbol(SYMBOL_VPS).Width();
  xt6M = xt7M - Roundness;
  xt5M = xt6M - 5 * BigGap;
  xt4M = xt5M - Roundness;
  // signal area x-coordinates
  xs0M = xt8M;
  xs1M = xt8M;
  // bottom area x-coordinates
  xb6M = x1M - BigGap;
  xb0M = x0M + BigGap;
  xb1M = xb0M + Roundness;
  xb2M = xb1M + font->Width("00:00") + BigGap;
  xb3M = xb2M + 3 * Gap;
  xb4M = xb3M + BigGap;
  xb5M = xb6M - Roundness;
  // top area y-coordinates
  y0M = 0;
  yt0 = y0M + BigGap;
  yt1M = yt0 + Roundness;
  yt2M = yt0 + lineHeightM;
  yt3M = yt2M + SmallGap;
  yt4M = yt3M + lineHeightM;
  // signal area y-coordinates
  ys1M = yt4M - 4 * Gap;
  ys0 = ys1M - 4 * Gap;
  // bottom area y-coordinates
  yb0 = yt4M;
  yb1M = yb0 + lineHeightM;
  yb2M = yb1M + lineHeightM;
  yb3M = yb2M + lineHeightM;
  yb5M = yb3M + lineHeightM;
  yb4M = yb5M - Roundness;
  y1M = yb5M;
  // create osd
  osdM = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop() + (Setup.ChannelInfoPos ? 0 : cOsd::OsdHeight() - y1M));
  tArea Areas[] = { { x0M, y0M, x1M - 1, y1M - 1, 8 } };
  if (Setup.AntiAlias && osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     if (SoppalusikkaConfig.GetShowLogo()) {
        tArea Areas[] = { { x0M, y0M, x0M + ChannelLogoWidth - 1, y0M + ChannelLogoHeight - 1, 4 },
                          { x0M + ChannelLogoWidth, y0M, x1M - 1, y0M + ChannelLogoHeight - 1, 4 },
                          { x0M, y0M + ChannelLogoHeight, x1M - 1, y1M - 1, 4 }
                        };
        if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
           osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        }
     else {
        tArea Areas[] = { { x0M, y0M, x1M - 1, y1M - 1, 4 } };
        if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
           osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        }
     }
  // clear all
  osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
  // draw top area
  DrawTopArea(NULL);
  // draw bottom area
  DrawBottomArea();
}

cSkinSoppalusikkaDisplayChannel::~cSkinSoppalusikkaDisplayChannel()
{
  delete osdM;
}

bool cSkinSoppalusikkaDisplayChannel::HasChannelTimerRecording(const cChannel *channelP)
{
  // try to find current channel from timers
  for (cTimer *t = Timers.First(); t; t = Timers.Next(t)) {
      if ((t->Channel() == channelP) && t->Recording())
         return true;
      }
  return false;
}

void cSkinSoppalusikkaDisplayChannel::ResetTopAreaCoordinates(bool isLogoP)
{
  const cFont *font = cFont::GetFont(fontOsd);
  // x-coordinates
  xt1M = (isLogoP ? x0M + ChannelLogoWidth + TinyGap : xt0M);
  xt2M = xt1M + Roundness;
  xt3M = xt2M + font->Width("0000-");
}

void cSkinSoppalusikkaDisplayChannel::DrawTopArea(const cChannel *channelP)
{
  // draw logo stuff
  if (SoppalusikkaConfig.GetShowLogo() && channelP &&
     (SoppalusikkaLogoCache.Load(*channelP->GetChannelID().ToString()) || SoppalusikkaLogoCache.Load(channelP->Name()))) {
     // load channel logo
     isLogoM = true;
     // set top area for logo
     ResetTopAreaCoordinates(isLogoM);
     // draw missing bling-blings
     osdM->DrawRectangle(x0M, y0M, x0M + TinyGap + ChannelLogoWidth - 1, y0M + SmallGap + ChannelLogoHeight - 1, ThemeS.Color(clrBackground));
     osdM->DrawRectangle(xt0M, y0M + SmallGap + ChannelLogoHeight, xt1M - 1, yt4M - 1, ThemeS.Color(clrChannelNameBg));
     // draw logo
     osdM->DrawBitmap(x0M, y0M, SoppalusikkaLogoCache.Get(), 0, 0, true);
     }
  else {
     // clear possible logo leftovers
     if (isLogoM) {
        osdM->DrawRectangle(x0M, y0M, xt1M - 1, yt4M - 1, clrTransparent);
        }
     // no logo
     isLogoM = false;
     // set top area for logoless
     ResetTopAreaCoordinates(isLogoM);
     }
  // draw clock row
  osdM->DrawRectangle(xt1M, yt0, xt4M - 1, yt2M - 1, ThemeS.Color(clrChannelNumberDateBg));
  osdM->DrawRectangle(xt4M, yt0, xt9M - 1, yt2M - 1, ThemeS.Color(clrBackground));
  // draw rounded left corner of clock row
  if (!isLogoM)
     osdM->DrawEllipse(xt1M, yt0, xt2M - 1, yt1M - 1, clrTransparent, -2);
  // draw rounded right edge of clock row
  osdM->DrawEllipse(xt4M, yt0, xt5M, yt2M - 1, ThemeS.Color(clrChannelNumberDateBg), 5);
  // draw rounded right corner of clock row
  osdM->DrawEllipse(xt8M, yt0, xt9M - 1, yt1M - 1, clrTransparent, -1);
  // draw separator
  osdM->DrawRectangle(xt1M, yt2M, xt9M - 1, yt3M - 1, ThemeS.Color(clrBackground));
  // draw channel row
  osdM->DrawRectangle(xt1M, yt3M, xt6M - 1, yt4M - 1, ThemeS.Color(clrChannelNameBg));
  osdM->DrawRectangle(xt6M, yt3M, xt9M - 1, yt4M - 1, ThemeS.Color(clrBackground));
  // draw rounded right edge of channel row
  osdM->DrawEllipse(xt6M, yt3M, xt7M, yt4M - 1, ThemeS.Color(clrChannelNameBg), 5);
}

void cSkinSoppalusikkaDisplayChannel::DrawBottomArea(void)
{
  // draw time area
  osdM->DrawRectangle(xb0M, yb0, xb2M - 1, yb5M - 1, ThemeS.Color(clrChannelEpgTimeBg));
  // draw rounded left corner
  osdM->DrawEllipse(xb0M, yb4M, xb1M - 1, yb5M - 1, clrTransparent, -3);
  // draw timebar area
  osdM->DrawRectangle(xb2M, yb0, xb3M - 1, yb5M - 1, ThemeS.Color(clrChannelTimeBarBg));
  // draw event area
  osdM->DrawRectangle(xb3M, yb0, xb6M - 1, yb5M - 1, ThemeS.Color(clrBackground));
  // draw rounded right corner
  osdM->DrawEllipse(xb5M, yb4M, xb6M - 1, yb5M - 1, clrTransparent, -4);
}

void cSkinSoppalusikkaDisplayChannel::DrawSignalArea(void)
{
  int x;
  int s = cDevice::ActualDevice()->SignalStrength();
  int q = cDevice::ActualDevice()->SignalQuality();
  // draw signal strength
  x = (xs1M - xs0M - 1) * (s < 0 ? 0 : s) / 100;
  osdM->DrawRectangle(xs0M, ys0, xs1M - 1, ys0 + Gap, ThemeS.Color(clrChannelSymbolInactive));
  osdM->DrawRectangle(xs0M, ys0, xs0M + x - 1, ys0 + Gap, ThemeS.Color(clrChannelSymbolActive));
  // draw signal quality
  x = (xs1M - xs0M - 1) * (q < 0 ? 0 : q) / 100;
  osdM->DrawRectangle(xs0M, ys0 + 2 * Gap, xs1M - 1, ys0 + 3 * Gap, ThemeS.Color(clrChannelSymbolInactive));
  osdM->DrawRectangle(xs0M, ys0 + 2 * Gap, xs0M + x - 1, ys0 + 3 * Gap, ThemeS.Color(clrChannelSymbolActive));
  // update timer
  lastSignalUpdateM.Set(SIGNALUPDATETIMEOUT_MS);
}

cString cSkinSoppalusikkaDisplayChannel::GetChannelName(const cChannel *channelP)
{
  char buffer[256];
  // check if channel exists
  if (channelP) {
     snprintf(buffer, sizeof(buffer), "%s", channelP->Name());
     }
  else {
     snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));
     }
  return buffer;
}

cString cSkinSoppalusikkaDisplayChannel::GetChannelNumber(const cChannel *channelP, int numberP)
{
  char buffer[256];
  // check if real channel exists
  if (channelP && !channelP->GroupSep()) {
     snprintf(buffer, sizeof(buffer), "%d%s", channelP->Number(), numberP ? "-" : "");
     }
  else if (numberP) {
     // no channel but number
     snprintf(buffer, sizeof(buffer), "%d-", numberP);
     }
  else {
     // no channel and no number
     snprintf(buffer, sizeof(buffer), " ");
     }
  return buffer;
}

void cSkinSoppalusikkaDisplayChannel::SetChannel(const cChannel *channelP, int numberP)
{
  // draw top area
  DrawTopArea(channelP);
  // draw channel number
  osdM->DrawText(xt2M, yt0, GetChannelNumber(channelP, numberP), ThemeS.Color(clrChannelNumberDateFg), ThemeS.Color(clrChannelNumberDateBg), cFont::GetFont(fontSml), xt3M - xt2M, yt2M - yt0);
  // draw channel name
  osdM->DrawText(xt2M, yt3M, GetChannelName(channelP), ThemeS.Color(clrChannelNameFg), ThemeS.Color(clrChannelNameBg), cFont::GetFont(fontSml), xt6M - xt2M, yt4M - yt3M);
  // draw symbols
  if (channelP && !channelP->GroupSep()) {
     int xs = xt8M;
     bool isvps = false;
     // check if vps
     if (SoppalusikkaConfig.GetShowVps()) {
        // get schedule
        cSchedulesLock SchedulesLock;
        const cSchedules *Schedules = cSchedules::Schedules(SchedulesLock);
        if (Schedules) {
           const cSchedule *Schedule = Schedules->GetSchedule(channelP);
           if (Schedule) {
              // get present event
              const cEvent *Event = Schedule->GetPresentEvent();
              // check if present event has vps
              if (Event && Event->Vps()) {
                 isvps = true;
                 }
              }
           }
        }
     // draw radio symbol if no video PID; otherwise draw teletext symbol
     if (channelP->Vpid()) {
        xs -= GetSymbol(SYMBOL_TELETEXT).Width();
        osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_TELETEXT).Height()) / 2, GetSymbol(SYMBOL_TELETEXT), ThemeS.Color(channelP->Tpid() ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
        }
     else {
        xs -= GetSymbol(SYMBOL_RADIO).Width();
        osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_RADIO).Height()) / 2, GetSymbol(SYMBOL_RADIO), ThemeS.Color(channelP->Apid(0) ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
        }
     // draw audio symbol according to current audio channel
     switch (cDevice::PrimaryDevice()->GetAudioChannel()) {
       default:
       case 0: /* stereo */
            xs -= (GetSymbol(SYMBOL_AUDIO).Width() + BigGap);
            osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_AUDIO).Height()) / 2, GetSymbol(SYMBOL_AUDIO), ThemeS.Color(channelP->Apid(1) ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
            break;
       case 1: /* mono left */
            xs -= (GetSymbol(SYMBOL_AUDIO_LEFT).Width() + BigGap);
            osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_AUDIO_LEFT).Height()) / 2, GetSymbol(SYMBOL_AUDIO_LEFT), ThemeS.Color(channelP->Apid(1) ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
            break;
       case 2: /* mono right */
            xs -= (GetSymbol(SYMBOL_AUDIO_RIGHT).Width() + 2 * BigGap);
            osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_AUDIO_RIGHT).Height()) / 2, GetSymbol(SYMBOL_AUDIO_RIGHT), ThemeS.Color(channelP->Apid(1) ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
            break;
        }
     // draw dolby digital symbol
     xs -= (GetSymbol(SYMBOL_DOLBY_DIGITAL).Width() + BigGap);
     osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_DOLBY_DIGITAL).Height()) / 2, GetSymbol(SYMBOL_DOLBY_DIGITAL), ThemeS.Color(channelP->Dpid(0) ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
     // draw encryption symbol
     xs -= (GetSymbol(SYMBOL_ENCRYPTED).Width() + BigGap);
     osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_ENCRYPTED).Height()) / 2, GetSymbol(SYMBOL_ENCRYPTED), ThemeS.Color(channelP->Ca() ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
     // draw vps symbol
     if (SoppalusikkaConfig.GetShowVps()) {
        xs -= (GetSymbol(SYMBOL_VPS).Width() + BigGap);
        osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_VPS).Height()) / 2, GetSymbol(SYMBOL_VPS), ThemeS.Color(isvps ? clrChannelSymbolActive : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
        }
     // draw recording symbol
     xs -= (GetSymbol(SYMBOL_RECORDING).Width() + BigGap);
     osdM->DrawBitmap(xs, yt0 + (ys1M - yt0 - GetSymbol(SYMBOL_RECORDING).Height()) / 2, GetSymbol(SYMBOL_RECORDING), ThemeS.Color(cRecordControls::Active() ? (HasChannelTimerRecording(channelP) ? clrChannelSymbolRecord : clrChannelSymbolActive) : clrChannelSymbolInactive), ThemeS.Color(clrBackground));
     // draw signal area
     xs0M = xs;
     ys0 = (yt0 + ys1M + GetSymbol(SYMBOL_RECORDING).Height()) / 2 + Gap;
     DrawSignalArea();
     }
  lastDateM = NULL;
}

void cSkinSoppalusikkaDisplayChannel::SetEvents(const cEvent *presentP, const cEvent *followingP)
{
  // draw bottom area
  DrawBottomArea();
  // check epg datas
  const cEvent *e = presentP;
  if (e) {
     cString s;
     int total = e->Duration();
     int now = int(time(NULL) - e->StartTime());
     if ((now < total) && ((now / 60) > 0))
        s = cString::sprintf("  %d / %d %s", now / 60, (SoppalusikkaConfig.GetShowDuration() ? total : (total - now)) / 60, tr("min"));
     else
        s = cString::sprintf("  %d %s", total / 60, tr("min"));
     // draw start time
     osdM->DrawText(xb1M, yb0, e->GetTimeString(), ThemeS.Color(clrChannelEpgTimeFg), ThemeS.Color(clrChannelEpgTimeBg), cFont::GetFont(fontOsd), xb2M - xb1M, yb1M - yb0);
     // draw title
     osdM->DrawText(xb4M, yb0, e->Title(), ThemeS.Color(clrChannelEpgTitle), ThemeS.Color(clrBackground), cFont::GetFont(fontOsd), xb5M - xb4M, yb1M - yb0);
     // draw timer symbol
     if (e->HasTimer()) {
        cString space("  ");
        osdM->DrawText(xb5M - cFont::GetFont(fontSml)->Width(s) - cFont::GetFont(fontSml)->Width(space), yb0, space, ThemeS.Color(clrChannelEpgDuration), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), cFont::GetFont(fontSml)->Width(space), yb1M - yb0);
        osdM->DrawBitmap(xb5M - cFont::GetFont(fontSml)->Width(s) - GetSymbol(SYMBOL_EVENT_TIMER).Width(), yb0 + (yb1M - yb0 - GetSymbol(SYMBOL_EVENT_TIMER).Height()) / 2, GetSymbol(SYMBOL_EVENT_TIMER), ThemeS.Color(clrChannelEpgDuration), ThemeS.Color(clrBackground));
        }
     // draw duration
     osdM->DrawText(xb5M - cFont::GetFont(fontSml)->Width(s), yb0, s, ThemeS.Color(clrChannelEpgDuration), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), cFont::GetFont(fontSml)->Width(s), yb1M - yb0);
     // draw vps time
     if (SoppalusikkaConfig.GetShowVps() && e->Vps() && (e->Vps() != e->StartTime())) {
        /* difference between start time and vps time in minutes */
        int delta = int(e->StartTime() - e->Vps()) / 60;
        /* check if difference is less than 10 hours */
        if (abs(delta) < 600) {
           /* relative vps time formats: "+0:30" "-1:30" */
           cString vps = cString::sprintf("%c%01d:%02d", delta < 0 ? '-' : '+', abs(delta) / 60, abs(delta) % 60);
           osdM->DrawText(xb1M, yb1M, vps, ThemeS.Color(clrChannelEpgShortText), ThemeS.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2M - xb1M - Gap, yb2M - yb1M, taRight);
           }
        else {
           /* absolute vps time format: "18:45" */
           osdM->DrawText(xb1M, yb1M, TimeString(e->Vps()), ThemeS.Color(clrChannelEpgShortText), ThemeS.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2M - xb1M - Gap, yb2M - yb1M, taRight);
           }
        }
     // draw shorttext
     osdM->DrawText(xb4M, yb1M, e->ShortText(), ThemeS.Color(clrChannelEpgShortText), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), xb5M - xb4M - Gap, yb2M - yb1M);
     // draw timebar
     int yc = yb0 + (int)(roundf((float)(now) / (float)(total) * (float)(yb5M - yb0)));
     yc = min(yc, yb5M);
     osdM->DrawRectangle(xb2M + SmallGap, yb0, xb3M - SmallGap - 1, yc - 1, ThemeS.Color(clrChannelTimeBarFg));
     }
  e = followingP;
  if (e) {
     cString s = cString::sprintf("  %d %s", e->Duration() / 60, tr("min"));
     // draw start time
     osdM->DrawText(xb1M, yb2M, e->GetTimeString(), ThemeS.Color(clrChannelEpgTimeFg), ThemeS.Color(clrChannelEpgTimeBg), cFont::GetFont(fontOsd), xb2M - xb1M, yb3M - yb2M);
     // draw title
     osdM->DrawText(xb4M, yb2M, e->Title(), ThemeS.Color(clrChannelEpgTitle), ThemeS.Color(clrBackground), cFont::GetFont(fontOsd), xb5M - xb4M, yb3M - yb2M);
     // draw timer symbol
     if (e->HasTimer()) {
        cString space("  ");
        osdM->DrawText(xb5M - cFont::GetFont(fontSml)->Width(s) - cFont::GetFont(fontSml)->Width(space), yb2M, space, ThemeS.Color(clrChannelEpgDuration), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), cFont::GetFont(fontSml)->Width(space), yb3M - yb2M);
        osdM->DrawBitmap(xb5M - cFont::GetFont(fontSml)->Width(s) - GetSymbol(SYMBOL_EVENT_TIMER).Width(), yb2M + (yb3M - yb2M - GetSymbol(SYMBOL_EVENT_TIMER).Height()) / 2, GetSymbol(SYMBOL_EVENT_TIMER), ThemeS.Color(clrChannelEpgDuration), ThemeS.Color(clrBackground));
        }
     // draw duration
     osdM->DrawText(xb5M - cFont::GetFont(fontSml)->Width(s), yb2M, s, ThemeS.Color(clrChannelEpgDuration), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), cFont::GetFont(fontSml)->Width(s), yb3M - yb2M);
     // draw vps time - only if skin dependent small fonts
     if (SoppalusikkaConfig.GetShowVps() && e->Vps() && (e->Vps() != e->StartTime())) {
        /* difference between start time and vps time in minutes */
        int delta = int(e->StartTime() - e->Vps()) / 60;
        /* check if difference is less than 10 hours */
        if (abs(delta) < 600) {
           /* relative vps time formats: "+0:30" "-1:30" */
           cString vps = cString::sprintf("%c%d:%02d", delta < 0 ? '-' : '+', abs(delta) / 60, abs(delta) % 60);
           osdM->DrawText(xb1M, yb3M, vps, ThemeS.Color(clrChannelEpgShortText), ThemeS.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2M - xb1M - Gap, yb5M - yb3M, taRight);
           }
        else {
           /* absolute vps time format: "18:45" */
           osdM->DrawText(xb1M, yb3M, TimeString(e->Vps()), ThemeS.Color(clrChannelEpgShortText), ThemeS.Color(clrChannelEpgTimeBg), cFont::GetFont(fontSml), xb2M - xb1M - Gap, yb5M - yb3M, taRight);
           }
        }
     // draw shorttext
     osdM->DrawText(xb4M, yb3M, e->ShortText(), ThemeS.Color(clrChannelEpgShortText), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), xb5M - xb4M, yb5M - yb3M);
     }
}

void cSkinSoppalusikkaDisplayChannel::SetMessage(eMessageType typeP, const char *textP)
{
  // check if message
  if (textP) {
     // save osd region
     osdM->SaveRegion(xb0M, yb3M, xb6M - 1, yb5M - 1);
     // draw message area
     osdM->DrawRectangle(xb0M, yb3M, xb6M - 1, yb5M - 1, ThemeS.Color(clrMessageStatusBg + 2 * typeP));
     // draw message
     osdM->DrawText(xb1M, yb3M, textP, ThemeS.Color(clrMessageStatusFg + 2 * typeP), ThemeS.Color(clrMessageStatusBg + 2 * typeP), cFont::GetFont(fontOsd), xb5M - xb1M, yb5M - yb3M, taCenter);
     // draw rounded left corner
     osdM->DrawEllipse(xb0M, yb4M, xb1M - 1, yb5M - 1, clrTransparent, -3);
     // draw rounded right corner
     osdM->DrawEllipse(xb5M, yb4M, xb6M - 1, yb5M - 1, clrTransparent, -4);
     }
  else {
     // restore saved osd region
     osdM->RestoreRegion();
     }
}

void cSkinSoppalusikkaDisplayChannel::Flush(void)
{
  cString date = DayDateTime();
  if (!*lastDateM || strcmp(date, lastDateM)) {
     // draw updated date string
     osdM->DrawText(xt3M, yt0, date, ThemeS.Color(clrChannelNumberDateFg), ThemeS.Color(clrChannelNumberDateBg), cFont::GetFont(fontSml), xt4M - xt3M, yt2M - yt0, taRight);
     lastDateM = date;
     }
  if (lastSignalUpdateM.TimedOut())
     DrawSignalArea();
  osdM->Flush();
}

// --- cSkinSoppalusikkaDisplayMenu -----------------------------------------------

class cSkinSoppalusikkaDisplayMenu : public cSkinDisplayMenu {
private:
  cOsd *osdM;
  int x0M, x1M, x2M, x3M, x4M, x5M;
  int y0M, y1M, y2M, y3M, y4M, y5M, y6M, y7M, y8M;
  int lineHeightM;
  int lastDiskUsageStateM;
  bool lastRecM;
  cString lastDateM;
  cString titleM;
  void DrawTitle();
  void DrawScrollbar(int totalP, int offsetP, int shownP, int topP, int heightP, bool canScrollUpP, bool canScrollDownP);
  void SetTextScrollbar(void);

public:
  cSkinSoppalusikkaDisplayMenu();
  virtual ~cSkinSoppalusikkaDisplayMenu();
  virtual void Scroll(bool upP, bool pageP);
  virtual int MaxItems(void);
  virtual void Clear(void);
  virtual void SetTitle(const char *titleP);
  virtual void SetButtons(const char *redP, const char *greenP = NULL, const char *yellowP = NULL, const char *blueP = NULL);
  virtual void SetMessage(eMessageType typeT, const char *textP);
  virtual void SetItem(const char *textP, int indexP, bool currentP, bool selectableP);
  virtual void SetScrollbar(int totalP, int offsetP);
  virtual void SetEvent(const cEvent *eventP);
  virtual void SetRecording(const cRecording *recordingP);
  virtual void SetText(const char *textP, bool fixedFontP);
  virtual int GetTextAreaWidth(void) const;
  virtual const cFont *GetTextAreaFont(bool fixedFontP) const;
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayMenu::cSkinSoppalusikkaDisplayMenu(void)
{
  const cFont *font = cFont::GetFont(fontOsd);
  lineHeightM = font->Height();
  lastDiskUsageStateM = -1;
  lastRecM = false;
  x0M = 0;
  x1M = x0M + Gap;
  x2M = x0M + Roundness;
  x5M = cOsd::OsdWidth();
  x4M = x5M - Gap;
  x3M = x5M - Roundness;
  y0M = 0;
  y1M = y0M + Roundness;
  y2M = y0M + lineHeightM;
  y3M = y2M + SmallGap;
  y4M = y3M + lineHeightM;
  y8M = cOsd::OsdHeight();
  y7M = y8M - Roundness;
  y6M = y8M - lineHeightM;
  y5M = y6M - lineHeightM;
  // create osd
  osdM = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop());
  tArea Areas[] = { { x0M, y0M, x5M - 1, y8M - 1, 8 } };
  if (Setup.AntiAlias && osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
     osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     // clear all
     osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
     }
  else {
     tArea Areas[] = { { x0M, y0M, x5M - 1, y8M - 1, 4 } };
     if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk) {
        osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        // clear all
        osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
        }
     else {
        tArea Areas[] = { { x0M, y0M, x5M - 1, y2M - 1, 2 },
                          { x0M, y2M, x5M - 1, y5M - 1, 2 },
                          { x0M, y5M, x5M - 1, y6M - 1, 2 },
                          { x0M, y6M, x5M - 1, y8M - 1, 4 }
                        };
        if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
           osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
        // clear all
        osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
        // fill up items area palette to prevent palette overflow
        osdM->GetBitmap(1)->Reset();
        osdM->GetBitmap(1)->SetColor(0, ThemeS.Color(clrBackground));
        osdM->GetBitmap(1)->SetColor(1, ThemeS.Color(clrMenuItemSelectable));
        osdM->GetBitmap(1)->SetColor(2, ThemeS.Color(clrMenuItemCurrentBg));
        osdM->GetBitmap(1)->SetColor(3, ThemeS.Color(clrMenuItemCurrentFg));
        }
     }
  // draw titlebar
  osdM->DrawRectangle(x0M, y0M, x5M - 1, y2M - 1, ThemeS.Color(clrMenuTitleBg));
  // draw rounded left corner of title bar
  osdM->DrawEllipse(x0M, y0M, x2M - 1, y1M - 1, clrTransparent, -2);
  // draw rounded right corner of title bar
  osdM->DrawEllipse(x3M, y0M, x5M - 1, y1M - 1, clrTransparent, -1);
  // draw gap + items + message area
  osdM->DrawRectangle(x0M, y2M, x5M - 1, y6M - 1, ThemeS.Color(clrBackground));
  // draw colorbar
  osdM->DrawRectangle(x0M, y6M, x5M - 1, y8M - 1, ThemeS.Color(clrBackground));
  osdM->DrawRectangle(x0M, y6M, x2M - 1, y8M - 1, ThemeS.Color(clrButtonRedBg));
  osdM->DrawRectangle(x3M, y6M, x5M - 1, y8M - 1, ThemeS.Color(clrButtonBlueBg));
  SetButtons(NULL);
  // draw rounded left corner of colorbar
  osdM->DrawEllipse(x0M, y7M, x2M - 1, y8M - 1, clrTransparent, -3);
  // draw rounded right corner of colorbar
  osdM->DrawEllipse(x3M, y7M, x5M - 1, y8M - 1, clrTransparent, -4);
}

cSkinSoppalusikkaDisplayMenu::~cSkinSoppalusikkaDisplayMenu()
{
  delete osdM;
}

void cSkinSoppalusikkaDisplayMenu::DrawScrollbar(int totalP, int offsetP, int shownP, int topP, int heightP, bool canScrollUpP, bool canScrollDownP)
{
  // check if scrollbar is needed
  if (totalP > 0 && totalP > shownP) {
     int yt = topP;
     int yb = yt + heightP;
     int st = yt;
     int sb = yb;
     int th = max(int((sb - st) * double(shownP) / totalP + 0.5), Gap);
     int tt = min(int(st + (sb - st) * double(offsetP) / totalP + 0.5), sb - th);
     int tb = min(tt + th, sb);
     int xl = x5M - Gap;
     // draw background of scrollbar
     osdM->DrawRectangle(xl, st, x5M - 1, sb - 1, ThemeS.Color(clrMenuScrollbarTotal));
     // draw visible area of scrollbar
     osdM->DrawRectangle(xl, tt, x5M - 1, tb - 1, ThemeS.Color(clrMenuScrollbarShown));
     }
}

void cSkinSoppalusikkaDisplayMenu::SetTextScrollbar(void)
{
  if (textScroller.CanScroll())
     DrawScrollbar(textScroller.Total(), textScroller.Offset(), textScroller.Shown(), textScroller.Top(), textScroller.Height(), textScroller.CanScrollUp(), textScroller.CanScrollDown());
}

void cSkinSoppalusikkaDisplayMenu::Scroll(bool upP, bool pageP)
{
  cSkinDisplayMenu::Scroll(upP, pageP);
  SetTextScrollbar();
}

int cSkinSoppalusikkaDisplayMenu::MaxItems(void)
{
  // max number of items
  return (y5M - y3M) / lineHeightM;
}

void cSkinSoppalusikkaDisplayMenu::Clear(void)
{
  textScroller.Reset();
  // clear items area
  osdM->DrawRectangle(x0M, y2M, x5M - 1, y6M - 1, ThemeS.Color(clrBackground));
}

void cSkinSoppalusikkaDisplayMenu::DrawTitle()
{
  const cFont *font = cFont::GetFont(fontSml);
  int w = Gap + font->Width("Wmm 07.07 07:07") + (cRecordControls::Active() ?  GetSymbol(SYMBOL_RECORDING).Width() : 0);
  // draw title
  if ((MenuCategory() == mcMain) || (MenuCategory() == mcRecording))
     osdM->DrawText(x2M, y0M, cString::sprintf("%s  -  %s", *titleM, *cVideoDiskUsage::String()), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), cFont::GetFont(fontSml), x3M - x2M - w, y2M - y0M);
  else
     osdM->DrawText(x2M, y0M, titleM, ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), cFont::GetFont(fontSml), x3M - x2M - w, y2M - y0M);
}

void cSkinSoppalusikkaDisplayMenu::SetTitle(const char *titleP)
{
  titleM = titleP;
  DrawTitle();
}

void cSkinSoppalusikkaDisplayMenu::SetButtons(const char *redP, const char *greenP, const char *yellowP, const char *blueP)
{
  const char *lutText[] = { redP, greenP, yellowP, blueP };
  tColor lutFg[] = { clrButtonRedFg, clrButtonGreenFg, clrButtonYellowFg, clrButtonBlueFg };
  tColor lutBg[] = { clrButtonRedBg, clrButtonGreenBg, clrButtonYellowBg, clrButtonBlueBg };
  const cFont *font = cFont::GetFont(fontSml);
  int t0 = x2M;
  int t4 = x3M;
  int t2 = t0 + (t4 - t0) / 2;
  int t1 = t0 + (t2 - t0) / 2;
  int t3 = t2 + (t4 - t2) / 2;
  // draw color buttons
  osdM->DrawText(t0, y6M, lutText[Setup.ColorKey0], ThemeS.Color(lutFg[Setup.ColorKey0]), ThemeS.Color(lutBg[Setup.ColorKey0]), font, t1 - t0, y8M - y6M, taCenter);
  osdM->DrawText(t1, y6M, lutText[Setup.ColorKey1], ThemeS.Color(lutFg[Setup.ColorKey1]), ThemeS.Color(lutBg[Setup.ColorKey1]), font, t2 - t1, y8M - y6M, taCenter);
  osdM->DrawText(t2, y6M, lutText[Setup.ColorKey2], ThemeS.Color(lutFg[Setup.ColorKey2]), ThemeS.Color(lutBg[Setup.ColorKey2]), font, t3 - t2, y8M - y6M, taCenter);
  osdM->DrawText(t3, y6M, lutText[Setup.ColorKey3], ThemeS.Color(lutFg[Setup.ColorKey3]), ThemeS.Color(lutBg[Setup.ColorKey3]), font, t4 - t3, y8M - y6M, taCenter);
}

void cSkinSoppalusikkaDisplayMenu::SetMessage(eMessageType typeP, const char *textP)
{
  // check if message
  if (textP) {
     // draw message
     osdM->DrawText(x2M, y5M, textP, ThemeS.Color(clrMessageStatusFg + 2 * typeP), ThemeS.Color(clrMessageStatusBg + 2 * typeP), cFont::GetFont(fontOsd), x4M - x2M, y6M - y5M, taCenter);
     }
  else {
     // clear message area
     osdM->DrawRectangle(x2M, y5M, x4M - 1, y6M - 1, ThemeS.Color(clrBackground));
     }
}

void cSkinSoppalusikkaDisplayMenu::SetItem(const char *textP, int indexP, bool currentP, bool selectableP)
{
  int y = y3M + indexP * lineHeightM;
  tColor ColorFg, ColorBg;
  // select colors
  if (currentP) {
     ColorFg = ThemeS.Color(clrMenuItemCurrentFg);
     ColorBg = ThemeS.Color(clrMenuItemCurrentBg);
     }
  else {
     ColorFg = ThemeS.Color(selectableP ? clrMenuItemSelectable : clrMenuItemCurrentBg);
     ColorBg = ThemeS.Color(clrBackground);
     }
  const cFont *font = cFont::GetFont(fontOsd);
  // this should prevent menu flickering
  osdM->DrawRectangle(x1M, y + lineHeightM / 2, x1M + 1, y + lineHeightM / 2 + 1, ThemeS.Color(clrMenuItemSelectable));
  // draw rounded left corner
  osdM->DrawEllipse(x1M, y, x2M - 1, y + lineHeightM - 1, ColorBg, 7);
  // draw item
  for (int i = 0; i < MaxTabs; i++) {
      const char *s = GetTabbedText(textP, i);
      if (s) {
         char buffer[9];
         int xt = x2M + Tab(i);
         bool iseventinfo = false;
         bool isnewrecording = false;
         bool isprogressbar = false;
         int now = 0, total = 0;
         // check if event info symbol: "tTV*" "R"
         if (SoppalusikkaConfig.GetShowSymbols() &&
             ((MenuCategory() == mcSchedule) || (MenuCategory() == mcScheduleNow) || (MenuCategory() == mcScheduleNext)) &&
             strlen(s) == 3 && ischaracter(s[0], " tTR") && ischaracter(s[1], " V") && ischaracter(s[2], " *")) {
            // update status
            iseventinfo = true;
            }
         // check if new recording: "0:45*", "10:10*", "01.01.06*"
         if (!iseventinfo && SoppalusikkaConfig.GetShowSymbols() &&
             (MenuCategory() == mcRecording) &&
             ((strlen(s) == 5 && s[4] == '*' && s[1] == ':' && isdigit(*s) && isdigit(*(s + 2)) && isdigit(*(s + 3))) ||
              (strlen(s) == 6 && s[5] == '*' && s[2] == ':' && isdigit(*s) && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4))) ||
              (strlen(s) == 9 && s[8] == '*' && s[5] == '.' && s[2] == '.' && isdigit(*s) && isdigit(*(s + 1)) && isdigit(*(s + 3)) && isdigit(*(s + 4)) && isdigit(*(s + 6)) && isdigit(*(s + 7))))) {
            // update status
            isnewrecording = true;
            // make a copy
            strncpy(buffer, s, strlen(s));
            // remove the '*' character
            buffer[strlen(s) - 1] = '\0';
            }
         // check if progress bar: "[|||||||   ]"
         if (!iseventinfo && !isnewrecording && SoppalusikkaConfig.GetShowProgressBar() &&
             ((MenuCategory() == mcSchedule) || (MenuCategory() == mcScheduleNow) || (MenuCategory() == mcScheduleNext)) &&
             (strlen(s) > 5 && s[0] == '[' && s[strlen(s) - 1] == ']')) {
            const char *p = s + 1;
            // update status
            isprogressbar = true;
            for (; *p != ']'; ++p) {
                // check if progressbar characters
                if (*p == ' ' || *p == '|') {
                   // update counters
                   ++total;
                   if (*p == '|')
                      ++now;
                   }
                else {
                   // wrong character detected; not a progressbar
                   isprogressbar = false;
                   break;
                   }
                }
            }
         if (iseventinfo) {
            int evx = xt + Gap;
            const char *p = s;
            // draw background
            osdM->DrawRectangle(xt, y, x3M, y + lineHeightM - 1, ColorBg);
            // draw symbols
            for (; *p; ++p) {
                switch (*p) {
                  case 't':
                       // partial timer event
                       osdM->DrawBitmap(evx, y + (lineHeightM - GetSymbol(SYMBOL_EVENT_PARTTIMER).Height()) / 2, GetSymbol(SYMBOL_EVENT_PARTTIMER), ColorFg, ColorBg);
                       evx += GetSymbol(SYMBOL_EVENT_PARTTIMER).Width();
                       break;
                  case 'T':
                       // timer event
                       osdM->DrawBitmap(evx, y + (lineHeightM - GetSymbol(SYMBOL_EVENT_TIMER).Height()) / 2, GetSymbol(SYMBOL_EVENT_TIMER), ColorFg, ColorBg);
                       evx += GetSymbol(SYMBOL_EVENT_TIMER).Width();
                       break;
                  case 'R':
                       // recording event (epgsearch)
                       osdM->DrawBitmap(evx, y + (lineHeightM - GetSymbol(SYMBOL_EVENT_RECORDING).Height()) / 2, GetSymbol(SYMBOL_EVENT_RECORDING), ColorFg, ColorBg);
                       evx += GetSymbol(SYMBOL_EVENT_RECORDING).Width();
                       break;
                  case 'V':
                       // vps event
                       osdM->DrawBitmap(evx, y + (lineHeightM - GetSymbol(SYMBOL_EVENT_VPS).Height()) / 2, GetSymbol(SYMBOL_EVENT_VPS), ColorFg, ColorBg);
                       evx += GetSymbol(SYMBOL_EVENT_VPS).Width();
                       break;
                  case '*':
                       // running event
                       osdM->DrawBitmap(evx, y + (lineHeightM - GetSymbol(SYMBOL_EVENT_RUNNING).Height()) / 2, GetSymbol(SYMBOL_EVENT_RUNNING), ColorFg, ColorBg);
                       evx += GetSymbol(SYMBOL_EVENT_RUNNING).Width();
                       break;
                  case ' ':
                  default:
                       // let's ignore space character
                       break;
                  }
                }
            }
         else if (isnewrecording) {
            // draw text
            osdM->DrawText(xt, y, buffer, ColorFg, ColorBg, font, x3M - xt);
            // draw symbol
            osdM->DrawBitmap(xt + font->Width(buffer), y + (lineHeightM - GetSymbol(SYMBOL_RECORDING_NEW).Height()) / 2, GetSymbol(SYMBOL_RECORDING_NEW), ColorFg, ColorBg);
            }
         else if (isprogressbar) {
            // define x coordinates of progressbar
            int px0M = xt;
            int px1M = px0M + SmallGap;
            int px2M = px1M + SmallGap;
            int px5M;
            if (Tab(i+1))
                px5M = px0M + Tab(i+1) - Tab(i);
            else
                px5M = px0M + x3M - x2M - Tab(i);
            int px4M = px5M - SmallGap;
            int px3M = px4M - SmallGap;
            int px = px2M + (int)((float)now * (float)(px3M - px2M) / (float)total);
            // define y coordinates of progressbar
            int py0M = y + Gap;
            int py1M = py0M + SmallGap;
            int py2M = py1M + Gap;
            int py5M = y + lineHeightM - Gap;
            int py4M = py5M - SmallGap;
            int py3M = py4M - Gap;
            // draw background
            osdM->DrawRectangle(xt, y, x3M, y + lineHeightM - 1, ColorBg);
            // draw progressbar
            osdM->DrawRectangle(px0M, py0M, px1M, py5M, ColorFg);
            osdM->DrawRectangle(px4M, py0M, px5M, py5M, ColorFg);
            osdM->DrawRectangle(px1M, py0M, px4M, py1M, ColorFg);
            osdM->DrawRectangle(px1M, py4M, px4M, py5M, ColorFg);
            osdM->DrawRectangle(px2M, py2M, px,  py3M, ColorFg);
            }
         else {
            // draw text
            osdM->DrawText(xt, y, s, ColorFg, ColorBg, font, x3M - xt);
            }
         }
      if (!Tab(i + 1))
         break;
      }
  // draw rounded right corner
  osdM->DrawEllipse(x3M, y, x4M - 1, y + lineHeightM - 1, ColorBg, 5);
  //set editable width
  SetEditableWidth(x3M - x2M - Tab(1));
}

void cSkinSoppalusikkaDisplayMenu::SetScrollbar(int totalP, int offsetP)
{
  DrawScrollbar(totalP, offsetP, MaxItems(), y2M, MaxItems() * lineHeightM, offsetP > 0, offsetP + MaxItems() < totalP);
}

void cSkinSoppalusikkaDisplayMenu::SetEvent(const cEvent *eventP)
{
  // check if event
  if (!eventP)
     return;
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smlfont = cFont::GetFont(fontSml);
  cTextScroller ts;
  cString info;
  int y = y3M;
  int xs = x2M;
  // check if event has timer
  if (eventP->HasTimer()) {
     // draw timer symbol
     osdM->DrawBitmap(xs, y + (y4M - y) / GetSymbol(SYMBOL_RECORDING).Height(), GetSymbol(SYMBOL_RECORDING), ThemeS.Color(clrMenuEventSymbolFg), ThemeS.Color(clrMenuEventSymbolBg));
     xs += (GetSymbol(SYMBOL_RECORDING).Width() + Roundness);
     }
  // check if event is running
  if (eventP->IsRunning()) {
     // draw running symbol
     osdM->DrawBitmap(xs, y + (y4M - y) / GetSymbol(SYMBOL_RUNNING).Height(), GetSymbol(SYMBOL_RUNNING), ThemeS.Color(clrMenuEventSymbolFg), ThemeS.Color(clrMenuEventSymbolBg));
     xs += (GetSymbol(SYMBOL_RUNNING).Width() + Roundness);
     }
  // check if event has VPS
  if (eventP->Vps()) {
     // draw VPS symbol
     osdM->DrawBitmap(xs, y + (y4M - y) / GetSymbol(SYMBOL_VPS).Height(), GetSymbol(SYMBOL_VPS), ThemeS.Color(clrMenuEventSymbolFg), ThemeS.Color(clrMenuEventSymbolBg));
     //xs += (GetSymbol(SYMBOL_VPS).Width() + Roundness);
     }
  y = y4M;
  // draw event date / duration string
  info = cString::sprintf("%s  %s - %s (%d %s)", *eventP->GetDateString(), *eventP->GetTimeString(), *eventP->GetEndTimeString(), eventP->Duration() / 60, tr("min"));
  ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *info, font, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
  y += ts.Height();
  // check if event has VPS
  if (eventP->Vps() && eventP->Vps() != eventP->StartTime()) {
     cString buffer = cString::sprintf("%s: %s", tr("VPS"), *eventP->GetVpsString());
     ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *buffer, smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
     y += ts.Height();
     }
  // draw event languages
  const cComponents *Components = eventP->Components();
  if (Components) {
     cString info, audio, subtitle;
     unsigned int numaudio = 0, numsubtitle = 0;
     for (int i = 0; i < Components->NumComponents(); i++) {
         const tComponent *p = Components->Component(i);
         if (p->language) {
            if (p->stream == 2) {
               audio = cString::sprintf("%s%s%s%s%s%s",
                                       (numaudio > 0) ? *audio : "",
                                       (numaudio > 0) ? ", " : "",
                                       p->description ? p->description : p->language,
                                       p->description ? " (" : "",
                                       p->description ? p->language : "",
                                       p->description ? ")" : "");
               numaudio++;
               }
            else if (p->stream == 3) {
               subtitle = cString::sprintf("%s%s%s (%s%s%s)",
                                        (numsubtitle > 0) ? *subtitle : "",
                                        (numsubtitle > 0) ? ", " : "",
                                        p->description ? p->description : p->language,
                                        p->description ? p->language : "",
                                        p->description ? "; " : "",
                                        (p->type < 0x10) ? tr("EBU") : trVDR("DVB"));
               numsubtitle++;
               }
            }
         }
     if (numaudio > 0)
        info = cString::sprintf("%s: %s", trVDR("Setup.DVB$Audio languages"), *audio);
     if (numsubtitle > 0)
        info = cString::sprintf("%s%s%s: %s", *info ? *info : "", *info ? "\n" : "", trVDR("Setup.DVB$Subtitle languages"), *subtitle);
     if (*info) {
        ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *info, smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
        y += ts.Height();
        }
     }
  y += smlfont->Height();
  // draw event title
  ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, eventP->Title(), font, ThemeS.Color(clrMenuEventTitle), ThemeS.Color(clrBackground));
  y += ts.Height();
  // draw content description
  info = "";
  for (int i = 0; eventP->Contents(i); i++) {
      const char *s = eventP->ContentToString(eventP->Contents(i));
      if (!isempty(s)) {
         info = cString::sprintf("%s%s%s", *info, isempty(*info) ? "" : ", ", s);
         }
      }
   if (!isempty(*info)) {
        ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *info, smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
        y += ts.Height();
      }
  // draw parental rating
  if (eventP->ParentalRating()) {
     ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *eventP->GetParentalRatingString(), smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
     y += ts.Height();
     }
  // draw recording short text and description
  if (isempty(eventP->Description())) {
     y += smlfont->Height();
     // check if short text
     if (!isempty(eventP->ShortText())) {
        // draw short text as description, if no description available
        textScroller.Set(osdM, x2M, y, x3M - x2M, y5M - y, eventP->ShortText(), font, ThemeS.Color(clrMenuEventDescription), ThemeS.Color(clrBackground));
        SetTextScrollbar();
        }
     }
  else {
     // check if short text
     if (!isempty(eventP->ShortText())) {
        // draw short text
        ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, eventP->ShortText(), smlfont, ThemeS.Color(clrMenuEventShortText), ThemeS.Color(clrBackground));
        y += ts.Height();
        }
     y += smlfont->Height();
     // draw description
     textScroller.Set(osdM, x2M, y, x3M - x2M, y5M - y, eventP->Description(), font, ThemeS.Color(clrMenuEventDescription), ThemeS.Color(clrBackground));
     SetTextScrollbar();
     }
}

void cSkinSoppalusikkaDisplayMenu::SetRecording(const cRecording *recordingP)
{
  // check if recording
  if (!recordingP)
     return;
  const cRecordingInfo *Info = recordingP->Info();
  const cFont *font = cFont::GetFont(fontOsd);
  const cFont *smlfont = cFont::GetFont(fontSml);
  int y = y3M;
  cTextScroller ts;
  char t[32];
  // draw recording date string
  snprintf(t, sizeof(t), "%s  %s", *DateString(recordingP->Start()), *TimeString(recordingP->Start()));
  ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, t, font, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
  y += ts.Height();
  // draw additional information
  cString info("");
  cChannel *channel = Channels.GetByChannelID(Info->ChannelID());
  if (channel)
     info = cString::sprintf("%s\n%s: %s", *info, trVDR("Channel"), *ChannelString(channel, 0));
  int length = recordingP->LengthInSeconds();
  int dirsize = DirSizeMB(recordingP->FileName());
  cString duration = (length >= 0) ? cString::sprintf("%s: %d:%02d:%02d  ", tr("Duration"), length / 3600, length / 60 % 60, length % 60) : cString("");
  cString size = (dirsize > 9999) ? cString::sprintf("%s: %.2f GB  ", tr("Size"), dirsize / 1024.0) : cString::sprintf("%s: %d MB ", tr("Size"), dirsize);
  cString bitrate = (length > 0) ? cString::sprintf("(%.2f MBit/s)", 8.0 * dirsize / length) : cString("");
  info = cString::sprintf("%s\n%s%s%s", *info, *duration, *size, *bitrate);
  info = cString::sprintf("%s\n%s: %d  %s: %d  %s: %s", *info, tr("Priority"), recordingP->Priority(), tr("Lifetime"), recordingP->Lifetime(), tr("Format"), recordingP->IsPesRecording() ? "PES" : "TS");
  if (SoppalusikkaConfig.GetShowAuxInfo() && Info->Aux()) {
     char *aux = strdup(Info->Aux());
     info = cString::sprintf("%s\n%s: %s", *info, tr("Auxiliary information"), parseaux(aux));
     free(aux);
     }
  const cComponents *Components = Info->Components();
  if (Components) {
     cString audio, subtitle;
     unsigned int numaudio = 0, numsubtitle = 0;
     for (int i = 0; i < Components->NumComponents(); i++) {
         const tComponent *p = Components->Component(i);
         if (p->language) {
            if (p->stream == 2) {
               audio = cString::sprintf("%s%s%s%s%s%s",
                                       (numaudio > 0) ? *audio : "",
                                       (numaudio > 0) ? ", " : "",
                                       p->description ? p->description : p->language,
                                       p->description ? " (" : "",
                                       p->description ? p->language : "",
                                       p->description ? ")" : "");
               numaudio++;
               }
            else if (p->stream == 3) {
               subtitle = cString::sprintf("%s%s%s (%s%s%s)",
                                        (numsubtitle > 0) ? *subtitle : "",
                                        (numsubtitle > 0) ? ", " : "",
                                        p->description ? p->description : p->language,
                                        p->description ? p->language : "",
                                        p->description ? "; " : "",
                                        (p->type < 0x10) ? tr("EBU") : trVDR("DVB"));
               numsubtitle++;
               }
            }
         }
     if (numaudio > 0)
        info = cString::sprintf("%s\n%s: %s", *info, trVDR("Setup.DVB$Audio languages"), *audio);
     if (numsubtitle > 0)
        info = cString::sprintf("%s\n%s: %s", *info, trVDR("Setup.DVB$Subtitle languages"), *subtitle);
     }
  if (!isempty(*info)) {
     ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *info, smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
     y += ts.Height();
     }
  y += smlfont->Height();
  // draw recording title
  const char *Title = Info->Title();
  if (isempty(Title))
     Title = recordingP->Name();
  ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, Title, font, ThemeS.Color(clrMenuEventTitle), ThemeS.Color(clrBackground));
  y += ts.Height();
  // draw parental rating
  if (Info->GetEvent()->ParentalRating()) {
     ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *Info->GetEvent()->GetParentalRatingString(), smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
     y += ts.Height();
     }
  // draw content description
  info = "";
  for (int i = 0; Info->GetEvent()->Contents(i); i++) {
      const char *s = Info->GetEvent()->ContentToString(Info->GetEvent()->Contents(i));
      if (!isempty(s)) {
         info = cString::sprintf("%s%s%s", *info, isempty(*info) ? "" : ", ", s);
         }
      }
  if (!isempty(*info)) {
     ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, *info, smlfont, ThemeS.Color(clrMenuEventTime), ThemeS.Color(clrBackground));
     y += ts.Height();
     }
  // draw recording short text and description
  if (isempty(Info->Description())) {
     y += smlfont->Height();
     // check if short text
     if (!isempty(Info->ShortText())) {
        // draw short text as description, if no description available
        textScroller.Set(osdM, x2M, y, x3M - x2M, y5M - y, Info->ShortText(), font, ThemeS.Color(clrMenuEventDescription), ThemeS.Color(clrBackground));
        SetTextScrollbar();
        }
     }
  else {
     // check if short text
     if (!isempty(Info->ShortText())) {
        // draw short text
        ts.Set(osdM, x2M, y, x3M - x2M, y5M - y, Info->ShortText(), smlfont, ThemeS.Color(clrMenuEventShortText), ThemeS.Color(clrBackground));
        y += ts.Height();
        }
     y += smlfont->Height();
     // draw description
     textScroller.Set(osdM, x2M, y, x3M - x2M, y5M - y, Info->Description(), font, ThemeS.Color(clrMenuEventDescription), ThemeS.Color(clrBackground));
     SetTextScrollbar();
     }
}

void cSkinSoppalusikkaDisplayMenu::SetText(const char *textP, bool fixedFontP)
{
  // draw text
  textScroller.Set(osdM, x2M, y3M, x3M - x2M, y5M - y3M, textP, GetTextAreaFont(fixedFontP), ThemeS.Color(clrMenuText), ThemeS.Color(clrBackground));
  SetTextScrollbar();
}

int cSkinSoppalusikkaDisplayMenu::GetTextAreaWidth(void) const
{
  // max text area width
  return x3M - x2M;
}

const cFont *cSkinSoppalusikkaDisplayMenu::GetTextAreaFont(bool fixedFontP) const
{
  // text area font
  return cFont::GetFont(fixedFontP ? fontFix : fontOsd);
}

void cSkinSoppalusikkaDisplayMenu::Flush(void)
{
  cString date = DayDateTime();
  const cFont *font = cFont::GetFont(fontSml);
  int w = font->Width("Wmm 07.07 07:07");
  bool rec = cRecordControls::Active();
  if (cVideoDiskUsage::HasChanged(lastDiskUsageStateM))
     DrawTitle();
  if (lastRecM != rec) {
     int xl = x3M - w - GetSymbol(SYMBOL_RECORDING).Width();
     // draw safety margin
     osdM->DrawRectangle(xl - Gap, y0M, xl - 1, y2M - 1, ThemeS.Color(clrMenuTitleBg));
     // draw recording symbol
     osdM->DrawBitmap(xl, y0M + (y2M - y0M - GetSymbol(SYMBOL_RECORDING).Height()) / 2, GetSymbol(SYMBOL_RECORDING), ThemeS.Color(clrMenuTitleBg), ThemeS.Color(rec ? clrMenuTitleFg : clrMenuTitleBg));
     lastRecM = rec;
     }
  if (!*lastDateM || strcmp(date, lastDateM)) {
     // update date string on titlebar
     osdM->DrawText(x3M - w, y0M, date, ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), font, w, y2M - y0M, taRight);
     lastDateM = date;
     }
  osdM->Flush();
}

// --- cSkinSoppalusikkaDisplayReplay ---------------------------------------------

class cSkinSoppalusikkaDisplayReplay : public cSkinDisplayReplay {
private:
  cOsd *osdM;
  int x0M, x1M, x2M, x3M, x4M, x5M, x6, x7, x8, x9, x10M;
  int y0M, y1M, y2M, y3M, y4M, y5M;
  int lineHeightM;
  bool drawDateM;
  bool modeOnlyM;
  cString lastDateM;

public:
  cSkinSoppalusikkaDisplayReplay(bool modeOnlyP);
  virtual ~cSkinSoppalusikkaDisplayReplay();
  virtual void SetRecording(const cRecording *recordingP);
  virtual void SetTitle(const char *titleP);
  virtual void SetMode(bool playP, bool forwardP, int speedP);
  virtual void SetProgress(int currentP, int totalP);
  virtual void SetCurrent(const char *currentP);
  virtual void SetTotal(const char *totalP);
  virtual void SetJump(const char *jumpP);
  virtual void SetMessage(eMessageType typeP, const char *textP);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayReplay::cSkinSoppalusikkaDisplayReplay(bool modeOnlyP)
{
  int symbolWidth = GetSymbol(SYMBOL_FREW).Width() + GetSymbol(SYMBOL_SREW).Width() + GetSymbol(SYMBOL_PLAY).Width() + GetSymbol(SYMBOL_PAUSE).Width() + GetSymbol(SYMBOL_SFWD).Width() + GetSymbol(SYMBOL_FFWD).Width() + 5 * Gap;
  int textWidth = cFont::GetFont(fontOsd)->Width("00:00:00/00:00:00");
  lineHeightM = cFont::GetFont(fontOsd)->Height();
  // clamp symbol/text width and line height
  if (textWidth > symbolWidth)
     symbolWidth = textWidth;
  if (lineHeightM < GetSymbol(SYMBOL_PLAY).Height())
     lineHeightM = GetSymbol(SYMBOL_PLAY).Height();
  drawDateM = true;
  modeOnlyM = modeOnlyP;
  x0M = 0;
  x1M = x0M + BigGap;
  x2M = x1M + Roundness;
  x3M = x2M + symbolWidth;
  x4M = x3M + 4 * BigGap;
  x10M = cOsd::OsdWidth();
  x9 = x10M - BigGap;
  x8 = x9 - Roundness;
  x6 = x8 - symbolWidth;
  x7 = x6 + (x8 - x6) / 2;
  x5M = x6 - 4 * BigGap;
  y0M = 0;
  y1M = y0M + Roundness;
  y2M = y0M + lineHeightM + Gap;
  y3M = y2M + lineHeightM;
  y5M = y3M + lineHeightM;
  y4M = y5M - Roundness;
  // create osd
  osdM = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop() + cOsd::OsdHeight() - y5M);
  tArea Areas[] = { { x0M, y0M, x10M - 1, y5M - 1, 8 } };
  if (Setup.AntiAlias && osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0M, y0M, x10M - 1, y5M - 1, 4 } };
     if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
  // select mode
  if (modeOnlyM) {
     // draw statusbar
     osdM->DrawRectangle(x6, y3M, x8 - 1, y5M - 1, ThemeS.Color(clrBackground));
     // draw rounded left corner
     osdM->DrawEllipse(x6 - Roundness - 1, y3M, x6 - 1, y5M - 1, ThemeS.Color(clrBackground), 7);
     // draw rounded right corner
     osdM->DrawEllipse(x8, y3M, x9 - 1, y5M - 1, ThemeS.Color(clrBackground), 5);
     }
  else {
     // draw titlebar
     osdM->DrawRectangle(x0M, y0M, x5M - 1, y2M - 1, ThemeS.Color(clrReplayTitleBorder));
     // draw rounded left corner of timebar
     osdM->DrawSlope(x5M, y0M, x6 - 1, y2M - 1, ThemeS.Color(clrBackground), 0);
     // draw timebar
     osdM->DrawRectangle(x6, y0M, x9 - 1, y2M - 1, ThemeS.Color(clrBackground));
     osdM->DrawText(x7 - Roundness / 2, y0M, "/", ThemeS.Color(clrReplayCurrent), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), Roundness, y2M - y0M, taCenter);
     // draw rounded right corner of timebar
     osdM->DrawEllipse(x8, y0M, x9 - 1, y1M - 1, clrTransparent, -1);
     // draw progressbar
     osdM->DrawRectangle(x1M, y2M, x9 - 1, y3M - 1, ThemeS.Color(clrBackground));
     // draw statusbar
     osdM->DrawRectangle(x1M, y3M, x3M - 1, y5M - 1, ThemeS.Color(clrBackground));
     osdM->DrawRectangle(x6, y3M, x9 - 1, y5M - 1, ThemeS.Color(clrBackground));
     // draw rounded center area of statusbar
     osdM->DrawSlope(x3M, y3M, x4M - 1, y5M - 1, ThemeS.Color(clrBackground), 1);
     osdM->DrawSlope(x5M, y3M, x6 - 1, y5M - 1, ThemeS.Color(clrBackground), 3);
     // draw rounded left corner of statusbar
     osdM->DrawEllipse(x1M, y4M, x2M - 1, y5M - 1, clrTransparent, -3);
     // draw rounded right corner of statusbar
     osdM->DrawEllipse(x8, y4M, x9 - 1, y5M - 1, clrTransparent, -4);
     }
}

cSkinSoppalusikkaDisplayReplay::~cSkinSoppalusikkaDisplayReplay()
{
  delete osdM;
}

void cSkinSoppalusikkaDisplayReplay::SetRecording(const cRecording *recordingP)
{
  SetTitle(recordingP->Title());
}

void cSkinSoppalusikkaDisplayReplay::SetTitle(const char *titleP)
{
  // draw titlebar
  osdM->DrawText(x0M + SmallGap, y0M + SmallGap, titleP, ThemeS.Color(clrReplayTitleFg), ThemeS.Color(clrReplayTitleBg), cFont::GetFont(fontSml), x5M - x0M - 2 * SmallGap, y2M - y0M - 2 * SmallGap);
}

void cSkinSoppalusikkaDisplayReplay::SetMode(bool playP, bool forwardP, int speedP)
{
  cBitmap *bm = NULL;
  // start drawing from right corner
  int xs = x8;
  // clamp parameters
  if (speedP < -1)
     speedP = -1;
  if (speedP > 3)
     speedP = 3;
  // check if current mode is fast forward
  if ((speedP > -1) && playP && forwardP) {
     // select symbol
     switch (speedP) {
       case 0:
            bm = &GetSymbol(SYMBOL_FFWD);
            break;
       case 1:
            bm = &GetSymbol(SYMBOL_FFWD1);
            break;
       case 2:
            bm = &GetSymbol(SYMBOL_FFWD2);
            break;
       default:
       case 3:
            bm = &GetSymbol(SYMBOL_FFWD3);
            break;
       }
     // draw active symbol
     xs -= bm->Width();
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - bm->Height()) / 2, *bm, ThemeS.Color(clrReplaySymbolActive), ThemeS.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= GetSymbol(SYMBOL_FFWD).Width();
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_FFWD).Height()) / 2, GetSymbol(SYMBOL_FFWD), ThemeS.Color(clrReplaySymbolInactive), ThemeS.Color(clrBackground));
     }
  // check if current mode is slow forward
  if ((speedP > -1) && !playP && forwardP) {
     // select symbol
     switch (speedP) {
       case 0:
            bm = &GetSymbol(SYMBOL_SFWD);
            break;
       case 1:
            bm = &GetSymbol(SYMBOL_SFWD1);
            break;
       case 2:
            bm = &GetSymbol(SYMBOL_SFWD2);
            break;
       default:
       case 3:
            bm = &GetSymbol(SYMBOL_SFWD3);
            break;
       }
     // draw active symbol
     xs -= bm->Width() + Gap;
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - bm->Height()) / 2, *bm, ThemeS.Color(clrReplaySymbolActive), ThemeS.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= GetSymbol(SYMBOL_SFWD).Width() + Gap;
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_SFWD).Height()) / 2, GetSymbol(SYMBOL_SFWD), ThemeS.Color(clrReplaySymbolInactive), ThemeS.Color(clrBackground));
     }
  // check if current mode is pause
  xs -= GetSymbol(SYMBOL_PAUSE).Width() + Gap;
  if ((speedP == -1) && !playP) {
     // draw active symbol
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_PAUSE).Height()) / 2, GetSymbol(SYMBOL_PAUSE), ThemeS.Color(clrReplaySymbolActive), ThemeS.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_PAUSE).Height()) / 2, GetSymbol(SYMBOL_PAUSE), ThemeS.Color(clrReplaySymbolInactive), ThemeS.Color(clrBackground));
     }
  // check if current mode is play
  xs -= GetSymbol(SYMBOL_PLAY).Width() + Gap;
  if ((speedP == -1) && playP) {
     // draw active symbol
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_PLAY).Height()) / 2, GetSymbol(SYMBOL_PLAY), ThemeS.Color(clrReplaySymbolActive), ThemeS.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_PLAY).Height()) / 2, GetSymbol(SYMBOL_PLAY), ThemeS.Color(clrReplaySymbolInactive), ThemeS.Color(clrBackground));
     }
  // check if current mode is slow reverse
  if ((speedP > -1) && !playP && !forwardP) {
     // select symbol
     switch (speedP) {
       case 0:
            bm = &GetSymbol(SYMBOL_SREW);
            break;
       case 1:
            bm = &GetSymbol(SYMBOL_SREW1);
            break;
       case 2:
            bm = &GetSymbol(SYMBOL_SREW2);
            break;
       default:
       case 3:
            bm = &GetSymbol(SYMBOL_SREW3);
            break;
       }
     // draw active symbol
     xs -= bm->Width() + Gap;
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - bm->Height()) / 2, *bm, ThemeS.Color(clrReplaySymbolActive), ThemeS.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= GetSymbol(SYMBOL_SREW).Width() + Gap;
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_SREW).Height()) / 2, GetSymbol(SYMBOL_SREW), ThemeS.Color(clrReplaySymbolInactive), ThemeS.Color(clrBackground));
     }
  // check if current mode is fast reverse
  if ((speedP > -1) && playP && !forwardP) {
     // select symbol
     switch (speedP) {
       case 0:
            bm = &GetSymbol(SYMBOL_FREW);
            break;
       case 1:
            bm = &GetSymbol(SYMBOL_FREW1);
            break;
       case 2:
            bm = &GetSymbol(SYMBOL_FREW2);
            break;
       default:
       case 3:
            bm = &GetSymbol(SYMBOL_FREW3);
            break;
       }
     // draw active symbol
     xs -= bm->Width() + Gap;
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - bm->Height()) / 2, *bm, ThemeS.Color(clrReplaySymbolActive), ThemeS.Color(clrBackground));
     }
  else {
     // draw inactive symbol
     xs -= GetSymbol(SYMBOL_FREW).Width() + Gap;
     osdM->DrawBitmap(xs, y3M + (y5M - y3M - GetSymbol(SYMBOL_FREW).Height()) / 2, GetSymbol(SYMBOL_FREW), ThemeS.Color(clrReplaySymbolInactive), ThemeS.Color(clrBackground));
     }
}

void cSkinSoppalusikkaDisplayReplay::SetProgress(int currentP, int totalP)
{
  // create progressbar
  cProgressBar pb(x9 - x1M - 2 * SmallGap, y3M - y2M - 2 * SmallGap, currentP, totalP, marks, ThemeS.Color(clrReplayProgressSeen), ThemeS.Color(clrReplayProgressRest), ThemeS.Color(clrReplayProgressSelected), ThemeS.Color(clrReplayProgressMark), ThemeS.Color(clrReplayProgressCurrent));
  // draw progressbar
  osdM->DrawBitmap(x1M + SmallGap, y2M + SmallGap, pb);
}

void cSkinSoppalusikkaDisplayReplay::SetCurrent(const char *currentP)
{
  // draw current time
  osdM->DrawText(x6, y0M, currentP, ThemeS.Color(clrReplayCurrent), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), x7 - x6 - BigGap, y2M - y0M, taRight);
}

void cSkinSoppalusikkaDisplayReplay::SetTotal(const char *totalP)
{
  // draw total time
  osdM->DrawText(x7 + BigGap, y0M, totalP, ThemeS.Color(clrReplayTotal), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), x8 - x7 - BigGap, y2M - y0M, taLeft);
}

void cSkinSoppalusikkaDisplayReplay::SetJump(const char *jumpP)
{
  // check if prompt
  if (jumpP) {
     // disallow date updating
     drawDateM = false;
     }
  else {
     // allow date updating
     drawDateM = true;
     lastDateM = NULL;
     }
  // draw jump prompt
  osdM->DrawText(x2M, y3M, jumpP, ThemeS.Color(clrReplayModeJump), ThemeS.Color(clrBackground), cFont::GetFont(fontOsd), x3M - x2M, y5M - y3M, taCenter);
}

void cSkinSoppalusikkaDisplayReplay::SetMessage(eMessageType typeP, const char *textP)
{
  // check if new message
  if (textP) {
     // disallow date updating
     drawDateM = false;
     // save current osd
     osdM->SaveRegion(x0M, y3M, x10M - 1, y5M - 1);
     // draw statusbar
     osdM->DrawRectangle(x0M, y3M, x1M - 1, y5M - 1, clrTransparent);
     osdM->DrawRectangle(x1M, y3M, x9 - 1, y5M - 1, ThemeS.Color(clrMessageStatusBg + 2 * typeP));
     osdM->DrawRectangle(x9, y3M, x10M - 1, y5M - 1, clrTransparent);
     // draw rounded left corner
     osdM->DrawEllipse(x1M, y3M, x2M - 1, y5M - 1, ThemeS.Color(clrMessageStatusBg + 2 * typeP), 7);
     // draw message
     osdM->DrawText(x2M, y3M, textP, ThemeS.Color(clrMessageStatusFg + 2 * typeP), ThemeS.Color(clrMessageStatusBg + 2 * typeP), cFont::GetFont(fontOsd), x8 - x2M, y5M - y3M, taCenter);
     // draw rounded right corner
     osdM->DrawEllipse(x8, y3M, x9 - 1, y5M - 1, ThemeS.Color(clrMessageStatusBg + 2 * typeP), 5);
     }
  else {
     // restore saved osd
     osdM->RestoreRegion();
     // allow date updating
     drawDateM = true;
     lastDateM = NULL;
     }
}

void cSkinSoppalusikkaDisplayReplay::Flush(void)
{
  if (drawDateM && !modeOnlyM) {
     cString date = DayDateTime();
     if (!*lastDateM || strcmp(date, lastDateM)) {
        // update date
        osdM->DrawText(x2M, y3M, date, ThemeS.Color(clrReplayModeJump), ThemeS.Color(clrBackground), cFont::GetFont(fontSml), x3M - x2M, y5M - y3M, taCenter);
        lastDateM = date;
        }
     }
  osdM->Flush();
}

// --- cSkinSoppalusikkaDisplayVolume ---------------------------------------------

class cSkinSoppalusikkaDisplayVolume : public cSkinDisplayVolume {
private:
  cOsd *osdM;
  int x0M, x1M, x2M, x3M, x4M, x5M;
  int y0M, y1M, y2M, y3M;
  int lineHeightM;

public:
  cSkinSoppalusikkaDisplayVolume();
  virtual ~cSkinSoppalusikkaDisplayVolume();
  virtual void SetVolume(int currentP, int totalP, bool muteP);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayVolume::cSkinSoppalusikkaDisplayVolume()
{
  lineHeightM = cFont::GetFont(fontSml)->Height();
  x0M = 0;
  x1M = x0M + BigGap;
  x2M = x1M + Roundness;
  x5M = cOsd::OsdWidth();
  x4M = x5M - BigGap;
  x3M = x4M - Roundness;
  y0M = 0;
  y1M = y0M + lineHeightM;
  y2M = y1M + Gap;
  y3M = y2M + lineHeightM;
  // create osd
  osdM = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop() + cOsd::OsdHeight() - y3M);
  tArea Areas[] = { { x0M, y0M, x5M - 1, y3M - 1, 8 } };
  if (Setup.AntiAlias && osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0M, x0M, x5M - 1, y3M - 1, 4 } };
     if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
}

cSkinSoppalusikkaDisplayVolume::~cSkinSoppalusikkaDisplayVolume()
{
  delete osdM;
}

void cSkinSoppalusikkaDisplayVolume::SetVolume(int currentP, int totalP, bool muteP)
{
  tColor ColorBar;
  const char *Prompt;
  const cFont *font = cFont::GetFont(fontSml);

  // select behaviour
  if (muteP) {
     ColorBar = ThemeS.Color(clrVolumeBarMute);
     Prompt = tr("Mute");
     }
  else {
     ColorBar = ThemeS.Color(clrVolumeBarLower);
     Prompt = tr("Volume");
     }
  // width of active titlebar
  int wfg = x0M + font->Width(Prompt) + 2 * Gap;
  int wbg = wfg + 2 * Gap;
  // current volume
  int vol = x2M + (x3M - x2M) * currentP / totalP;
  // draw titlebar
  osdM->DrawRectangle(x0M, y0M, wbg - 1, y2M - 1, ThemeS.Color(clrVolumePromptBorder));
  osdM->DrawRectangle(wbg, y0M, x4M - 1, y2M - 1, clrTransparent);
  osdM->DrawText(x0M + Gap, y0M + Gap, Prompt, ThemeS.Color(clrVolumePromptFg), ThemeS.Color(clrVolumePromptBg), font, wfg, lineHeightM, taCenter);
  // draw rounded left corner
  osdM->DrawEllipse(x1M, y2M, x2M - 1, y3M - 1, ThemeS.Color(clrBackground), 3);
  // draw volumebar
  osdM->DrawRectangle(x2M, y2M, x3M - 1,  y3M - 1, ThemeS.Color(clrBackground));
  osdM->DrawRectangle(x2M, y2M + Gap, vol, y3M - Gap - 1, ColorBar);
  osdM->DrawRectangle(vol + 1, y2M + Gap, x3M - 1, y3M - Gap - 1, ThemeS.Color(clrVolumeBarUpper));
  // draw rounded right corner
  osdM->DrawEllipse(x3M, y2M, x4M - 1, y3M - 1, ThemeS.Color(clrBackground), 5);
}

void cSkinSoppalusikkaDisplayVolume::Flush(void)
{
  osdM->Flush();
}

// --- cSkinSoppalusikkaDisplayTracks ---------------------------------------------

class cSkinSoppalusikkaDisplayTracks : public cSkinDisplayTracks {
private:
  cOsd *osdM;
  int x0M, x1M, x2M, x3M, x4M, x5M, x6;
  int y0M, y1M, y2M, y3M, y4M, y5M, y6M, y7M;
  int lineHeightM;
  int currentIndexM;
  void SetItem(const char *textP, int indexP, bool currentP);

public:
  cSkinSoppalusikkaDisplayTracks(const char *titleP, int numTracksP, const char * const *tracksP);
  virtual ~cSkinSoppalusikkaDisplayTracks();
  virtual void SetTrack(int indexP, const char * const *tracksP);
  virtual void SetAudioChannel(int audioChannelP);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayTracks::cSkinSoppalusikkaDisplayTracks(const char *titleP, int numTracksP, const char * const *tracksP)
{
  lineHeightM = cFont::GetFont(fontOsd)->Height();
  currentIndexM = -1;
  const cFont *font = cFont::GetFont(fontSml);
  int ItemsWidth = font->Width(titleP);
  for (int i = 0; i < numTracksP; i++)
      ItemsWidth = max(ItemsWidth, font->Width(tracksP[i]));
  ItemsWidth = max(ItemsWidth, font->Width(tr("Stereo")));
  ItemsWidth = max(ItemsWidth, font->Width(tr("Left")));
  ItemsWidth = max(ItemsWidth, font->Width(tr("Right")));
  ItemsWidth += 2 * BigGap + 2 * GetSymbol(SYMBOL_AUDIO).Width();
  x0M = 0;
  x1M = x0M + Roundness;
  x2M = x1M + ItemsWidth;
  x3M = x2M + BigGap;
  x4M = x3M + GetSymbol(SYMBOL_AUDIO).Width();
  x5M = x4M + Roundness;
  x6 = cOsd::OsdWidth();
  y0M = 0;
  y1M = y0M + Roundness;
  y3M = y0M + lineHeightM;
  y2M = y3M - Gap;
  y4M = y3M + numTracksP * lineHeightM;
  y5M = y4M + Gap;
  y7M = y5M + lineHeightM;
  y6M = y7M - Roundness;
  // create osd
  osdM = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop() + cOsd::OsdHeight() - y7M);
  tArea Areas[] = { { x0M, y0M, x6 - 1, y7M - 1, 8 } };
  if (Setup.AntiAlias && osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0M, y0M, x6 - 1, y7M - 1, 4 } };
     if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
  // draw titlebar
  osdM->DrawRectangle(x0M, y0M, x4M - 1, y2M - 1, ThemeS.Color(clrMenuTitleBg));
  osdM->DrawText(x1M, y0M, titleP, ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), font, x5M - x1M, y2M - y0M);
  // draw rounded left corner of titlebar
  osdM->DrawEllipse(x0M, y0M, x1M - 1, y1M - 1, clrTransparent, -2);
  // draw rounded right corner of titlebar
  osdM->DrawEllipse(x4M, y0M, x5M - 1, y1M - 1, clrTransparent, -1);
  // draw tracks area
  osdM->DrawRectangle(x0M, y2M, x5M - 1, y5M - 1, ThemeS.Color(clrBackground));
  // draw bottombar
  osdM->DrawRectangle(x0M, y5M, x5M - 1, y7M - 1, ThemeS.Color(clrMenuTitleBg));
  // draw rounded left corner of titlebar
  osdM->DrawEllipse(x0M, y6M, x1M - 1, y7M - 1, clrTransparent, -3);
  // draw rounded right corner of titlebar
  osdM->DrawEllipse(x4M, y6M, x5M - 1, y7M - 1, clrTransparent, -4);
  // fill up audio tracks
  SetAudioChannel(cDevice::PrimaryDevice()->GetAudioChannel());
  for (int i = 0; i < numTracksP; i++)
      SetItem(tracksP[i], i, false);
}

cSkinSoppalusikkaDisplayTracks::~cSkinSoppalusikkaDisplayTracks()
{
  delete osdM;
}

void cSkinSoppalusikkaDisplayTracks::SetItem(const char *textP, int indexP, bool currentP)
{
  const cFont *font = cFont::GetFont(fontOsd);
  int y = y3M + indexP * lineHeightM;
  tColor ColorFg, ColorBg;
  if (currentP) {
     ColorFg = ThemeS.Color(clrMenuItemCurrentFg);
     ColorBg = ThemeS.Color(clrMenuItemCurrentBg);
     currentIndexM = indexP;
     }
  else {
     ColorFg = ThemeS.Color(clrMenuItemSelectable);
     ColorBg = ThemeS.Color(clrBackground);
     }
  // draw track id
  osdM->DrawText(x1M, y, textP, ColorFg, ColorBg, font, x4M - x1M, lineHeightM);
  // draw rounded left corner
  osdM->DrawEllipse(x0M + SmallGap, y, x1M - 1, y + lineHeightM - 1, ColorBg, 7);
  // draw rounded right corner
  osdM->DrawEllipse(x4M, y, x5M - SmallGap - 1, y + lineHeightM - 1, ColorBg, 5);
}

void cSkinSoppalusikkaDisplayTracks::SetAudioChannel(int audioChannelP)
{
  const cFont *font = cFont::GetFont(fontSml);
  switch (audioChannelP) {
    case 0:
         /* stereo */
         osdM->DrawBitmap(x3M, y0M + (y2M - GetSymbol(SYMBOL_AUDIO).Height()) / 2, GetSymbol(SYMBOL_AUDIO), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg));
         osdM->DrawText(x1M, y5M, tr("Stereo"), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), font, x4M - x1M, y7M - y5M, taCenter);
         break;
    case 1:
         /* mono left */
         osdM->DrawBitmap(x3M, y0M + (y2M - GetSymbol(SYMBOL_AUDIO_LEFT).Height()) / 2, GetSymbol(SYMBOL_AUDIO_LEFT), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg));
         osdM->DrawText(x1M, y5M, tr("Left"), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), font, x4M - x1M, y7M - y5M, taCenter);
         break;
    case 2:
         /* mono right */
         osdM->DrawBitmap(x3M, y0M + (y2M - GetSymbol(SYMBOL_AUDIO_RIGHT).Height()) / 2, GetSymbol(SYMBOL_AUDIO_RIGHT), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg));
         osdM->DrawText(x1M, y5M, tr("Right"), ThemeS.Color(clrMenuTitleFg), ThemeS.Color(clrMenuTitleBg), font, x4M - x1M, y7M - y5M, taCenter);
         break;
    default:
         break;
    }
}

void cSkinSoppalusikkaDisplayTracks::SetTrack(int indexP, const char * const *tracksP)
{
  if (currentIndexM >= 0)
     SetItem(tracksP[currentIndexM], currentIndexM, false);
  SetItem(tracksP[indexP], indexP, true);
}

void cSkinSoppalusikkaDisplayTracks::Flush(void)
{
  osdM->Flush();
}

// --- cSkinSoppalusikkaDisplayMessage --------------------------------------------

class cSkinSoppalusikkaDisplayMessage : public cSkinDisplayMessage {
private:
  cOsd *osdM;
  int x0M, x1M, x2M, x3M;
  int y0M, y1M;
  int lineHeightM;

public:
  cSkinSoppalusikkaDisplayMessage();
  virtual ~cSkinSoppalusikkaDisplayMessage();
  virtual void SetMessage(eMessageType typeP, const char *textP);
  virtual void Flush(void);
  };

cSkinSoppalusikkaDisplayMessage::cSkinSoppalusikkaDisplayMessage()
{
  lineHeightM = cFont::GetFont(fontOsd)->Height();
  x0M = 0;
  x1M = x0M + Roundness;
  x3M = cOsd::OsdWidth();
  x2M = x3M - Roundness;
  y0M = 0;
  y1M = y0M + lineHeightM;
  // create osd
  osdM = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop() + cOsd::OsdHeight() - y1M);
  tArea Areas[] = { { x0M, y0M, x3M - 1, y1M - 1, 8 } };
  if (Setup.AntiAlias && osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
     osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
  else {
     tArea Areas[] = { { x0M, y0M, x3M - 1, y1M - 1, 2 } };
     if (osdM->CanHandleAreas(Areas, sizeof(Areas) / sizeof(tArea)) == oeOk)
        osdM->SetAreas(Areas, sizeof(Areas) / sizeof(tArea));
     }
  // clear all
  osdM->DrawRectangle(0, 0, osdM->Width(), osdM->Height(), clrTransparent);
}

cSkinSoppalusikkaDisplayMessage::~cSkinSoppalusikkaDisplayMessage()
{
  delete osdM;
}

void cSkinSoppalusikkaDisplayMessage::SetMessage(eMessageType typeP, const char *textP)
{
  // draw rounded left corner
  osdM->DrawEllipse(x0M, y0M, x1M - 1, y1M - 1, ThemeS.Color(clrMessageStatusBg + 2 * typeP), 7);
  // draw centered message text
  osdM->DrawText(x1M, y0M, textP, ThemeS.Color(clrMessageStatusFg + 2 * typeP), ThemeS.Color(clrMessageStatusBg + 2 * typeP), cFont::GetFont(fontOsd), x2M - x1M, y1M - y0M, taCenter);
  // draw rounded right corner
  osdM->DrawEllipse(x2M, y0M, x3M - 1, y1M - 1, ThemeS.Color(clrMessageStatusBg + 2 * typeP), 5);
}

void cSkinSoppalusikkaDisplayMessage::Flush(void)
{
  osdM->Flush();
}

// --- cSkinSoppalusikka ----------------------------------------------------------

cSkinSoppalusikka::cSkinSoppalusikka()
:cSkin("soppalusikka", &::ThemeS)
{
}

const char *cSkinSoppalusikka::Description(void)
{
  return tr("Soppalusikka");
}

cSkinDisplayChannel *cSkinSoppalusikka::DisplayChannel(bool withInfoP)
{
  SoppalusikkaLogoCache.Refresh();
  SoppalusikkaSymbolCache.Refresh();
  return new cSkinSoppalusikkaDisplayChannel(withInfoP);
}

cSkinDisplayMenu *cSkinSoppalusikka::DisplayMenu(void)
{
  SoppalusikkaSymbolCache.Refresh();
  return new cSkinSoppalusikkaDisplayMenu;
}

cSkinDisplayReplay *cSkinSoppalusikka::DisplayReplay(bool modeOnlyP)
{
  SoppalusikkaSymbolCache.Refresh();
  return new cSkinSoppalusikkaDisplayReplay(modeOnlyP);
}

cSkinDisplayVolume *cSkinSoppalusikka::DisplayVolume(void)
{
  return new cSkinSoppalusikkaDisplayVolume;
}

cSkinDisplayTracks *cSkinSoppalusikka::DisplayTracks(const char *titleP, int numTracksP, const char * const *tracksP)
{
  SoppalusikkaSymbolCache.Refresh();
  return new cSkinSoppalusikkaDisplayTracks(titleP, numTracksP, tracksP);
}

cSkinDisplayMessage *cSkinSoppalusikka::DisplayMessage(void)
{
  return new cSkinSoppalusikkaDisplayMessage;
}
