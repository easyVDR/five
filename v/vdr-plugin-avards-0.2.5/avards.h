/*
 * avards.h: the structure for global parameters
 *
 * See the main source file 'avards.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#define MAXDELAY 100

// modes correspond with aAspectInfo[]
#define MODE_AUTO  0
#define MODE_43    1
#define MODE_L149  2
#define MODE_L169  3
#define MODE_LG169 4
#define MODE_169   5

struct cParameter
{
   int ShowInMainMenu;
   int AutoStart;
   int Mode;
   char *pszDevVideo;
   char *pszDevDvbAdapter;
   char *pszDevVbi;
   int PanToneTolerance;
   int PanToneBlackMax;
   int Overscan_pct;
   int ShowAllLines;
   int MaxLogoWidth_pct;
   int PollRate;
   int Delay;
   int Overzoom;
   int ShowMsg;
   int VDROSDisPAL;
};

extern cParameter config;
