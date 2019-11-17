/*
 * config.h: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: config.h,v 1.28 2012/11/28 06:29:24 wendel Exp $
 */

#ifndef __SEDU_CONFIG_H
#define __SEDU_CONFIG_H

#include "common.h"
#include "ledsconf.h"
#include "seduservice.h"

//***************************************************************************
// Configuration
//***************************************************************************

class cSeduConfig : public cSeduService
{
   public:

      cSeduConfig();
      ~cSeduConfig();

      // geometry

      int grabWidth;
      int grabHeight;

      // adjust 

      int threshold;
      int adjGreen;
      int adjRed;
      int adjBlue;
      int frequence;
      unsigned int average;
      int gamma;
      int xDeep;
      int yDeep;
      int black;

      // technical

      ViewMode viewMode;
      int fixedR;
      int fixedG;
      int fixedB;
      int effectSpeed;

      int showMainmenu;
      char seduRGBOrder[4];
      SeduMode seduMode;
      Cinebars detectCineBars;

      int loglevel;

      cLed* leds;
      int ledCount;

      // functions

      cLed* createLeds(cLedConfs* conf);
      void copyLeds(cSeduConfig* c) 
      {
         ledCount = c->ledCount;
         leds = new cLed[ledCount];
         memset(leds, 0, ledCount*sizeof(cLed));
         
         for (int i = 0; i < ledCount; i++)
            leds[i] = c->leds[i];
      }
};

//***************************************************************************
// Global Configuration
//***************************************************************************

extern cSeduConfig cfg;

#endif // __SEDU_CONFIG_H
