/*
 * dm140vfd plugin for VDR (C++)
 *
 * (C) 2010 Andreas Brachold <vdr07 AT deltab de>

 * This dm140vfd plugin is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 3 of the License.
 *
 * See the files README and COPYING for details.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

#include <vdr/tools.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/hiddev.h>

#include "setup.h"
#include "ffont.h"
#include "vfd.h"

/* 
 * Bits in the image mask for DM140 display. "-1" because we use this as shift values
 */
#define DM140_ICON_RECORD_BIT	(9-1)
#define DM140_ICON_MUTE_BIT	(10-1)
#define DM140_ICON_FWD_BIT	(11-1)
#define DM140_ICON_PAUSE_BIT	(12-1)
#define DM140_ICON_RWD_BIT	(13-1)
#define DM140_ICON_PLAY_BIT	(14-1)
#define DM140_ICON_CD_BIT	(15-1)
#define DM140_ICON_VIDEO_BIT	(16-1)
#define DM140_ICON_DVD_BIT	(17-1)
#define DM140_ICON_RADIO_BIT	(18-1)
#define DM140_ICON_VOLUME_BIT	(19-1)
#define DM140_ICON_VOL1_BIT	(20-1)
#define DM140_ICON_VOL2_BIT	(24-1)
#define DM140_ICON_VOL3_BIT	(28-1)
#define DM140_ICON_VOL4_BIT	(32-1)
#define DM140_ICON_VOL5_BIT	(36-1)
#define DM140_ICON_VOL6_BIT	(40-1)
#define DM140_ICON_VOL7_BIT	(44-1)
#define DM140_ICON_VOL8_BIT	(48-1)
#define DM140_ICON_VOL9_BIT	(52-1)
#define DM140_ICON_VOL10_BIT	(56-1)
#define DM140_ICON_VOL11_BIT	(60-1)
#define DM140_ICON_VOL12_BIT	(64-1)

//
// this is used to map cVFD icon bits to DM140 icon bits. For example cVFD bit 0 is used for Play, which is bit 14 on the DM140
// -1 means this bit is not supported
//
static char icon_bits[] =
{
  DM140_ICON_PLAY_BIT,  DM140_ICON_PAUSE_BIT, DM140_ICON_RECORD_BIT, DM140_ICON_RWD_BIT,
  DM140_ICON_FWD_BIT,   DM140_ICON_MUTE_BIT,  DM140_ICON_RADIO_BIT,  -1,
  -1,                   -1,                   DM140_ICON_VOLUME_BIT, DM140_ICON_VOL1_BIT,
  DM140_ICON_VOL2_BIT,  DM140_ICON_VOL3_BIT,  DM140_ICON_VOL4_BIT,   DM140_ICON_VOL5_BIT,
  DM140_ICON_VOL6_BIT,  DM140_ICON_VOL7_BIT,  DM140_ICON_VOL8_BIT,   DM140_ICON_VOL9_BIT,
  DM140_ICON_VOL10_BIT, DM140_ICON_VOL11_BIT, DM140_ICON_VOL12_BIT
};

static const byte ICON_PLAY       = 0x00; //Play
static const byte ICON_PAUSE      = 0x01; //Pause
static const byte ICON_RECORD     = 0x02; //Record
static const byte ICON_MESSAGE    = 0x03; //Message symbol (without the inner @)  - abused for rewind
static const byte ICON_MSGAT      = 0x04; //Message @                             - abused for forward
static const byte ICON_MUTE       = 0x05; //Mute
static const byte ICON_WLAN1      = 0x06; //WLAN (tower base)                     - we only have one wlan symbol on the DM140
static const byte ICON_WLAN2      = 0x07; //WLAN strength (1 of 3)                - we only have one wlan symbol on the DM140
static const byte ICON_WLAN3      = 0x08; //WLAN strength (2 of 3)                - we only have one wlan symbol on the DM140
static const byte ICON_WLAN4      = 0x09; //WLAN strength (3 of 3)                - we only have one wlan symbol on the DM140
static const byte ICON_VOLUME     = 0x0A; //Volume (the word)
static const byte ICON_VOL1       = 0x0B; //Volume level 1 of 14
static const byte ICON_VOL2       = 0x0C; //Volume level 2 of 14
static const byte ICON_VOL3       = 0x0D; //Volume level 3 of 14
static const byte ICON_VOL4       = 0x0E; //Volume level 4 of 14
static const byte ICON_VOL5       = 0x0F; //Volume level 5 of 14
static const byte ICON_VOL6       = 0x10; //Volume level 6 of 14
static const byte ICON_VOL7       = 0x11; //Volume level 7 of 14
static const byte ICON_VOL8       = 0x12; //Volume level 8 of 14
static const byte ICON_VOL9       = 0x13; //Volume level 9 of 14
static const byte ICON_VOL10      = 0x14; //Volume level 10 of 14
static const byte ICON_VOL11      = 0x15; //Volume level 11 of 14
static const byte ICON_VOL12      = 0x16; //Volume level 12 of 14
//static const byte ICON_VOL13      = 0x17; //Volume level 13 of 14
//static const byte ICON_VOL14      = 0x18; //Volume level 14 of 14

#define VALUE_FILTER(_value)  (_value>0x7F)?(__s32)(0xFFFFFF00 | _value):(_value)

cVFD::cVFD() 
{
	this->pFont = NULL;
	this->lastIconState = 0;
}

cVFD::~cVFD() {
  this->close();
}

/**
 * Initialize the driver.
 * \retval 0	   Success.
 * \retval <0	  Error.
 */
bool cVFD::open()
{
  int vendor, product;

  fd = -1;   

  if(!SetFont(theSetup.m_szFont, 
              theSetup.m_nRenderMode == eRenderMode_DualLine, 
              theSetup.m_nBigFontHeight, 
              theSetup.m_nSmallFontHeight)) {
		return false;
  }

  // default values
  width = theSetup.m_cWidth;
  if (width <= 0)
     width = 112;
  height = theSetup.m_cHeight;
  if (height <= 0)
     height = 16;

  if (theSetup.m_nDisplayType == eDisplayType_Hiper) {
     vendor  = 0x040b;
     product = 0x7001;
  } else if (theSetup.m_nDisplayType == eDisplayType_Activy) {
     vendor  = 0x1509;
     product = 0x925d;
  } else {
     esyslog("DM140VFD: unknown DisplayType %d", theSetup.m_nDisplayType);
     return false;
  }

  //******************************************************
  // Loop through all the 16 HID ports/devices looking for
  // one that matches our device.
  //******************************************************
  const char *hiddev_prefix = "/dev/usb/hiddev"; /* in devfs */
  for(int i=0;i<16;i++)
  {
        char port[32];
        sprintf(port, "%s%d", hiddev_prefix, i);
        //dsyslog("DM140VFD: scaning hiddev port: %s", port);
        if((fd = ::open(port,O_WRONLY))>=0)
        {
            struct hiddev_devinfo device_info;
            ioctl(fd, HIDIOCGDEVINFO, &device_info);
            //dsyslog("DM140VFD: detected hiddev device: 0x%x:0x%x", device_info.vendor, device_info.product);

            // If we've found our device, no need to look further, time to stop searching
            if(vendor==(device_info.vendor&0xFFFF) && product==(device_info.product&0xFFFF))
            {
                //char name[256];
                //ioctl(fd, HIDIOCGNAME(sizeof(name)), name);
                //name[sizeof(name)-1]='\0';
                //syslog(LOG_INFO, "%s: The device %s was opened successfully.\n", config->name.c_str(), name);

                break; // stop the for loop
            }
            ::close(fd); // Added by HL
            fd=-1;
        }
    }

    if (-1 == fd)
    {
        esyslog("DM140VFD: Cannot open device 0x%.4x:0x%.4x.\n", vendor, product);
        return false;
    }
    dsyslog("DM140VFD: open Device 0x%.4x:0x%.4x successful", vendor, product);

    //******************************************************
    // Initialize the internal report structures
    //******************************************************
    if(ioctl(fd, HIDIOCINITREPORT,0)<0)
    {
        syslog(LOG_ERR, "DM140VFD: cannot init device.\n");
        return false;
    }

    //******************************************************
    // Set up the display to show graphics
    //******************************************************
    const char panelCmd[]  = {0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
    const char iconCmd[]   = {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //icon command
    const char iconoff[]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //icon data
    SendReport(panelCmd, sizeof(panelCmd));
    SendReport(iconCmd, sizeof(iconCmd));
    SendReport(iconoff, sizeof(iconoff));

    const char setup[] = {0x01, 0x05, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00};
    SendReport(setup, sizeof(setup));

    screensize = width * height / 8;

    dsyslog("DM140VFD: display initialized.\n");

  /* Make sure the frame buffer is there... */
  this->framebuf = new cVFDBitmap(theSetup.m_cWidth,theSetup.m_cHeight);
  if (this->framebuf == NULL) {
  	esyslog("DM140VFD: unable to allocate framebuffer");
        ::close(fd);
        fd=-1;
  	return false;
  }
  m_iSizeYb = ((theSetup.m_cHeight + 7) / 8);

  /* Make sure the framebuffer backing store is there... */
  this->backingstore = new unsigned char[theSetup.m_cWidth * m_iSizeYb];
  if (this->backingstore == NULL) {
  	esyslog("DM140VFD: unable to create framebuffer backing store");
        ::close(fd);
        fd=-1;
  	return false;
  }

  this->lastIconState = 0;

  dsyslog("DM140VFD: init() done");
  return true;
}

/*
 * turning display off
 */
bool cVFD::SendCmdShutdown() {
    dsyslog("DM140VFD: shutdown command");
    if(-1 == fd) {
       return true;
    }
    icons(0);
    clear();
    flush();

    const char panelCmd[]  = {0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
    const char iconCmd[]   = {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //icon command
    const char iconoff[]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //icon data

    SendReport(panelCmd, sizeof(panelCmd));
    SendReport(iconCmd, sizeof(iconCmd));
    SendReport(iconoff, sizeof(iconoff));

    const char setup[] = {0x01, 0x05, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00};

    SendReport(setup, sizeof(setup));
   
    return true;
}


int cVFD::SendReport(const char *cbuf, size_t size)
{
    const unsigned char *buf=reinterpret_cast<const unsigned char *>(cbuf);
    struct hiddev_report_info rinfo;
    struct hiddev_usage_ref uref;
    int err;

    //******************************************************
    // Initialize the usage Reference and mark it for OUTPUT
    //******************************************************
    memset(&uref, 0, sizeof(uref));
    uref.report_type = HID_REPORT_TYPE_OUTPUT;
    uref.report_id = 0;
    uref.field_index = 0;

    //**************************************************************
    // Fill in the information that we wish to set
    //**************************************************************
    uref.usage_code  = 0xffa10005; //unused?
    for(size_t i=0;i<size;i++)
    {
        uref.usage_index = i;
        uref.value = VALUE_FILTER(buf[i]);

        //**************************************************************
        // HIDIOCSUSAGE - struct hiddev_usage_ref (write)
        // Sets the value of a usage in an output report.  The user fills
        // in the hiddev_usage_ref structure as above, but additionally
        // fills in the value field.
        //**************************************************************
        if((err = ioctl(fd, HIDIOCSUSAGE, &uref)) < 0)
        {
                syslog(LOG_INFO, "DM140VFD: Error with sending the USAGE ioctl %d,0x%02X;size:%d\n", i, (int)buf[i],size);
                return err;
        }
        uref.usage_code = 0xffa10006; //unused?
    }

    //**************************************************************
    // HIDIOCSREPORT - struct hiddev_report_info (write)
    // Instructs the kernel to SEND a report to the device.  This
    // report can be filled in by the user through HIDIOCSUSAGE calls
    // (below) to fill in individual usage values in the report before
    // sending the report in full to the device.
    //**************************************************************
    memset(&rinfo, 0, sizeof(rinfo));
    rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
    rinfo.report_id = 0;
    rinfo.num_fields = 0;
    if((err = ioctl(fd, HIDIOCSREPORT, &rinfo)) < 0)
    {
        syslog(LOG_INFO, "DM140VFD: Error with sending the REPORT ioctl %d\n", err);
    }

    //******************************************************
    // All done, let's return what we did.
    //******************************************************
    return err;
}

// was used for clock
//inline byte toBCD(int x){ 
// return (byte)(((x) / 10 * 16) + ((x) % 10));
//}
//
///*
// * DM140 has no clock.
// */
//bool cVFD::SendCmdClock() {
//  return true;
//}

/**
 * Close the driver (do necessary clean-up).
 */
void cVFD::close()
{
//  icons(0);
//  clear();
  ::close(fd);

  if(pFont) {
    delete pFont;
    pFont = NULL;
  }
  if(framebuf) {
    delete framebuf;
    framebuf = NULL;
  }
  if(backingstore) {
    delete backingstore;
    backingstore = NULL;
  }
  dsyslog("DM140VFD: close() done");
}


/**
 * Clear the screen.
 */
void cVFD::clear()
{
  if(framebuf)
    framebuf->clear();
}


/**
 * Flush cached bitmap data and submit changes rows to the Display.
 */

bool cVFD::flush(bool refreshAll)
{
  unsigned int n, x, yb;

  if (!backingstore || !framebuf)
      return false;

  const uchar* fb = framebuf->getBitmap();
  const unsigned int width = framebuf->Width();

  bool doRefresh = false;
  unsigned int minX = width;
  unsigned int maxX = 0;

  for (yb = 0; yb < m_iSizeYb; ++yb)
      for (x = 0; x < width; ++x)
      {
          n = x + (yb * width);
          if (*(fb + n) != *(backingstore + n))
          {
              *(backingstore + n) = *(fb + n);
              minX = min(minX, x);
              maxX = max(maxX, x + 1);
              doRefresh = true;
          }
      }

  if (refreshAll || doRefresh) {
    char packet[] = {0x1D, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};

    SendReport(packet, sizeof(packet));

    // Send the actual graphics
    for(int i=0; i< screensize;i+=8)
    {
        // make sure we only send 8 bytes at a time
        int size = (screensize - i);
        size = (size > 8) ? 8 : size;
        SendReport((char *)(backingstore+i), size);
    }

    const char show[] = {0x01, 0x05, 0x03, 0x03,  0x00, 0x00, 0x00, 0x00};

    SendReport(show, sizeof(show));
  }
  return true;
}

/**
 * Print a string on the screen at position (x,y).
 * The upper-left corner is (1,1), the lower-right corner is (this->width, this->height).
 * \param x        Horizontal character position (column).
 * \param y        Vertical character position (row).
 * \param string   String that gets written.
 */
int cVFD::DrawText(int x, int y, const char* string)
{
  if(pFont && framebuf)
    return pFont->DrawText(framebuf, x, y, string, 1024);
  return -1;
}

/**
 * Height of framebuffer from current device.
 */
int cVFD::Height() const
{
  if(framebuf)
    return framebuf->Height();
  return 0;
}

/**
 * Width of framebuffer from current device.
 */
int cVFD::Width() const
{
  if(framebuf)
    return framebuf->Width();
  return 0;
}

/**
 * Draw a rectangle on framebuffer on device. 
 *
 * \param x1       First horizontal corner (column).
 * \param y1       First vertical corner (row).
 * \param x2       Second horizontal corner (column).
 * \param y2       Second vertical corner (row).
 * \param filled   drawing of rectangle should be filled.
 */
bool cVFD::Rectangle(int x1, int y1, int x2, int y2, bool filled)
{
  if(framebuf)
    return framebuf->Rectangle(x1, y1, x2, y2, filled);
  return false;
}


/**
 * Sets the "icons state" for the device. We use this to control the icons
   * around the outside the display. 
 *
 * \param state    This symbols to display.
 */
void cVFD::icons(unsigned int state)
{
  unsigned char bitmap[8];
  const unsigned char panelCmd[]  = {0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
  const unsigned char iconCmd[]   = {0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  dsyslog("DM140VFD: set icon state: 0x%x", state);

  lastIconState = state;

  memset(bitmap, 0, sizeof(bitmap));
  for(unsigned i=0; i < sizeof(icon_bits); i++)
  {
    if ((state & (1 << i)) != 0 && icon_bits[i] != -1) {
       bitmap[icon_bits[i]/8] |= 1 << ((icon_bits[i]) % 8);
       //dsyslog("DM140VFD: setting bit %d, Bitmap[%d]=0x%02x",icon_bits[i],icon_bits[i]/8,bitmap[icon_bits[i]/8]);
    }
  }

  SendReport( (char *)panelCmd, sizeof(panelCmd));
  SendReport( (char *)iconCmd, sizeof(iconCmd));
  SendReport( (char *)bitmap, sizeof(bitmap));
}

/**
 * Sets the brightness of the display.
 * 
 * haven't yet found out how to control the brightness
 *
 * \param nBrightness The value the brightness (0 = off
 *                  1 = half brightness; 2 = highest brightness).
 */
//void cVFD::Brightness(int nBrightness)
//{
//   return;
//}

bool cVFD::SetFont(const char *szFont, bool bTwoLineMode, int nBigFontHeight, int nSmallFontHeight) {

  cVFDFont* tmpFont = NULL;

  cString sFileName = cFont::GetFontFileName(szFont);
  if(!isempty(sFileName))
  {
    if (bTwoLineMode) {
      tmpFont = new cVFDFont(sFileName,nSmallFontHeight);
    } else {
      tmpFont = new cVFDFont(sFileName,nBigFontHeight);
    }
  } else {
		esyslog("DM140VFD: unable to find font '%s'",szFont);
  }
  if(tmpFont) {
    if(pFont) {
      delete pFont;
    }
    pFont = tmpFont;
    return true;
  }
  return false;
}
