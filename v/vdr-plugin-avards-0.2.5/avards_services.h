/*
 * avards-services.h: definitions for the service interface
 *
 * See the main source file 'avards.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __AVARDS_SERVICES_H
#define __AVARDS_SERVICES_H

#if APIVERSNUM < 10504
#define AVARDS_MAXOSDSIZE_SERVICE_STRING_ID "avards-MaxOSDsize-v1.0"

struct avards_MaxOSDsize_v1_0 {
  int left, top, width, height;
};
#endif

#define AVARDS_CURRENT_WSSMODE_SERVICE_STRING_ID "avards-CurrentWSSMode-v1.0"

struct avards_CurrentWssMode_v1_0 {
  const char *ModeString;
};


#define AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID "avards-CurrentVideoFormat-v1.0"

struct avards_CurrentVideoFormat_v1_0 {
  const char *ModeString;
  int Mode;
  int Width;
  int Height;
};

#define AVARDS_VIDEOMODE_UNKNOWN 0
#define AVARDS_VIDEOMODE_4_3     1
#define AVARDS_VIDEOMODE_L14_9   2
#define AVARDS_VIDEOMODE_L16_9   3
#define AVARDS_VIDEOMODE_LG16_9  4
#define AVARDS_VIDEOMODE_16_9    5

#endif // __AVARDS_SERVICES_H
