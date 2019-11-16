/*
 * detector.h: frame size detection and signaling
 *
 * See the source file 'detector.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <float.h>
#include <vdr/thread.h>
#include <vdr/device.h>
#include "avards_services.h"

#define MAXSTRING 100

class cWSSoverdrive
{
private:
   int fdDevVbi;
   int lastWssData;
   char *szVbiDevice;
public:
   cWSSoverdrive(const char* szDevice);
   ~cWSSoverdrive();
   bool OpenVbiDevice(void);
   void CloseVbiDevice(void);
   bool SetWSS(int);
};

class cDetector : public cThread
{
private:
   bool grabfromdevice;
   virtual void Action(void);
   cCondWait Wait;
   char szDevDvbVideo[MAXSTRING];
   char szDevDvbFrontend[MAXSTRING];
   int ImageWidth;
   int ImageHeight;
   int naAspectInfo;
   int lastAspectInfo;
   int lastHeight, lastWidth;
   int top, height;
   bool wasPAL;
   cWSSoverdrive *wssoverdrive;

   bool InitDevices(const char* DevDvbVideo, const char* DevVideo);
   void UninitDevices(void);
   int AnalyzeFrame(void);
   unsigned char *pbVideo;

   // for v4l2:
   int fdDevVideo;
   int fdDevDvbVideo;
   bool init_devices(const char* pszDevVideo, const char* pszDevDvbVideo);
   void uninit_devices(void);

public:
   cDetector(bool, bool);
   ~cDetector();
   bool Active() { return Running(); }
   bool StartUp();
   void Stop();
#if APIVERSNUM < 10504
   bool GetMaxOSDSize(avards_MaxOSDsize_v1_0 *);
#endif
   const char *GetWSSModeString();
   void GetVideoFormat(avards_CurrentVideoFormat_v1_0 *);
};
