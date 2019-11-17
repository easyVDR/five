/*
 *************************************************************************
 *
 * serdisp_specific_rs232.c
 * routines for controlling displays that are driven using RS232
 *
 *************************************************************************
 *
 * copyright (C) 2009-2017  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 * testing and prototype support for omaura display contributed by:
 *           (C)      2009  Nico Ehinger

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
#include <unistd.h>     /* write() */

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"


/* #define OPT_USEOLDUPDATEALGO */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_EAKIT128GXT   1
#define DISPID_OMAURA        9

serdisp_options_t serdisp_eakit128gxt_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
  ,{  "BAUDRATE",  "BAUD",     1,   5,  1, 0,  "1200=1,2400=2,4800=3,9600=4,19200=5"}
};


/* internal typedefs and functions */

static void serdisp_rs232_init       (serdisp_t*);
static void serdisp_rs232_update     (serdisp_t*);
static int  serdisp_rs232_setoption  (serdisp_t*, const char*, long);
static void serdisp_rs232_clear      (serdisp_t*);
static void serdisp_rs232_close      (serdisp_t*);

static void serdisp_omaura_update    (serdisp_t*);

static void write_byte               (serdisp_t*, byte);
static void do_commit                (serdisp_t*);


void write_byte (serdisp_t* dd, byte item) {
  if (! (dd->dsp_id == DISPID_OMAURA) ) {
    SDCONN_writedelay(dd->sdcd, item, 0, dd->delay);
  } else {
    write(dd->sdcd->fd, &item, 1);
  }
}


void do_commit (serdisp_t* dd) {
  if (! (dd->dsp_id == DISPID_OMAURA) ) {
    SDCONN_commit(dd->sdcd);
  }
}

/* callback-function for setting non-standard options */
static void* serdisp_rs232_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "BAUDRATE")) {
    *typesize = sizeof(unsigned int);
    return &(dd->sdcd->rs232.baudrate);
  }
  return 0;
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_rs232_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_rs232_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;
  byte infobuffer[16];
  int rc;


  if (! sdcd ) {
    sd_error(SERDISP_EMALLOC, "%s(): output device not open", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));


  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("EAKIT128GXT", dispname))
    dd->dsp_id = DISPID_EAKIT128GXT;
  else if (serdisp_comparedispnames("OMAURA", dispname))
    dd->dsp_id = DISPID_OMAURA;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_rs232.c", dispname);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_RS232;

  /* force device-check at this point because _setup already does some communication with the display */
  if (! (dd->connection_types & dd->sdcd->conntype)) {
    sd_error(SERDISP_EDEVNOTSUP, "'%s' only supports 'RS232' as connection type (try using 'RS232:<device>')", dispname);
    free(dd->specific_data);
    free(dd);
    return (serdisp_t*)0;
  }

  /* per display settings */

  dd->width             = 128;
  dd->height            = 64;
  dd->depth             = 1;
  dd->min_contrast      = 1 /*0x0*/;
  dd->max_contrast      = 9 /*0x15*/;

  dd->feature_contrast  = 0;
  dd->feature_invert    = 0;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 6;

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 1;         /* start with backlight on */

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_GREYSCALE;

  dd->fp_init           = &serdisp_rs232_init;
  dd->fp_update         = &serdisp_rs232_update;
  dd->fp_clear          = &serdisp_rs232_clear;
  dd->fp_close          = &serdisp_rs232_close;
  dd->fp_setoption      = &serdisp_rs232_setoption;
  dd->fp_getvalueptr    = &serdisp_rs232_getvalueptr;

  dd->sdcd->rs232.baudrate = B1200;

  /* requesting ID for omaura display */
  if (dd->dsp_id == DISPID_OMAURA) {
#ifdef B500000
    /* set RS232 parameters (will be set / initialised by SDCONN_confinit() */
    dd->sdcd->rs232.baudrate = B500000;
    dd->sdcd->rs232.c_cread = 1;   /* enable receiver */
    dd->sdcd->rs232.c_local = 1;   /* enable local mode */

    dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
    dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;

    write_byte (dd, 0x70);  /* request display information */
    write_byte (dd, 0x71);  /* param: 0x00 */
    do_commit  (dd);
    SDCONN_usleep(dd->sdcd, 5000);

    rc = SDCONN_readstream(dd->sdcd, infobuffer, 9);
    sd_debug(1, "%s(): omaura display information: bytes read: %d, signature: %7s", __func__, rc, infobuffer);

    if (strncmp((char*)infobuffer, "OLED", 4) != 0 ) {
      sd_error(SERDISP_ERUNTIME, "setup of omaura: display not found.");
      return 0;
    }
#else
    sd_error(SERDISP_ENOTSUP, "required baudrate 'B500000' not defined by your operating system.");
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
#endif /* ifdef B500000 */
  }


  switch (dd->dsp_id) {
    case DISPID_EAKIT128GXT: {
      dd->sdcd->rs232.baudrate = B1200;
      dd->sdcd->rs232.c_cstopb = 1;  /* two stop bits */
      dd->sdcd->rs232.c_rtscts = 1;  /* if not set: garbage when using higher baud rates */

      dd->min_contrast      = 10;
      dd->max_contrast      = 20;

      dd->feature_contrast  = 0;  /* setting contrast supported but display starts to flicker if changed */
      dd->feature_invert    = 1;  /* invert supported but not working as expected */
      dd->feature_backlight = 1;  /* switching backlight supported AND usable ... */

#if 0
      write_byte (dd, 0x1B);  /* ESC */
      write_byte (dd, 3);     /* gfx mode SET */
      do_commit(dd);

      dd->sdcd->rs232.baudrate = B9600;
      dd->sdcd->flags.needs_confinit = 1;
#endif

      serdisp_setupstructinfos(dd, 0, 0, serdisp_eakit128gxt_options);
    } break;
    case DISPID_OMAURA: {
      dd->width             = 256;
      dd->height            = 64;
      dd->depth             = 4;
      dd->colour_spaces     = SD_CS_SCRBUFCUSTOM | SD_CS_SELFEMITTING | SD_CS_GREYSCALE;

      dd->fp_update         = &serdisp_omaura_update;
      dd->fp_clear          = 0;  /* serdisp_rs232_clear hanging up display */

      serdisp_setupstructinfos(dd, 0, 0, 0);
    } break;
    default :
      serdisp_setupstructinfos(dd, 0, 0, 0/*serdisp_eakit128gxt_options*/);
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  if (dd->dsp_id != DISPID_OMAURA) {  /* ugly hack: corr. baudrate from [0 .. 5] -> [auto, B1200, ... B19200] */
    switch(dd->sdcd->rs232.baudrate) {
      case  1: dd->sdcd->rs232.baudrate = B1200; break;
      case  2: dd->sdcd->rs232.baudrate = B2400; break;
      case  3: dd->sdcd->rs232.baudrate = B4800; break;
      case  4: dd->sdcd->rs232.baudrate = B9600; break;
      case  5: dd->sdcd->rs232.baudrate = B19200; break;
    }
  }

  return dd;
}


/* *********************************
   void serdisp_rs232_init(dd)
   *********************************
   initialise an RS232 based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_rs232_init(serdisp_t* dd) {
  if (dd->dsp_id == DISPID_EAKIT128GXT) {
    write_byte (dd, 0x1B);  /* set display on/off; ESC D <n>   n=1: on */
    write_byte (dd, 'D');
    write_byte (dd, 1);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);

    write_byte (dd, 0x1B);  /* set output ports; ESC Y <n1> <n2>   n1=255: all n2=0: reset */
    write_byte (dd, 'Y');
    write_byte (dd, 255);
    write_byte (dd, 0);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);

    write_byte (dd, 0x1B);  /* set gfx mode; ESC V <n>   n=1: normal */
    write_byte (dd, 'V');
    write_byte (dd, 1);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);
  }
  sd_debug(2, "%s(): done with initialising", __func__);
}


/* *********************************
   void serdisp_rs232_update(dd)
   *********************************
   updates a generic RS232 based display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_rs232_update(serdisp_t* dd) {
  int i;
  byte data = 0;

#ifdef OPT_USEOLDUPDATEALGO

  write_byte(dd, 0x1B);       /* send image clip: ESC B <x> <y> <w> <h> <data ...> */
  write_byte(dd, 'B');
  write_byte(dd, 0);
  write_byte(dd, 0);
  write_byte(dd, dd->width);
  write_byte(dd, dd->height);

  for (i = 0; i < dd->scrbuf_size; i++) {
    if (dd->curr_invert /*&& !(dd->feature_invert)*/)
      data = ~data;

    data = dd->scrbuf[i];
    write_byte(dd, data);
  }

#else /* OPT_USEOLDUPDATEALGO */
  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  int x, y;
  int xt = 0, yt = 0, xb = 0, yb = 0;

  i = sdtools_calc_bbox (dd, /*0,*/ &xt, &yt, &xb, &yb);
  /*fprintf(stderr, "[%d] %3d/%3d - %3d/%3d\n", i, xt, yt, xb, yb);*/

  if (i != 0) {
    write_byte(dd, 0x1B);       /* send image clip: ESC B <x> <y> <w> <h> <data ...> */
    write_byte(dd, 'B');
    write_byte(dd, xt);
    write_byte(dd, yt);
    write_byte(dd, xb-xt+1);
    write_byte(dd, yb-yt+1);

    for (y = yt; y <= yb; y+=8) {
      for (x = xt; x <= xb; x++) {
        data = dd->scrbuf [ (y/8) * dd->width + x];

        if (dd->curr_invert /*&& !(dd->feature_invert)*/)
          data = ~data;

        write_byte(dd, data);  /* data */
        dd->scrbuf_chg[x] &= (0xFF ^ (1 << (y/8))) ;
      }
    }
  }
#endif /* OPT_USEOLDUPDATEALGO */
  do_commit(dd); /* if streaming: be sure that every data is transmitted */
}


/* *********************************
   void serdisp_omaura_update(dd)
   *********************************
   updates an omaura display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_omaura_update(serdisp_t* dd) {
  int i, j;
  byte data = 0;

#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */

  write_byte(dd, 0x70);    /* start code */
  write_byte(dd, 0x81);    /* icon code */
  do_commit(dd);

  for (j = 0; j < dd->height; j++) { 
    for (i = 0; i < dd->width >> 1; i++) {
      data = dd->scrbuf[i + (dd->height - 1 - j) * (dd->width >> 1)];
      if (dd->curr_invert)
        data = ~data;
      write_byte(dd, data);
    }
  }
  write_byte(dd, 0x00);   /* last display buffer byte == 0x00 */
  do_commit(dd);

#else /* OPT_USEOLDUPDATEALGO */
  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  int rc;
  int xt = 0, yt = 0, xb = 0, yb = 0;

  rc = sdtools_calc_bbox (dd, /*0,*/ &xt, &yt, &xb, &yb);
  /*fprintf(stderr, "[%d] %3d/%3d - %3d/%3d\n", i, xt, yt, xb, yb);*/

  if (rc != 0) {
    write_byte(dd, 0x70);             /* start code */
    write_byte(dd, 0x82);             /* icon code */
    write_byte(dd, xt>>1);            /* x/2 (0 - 127) */
    write_byte(dd, yt);               /* y   (0 - 63) */
    write_byte(dd, (xb-xt+1)>>1);     /* w/2 (0 - 128) */
    write_byte(dd, yb-yt+1);          /* h   (0 - 63) */
    write_byte(dd, 1);                /* vertical: 1 */

    for (j = yt; j <= yb; j++) {
      for (i = xt; i <= (xb >> 1); i++) {
        data = dd->scrbuf [i+ (dd->height - 1 - j) * (dd->width >> 1)];

        if (dd->curr_invert /*&& !(dd->feature_invert)*/)
          data = ~data;

        write_byte(dd, data);  /* data */
                        /*                      w / 2 / 8          */
        dd->scrbuf_chg[ (dd->height - 1 - j) * (dd->width >> 4) + (i >> 3) ] &= (0xFF ^ (1 << (i%8))) ;
      }
    }
    write_byte(dd, 0x00);   /* last display buffer byte == 0x00 */
  }
  do_commit(dd); /* if streaming: be sure that every data is transmitted */
#endif /* OPT_USEOLDUPDATEALGO */
}



/* *********************************
   int serdisp_rs232_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_rs232_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT" )) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */
    write_byte (dd, 0x1B);  /* switch bg-light on/off: ESC H <n>   n=0: off, n=1: on */
    write_byte (dd, 'H');
    write_byte (dd, dd->curr_backlight);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);
  } else if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT" )) {
    int old_invert = dd->curr_invert;
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;

    if (old_invert != dd->curr_invert) {  /* only invert if old inv. status != new inv. status */
      write_byte (dd, 0x1B);  /* invert display: ESC D \3 */
      write_byte (dd, 'D');
      write_byte (dd, 3);
      do_commit(dd);
      SDCONN_usleep(dd->sdcd, 100000);
    }
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}


/* *********************************
   void serdisp_rs232_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_rs232_clear(serdisp_t* dd) {
  if (dd->dsp_id == DISPID_OMAURA) {
    write_byte (dd, 0x70);    /* start code */
    write_byte (dd, 0x71);    /* clear code */
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);
  } else {
    write_byte (dd, 0x1B);  /* clear display off: ESC D \1 */
    write_byte (dd, 'D');
    write_byte (dd, 2);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);

    if ( dd->curr_invert ) {
      write_byte (dd, 0x1B);  /* invert display: ESC D \3 */
      write_byte (dd, 'D');
      write_byte (dd, 3);
      do_commit(dd);
      SDCONN_usleep(dd->sdcd, 100000);
    }
  }
}


/* *********************************
   void serdisp_rs232_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_rs232_close(serdisp_t* dd) {
  if (dd->dsp_id == DISPID_EAKIT128GXT) {
    write_byte (dd, 0x1B);  /* switch bg-light off: ESC H \0 */
    write_byte (dd, 'H');
    write_byte (dd, 0);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);

    write_byte (dd, 0x1B);  /* switch display off: ESC D \0 */
    write_byte (dd, 'D');
    write_byte (dd, 0);
    do_commit(dd);
    SDCONN_usleep(dd->sdcd, 5000);
  }
}

