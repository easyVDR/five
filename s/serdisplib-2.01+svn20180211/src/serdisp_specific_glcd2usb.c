/** \file    serdisp_specific_glcd2usb.c
  *
  * \brief   Routines for drawing content to GLCD2USB based devices
  * \date    (C) 2012-2013
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
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

/*
 * includes
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../config.h"

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_fctptr.h"
#include "serdisplib/serdisp_connect_usb.h"

#if (SERDISP_VERSION_CODE > SERDISP_VERSION(1,97))
 #include "serdisplib/serdisp_colour.h"
#endif

#ifdef HAVE_LIBPTHREAD
 #include "serdisplib/serdisp_gpevents.h"
 #include <pthread.h>
 static pthread_mutex_t mutex_comm = PTHREAD_MUTEX_INITIALIZER;
#endif


/* #define OPT_USEOLDUPDATEALGO */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_GLCD2USB   1



/* defines taken from glcd2usb/lcd4linux/glcd2usb.h */
#define FLAG_SIX_BIT          (1<<0)
#define FLAG_VERTICAL_UNITS   (1<<1)
#define FLAG_BOTTOM_START     (1<<2)
#define FLAG_VERTICAL_INC     (1<<3)
#define FLAG_BACKLIGHT        (1<<4)

#define GLCD2USB_RID_GET_INFO      1    /* get display info */
#define GLCD2USB_RID_SET_ALLOC     2    /* allocate/free display */
#define GLCD2USB_RID_GET_BUTTONS   3    /* get state of the four buttons */
#define GLCD2USB_RID_SET_BL        4    /* set backlight brightness */
/*#define GLCD2USB_RID_GET_IR        5*/    /* get last ir message */
#define GLCD2USB_RID_WRITE         8    /* write some bitmap data to the display */
#define GLCD2USB_RID_WRITE_4       (GLCD2USB_RID_WRITE+0)
#define GLCD2USB_RID_WRITE_8       (GLCD2USB_RID_WRITE+1)
#define GLCD2USB_RID_WRITE_16      (GLCD2USB_RID_WRITE+2)
#define GLCD2USB_RID_WRITE_32      (GLCD2USB_RID_WRITE+3)
#define GLCD2USB_RID_WRITE_64      (GLCD2USB_RID_WRITE+4)
#define GLCD2USB_RID_WRITE_128     (GLCD2USB_RID_WRITE+5)

#define USBRQ_HID_GET_REPORT       0x01
#define USBRQ_HID_SET_REPORT       0x09
#define USB_HID_REPORT_TYPE_FEATURE 3

serdisp_options_t serdisp_glcd2usb_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "BACKLIGHT", "",         0,   1,  1, 1,  "ON=1,OFF=0,YES=1,NO=0"}  /* backlight on/off */
  ,{  "BACKLIGHTLEVEL", "BGLEVEL", 0, 0xFF,  1, 1,  ""}                  /* backlight level */
  ,{  "BRIGHTNESS", "",        0, 100,  1, 1,  ""}                       /* brightness [0 .. 100] */
};


#ifdef HAVE_LIBPTHREAD
SDGPI_t serdisp_glcd2usb_GPIs[] = {
   /* id  name         aliasnames   type         enabled  last value */
   {  0, "BUTTONMASK", "BUTTONS",   SDGPT_VALUE, 0,       {0} }
};
#endif


/* 
 * private function prototypes
 */

static void   serdisp_glcd2usb_init                 (serdisp_t *dd);
static void   serdisp_glcd2usb_update               (serdisp_t *dd);
static int    serdisp_glcd2usb_setoption            (serdisp_t *dd, const char *option, long value);
static void   serdisp_glcd2usb_close                (serdisp_t *dd);

static int    serdisp_glcd2usb_bglevel2brightness   (int bglevel);
static int    serdisp_glcd2usb_brightness2bglevel   (int brightness);


static void*  serdisp_glcd2usb_getvalueptr          (serdisp_t *dd, const char *optionname, int *typesize);

static int    serdisp_glcd2usb_setreport            (serdisp_usbdev_t* usbitems, int reportType, byte* buffer, int len);
static int    serdisp_glcd2usb_getreport            (serdisp_usbdev_t* usbitems, int reportType, int reportNumber, byte* buffer, int* len);

#ifdef HAVE_LIBPTHREAD
static int    serdisp_glcd2usb_gpi_enable           (serdisp_t* dd, byte gpid, int enable);
static SDGP_event_t* serdisp_glcd2usb_evlp_receiver (serdisp_t* dd, SDGP_event_t* recycle);
#endif

/*
 * private structures
 */

typedef struct serdisp_glcd2usb_specific_s {
  byte buf[132];          /* transfer buffer */
  byte flag_vertical;     /* 1: 8 vertical pixels = 1 byte, 0: 8 horicontal pixels = 1 byte */
  byte bglevel;           /* backlight level */
} serdisp_glcd2usb_specific_t;


static serdisp_glcd2usb_specific_t* serdisp_glcd2usb_internal_getStruct(serdisp_t* dd) {
  return (serdisp_glcd2usb_specific_t*)(dd->specific_data);
}



/**
  * \brief   set up display descriptor
  *
  * sets up a display descriptor fitting to dispname and extra
  *
  * \param   sdcd           output device handle
  * \param   dispname       display name (case-insensitive)
  * \param   optionstring   option string containing individual options
  *
  * \return  display descriptor or NULL if unsuccessful
  *
  * \since   2.00
  */
serdisp_t* serdisp_glcd2usb_setup(const serdisp_CONN_t *sdcd, const char *dispname, const char *optionstring) {
  serdisp_t* dd = 0;
  serdisp_usbdev_t* usbitems = 0;

  /* info report:
   *  0:      report_id
   *  1 - 32: display name
   * 33 - 34: width (33 = lower byte)
   * 35 - 36: height
   * 37     : flags
   */
  byte display_info[1 + 32 + 2 + 2 + 1];
  
  int flags = 0;  
  int len = 0;

  
  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBUSB) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libusb is not loaded but is a requirement for serdisp_specific_glcd2usb.c.", __func__);
    return (serdisp_t*)0;
  }
  
  if (! sdcd ) {
    sd_error(SERDISP_EMALLOC, "%s(): output device not open", __func__);
    return (serdisp_t*)0;
  }

  
  if (! sdcd->extra ) {
    sd_error(SERDISP_EMALLOC, "%s(): sdcd->extra not available", __func__);
    return (serdisp_t*)0;
  }
  usbitems = (serdisp_usbdev_t*)(sdcd->extra);

  
  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));
  dd->sdcd = (serdisp_CONN_t*)sdcd;


  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_glcd2usb_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_glcd2usb_specific_t));

  /* glcd2usb based display-modules supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("GLCD2USB", dispname))
    dd->dsp_id = DISPID_GLCD2USB;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_glcd2usb.c", dispname);
    return (serdisp_t*)0;
  }


  dd->connection_types  = SERDISPCONNTYPE_PARPORT; /* == USB */

  /* force device-check at this point because _setup already does some communication with the display */
  if (! (dd->connection_types & dd->sdcd->conntype)) {
    sd_error(SERDISP_EDEVNOTSUP, "'%s' only supports 'USB' as connection type (try using 'USB:<device>').", dispname);
    free(dd->specific_data);
    free(dd);
    return (serdisp_t*)0;
  }

  memset(display_info, 0, sizeof(display_info));

  len = sizeof(display_info);
  if ( (serdisp_glcd2usb_getreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, GLCD2USB_RID_GET_INFO, display_info, &len) != 0) || 
       (len != sizeof(display_info))
     )
  {
    sd_error(SERDISP_ERUNTIME, "retrieved erraneous display info for '%s' (len %d != %d).", dispname, len, (int)sizeof(display_info));
    free(dd->specific_data);
    free(dd);
    return (serdisp_t*)0;
  }

  dd->width  = display_info[33] + (display_info[34] << 8);
  dd->height = display_info[35] + (display_info[36] << 8);
  flags = display_info[37];

  /* detect display type */
  dd->depth             = 1;  /* glcd2usb only supports monochrome */
  dd->feature_contrast  = 0;  /* no contrast support */
  dd->feature_invert    = 0;  /* no command for inverting */
  dd->curr_invert       = 0;  /* not yet inverted */
  dd->curr_rotate       = 0;  /* not yet rotated */

  dd->feature_backlight = (flags & FLAG_BACKLIGHT) ? 1 : 0;  /* has backlight */
  dd->curr_backlight    = 1;  /* backlight is on */
  dd->fp_init           = &serdisp_glcd2usb_init;
  dd->fp_update         = &serdisp_glcd2usb_update;
  dd->fp_setoption      = &serdisp_glcd2usb_setoption;
  dd->fp_close          = &serdisp_glcd2usb_close;
  dd->fp_getvalueptr    = &serdisp_glcd2usb_getvalueptr;

  serdisp_glcd2usb_internal_getStruct(dd)->flag_vertical = (flags & FLAG_VERTICAL_UNITS) ? 1 : 0;

  dd->colour_spaces     = SD_CS_SCRBUFCUSTOM | SD_CS_GREYSCALE;
  dd->optalgo_maxdelta  = 6;

  if ( (flags & FLAG_SIX_BIT) || (flags & FLAG_VERTICAL_INC) || !(flags & FLAG_VERTICAL_UNITS) ) {
    sd_error(SERDISP_EDEVNOTSUP, "one or more flags not yet supported by '%s' (flags=%02x).", dispname, flags);
    free(dd->specific_data);
    free(dd);
    return (serdisp_t*)0;
  }

  
  serdisp_setupstructinfos(dd, 0, 0, serdisp_glcd2usb_options);

  /* parse and set options */

  if(serdisp_setupoptions(dd, dispname, optionstring)) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t *)0;
  }

  /* correct bglevel if option 'BRIGHTNESS' was set */
  if (dd->feature_backlight) {
    serdisp_glcd2usb_internal_getStruct(dd)->bglevel = serdisp_glcd2usb_brightness2bglevel(100 - dd->curr_dimming);
  }

  sd_debug(2, "%s(): detected display information: w/h: %d/%d   has_backlight: %d", 
           __func__, dd->width, dd->height, dd->feature_backlight
  );

/* only GPIs required, thus execute the whole gpevent-stuff only if libpthread is available */
#ifdef HAVE_LIBPTHREAD
  /* add gpevset */
  if (! (dd->gpevset = (SDGP_gpevset_t*) sdtools_malloc( sizeof(SDGP_gpevset_t)) )) {
    sd_debug(0, 
      "%s(): cannot allocate memory for general purpose event set. continuing without support for it ...",
      __func__
    );
  }

  if (dd->gpevset) {
    memset(dd->gpevset, 0, sizeof(SDGP_gpevset_t));

    /* add GPIs */
    dd->gpevset->gpis = sdtools_malloc(sizeof(serdisp_glcd2usb_GPIs));
    if (dd->gpevset->gpis) {
      memcpy(dd->gpevset->gpis, serdisp_glcd2usb_GPIs, sizeof(serdisp_glcd2usb_GPIs));
      dd->gpevset->amountgpis = sizeof(serdisp_glcd2usb_GPIs)/sizeof(SDGPI_t);

      /* function pointers */
      dd->gpevset->fp_hnd_gpi_enable =  &serdisp_glcd2usb_gpi_enable;
      dd->gpevset->fp_evlp_receiver =   &serdisp_glcd2usb_evlp_receiver;
    }
    /* no need to autostart event loop if the sole GPI isn't enabled */
    dd->gpevset->evlp_noautostart = 1;
  }
#endif /* HAVE_LIBPTHREAD */

  return(dd);
}

/* *********************************
   void serdisp_glcd2usb_init(dd)
   *********************************
   initialise a glcd2usb based display module
   *********************************
   dd     ... display descriptor
*/
void serdisp_glcd2usb_init(serdisp_t *dd) {
  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  byte buffer[2];
  int err = 0;

  /* get access to display */
  buffer[0] = GLCD2USB_RID_SET_ALLOC;
  buffer[1] = 1;  /* 1 -> alloc display, 0 -> free it */
  if((err = serdisp_glcd2usb_setreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, buffer, 2)) != 0) {
    sd_error(SERDISP_ERUNTIME, "%s(): error when allocating display (GLCD2USB_RID_SET_ALLOC).", __func__);
    sd_runtimeerror = 1;
  } else {
    /* set backlight */
    serdisp_glcd2usb_setoption(dd, "BACKLIGHT", dd->curr_backlight);
  }
}

/* *********************************
   void serdisp_glcd2usb_update(dd)
   *********************************
   updates the display
   *********************************
   dd     ... display descriptor
   *********************************
*/
void serdisp_glcd2usb_update(serdisp_t *dd) {
  int i;
  int pages = (dd->height+7)/8;
  byte data;
  byte* buffer = serdisp_glcd2usb_internal_getStruct(dd)->buf;
  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  int max_i = dd->width * pages;
#ifdef OPT_USEOLDUPDATEALGO
  int start_i;

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return;

  buffer[3] = 0;
  start_i = 0;
  for (i = 0; i <= max_i; i++) {  /* i <= max_i: if i == max_i: send last message but do not set any data */
    if ( (buffer[3] >= 128) || (i == max_i) ) {
      buffer[0] = GLCD2USB_RID_WRITE;
      buffer[1] = start_i % 256;
      buffer[2] = start_i / 256;

#ifdef HAVE_LIBPTHREAD
      if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
        fp_pthread_mutex_lock( &mutex_comm );
#endif
      if (serdisp_glcd2usb_setreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, buffer, 4 + buffer[3]) != 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): error when updating display (GLCD2USB_RID_WRITE).", __func__);
        sd_runtimeerror = 1;
      }
#ifdef HAVE_LIBPTHREAD
      if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
        fp_pthread_mutex_unlock( &mutex_comm );
#endif
      buffer[3] = 0;
      start_i = i;
    }

    if (i < max_i) {
      data = dd->scrbuf[ i ];
      if (dd->curr_invert)
        data = ~data;
      buffer[ 4 + buffer[3] ] = data;
      buffer[3]++ ;
    }
  }

#else /* ! OPT_USEOLDUPDATEALGO */
  int x = 0, page = 0;

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return;

  i = 0;
  while (i < max_i) {  /* i <= max_i: if i == max_i: send last message but do not set any data */    
    int end_i, end_ii;
    int dirtysize = 0;

    x = i % dd->width;
    page = i / dd->width;
    /* look for 1st changed 8 pixel screen byte */
    while ( (i < max_i) && ! ( dd->scrbuf_chg [ x + dd->width * (page/8) ] & (1 << (page % 8) ) ) ) {
      i ++;

      x = i % dd->width;
      page = i / dd->width;
    }

    end_i = i + 1;
    end_ii = i;

    if (i < max_i) {
      int delta = 0;
      int chunks, chunk = 0;
      int dirty = 0;

      x = end_i % dd->width;
      page = end_i / dd->width;

      /* how many bytes are dirty? */
      dirty = (dd->scrbuf_chg [ x + dd->width * (page/8) ] & (1 << (page % 8))) ? 1 : 0;
      while ( (end_i < max_i) && ( (delta < dd->optalgo_maxdelta) || dirty ))  {
        if (dirty) {
          delta = 0;
          end_ii = end_i;
          /*dd->scrbuf_chg [ x + dd->width * (page/8) ] &= 0xFF - (1 << (page%8));*/
        } else {
          delta ++;
        }

        end_i ++;
        x = end_i % dd->width;
        page = end_i / dd->width;
        dirty = (dd->scrbuf_chg [ x + dd->width * (page/8) ] & (1 << (page % 8))) ? 1 : 0;
      }
      
      dirtysize = end_ii - i + 1;
      chunks = (dirtysize + 127) / 128;

      for (chunk = 0; chunk < chunks; chunk ++) {
        int j = chunk * 128 + i;
        buffer[0] = GLCD2USB_RID_WRITE;
        buffer[1] = j % 256;
        buffer[2] = j / 256;
        buffer[3] = 0;

        while (buffer[3] < 128 && j <= end_ii) {
          data = dd->scrbuf[ j ];
          if (dd->curr_invert)
            data = ~data;
          buffer[ 4 + buffer[3] ] = data;
          buffer[3] ++;
          j++;
        }

#ifdef HAVE_LIBPTHREAD
        if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
          fp_pthread_mutex_lock( &mutex_comm );
#endif
        if (serdisp_glcd2usb_setreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, buffer, 4 + buffer[3]) != 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): error when updating display (GLCD2USB_RID_WRITE).", __func__);
          sd_runtimeerror = 1;
        }
#ifdef HAVE_LIBPTHREAD
        if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
          fp_pthread_mutex_unlock( &mutex_comm );
#endif
      }
      i = end_i + 1;
    }
  }
  memset(dd->scrbuf_chg, 0, dd->scrbuf_chg_size);
#endif  /* OPT_USEOLDUPDATEALGO */
}


/* *********************************
   int serdisp_glcd2usb_setoption(dd, optionname, value)
   *********************************
   change a display option
   *********************************
   dd          ... display descriptor
   optionname  ... name of option to change
   value       ... value for option
   *********************************
*/
int serdisp_glcd2usb_setoption(serdisp_t *dd, const char *optionname, long value) {
  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  byte buffer[2];

  if (dd->feature_backlight && serdisp_compareoptionnames(dd, optionname, "BACKLIGHT")) {    
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;

#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );
#endif

    /* get access to display */
    buffer[0] = GLCD2USB_RID_SET_BL;
    buffer[1] = (dd->curr_backlight) ? serdisp_glcd2usb_internal_getStruct(dd)->bglevel : 0;
    serdisp_glcd2usb_setreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, buffer, 2); /* ignore errors */
    
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
#endif
  } else if(serdisp_compareoptionnames(dd, optionname, "BACKLIGHTLEVEL") ||
            serdisp_compareoptionnames(dd, optionname, "BRIGHTNESS"))
  {
    byte curr_bgvalue = 0;

    /* DISPID_SDCMEGTRON only! */
    if (serdisp_compareoptionnames(dd, optionname, "BACKLIGHTLEVEL")) {
      serdisp_glcd2usb_internal_getStruct(dd)->bglevel = (value > 255 || value < 0) ? 0xFF : (byte)value;
      dd->curr_dimming = 100 - serdisp_glcd2usb_bglevel2brightness(serdisp_glcd2usb_internal_getStruct(dd)->bglevel);

      curr_bgvalue = (dd->curr_backlight == 0) ? 0 : serdisp_glcd2usb_internal_getStruct(dd)->bglevel;
    } else {  /* BRIGHTNESS */
      if (value < 0)
        value = 0;
      if (value > 100)
        value = 100;
      dd->curr_dimming = 100 - value;
      serdisp_glcd2usb_internal_getStruct(dd)->bglevel = serdisp_glcd2usb_brightness2bglevel(value);
      curr_bgvalue = (dd->curr_backlight == 0) ? 0 : serdisp_glcd2usb_internal_getStruct(dd)->bglevel;
    }

#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );
#endif

    /* get access to display */
    buffer[0] = GLCD2USB_RID_SET_BL;
    buffer[1] = curr_bgvalue;
    serdisp_glcd2usb_setreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, buffer, 2); /* ignore errors */
    
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
#endif
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */

    sd_debug(1, "%s(): option unhandled - using fallback", __func__);
    return 0;
  }
  return 1;
}


/* *********************************
   int serdisp_glcd2usb_getvalueptr(dd, optionname, typesize)
   *********************************
   get a display option
   *********************************
   dd          ... display descriptor
   optionname  ... name of option to change
   typesize    ... pointer to typesize storage
*/
void *serdisp_glcd2usb_getvalueptr(serdisp_t *dd, const char *optionname, int *typesize) {
  if(serdisp_compareoptionnames(dd, optionname, "BACKLIGHTLEVEL")) {
    *typesize = sizeof(byte);
    return &(serdisp_glcd2usb_internal_getStruct(dd)->bglevel);
  }
  return 0;
}

/* *********************************
   void serdisp_glcd2usb_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_glcd2usb_close(serdisp_t *dd) {
  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  byte buffer[2];

  /* switch off background light */
  if (dd->feature_backlight)
    serdisp_glcd2usb_setoption(dd, "BACKLIGHT", 0);

  /* get access to display */
  buffer[0] = GLCD2USB_RID_SET_ALLOC;
  buffer[1] = 0;  /* 1 -> alloc display, 0 -> free it */
  serdisp_glcd2usb_setreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, buffer, 2); /* ignore errors */
}



int serdisp_glcd2usb_setreport(serdisp_usbdev_t*  usbitems, int reportType, byte* buffer, int len) {
  int bytesSent;

  bytesSent = fp_usb_control_msg(
    usbitems->usb_dev,
    USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_ENDPOINT_OUT,
    USBRQ_HID_SET_REPORT,
    reportType << 8 | buffer[0],
    0, (char*)buffer, len, usbitems->write_timeout
  );
  if(bytesSent != len) {
    return -1;
  }
  return 0;
}


int serdisp_glcd2usb_getreport(serdisp_usbdev_t*  usbitems, int reportType, int reportNumber, byte* buffer, int* len) {
  int bytesReceived = 0;
  int maxLen = *len;

  bytesReceived = fp_usb_control_msg(
    usbitems->usb_dev,
    USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_ENDPOINT_IN,
    USBRQ_HID_GET_REPORT,
    reportType << 8 | reportNumber,
    0, (char*)buffer, maxLen, usbitems->read_timeout
  );
  if(bytesReceived < 0){
    return -1;
  }
  *len = bytesReceived;
  return 0;
}

int serdisp_glcd2usb_bglevel2brightness (int bglevel) {
  /* bglevel: [0 - 255]
     brightness: [0 - 100]
   */
  return (bglevel * 100 ) / 255;
}

int serdisp_glcd2usb_brightness2bglevel (int brightness) {
  return (brightness * 255) / 100;
}


#ifdef HAVE_LIBPTHREAD
/* *********************************
   int serdisp_glcd2usb_gpi_enable(dd, gpid, enable)
   *********************************
   enable/disable a GPI
   *********************************
   dd     ... display descriptor
   gpid   ... GPI id
   enable ... 1 .. enable GPI, 0 .. disable GPI
   *********************************
   returns:
    0  ... successful
   <0  ... error
*/
int serdisp_glcd2usb_gpi_enable (serdisp_t* dd, byte gpid, int enable) {
  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);

  if (!dd || !dd->gpevset || gpid >= dd->gpevset->amountgpis)
    return -1;

  if (enable && SDEVLP_getstatus(dd) == SDEVLP_STOPPED) {
    byte buffer[2];
    int len = 2;

    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );

    /* flush button events that are already in the queue */
    serdisp_glcd2usb_getreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, GLCD2USB_RID_GET_BUTTONS, buffer, &len);

    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
  
    SDEVLP_start(dd);
  }

  dd->gpevset->gpis[gpid].enabled = enable;

  if (!enable && SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    /* if mutex is still locked: unlock it */
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
    SDEVLP_stop(dd);
  }

  return 0;
}


/* *********************************
   SDGP_event_t* serdisp_glcd2usb_evlp_receiver(dd, recycle)
   *********************************
   reads next event in line
   *********************************
   dd      ... display descriptor
   recylce ... reusable event. if == 0:  space for event is allocated and has to be freed afterwards!
   *********************************
   returns:
    0  ... no event or error
    else: event
*/
SDGP_event_t* serdisp_glcd2usb_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle) {
  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  SDGP_event_t*  event = 0;
  int len = 2;
  byte buffer[2];

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return 0;

  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );

  serdisp_glcd2usb_getreport(usbitems, USB_HID_REPORT_TYPE_FEATURE, GLCD2USB_RID_GET_BUTTONS, buffer, &len);

  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
  
  if (buffer[0] == GLCD2USB_RID_GET_BUTTONS && len == 2 && buffer[1] != (byte)(dd->gpevset->gpis[0].value) ) {
    /* if no recycle-event struct: create new one */  
    if (!recycle) {
      event = sdtools_malloc(sizeof(SDGP_event_t));
      if (!event) {
        sd_error(SERDISP_EMALLOC, "%s(): cannot allocate memory for event", __func__);
        return 0;
      }
    } else {
      event = recycle;
    }

    memset(event, 0, sizeof(SDGP_event_t));

    event->cmdid = SD_CMD_SEND_GPI;
    event->devid = 0;
    event->subid = 0; /* gpid */
    event->type = SDGPT_VALUE;
    gettimeofday(&event->timestamp, 0);
    event->value = (int32_t) buffer[1];
    
    dd->gpevset->gpis[0].value = (int32_t)buffer[1];
    
#if 0
      /* convert header and payload to network byte order */
      SDGPT_event_header_hton(event);
      SDGPT_event_payload_hton(stream, sizeof(stream), (byte)sizeof(int16_t));


      /* convert header and payload back to host byte order */
      SDGPT_event_header_ntoh(event);
      SDGPT_event_payload_ntoh(stream, sizeof(stream), (byte)sizeof(int16_t));
#endif
    return event;
  }
  usleep(50000);
  return 0;
}
#endif /* HAVE_LIBPTHREAD */

