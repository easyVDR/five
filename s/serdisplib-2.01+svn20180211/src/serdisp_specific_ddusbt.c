/*
 *************************************************************************
 *
 * serdisp_specific_ddusbt.c
 * routines for controlling USB displays modules with GPIs (eg. touch screen) made by digital devices
 *
 * supported:
 * - Linux4Media L4M-2.8T320LCD USB LCD 320 Touch  320x240x16 w/ touch screen
 *
 *************************************************************************
 *
 * copyright (C)  2010-2014  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 * additional improvements:
 * markus bauernfeind, fast-lta:
 * + multi-packet frames for speeding up usb-traffic
 *
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

#include <time.h>
#include <errno.h>

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"
#include "serdisplib/serdisp_connect_usb.h"

#include "serdisplib/serdisp_gpevents.h"

#if defined(HAVE_LIBPTHREAD) /* || defined(HAVE_NETSOCK_LIBS) */
 #include "serdisplib/serdisp_fctptr.h"
#endif

#ifdef HAVE_LIBPTHREAD
 #include <pthread.h>
 static pthread_mutex_t mutex_comm = PTHREAD_MUTEX_INITIALIZER;
#endif


/* #define OPT_USEOLDUPDATEALGO */

/* different display types/models supported by this driver */
#define DISPID_L4M320T      1

#define MAXPACKETSIZE       64
#define MAXCHUNK            (MAXPACKETSIZE - 2)

serdisp_options_t serdisp_l4m320t_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "CONTRAST",    "",             0,    10,  1, 1,  ""} 
  ,{  "ALARMHOUR",   "ALHOUR",       0,    23,  1, 0,  ""}
  ,{  "ALARMMINUTE", "ALMIN",        0,    59,  1, 0,  ""}
  ,{  "ALARMDAYS",   "ALDAY,ALDAYS", 0,  0x7F,  1, 0,  "OFF=0,ALL=127"}
  ,{  "TOUCHSKIP",   "",             0,    10,  1, 0,  "OFF=0,DEFAULT=4"} 
  ,{  "BRIGHTNESS",  "",             0,   100,  1, 1,  ""}     /* brightness [0 .. 100] */
  ,{  "ALARMMODE",   "",             0,     3,  1, 0,  "POST=0,IMMEDIATE=1,POSTDELAY=2,IMMEDDELAY=3"}
};


#ifdef HAVE_LIBPTHREAD
SDGPI_t serdisp_l4m320t_GPIs[] = {
   /* id  name     aliasnames         type               enabled  file descr. or last value */
   {  0, "TOUCHSCREEN",  "TOUCH",     SDGPT_SIMPLETOUCH,  0,      {0} }
};
#endif


/* internal typedefs and functions */

static void serdisp_ddusbt_init           (serdisp_t*);
static int  serdisp_ddusbt_setoption      (serdisp_t*, const char*, long);
static void serdisp_ddusbt_close          (serdisp_t*);

static void serdisp_ddusbt_update_l4m320t (serdisp_t*);

static void serdisp_ddusbt_clear          (serdisp_t*);

static int  serdisp_ddusbt_freeresources  (serdisp_t* dd);

#ifdef HAVE_LIBPTHREAD
static int           serdisp_l4m320t_gpi_enable     (serdisp_t* dd, byte gpid, int enable);
static SDGP_event_t* serdisp_l4m320t_evlp_receiver  (serdisp_t* dd, SDGP_event_t* recycle);
#endif /* HAVE_LIBPTHREAD */


typedef struct serdisp_l4m320t_specific_s {
  byte alarmhour, alarmminute, alarmdays;
  int  alarmmode;
  int touchskip;
  int touchskip_curr;
  struct timeval touchlastts;
} serdisp_l4m320t_specific_t;


static serdisp_l4m320t_specific_t* serdisp_l4m320t_internal_getStruct(serdisp_t* dd) {
  return (serdisp_l4m320t_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_l4m320t_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "TOUCHSKIP")) {
    *typesize = sizeof(int);
    return &(serdisp_l4m320t_internal_getStruct(dd)->touchskip);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMHOUR")) {
    *typesize = sizeof(byte);
    return &(serdisp_l4m320t_internal_getStruct(dd)->alarmhour);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMMINUTE")) {
    *typesize = sizeof(byte);
    return &(serdisp_l4m320t_internal_getStruct(dd)->alarmminute);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMDAYS")) {
    *typesize = sizeof(byte);
    return &(serdisp_l4m320t_internal_getStruct(dd)->alarmdays);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMMODE")) {
    *typesize = sizeof(int);
    return &(serdisp_l4m320t_internal_getStruct(dd)->alarmmode);
  }
  return 0;
}


/* callback-function for memory-deallications not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_ddusbt_freeresources (serdisp_t* dd) {
#ifdef HAVE_LIBPTHREAD
  if (dd->gpevset && dd->gpevset->gpis)  {
    free(dd->gpevset->gpis);
  }
#endif
  return 0;
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_ddusbt_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_ddusbt_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("L4M320T", dispname))
    dd->dsp_id = DISPID_L4M320T;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_ddusbt.c", dispname);
    return (serdisp_t*)0;
  }

  /* specific data for L4M320T-modules */
  if (dd->dsp_id == DISPID_L4M320T) {
    if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_l4m320t_specific_t)) )) {
      serdisp_freeresources(dd);
      return (serdisp_t*)0;
    }

    memset(dd->specific_data, 0, sizeof(serdisp_l4m320t_specific_t));
  }


  /* per display settings */

  dd->width             = 240;
  dd->height            = 320;
  dd->depth             = 16;
  dd->feature_backlight = 0;
  dd->feature_invert    = 1;

  dd->feature_contrast  = 1;

  dd->min_contrast      = 0x01;
  dd->max_contrast      = 0x7F;
  dd->mid_contrast      = 0xFF;

  dd->dsparea_width     = 43200;     /* active viewing area in micrometres  */
  dd->dsparea_height    = 57600;     /* source: datasheet */

  /* supported colour space */
  dd->colour_spaces     = SD_CS_RGB565 | SD_CS_BGR | SD_CS_SELFEMITTING;

  /* default setup for function pointers */
  dd->fp_init           = &serdisp_ddusbt_init;
  dd->fp_update         = &serdisp_ddusbt_update_l4m320t;
  dd->fp_close          = &serdisp_ddusbt_close;
  dd->fp_setoption      = &serdisp_ddusbt_setoption;
  dd->fp_clear          = &serdisp_ddusbt_clear;

  dd->fp_freeresources  = &serdisp_ddusbt_freeresources;
  dd->fp_getvalueptr    = &serdisp_l4m320t_getvalueptr;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta    = MAXCHUNK;

  serdisp_l4m320t_internal_getStruct(dd)->alarmmode = 0;  /* set alarm when closing the driver */

  serdisp_l4m320t_internal_getStruct(dd)->touchskip = 4;  /* filter first 4 touch events */
  gettimeofday(&(serdisp_l4m320t_internal_getStruct(dd)->touchlastts), 0);


  /* finally set some non display specific defaults */

  dd->curr_rotate         = 0;         /* unrotated display */
  dd->curr_invert         = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  serdisp_setupstructinfos(dd, 0, 0, serdisp_l4m320t_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }

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
    dd->gpevset->gpis = sdtools_malloc(sizeof(serdisp_l4m320t_GPIs));
    if (dd->gpevset->gpis) {
      memcpy(dd->gpevset->gpis, serdisp_l4m320t_GPIs, sizeof(serdisp_l4m320t_GPIs));
      dd->gpevset->amountgpis = sizeof(serdisp_l4m320t_GPIs)/sizeof(SDGPI_t);

      /* function pointers */
      dd->gpevset->fp_hnd_gpi_enable =  &serdisp_l4m320t_gpi_enable;
      dd->gpevset->fp_evlp_receiver =   &serdisp_l4m320t_evlp_receiver;
    }
  }
#endif /* HAVE_LIBPTHREAD */

  return dd;
}



/* *********************************
   void serdisp_ddusbt_init(dd)
   *********************************
   initialise a linux4media usb-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ddusbt_init(serdisp_t* dd) {
  SDCONN_write(dd->sdcd, 0x10000 | 0x01, 0);  /* init lcd */

  SDCONN_write(dd->sdcd, 0x10000 | 0x05, 0);  /* set normal display */

  /* set or unset alarm if ALARMMODE is set to IMMEDIATE */
  if (serdisp_l4m320t_internal_getStruct(dd)->alarmmode & 0x01) {
    /* if alarm is set: set time */
    if (serdisp_l4m320t_internal_getStruct(dd)->alarmdays != 0) {
      time_t t;
      struct tm *tm;

      t = time( ((time_t *)0) );
      tm = localtime(&t);

      SDCONN_write(dd->sdcd, 0x51, 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_hour), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_min), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_sec), 0);
      SDCONN_commit(dd->sdcd);

      SDCONN_write(dd->sdcd, 0x52, 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_mday), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_mon + 1), 0);     /* tm_mon in [0, 11] !! */
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_year % 100), 0);  /* tm_year = years since 1900 */
      SDCONN_commit(dd->sdcd);
    }

    /* enable / disable alarm time */
    SDCONN_write(dd->sdcd, 0x56, 0);
    if (serdisp_l4m320t_internal_getStruct(dd)->alarmdays) {
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(serdisp_l4m320t_internal_getStruct(dd)->alarmminute), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(serdisp_l4m320t_internal_getStruct(dd)->alarmhour), 0);
      SDCONN_write(dd->sdcd, serdisp_l4m320t_internal_getStruct(dd)->alarmdays, 0);
    } else {
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_write(dd->sdcd, 0x00, 0);
    }
    SDCONN_commit(dd->sdcd);

    serdisp_clear(dd);
    /* activate workaround for timinig bug in firmware (if DELAY flag is set) */
    if (serdisp_l4m320t_internal_getStruct(dd)->alarmmode & 0x02)
      sleep(4);
  }

#ifdef HAVE_LIBPTHREAD
  /* no need to autostart event loop if the sole GPI isn't enabled */
  if (dd->gpevset) {
    dd->gpevset->evlp_noautostart = 1;
  }
#endif

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_ddusbt_update_l4m320t(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg (colour display module)
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:
*/
void serdisp_ddusbt_update_l4m320t(serdisp_t* dd) {
#ifdef OPT_USEOLDUPDATEALGO
  int i;

#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );
#endif
  SDCONN_write(dd->sdcd, 0x0C, 0);                            /* set window */
  SDCONN_write(dd->sdcd, 0x00, 0);                            /* start horiz high 0-239 = 0x00EF */
  SDCONN_write(dd->sdcd, 0x00, 0);                            /* start horiz low */
  SDCONN_write(dd->sdcd, ((dd->width-1) & 0xFF00) >> 8, 0);   /* end horiz high */
  SDCONN_write(dd->sdcd, (dd->width-1) & 0x00FF, 0);          /* end horiz low */
  SDCONN_write(dd->sdcd, 0x00, 0);                            /* start vert high 0-319 = 0x0140 */
  SDCONN_write(dd->sdcd, 0x00, 0);                            /* start vert low */
  SDCONN_write(dd->sdcd, ((dd->height-1) & 0xFF00) >> 8, 0);  /* end vert high */
  SDCONN_write(dd->sdcd, (dd->height-1) & 0x00FF, 0);         /* end vert low */
  SDCONN_commit(dd->sdcd);

  for (i = 0; i < dd->scrbuf_size; i+=2) {
    if ((i % MAXCHUNK) == 0) {
      int chunk = MAXCHUNK;
      if (i > dd->scrbuf_size - chunk)
          chunk = dd->scrbuf_size - i;

      SDCONN_commit(dd->sdcd);
      SDCONN_write(dd->sdcd, 0x0e, 0);
      SDCONN_write(dd->sdcd, chunk, 0);
    }
    SDCONN_write(dd->sdcd, dd->scrbuf[i  ], 0);
    SDCONN_write(dd->sdcd, dd->scrbuf[i+1], 0);
  }
  SDCONN_commit(dd->sdcd);
  SDCONN_write(dd->sdcd, 0x0e, 0);
  SDCONN_write(dd->sdcd, 0, 0);    /* end data marker */
  SDCONN_commit(dd->sdcd);
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
#endif

#else  /* ! OPT_USEOLDUPDATEALGO */
  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  serdisp_usbdev_t*  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  int maxpayload = (usbitems->streamsize == MAXPACKETSIZE)
                   ? MAXCHUNK
                   : (usbitems->streamsize / MAXPACKETSIZE) * (MAXPACKETSIZE - MAXCHUNK);

  int w  = dd->width;
  int h  = dd->height;
  int xt = w;
  int yt = h;
  int xb = 0;
  int yb = 0;
  int rc = 0;
  int x,y, ypos=0;
  int ystep = w >> 3;

  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x+=8) {
      if (dd->scrbuf_chg[ypos+ (x>>3)] ) {
        rc = 1;
        if (x < xt) xt = x;
        if (y < yt) yt = y;
        if (x > xb) xb = x;
        if (y > yb) yb = y;
      }
    }
    ypos+=ystep;
  }

  xb += 7;

  if (rc) {
    int i, j, cnt, pos;
    int chunk;
    int itotal;
    int istart;

    /* fprintf(stderr, "rc: %d  xt/yt: %d/%d   xb/yb: %d/%d\n", rc, xt, yt, xb, yb); */
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );
#endif

    SDCONN_write(dd->sdcd, 0x0C, 0);                            /* set window */
    SDCONN_write(dd->sdcd, (xt & 0xFF00) >> 8, 0);              /* start horiz high 0-239 = 0x00EF */
    SDCONN_write(dd->sdcd, (xt & 0x00FF), 0);                   /* start horiz low */
    SDCONN_write(dd->sdcd, (xb & 0xFF00) >> 8, 0);              /* end horiz high */
    SDCONN_write(dd->sdcd, (xb & 0x00FF), 0);                   /* end horiz low */
    SDCONN_write(dd->sdcd, (yt & 0xFF00) >> 8, 0);              /* start vert high 0-319 = 0x0140 */
    SDCONN_write(dd->sdcd, (yt & 0x00FF), 0);                   /* start vert low */
    SDCONN_write(dd->sdcd, (yb & 0xFF00) >> 8, 0);              /* end vert high */
    SDCONN_write(dd->sdcd, (yb & 0x00FF), 0);                   /* end vert low */
    SDCONN_commit(dd->sdcd);

    istart = (w << 1) * yt;
    itotal = ((xb - xt + 1) << 1) * (yb - yt + 1);

    cnt = 0;
    pos = istart;
    for (j = yt; j <= yb; j++) {
      pos += (xt << 1);
      for (i = xt; i <= xb; i++) {
        if ((cnt % MAXCHUNK) == 0) {
          chunk = MAXCHUNK;
          if (cnt > itotal - chunk)
              chunk = itotal - cnt;

          /*mpframes supported (maxpayload > 64): commit only if maxpayload is reached */
          if ( (cnt % maxpayload) == 0 )
            SDCONN_commit(dd->sdcd);

          SDCONN_write(dd->sdcd, 0x0e, 0);
          SDCONN_write(dd->sdcd, chunk, 0);
        }
        SDCONN_write(dd->sdcd, dd->scrbuf[pos++], 0);
        SDCONN_write(dd->sdcd, dd->scrbuf[pos++], 0);
        cnt+=2;
      }
      pos += ((w - 1 - xb) << 1);
    }

    SDCONN_commit(dd->sdcd);

    SDCONN_write(dd->sdcd, 0x0e, 0);
    SDCONN_write(dd->sdcd, 0, 0);    /* end data marker */
    SDCONN_commit(dd->sdcd);

#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
#endif

    memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
  }
#endif /* OPT_USEOLDUPDATEALGO */
}



/* *********************************
   int serdisp_ddusbt_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_ddusbt_setoption(serdisp_t* dd, const char* option, long value) {
  /*serdisp_l4m320t_specific_t*  data     = 0;
  data     = (serdisp_l4m320t_specific_t *)(dd->specific_data);*/

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    /* 0x04: invers mode;  0x05: normal mode */
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );
#endif
    SDCONN_write(dd->sdcd, 0x10000 | ((dd->curr_invert) ? 0x04 : 0x05), 0);
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
#endif
  } else if (dd->feature_contrast && 
             (serdisp_compareoptionnames(dd, option, "CONTRAST" ) ||
              serdisp_compareoptionnames(dd, option, "BRIGHTNESS" )
             )
            ) {
    int dimmed_contrast;

    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
    } else {
      dd->curr_dimming = 100 - (int)value;
    }

    dimmed_contrast = (((dd->curr_contrast - dd->min_contrast) * (100 - dd->curr_dimming)) / 100) + dd->min_contrast;

    /* workaround to be able to disable backlight or set values < dd->min_contrast */
    if (dd->curr_dimming >= (100 - dd->min_contrast)) {
      dimmed_contrast -= (dd->min_contrast - (100 - dd->curr_dimming));
    }

#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_lock( &mutex_comm );
#endif
    SDCONN_write(dd->sdcd, 0x06, 0);
    SDCONN_write(dd->sdcd, dimmed_contrast /*dd->curr_contrast*/, 0);
    SDCONN_commit(dd->sdcd);
#ifdef HAVE_LIBPTHREAD
    if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
      fp_pthread_mutex_unlock( &mutex_comm );
#endif
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}





#ifdef HAVE_LIBPTHREAD
/* *********************************
   int serdisp_l4m320t_gpi_enable(dd, gpid, enable)
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
int serdisp_l4m320t_gpi_enable (serdisp_t* dd, byte gpid, int enable) {
  if (!dd || !dd->gpevset || gpid >= dd->gpevset->amountgpis)
    return -1;

  if (enable && SDEVLP_getstatus(dd) == SDEVLP_STOPPED) {
    SDEVLP_start(dd);
  }

  dd->gpevset->gpis[gpid].enabled = enable;

  if (enable)
    serdisp_l4m320t_internal_getStruct(dd)->touchskip_curr = 0;

  if (!enable && SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    unsigned char  buffer[64];
    int bytes;
    /* clear queue */
    do {
      bytes = SDCONN_readstream(dd->sdcd, buffer, 64);
    } while (bytes > 0 && bytes != 7);
    SDEVLP_stop(dd);
  }

  return 0;
}
#endif


#ifdef HAVE_LIBPTHREAD
/* *********************************
   SDGP_event_t* serdisp_l4m320t_evlp_receiver(dd, recycle)
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
SDGP_event_t* serdisp_l4m320t_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle) {
  unsigned char  buffer[64];
  int            bytes;
  SDGP_event_t*  event = 0;
  SDGP_evpkt_simpletouch_t  simpletouch;
  int            skipevent = 0;
  int            try = 0;

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return 0;

  if (SDGPI_isenabled(dd, 0)) {

    try = 0;
    do {
      bytes = SDCONN_readstream(dd->sdcd, buffer, 64);

      if (buffer[0] != 0x49) try++;
    } while (bytes > 0 && (! (buffer[0] == 0x54 || buffer[0] == 0x49)) && try < 10);

    if( bytes < 0) {
      return (SDGP_event_t*)0;
    }

    if( buffer[0] == 0x54 && bytes != 7 ) {
      fprintf(stderr,"Short read from USB: %d bytes, stopping reading of events\n", bytes);
      SDEVLP_stop(dd);
      return 0;
    }

    if (buffer[0] == 0x54 && (buffer[3] != 0 || buffer[4] != 0)) {
      uint64_t timediff;

      int16_t w = (int16_t)(serdisp_getwidth(dd));
      int16_t h = (int16_t)(serdisp_getheight(dd));

#if 0
      if (buffer[0] == 0x54 && (buffer[3] != 0 || buffer[4] != 0)) {
        int tx = ((buffer[1]<<8)+buffer[2]) /* - 0xE900 */;
        int ty = (short)((((buffer[5]<<8)+buffer[6]) /*-0xFC40 */));
        int tt = (buffer[3] << 8) + buffer[4];
        /*if (tx >= 0 && ty >= 0) {*/
          printf( "USB read[%d]: %3d / %3d / %4d   %02x  %02x %02x  %02x %02x  %02x %02x\n", bytes, tx, ty, tt, buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6]);
        /*}*/
      }
#endif

      switch (dd->curr_rotate) {
        case 0:  /* 0 degrees */
          simpletouch.raw_x      = (int16_t)((buffer[1]<<8)+buffer[2]);
          simpletouch.raw_y      = (int16_t)((buffer[5]<<8)+buffer[6]);
        break;
        case 1:  /* 180 degrees */
          simpletouch.raw_x      = w - ((int16_t)((buffer[1]<<8)+buffer[2]));
          simpletouch.raw_y      = h - ((int16_t)((buffer[5]<<8)+buffer[6]));
        break;
        case 2:  /* 90 degrees */
          simpletouch.raw_x      = w - ((int16_t)((buffer[5]<<8)+buffer[6]));
          simpletouch.raw_y      = (int16_t)((buffer[1]<<8)+buffer[2]);
        break;
        default:  /* 270 degrees */
          simpletouch.raw_x      = (int16_t)((buffer[5]<<8)+buffer[6]);
          simpletouch.raw_y      = h - ((int16_t)((buffer[1]<<8)+buffer[2]));
      }
      simpletouch.raw_touch  = (int16_t)((buffer[3]<<8)+buffer[4]);
      simpletouch.norm_x     = simpletouch.raw_x;  /* no need for normalisation */
      simpletouch.norm_y     = simpletouch.raw_y;
      simpletouch.norm_touch = 4096-simpletouch.raw_touch;
      if (simpletouch.norm_touch < 0)
        simpletouch.norm_touch = 0;

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
      event->type = SDGPT_SIMPLETOUCH;
      gettimeofday(&event->timestamp, 0);
      memcpy(&event->data, &simpletouch, sizeof(SDGP_evpkt_simpletouch_t));


      timediff = (event->timestamp.tv_sec * 1000000 + event->timestamp.tv_usec) -
                 (serdisp_l4m320t_internal_getStruct(dd)->touchlastts.tv_sec * 1000000 +
                  serdisp_l4m320t_internal_getStruct(dd)->touchlastts.tv_usec);

      /* check if event is to be skipped */
      if ( serdisp_l4m320t_internal_getStruct(dd)->touchskip > 0 &&
           /* to be improved */
           timediff >= (uint64_t) (/*(serdisp_l4m320t_internal_getStruct(dd)->touchskip) * */ 40000) &&
           serdisp_l4m320t_internal_getStruct(dd)->touchskip_curr < serdisp_l4m320t_internal_getStruct(dd)->touchskip
         ) {
        skipevent = 1;
        /*fprintf(stderr, "skipping event #%d\n", serdisp_l4m320t_internal_getStruct(dd)->touchskip_curr);*/
        serdisp_l4m320t_internal_getStruct(dd)->touchskip_curr ++;
      } else {
        serdisp_l4m320t_internal_getStruct(dd)->touchskip_curr = 0;
        serdisp_l4m320t_internal_getStruct(dd)->touchlastts.tv_sec = event->timestamp.tv_sec;
        serdisp_l4m320t_internal_getStruct(dd)->touchlastts.tv_usec = event->timestamp.tv_usec;
      }

#if 0
      /* convert header and payload to network byte order */
      SDGPT_event_header_hton(event);
      SDGPT_event_payload_hton(stream, sizeof(stream), (byte)sizeof(int16_t));


      /* convert header and payload back to host byte order */
      SDGPT_event_header_ntoh(event);
      SDGPT_event_payload_ntoh(stream, sizeof(stream), (byte)sizeof(int16_t));
#endif
    } else if (buffer[0] == 0x49 && (buffer[1] != 0xFF && buffer[2] != 0xFF && buffer[3] != 0xFF)) {
#if 0
      int i;
      fprintf(stderr, "IR signal: ");
      for (i = 1; i < bytes; i++)
        fprintf(stderr, "%02x ", buffer[i]);
      fprintf(stderr, "\n");
#endif
      event = 0;
    } else {
      event = 0;
    }
  }

  usleep(100);
  return (skipevent) ? 0 : event;
}
#endif /* HAVE_LIBPTHREAD */


/* *********************************
   void serdisp_ddusbt_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ddusbt_clear(serdisp_t* dd) {
#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
#endif

  SDCONN_commit(dd->sdcd);
  /* clear display using internal function 0x02 */
  SDCONN_write(dd->sdcd, 0x02, 0);
  SDCONN_write(dd->sdcd, 0x00, 0); /* clear with colour black (= white if in invert-mode) */
  SDCONN_write(dd->sdcd, 0x00, 0);
  SDCONN_commit(dd->sdcd);

  /* set screen buffer to be clean */
  memset (dd->scrbuf, 0x00, dd->scrbuf_size);

#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
#endif
}


/* *********************************
   void serdisp_ddusbt_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ddusbt_close(serdisp_t* dd) {
  /* if ALARMMODE is set to POST, set / unset alarm when closing the driver */
  if (! (serdisp_l4m320t_internal_getStruct(dd)->alarmmode & 0x01)) {
    /* if alarm is set: set time */
    if (serdisp_l4m320t_internal_getStruct(dd)->alarmdays != 0) {
      time_t t;
      struct tm *tm;

      t = time( ((time_t *)0) );
      tm = localtime(&t);

      SDCONN_write(dd->sdcd, 0x51, 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_hour), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_min), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_sec), 0);
      SDCONN_commit(dd->sdcd);

      SDCONN_write(dd->sdcd, 0x52, 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_mday), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_mon + 1), 0);     /* tm_mon in [0, 11] !! */
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_year % 100), 0);  /* tm_year = years since 1900 */
      SDCONN_commit(dd->sdcd);
    }

    /* enable / disable alarm time */
    SDCONN_write(dd->sdcd, 0x56, 0);
    if (serdisp_l4m320t_internal_getStruct(dd)->alarmdays) {
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(serdisp_l4m320t_internal_getStruct(dd)->alarmminute), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(serdisp_l4m320t_internal_getStruct(dd)->alarmhour), 0);
      SDCONN_write(dd->sdcd, serdisp_l4m320t_internal_getStruct(dd)->alarmdays, 0);
    } else {
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_write(dd->sdcd, 0x00, 0);
    }
    SDCONN_commit(dd->sdcd);
    serdisp_clear(dd);
    /* activate workaround for timinig bug in firmware (if D flag is set) */
    if (serdisp_l4m320t_internal_getStruct(dd)->alarmmode & 0x02)
      sleep(3);
  }
  serdisp_clear(dd);
}
