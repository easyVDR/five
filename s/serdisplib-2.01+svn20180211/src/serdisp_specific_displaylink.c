/** \file    serdisp_specific_displaylink.c
  *
  * \brief   Routines for drawing content to displaylink compliant devices
  * \date    (C) 2010-2014
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "../config.h"

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"

#include "serdisplib/serdisp_fctptr.h"

#include "serdisplib/serdisp_gpevents.h"


/* #define OPT_USEOLDUPDATEALGO */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_DISPLAYLINK   1


serdisp_options_t serdisp_displaylink_options[] = {
   /*  name          aliasnames     min    max mod int                 defines  */
  {  "WIDTH",        "",             0,   1920,  1, 0,                 "default=0"}
 ,{  "HEIGHT",       "",             0,   1280,  1, 0,                 "default=0"}
 ,{  "WORKAROUND",   "WO",           0,      2,  1, 0,                 ""}
 ,{  "TOUCHDEVICE",  "TSDEV",       -1,     -1, -1, SD_OPTIONFLAG_LOC, ""}
 ,{  "TOUCHSWAPX",   "TSSWAPX",      0,      1,  1, 0,                 "OFF=0,ON=1,NO=0,YES=1"}
 ,{  "TOUCHSWAPY",   "TSSWAPY",      0,      1,  1, 0,                 "OFF=0,ON=1,NO=0,YES=1"}
 ,{  "TOUCHMINX",    "TSMINX",       0, 0xFFFF,  1, 0,                 ""}
 ,{  "TOUCHMINY",    "TSMINY",       0, 0xFFFF,  1, 0,                 ""}
 ,{  "TOUCHMAXX",    "TSMAXX",       0, 0xFFFF,  1, 0,                 ""}
 ,{  "TOUCHMAXY",    "TSMAXY",       0, 0xFFFF,  1, 0,                 ""}
};


#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
SDGPI_t serdisp_displaylink_GPIs[] = {
   /* id  name     aliasnames         type                enabled  file descr. or last value */
   {  0, "TOUCHSCREEN",  "TOUCH",     SDGPT_GENERICTOUCH,  0,      {0} }
};
#endif


/* internal typedefs and functions */

static void serdisp_displaylink_init      (serdisp_t*);
static void serdisp_displaylink_update    (serdisp_t*);
static int  serdisp_displaylink_setoption (serdisp_t*, const char*, long);
static void serdisp_displaylink_close     (serdisp_t*);
static void serdisp_displaylink_clear     (serdisp_t*);

#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
static int           serdisp_displaylink_gpi_enable     (serdisp_t* dd, byte gpid, int enable);
static SDGP_event_t* serdisp_displaylink_evlp_receiver  (serdisp_t* dd, SDGP_event_t* recycle);
#endif /* HAVE_LIBPTHREAD */


typedef struct serdisp_displaylink_specific_s {
  void*  uid;
  int    workaround;
  char*  ts_devname;
  SDTOUCH_idev_touchdevice_t touchdev;   /* touch device */
} serdisp_displaylink_specific_t;

static serdisp_displaylink_specific_t* serdisp_displaylink_internal_getStruct(serdisp_t* dd) {
  return (serdisp_displaylink_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_displaylink_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "WORKAROUND")) {
    *typesize = sizeof(int);
    return &(serdisp_displaylink_internal_getStruct(dd)->workaround);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHDEVICE")) {
    *typesize = 0;
    return &(serdisp_displaylink_internal_getStruct(dd)->ts_devname);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHSWAPX")) {
    *typesize = sizeof(int);
    return &(serdisp_displaylink_internal_getStruct(dd)->touchdev.swapx);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHSWAPY")) {
    *typesize = sizeof(int);
    return &(serdisp_displaylink_internal_getStruct(dd)->touchdev.swapy);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMINX")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_displaylink_internal_getStruct(dd)->touchdev.minrawx);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMINY")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_displaylink_internal_getStruct(dd)->touchdev.minrawy);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMAXX")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_displaylink_internal_getStruct(dd)->touchdev.maxrawx);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMAXY")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_displaylink_internal_getStruct(dd)->touchdev.maxrawy);
  }
  return 0;
}


/* callback-function for memory-deallocations not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_displaylink_freeresources (serdisp_t* dd) {
  if (serdisp_displaylink_internal_getStruct(dd)->ts_devname)
    free (serdisp_displaylink_internal_getStruct(dd)->ts_devname);
#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
  if (dd->gpevset && dd->gpevset->gpis)  {
    free(dd->gpevset->gpis);
  }
#endif
  return 0;
}


/* main functions */


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
  * \since   1.98
  */
serdisp_t* serdisp_displaylink_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  dlo_init_t     ini_flags = { 0 };
  dlo_claim_t    cnf_flags = { 0 };
  dlo_retcode_t  rc;
  dlo_dev_t      uid = 0;
  dlo_mode_t*    mode_info;
  int            retry = 10;

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBDLO) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libdlo is not loaded.", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_displaylink_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_displaylink_specific_t));

  /* output devices like SDL supported here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("DISPLAYLINK", dispname))
    dd->dsp_id = DISPID_DISPLAYLINK;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_displaylink.c", dispname);
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_OUT;


  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 0;

  if ( (rc = fp_dlo_init(ini_flags)) != dlo_ok) {
    sd_error(SERDISP_ERUNTIME, "%s(): error when calling dlo_init: %s", __func__, fp_dlo_strerror(rc));
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }

  while (!uid && retry > 0) {
    uid = fp_dlo_claim_first_device(cnf_flags, 0);
    retry --;
    sdtools_nsleep(1000);
  }
  if (!uid) {
    sd_error(SERDISP_ERUNTIME, "%s(): error when claiming first device", __func__);
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }
  serdisp_displaylink_internal_getStruct(dd)->uid = (void*)uid;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 0;  /* unused, bbox is used for optimisation */

  dd->delay = 0;

  serdisp_displaylink_internal_getStruct(dd)->workaround = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 0;         /* start with backlight on */

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_TRUECOLOUR;
  dd->colour_spaces    |= SD_CS_SELFEMITTING;

  dd->fp_init           = &serdisp_displaylink_init;
  dd->fp_update         = &serdisp_displaylink_update;
  dd->fp_clear          = &serdisp_displaylink_clear;
  dd->fp_close          = &serdisp_displaylink_close;
  dd->fp_setoption      = &serdisp_displaylink_setoption;
  dd->fp_getvalueptr    = &serdisp_displaylink_getvalueptr;

  dd->fp_freeresources  = &serdisp_displaylink_freeresources;

  serdisp_setupstructinfos(dd, 0, 0, serdisp_displaylink_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  if (dd->width && dd->height) {
    dd->depth             = 24;
    dlo_mode_t     direct_mode_info;


    direct_mode_info.view.width  = dd->width;
    direct_mode_info.view.height = dd->height;
    direct_mode_info.view.bpp    = dd->depth;

    if ( (rc = fp_dlo_set_mode(uid, &direct_mode_info) ) != dlo_ok) {
      sd_error(SERDISP_ERUNTIME, "%s(): error when setting screen mode: %s", __func__, fp_dlo_strerror(rc));
      fp_dlo_release_device(uid);
      serdisp_freeresources(dd);
      return (serdisp_t*)0;
    } else {
      sd_debug(2, "%s(): display mode set to w/h = %d/%d, depth: %d", __func__, 
               direct_mode_info.view.width, direct_mode_info.view.height, direct_mode_info.view.bpp);
    }
  }

  mode_info = fp_dlo_get_mode(uid);
  if (!mode_info) {
    sd_error(SERDISP_ERUNTIME, "%s(): error when getting mode info", __func__);
    fp_dlo_release_device(uid);
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  } else {
    sd_debug(2, "%s(): display found with w/h = %d/%d, depth: %d", __func__, 
             mode_info->view.width, mode_info->view.height, mode_info->view.bpp);
  }

  /* per display settings */
  dd->width             = mode_info->view.width;
  dd->height            = mode_info->view.height;
  dd->depth             = mode_info->view.bpp;

/* only GPIs required, thus execute the whole gpevent-stuff only if libpthread is available */
#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
  serdisp_displaylink_internal_getStruct(dd)->touchdev.fd = -1;

  /* only one GPI (touchscreen) is supported: so activate events only if touchscreen device is set */
  if (serdisp_displaylink_internal_getStruct(dd)->ts_devname) {
    int rc = SDTOUCH_idev_open (
      serdisp_displaylink_internal_getStruct(dd)->ts_devname,
      &(serdisp_displaylink_internal_getStruct(dd)->touchdev)
    );

    if(rc == 0) {
      /* add gpevset */
      if (! (dd->gpevset = (SDGP_gpevset_t*) sdtools_malloc( sizeof(SDGP_gpevset_t)) )) {
        sd_debug(0, 
          "%s(): cannot allocate memory for general purpose event set. continuing without support for it ...",
          __func__
        );
        SDTOUCH_idev_close( &(serdisp_displaylink_internal_getStruct(dd)->touchdev) );
      } else {
        memset(dd->gpevset, 0, sizeof(SDGP_gpevset_t));

        /* add GPIs */
        dd->gpevset->gpis = sdtools_malloc(sizeof(serdisp_displaylink_GPIs));
        if (dd->gpevset->gpis) {
          memcpy(dd->gpevset->gpis, serdisp_displaylink_GPIs, sizeof(serdisp_displaylink_GPIs));
          dd->gpevset->amountgpis = sizeof(serdisp_displaylink_GPIs)/sizeof(SDGPI_t);

          /* function pointers */
          dd->gpevset->fp_hnd_gpi_enable =  &serdisp_displaylink_gpi_enable;
          dd->gpevset->fp_evlp_receiver =   &serdisp_displaylink_evlp_receiver;
        }
      }
    } else if (rc == -2) {
      sd_debug(1, "%s(): touchscreen already open", __func__);
    } else {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot open touchscreen", __func__);
    }
  }
#endif /* HAVE_LIBPTHREAD */

  return dd;
}


/**
  * \brief   initialise
  *
  * initialises framebuffer driver
  *
  * \param   dd             display descriptor
  *
  * \since   1,98
  */
void serdisp_displaylink_init(serdisp_t* dd) {
#if 0
  dlo_mode_t*    mode_info;
  dlo_dev_t      uid = (dlo_dev_t)serdisp_displaylink_internal_getStruct(dd)->uid;

  mode_info = fp_dlo_get_mode(uid);
  mode_info->view.base = (dlo_ptr_t)(dd->scrbuf);
  fp_dlo_set_mode(uid, mode_info);
#endif

#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
  /* no need to autostart event loop if the sole GPI isn't enabled */
  if (dd->gpevset) {
    dd->gpevset->evlp_noautostart = 1;
  }
#endif

  sd_debug(2, "%s(): done with initialising", __func__);
}


/**
  * \brief   update displaylink output
  *
  * updates the displaylink output using display-buffer scrbuf+scrbuf_chg using a time-saving algorithm.
  *
  * \param   dd             display descriptor
  *
  * \since   1.98
  */
void serdisp_displaylink_update(serdisp_t* dd) {
  dlo_dev_t      uid = (dlo_dev_t)serdisp_displaylink_internal_getStruct(dd)->uid;
  dlo_bmpflags_t flags = { 0 };
  dlo_fbuf_t     fbuf;
#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */
  fbuf.width  = serdisp_getwidth(dd) - ((serdisp_displaylink_internal_getStruct(dd)->workaround == 1) ? 1 : 0);
  fbuf.height = serdisp_getheight(dd);
  fbuf.base   = dd->scrbuf;
  fbuf.fmt    = dlo_pixfmt_bgr888;
  fbuf.stride = serdisp_getwidth(dd);

  fp_dlo_copy_host_bmp (uid, flags, &fbuf, NULL, NULL);
  memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
#else
  dlo_dot_t      dot;
  int w  = serdisp_getwidth(dd);
  int h  = serdisp_getheight(dd);
  int xt = w+1;
  int yt = h+1;
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

/* fprintf(stderr, "rc: %d  xt/yt: %3d/%3d   xb/yb: %3d/%3d   w/h:%3d/%3d\n", rc, xt, yt, xb, yb, xb-xt+1, yb-yt+1); */
  if (rc) {
    if ( serdisp_displaylink_internal_getStruct(dd)->workaround == 2) {
      fbuf.width  = w;
      fbuf.height = h;
      fbuf.base   = dd->scrbuf;
      fbuf.fmt    = dlo_pixfmt_bgr888;
      fbuf.stride = w;
      dot.x = 0;
      dot.y = 0;
    } else {
      fbuf.width  = xb-xt+1;
      fbuf.height = yb-yt+1;
      fbuf.base   = dd->scrbuf + (dd->depth >> 3) * (yt * w + xt);
      fbuf.fmt    = dlo_pixfmt_bgr888;
      fbuf.stride = w;
      dot.x = xt;
      dot.y = yt;
    }

    if ( serdisp_displaylink_internal_getStruct(dd)->workaround == 1 && xt+fbuf.width >= w)
      fbuf.width--;

    fp_dlo_copy_host_bmp (uid, flags, &fbuf, NULL, &dot);

    memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
  }

#endif /* OPT_USEOLDUPDATEALGO */
}


/**
  * \brief   change a display option
  *
  * changes a display option
  *
  * \param   dd             display descriptor
  * \param   option         name of option to change
  * \param   value          value for given option
  * 
  * \retval 0    option not found, call generic function serdisp_setoption()
  * \retval 1    option found
  *
  * \since   1.98
  */
int serdisp_displaylink_setoption(serdisp_t* dd, const char* option, long value) {
  if (serdisp_compareoptionnames(dd, option, "TOUCHDEVICE") ) {
    serdisp_displaylink_internal_getStruct(dd)->ts_devname = (char*) sdtools_malloc( strlen( (char*) value )+1);
    strcpy(serdisp_displaylink_internal_getStruct(dd)->ts_devname, (char*)value);
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHSWAPX") ) {
    serdisp_displaylink_internal_getStruct(dd)->touchdev.swapx = (int) value;
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHSWAPY") ) {
    serdisp_displaylink_internal_getStruct(dd)->touchdev.swapy = (int) value;
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHMINX") ) {
    serdisp_displaylink_internal_getStruct(dd)->touchdev.minrawx = (int32_t) value;
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHMINY") ) {
    serdisp_displaylink_internal_getStruct(dd)->touchdev.minrawy = (int32_t) value;
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHMAXX") ) {
    serdisp_displaylink_internal_getStruct(dd)->touchdev.maxrawx = (int32_t) value;
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHMAXY") ) {
    serdisp_displaylink_internal_getStruct(dd)->touchdev.maxrawy = (int32_t) value;
  }
#if 0
  if (dd->feature_contrast && serdisp_compareoptionnames(dd, option, "CONTRAST")) {
    dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
  } else {
#endif
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
#if 0
  }
  return 1;
#endif
}


/**
  * \brief   clear displaylink output
  *
  * clears displaylink device output
  *
  * \param   dd             display descriptor
  * 
  * \since   1.98
  */
void serdisp_displaylink_clear(serdisp_t* dd) {
  dlo_dev_t      uid = (dlo_dev_t)serdisp_displaylink_internal_getStruct(dd)->uid;
  if (dd->curr_invert) {
    fp_dlo_fill_rect(uid, NULL, NULL, 0x00FFFFFF);
  } else {
    fp_dlo_fill_rect(uid, NULL, NULL, 0x00000000);
  }
}


/**
  * \brief   close (switch off) displaylink device
  *
  * closes displaylink device
  *
  * \param   dd             display descriptor
  * 
  * \since   1.98
  */
void serdisp_displaylink_close(serdisp_t* dd) {
  dlo_dev_t      uid = (dlo_dev_t)serdisp_displaylink_internal_getStruct(dd)->uid;
  dlo_final_t    fin_flags = { 0 };

#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
  SDTOUCH_idev_close( &(serdisp_displaylink_internal_getStruct(dd)->touchdev) );
#endif
  serdisp_clear(dd);
  fp_dlo_release_device(uid);
  fp_dlo_final(fin_flags);
}


#if defined(__linux__) && defined(HAVE_LIBPTHREAD)
/* *********************************
   int serdisp_displaylink_gpi_enable(dd, gpid, enable)
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
int serdisp_displaylink_gpi_enable (serdisp_t* dd, byte gpid, int enable) {
  if (!dd || !dd->gpevset || gpid >= dd->gpevset->amountgpis)
    return -1;

  dd->gpevset->gpis[gpid].enabled = enable;

  if (enable && SDEVLP_getstatus(dd) == SDEVLP_STOPPED && serdisp_displaylink_internal_getStruct(dd)->touchdev.fd != -1) {
    SDTOUCH_idev_enable( &(serdisp_displaylink_internal_getStruct(dd)->touchdev), 1);

    SDEVLP_start(dd);
  }

  if (!enable && SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    SDTOUCH_idev_enable( &(serdisp_displaylink_internal_getStruct(dd)->touchdev), 0);

    SDEVLP_stop(dd);
  }

  return 0;
}


/* *********************************
   SDGP_event_t* serdisp_displaylink_evlp_receiver(dd, recycle)
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
SDGP_event_t* serdisp_displaylink_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle) {
  return SDTOUCH_idev_evlp_receiver(dd, &(serdisp_displaylink_internal_getStruct(dd)->touchdev), recycle);
}
#endif /* HAVE_LIBPTHREAD */


