/*
 * config.c: Setup and configuration file handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include "common.h"
#include "config.h"

cSkinElchiConfig ElchiConfig;

cTheme Theme;

// --- cSkinElchiConfig ----------------------------------------------------------
cSkinElchiConfig::cSkinElchiConfig(void)
{
   // general
   trySingle8BppArea = 0;
   useScrolling = 1;
   showTimer = 0;
   showRecDetails = 1;
   showEPGDetails = 0;
   showVideoInfo = 0;
   showMailIcon = 0;
   GraphicalProgressbar = 1;
   showIcons = 1;
   EpgImageSize = 0;
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   EpgImageDisplayTime = 5;
   ImgResizeAlgo = 0;
   SetEpgImageDir("/video/epgimages");
   logox = 64; //TODO
   logoy = 48;
#endif

   // channel display
   strcpy(logoBaseDir, "");
   showRecInfo = 0;
   tryChannelDisplay8Bpp = 0;
   useHQlogo = 1;
   LogoSearchNameFirst = 1;
   LogoMessages = 0;

   // clr dialog
   clrdlgActive = false;
}

cSkinElchiConfig::~cSkinElchiConfig()
{
}

void cSkinElchiConfig::SetLogoBaseDir(const char *dir)
{
   if (dir) {
      strncpy(logoBaseDir, dir, sizeof(logoBaseDir));
      DSYSLOG("skinelchi: setting logoBaseDir to '%s'", logoBaseDir)
   }
}


#ifdef SKINELCHI_HAVE_IMAGEMAGICK
void cSkinElchiConfig::SetEpgImageDir(const char *dir)
{
   if (dir) {
      strncpy(epgimageDir, dir, sizeof(epgimageDir));
      DSYSLOG("skinelchi: setting epgimageDir to '%s'", epgimageDir)
   }
}
#endif

bool cSkinElchiConfig::SetupParse(const char *Name, const char *Value)
{
   if      (strcmp(Name, "singlearea") == 0)                   trySingle8BppArea = atoi(Value); // old value
   else if (strcmp(Name, "trySingle8BppArea") == 0)            trySingle8BppArea = atoi(Value);
   else if (strcmp(Name, "scrolling") == 0)                    useScrolling = atoi(Value); //old value
   else if (strcmp(Name, "useScrolling") == 0)                 useScrolling = atoi(Value);
   else if (strcmp(Name, "timer") == 0)                        showTimer = atoi(Value); // old value
   else if (strcmp(Name, "showTimer") == 0)                    showTimer = atoi(Value);
   else if (strcmp(Name, "showRecDetails") == 0)               showRecDetails = atoi(Value);
   else if (strcmp(Name, "showEPGDetails") == 0)               showEPGDetails = atoi(Value);
   else if (strcmp(Name, "showVideoInfo") == 0)                showVideoInfo = atoi(Value);
   else if (strcmp(Name, "showMailIcon") == 0)                 showMailIcon = atoi(Value);
   else if (strcmp(Name, "GraphicalProgressbar") == 0)         GraphicalProgressbar = atoi(Value);
   else if (strcmp(Name, "showIcons") == 0)                    showIcons = atoi(Value);
   else if (strcmp(Name, "epgimagesize") == 0)                 EpgImageSize = atoi(Value); // old value
   else if (strcmp(Name, "EpgImageSize") == 0)                 EpgImageSize = atoi(Value);
#ifdef SKINELCHI_HAVE_IMAGEMAGICK
   else if (strcmp(Name, "epgshowtime") == 0)                  EpgImageDisplayTime = atoi(Value); // old value
   else if (strcmp(Name, "EpgImageDisplayTime") == 0)          EpgImageDisplayTime = atoi(Value);
   else if (strcmp(Name, "resize") == 0)                       ImgResizeAlgo = atoi(Value); // old value
   else if (strcmp(Name, "ImgResizeAlgo") == 0)                ImgResizeAlgo = atoi(Value);
#endif

   else if (strcmp(Name, "showRecInfo") == 0)                  showRecInfo = atoi(Value);
   else if (strcmp(Name, "logo") == 0)                         showLogo = atoi(Value); // old value
   else if (strcmp(Name, "showLogo") == 0)                     showLogo = atoi(Value);
   else if (strcmp(Name, "tryChannelDisplay8Bpp") == 0)        tryChannelDisplay8Bpp = atoi(Value);
   else if (strcmp(Name, "useHQlogo") == 0)                    useHQlogo = atoi(Value);
   else if (strcmp(Name, "LogoSearchNameFirst") == 0)          LogoSearchNameFirst = atoi(Value);
   else if (strcmp(Name, "LogoMessages") == 0)                 LogoMessages = atoi(Value);

   else return false;
   return true;
}


