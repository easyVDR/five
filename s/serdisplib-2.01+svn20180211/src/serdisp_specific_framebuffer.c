/** \file    serdisp_specific_framebuffer.c
  *
  * \brief   Routines for drawing content to a framebuffer device
  * \date    (C) 2010-2017
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
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/fd.h>

#include "../config.h"

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"


#if defined(HAVE_LIBPTHREAD)
  #include "serdisplib/serdisp_gpevents.h"
  #include "serdisplib/serdisp_fctptr.h"
#endif

#define  FB_DEFAULT_DEVICE "/dev/fb0"

/* #define OPT_USEOLDUPDATEALGO  */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_FRAMEBUFFER   1


serdisp_options_t serdisp_framebuffer_options[] = {
   /*  name          aliasnames     min    max mod int defines  */
  {  "FBDEVICE",     "FBDEV",       -1,     -1, -1, SD_OPTIONFLAG_LOC, "DEFAULT=/dev/fb0"}
 ,{  "REPORTDAMAGE", "DAMAGE",       0,      3, -1, SD_OPTIONFLAG_LOC, "NONE=0,OFF=0,AUTO=1,UDLFB=2,UGLY=3"}
 ,{  "TOUCHDEVICE",  "TSDEV",       -1,     -1, -1, SD_OPTIONFLAG_LOC, ""}
 ,{  "TOUCHSWAPX",   "TSSWAPX",      0,      1,  1, 0,                 "OFF=0,ON=1,NO=0,YES=1"}
 ,{  "TOUCHSWAPY",   "TSSWAPY",      0,      1,  1, 0,                 "OFF=0,ON=1,NO=0,YES=1"}
 ,{  "TOUCHMINX",    "TSMINX",       0, 0xFFFF,  1, 0,                 ""}
 ,{  "TOUCHMINY",    "TSMINY",       0, 0xFFFF,  1, 0,                 ""}
 ,{  "TOUCHMAXX",    "TSMAXX",       0, 0xFFFF,  1, 0,                 ""}
 ,{  "TOUCHMAXY",    "TSMAXY",       0, 0xFFFF,  1, 0,                 ""}
};


#ifdef HAVE_LIBPTHREAD
SDGPI_t serdisp_framebuffer_GPIs[] = {
   /* id  name     aliasnames         type                enabled  file descr. or last value */
   {  0, "TOUCHSCREEN",  "TOUCH",     SDGPT_GENERICTOUCH,  0,      {0} }
};
#endif


/* internal typedefs and functions */

static void  serdisp_framebuffer_init          (serdisp_t*);
static void  serdisp_framebuffer_update        (serdisp_t*);
static int   serdisp_framebuffer_setoption     (serdisp_t*, const char*, long);
static void  serdisp_framebuffer_clear         (serdisp_t*);
static void  serdisp_framebuffer_close         (serdisp_t*);

static void  serdisp_framebuffer_setsdpixel    (serdisp_t*, int, int, SDCol);
static SDCol serdisp_framebuffer_getsdpixel    (serdisp_t*, int, int);

static int   serdisp_framebuffer_freeresources (serdisp_t* dd);

#ifdef HAVE_LIBPTHREAD
static int           serdisp_framebuffer_gpi_enable     (serdisp_t* dd, byte gpid, int enable);
static SDGP_event_t* serdisp_framebuffer_evlp_receiver  (serdisp_t* dd, SDGP_event_t* recycle);
#endif /* HAVE_LIBPTHREAD */



typedef struct serdisp_framebuffer_specific_s {
  char* fb_devname;
  int fbhd;
  char* fbmmap;
  int   report_damage; /* damage reporting required? */
  int   fb_typeid;     /* if damage reporting:  type id of framebuffer (1=udlfb) */
  uint8_t  idx_lot[4]; /* look up table for index offset (little vs. big endian) */
  char* ts_devname;
  SDTOUCH_idev_touchdevice_t touchdev;   /* touch device */
} serdisp_framebuffer_specific_t;


static serdisp_framebuffer_specific_t* serdisp_framebuffer_internal_getStruct(serdisp_t* dd) {
  return (serdisp_framebuffer_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_framebuffer_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "FBDEVICE")) {
    *typesize = 0;
    return &(serdisp_framebuffer_internal_getStruct(dd)->fb_devname);
  } else if (serdisp_compareoptionnames(dd, optionname, "REPORTDAMAGE")) {
    *typesize = sizeof(int);
    return &(serdisp_framebuffer_internal_getStruct(dd)->report_damage);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHDEVICE")) {
    *typesize = 0;
    return &(serdisp_framebuffer_internal_getStruct(dd)->ts_devname);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHSWAPX")) {
    *typesize = sizeof(int);
    return &(serdisp_framebuffer_internal_getStruct(dd)->touchdev.swapx);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHSWAPY")) {
    *typesize = sizeof(int);
    return &(serdisp_framebuffer_internal_getStruct(dd)->touchdev.swapy);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMINX")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_framebuffer_internal_getStruct(dd)->touchdev.minrawx);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMINY")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_framebuffer_internal_getStruct(dd)->touchdev.minrawy);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMAXX")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_framebuffer_internal_getStruct(dd)->touchdev.maxrawx);
  } else if (serdisp_compareoptionnames(dd, optionname, "TOUCHMAXY")) {
    *typesize = sizeof(int32_t);
    return &(serdisp_framebuffer_internal_getStruct(dd)->touchdev.maxrawy);
  }

  return 0;
}


/* callback-function for memory-deallocations not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_framebuffer_freeresources (serdisp_t* dd) {
  if (serdisp_framebuffer_internal_getStruct(dd)->fb_devname)
    free (serdisp_framebuffer_internal_getStruct(dd)->fb_devname);
  if (serdisp_framebuffer_internal_getStruct(dd)->ts_devname)
    free (serdisp_framebuffer_internal_getStruct(dd)->ts_devname);
#ifdef HAVE_LIBPTHREAD
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
serdisp_t* serdisp_framebuffer_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  int fb_success;  /* errors when opening and initialising framebuffer device? */

  if (! sdcd ) {
    sd_error(SERDISP_ERUNTIME, "%s(): output device not open", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_framebuffer_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_framebuffer_specific_t));

  /* output devices like SDL supported here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("FRAMEBUFFER", dispname))
    dd->dsp_id = DISPID_FRAMEBUFFER;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_framebuffer.c", dispname);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_OUT;

  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 0;

  /* per display settings */

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 0;  /* unused, bbox is used for optimisation */

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 0;         /* start with backlight on */

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_RGB444 | SD_CS_RGB565;
  dd->colour_spaces    |= SD_CS_TRUECOLOUR | SD_CS_ATRUECOLOUR;
  dd->colour_spaces    |= SD_CS_SELFEMITTING;

  dd->fp_init           = &serdisp_framebuffer_init;
  dd->fp_update         = &serdisp_framebuffer_update;
  dd->fp_clear          = &serdisp_framebuffer_clear;
  dd->fp_close          = &serdisp_framebuffer_close;
  dd->fp_setoption      = &serdisp_framebuffer_setoption;
  dd->fp_getvalueptr    = &serdisp_framebuffer_getvalueptr;

  dd->fp_setsdpixel     = &serdisp_framebuffer_setsdpixel;
  dd->fp_getsdpixel     = &serdisp_framebuffer_getsdpixel;

  dd->fp_freeresources  = &serdisp_framebuffer_freeresources;

  serdisp_framebuffer_internal_getStruct(dd)->report_damage = 0; /* default: don't report damage info */

  serdisp_setupstructinfos(dd, 0, 0, serdisp_framebuffer_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  fb_success = 1;

  /* no framebuffer device in option string: set default device */
  if ( ! serdisp_framebuffer_internal_getStruct(dd)->fb_devname ||
         strlen(serdisp_framebuffer_internal_getStruct(dd)->fb_devname) <= 1
     ) {
    if ( (serdisp_framebuffer_internal_getStruct(dd)->fb_devname = (char*) sdtools_malloc( strlen( FB_DEFAULT_DEVICE )+1) ) != 0) {
      strcpy(serdisp_framebuffer_internal_getStruct(dd)->fb_devname, FB_DEFAULT_DEVICE);
    }
  }

  if (serdisp_framebuffer_internal_getStruct(dd)->fb_devname && strlen(serdisp_framebuffer_internal_getStruct(dd)->fb_devname) > 0) {
    serdisp_framebuffer_internal_getStruct(dd)->fbhd = open(serdisp_framebuffer_internal_getStruct(dd)->fb_devname, O_RDWR);

    if ( ! serdisp_framebuffer_internal_getStruct(dd)->fbhd ) {
      sd_error(SERDISP_EACCES, "framebuffer device cannot be opened: %s", strerror(errno));
      fb_success = 0;
    }

    if (fb_success) {
      /* get fixed screen information */
      if (ioctl(serdisp_framebuffer_internal_getStruct(dd)->fbhd, FBIOGET_FSCREENINFO, &finfo)) {
        sd_error(SERDISP_ERUNTIME, "unable to receive fixed screen information");
        fb_success = 0;
      }
    }
    if (fb_success) {
      /* get variable screen information */
      if (ioctl(serdisp_framebuffer_internal_getStruct(dd)->fbhd, FBIOGET_VSCREENINFO, &vinfo)) {
        sd_error(SERDISP_ERUNTIME, "unable to receive variable screen information");
        fb_success = 0;
      }
    }

    if (fb_success) {
      /* check if colour mode is supported */
      if (! (vinfo.bits_per_pixel == 16 || vinfo.bits_per_pixel == 24 || vinfo.bits_per_pixel == 32) ) {
        sd_error(SERDISP_ERUNTIME, "unsupported colour depth (%d)", vinfo.bits_per_pixel);
        fb_success = 0;
      }
    }

    dd->scrbuf_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel/8;

    if (fb_success) {
      /* map framebuffer device to memory */
      serdisp_framebuffer_internal_getStruct(dd)->fbmmap = (char*) mmap(
        0,
        dd->scrbuf_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        serdisp_framebuffer_internal_getStruct(dd)->fbhd,
        0
      );

      if (serdisp_framebuffer_internal_getStruct(dd)->fbmmap == MAP_FAILED || (! serdisp_framebuffer_internal_getStruct(dd)->fbmmap)) {
        sd_error(SERDISP_ERUNTIME, "unable to map framebuffer device to memory");
        fb_success = 0;
      }
      vinfo.yoffset = 0;
      ioctl(serdisp_framebuffer_internal_getStruct(dd)->fbhd,FBIOPAN_DISPLAY,&vinfo);

      /*ioctl(serdisp_framebuffer_internal_getStruct(dd)->fbhd, FBIOBLANK, FB_BLANK_UNBLANK);*/
    }
  } else {
    fb_success = 0;
  }


  if (! fb_success) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  sd_debug(2, "%s(): detected display information: w/h: %d/%d   bpp: %d",
              __func__, vinfo.xres, vinfo.yres, vinfo.bits_per_pixel
  );

  /* enable damage report? */
  switch (serdisp_framebuffer_internal_getStruct(dd)->report_damage) {
    case 1: /* automatic detection */
      if (strncasecmp(finfo.id, "udlfb", 16) == 0) {
        serdisp_framebuffer_internal_getStruct(dd)->fb_typeid = 1;
        sd_debug(2, "%s(): auto detected udlfb-based framebuffer device", __func__);
      } else {  /* not supported / not detected */
        serdisp_framebuffer_internal_getStruct(dd)->report_damage = 0;
        serdisp_framebuffer_internal_getStruct(dd)->fb_typeid = 0;
      }
      break;
    case 2: /* force to udlfb */
      serdisp_framebuffer_internal_getStruct(dd)->fb_typeid = 1;
      break;
    default:
      serdisp_framebuffer_internal_getStruct(dd)->report_damage = 0;
  }

  dd->width  = vinfo.xres;
  dd->height = vinfo.yres;
  dd->depth  = (byte) vinfo.bits_per_pixel;

  /* default index offsets */
  serdisp_framebuffer_internal_getStruct(dd)->idx_lot[0] = 0;
  serdisp_framebuffer_internal_getStruct(dd)->idx_lot[1] = 1;
  serdisp_framebuffer_internal_getStruct(dd)->idx_lot[2] = 2;
  serdisp_framebuffer_internal_getStruct(dd)->idx_lot[3] = 3;
  /* if big endian: adapt look up table for index offsets */
  if (dd->sdcd->flags.endian) {
    switch (dd->depth) {
      case 16:
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[0] = 1;
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[1] = 0;
        break;
      case 24:
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[0] = 2;
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[1] = 1;
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[2] = 0;
        break;
      default: /* case 32 */
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[0] = 3;
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[1] = 2;
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[2] = 1;
        serdisp_framebuffer_internal_getStruct(dd)->idx_lot[3] = 0;
    }
  }

/* only GPIs required, thus execute the whole gpevent-stuff only if libpthread is available */
#ifdef HAVE_LIBPTHREAD
  serdisp_framebuffer_internal_getStruct(dd)->touchdev.fd = -1;

  /* only one GPI (touchscreen) is supported: so activate events only if touchscreen device is set */
  if (serdisp_framebuffer_internal_getStruct(dd)->ts_devname) {
    int rc = SDTOUCH_idev_open (
      serdisp_framebuffer_internal_getStruct(dd)->ts_devname,
      &(serdisp_framebuffer_internal_getStruct(dd)->touchdev)
    );

    if(rc == 0) {
      /* add gpevset */
      if (! (dd->gpevset = (SDGP_gpevset_t*) sdtools_malloc( sizeof(SDGP_gpevset_t)) )) {
        sd_debug(0, 
          "%s(): cannot allocate memory for general purpose event set. continuing without support for it ...",
          __func__
        );
        SDTOUCH_idev_close( &(serdisp_framebuffer_internal_getStruct(dd)->touchdev) );
      } else {
        memset(dd->gpevset, 0, sizeof(SDGP_gpevset_t));

        /* add GPIs */
        dd->gpevset->gpis = sdtools_malloc(sizeof(serdisp_framebuffer_GPIs));
        if (dd->gpevset->gpis) {
          memcpy(dd->gpevset->gpis, serdisp_framebuffer_GPIs, sizeof(serdisp_framebuffer_GPIs));
          dd->gpevset->amountgpis = sizeof(serdisp_framebuffer_GPIs)/sizeof(SDGPI_t);

          /* function pointers */
          dd->gpevset->fp_hnd_gpi_enable =  &serdisp_framebuffer_gpi_enable;
          dd->gpevset->fp_evlp_receiver =   &serdisp_framebuffer_evlp_receiver;
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
  * \brief   initialise framebuffer driver
  *
  * initialises framebuffer driver
  *
  * \param   dd             display descriptor
  *
  * \since   1,98
  */
void serdisp_framebuffer_init(serdisp_t* dd) {
  dd->bbox[0] = dd->width-1;
  dd->bbox[1] = dd->height-1;
  dd->bbox[2] = 0;
  dd->bbox[3] = 0;


#ifdef HAVE_LIBPTHREAD
  /* no need to autostart event loop if the sole GPI isn't enabled */
  if (dd->gpevset) {
    dd->gpevset->evlp_noautostart = 1;
  }
#endif

  sd_debug(2, "%s(): done with initialising", __func__);
}

/* defines for different damage processing calls needed by _update() */
#define DLFB_IOCTL_REPORT_DAMAGE 0xAA

/**
  * \brief   update framebuffer output
  *
  * updates the framebuffer output using display-buffer scrbuf+scrbuf_chg using a time-saving algorithm.
  *
  * \param   dd             display descriptor
  *
  * \since   1.98
  */
void serdisp_framebuffer_update(serdisp_t* dd) {
  char* fbmmap = (char*)serdisp_framebuffer_internal_getStruct(dd)->fbmmap;
  int chunksize = (dd->bbox[2] - dd->bbox[0] + 1) * (dd->depth >> 3);
  int pregap = dd->bbox[0] * (dd->depth >> 3);
  int linesize = dd->width * (dd->depth >> 3);
  int startdata = dd->bbox[1] * linesize;
  int i;

  for (i = dd->bbox[1]; i <= dd->bbox[3]; i++) {
    memcpy(fbmmap+startdata+pregap, dd->scrbuf+startdata+pregap, chunksize);
    startdata+=linesize;
  }

  /* damage reporting */
  if (serdisp_framebuffer_internal_getStruct(dd)->report_damage > 0) {
    struct fb_rect {
      int x; int y; int w; int h;
    } damage = {0, 0, 0, 0};
    damage.x = dd->bbox[0];
    damage.y = dd->bbox[1];
    damage.w = dd->bbox[2] - dd->bbox[0] + 1;
    damage.h = dd->bbox[3] - dd->bbox[1] + 1;

    if (serdisp_framebuffer_internal_getStruct(dd)->fb_typeid == 1) { /* udlfb */
      ioctl(serdisp_framebuffer_internal_getStruct(dd)->fbhd, DLFB_IOCTL_REPORT_DAMAGE, &damage);
    } else if (serdisp_framebuffer_internal_getStruct(dd)->fb_typeid == 2) { /* ugly */
      byte buf[3] = "\n";
      write(serdisp_framebuffer_internal_getStruct(dd)->fbhd, buf, 2);
    }
  }

  /* reset bounding box */
  dd->bbox[0] = dd->width-1;
  dd->bbox[1] = dd->height-1;
  dd->bbox[2] = 0;
  dd->bbox[3] = 0;
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
int serdisp_framebuffer_setoption(serdisp_t* dd, const char* option, long value) {
    /* option not found here: try generic one in calling serdisp_setoption(); */
  if (serdisp_compareoptionnames(dd, option, "FBDEVICE") ) {
    serdisp_framebuffer_internal_getStruct(dd)->fb_devname = (char*) sdtools_malloc( strlen( (char*) value )+1);
    strcpy(serdisp_framebuffer_internal_getStruct(dd)->fb_devname, (char*)value);
  } else if (serdisp_compareoptionnames(dd, option, "TOUCHDEVICE") ) {
    serdisp_framebuffer_internal_getStruct(dd)->ts_devname = (char*) sdtools_malloc( strlen( (char*) value )+1);
    strcpy(serdisp_framebuffer_internal_getStruct(dd)->ts_devname, (char*)value);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}


/**
  * \brief   close (switch off) framebuffer
  *
  * closes framebuffer device
  *
  * \param   dd             display descriptor
  * 
  * \since   1.98
  */
void serdisp_framebuffer_close(serdisp_t* dd) {
#ifdef HAVE_LIBPTHREAD
  SDTOUCH_idev_close( &(serdisp_framebuffer_internal_getStruct(dd)->touchdev) );
#endif
  munmap(serdisp_framebuffer_internal_getStruct(dd)->fbmmap, dd->scrbuf_size );
  close(serdisp_framebuffer_internal_getStruct(dd)->fbhd);
}

/**
  * \brief   clear framebuffer output
  *
  * clears framebuffer output
  *
  * \param   dd             display descriptor
  * 
  * \since   1.98
  */
void serdisp_framebuffer_clear(serdisp_t* dd) {
  memset(dd->scrbuf, 0, dd->scrbuf_size);
  /* set the whole scrbuf -> dirty */
  dd->bbox[0] = 0;
  dd->bbox[1] = 0;
  dd->bbox[2] = dd->width-1;
  dd->bbox[3] = dd->height-1;
  serdisp_framebuffer_update(dd);
}

/**
  * \brief   changes a pixel into the display buffer
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   sdcol         monochrome: 0: clear (white), <>0: set (black); else: grey value (supported depths: 1, 2, 4)
  */
void serdisp_framebuffer_setsdpixel (serdisp_t* dd, int x, int y, SDCol sdcol) {
  int x_i = 0, y_i = 0;
  uint8_t* lot = serdisp_framebuffer_internal_getStruct(dd)->idx_lot;

  if (dd->curr_rotate <= 1) {
    if (x >= dd->width || y >= dd->height || x < 0 || y < 0)
      return;
  } else {
    if (x >= dd->height || y >= dd->width || x < 0 || y < 0)
      return;
  }

  switch (dd->curr_rotate) {
    case 0:  /* 0 degrees */
      x_i = x;
      y_i = y;
      break;
    case 1:  /* 180 degrees */
      x_i = (dd->width  - 1 - x);
      y_i = (dd->height - 1 - y);
      break;
    case 2:  /* 90 degrees */
      x_i = y;
      y_i = (dd->height - 1 - x);
      break;
    case 3:  /* 270 degrees */
      x_i = (dd->width  - 1 - y);
      y_i = x;
      break;
  }

  int changed = 0;
  int idx_2 = ((x_i  + y_i * dd->width) * (dd->depth << 1)) >> 3;
  int idx = idx_2 >> 1;

  switch(dd->depth) {
    case 16:
      if ( dd->scrbuf[idx + lot[1]] != ((sdcol & 0xFF00) >> 8) ) {
        dd->scrbuf[idx + lot[1]] = ((sdcol & 0xFF00) >> 8);
        changed = 1;
      }
      if ( dd->scrbuf[idx + lot[0]] != (sdcol & 0xFF) ) {
        dd->scrbuf[idx + lot[0]] = (sdcol & 0x00FF);
        changed = 1;
      }
      break;
    case 24:
      if ( dd->scrbuf[idx + lot[2]] != ((sdcol & 0xFF0000) >> 16) ) {
        dd->scrbuf[idx + lot[2]] = ((sdcol & 0xFF0000) >> 16);
        changed = 1;
      }
      if ( dd->scrbuf[idx + lot[1]] != ((sdcol & 0x00FF00) >> 8) ) {
        dd->scrbuf[idx + lot[1]] = ((sdcol & 0x00FF00) >> 8);
        changed = 1;
      }
      if ( dd->scrbuf[idx + lot[0]] != (sdcol & 0x0000FF) ) {
        dd->scrbuf[idx + lot[0]] = (sdcol & 0x0000FF);
        changed = 1;
      }
      break;
    case 32:
      if ( dd->scrbuf[idx + lot[3]] != ((sdcol & 0xFF000000) >> 24) ) {
        dd->scrbuf[idx + lot[3]] = ((sdcol & 0xFF000000) >> 24);
        changed = 1;
      }
      if ( dd->scrbuf[idx + lot[2]] != ((sdcol & 0x00FF0000) >> 16) ) {
        dd->scrbuf[idx + lot[2]] = ((sdcol & 0x00FF0000) >> 16);
        changed = 1;
      }
      if ( dd->scrbuf[idx + lot[1]] != ((sdcol & 0x0000FF00) >> 8) ) {
        dd->scrbuf[idx + lot[1]] = ((sdcol & 0x0000FF00) >> 8);
        changed = 1;
      }
      if ( dd->scrbuf[idx + lot[0]] != (sdcol & 0x0000FF) ) {
        dd->scrbuf[idx + lot[0]] = (sdcol & 0x000000FF);
        changed = 1;
      }
      break;
  }

  if (changed) {
    /* bounding box changed? */
    if (x_i < dd->bbox[0]) dd->bbox[0] = x_i;
    if (y_i < dd->bbox[1]) dd->bbox[1] = y_i;
    if (x_i > dd->bbox[2]) dd->bbox[2] = x_i;
    if (y_i > dd->bbox[3]) dd->bbox[3] = y_i;
  }
}


/**
  * \brief   gets a pixel from the display buffer
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \return  hardware-dependent grey-level or colour information at (x/y)
  */
SDCol serdisp_framebuffer_getsdpixel (serdisp_t* dd, int x, int y) {
  int x_i = 0, y_i = 0;
  SDCol value = 0;
  uint8_t* lot = serdisp_framebuffer_internal_getStruct(dd)->idx_lot;

  if (dd->curr_rotate <= 1) {
    if (x >= dd->width || y >= dd->height || x < 0 || y < 0)
      return 0;
  } else {
    if (x >= dd->height || y >= dd->width || x < 0 || y < 0)
      return 0;
  }

  switch (dd->curr_rotate) {
    case 0:  /* 0 degrees */
      x_i = x;
      y_i = y;
      break;
    case 1:  /* 180 degrees */
      x_i = (dd->width  - 1 - x);
      y_i = (dd->height - 1 - y);
      break;
    case 2:  /* 90 degrees */
      x_i = y;
      y_i = (dd->height - 1 - x);
      break;
    case 3:  /* 270 degrees */
      x_i = (dd->width  - 1 - y);
      y_i = x;
      break;
  }

  int idx_2 = ((x_i  + y_i * dd->width) * (dd->depth << 1)) >> 3;
  int idx = idx_2 >> 1;


  switch(dd->depth) {
    case 16:
      value = (SDCol)((dd->scrbuf[idx+lot[1]] << 8) + dd->scrbuf[idx+lot[0]]);
      break;
    case 24:
      value = (SDCol)((dd->scrbuf[idx+lot[2]] << 16) | (dd->scrbuf[idx+lot[1]] << 8) | dd->scrbuf[idx+lot[0]]);
      break;
    case 32:
      value = (SDCol)((dd->scrbuf[idx+lot[3]] << 24) | (dd->scrbuf[idx+lot[2]] << 16) | (dd->scrbuf[idx+lot[1]] << 8) | dd->scrbuf[idx+lot[0]]);
      break;
  }

  return value;
}



#ifdef HAVE_LIBPTHREAD
/* *********************************
   int serdisp_framebuffer_gpi_enable(dd, gpid, enable)
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
int serdisp_framebuffer_gpi_enable (serdisp_t* dd, byte gpid, int enable) {
  if (!dd || !dd->gpevset || gpid >= dd->gpevset->amountgpis)
    return -1;

  dd->gpevset->gpis[gpid].enabled = enable;

  if (enable && SDEVLP_getstatus(dd) == SDEVLP_STOPPED && serdisp_framebuffer_internal_getStruct(dd)->touchdev.fd != -1) {
    SDTOUCH_idev_enable( &(serdisp_framebuffer_internal_getStruct(dd)->touchdev), 1);

    SDEVLP_start(dd);
  }

  if (!enable && SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    SDTOUCH_idev_enable( &(serdisp_framebuffer_internal_getStruct(dd)->touchdev), 0);

    SDEVLP_stop(dd);
  }

  return 0;
}


/* *********************************
   SDGP_event_t* serdisp_framebuffer_evlp_receiver(dd, recycle)
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
SDGP_event_t* serdisp_framebuffer_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle) {
  return SDTOUCH_idev_evlp_receiver(dd, &(serdisp_framebuffer_internal_getStruct(dd)->touchdev), recycle);
}
#endif /* HAVE_LIBPTHREAD */


