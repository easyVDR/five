/*
 * detector.c: frame size detection and signaling
 *
 * Copyright (C) 2006 - 2007 Jens Vogel
 * Copyright (C) 2007 - 2011 Christoph Haubrich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * See the README file how to reach the author.
 *
 * $Id$
 */

#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <linux/dvb/video.h>
#include <linux/dvb/frontend.h>

#include <vdr/tools.h>
#include <vdr/osdbase.h>
#include <vdr/skins.h>

#include "detector.h"
#include "avards.h"

struct SAspectInfo
{
   float       fMinAspect;
   const char* pszName;
   unsigned    iWssData;
   int         Lines;
};

/*
 according to Wikipedia (http://en.wikipedia.org/wiki/Widescreen_signaling)
 the following codes are used:
                    Aspect
 B3 B2 B1 B0  hex   ratio  Picture placement                   Active lines
 1  0  0  0   0x08    4:3  Full                                      576
 0  0  0  1   0x01   14:9  Letterbox Centre                          504
 0  0  1  0   0x02   14:9  Letterbox Top                             504
 1  0  1  1   0x0b   16:9  Letterbox Centre                          430
 0  1  0  0   0x04   16:9  Letterbox Top                             430
 1  1  0  1   0x0d  >16:9  Letterbox deeper than 16:9
 1  1  1  0   0x0e   14:9  Full-height 4:3, framed to be "14:9-safe" 576
 0  1  1  1   0x07   16:9  Full-height 16:9 (anamorphic)             576
*/

SAspectInfo aAspectInfo[] =
{
   { 0.00,    trNOOP("unknown"), 0x08, 576 },
   { 1.46,    "4:3",             0x08, 576 },
   { 1.66,    "L14:9",           0x01, 504 },
   { 1.90,    "L16:9",           0x0b, 432 },
   { FLT_MAX, "L>16:9",          0x0d, 340 },
   { 1.77,    "16:9",            0x07, 576 }
};

cDetector::cDetector(bool quirkmode, bool GrabFromDevice)
:cThread("avards-detector")
{
   fdDevVideo = -1;
   fdDevDvbVideo = -1;

   snprintf(szDevDvbVideo, MAXSTRING, "%s%s", config.pszDevDvbAdapter, "/video0");
   snprintf(szDevDvbFrontend, MAXSTRING, "%s%s", config.pszDevDvbAdapter, "/frontend0");

   grabfromdevice = GrabFromDevice;
   lastAspectInfo = 0;
   lastWidth = 0;
   lastHeight = 0;
   top    = Setup.OSDTop;
   height = Setup.OSDHeight;

   if (quirkmode) {
      aAspectInfo[AVARDS_VIDEOMODE_UNKNOWN].iWssData = 0x0e;
      aAspectInfo[AVARDS_VIDEOMODE_4_3].iWssData = 0x0e;
   }
}

cDetector::~cDetector()
{
   Stop();
}

#if APIVERSNUM < 10504
bool cDetector::GetMaxOSDSize(avards_MaxOSDsize_v1_0 *OSDsize)
{
   //isyslog("avards: cDetector::GetOSDSize()");
   OSDsize->left   = Setup.OSDLeft;
   OSDsize->width  = Setup.OSDWidth;

   if (Running()) {
      Lock();
      OSDsize->top    = top;
      OSDsize->height = height;
      Unlock();
   } else {
      OSDsize->top    = Setup.OSDTop;
      OSDsize->height = Setup.OSDHeight;
   }
   return true;
}
#endif

const char *cDetector::GetWSSModeString()
{
   Lock();
   int AspectIndex = lastAspectInfo;
   Unlock();
   return tr(aAspectInfo[AspectIndex].pszName);
}

void cDetector::GetVideoFormat(avards_CurrentVideoFormat_v1_0 *format)
{
   Lock();
   format->ModeString = tr(aAspectInfo[lastAspectInfo].pszName);
   format->Mode       = lastAspectInfo;
   format->Width      = lastWidth;
   format->Height     = lastHeight;
   Unlock();
   return;
}

bool cDetector::StartUp()
{
   if (!Running())
      Start();

   return true;
}

void cDetector::Stop()
{
   if (Running()) {
      Cancel(-1);
      Wait.Signal();
   }
}

void cDetector::UninitDevices(void)
{
   if (grabfromdevice && (fdDevVideo >= 0)) {
      close(fdDevVideo);
      fdDevVideo = -1;
   }

   if (fdDevDvbVideo >= 0) {
      close(fdDevDvbVideo);
      fdDevDvbVideo = -1;
   }
}


bool cDetector::InitDevices(const char* DevDvbVideo, const char* DevVideo)
{
   fdDevDvbVideo = open(DevDvbVideo,O_RDONLY|O_NONBLOCK); /// /dev/dvb/adapter0/video0
   if (fdDevDvbVideo < 0) {
      esyslog("avards Error: Can't open dvb video device '%s' (%s)", DevDvbVideo, strerror(errno));
      return false;
   }

   if (grabfromdevice) {
      fdDevVideo = open(DevVideo, O_RDWR); /// /dev/video0
      if (-1 == fdDevVideo) {
         esyslog("avards Error: Can't open video device '%s' (%s)", DevVideo, strerror(errno));
         return false;
      }

      // check device capabilities
      struct v4l2_capability cap;

      if (0 != ioctl(fdDevVideo, VIDIOC_QUERYCAP, &cap))
      {
         esyslog("avards Error: query cap of '%s' failed (%s)", DevVideo, strerror(errno));
         return false;
      }

      if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
      {
         esyslog("avards Error: '%s' is no capture device (%s)", DevVideo, strerror(errno));
         return false;
      }

      if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
         esyslog("avards Error: '%s' does not support streaming I/O (%s)", DevVideo, strerror(errno));
         return false;
      }

      // set default cropping and scaling
      struct v4l2_cropcap cropcap;
      struct v4l2_crop crop;

      memset(&cropcap, 0, sizeof(cropcap));
      cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

      if (0 == ioctl(fdDevVideo, VIDIOC_CROPCAP, &cropcap))
      {
         crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         crop.c = cropcap.defrect; /* reset to default */
         ioctl(fdDevVideo, VIDIOC_S_CROP, &crop);
      }
   }

   return true;
}


int cDetector::AnalyzeFrame(void)
{
   static int iAspectInfo = 0;
   bool bFormatChanged = false;

   const int nMaxLogoWidth = int(ImageWidth*config.MaxLogoWidth_pct/100);
   const int nYoverscan = int (ImageHeight*config.Overscan_pct/100);
   const int nXoverscan = int (ImageWidth*config.Overscan_pct/100);

   unsigned char iPanToneMin;
   unsigned char iPanToneMax;

   int nActiveLines = 0;
   int iBlackLines = 999;
   int iYtop = nYoverscan;

   if (config.PanToneBlackMax) {
      iPanToneMin = 0;
      iPanToneMax = config.PanToneBlackMax;
   }
   else {
      int aHistogram[256];
      memset(aHistogram, 0, sizeof(aHistogram));
      unsigned char *pbVideoLine = pbVideo+iYtop*ImageWidth;
      for (int iX = nXoverscan; iX < ImageWidth-nXoverscan; iX++)
         aHistogram[pbVideoLine[iX]]++;
      int iXmax = 0;
      for (int i = 0; i < 256; i++)
         if (aHistogram[i] > aHistogram[iXmax])
            iXmax = i;
         if (iXmax - config.PanToneTolerance < 0)
            iPanToneMin = 0;
         else
            iPanToneMin = iXmax - config.PanToneTolerance;
         if (iXmax + config.PanToneTolerance > 255)
            iPanToneMax = 255;
         else
            iPanToneMax = iXmax + config.PanToneTolerance;
   }

   while (iYtop < ImageHeight/2) {
      unsigned char *pbVideoLine = pbVideo+iYtop*ImageWidth;
      int iXleft = nXoverscan;
      while ( (iXleft < (ImageWidth-nXoverscan)) &&
               (pbVideoLine[iXleft] >= iPanToneMin)    &&
               (pbVideoLine[iXleft] <= iPanToneMax)  )
         iXleft++;

      int iXright = ImageWidth-nXoverscan-1;
      while ( (iXright > iXleft) &&
               (pbVideoLine[iXright] >= iPanToneMin) &&
               (pbVideoLine[iXright] <= iPanToneMax) )
         iXright--;

      if (iXright - iXleft > nMaxLogoWidth)
         break;
      iYtop++;
   }

   if (iYtop < ImageHeight/2) {
      int iYbottom = nYoverscan;
      while (iYbottom < ImageHeight/2)
      {
         unsigned char *pbVideoLine = pbVideo+(ImageHeight-1-iYbottom)*ImageWidth;
         int iXleft = nXoverscan;
         while ( (iXleft < (ImageWidth-nXoverscan)) &&
                  (pbVideoLine[iXleft] >= iPanToneMin) &&
                  (pbVideoLine[iXleft] <= iPanToneMax)  )
            iXleft++;

         int iXright = ImageWidth-nXoverscan-1;
         while ( (iXright > iXleft) &&
                  (pbVideoLine[iXright] >= iPanToneMin) &&
                  (pbVideoLine[iXright] <= iPanToneMax) )
            iXright--;

         if (iXright - iXleft > nMaxLogoWidth)
            break;
         iYbottom++;
      }

      if (iYbottom < ImageHeight/2) {
         iBlackLines = 2*min(iYtop, iYbottom);
         nActiveLines = ImageHeight - iBlackLines;
      }
   }

   if (nActiveLines > 0) {
      const float fLinearWidth = 4./3.*ImageHeight;
      if (iAspectInfo == 0)
         bFormatChanged = true;
      else {
         const int nAspectHyst = 2;
         int nMinActiveLines = (int)(fLinearWidth/aAspectInfo[iAspectInfo].fMinAspect) - nAspectHyst;
         int nMaxActiveLines =   (iAspectInfo > 1)
            ? (int)(fLinearWidth/aAspectInfo[iAspectInfo-1].fMinAspect) + nAspectHyst
            : ImageHeight;
         bFormatChanged = (nActiveLines < nMinActiveLines) || (nActiveLines > nMaxActiveLines);
      }
      float fAspect = fLinearWidth/nActiveLines;
      if (bFormatChanged) {
         if (config.ShowAllLines) {
            iAspectInfo = config.Overzoom ? 4 : 3;
            while (nActiveLines > aAspectInfo[iAspectInfo].Lines && iAspectInfo > 1)
               iAspectInfo--;
         }
         else {
            iAspectInfo = 1;
            while (fAspect > aAspectInfo[iAspectInfo].fMinAspect && iAspectInfo < (config.Overzoom?4:3))
               iAspectInfo++;
         }
      }
   }

   return iAspectInfo;
}


void cDetector::Action(void)
{
   naAspectInfo = sizeof(aAspectInfo) / sizeof(SAspectInfo);
   pbVideo = NULL;

   if (InitDevices(szDevDvbVideo, config.pszDevVideo)) {

      wssoverdrive = new cWSSoverdrive(config.pszDevVbi);
      int iAspectInfo = 0;
      Lock();
      lastAspectInfo = 0;
      Unlock();
      char ModeBuffer[MAXDELAY];
      memset (&ModeBuffer, 0, sizeof(ModeBuffer));

      bool bError = false;
      bool isPAL = config.VDROSDisPAL;
      video_size_t videoSize;

      while (!bError && Running()) {
         Wait.Wait(max(80, config.PollRate));
         // get displayed video size (might not be the same as the video size of the stream)
         if (grabfromdevice) {
            v4l2_std_id std_id;
            bError = ioctl(fdDevVideo, VIDIOC_G_STD, &std_id) < 0;
            if (bError) {
               esyslog("avards ERROR: VIDIOC_G_STD failed: %s", strerror(errno));
               continue;
            }
            if (!(std_id & (V4L2_STD_PAL|V4L2_STD_NTSC))) {
                esyslog("avards ERROR: VIDIOC_G_STD returned an unknown video standard (%x)\n", (unsigned int)std_id);
                bError = true;
                continue;
            }
            isPAL = std_id & V4L2_STD_PAL ? true : false;
            struct v4l2_format fmt;
            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            // try to capture as large as possible ;-)
            fmt.fmt.pix.width = 100000;
            fmt.fmt.pix.height = 100000;
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
            fmt.fmt.pix.field = V4L2_FIELD_ANY;
            bError = ioctl(fdDevVideo, VIDIOC_TRY_FMT, &fmt) < 0;
            if (bError) {
               esyslog("avards ERROR: VIDIOC_TRY_FMT failed: %s", strerror(errno));
               continue;
            }
            ImageWidth = fmt.fmt.pix.width;
            ImageHeight = fmt.fmt.pix.height;
         }
         else {
            isPAL = cDevice::PrimaryDevice()->GetVideoSystem() == vsPAL;
            if (isPAL) {
               ImageHeight = 576;
               ImageWidth = 768;
            }
            else {
               ImageHeight = 480;
               ImageWidth = 640;
            }
         }

         // get broadcasted video size and aspect ratio
         bError = ioctl(fdDevDvbVideo, VIDEO_GET_SIZE, &videoSize) < 0;
         if (bError) {
            esyslog("avards Error: Can't get video size on device '%s' (%s)", szDevDvbVideo, strerror(errno));
            continue;
         }
         //isyslog("ImageHeight=%d/%d ImageWidth=%d/%d isPAL=%s\n", ImageHeight, videoSize.h, ImageWidth, videoSize.w, isPAL ? "yes" : "no");

         if (config.Mode > MODE_AUTO)
            iAspectInfo = config.Mode;
         else {
            iAspectInfo = 0;

            switch (videoSize.aspect_ratio) {
            case VIDEO_FORMAT_221_1:
               //isyslog("avards: aspect ratio 2.21:1 sent - using 16:9");
               // fall through
            case VIDEO_FORMAT_16_9:
               iAspectInfo = 5;
               break;

            case  VIDEO_FORMAT_4_3:
            default:
               if (isPAL) {
                  int msize = 0;
                  if (grabfromdevice) {
                     // grab with V4l2
                     bError = true;

                     v4l2_format fmt;
                     memset(&fmt, 0, sizeof(fmt));
                     fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                     fmt.fmt.pix.width = ImageWidth;
                     fmt.fmt.pix.height = ImageHeight;
                     fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
                     fmt.fmt.pix.field = V4L2_FIELD_ANY;
                     if (ioctl(fdDevVideo, VIDIOC_S_FMT, &fmt) != 0)
                        esyslog("avards ERROR: VIDIOC_S_FMT failed: %s", strerror(errno));
                     else {
                        v4l2_requestbuffers reqBuf;
                        memset(&reqBuf, 0, sizeof(reqBuf));
                        reqBuf.count = 2;
                        reqBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        reqBuf.memory = V4L2_MEMORY_MMAP;
                        if (ioctl(fdDevVideo, VIDIOC_REQBUFS, &reqBuf) < 0)
                           esyslog("avards ERROR: VIDIOC_REQBUFS failed: %s", strerror(errno));
                        else {
                           v4l2_buffer mbuf;
                           memset(&mbuf, 0, sizeof(mbuf));
                           mbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                           mbuf.memory = V4L2_MEMORY_MMAP;
                           if (ioctl(fdDevVideo, VIDIOC_QUERYBUF, &mbuf) != 0)
                              esyslog("avards ERROR: VIDIOC_QUERYBUF failed: %s", strerror(errno));
                           else {
                              msize = mbuf.length;
                              pbVideo = (unsigned char *)mmap(0, msize, PROT_READ | PROT_WRITE, MAP_SHARED, fdDevVideo, 0);
                              if (!(pbVideo && pbVideo != (unsigned char *)-1)) {
                                 esyslog("avards ERROR: failed to memmap video device: %s", strerror(errno));
                                 pbVideo = NULL;
                              }
                              else {
                                 //isyslog("avards: mmap %08llX, %08X", (unsigned long long)pbVideo, msize);
                                 v4l2_buffer buf;
                                 memset(&buf, 0, sizeof(buf));
                                 buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                 buf.memory = V4L2_MEMORY_MMAP;
                                 buf.index = 0;

                                 if (ioctl(fdDevVideo, VIDIOC_QBUF, &buf) != 0)
                                    esyslog("avards ERROR: VIDIOC_QBUF failed: %s", strerror(errno));
                                 else {
                                    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                    if (ioctl (fdDevVideo, VIDIOC_STREAMON, &type) != 0)
                                       esyslog("avards ERROR: VIDIOC_STREAMON failed: %s", strerror(errno));
                                    else {
                                       memset(&buf, 0, sizeof(buf));
                                       buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                       buf.memory = V4L2_MEMORY_MMAP;
                                       buf.index = 0;
                                       if (ioctl(fdDevVideo, VIDIOC_DQBUF, &buf) != 0)
                                          esyslog("avards ERROR: VIDIOC_DQBUF failed: %s", strerror(errno));
                                       else {
                                          if (ioctl(fdDevVideo, VIDIOC_STREAMOFF, &type) != 0)
                                             esyslog("avards ERROR: VIDIOC_STREAMOFF failed: %s", strerror(errno));
                                          else
                                             // we finally got an image!
                                             bError = false;
                                       }
                                    }
                                 }
                              }
                           }
                           }
                     }
                  }
                  else {
                     // grab from VDR
                     int ImageSize;
                     bError = true;
                     for (int i = 0; i < 10; i++) {
                        pbVideo = cDevice::PrimaryDevice()->GrabImage(ImageSize, false, 100, 0, 0);
                        // GrabImage returns NULL or pointer to PNM-bitmap
                        if (pbVideo || !Running())
                           break;
                     }
                     if (pbVideo) {
                        const char format[] = "P6\n%d\n%d\n255\n";
                        char header[50];
                        if (2 == sscanf((char*)pbVideo, format, &ImageWidth, &ImageHeight) && ImageHeight && ImageWidth) {
                           sprintf(header, format, ImageWidth, ImageHeight);
                           unsigned char* src = pbVideo + strlen(header);
                           unsigned char* dst = pbVideo;
                           ImageSize = ImageWidth * ImageHeight;
                           unsigned char r, g, b;
                           for (int i = 0; i < ImageSize; i++) {
                              r = *src++;
                              g = *src++;
                              b = *src++;
                              *dst++ = (unsigned char)((299 * (unsigned long)r + 587 * (unsigned long)g + 114 * (unsigned long)b) / 1000);
                           }
                           bError = false;
                        } else {
                           free(pbVideo);
                           pbVideo = NULL;
                        }
                     }
                  }

                  if (bError) {
                     esyslog("avards: Error: Can't grab image");
                     continue;
                  }

                  iAspectInfo = AnalyzeFrame();
                  if (grabfromdevice) {
                     munmap(pbVideo, msize);
                     //int RC = munmap(pbVideo, msize);
                     //isyslog("avards: munmap %08llX, RC=%08X, errno=%d", (unsigned long long)pbVideo, RC, errno);
                  }
                  else
                     free(pbVideo);
                  pbVideo = NULL;
               }
               break;
            }
         }
         //isyslog("avards1: iAspectInfo=%d", iAspectInfo);

         if (!bError) {
            int iDelay = 0;
            if (iAspectInfo == 5 || !isPAL)
               iDelay = config.Delay;
            else
               if (iAspectInfo > 0) {
                  while ((iDelay < config.Delay) && ModeBuffer[iDelay] == iAspectInfo)
                  iDelay++;
               }

            if (iDelay == config.Delay && (lastAspectInfo != iAspectInfo || wasPAL != isPAL)) {
               int newtop, newheight;
               cString msg;

               if (isPAL) { // PAL: setup WSS
                  isyslog("avards: switching WSS to %s", aAspectInfo[iAspectInfo].pszName);
                  int BlackLines = ImageHeight - aAspectInfo[iAspectInfo].Lines;
                  wssoverdrive->SetWSS(aAspectInfo[iAspectInfo].iWssData);
                  newtop    = iAspectInfo?Setup.OSDTop + BlackLines/2 : Setup.OSDTop;
                  newheight = iAspectInfo?Setup.OSDHeight - BlackLines : Setup.OSDHeight;
                  if (config.ShowMsg)
                     msg = cString::sprintf("%s %s", tr("switching WSS mode to"), aAspectInfo[iAspectInfo].pszName);
               }
               else {  // non-PAL: stop WSS
                  isyslog("avards: switching off WSS");
                  wssoverdrive->SetWSS(0);
                  double factor = config.VDROSDisPAL ? ImageHeight/576.0 : ImageHeight/480.0;
                  newtop    = int(factor * Setup.OSDTop);
                  newheight = int(factor * Setup.OSDHeight);
                  //isyslog("avards:non-PAL: %d/%d f=%.2f %d/%d %d/%d", ImageWidth, ImageHeight, factor, Setup.OSDTop, newtop, Setup.OSDHeight, newheight);
                  if (newheight < MINOSDHEIGHT) {
                     newheight = MINOSDHEIGHT;
                     newtop = (ImageHeight - newheight) / 2;
                  }
                  if (newheight > MAXOSDHEIGHT) {
                     newheight = MAXOSDHEIGHT;
                     newtop = (ImageHeight - newheight) / 2;
                  }
                  if (config.ShowMsg)
                     msg = cString::sprintf("%s", tr("switching off WSS"));
               }

               Lock();
               top    = newtop;
               height = newheight;
               lastAspectInfo = iAspectInfo;
               lastWidth = videoSize.w;
               lastHeight = videoSize.h;
               wasPAL = isPAL;
               Unlock();
#if APIVERSNUM >= 10504
//             isyslog("top=%d, height=%d\n", top, height);
               cOsd::SetOsdPosition(Setup.OSDLeft, top, Setup.OSDWidth, height);
#endif

               if (config.ShowMsg) {
                  Skins.QueueMessage(mtInfo, msg, 0, -1);
               }
            }

            if (config.Delay) {
               memmove(&ModeBuffer[1], &ModeBuffer[0], config.Delay-1);
               ModeBuffer[0] = iAspectInfo;
            }
         }
      }

      lastAspectInfo = 0;
      top    = Setup.OSDTop;
      height = Setup.OSDHeight;
#if APIVERSNUM >= 10504
      cOsd::SetOsdPosition(Setup.OSDLeft, Setup.OSDTop, Setup.OSDWidth, Setup.OSDHeight);
#endif

      isyslog("avards: switching off WSS");
      wssoverdrive->SetWSS(0);
      delete wssoverdrive;
      wssoverdrive = NULL;
   }
   UninitDevices();
}


cWSSoverdrive::cWSSoverdrive(const char* szDevice)
{
   fdDevVbi = -1;
   lastWssData = -1;
   szVbiDevice = strdup(szDevice);
}

cWSSoverdrive::~cWSSoverdrive(void)
{
   if (fdDevVbi >= 0)
      CloseVbiDevice();
   free(szVbiDevice);
}

bool cWSSoverdrive::OpenVbiDevice(void)
{
   fdDevVbi = open(szVbiDevice, O_WRONLY);
   if (fdDevVbi < 0)
   {
      esyslog("avards: Error: Can't open vbi device '%s' (%s)", szVbiDevice, strerror(errno));
      return false;
   }

   struct v4l2_format fmt;
   memset(&fmt, 0, sizeof fmt);
   fmt.type = V4L2_BUF_TYPE_SLICED_VBI_OUTPUT;
   fmt.fmt.sliced.service_set = V4L2_SLICED_WSS_625;
   if (ioctl(fdDevVbi, VIDIOC_S_FMT, &fmt) < 0) {
      esyslog("avards Error: Can't init vbi device '%s' (%s)", szVbiDevice, strerror(errno));
      CloseVbiDevice();
      return false;
   }

   return true;
}

void cWSSoverdrive::CloseVbiDevice(void)
{
   if (fdDevVbi >= 0) {
      //isyslog("avards: closing vbi device");
      close(fdDevVbi);
      fdDevVbi = -1;
   }
}

bool cWSSoverdrive::SetWSS(int iWssData)
{
   bool result = true;

   if ( 0 == iWssData) {
      CloseVbiDevice();
   }
   else {
      if (fdDevVbi < 0 && !OpenVbiDevice())
         return false;

      if (iWssData != lastWssData) {
         //isyslog("avards: setting WSS data to %02x", iWssData);
         struct v4l2_sliced_vbi_data vbidata;
         memset(&vbidata, 0, sizeof(vbidata));
         vbidata.id = (iWssData) ? V4L2_SLICED_WSS_625 : 0;
         vbidata.line = 23;
         vbidata.data[0] = iWssData & 0xff;
         vbidata.data[1] = (iWssData >> 8) & 0x3f;

         if (write(fdDevVbi, &vbidata, sizeof(vbidata)) != sizeof(vbidata)) {
            esyslog("avards Error: write to vbi device failed (%s)", strerror(errno));
            CloseVbiDevice();
            result = false;
         }
         lastWssData = iWssData;
      }
   }
   return result;
}
