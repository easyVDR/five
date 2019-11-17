/*
 * videostatus.c: Keeping track of video status information
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <sys/ioctl.h>

#include "config.h"
#include "common.h"
#include "vdrstatus.h"  // für ReplayingName
#include "videostatus.h"

#define ELCHI_DEV_DVB_VIDEO     "/dev/dvb/adapter%d/video%d"

#ifdef DVB_OLDER_20050331
#error DVB_OLDER_20050331 is no longer supported with this version of skinElchi
#endif

// --------------------- VideoStatus -----------------------------
cVideoStatus::cVideoStatus(void)
:cThread("skinelchi-video-status")
{
   DSYSLOG("skinelchi: cVideoStatus::cVideoStatus() is called")
   avards = NULL;
}

cVideoStatus::~cVideoStatus()
{
   DSYSLOG("skinelchi: cVideoStatus::~cVideoStatus() is called")
   Stop();
}

void cVideoStatus::Stop()
{
   DSYSLOG("skinelchi: cVideoStatus::Stop() is called")
   if (Running()) {
      Cancel(-1);
      Wait.Signal();
   }
}

void cVideoStatus::Activate(bool activate)
{
#if VDRVERSNUM < 10707
   if (activate) {
      bool using_avards = false;
      avards = cPluginManager::GetPlugin("avards");

      if (avards) {
         avards_CurrentVideoFormat_v1_0 videoformat;
         avards->Service(AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID, &videoformat);
         using_avards = videoformat.Mode != AVARDS_VIDEOMODE_UNKNOWN;
      }
      if (!using_avards)
         Start();
   }
   else
      Stop();
#endif
}

bool cVideoStatus::GetVideoInfo(cVideoInfo *videoinfo)
{
#if VDRVERSNUM >= 10707
   int Width, Height;
   double VideoAspect;
   cDevice::PrimaryDevice()->GetVideoSize(Width, Height, VideoAspect);
   videoinfo->height      = Height;
   videoinfo->width       = Width;
   if (Height >= 720) videoinfo->aspectratio = arHD;
   else if (Height == 0 && Width == 0) videoinfo->aspectratio = ar_unknown;
      else if (VideoAspect == 4.0/3.0) videoinfo->aspectratio = ar4_3;
         else if(VideoAspect == 16.0/9.0) videoinfo->aspectratio = ar16_9;
            else if (VideoAspect == 2.21) videoinfo->aspectratio = ar221_1;
               else videoinfo->aspectratio = ar_unknown;
   //isyslog("skinelchi: %d x %d @ %.5f", Width, Height, VideoAspect);
   return true;
#else
   if (Running()) {
      Lock();
      videoinfo->height      = height;
      videoinfo->width       = width;
      videoinfo->aspectratio = aspectratio;
      Unlock();
      return true;
   }
   else {
      if (avards) {
         avards_CurrentVideoFormat_v1_0 videoformat;
         avards->Service(AVARDS_CURRENT_VIDEO_FORMAT_SERVICE_STRING_ID, &videoformat);
         //isyslog("skinelchi: cVideoStatus avards %d*%d %X %s", videoformat.Width, videoformat.Height, videoformat.Mode, videoformat.ModeString);
         videoinfo->width  = videoformat.Width;
         videoinfo->height = videoformat.Height;

         switch (videoformat.Mode) {
            case AVARDS_VIDEOMODE_4_3:
               videoinfo->aspectratio = ar4_3;
               break;
            case AVARDS_VIDEOMODE_L14_9:
               videoinfo->aspectratio = arL14_9;
               break;
            case AVARDS_VIDEOMODE_L16_9:
               videoinfo->aspectratio = arL16_9;
               break;
            case AVARDS_VIDEOMODE_LG16_9:
               videoinfo->aspectratio = arL16_9;
               break;
            case AVARDS_VIDEOMODE_16_9:
               videoinfo->aspectratio = ar16_9;
               break;
            default:
               videoinfo->aspectratio = ar_unknown;
               break;
         }
         return true;
      }
   }
#endif
   return  false;
}

void cVideoStatus::Action(void)
{
   DSYSLOG("skinelchi: cVideoStatus::Action()")

   int fd_video = -1;

   if (cDevice::PrimaryDevice()->HasDecoder() && ElchiConfig.showVideoInfo) {
      char dev[sizeof(ELCHI_DEV_DVB_VIDEO)];
      snprintf(dev, sizeof(dev), ELCHI_DEV_DVB_VIDEO, cDevice::PrimaryDevice()->CardIndex(), 0);
      fd_video = open(dev, O_RDONLY | O_NONBLOCK);
      if (fd_video < 0) {
         esyslog("skinelchi: opening video device %s failed", dev);
         return;
      }
   }

   while (Running()) {
      video_size_t vs;
      int rc = ioctl(fd_video, VIDEO_GET_SIZE, &vs);
      Lock();
      if (rc == 0) {
         width = vs.w;
         height = vs.h;
         switch (vs.aspect_ratio) {
            case VIDEO_FORMAT_4_3:
               aspectratio = ar4_3;
               break;
            case VIDEO_FORMAT_16_9:
               aspectratio = ar16_9;
               break;
            case VIDEO_FORMAT_221_1:
               aspectratio = ar221_1;
               break;
            default:
               aspectratio = ar_unknown;
               break;
         }

         DSYSLOG2("skinelchi: cVideoStatus %d*%d %d", vs.w, vs.h, vs.aspect_ratio)
      }
      else {
         width = 0;
         height = 0;
         aspectratio = ar_unknown;
         DSYSLOG2("skinelchi: cVideoStatus Error, rc=%d", rc)
      }
      Unlock();

      if (Running())
         Wait.Wait(250);

   } // while Running()

   Lock();
   width = 0;
   height = 0;
   aspectratio = ar_unknown;
   Unlock();

   close(fd_video);

   DSYSLOG("skinelchi: cVideoStatus - Action end")
}
