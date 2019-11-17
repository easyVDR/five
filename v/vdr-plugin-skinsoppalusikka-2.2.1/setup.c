/*
 * setup.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/menu.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "logo.h"
#include "setup.h"

cSkinSoppalusikkaSetup::cSkinSoppalusikkaSetup()
: showAuxInfoM(SoppalusikkaConfig.GetShowAuxInfo()),
  showLogoM(SoppalusikkaConfig.GetShowLogo()),
  showVpsM(SoppalusikkaConfig.GetShowVps()),
  showDurationM(SoppalusikkaConfig.GetShowDuration()),
  showSymbolsM(SoppalusikkaConfig.GetShowSymbols()),
  showProgressBarM(SoppalusikkaConfig.GetShowProgressBar()),
  cacheSizeM(100)
{
  debug1("%s", __PRETTY_FUNCTION__);
  SetMenuCategory(mcSetupPlugins);
  Setup();
  SetHelp(tr("Button$Flush cache"), NULL, NULL, NULL);
}

void cSkinSoppalusikkaSetup::Setup(void)
{
  // update setup display
  int current = Current();

  Clear();
  helpM.Clear();

  Add(new cMenuEditBoolItem(tr("Show auxiliary information"), &showAuxInfoM));
  helpM.Append(tr("Define whether any auxiliary information is shown in info menus."));

  Add(new cMenuEditBoolItem(tr("Show progressbar"), &showProgressBarM));
  helpM.Append(tr("Define whether a progressbar is shown in schedule menu."));

  Add(new cMenuEditBoolItem(tr("Show symbols"), &showSymbolsM));
  helpM.Append(tr("Define whether symbols are shown in recordings and schedule menus."));

  Add(new cMenuEditBoolItem(tr("Show VPS in channel info"), &showVpsM));
  helpM.Append(tr("Define whether VPS information is shown in channel info menu."));

  Add(new cMenuEditBoolItem(tr("Show event duration in channel info"), &showDurationM, tr("remaining"), tr("total")));
  helpM.Append(tr("Define whether remaining or total event duration is shown in channel info menu."));

  Add(new cMenuEditBoolItem(tr("Show channel logos"), &showLogoM));
  helpM.Append(tr("Define whether channels logos are shown in channel info menu.\n\nOnly XPM format is accepted: 64x48 pixel and max. 13 colors."));

  if (showLogoM) {
     Add(new cMenuEditIntItem( tr("Channel logo cache size"), &cacheSizeM, 0, 1000));
     helpM.Append(tr("Define the cache size for channel logos.\n\nThe bigger cache results faster zapping."));
    }

  SetCurrent(Get(current));
  Display();
}

void cSkinSoppalusikkaSetup::Store(void)
{
  debug1("%s", __PRETTY_FUNCTION__);
  // Store values into setup.conf
  SetupStore("ShowAuxInfo", showAuxInfoM);
  SetupStore("ShowProgressBar", showProgressBarM);
  SetupStore("ShowSymbols", showSymbolsM);
  SetupStore("ShowLogo", showLogoM);
  SetupStore("ShowVPS", showVpsM);
  SetupStore("ShowDuration", showDurationM);
  SetupStore("CacheSize", cacheSizeM);
  // Update global config
  SoppalusikkaConfig.SetShowAuxInfo(showAuxInfoM);
  SoppalusikkaConfig.SetShowProgressBar(showProgressBarM);
  SoppalusikkaConfig.SetShowSymbols(showSymbolsM);
  SoppalusikkaConfig.SetShowLogo(showLogoM);
  SoppalusikkaConfig.SetShowVps(showVpsM);
  SoppalusikkaConfig.SetShowDuration(showDurationM);
  SoppalusikkaConfig.SetCacheSize(cacheSizeM);
  // resize logo cache
  SoppalusikkaLogoCache.Resize(SoppalusikkaConfig.GetCacheSize());
}

eOSState cSkinSoppalusikkaSetup::ProcessKey(eKeys keyP)
{
  // process key presses
  int oldShowLogo = showLogoM;

  eOSState state = cMenuSetupPage::ProcessKey(keyP);

  if (keyP != kNone && (showLogoM != oldShowLogo))
     Setup();

  if (state == osUnknown) {
     switch (keyP) {
       case kRed:
            Skins.Message(mtInfo, tr("Flushing channel logo cache..."));
            SoppalusikkaLogoCache.Flush();
            Skins.Message(mtInfo, NULL);
            state = osContinue;
            break;
       case kInfo:
            if (Current() < helpM.Size())
               return AddSubMenu(new cMenuText(cString::sprintf("%s - %s '%s'", tr("Help"), trVDR("Plugin"), PLUGIN_NAME_I18N), helpM[Current()]));
            break;
       default:
            break;
       }
     }

  return state;
}
