/** \file    serdisp_specific_dpfax.c
  *
  * \brief   Routines for controlling DPF-AX based display modules with modified firmware
  * \date    (C) 2017
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
  */

/*
 *************************************************************************
 * based on superelchi's dpf-ax code:
 * 
 *   https://sourceforge.net/projects/dpf-ax/
 *   -> dpflib/dpfcore4driver/dpfcore4driver.[ch]
 *
 * 
 * documentation on dpf-ax based modules with modified firmware:
 *   https://sourceforge.net/p/dpf-ax/code/HEAD/tree/trunk/README
 *
 *************************************************************************
 */

/*
 *************************************************************************
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or (at  
 * your option) any later version.                                        
 *                                                                        
 * This program is distributed in the hope that it will be useful, but    
 * WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 * General Public License for more details.                               
 *                                                                        
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA              
 * 02111-1307, USA.  Or, point your browser to                            
 * http://www.gnu.org/copyleft/gpl.html                                   
 *************************************************************************
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <errno.h>
#include "../config.h"

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"
#include "serdisplib/serdisp_connect_usb.h"

#include "serdisplib/serdisp_fctptr.h"


/* #define OPT_USEOLDUPDATEALGO */

/* different display types/models supported by this driver */
#define DISPID_DPFAX        1

serdisp_options_t serdisp_dpfax_options[] = {
   /*  name          aliasnames    min  max mod int defines  */
   {  "BACKLIGHT",             "",  0,   1,  1,  1,  "ON=1,OFF=0,YES=1,NO=0"}  /* backlight on/off */
  ,{  "BACKLIGHTLEVEL", "BGLEVEL",  0,   7,  1,  1,  ""}                       /* backlight level */
  ,{  "BRIGHTNESS",            "",  0, 100,  1,  1,  ""}                       /* brightness [0 .. 100] */
};


/* some defines */
#define     DPFAX_DIR_IN                 0
#define     DPFAX_DIR_OUT                1
#define     DPFAX_CMD_SIZE               16  /* SCSI cmd length */
#define     DPFAX_CMD_STARTIDX           15  /* SCSI cmd: start index in command buffer */
#define     DPFAX_ACK_SIZE               13


/* internal typedefs and functions */
static void serdisp_dpfax_init           (serdisp_t*);
static int  serdisp_dpfax_setoption      (serdisp_t*, const char*, long);
static void serdisp_dpfax_close          (serdisp_t*);

static void serdisp_dpfax_update         (serdisp_t*);

static void serdisp_dpfax_clear          (serdisp_t*);

static int  serdisp_dpfax_freeresources  (serdisp_t*);

static int  dpfax_process_data           (serdisp_t*, serdisp_usbdev_t*, byte, byte*, uint32_t);


/* initial buffer sequence (31 byte) for command buffer */
static byte default_buf[] = {
  0x55, 0x53, 0x42, 0x43, /* dCBWSignature */
  0xde, 0xad, 0xbe, 0xef, /* dCBWTag */
  0x00, 0x80, 0x00, 0x00, /* dCBWLength */
  0x00, /* bmCBWFlags: 0x80: data in (dev to host), 0x00: Data out */
  0x00, /* bCBWLUN */
  0x10, /* bCBWCBLength */

  /* SCSI cmd, template */
  0xcd, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0,
};


typedef struct serdisp_dpfax_specific_s {
  byte  buffer[sizeof(default_buf)];  /* send buffer */
  byte  ack[DPFAX_ACK_SIZE];          /* acknowledge buffer */
  byte  cmd[DPFAX_CMD_SIZE];          /* command buffer */
  byte* transfer;                     /* transfer lcd buffer */
  byte  bglevel;                      /* backlight level */
} serdisp_dpfax_specific_t;


static serdisp_dpfax_specific_t* serdisp_dpfax_internal_getStruct(serdisp_t* dd) {
  return (serdisp_dpfax_specific_t*)(dd->specific_data);
}


/* callback-function for memory-deallocations not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_dpfax_freeresources (serdisp_t* dd) {
  if (dd->specific_data) {
    if (serdisp_dpfax_internal_getStruct(dd)->transfer) {
      free(serdisp_dpfax_internal_getStruct(dd)->transfer);
    }
  }
  return 0;
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_dpfax_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (if null: used for querying display module parameters)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_dpfax_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t*                dd = 0;
  serdisp_usbdev_t*         usbitems = 0;
  int                       dsp_id = 0;

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBUSB) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libusb is not loaded but is a requirement for serdisp_specific_dpfax.c.", __func__);
    return (serdisp_t*)0;
  }

  if(sdcd) {
    /* maybe null in case of wirequery ( when called through serdisp_getdisplaydescription() ) */
    usbitems = (serdisp_usbdev_t*)(sdcd->extra);
  }

  /* check dispname and get dsp_id (paranoia check) */
  if (serdisp_comparedispnames("DPFAX", dispname))
    dsp_id = DISPID_DPFAX;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_dpfax.c", dispname);
    return (serdisp_t*)0;
  }

  /* allocate descriptor */
  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));
  dd->dsp_id = dsp_id;


  /* get display parameters from display module */
  if(usbitems) {
    byte buf[5];

    /* specific data for DPFAX-modules */
    if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_dpfax_specific_t)) )) {
      serdisp_freeresources(dd);
      return (serdisp_t*)0;
    }
    memset(dd->specific_data, 0, sizeof(serdisp_dpfax_specific_t));

    /* initialise buffer with default sequence */
    memcpy( serdisp_dpfax_internal_getStruct(dd)->buffer, default_buf, sizeof(default_buf));

    /* initialise SCSI cmd */
    memcpy( serdisp_dpfax_internal_getStruct(dd)->cmd, &(default_buf[DPFAX_CMD_STARTIDX]), DPFAX_CMD_SIZE);

    serdisp_dpfax_internal_getStruct(dd)->cmd[5] = 2; /* get LCD parameters */

    if (dpfax_process_data(dd, usbitems, DPFAX_DIR_IN, buf, 5) == 0) {
      dd->width = (buf[0]) | (buf[1] << 8);
      dd->height = (buf[2]) | (buf[3] << 8);
      sd_debug(2, "%s(): got LCD dimensions: %dx%d", __func__, dd->width, dd->height);
    } else {
      sd_error(SERDISP_ERUNTIME, "%s(): error reading LCD resolution", __func__);
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t *)0;
    }

    /* transfer buffer */
    if (! (serdisp_dpfax_internal_getStruct(dd)->transfer = (void*) sdtools_malloc( (dd->width * dd->height) << 1) )) {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate transfer buffer", __func__);
      serdisp_freeresources(dd);
      return (serdisp_t*)0;
    }
  }

  /* other display settings */

  dd->depth             = 16;
  dd->feature_backlight = 1;
  dd->feature_invert    = 0;

  dd->feature_contrast  = 0;

  /* supported colour space */
  dd->colour_spaces     = SD_CS_RGB565 | SD_CS_SELFEMITTING;

  /* default setup for function pointers */
  dd->fp_init           = &serdisp_dpfax_init;
  dd->fp_update         = &serdisp_dpfax_update;
  dd->fp_close          = &serdisp_dpfax_close;
  dd->fp_setoption      = &serdisp_dpfax_setoption;
  dd->fp_clear          = &serdisp_dpfax_clear;

  dd->fp_freeresources  = &serdisp_dpfax_freeresources;

  /* using bounding box, so maxdelta not needed */
  dd->optalgo_maxdelta    = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate         = 0;         /* unrotated display */
  dd->curr_invert         = 0;         /* display not inverted */

  /* supported output devices (dummy) */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  serdisp_setupstructinfos(dd, 0, 0, serdisp_dpfax_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }

  return dd;
}



/* *********************************
   void serdisp_dpfax_init(dd)
   *********************************
   initialise a dpf-ax based display module
   *********************************
   dd     ... display descriptor
*/
void serdisp_dpfax_init(serdisp_t* dd) {
  serdisp_dpfax_internal_getStruct(dd)->bglevel = 7;
  dd->curr_backlight = 1;
  //serdisp_dpfax_setoption(dd, "BACKLIGHT", 1);
  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_dpfax_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg (colour display module)
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:
*/
void serdisp_dpfax_update(serdisp_t* dd) {
  byte* transfer = serdisp_dpfax_internal_getStruct(dd)->transfer;
  byte* cmd = serdisp_dpfax_internal_getStruct(dd)->cmd;

  /* initialise SCSI cmd */
  memcpy( cmd, &default_buf[DPFAX_CMD_STARTIDX], DPFAX_CMD_SIZE);

#ifdef OPT_USEOLDUPDATEALGO
  uint32_t len = ( dd->width * dd->height ) << 1;
  cmd[5] = 6;
  cmd[6] = 0x12; /*USBCMD_BLIT */
  cmd[7] = 0;
  cmd[8] = 0;
  cmd[9] = 0;
  cmd[10] = 0;
  cmd[11] = dd->width - 1 ;  //rect[2] - 1;
  cmd[12] = (dd->width - 1) >> 8; //(rect[2] - 1) >> 8;
  cmd[13] = dd->height - 1; //rect[3] - 1;
  cmd[14] = (dd->height - 1) >> 8; //(rect[3] - 1) >> 8;
  cmd[15] = 0;

  if (dd->curr_invert) {
    uint32_t pos = 0;
    for (pos = 0; pos < dd->scrbuf_size; pos++) {
      transfer[pos] = ~ dd->scrbuf[pos];
    }
    dpfax_process_data(dd, (serdisp_usbdev_t*)(dd->sdcd->extra), DPFAX_DIR_OUT, transfer, len);
  } else {
    dpfax_process_data(dd, (serdisp_usbdev_t*)(dd->sdcd->extra), DPFAX_DIR_OUT, dd->scrbuf, len);
  }
#else  /* ! OPT_USEOLDUPDATEALGO */
  if (dd->bbox_dirty) {
    uint32_t len = (( dd->bbox[2] - dd->bbox[0] + 1) * ( dd->bbox[3] - dd->bbox[1] + 1)) << 1;
    uint16_t j;
    uint32_t pdest = 0;
    uint32_t psource;

    cmd[5] = 6;
    cmd[6] = 0x12; /*USBCMD_BLIT */
    cmd[7] = dd->bbox[0];
    cmd[8] = dd->bbox[0] >> 8;
    cmd[9] = dd->bbox[1];
    cmd[10] = dd->bbox[1] >> 8;
    cmd[11] = dd->bbox[2];
    cmd[12] = dd->bbox[2] >> 8;
    cmd[13] = dd->bbox[3];
    cmd[14] = dd->bbox[3] >> 8;
    cmd[15] = 0;

    /* no need to clean transfer buffer before writing data into it */
    if (dd->curr_invert) {
      uint16_t i;

      for (j = dd->bbox[1]; j <= dd->bbox[3]; j++) {
        psource = j * dd->width;
        for(i = dd->bbox[0]; i <= dd->bbox[2]; i++) {
          transfer[pdest++] = ~ (dd->scrbuf[ (psource + i ) << 1 ]);
          transfer[pdest++] = ~ (dd->scrbuf[ ((psource + i ) << 1) + 1 ]);
        }
      }
    } else {
      uint32_t plen = (dd->bbox[2] - dd->bbox[0] + 1) << 1;

      for (j = dd->bbox[1]; j <= dd->bbox[3]; j++) {
        psource = ( j * dd->width + dd->bbox[0] ) << 1;
        memcpy(&(transfer[pdest]), &(dd->scrbuf[psource]), plen);
        pdest += plen;
      }
    }
    dpfax_process_data(dd, (serdisp_usbdev_t*)(dd->sdcd->extra), DPFAX_DIR_OUT, transfer, len);
  }
#endif /* OPT_USEOLDUPDATEALGO */
}



/* *********************************
   int serdisp_dpfax_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_dpfax_setoption(serdisp_t* dd, const char* option, long value) {
  byte* cmd = serdisp_dpfax_internal_getStruct(dd)->cmd;

  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;

    /* initialise SCSI cmd */
    memcpy( cmd, &(default_buf[DPFAX_CMD_STARTIDX]), DPFAX_CMD_SIZE);

    /* if bglevel = 0 and BACKLIGHT = on -> set bglevel to at least 1 */
    if (dd->curr_backlight && serdisp_dpfax_internal_getStruct(dd)->bglevel == 0) {
      serdisp_dpfax_internal_getStruct(dd)->bglevel = 1;
      dd->curr_dimming = 100 - ( serdisp_dpfax_internal_getStruct(dd)->bglevel * 100) / 7;
    }

    cmd[5] = 6;
    cmd[6] = 0x01;  /* USBCMD_SETPROPERTY */
    cmd[7] = 0x01;  /* PROPERTY_BRIGHTNESS */
    cmd[8] = 0x00;  /* PROPERTY_BRIGHTNESS >> 8 */
    cmd[9] = (dd->curr_backlight) ? serdisp_dpfax_internal_getStruct(dd)->bglevel : 0;
    cmd[10] = 0; /* value >> 8 */

    dpfax_process_data(dd, (serdisp_usbdev_t*)(dd->sdcd->extra), DPFAX_DIR_OUT, NULL, 0);
  } else if(serdisp_compareoptionnames(dd, option, "BACKLIGHTLEVEL") ||
            serdisp_compareoptionnames(dd, option, "BRIGHTNESS"))
  {
    byte curr_bgvalue = 0;

    if (serdisp_compareoptionnames(dd, option, "BACKLIGHTLEVEL")) {
      serdisp_dpfax_internal_getStruct(dd)->bglevel = (value > 7 || value < 0) ? 0x7 : (byte)value;
      dd->curr_dimming = 100 - ( serdisp_dpfax_internal_getStruct(dd)->bglevel * 100) / 7;

      curr_bgvalue = (dd->curr_backlight == 0) ? 0 : serdisp_dpfax_internal_getStruct(dd)->bglevel;
    } else {  /* BRIGHTNESS */
      if (value < 0)
        value = 0;
      if (value > 100)
        value = 100;
      dd->curr_dimming = 100 - value;
      serdisp_dpfax_internal_getStruct(dd)->bglevel = (value * 7) / 100;
      curr_bgvalue = (dd->curr_backlight == 0) ? 0 : serdisp_dpfax_internal_getStruct(dd)->bglevel;
    }

    cmd[5] = 6;
    cmd[6] = 0x01;  /* USBCMD_SETPROPERTY */
    cmd[7] = 0x01;  /* PROPERTY_BRIGHTNESS */
    cmd[8] = 0x00;  /* PROPERTY_BRIGHTNESS >> 8 */
    cmd[9] = curr_bgvalue;
    cmd[10] = curr_bgvalue >> 8;

    dpfax_process_data(dd, (serdisp_usbdev_t*)(dd->sdcd->extra), DPFAX_DIR_OUT, NULL, 0);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */

    sd_debug(1, "%s(): option unhandled - using fallback", __func__);
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_dpfax_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_dpfax_clear(serdisp_t* dd) {
  /* wipe screen buffer */
  memset (dd->scrbuf, 0x00, dd->scrbuf_size);

#ifndef OPT_USEOLDUPDATEALGO
  sdtools_init_bbox(dd, 1);
#endif

  serdisp_update(dd);
}


/* *********************************
   void serdisp_dpfax_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_dpfax_close(serdisp_t* dd) {
  serdisp_setoption(dd, "BACKLIGHT", 0);
  serdisp_clear(dd);
}


/* *********************************
   int dpfax_process_data(dd, usbitems, out, data, data_len)
   *********************************
   exchange data with display module
   *********************************
   dd       ... display descriptor
   usbitems ... usb device descriptor
   out      ... direction (in / out)
   data     ... data buffer
   data_len ... data length
   *********************************
   returns return code
*/
int dpfax_process_data(serdisp_t* dd, serdisp_usbdev_t* usbitems, byte out, byte* data, uint32_t data_len) {
  int ret;
  int retry = 0;
  int timeout = 0;

  serdisp_dpfax_internal_getStruct(dd)->buffer[8] =  (data_len & 0xFF);
  serdisp_dpfax_internal_getStruct(dd)->buffer[9] =  (data_len & 0xFF00) >> 8;
  serdisp_dpfax_internal_getStruct(dd)->buffer[10] = (data_len & 0xFF0000) >> 16;
  serdisp_dpfax_internal_getStruct(dd)->buffer[11] = (data_len & 0xFF000000) >> 24;

  serdisp_dpfax_internal_getStruct(dd)->buffer[14] = DPFAX_CMD_SIZE;

  /* update SCSI command sequence in transfer buffer */
  memcpy(&(serdisp_dpfax_internal_getStruct(dd)->buffer[15]), serdisp_dpfax_internal_getStruct(dd)->cmd, (size_t)DPFAX_CMD_SIZE);

  ret = fp_usb_bulk_write(usbitems->usb_dev, usbitems->out_ep, (const char *)(serdisp_dpfax_internal_getStruct(dd)->buffer), sizeof(default_buf), usbitems->write_timeout);
  if (ret < 0) {
    sd_error(SERDISP_ERUNTIME, "%s(): error writing command sequence (rv: %d)", __func__, ret);
    return ret;
  }

  if (out) {
    if (data) {
      ret = fp_usb_bulk_write(usbitems->usb_dev, usbitems->out_ep, (const char* )data, (int)data_len, usbitems->write_timeout * 3);
      if (ret != (int) data_len) {
        sd_error(SERDISP_ERUNTIME, "%s(): error writing data (rv: %d)", __func__, ret);
        return ret;
      }
    }
  } else if (data) {
    ret = fp_usb_bulk_read(usbitems->usb_dev, usbitems->in_ep, (char *) data, data_len, usbitems->read_timeout * 4);
    if (ret != (int) data_len) {
      sd_error(SERDISP_ERUNTIME, "%s(): error reading data (rv: %d)", __func__, ret);
      return ret;
    }
  }
  /* get ACK */
  do {
    timeout = 0;
    ret = fp_usb_bulk_read(usbitems->usb_dev, usbitems->in_ep, (char *) serdisp_dpfax_internal_getStruct(dd)->ack, DPFAX_ACK_SIZE, usbitems->read_timeout * 5);
    if (ret != DPFAX_ACK_SIZE) {
      sd_error(SERDISP_ERUNTIME, "%s(): error reading ACK (rv: %d)", __func__, ret);
      timeout = 1;
    }
    retry++;
  } while (timeout && retry < 5);
  if (strncmp((char *) serdisp_dpfax_internal_getStruct(dd)->ack, "USBS", 4)) {
    sd_error(SERDISP_ERUNTIME, "%s(): error: got invalid ACK reply", __func__);
    return -1;
  }
  /* pass back return code set by peer */
  return serdisp_dpfax_internal_getStruct(dd)->ack[12];
}

