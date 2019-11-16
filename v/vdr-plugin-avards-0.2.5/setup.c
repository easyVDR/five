/*
 * setup.c: configuration file handling
 *
 * See the main source file 'avards.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "avards.h"
#include "setup.h"

#if APIVERSNUM < 10507
#include "i18n.h"
#define trNOOP(s) (s)
#endif

#if APIVERSNUM < 10509
#define trVDR(s) tr(s)
#endif


extern cParameter config;

cMenuSetupAvards::cMenuSetupAvards(void)
{
   modes[0] = tr("auto");
   modes[1] = tr("force 4:3");
   modes[2] = tr("force L14:9");
   modes[3] = tr("force L16:9");
   modes[4] = tr("force L>16:9");
   modes[5] = tr("force 16:9");

   tmpconfig = config;


   Add(new cMenuEditBoolItem(tr("start/stop entry in main menu"), &tmpconfig.ShowInMainMenu, trVDR("no"), trVDR("yes")));
   Add(new cMenuEditBoolItem(tr("Auto start plugin"),             &tmpconfig.AutoStart, trVDR("no"), trVDR("yes")));
   Add(new cMenuEditStraItem(tr("WSS mode"),                      &tmpconfig.Mode, MAX_MODES, modes));
   Add(new cMenuEditBoolItem(tr("Enable L>16:9 (2,4:1)"),         &tmpconfig.Overzoom, trVDR("no"), trVDR("yes")));
   Add(new cMenuEditIntItem(tr("Pan tone tolerance"),             &tmpconfig.PanToneTolerance, 0, 255));
   Add(new cMenuEditIntItem(tr("Pan tone max. black"),            &tmpconfig.PanToneBlackMax, 0, 255));
   Add(new cMenuEditIntItem(tr("overscan (%)"),                   &tmpconfig.Overscan_pct, 0, 100));
   Add(new cMenuEditIntItem(tr("logo width (%)"),                 &tmpconfig.MaxLogoWidth_pct, 0, 100));
   Add(new cMenuEditBoolItem(tr("detection"),                     &tmpconfig.ShowAllLines, tr("preserve aspect ratio"), tr("show all lines")));
   Add(new cMenuEditIntItem(tr("poll rate (milliseconds)"),       &tmpconfig.PollRate, 80, 5000));
   Add(new cMenuEditIntItem(tr("delay (in polls)"),               &tmpconfig.Delay, 1, MAXDELAY));
   Add(new cMenuEditBoolItem(tr("Show message on WSS switch"),    &tmpconfig.ShowMsg, trVDR("no"), trVDR("yes")));
   Add(new cMenuEditBoolItem(tr("VDR OSD is"),                    &tmpconfig.VDROSDisPAL, "NTSC", "PAL"));
}

void cMenuSetupAvards::Store(void)
{
   config = tmpconfig;
   SetupStore("ShowInMainMenu",     config.ShowInMainMenu);
   SetupStore("AutoStart",          config.AutoStart);
   SetupStore("Mode",               config.Mode);
   SetupStore("Overzoom",           config.Overzoom);
   SetupStore("PanToneTolerance",   config.PanToneTolerance);
   SetupStore("PanToneBlackMax",    config.PanToneBlackMax);
   SetupStore("Overscan",           config.Overscan_pct);
   SetupStore("ShowAllLines",       config.ShowAllLines);
   SetupStore("LogoWidth",          config.MaxLogoWidth_pct);
   SetupStore("PollRate",           config.PollRate);
   SetupStore("Delay",              config.Delay);
   SetupStore("FEHasLock");
   SetupStore("ShowMsg",            config.ShowMsg);
   SetupStore("VDROSDisPAL",        config.VDROSDisPAL);
}
