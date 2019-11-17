/*
 * videostatus.h: Keeping track of video status information
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINELCHI_VIDEOSTATUS_H_
#define __SKINELCHI_VIDEOSTATUS_H_

#include <vdr/thread.h>
#include <vdr/plugin.h>

// use the following #define only if you really need the kernelspace header files
// you should really consider installing the corresponding glibc header files from packages like linux-glibc-devel ! 
// #define __user     
#include <linux/dvb/video.h>


typedef enum {
   ar_unknown,
   ar4_3,
   arL14_9,
   arL16_9,
   arLG16_9,
   ar16_9,
   ar221_1,
   arHD
} eAspectRatio;

struct cVideoInfo
{
   int width;       // in pixels
   int height;      // in pixels
   eAspectRatio aspectratio; // aspect ratio
};

class cVideoStatus : public cThread
{
private:
   cPlugin *avards;
   cCondWait Wait;
   int height;
   int width;
   eAspectRatio aspectratio;
   virtual void Action(void);
public:
   cVideoStatus(void);
   ~cVideoStatus();
   void Activate(bool);
   void Stop();
   bool GetVideoInfo(cVideoInfo *videoinfo);
};

extern cVideoStatus *VideoStatus;

#endif
