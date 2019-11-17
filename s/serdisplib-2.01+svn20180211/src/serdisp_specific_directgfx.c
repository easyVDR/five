/*
 *************************************************************************
 *
 * serdisp_specific_directgfx.c
 * routines for drawing content to SDL / GL / ... output
 *
 *************************************************************************
 *
 * copyright (C)  2008-2013  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
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

#if defined(HAVE_LIBPTHREAD)
  #include "serdisplib/serdisp_gpevents.h"
#endif

/* #define OPT_USEOLDUPDATEALGO */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_SDL   1


serdisp_options_t serdisp_directgfx_options[] = {
   /*  name          aliasnames     min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS", 1,    32,  2, 0,  "64k=16,4096=12,4k=12,256=8"}
  ,{  "WIDTH",       "",             1,  1024,  1, 0,  ""}
  ,{  "HEIGHT",      "",             1,  1024,  1, 0,  ""}
  ,{  "MOUSEEVENTS", "MOUSE",        0,     1,  1, 0,  "OFF=0,ON=1,NO=0,YES=1"}
/*  ,{  "FULLSCREEN",  "FS",           0,     1,  1, 0,  ""}*/
};


#ifdef HAVE_LIBPTHREAD
SDGPI_t serdisp_directgfx_GPIs[] = {
   /* id  name     aliasnames         type                enabled  file descr. or last value */
   {  0, "TOUCHSCREEN",  "TOUCH",     SDGPT_GENERICTOUCH,  0,      {0} }
};
#endif


/* internal typedefs and functions */

static void serdisp_directgfx_init      (serdisp_t*);
static void serdisp_directgfx_update    (serdisp_t*);
static int  serdisp_directgfx_setoption (serdisp_t*, const char*, long);
static void serdisp_directgfx_close     (serdisp_t*);

static int  serdisp_directgfx_freeresources (serdisp_t* dd);

#ifdef HAVE_LIBPTHREAD
static int           serdisp_directgfx_gpi_enable     (serdisp_t* dd, byte gpid, int enable);
static SDGP_event_t* serdisp_directgfx_evlp_receiver  (serdisp_t* dd, SDGP_event_t* recycle);
#endif /* HAVE_LIBPTHREAD */



typedef struct serdisp_directgfx_specific_s {
  int fullscreen;
  void* screen;
  int mouseevents;
  uint8_t old_mousemask;
  int old_x;
  int old_y;
} serdisp_directgfx_specific_t;


static serdisp_directgfx_specific_t* serdisp_directgfx_internal_getStruct(serdisp_t* dd) {
  return (serdisp_directgfx_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_directgfx_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "DEPTH")) {
    *typesize = sizeof(byte);
    return &(dd->depth);
  } else if (serdisp_compareoptionnames(dd, optionname, "FULLSCREEN")) {
    *typesize = sizeof(int);
    return &(serdisp_directgfx_internal_getStruct(dd)->fullscreen);
  } else if (serdisp_compareoptionnames(dd, optionname, "MOUSEEVENTS")) {
    *typesize = sizeof(int);
    return &(serdisp_directgfx_internal_getStruct(dd)->mouseevents);
  }
  return 0;
}



/* callback-function for memory-deallocations not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_directgfx_freeresources (serdisp_t* dd) {
#ifdef HAVE_LIBPTHREAD
  if (dd->gpevset && dd->gpevset->gpis)  {
    free(dd->gpevset->gpis);
  }
#endif
  return 0;
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_directgfx_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_directgfx_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBSDL) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libSDL is not loaded.", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_directgfx_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_directgfx_specific_t));

  /* output devices like SDL supported here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("SDL", dispname))
    dd->dsp_id = DISPID_SDL;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_directgfx.c", dispname);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_OUT;


  serdisp_directgfx_internal_getStruct(dd)->fullscreen = 1;

  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 0;

  /* per display settings */

  dd->width             = 320;
  dd->height            = 240;
  dd->depth             = 32;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 0;  /* unused, bbox is used for optimisation */

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 0;         /* start with backlight on */

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_GREYSCALE | SD_CS_RGB332 | SD_CS_RGB444 | SD_CS_RGB565;
  dd->colour_spaces    |= SD_CS_TRUECOLOUR | SD_CS_ATRUECOLOUR;
  dd->colour_spaces    |= SD_CS_SELFEMITTING;

  dd->fp_init           = &serdisp_directgfx_init;
  dd->fp_update         = &serdisp_directgfx_update;
/*  dd->fp_clear          = &serdisp_directgfx_clear;*/
  dd->fp_close          = &serdisp_directgfx_close;
  dd->fp_setoption      = &serdisp_directgfx_setoption;
  dd->fp_getvalueptr    = &serdisp_directgfx_getvalueptr;

  dd->fp_freeresources  = &serdisp_directgfx_freeresources;

  serdisp_setupstructinfos(dd, 0, 0, serdisp_directgfx_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

/* only GPIs required, thus execute the whole gpevent-stuff only if libpthread is available */
#ifdef HAVE_LIBPTHREAD
  /* only one GPI (touchscreen) is supported: so activate events only if enabled by the user */
  if (serdisp_directgfx_internal_getStruct(dd)->mouseevents) {
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
      dd->gpevset->gpis = sdtools_malloc(sizeof(serdisp_directgfx_GPIs));
      if (dd->gpevset->gpis) {
        memcpy(dd->gpevset->gpis, serdisp_directgfx_GPIs, sizeof(serdisp_directgfx_GPIs));
        dd->gpevset->amountgpis = sizeof(serdisp_directgfx_GPIs)/sizeof(SDGPI_t);

        /* function pointers */
        dd->gpevset->fp_hnd_gpi_enable =  &serdisp_directgfx_gpi_enable;
        dd->gpevset->fp_evlp_receiver =   &serdisp_directgfx_evlp_receiver;
      }
    }
  }
#endif /* HAVE_LIBPTHREAD */

  return dd;
}



/* *********************************
   void serdisp_directgfx_init(dd)
   *********************************
   initialise
   *********************************
   dd     ... display descriptor
*/
void serdisp_directgfx_init(serdisp_t* dd) {
  char caption[40];
  SDL_Surface* screen;

  if (fp_SDL_Init(SDL_INIT_VIDEO) != 0) {
    sd_error(SERDISP_ENOTSUP, "%s(): unable to initialise SDL: %s", __func__, fp_SDL_GetError());
    sd_runtimeerror = 1;
    fp_SDL_Quit();
    return;
  }

  screen = serdisp_directgfx_internal_getStruct(dd)->screen = 
    (void*)fp_SDL_SetVideoMode(dd->width, dd->height, 0 /*sdl_depth*/, SDL_HWSURFACE | SDL_HWACCEL);

  if (screen == NULL) {
    sd_error(SERDISP_ENOTSUP, "%s(): unable to set video mode: %s", __func__, fp_SDL_GetError());
    sd_runtimeerror = 1;
    fp_SDL_Quit();
    return;
  }

  snprintf(caption, sizeof(caption)-1, "serdisplib::SDL %dx%dx%d", screen->w, screen->h, screen->format->BitsPerPixel);
  fp_SDL_WM_SetCaption(caption, NULL);

#ifdef HAVE_LIBPTHREAD
  /* no need to autostart event loop if the sole GPI isn't enabled */
  if (dd->gpevset) {
    dd->gpevset->evlp_noautostart = 1;
  }
#endif

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_directgfx_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_directgfx_update(serdisp_t* dd) {
  int x, y;

  SDL_Surface* screen = (SDL_Surface*)serdisp_directgfx_internal_getStruct(dd)->screen;
  uint8_t* bufp8;
  uint16_t* bufp16;
  uint32_t* bufp32;
  uint32_t colour;
  uint32_t colour_raw;
  byte ashift = screen->format->Ashift/8;
  byte rshift = screen->format->Rshift/8;
  byte gshift = screen->format->Gshift/8;
  byte bshift = screen->format->Bshift/8;

  if(dfn_SDL_MUSTLOCK( screen ) )
    fp_SDL_LockSurface( screen );

  if (screen->format->BitsPerPixel == dd->depth && dd->depth >= 24) {
    int idx;
    uint8_t* ddbuf = (uint8_t*)dd->scrbuf;
    bufp8 = (uint8_t*)screen->pixels;
    int xstep = (dd->depth == 24) ? 3 : 4;

    int idx_start = 0;
    int idx_end = dd->scrbuf_size;
#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */
#else
    int idx_limit = 0;
    int idx_border = 0;

    if (dd->scrbuf_chg) {
      idx_start = dd->scrbuf_size;
      idx_end = 0;

      idx = 0;
      idx_border = 0;
      while (idx < dd->scrbuf_chg_size) {
        if (dd->scrbuf_chg[idx]) {
          idx_start = idx_border;
          idx_limit = idx;
          idx = dd->scrbuf_chg_size;
        } else {
          idx++;
          idx_border += xstep << 3;
        }
      }
      idx = dd->scrbuf_chg_size - 1;
      idx_border = (idx << 3) * xstep + (xstep << 3) - 1;
      while (idx >= 0 && idx > idx_limit) {
        if (dd->scrbuf_chg[idx]) {
          idx_end = idx_border;
          idx = -1;
        } else {
          idx--;
          idx_border -= xstep << 3;
        }
      }
    }
/*    fprintf(stderr, "idx_start: %d   idx_end: %d\n", idx_start, idx_end);*/
#endif /* OPT_USEOLDUPDATEALGO */
    if (dd->depth == 24) {
      for (idx = idx_start; idx < idx_end; idx += xstep) {
         *(bufp8 + idx + rshift) = *(ddbuf + idx);
         *(bufp8 + idx + gshift) = *(ddbuf + idx + 1);
         *(bufp8 + idx + bshift) = *(ddbuf + idx + 2);

         if (dd->curr_invert) {
           *(bufp8 + idx + rshift) = ~ *(bufp8 + idx + rshift);
           *(bufp8 + idx + gshift) = ~ *(bufp8 + idx + gshift);
           *(bufp8 + idx + bshift) = ~ *(bufp8 + idx + bshift);
         }
      }
    } else {
      for (idx = idx_start; idx < idx_end; idx += xstep) {
         *(bufp8 + idx + ashift) = *(ddbuf + idx);
         *(bufp8 + idx + rshift) = *(ddbuf + idx + 1);
         *(bufp8 + idx + gshift) = *(ddbuf + idx + 2);
         *(bufp8 + idx + bshift) = *(ddbuf + idx + 3);

         if (dd->curr_invert) {
           *(bufp8 + idx + rshift) = ~ *(bufp8 + idx + rshift);
           *(bufp8 + idx + gshift) = ~ *(bufp8 + idx + gshift);
           *(bufp8 + idx + bshift) = ~ *(bufp8 + idx + bshift);
         }
      }
    }
  } else {
    /* unset curr_rotate so that it will not interfere with serdisp_setsdcol() */
    int temp_rotate = dd->curr_rotate;
    dd->curr_rotate = 0;

    for (y = 0; y < dd->height; y++) {
      for (x = 0; x < dd->width; x++) {
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
        colour_raw = serdisp_getsdcol(dd, x, y);
#else
        colour_raw = (uint32_t)serdisp_getcolour(dd, x, y);
#endif

        if (dd->curr_invert)
          colour_raw = (colour_raw & 0xFF000000) | ~(colour_raw & 0x00FFFFFF);

        colour = fp_SDL_MapRGB(
          screen->format,
          (colour_raw & 0x00FF0000) >> 16,
          (colour_raw & 0x0000FF00) >>  8,
          (colour_raw & 0x000000FF)
        );

        switch(screen->format->BytesPerPixel) {
          case 1: {
            bufp8 = (uint8_t*)screen->pixels + y * screen->pitch + x;
            *bufp8 = (uint8_t)colour;
          }
          break;
          case 2: {
            bufp16 = (uint16_t*)screen->pixels + y * screen->pitch/2 + x;
            *bufp16 = (uint16_t)colour;
          }
          break;
          case 3: {
            bufp8 = (uint8_t*)screen->pixels + y * screen->pitch + x * 3;
            *(bufp8 + rshift) = (colour_raw & 0x00FF0000) >> 16;
            *(bufp8 + gshift) = (colour_raw & 0x0000FF00) >>  8;
            *(bufp8 + bshift) = (colour_raw & 0x000000FF);
          }
          break;
          case 4: {
            bufp32 = (uint32_t*)screen->pixels + y * screen->pitch/4 + x;
            *bufp32 = (uint32_t)colour;
          }
          break;
        }
      }
    }
    /* re-enable curr_rotate */
    dd->curr_rotate = temp_rotate;
  }

#ifndef OPT_USEOLDUPDATEALGO
  memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
#endif

  if(dfn_SDL_MUSTLOCK( screen ) )
    fp_SDL_UnlockSurface( screen );

  fp_SDL_Flip(screen);
}



/* *********************************
   int serdisp_directgfx_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_directgfx_setoption(serdisp_t* dd, const char* option, long value) {
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


/* *********************************
   void serdisp_directgfx_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_directgfx_close(serdisp_t* dd) {
  fp_SDL_FreeSurface((SDL_Surface*)(serdisp_directgfx_internal_getStruct(dd)->screen));
  fp_SDL_Quit();
}



#ifdef HAVE_LIBPTHREAD
/* *********************************
   int serdisp_directgfx_gpi_enable(dd, gpid, enable)
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
int serdisp_directgfx_gpi_enable (serdisp_t* dd, byte gpid, int enable) {
  if (!dd || !dd->gpevset || gpid >= dd->gpevset->amountgpis)
    return -1;

  dd->gpevset->gpis[gpid].enabled = enable;

  if (enable && SDEVLP_getstatus(dd) == SDEVLP_STOPPED) {
    SDL_Event sdl_event;
    while(fp_SDL_PollEvent(&sdl_event));  /* empty queue */
    SDEVLP_start(dd);
  }

  if (!enable && SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    SDEVLP_stop(dd);
  }

  return 0;
}
#endif


#ifdef HAVE_LIBPTHREAD
/* *********************************
   SDGP_event_t* serdisp_directgfx_evlp_receiver(dd, recycle)
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
SDGP_event_t* serdisp_directgfx_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle) {
  SDGP_event_t*  event = 0;
  SDGP_evpkt_generictouch_t  generictouch;
  uint8_t old_mousemask = serdisp_directgfx_internal_getStruct(dd)->old_mousemask;
  int old_x = serdisp_directgfx_internal_getStruct(dd)->old_x;
  int old_y = serdisp_directgfx_internal_getStruct(dd)->old_y;

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return 0;

  if (SDGPI_isenabled(dd, 0)) {
    uint8_t curr_mousemask;
    int scroll = 0;  /* down: 1, up: 2 */
    int x = 0, y = 0;
    int validevent = 1;

    SDL_Event sdl_event;
    //fp_SDL_PumpEvents();
    fp_SDL_WaitEvent(&sdl_event); /* blocking wait */
    curr_mousemask = fp_SDL_GetMouseState(&x, &y);
    switch(sdl_event.type) {
      case SDL_MOUSEBUTTONDOWN:
        generictouch.type = SDGPT_TOUCHDOWN;
        switch(sdl_event.button.button) {
          case SDL_BUTTON_WHEELDOWN:
            scroll = 1;
            break;
          case SDL_BUTTON_WHEELUP:
            scroll = 2;
            break;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        generictouch.type = SDGPT_TOUCHUP;
        switch(sdl_event.button.button) {
          case SDL_BUTTON_WHEELDOWN:
            scroll = 1;
            break;
          case SDL_BUTTON_WHEELUP:
            scroll = 2;
            break;
        }
        break;
      case SDL_MOUSEMOTION:
        generictouch.type = SDGPT_TOUCHMOVE;
        break;
      default:
        validevent = 0;
    }

    if (validevent && (scroll || curr_mousemask != old_mousemask || x != old_x || y != old_y)) {
#if 0
      if (curr_mousemask && curr_mousemask == old_mousemask) {
        generictouch.type = SDGPT_TOUCHMOVE;
      } else if (curr_mousemask && !old_mousemask) {
        generictouch.type = SDGPT_TOUCHDOWN;
      } else {
        generictouch.type = SDGPT_TOUCHUP;
      }
#endif
      generictouch.flags = 0x01;  /* flag: binary touch, no raw information included */
      generictouch.norm_x = x;
      generictouch.norm_y = y;
      generictouch.norm_touch = 
        /* switch SDL mouse button order from left, middle, right to left, right, middle */
        ((curr_mousemask & 0x0001 ) ? 0x01 : 0x00) |
        ((curr_mousemask & 0x0002 ) ? 0x04 : 0x00) |
        ((curr_mousemask & 0x0004 ) ? 0x02 : 0x00);
      if (scroll) {
        generictouch.type = SDGPT_TOUCHSCROLL;
        generictouch.norm_touch |= (scroll-1) << 4;
      }

      serdisp_directgfx_internal_getStruct(dd)->old_mousemask = curr_mousemask;
      serdisp_directgfx_internal_getStruct(dd)->old_x = x;
      serdisp_directgfx_internal_getStruct(dd)->old_y = y;
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
      event->type = SDGPT_GENERICTOUCH;
      gettimeofday(&event->timestamp, 0);
      memcpy(&event->data, &generictouch, sizeof(SDGP_evpkt_generictouch_t));

#if 0
        /* convert header and payload to network byte order */
        SDGPT_event_header_hton(event);
        SDGPT_event_payload_hton(stream, sizeof(stream), (byte)sizeof(int16_t));


        /* convert header and payload back to host byte order */
        SDGPT_event_header_ntoh(event);
        SDGPT_event_payload_ntoh(stream, sizeof(stream), (byte)sizeof(int16_t));
#endif
      return event;
#if 0
    } else {
      usleep(500);
#endif
    }
  }

  return 0;
}
#endif /* HAVE_LIBPTHREAD */
