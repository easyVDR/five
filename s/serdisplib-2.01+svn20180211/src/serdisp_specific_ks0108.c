/*
 *************************************************************************
 *
 * serdisp_specific_ks0108.c
 * routines for controlling samsung ks0108-based displays 
 * (eg: powertip pg-12864 series)
 *
 *************************************************************************
 *
 * copyright (C) 2003-2013  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 *
 * contributions:
 *
 * RC5-handling and threading (c) 2006 by maf
 *
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

#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_gpevents.h"


#if defined(HAVE_LIBPTHREAD) /* || defined(HAVE_NETSOCK_LIBS) */ 
 #include "serdisplib/serdisp_fctptr.h"
#endif

/*
#ifdef HAVE_NETSOCK_LIBS
 #include <signal.h>
#endif
*/

#ifdef HAVE_LIBPTHREAD
 #include <pthread.h>
 static pthread_mutex_t mutex_comm = PTHREAD_MUTEX_INITIALIZER;
#endif


#undef  OPT_USEOLDUPDATEALGO
#define OPT_USEUPDATEDELTAALGO

#ifndef NULL
#define NULL 0
#endif

/*
 * command constants
 */

#define CMD_DISPLAYON   0x3F
#define CMD_DISPLAYOFF  0x3E

#define CMD_STRTLINEADR 0xC0
#define CMD_SPAGEADR    0xB8
#define CMD_SCOLADR     0x40

/* define shorts for signals to make coding simpler. must match order of serdisp_ks0108_wiresignals[] */
#define SIG_D0          (dd->sdcd->signals[ 0])
#define SIG_D1          (dd->sdcd->signals[ 1])
#define SIG_D2          (dd->sdcd->signals[ 2])
#define SIG_D3          (dd->sdcd->signals[ 3])
#define SIG_D4          (dd->sdcd->signals[ 4])
#define SIG_D5          (dd->sdcd->signals[ 5])
#define SIG_D6          (dd->sdcd->signals[ 6])
#define SIG_D7          (dd->sdcd->signals[ 7])
#define SIG_A0          (dd->sdcd->signals[ 8])
#define SIG_CS1         (dd->sdcd->signals[ 9])
#define SIG_CS2         (dd->sdcd->signals[10])
#define SIG_EN          (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])


/* different display types/models supported by this driver */
#define DISPID_KS0108      1
#define DISPID_CTINCLUD    2


serdisp_wiresignal_t serdisp_ks0108_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "A0",         0,   'C',    8 }
  ,{SDCT_PP, "CS1",        0,   'C',    9 }
  ,{SDCT_PP, "CS2",        0,   'C',   10 }
  ,{SDCT_PP, "EN",         0,   'C',   11 }
  ,{SDCT_PP, "RESET",      1,   'D',   12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',   13 }
};


serdisp_wiredef_t serdisp_ks0108_wiredefs[] = {
   {  0, SDCT_PP, "Standard", "DATA8,A0:nSELIN,CS1:nAUTO,CS2:INIT,EN:nSTRB", "Standard wiring"}
};


serdisp_options_t serdisp_ks0108_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "WIDTH",     "",        64, 256, 64, 0,  ""}
  ,{  "HEIGHT",    "",        64,  64, 64, 0,  ""}
  ,{  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
};

serdisp_options_t serdisp_ctinclud_options[] = {
   /*  name       aliasnames  min  max mod flags              defines  */
   {  "DELAY",     "",         0,  -1,  1, SD_OPTIONFLAG_RW,  ""}
/*  ,{  "PROCCMDPORT","CMDPORT",-1,  -1, -1, SD_OPTIONFLAG_RO,  ""}*/
#ifdef HAVE_LIBPTHREAD
  ,{  "IRFLASH",   "",        -1,   4,  1, SD_OPTIONFLAG_RW,  ""}
  ,{  "EVPROXY",   "",        -1,  -1, -1, SD_OPTIONFLAG_LOC, "DEFAULT=/tmp/sdproxyd"}
#endif
};


#ifdef HAVE_LIBPTHREAD
SDGPI_t serdisp_ctinclud_GPIs[] = {
   /* id  name     aliasnames         type               enabled  file descr. or last value */
   {  0, "RC5",   "IR,IRRECV",        SDGPT_NORMRCSTREAM, 0,      {0} }
};
#endif

SDGPO_t serdisp_ctinclud_GPOs[] = {
   /* id  name     aliasnames         type          mode min max defines */
   {  0, "DGPO0", "LED1,GREEN,PLAY",  SDGPT_INVBOOL, 'S', 0,  1, ""}
  ,{  1, "DGPO1", "LED2,RED,RECORD",  SDGPT_INVBOOL, 'S', 0,  1, ""}
  ,{  2, "DGPO2", "LED3,BLUE1",       SDGPT_INVBOOL, 'S', 0,  1, ""}
  ,{  3, "DGPO3", "LED4,BLUE2",       SDGPT_INVBOOL, 'S', 0,  1, ""}
  ,{  4, "DGPO4", "LED5,USERLED",     SDGPT_INVBOOL, 'S', 0,  1, ""}
};


/* internal typedefs and functions */

#define TRANSFER_CMD     0
#define TRANSFER_DATA    1
#define TRANSFER_RESET   2


typedef struct serdisp_ks0108_specific_s {
  byte     currcs;         /* current select chip */
  int      controllers;    /* amount of controllers */
  uint32_t chipselect[4];  /* supports up to 4 controllers -> CS1 + CS2 -> 4 possibilities */
  void     (*fp_transfer)  (serdisp_t* dd, int type, byte item);
  void     (*fp_switchcs)  (serdisp_t* dd, byte controller);
#ifdef HAVE_LIBPTHREAD
  int      irflash;
  char*    evproxy_devname;
  int      fd_proxy;
  struct timeval proxy_lastcheck;
#endif /* HAVE_LIBPTHREAD */
#if 0
  long testsec;
  int  testcnt, testcnt2;
#endif
} serdisp_ks0108_specific_t;


static void  serdisp_ks0108_init               (serdisp_t*);
static void  serdisp_ks0108_update             (serdisp_t*);
static int   serdisp_ks0108_setoption          (serdisp_t*, const char*, long);
static void  serdisp_ks0108_close              (serdisp_t*);

static void  serdisp_ks0108_transfer_parport   (serdisp_t*, int, byte);
static void  serdisp_ks0108_switchcs_parport   (serdisp_t*, byte);

static void  serdisp_ks0108_transfer_autoclock (serdisp_t*, int, byte);
static void  serdisp_ks0108_switchcs_autoclock (serdisp_t*, byte);

static int   serdisp_ctinclud_gpo_value (serdisp_t* dd, byte gpid, int32_t value);
#ifdef HAVE_LIBPTHREAD
static int   serdisp_ctinclud_gpi_enable (serdisp_t* dd, byte gpid, int enable);
static SDGP_event_t* serdisp_ctinclud_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle);
static int   serdisp_ctinclud_evlp_trigevents(serdisp_t* dd, SDGP_event_t* currevent);
static int   serdisp_ctinclud_fopen_proxy(serdisp_t* dd);
#endif /* HAVE_LIBPTHREAD */

static int   serdisp_ks0108_freeresources      (serdisp_t* dd);

#if 0
static int serdisp_ctinclud_evlp_schedevents_rottest(serdisp_t* dd);
#endif


static serdisp_ks0108_specific_t* serdisp_ks0108_internal_getStruct(serdisp_t* dd) {
  return (serdisp_ks0108_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_ks0108_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
/*
  } else if (serdisp_compareoptionnames(dd, optionname, "IRMODE")) {
    *typesize = sizeof(int);
    return &(serdisp_ks0108_internal_getStruct(dd)->irmode);
*/
#ifdef HAVE_LIBPTHREAD
  if (serdisp_compareoptionnames(dd, optionname, "IRFLASH")) {
    *typesize = sizeof(int);
    return &(serdisp_ks0108_internal_getStruct(dd)->irflash);
  } else if (serdisp_compareoptionnames(dd, optionname, "EVPROXY")) {
    *typesize = 0;
    return &(serdisp_ks0108_internal_getStruct(dd)->evproxy_devname);
  }
#endif
  return 0;
}

/* callback-function for memory-deallocations not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_ks0108_freeresources (serdisp_t* dd) {
/*  if (serdisp_ks0108_internal_getStruct(dd)->lircdev)
    free (serdisp_ks0108_internal_getStruct(dd)->lircdev);
 */
#ifdef HAVE_LIBPTHREAD
  if (serdisp_ks0108_internal_getStruct(dd)->evproxy_devname)
    free (serdisp_ks0108_internal_getStruct(dd)->evproxy_devname);
#endif

  return 0;
}


/* *********************************
   serdisp_t* serdisp_ks0108_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_ks0108_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_ks0108_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_ks0108_specific_t));

  /* "KS0108"-based displays supported in here (eg. powertip pg-12864 series)  */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("KS0108", dispname))
    dd->dsp_id = DISPID_KS0108;
  else if (serdisp_comparedispnames("CTINCLUD", dispname))
    dd->dsp_id = DISPID_CTINCLUD;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_ks0108.c", dispname);
    return (serdisp_t*)0;
  }

  dd->width             = 128;
  dd->height            = 64;
  dd->depth             = 1;
  dd->startxcol         = 0;
  dd->feature_contrast  = 0;
  dd->feature_invert    = 0;
  dd->curr_rotate       = 0;         /* unrotated display */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;
  dd->fp_init           = &serdisp_ks0108_init;
  dd->fp_update         = &serdisp_ks0108_update;
  dd->fp_setoption      = &serdisp_ks0108_setoption;
  dd->fp_close          = &serdisp_ks0108_close;

  serdisp_ks0108_internal_getStruct(dd)->fp_transfer = &serdisp_ks0108_transfer_parport;
  serdisp_ks0108_internal_getStruct(dd)->fp_switchcs = &serdisp_ks0108_switchcs_parport;

  dd->delay             = 180;  /* may be reduced when using short and shielded wires */
  dd->optalgo_maxdelta  = 3;

  if (dd->dsp_id == DISPID_CTINCLUD) {
    dd->fp_getvalueptr    = &serdisp_ks0108_getvalueptr;
    dd->fp_freeresources  = &serdisp_ks0108_freeresources;
    dd->connection_types  = SERDISPCONNTYPE_PARPORT | SERDISPCONNTYPE_IOW24;

    serdisp_ks0108_internal_getStruct(dd)->fp_transfer = &serdisp_ks0108_transfer_autoclock;
    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs = &serdisp_ks0108_switchcs_autoclock;

    dd->delay             = 0;  /* auto-clocking by iowarrior */
    dd->optalgo_maxdelta  = 6;

    /* add gpevset */
    if (! (dd->gpevset = (SDGP_gpevset_t*) sdtools_malloc( sizeof(SDGP_gpevset_t)) )) {
      sd_debug(0, 
        "%s(): cannot allocate memory for general purpose event set. continuing without support for it ...",
        __func__
      );
    }

    if (dd->gpevset) {
      memset(dd->gpevset, 0, sizeof(SDGP_gpevset_t));

#ifdef HAVE_LIBPTHREAD
      /* add GPIs */
      dd->gpevset->gpis = serdisp_ctinclud_GPIs;
      dd->gpevset->amountgpis = sizeof(serdisp_ctinclud_GPIs)/sizeof(SDGPI_t);
#endif

      /* add GPOs */
      dd->gpevset->gpos = serdisp_ctinclud_GPOs;
      dd->gpevset->amountgpos = sizeof(serdisp_ctinclud_GPOs)/sizeof(SDGPO_t);

      /* function pointers */
      dd->gpevset->fp_hnd_gpo_value =   &serdisp_ctinclud_gpo_value;
      dd->gpevset->fp_hnd_gpo_package = 0;  /* no data package gpos */
#ifdef HAVE_LIBPTHREAD
      dd->gpevset->fp_hnd_gpi_enable =  &serdisp_ctinclud_gpi_enable;
      dd->gpevset->fp_evlp_receiver =   &serdisp_ctinclud_evlp_receiver;
      dd->gpevset->fp_evlp_trigevents = &serdisp_ctinclud_evlp_trigevents;
      /*dd->gpevset->fp_evlp_schedevents = &serdisp_ctinclud_evlp_schedevents_rottest;*/

      serdisp_ks0108_internal_getStruct(dd)->irflash = -1;  /* default: no ir flash  */
      serdisp_ks0108_internal_getStruct(dd)->fd_proxy = -1;  /* initialise file desc. to proxy  */
      serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck.tv_sec = 0;  /* init check sec with 0  */

      /* no need to autostart event loop if the sole GPI isn't enabled */
      dd->gpevset->evlp_noautostart = 1;
#endif
    }
  }

  /* serdisp_ks0108_internal_getStruct(dd)->controllers = 2; */  /* calculated in serdisp_ks0108_init() */

  serdisp_ks0108_internal_getStruct(dd)->currcs = 0;  /* pre-init currcs with controller 0 */

  if (dd->dsp_id == DISPID_CTINCLUD) {
    serdisp_setupstructinfos(dd, 0, 0, serdisp_ctinclud_options);
  } else {
    serdisp_setupstructinfos(dd, serdisp_ks0108_wiresignals, serdisp_ks0108_wiredefs, serdisp_ks0108_options);
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  /* force c't include display to width=128 (== ignore user-defined options for width other than 128 */
  if (dd->dsp_id == DISPID_CTINCLUD) {
    if (dd->width != 128) {
      dd->width = 128;
      serdisp_ks0108_internal_getStruct(dd)->controllers = 2;
      sd_debug(0, "%s(): c't includ display only supports 128x64 => width will be forced to 128", __func__);
    }
  }
  return dd;
}


/* *********************************
   void serdisp_ks0108_init(dd)
   *********************************
   initialise a ks0108-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ks0108_init(serdisp_t* dd) {
  int ctr;  /* loop through controllers */

  /* calculate amount of controllers to be driven - only multiples of 64 are accepted by serdisp_ks0108_options[] */
  serdisp_ks0108_internal_getStruct(dd)->controllers = dd->width / 64;

  if (dd->dsp_id != DISPID_CTINCLUD) {
    if (serdisp_ks0108_internal_getStruct(dd)->controllers <= 2) {
      serdisp_ks0108_internal_getStruct(dd)->chipselect[0] = SIG_CS1;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[1] = SIG_CS2;
    } else {  /* requires a multiplexer (eg 74LS42) */
      serdisp_ks0108_internal_getStruct(dd)->chipselect[0] = 0;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[1] = SIG_CS1;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[2] = SIG_CS2;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[3] = SIG_CS1 | SIG_CS2 ;
    }
    dd->feature_backlight = (SIG_BACKLIGHT) ? 1 : 0;
  }

  serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_RESET, 0);

  serdisp_ks0108_internal_getStruct(dd)->currcs = -1;

  /* the initialisation and command reference can be found in the datasheet of KS0108 */
  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {  
    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);

    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_DISPLAYON);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR);
  }
  SDCONN_commit(dd->sdcd);

#ifdef HAVE_LIBPTHREAD
  /* create proxy socket for sending events */
  if (serdisp_ks0108_internal_getStruct(dd)->evproxy_devname) {
    /* only return value 0 (success) or -4 (runtime error) may occur in this context */
    if (! serdisp_ctinclud_fopen_proxy(dd) ) {
      dd->gpevset->gpis[0].enabled = 1;
      /*SDEVLP_start(dd);*/   /* will be started in serdisp_control.c/_init() */
      dd->gpevset->evlp_noautostart = 0;
    }
  }
#endif

  sd_debug(2, "%s(): done with initialising", __func__);
}


/* *********************************
   void serdisp_ks0108_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:

     background knowledge: after writing a page-entry to the display,
     the x-address is increased automatically =>
     * try to utilize this auto-increasing
       (if a whole horizontal line needs to be redrawn: only 4x PutCtrl per page will be needed)

     * on the other hand try to avoid writing of unchanged data: 
       best case: no need to change any data in a certain page: 0x PutCtrl (set page) + 0x PutCtrl (set xpos) + 0x PutData)

*/
void serdisp_ks0108_update(serdisp_t* dd) {
  int ctr;  /* loop through controllers */
  int x, page;
  int pages = (dd->height+7)/8;
  int xoffset;
  byte data;

#ifdef OPT_USEOLDUPDATEALGO

#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
#endif
  for (ctr = 0; ctr < 2; ctr ++) {
    xoffset = 64 * ctr;

    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);

    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR);
      serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR);
    for (page = 0; page < pages; page++) {
      serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR | page);

      for(x = 0; x < /*(dd->width + dd->xcolgaps) % 64*/ 64; x++) {
        data = dd->scrbuf[ dd->width * page  +  x + xoffset];

        /* if (dd->curr_invert && !(dd->feature_invert)) */
        if (dd->curr_invert)
          data = ~data;

        serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_DATA, data);
        dd->scrbuf_chg[dd->width * (page/8) + x + xoffset] &= 0xFF - (1 << (page%8)) ;
      }
    }
    SDCONN_commit(dd->sdcd);
  }
#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
#endif

#elif defined(OPT_USEUPDATEDELTAALGO)  /* en theorie this should be the fastest update algo */
  int col;
  int page_set;
  int col_delta, delta;
  int max_delta = dd->optalgo_maxdelta;

#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
#endif
  serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR);

  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {  
    xoffset = 64 * ctr;

    for (page = 0; page < pages; page++) {
      col = 0;
      page_set = 0;

      while (col < 64) {
        if ( dd->scrbuf_chg[ col + xoffset + dd->width *(page/8)] & ( 1 << (page%8)) ) {
          col_delta = col;

          delta = 0;
          while (col_delta < 64 - delta - 1 && delta < max_delta) {
            if (dd->scrbuf_chg[ col_delta + 1 + delta + xoffset  + dd->width * (page/8)] & ( 1 << (page%8)) ) {
              col_delta += delta + 1;
              delta = 0;
            } else {
              delta++;
            }
          }

          if (!page_set) {
            serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
            serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR | page);
            page_set = 1;
          }

          serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR | col);
          for (x = col ; x <= col_delta; x++) {
            data = dd->scrbuf[x + xoffset + dd->width * page ];

            /* if (dd->curr_invert && !(dd->feature_invert)) */
            if (dd->curr_invert)
              data = ~data;

            serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_DATA, data);
            dd->scrbuf_chg[ x + xoffset + dd->width * (page/8)] &= 0xFF - (1 << (page%8)) ;
          }
          col = col_delta+1;
        } else {
          col++;
        }
      }
    }
    SDCONN_commit(dd->sdcd);
  }

  /* dummy operation for multiplexed CS-lines (else: last page column will not be drawn) */
  serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, (ctr+1)%serdisp_ks0108_internal_getStruct(dd)->controllers);

#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
#endif

#else /* OPT_USEOOLDUPDATEALGO -> 2DELTA */
  int set_page, last_x;

#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
#endif
  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {  
    xoffset = 64 * ctr;

    for (page = 0; page < pages; page++) {

      last_x = -2;
      set_page = 1;

      for(x = 0; x < /*dd->width + dd->xcolgaps*/ 64; x++) {

        /* either actual_x or actual_x + 1 has changed  or one of left/right-most */
        if ( dd->scrbuf_chg[x + xoffset + dd->width *(page/8)] & ( 1 << (page%8)) ) {
          if (x > last_x+1 ) {

            if (set_page) {
              serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);

              serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR | page);
              set_page = 0;
            }

            /* x-position may be written directly to ks0108 */
            serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
            serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR | x);
          }
          data = dd->scrbuf[ dd->width * page  +  x + xoffset];

          /* if (dd->curr_invert && !(dd->feature_invert)) */
          if (dd->curr_invert)
            data = ~data;

          serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
          serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_DATA, data);
          dd->scrbuf_chg[x + xoffset + dd->width * (page/8)] &= 0xFF - (1 << (page%8)) ;

          last_x = x;
        }
      }
    }
    SDCONN_commit(dd->sdcd);
  }
#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
#endif

#endif  /* OPT_USEOLDUPDATEALGO */
}


/* *********************************
   int serdisp_ks0108_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_ks0108_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT") ) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;

    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_DISPLAYON);
    SDCONN_commit(dd->sdcd);

#ifdef HAVE_LIBPTHREAD
  } else if (serdisp_compareoptionnames(dd, option, "IRFLASH") ) {
    serdisp_ks0108_internal_getStruct(dd)->irflash = (int)value;
  } else if (serdisp_compareoptionnames(dd, option, "EVPROXY") ) {
    serdisp_ks0108_internal_getStruct(dd)->evproxy_devname = (char*) sdtools_malloc( strlen( (char*) value )+1);
#endif
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}


/* *********************************
   void serdisp_ks0108_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ks0108_close(serdisp_t* dd) {
  int ctr;

#ifdef HAVE_LIBPTHREAD
  if ( SDGPI_isenabled(dd, 0)) {
     SDGPI_enable(dd, 0, 0);
  }

  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
#endif
  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {
    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_DISPLAYOFF);
    SDCONN_commit(dd->sdcd);
  }
  serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_RESET, 0);
#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
#endif
}





/* *********************************
   void serdisp_ks0108_transfer_parport(dd, type, item)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd         ... display descriptor
   type       ... one if TRANSFER_CMD, _DATA, _RESET
   item       ... byte to be processed
*/
void serdisp_ks0108_transfer_parport(serdisp_t* dd, int type, byte item) {
  uint32_t item_split = 0;
  uint32_t td_clk1 = 0;
  uint32_t td_clk2 = 0;
  int      ctr;
  int      i;

  switch (type) {
    case TRANSFER_CMD:
    case TRANSFER_DATA:

      /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
      td_clk1 = serdisp_ks0108_internal_getStruct(dd)->chipselect[serdisp_ks0108_internal_getStruct(dd)->currcs] | SIG_EN;
      td_clk2 = serdisp_ks0108_internal_getStruct(dd)->chipselect[serdisp_ks0108_internal_getStruct(dd)->currcs];

      if (type == TRANSFER_DATA) { /* high: data, low: command */
        td_clk1 |= SIG_A0;
        td_clk2 |= SIG_A0;
      }

      for (i = 0; i < 8; i++)
        if (item & (1 << i))
           item_split |= dd->sdcd->signals[i];


      td_clk1 |= item_split;

      /* signal EN from high to low */
      SDCONN_writedelay(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
      /* EN needs to be kept on this level ~ 12 times as long as needed for a signal change */    
      SDCONN_writedelay(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd, 12 * dd->delay); 
      break;
    case TRANSFER_RESET:
      /* reset */

      serdisp_ks0108_internal_getStruct(dd)->currcs = 0;
      for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {
        serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
        if (SIG_RESET) {  /* reset signal sent by parport */
          SDCONN_write(dd->sdcd, SIG_RESET | SIG_EN | serdisp_ks0108_internal_getStruct(dd)->chipselect[ctr], 
                       dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
        }
        /* min. 1 uS according to data sheet. we use 5 to be sure */
        SDCONN_usleep(dd->sdcd, 5);
        SDCONN_write(dd->sdcd, SIG_EN | serdisp_ks0108_internal_getStruct(dd)->chipselect[ctr], 
                     dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
        SDCONN_usleep(dd->sdcd, 5);
      }
      break;
    default:
      break;
  }
}


/* *********************************
   void serdisp_ks0108_transfer_autoclock(dd, type, item)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd         ... display descriptor
   type       ... one if TRANSFER_CMD, _DATA, _RESET
   item       ... byte to be processed
*/
void serdisp_ks0108_transfer_autoclock(serdisp_t* dd, int type, byte item) {
  uint32_t t_item;

  switch (type) {
    case TRANSFER_CMD:
    case TRANSFER_DATA:
      t_item = ((type == TRANSFER_CMD) ? 0x00010000 : 0x0) + item;
      SDCONN_write(dd->sdcd, t_item, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      break;
    case TRANSFER_RESET:
      break;
    default:
      break;
  }
}




/* *********************************
   void serdisp_ks0108_switchcs_parport(dd, controller)
   *********************************
   switch controller
   *********************************
   dd         ... display descriptor
   controller ... id of controller
*/
void serdisp_ks0108_switchcs_parport(serdisp_t* dd, byte controller) {
  /* only switch if differing from prior controller id */
  if (serdisp_ks0108_internal_getStruct(dd)->currcs != controller) {

    /* swing over to next controller */
    serdisp_ks0108_internal_getStruct(dd)->currcs = controller;

    SDCONN_writedelay(dd->sdcd, 
                      serdisp_ks0108_internal_getStruct(dd)->chipselect[
                       serdisp_ks0108_internal_getStruct(dd)->currcs
                      ],
                      dd->sdcd->io_flags_writecmd, dd->delay >> 2);
  }
}



/* *********************************
   void serdisp_ks0108_switchcs_autoclock(dd, controller)
   *********************************
   switch controller
   *********************************
   dd         ... display descriptor
   controller ... id of controller
*/
void serdisp_ks0108_switchcs_autoclock(serdisp_t* dd, byte controller) {
  /* only switch if differing from prior controller id */
  if (serdisp_ks0108_internal_getStruct(dd)->currcs != controller) {

    /* swing over to next controller */
    serdisp_ks0108_internal_getStruct(dd)->currcs = controller;

    /* small hack to avoid erraneous pixels when switching CS-lines (CMD_STRTLINEADR used as NOP) */
    /* serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR); */

    /*            ctrl. byte stat.byte  data byte
     * xxxx xxxx  0000 0010  ---- ----  cccc cccc 
     *                   ^-------------------------- indicates chip select, value -> data byte
     */

    SDCONN_writedelay(dd->sdcd, 
                      0x00020000 | ((uint32_t)(serdisp_ks0108_internal_getStruct(dd)->currcs)),
                      dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 0);
  }
}


/* ****** GPI / GPO ***** */


/* *********************************
   int serdisp_ctinclud_gpo_value(dd, gpid, value)
   *********************************
   enable/disable a GPI
   *********************************
   dd     ... display descriptor
   gpid   ... GPO id
   value  ... new value for GPO
   *********************************
   returns:
    0  ... successful
   <0  ... error
*/
int serdisp_ctinclud_gpo_value (serdisp_t* dd, byte gpid, int32_t value) {
  if (gpid <= 4) {
    int32_t mask = 0;
    switch (gpid) {
      case 0: mask = 0x08; break;
      case 1: mask = 0x04; break;
      case 2: mask = 0x01; break;
      case 3: mask = 0x80; break;
      case 4: mask = 0x02; break;
    }

#ifdef HAVE_LIBPTHREAD
/*
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
*/
#endif
    if (value == 2) { /* toggle */
      SDCONN_writedelay(dd->sdcd, 
                        (uint32_t)(0x00060000  | mask), 
                        dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 0);
    } else {
      value = (value) ? 0 : 1;      /* all available gpos are active high */
      SDCONN_writedelay(dd->sdcd, 
                        (uint32_t) (((value) ? 0x00050000 : 0x00040000) | mask) , 
                        dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 0);
    }
#ifdef HAVE_LIBPTHREAD
/*
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );
*/
#endif
  } else {
    return -1;
  }
  return 0;
}


/* *********************************
   int serdisp_ctinclud_gpi_enable(dd, gpid, enable)
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
#ifdef HAVE_LIBPTHREAD
int serdisp_ctinclud_gpi_enable (serdisp_t* dd, byte gpid, int enable) {
  if (!dd || !dd->gpevset || gpid >= dd->gpevset->amountgpis)
    return -1;

  if (enable && SDEVLP_getstatus(dd) == SDEVLP_STOPPED) {
    SDEVLP_start(dd);

    if (pipe(dd->gpevset->gpis[gpid].fd) < 0) {
      sd_error(SERDISP_ERUNTIME, "%s(): could not create pipe for GPI '%s'.", __func__,
               dd->gpevset->gpis[gpid].name);
      return -1;
    }
    sd_debug(2, "%s(): pipe created for GPI '%s'. pipe (fd r/w): %d/%d", __func__,
             dd->gpevset->gpis[gpid].name, dd->gpevset->gpis[gpid].fd[0], dd->gpevset->gpis[gpid].fd[1]);
  }

  dd->gpevset->gpis[gpid].enabled = enable;

  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_comm );
  /* enable or disable RC5 mode */
  SDCONN_writedelay(dd->sdcd, 
                    0x00030000 | enable, 
                    dd->sdcd->io_flags_writecmd /*| dd->sdcd->io_flags_writedata*/, 0);
  SDCONN_commit(dd->sdcd);
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_comm );

  if (!enable && SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    unsigned char  buffer[8];
    int            bytes;
    /* clear queue */
    do {
        bytes = SDCONN_readstream(dd->sdcd, buffer, 8);
    } while (bytes > 0);
    close(dd->gpevset->gpis[gpid].fd[0]);
    SDEVLP_stop(dd);
  }

  return 0;
}
#endif


/* *********************************
   SDGP_event_t* serdisp_ctinclud_evlp_receiver(dd, recycle)
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
#ifdef HAVE_LIBPTHREAD
SDGP_event_t* serdisp_ctinclud_evlp_receiver(serdisp_t* dd, SDGP_event_t* recycle) {
  unsigned char  buffer[8];
  int            bytes;
  SDGP_event_t*  event;

  uint32_t       stream[4] = {0,0,0,0};

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return 0;

  if (! SDGPI_isenabled (dd, 0)) {
    usleep(5000);
    return 0;
  }

  bytes = SDCONN_readstream(dd->sdcd, buffer, 8);
  if( bytes < 0 ) {
    if( errno==EAGAIN || errno==ETIMEDOUT ) {
      return (SDGP_event_t*)0;
    }
  }

  if( bytes != 8 ) {
    fprintf(stderr,"Short read from USB: %d bytes (sd_runtimeerror: %d): \n", bytes, sd_runtime_error() );
    sd_runtimeerror = 1;
    if (!sd_runtime_error()) {  /* de-activated for now -> jump to else-tree */
        /* retry */
        fprintf(stderr,"cycling IOW/RC5 (sd_runtimeerror: %d): \n", sd_runtime_error() );

        /* cycle RC5-support */
        SDCONN_writedelay(dd->sdcd, 
                          0x00030000 | ((uint32_t)(0)),
                          dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 0);
        SDCONN_writedelay(dd->sdcd, 
                          0x00030000 | ((uint32_t)(1)),
                          dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 0);
        fprintf(stderr,"done: cycling IOW/RC5 (sd_runtimeerror: %d): \n", sd_runtime_error() );

        bytes = SDCONN_readstream(dd->sdcd, buffer, 8);
        if( bytes<0 ) {
          if( errno == EAGAIN || errno==ETIMEDOUT )
            return 0;
        }
        if( bytes != 8 ) {
          fprintf(stderr,"Short read from USB: %d bytes (sd_runtimeerror: %d): \n", bytes, sd_runtime_error() );
          dd->gpevset->gpis[0].enabled = 0;
          SDEVLP_stop(dd);
          return 0;
        }
    } else {
      return 0;
    }
  }

#if 0
    printf( "USB read: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", 
            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5],
            buffer[6], buffer[7] );
#endif

  if( buffer[0] == 0x02 ) {   /* ignore event '0x02' in here */
    return 0;
  } else if( buffer[0]!=IOW_RC5_ENABLE_REPORT ) {
    sd_debug(1, "%s(): read unknown incoming event from USB stream: 0x%02x", __func__, buffer[0]);
    return 0;
  }

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
  event->type = SDGPT_NORMRCSTREAM;
  event->length = (uint16_t) sizeof(stream);
  gettimeofday(&event->timestamp, 0);

  event->word_size = (byte)sizeof(stream[0]);

  /* extract RC5 message from report */
  stream[0] = buffer[1] + 256*buffer[2];;

  /* convert header and payload to network byte order */
  SDGPT_event_header_hton(event);
  SDGPT_event_payload_hton(stream, sizeof(stream), (byte)sizeof(stream[0]));

  /* and write it to associated pipe */
  if( write(dd->gpevset->gpis[0].fd[1],event,sizeof(SDGP_event_t)) != sizeof(SDGP_event_t)) {
    sd_error(SERDISP_EPIPE, "%s(): unable to write event header to pipe. cause: %s", __func__, strerror(errno));
    if (!recycle)
      free(event);
    return 0;
  }
  if( write(dd->gpevset->gpis[0].fd[1],stream,sizeof(stream)) != sizeof(stream)) {
    sd_error(SERDISP_EPIPE, "%s(): unable to write event body to pipe. cause: %s", __func__, strerror(errno));
    return 0;
  }

  /* if no proxy socket:  closed because of broken pipe? (proxy_lastcheck.tv_sec != 0) */
  if ( (serdisp_ks0108_internal_getStruct(dd)->fd_proxy < 0) && 
       serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck.tv_sec ) {
    serdisp_ctinclud_fopen_proxy(dd);
  }

  /* disable signals during send to avoid being terminated by a SIGPIPE */
  if (serdisp_ks0108_internal_getStruct(dd)->fd_proxy >= 0) {
    void* orig_sighnd = signal(SIGPIPE, SIG_IGN);
    errno = 0;
    if( fp_send(serdisp_ks0108_internal_getStruct(dd)->fd_proxy,event,sizeof(SDGP_event_t), 0) != sizeof(SDGP_event_t) ) {
      sd_error(SERDISP_EPIPE, "%s(): unable to write event header to proxy. cause: %s", __func__, strerror(errno));
    } else {
      if( fp_send(serdisp_ks0108_internal_getStruct(dd)->fd_proxy,stream,sizeof(stream), 0) != sizeof(stream) ) {
        sd_error(SERDISP_EPIPE, "%s(): unable to write event body to proxy. cause: %s", __func__, strerror(errno));
      }
    }
    signal(SIGPIPE, (void*)orig_sighnd);

    if (errno == EPIPE) {
      sd_debug(0, "%s(): proxy no longer available. trying to re-connect in >= 3 seconds.", __func__);
      close(serdisp_ks0108_internal_getStruct(dd)->fd_proxy);
      serdisp_ks0108_internal_getStruct(dd)->fd_proxy = -1;
      gettimeofday(&(serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck), 0);
    }
  }

  /* convert header and payload back to host byte order */
  SDGPT_event_header_ntoh(event);
  SDGPT_event_payload_ntoh(stream, sizeof(stream), (byte)sizeof(stream[0]));

  return event;
}
#endif /* HAVE_LIBPTHREAD */


/* *********************************
   SDGP_event_t* serdisp_ctinclud_evlp_trigevents(dd, currevent)
   *********************************
   reads next event in line
   *********************************
   dd        ... device descriptor
   currevent ... current event read by receiver-function
   *********************************
   returns:
       0 ... no error
       1 ... event unsupported
      <0 ... error code
*/
#ifdef HAVE_LIBPTHREAD
int serdisp_ctinclud_evlp_trigevents(serdisp_t* dd, SDGP_event_t* currevent) {
  if (currevent && currevent->type == SDGPT_NORMRCSTREAM) {
    int irflash = serdisp_ks0108_internal_getStruct(dd)->irflash;

    if (irflash >= 0) {
      /* only flash if
         libpthread is unavailable at all (thus no threads anyway)
         or libpthread is available AND the mutex is not locked by _update() yet
         => !a OR (a AND b) == !a OR b
      */
      if ( (! SDFCTPTR_checkavail(SDFCTPTR_PTHREAD)) || (fp_pthread_mutex_trylock( &mutex_comm ) == 0) ) {
        SDGPO_invert(dd, irflash);
        SDGPO_invert(dd, irflash);
        if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
          fp_pthread_mutex_unlock( &mutex_comm );
      }
    }
  } else {
    return 1;
  }
  return 0;
}
#endif /* HAVE_LIBPTHREAD */


/* *********************************
   int serdisp_ctinclud_fopen_proxy(dd)
   *********************************
   try to (re-)open the proxy socket
   *********************************
   dd        ... device descriptor
   *********************************
   returns:
       0 ... success
      -1 ... already connected
      -2 ... evproxy_devname not set or invalid (too long)
      -3 ... re-connect time too short (at least 3 secs between to tries)
      -4 ... runtime error (socket() or connect() have failed)
*/
#ifdef HAVE_LIBPTHREAD
int serdisp_ctinclud_fopen_proxy(serdisp_t* dd) {
  struct timeval curr_timestamp;
  struct sockaddr_un addr;
  int reconnect = (serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck.tv_sec != 0);

  if (!serdisp_ks0108_internal_getStruct(dd)->evproxy_devname)
    return -2;

  if ( serdisp_ks0108_internal_getStruct(dd)->fd_proxy >= 0)
    return -1;

  gettimeofday(&curr_timestamp, 0);

  if ( serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck.tv_sec && 
       (curr_timestamp.tv_sec - serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck.tv_sec) < 3)
    return -3;


  sd_debug(2, "%s(): trying to %sconnect to socket '%s'",
              __func__, 
              ((reconnect) ? "re-" : ""),
              serdisp_ks0108_internal_getStruct(dd)->evproxy_devname
  );

  if (strlen(serdisp_ks0108_internal_getStruct(dd)->evproxy_devname) >= sizeof(addr.sun_path) ) {
    sd_debug(0, "%s(): pathname for socket too long (max. %d chars): %s", __func__,
                (int)sizeof(addr.sun_path), serdisp_ks0108_internal_getStruct(dd)->evproxy_devname);
    return -2;
  }

  memset( &addr, 0, sizeof(addr) );
  addr.sun_family = AF_UNIX;
  strcpy( addr.sun_path, serdisp_ks0108_internal_getStruct(dd)->evproxy_devname);

  serdisp_ks0108_internal_getStruct(dd)->fd_proxy =  fp_socket( AF_UNIX, SOCK_STREAM, 0);
  if (serdisp_ks0108_internal_getStruct(dd)->fd_proxy >= 0) {
    if (fp_connect (serdisp_ks0108_internal_getStruct(dd)->fd_proxy,(struct sockaddr *)&addr,sizeof(addr)) != 0 ) {
      sd_debug( ((reconnect) ? 2 : 0),
                "%s(): unable to %sconnect fd %d to socket '%s', error: %s", __func__,
                ((reconnect) ? "re-" : ""),
                serdisp_ks0108_internal_getStruct(dd)->fd_proxy,
                serdisp_ks0108_internal_getStruct(dd)->evproxy_devname, strerror(errno)
      );
      close(serdisp_ks0108_internal_getStruct(dd)->fd_proxy);
      serdisp_ks0108_internal_getStruct(dd)->fd_proxy = -1;
      gettimeofday(&(serdisp_ks0108_internal_getStruct(dd)->proxy_lastcheck), 0);
      return -4; /* runtime error */
    } else {
      sd_debug(2, "%s(): socket '%s' %sconnected to fd %d", __func__, 
                ((reconnect) ? "re-" : ""),
                serdisp_ks0108_internal_getStruct(dd)->evproxy_devname,
                serdisp_ks0108_internal_getStruct(dd)->fd_proxy);
    }
  } else {
    sd_debug(0, "%s(): unable to %sconnect to socket '%s', error: %s", __func__,
                ((reconnect) ? "re-" : ""),
                serdisp_ks0108_internal_getStruct(dd)->evproxy_devname, strerror(errno));
    return -4;
  }
  return 0;
}
#endif /* HAVE_LIBPTHREAD */


#if 0
/* *********************************
   SDGP_event_t* serdisp_ctinclud_evlp_schedevents_rottest(dd)
   *********************************
   test for scheduled events
   *********************************
   dd      ... device descriptor
   *********************************
   returns:
    0  ... no event or error
    else: event
*/
int serdisp_ctinclud_evlp_schedevents_rottest(serdisp_t* dd) {
  struct timeval tv;
  gettimeofday(&tv, 0);
  int cnt = serdisp_ks0108_internal_getStruct(dd)->testcnt;
  int cnt2 = serdisp_ks0108_internal_getStruct(dd)->testcnt2;

  if (tv.tv_sec > serdisp_ks0108_internal_getStruct(dd)->testsec + 1) {
    if (cnt != cnt2)
      SDGPO_invert(dd, cnt2);

    SDGPO_invert(dd, cnt);

    serdisp_ks0108_internal_getStruct(dd)->testsec = tv.tv_sec;
    serdisp_ks0108_internal_getStruct(dd)->testcnt2 = cnt;
    serdisp_ks0108_internal_getStruct(dd)->testcnt = ((cnt + 1) % 5);
  }
  return 0;
}
#endif
