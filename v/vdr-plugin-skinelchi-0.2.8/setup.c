/*
 * setup.c: Setup and configuration file handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <vdr/themes.h>
#include <vdr/device.h>
#include <vdr/menuitems.h>
#include "common.h"
#include "config.h"
#include "setup.h"

extern const char *OSDSKIN;

extern cSkinElchiConfig ElchiConfig;

// --- cMenuEditColorItem ------------------------------------------------------
union clrArray {
   tColor color;
   unsigned char argb[4];
};

class cMenuEditColorItem : public cMenuEditItem {
private:
   tColor orgcolor;
   int pos;
   clrArray *clr;
   virtual void Set(void);
public:
   cMenuEditColorItem(const char *Name, tColor *Value);
   virtual eOSState ProcessKey(eKeys Key);
};


cMenuEditColorItem::cMenuEditColorItem(const char *Name, tColor *Value)
:cMenuEditItem(Name)
{
   orgcolor = *Value;
   clr = (clrArray *)Value;
   pos = 0;
   Set();
}

void cMenuEditColorItem::Set(void)
{
   char buf[16];
   int strpos = 0;

   for (int i=1; i<5; i++) {
      strpos += snprintf(&buf[strpos], sizeof(buf) - strpos, (pos==i)?"[%02X] ":"%02X ", clr->argb[4-i]);
   }
   SetValue(buf);
}

eOSState cMenuEditColorItem::ProcessKey(eKeys Key)
{
   eOSState state = cMenuEditItem::ProcessKey(Key);
   bool changed = false;

   if (state == osUnknown) {
      Key = NORMALKEY(Key);
      switch (Key) {
         case kNone: break;

         case kLeft|k_Repeat:
         case kLeft:
               if (pos > 0) {
                  pos--;
                  changed = true;
               }
               break;

         case kRight|k_Repeat:
         case kRight:
               if (pos < 4) {
                  pos++;
                  changed = true;
               }
               break;
         case kUp|k_Repeat:
         case kUp:
         case kDown|k_Repeat:
         case kDown:
               if (pos > 0) {
                  clr->argb[4-pos] += (NORMALKEY(Key) == kUp) ? 1 : -1;
                  changed = true;
               }
               else
                  return cMenuEditItem::ProcessKey(Key);
               break;

         case kBack:
         case kOk:
            if (pos > 0) {
               if (Key == kBack) {
                  clr->color = orgcolor;
               }
               pos = 0;
               changed = true;
               break;
            }
            return cMenuEditItem::ProcessKey(Key);

         default:
            return state;
      }

      state = osContinue;
      if (changed) {
         Set();
      }
   }
   return state;
}


// --- cSkinElchiSetupColor ------------------------------------------------------
static const char * ColorNames [] = {
   "clrBackground",
   "clrButtonRedFg","clrButtonRedBg","clrButtonGreenFg","clrButtonGreenBg","clrButtonYellowFg","clrButtonYellowBg","clrButtonBlueFg","clrButtonBlueBg",
   "clrMessageStatusFg","clrMessageStatusBg","clrMessageInfoFg","clrMessageInfoBg","clrMessageWarningFg","clrMessageWarningBg","clrMessageErrorFg","clrMessageErrorBg",
   "clrVolumePrompt","clrVolumeBarUpper","clrVolumeBarLower","clrVolumeSymbolMuteFg","clrVolumeSymbolMuteBg","clrVolumeSymbolVolumeFg","clrVolumeSymbolVolumeBg",
   "clrChannelNameFg","clrChannelNameBg","clrChannelDateFg","clrChannelDateBg","clrChannelEpgTimeFg","clrChannelEpgTimeBg","clrChannelEpgTitleFg","clrChannelEpgTitleBg","clrChannelEpgShortText",
   "clrChannelSymbolOn","clrChannelSymbolOff","clrSymbolRecFg","clrSymbolRecBg",
   "clrChannelTimebarSeen","clrChannelTimebarRest",
   "clrMenuTitleFg","clrMenuTitleBg","clrMenuDate","clrMenuItemCurrentFg","clrMenuItemCurrentBg","clrMenuItemSelectable","clrMenuItemNonSelectable","clrMenuEventTime","clrMenuEventVpsFg","clrMenuEventVpsBg","clrMenuEventTitle","clrMenuEventShortText","clrMenuEventDescription","clrMenuScrollbarTotal","clrMenuScrollbarShown","clrMenuText",
   "clrReplayTitleFg","clrReplayTitleBg","clrReplayCurrent","clrReplayTotal","clrReplayModeJump","clrReplayProgressSeen","clrReplayProgressRest","clrReplayProgressSelected","clrReplayProgressMark","clrReplayProgressCurrent","clrReplaySymbolOn","clrReplaySymbolOff",
   "clrChanging",
};

class cSkinElchiSetupColor : public cOsdMenu
{
private:
   int numColors;
   tColor lastcolor;
   tColor ThemeColors[MAX_ELCHI_THEME_COLORS - 1];
   void Setup(void);

protected:
   virtual eOSState ProcessKey(eKeys Key);

public:
   cSkinElchiSetupColor(void);
   virtual ~cSkinElchiSetupColor(void);
};

cSkinElchiSetupColor::cSkinElchiSetupColor(void)
:cOsdMenu("", 33)
{
   SetTitle(cString::sprintf("%s - '%s' %s", trVDR("Setup"), tr("skinelchi"), tr("Colors")));

   numColors = MAX_ELCHI_THEME_COLORS - 1;

   for (int i = 0; i < numColors; i++) {
      ThemeColors[i] = Theme.Color(i);
   }

   Setup();
   ElchiConfig.clrdlgActive = true;
}

cSkinElchiSetupColor::~cSkinElchiSetupColor()
{
   ElchiConfig.clrdlgActive = false;
}

void cSkinElchiSetupColor::Setup(void)
{
   Add(new cOsdItem(cString::sprintf("%s\t%s", tr("theme name"), Skins.Current()->Theme()->Name()), osUnknown, false));

   for (int i = 0; i < numColors; i++)
      Add(new cMenuEditColorItem(ColorNames[i], &ThemeColors[i]));
}


eOSState cSkinElchiSetupColor::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);

   int current = Current()-1;
   if (lastcolor != ThemeColors[current]) {
      //isyslog("skinelchi: clr: %08X-%08X", lastcolor, ThemeColors[current]);
      lastcolor = ThemeColors[current];
      ElchiConfig.clrDlgFg = lastcolor;
      ElchiConfig.clrDlgBg = lastcolor;
      if (strcasestr(ColorNames[current], "Fg"))
         ElchiConfig.clrDlgBg = ThemeColors[current + 1];
      else if (strcasestr(ColorNames[current], "Bg")) {
            ElchiConfig.clrDlgFg = ThemeColors[current - 1];
         }

      Display(); // kompletter Refresh: Clear, Title, Buttons, items, Flush
   }

   if (state == osUnknown) {
      switch (Key) {
         case kOk:
               for (int i = 0; i < numColors; i++)
                  Theme.AddColor(ColorNames[i], ThemeColors[i]);

               Theme.Save(cString::sprintf("%s/../../themes/%s-%s.theme",
                     cPlugin::ConfigDirectory(PLUGIN_NAME_I18N),
                     Skins.Current()->Name(),
                     Theme.Name()));
               Display();
               // fall through
         case kBack:
               state = osBack;
               break;
         default: break;
      }
   }
   return state;
}


// --- cSkinElchiSetupGeneral ------------------------------------------------------
class cSkinElchiSetupGeneral : public cOsdMenu
{
private:
   cSkinElchiConfig *tmpconfig;
   int oldepgimagesize;
   void Setup(void);
   const char * EpgImageSizeItems[4];
   const char * ResizeItems[3];

   const char * RecInfoItems[3];
   const char * VideoInfoItems[3];
   const char * TimerCheckItems[4];
   const char * MailIconItems[3];
   const char * EpgDetails[3];

protected:
   virtual eOSState ProcessKey(eKeys Key);

public:
   cSkinElchiSetupGeneral(cSkinElchiConfig *tmpElchiConfig);
   virtual ~cSkinElchiSetupGeneral();
};

cSkinElchiSetupGeneral::cSkinElchiSetupGeneral(cSkinElchiConfig *tmpConfig)
:cOsdMenu("", 33)
{
   SetTitle(cString::sprintf("%s - '%s' %s", trVDR("Setup"), tr("skinelchi"), tr("General")));

   tmpconfig = tmpConfig;

   EpgImageSizeItems[0] = tr("don't show");
   EpgImageSizeItems[1] = "120x90 (epg4vdr)";
   EpgImageSizeItems[2] = "max. 180x144";
   EpgImageSizeItems[3] = "120x120 (tvmovie2vdr)";

   ResizeItems[0] = tr("pixel algo");
   ResizeItems[1] = tr("ratio algo");
   ResizeItems[2] = tr("zoom image");

   VideoInfoItems[0]  = trVDR("no");
   VideoInfoItems[1]  = tr("aspect ratio");
   VideoInfoItems[2]  = tr("size and aspect ratio");

   TimerCheckItems[0] = trVDR("no");
   TimerCheckItems[1] = tr("Next recording");
   TimerCheckItems[2] = tr("Timer-Conflict");
   TimerCheckItems[3] = tr("all");

   MailIconItems[0]   = tr("never");
   MailIconItems[1]   = tr("only if new mail present");
   MailIconItems[2]   = tr("always");

   EpgDetails[0]   = trVDR("EPG");
   EpgDetails[1]   = tr("EPG+Details");
   EpgDetails[2]   = tr("EPG+Details+Genre");

   Setup();
}

cSkinElchiSetupGeneral::~cSkinElchiSetupGeneral()
{
}


void cSkinElchiSetupGeneral::Setup(void)
{
   Add(new cMenuEditBoolItem(tr("use one 8bpp area (if possible)"), &tmpconfig->trySingle8BppArea));
   Add(new cMenuEditBoolItem(tr("scroll text"), &tmpconfig->useScrolling));
   Add(new cMenuEditStraItem(tr("show timer and conflict in menu"), &tmpconfig->showTimer, 4, TimerCheckItems));
   Add(new cMenuEditStraItem(tr("show recording details"), &tmpconfig->showRecDetails, 3, EpgDetails));
   Add(new cMenuEditStraItem(tr("show EPG details"), &tmpconfig->showEPGDetails, 3, EpgDetails));
   Add(new cMenuEditStraItem(tr("show video format info (if available)"), &tmpconfig->showVideoInfo, 3, VideoInfoItems));
   Add(new cMenuEditStraItem(tr("show mail icon"), &tmpconfig->showMailIcon, 3, MailIconItems));
   Add(new cMenuEditBoolItem(tr("use graphic progressbar"), &tmpconfig->GraphicalProgressbar));
   Add(new cMenuEditBoolItem(tr("show symbols"), &tmpconfig->showIcons));
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   oldepgimagesize = tmpconfig->EpgImageSize;
   Add(new cMenuEditStraItem(tr("EPG picture size"), &tmpconfig->EpgImageSize, 4, EpgImageSizeItems));
   if (tmpconfig->EpgImageSize) {
      // TRANSLATORS: note the two leading spaces
      Add(new cMenuEditIntItem(tr("  duration of each EPG image [s]"), &tmpconfig->EpgImageDisplayTime, 2, 15));
      if (tmpconfig->EpgImageSize == 2) {
         // TRANSLATORS: note the two leading spaces
         Add(new cMenuEditStraItem(tr("  resize algorithm"), &tmpconfig->ImgResizeAlgo, 3, ResizeItems));
      }
   }
#endif
}

eOSState cSkinElchiSetupGeneral::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);

   if (state == osUnknown) {
      switch (Key) {
         case kOk:
         case kBack:
               state = osBack;
               break;
         default: break;
      }
   }

   if (oldepgimagesize != tmpconfig->EpgImageSize) {
      oldepgimagesize = tmpconfig->EpgImageSize;
      int oldcurrent = Current();
      Clear();
      Setup();
      SetCurrent(Get(oldcurrent));
      Display();
   }

   return state;
}

// --- cSkinElchiSetupChannelDisplay ------------------------------------------------------
class cSkinElchiSetupChannelDisplay : public cOsdMenu
{
private:
   cSkinElchiConfig *tmpconfig;
   void Setup(void);
   const char * RecInfoItems[3];

protected:
   virtual eOSState ProcessKey(eKeys Key);

public:
   cSkinElchiSetupChannelDisplay(cSkinElchiConfig *tmpConfig);
   virtual ~cSkinElchiSetupChannelDisplay();
};

cSkinElchiSetupChannelDisplay::cSkinElchiSetupChannelDisplay(cSkinElchiConfig *tmpConfig)
:cOsdMenu("", 33)
{
   SetTitle(cString::sprintf("%s - '%s' %s", trVDR("Setup"), tr("skinelchi"), tr("Channel Display")));

   tmpconfig = tmpConfig;

   RecInfoItems[0] = tr("never");
   RecInfoItems[1] = tr("only if recording");
   RecInfoItems[2] = tr("always");

   Setup();
}

cSkinElchiSetupChannelDisplay::~cSkinElchiSetupChannelDisplay()
{
}

void cSkinElchiSetupChannelDisplay::Setup(void)
{
   Add(new cMenuEditStraItem(tr("show recording Info"), &tmpconfig->showRecInfo, 3, RecInfoItems));
   Add(new cMenuEditBoolItem(tr("show channel logos"), &tmpconfig->showLogo));
   Add(new cMenuEditBoolItem(tr("use one 8bpp area (if possible)"), &tmpconfig->tryChannelDisplay8Bpp));
   Add(new cMenuEditBoolItem(tr("use HQ-logos"), &tmpconfig->useHQlogo));
   Add(new cMenuEditBoolItem(tr("try to load logo first by"), &tmpconfig->LogoSearchNameFirst, tr("Channel ID"), tr("Channel Name")));
   Add(new cMenuEditBoolItem(tr("display logo messages in syslog"), &tmpconfig->LogoMessages));
}

eOSState cSkinElchiSetupChannelDisplay::ProcessKey(eKeys Key)
{
   eOSState state = cOsdMenu::ProcessKey(Key);

   if (state == osUnknown) {
      switch (Key) {
         case kOk:
         case kBack:
               state = osBack;
               break;
         default: break;
      }
   }
   return state;
}


// --- cSkinElchiSetup ------------------------------------------------------
cSkinElchiSetup::cSkinElchiSetup(void)
{
   tmpElchiConfig = ElchiConfig;

   Setup();
}

cSkinElchiSetup::~cSkinElchiSetup()
{
}

void cSkinElchiSetup::Setup(void)
{
   Add(new cOsdItem(tr("General"), osUser1));
   Add(new cOsdItem(tr("Channel Display"), osUser2));

   if (Skins.Current()->Name() && !strcmp(OSDSKIN, Skins.Current()->Name()))
      Add(new cOsdItem(tr("Colors"), osUser3));
   else  // disable color dialog if current skin is not skinelchi
      Add(new cOsdItem(tr("Colors"), osUnknown, false));
}

eOSState cSkinElchiSetup::ProcessKey(eKeys Key)
{
   bool hadSubMenu = HasSubMenu();
   eOSState state = cOsdMenu::ProcessKey(Key);

   if (hadSubMenu && !HasSubMenu() && Key == kOk)
      Store();

   switch (state) {
      case osUser1:
         AddSubMenu(new cSkinElchiSetupGeneral(&tmpElchiConfig));
         state=osContinue;
         break;
      case osUser2:
         AddSubMenu(new cSkinElchiSetupChannelDisplay(&tmpElchiConfig));
         state=osContinue;
         break;
      case osUser3:
         AddSubMenu(new cSkinElchiSetupColor());
         state=osContinue;
         break;
      default:
         break;
   }
   return state;
}

void cSkinElchiSetup::Store(void)
{
   ElchiConfig = tmpElchiConfig;

   // general values
   SetupStore("trySingle8BppArea", ElchiConfig.trySingle8BppArea);
   SetupStore("useScrolling", ElchiConfig.useScrolling);
   SetupStore("showTimer", ElchiConfig.showTimer);
   SetupStore("showRecDetails", ElchiConfig.showRecDetails);
   SetupStore("showEPGDetails", ElchiConfig.showEPGDetails);
   SetupStore("showVideoInfo", ElchiConfig.showVideoInfo);
   SetupStore("showMailIcon", ElchiConfig.showMailIcon);
   SetupStore("GraphicalProgressbar", ElchiConfig.GraphicalProgressbar);
   SetupStore("showIcons", ElchiConfig.showIcons);
   SetupStore("EpgImageSize", ElchiConfig.EpgImageSize);
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   SetupStore("EpgImageDisplayTime", ElchiConfig.EpgImageDisplayTime);
   SetupStore("ImgResizeAlgo", ElchiConfig.ImgResizeAlgo);
#endif

   // channel Display values
   SetupStore("showRecInfo", ElchiConfig.showRecInfo);
   SetupStore("showLogo", ElchiConfig.showLogo);
   SetupStore("tryChannelDisplay8Bpp", ElchiConfig.tryChannelDisplay8Bpp);
   SetupStore("useHQlogo", ElchiConfig.useHQlogo);
   SetupStore("LogoSearchNameFirst", ElchiConfig.LogoSearchNameFirst);
   SetupStore("LogoMessages", ElchiConfig.LogoMessages);

   // delete old unused entries
   SetupStore("singlearea");
   SetupStore("scrolling");
   SetupStore("timer");
   //SetupStore("epgimagesize"); setup names are not case sensitive
   SetupStore("resize");
   SetupStore("logo");
   SetupStore("logousemagick");
   SetupStore("logox");
   SetupStore("logoy");
   SetupStore("usenofixedfont");
   SetupStore("hidemenu");
   SetupStore("logodir");
   SetupStore("epgimagedir");
   SetupStore("epgshowtime");
   SetupStore("epgimages");
   SetupStore("menuentry");
   SetupStore("osdheight");
   SetupStore("osdleft");
   SetupStore("osdtop");
   SetupStore("osdwidth");
   SetupStore("videostatus");
   SetupStore("videosystem");
   SetupStore("switchvideodisplayformat");
   SetupStore("mailicon");
   SetupStore("showepgimages");
}
