/*
 *************************************************************************
 *
 * serdisp_specific_remote.c
 * client side for sending commands to a serdispd server
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance and enhancements:
 * copyright (C) 2006-2012  wolfgang astleitner
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

#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
/*#include <ctype.h>*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_gpevents.h"
#include "serdisplib/serdisp_srvtools.h"
#include "serdisplib/serdisp_connect_srv.h"
#include "serdisplib/serdisp_tools.h"

#include "serdisplib/serdisp_fctptr.h"


/* private symbols */
static void  serdisp_remote_init(serdisp_t* dd);
static void  serdisp_remote_update(serdisp_t* dd);
static int   serdisp_remote_setoption(serdisp_t* dd, const char* optionname, long value);
static long  serdisp_remote_getoption(serdisp_t* dd, const char* optionname, int* typesize);
static void  serdisp_remote_close(serdisp_t* dd);
static void  serdisp_remote_setsdpixel(serdisp_t* dd, int x, int y, SDCol sdcol);
static SDCol serdisp_remote_getsdpixel(serdisp_t* dd, int x, int y );
static int   serdisp_remote_setupoptions(serdisp_t* dd, const char* dispname, const char* optionstring);

static int   serdisp_remote_local_setoption(serdisp_t* dd, const char* optionname, long value);
static int   serdisp_remote_freeresources (serdisp_t* dd);


/* remote-driver specific options. must start with 'SDR' (if not out of standard options) */
serdisp_options_t serdisp_remote_options[] = {
   /*  name              aliasnames  min  max mod flags              defines  */
   {  "SDRDEBUGEVENTS", "SDRDBGEV",  -1,  -1, -1, SD_OPTIONFLAG_LOC, ""}
/*  ,{  "PROCCMDPORT","CMDPORT",-1,  -1, -1, SD_OPTIONFLAG_RO,  ""}*/
};



typedef struct serdisp_remote_specific_s {
  int  sdrdebugevents;
} serdisp_remote_specific_t;


static serdisp_remote_specific_t* serdisp_remote_internal_getStruct(serdisp_t* dd) {
  return (serdisp_remote_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_remote_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "SDRDEBUGEVENTS")) {
    *typesize = sizeof(int);
    return &(serdisp_remote_internal_getStruct(dd)->sdrdebugevents);
  }
  return 0;
}


/* callback-function for memory-deallications not handled by serdisp_freeresources() in serdisp_control.c */
int serdisp_remote_freeresources (serdisp_t* dd) {
  if (dd->options) {
    int i;
    /* don't free space for local scope options */
    int locopts = sizeof(serdisp_remote_options) / sizeof(serdisp_options_t);

    for (i = 0; i < dd->amountoptions - locopts; i ++) {
      if (dd->options[i].name) free(dd->options[i].name);
      if (dd->options[i].aliasnames) free(dd->options[i].aliasnames);
      if (dd->options[i].defines) free(dd->options[i].defines);
    }
    free (dd->options);
  }

  return 0;
}



/*========================================================================*\
    Setup a server display
\*========================================================================*/
serdisp_t* serdisp_remote_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t*      dd;
  SERDISP_SRVCON *con;

  int xreloctab_size = 0;
  int yreloctab_size = 0;
  int ctable_size    = 0;

  sd_debug( 2, "%s(): Called for display %s with options %s", __func__, dispname, optionstring );

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_NETSOCK) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): network functions could not beloaded but are a requirement for serdisp_specific_remote.c.", __func__);
    return (serdisp_t*)0;
  }

  /* only server device types are supported! */
  if( sdcd->conntype!=SERDISPCONNTYPE_SERVER ) {
    sd_error(SERDISP_ENOTSUP, "%s(): need a server device", __func__);
    return NULL;
  }
  con = ((serdisp_srvdev_t*)sdcd->extra)->con;

  /* create display descriptor */
  dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t));
  if( !dd ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset( dd, 0, sizeof(serdisp_t) );

  /* specific data for local scope options */
  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_remote_specific_t)) )) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate memory for local scope options", __func__);
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_remote_specific_t));

  /* set local function handlers ... */
  dd->fp_init           = &serdisp_remote_init;
  dd->fp_update         = &serdisp_remote_update;
  dd->fp_setoption      = &serdisp_remote_setoption;
  dd->fp_getoption      = &serdisp_remote_getoption;
  dd->fp_close          = &serdisp_remote_close;
  dd->fp_freeresources  = &serdisp_remote_freeresources;

  if( con->mode==CON_TRANSPARENT ) { 
    dd->fp_setsdpixel   = &serdisp_remote_setsdpixel;
    dd->fp_getsdpixel   = &serdisp_remote_getsdpixel;
  }
  if( con->mode==CON_TRANSPARENT || con->mode==CON_WRITETHROUGH ) { 
    dd->fp_setsdpixel   = &serdisp_remote_setsdpixel;
    dd->fp_getsdpixel   = &serdisp_remote_getsdpixel;
  }
  dd->connection_types  = sdcd->conntype;

  dd->curr_invert       = 0;
  dd->curr_rotate       = 0;         /* unrotated display */

  dd->fp_getvalueptr    = &serdisp_remote_getvalueptr;

  /* pre-initialise local scope options */
  serdisp_remote_internal_getStruct(dd)->sdrdebugevents = 0;  /* no debugging of remote events */

  /* check if client protocol version is matching server protocol version */
  {
    int  client_protovers_major = SD_SRV_PROTOVERS_MAJOR;
    int  client_protovers_minor = SD_SRV_PROTOVERS_MINOR;
    int  server_protovers_major;
    int  server_protovers_minor;

    SERDISP_SRVARG args[] = { { MBUF_ARGINT, &client_protovers_major, 0 },
                              { MBUF_ARGINT, &client_protovers_minor, 0 },
                              { MBUF_ARGEND, NULL, 0} }; 
    SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &server_protovers_major, 0 },
                              { MBUF_ARGINT, &server_protovers_minor, 0 },
                              { MBUF_ARGEND, NULL, 0} };

    if( serdisp_srv_sendcmd(con,MBUF_CMD_PROTOVERS,args,rets) < 0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd 'MBUF_CMD_PROTOVERS'", __func__);
      serdisp_freeresources (dd);
      return (serdisp_t*) 0;
    }

    sd_debug( 1, "%s(): server protocol version: %d.%d", __func__, 
                 server_protovers_major, server_protovers_minor);

  }


  /* allocate space for normalised display exchange buffer */
  {
  }

  /* get display characteristics from remote peer ... */
  {
    SERDISP_SRVARG vals[] = { { MBUF_ARGINT,  &dd->width, 0},
                              { MBUF_ARGINT,  &dd->height, 0}, 
                              { MBUF_ARGINT,  &dd->depth, 0}, 
                              { MBUF_ARGINT,  &dd->curr_rotate, 0}, 
                              { MBUF_ARGINT,  &dd->curr_invert, 0}, 
                              { MBUF_ARGINT,  &dd->feature_contrast, 0}, 
                              { MBUF_ARGINT,  &dd->feature_backlight, 0}, 
                              { MBUF_ARGINT,  &dd->scrbuf_size, 0}, 
                              { MBUF_ARGINT,  &dd->scrbuf_chg_size, 0}, 
                              { MBUF_ARGBYTE, &dd->scrbuf_bits_used, 0}, 
                              { MBUF_ARGLONG, &dd->colour_spaces, 0}, 
                              { MBUF_ARGLONG, &dd->dsparea_width, 0},
                              { MBUF_ARGLONG, &dd->dsparea_height, 0},
                              { MBUF_ARGLONG, &dd->default_bgcolour, 0},
                              { MBUF_ARGINT,  &dd->startxcol, 0}, 
                              { MBUF_ARGINT,  &dd->startycol, 0}, 
                              { MBUF_ARGINT,  &dd->xcolgaps, 0}, 
                              { MBUF_ARGINT,  &dd->ycolgaps, 0}, 
                              { MBUF_ARGINT,  &xreloctab_size, 0}, 
                              { MBUF_ARGINT,  &yreloctab_size, 0}, 
                              { MBUF_ARGINT,  &ctable_size, 0}, 
                              { MBUF_ARGEND,  NULL, 0} }; 

    if( serdisp_srv_sendcmd(con,MBUF_CMD_DDINFO,NULL,vals)<0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot read remote configuration", __func__);
      free( dd );
      return NULL;
    }
  }

  /* contrast setting is done by the real display driver and not in here */
  if (dd->feature_contrast)
    dd->curr_contrast = -1;
  /* the same with background light */
  if (dd->feature_backlight)
    dd->curr_backlight = -1;

  /* allocate memory for relocation and colour tables */
  if (xreloctab_size) {
    dd->xreloctab = (int*)  sdtools_malloc( sizeof(int) * (xreloctab_size));
    if (! dd->xreloctab) {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate relocation table xreloctab", __func__);
      serdisp_freeresources (dd);
      return (serdisp_t*) 0;
    }
  }

  if (yreloctab_size) {
    dd->yreloctab = (int*)  sdtools_malloc( sizeof(int) * (yreloctab_size));
    if (! dd->yreloctab) {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate relocation table yreloctab", __func__);
      serdisp_freeresources (dd);
      return (serdisp_t*) 0;
    }
  }

  if (ctable_size) {
    dd->ctable =    (SDCol*) sdtools_malloc( sizeof(SDCol) * ctable_size);
    if (! dd->ctable ) {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate colour tables", __func__);
      serdisp_freeresources (dd);
      return (serdisp_t*) 0;
    }
  }

  /* get relocation and colour tables */
  {
    SERDISP_SRVARG vals[] = { { MBUF_ARGINTS,  dd->xreloctab, xreloctab_size },
                              { MBUF_ARGINTS,  dd->yreloctab, yreloctab_size },
                              { MBUF_ARGLONGS, dd->ctable,    ctable_size },
                              { MBUF_ARGEND,  NULL, 0} }; 


    if( serdisp_srv_sendcmd(con,MBUF_CMD_DDGETTABLES,NULL,vals) < 0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd 'MBUF_CMD_PROTOVERS'", __func__);
      serdisp_freeresources (dd);
      return (serdisp_t*) 0;
    }

  }

  /* get options supported by remote display */
  {
    int i;
    int locopts = sizeof(serdisp_remote_options) / sizeof(serdisp_options_t);

    SERDISP_SRVARG vals[] = { { MBUF_ARGINT,  &dd->amountoptions, 0 },
                              { MBUF_ARGEND,  NULL, 0} };

    if( serdisp_srv_sendcmd(con,MBUF_CMD_OPTIONAMOUNT, NULL, vals)<0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot get amount of supported options", __func__);
      dd->amountoptions = 0;
    }
    sd_debug( 1, "%s(): supported options: total: %d, remote: %d, local: %d", __func__, dd->amountoptions+locopts, dd->amountoptions, locopts);

    if (dd->amountoptions + locopts) {
      dd->options = sdtools_malloc(sizeof(serdisp_options_t) * (dd->amountoptions + locopts) );
      if (! dd->options) {
        sd_error(SERDISP_ERUNTIME, "%s(): unable to allocate memory for options", __func__);
        serdisp_freeresources (dd);
        return (serdisp_t*) 0;
      }

      for (i = 0; i < dd->amountoptions; i++) {
        char buf_name[50];
        char buf_aliasnames[256];
        char buf_defines[256];

        SERDISP_SRVARG args[] = { { MBUF_ARGINT, &i, 0 },
                                  { MBUF_ARGEND, NULL, 0} }; 
        SERDISP_SRVARG rets[] = { { MBUF_ARGSTR, buf_name, sizeof(buf_name)-1 },
                                  { MBUF_ARGSTR, buf_aliasnames, sizeof(buf_aliasnames)-1 },
                                  { MBUF_ARGLONG, &(dd->options[i].minval), 0}, 
                                  { MBUF_ARGLONG, &(dd->options[i].maxval), 0}, 
                                  { MBUF_ARGLONG, &(dd->options[i].modulo), 0}, 
                                  { MBUF_ARGBYTE, &(dd->options[i].flag), 0}, 
                                  { MBUF_ARGSTR, buf_defines, sizeof(buf_defines)-1 },
                                  { MBUF_ARGEND, NULL, 0} };

        if( serdisp_srv_sendcmd(con,MBUF_CMD_OPTIONDESC,args,rets) < 0 ) {
          sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd 'MBUF_CMD_OPTIONDESC'", __func__);
          serdisp_freeresources (dd);
          return (serdisp_t*) 0;
        }

        dd->options[i].name = sdtools_malloc( (size_t)(strlen(buf_name)+1) );
        dd->options[i].aliasnames = sdtools_malloc( (size_t)(strlen(buf_aliasnames)+1) );
        dd->options[i].defines = sdtools_malloc( (size_t)(strlen(buf_defines)+1) );
        if ( ! (dd->options[i].name && dd->options[i].aliasnames && dd->options[i].defines) ) {
          sd_error(SERDISP_ERUNTIME, "%s(): unable to allocate memory for strings", __func__);
          serdisp_freeresources (dd);
          return (serdisp_t*) 0;
        }
        sdtools_strncpy(dd->options[i].name, buf_name, strlen(buf_name));
        sdtools_strncpy(dd->options[i].aliasnames, buf_aliasnames, strlen(buf_aliasnames));
        sdtools_strncpy(dd->options[i].defines, buf_defines, strlen(buf_defines));
      }
    }
    /* append local scope options */
    for (i = 0; i < locopts; i++) {
      dd->options[dd->amountoptions+i] = serdisp_remote_options[i];
    }
    dd->amountoptions += locopts;
  }

  /* get GPIs/GPOs supported by remote display */
  {
    byte amountgpis;
    byte amountgpos;

    SERDISP_SRVARG vals[] = { { MBUF_ARGBYTE,  &amountgpis, 0 },
                              { MBUF_ARGBYTE,  &amountgpos, 0 },
                              { MBUF_ARGEND,  NULL, 0} };

    if( serdisp_srv_sendcmd(con,MBUF_CMD_GPIOAMOUNT, NULL, vals)<0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot get amount of supported GPIs/GPOs", __func__);
      amountgpis = 0;
      amountgpos = 0;
    }
    sd_debug( 1, "%s(): supported GPIs: %d, GPOs: %d", __func__, amountgpis, amountgpos);

    if (! dd->gpevset) {
      /* add gpevset */
      if (! (dd->gpevset = (SDGP_gpevset_t*) sdtools_malloc( sizeof(SDGP_gpevset_t)) )) {
        sd_debug(0,
          "%s(): cannot allocate memory for general purpose event set. continuing without support for it ...",
          __func__
        );
      } else {
        int i;

        memset(dd->gpevset, 0, sizeof(SDGP_gpevset_t));
        dd->gpevset->amountgpis = amountgpis;
        dd->gpevset->amountgpos = amountgpos;

        if (dd->gpevset->amountgpis) {
          dd->gpevset->gpis = sdtools_malloc(sizeof(SDGPI_t) * dd->gpevset->amountgpis);
          if (! dd->gpevset->gpis) {
            sd_debug(0,
              "%s(): cannot allocate memory for GPIs. continuing without support for these ...",
              __func__
            );
            dd->gpevset->amountgpis = 0;
          }

          for (i = 0; i < dd->gpevset->amountgpis; i++) {
            char buf_name[50];
            char buf_aliasnames[256];

            SERDISP_SRVARG args[] = { { MBUF_ARGINT, &i, 0 },
                                      { MBUF_ARGEND, NULL, 0} }; 
            SERDISP_SRVARG rets[] = { { MBUF_ARGBYTE, &(dd->gpevset->gpis[i].id), 0},
                                      { MBUF_ARGSTR, buf_name, sizeof(buf_name)-1 },
                                      { MBUF_ARGSTR, buf_aliasnames, sizeof(buf_aliasnames)-1 },
                                      { MBUF_ARGBYTE, &(dd->gpevset->gpis[i].type), 0},
                                      { MBUF_ARGINT,  &(dd->gpevset->gpis[i].enabled), 0},
                                      { MBUF_ARGLONG, &(dd->gpevset->gpis[i].value), 0},
                                      { MBUF_ARGEND, NULL, 0} };

            if( serdisp_srv_sendcmd(con,MBUF_CMD_GPIDESC,args,rets) < 0 ) {
              sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd 'MBUF_CMD_GPIDESC'", __func__);
              serdisp_freeresources (dd);
              return (serdisp_t*) 0;
            }

            dd->gpevset->gpis[i].name = sdtools_malloc( (size_t)(strlen(buf_name)+1) );
            dd->gpevset->gpis[i].aliasnames = sdtools_malloc( (size_t)(strlen(buf_aliasnames)+1) );
            if ( ! (dd->gpevset->gpis[i].name && dd->gpevset->gpis[i].aliasnames) ) {
              sd_error(SERDISP_ERUNTIME, "%s(): unable to allocate memory for strings", __func__);
              serdisp_freeresources (dd);
              return (serdisp_t*) 0;
            }
            sdtools_strncpy(dd->gpevset->gpis[i].name, buf_name, strlen(buf_name));
            sdtools_strncpy(dd->gpevset->gpis[i].aliasnames, buf_aliasnames, strlen(buf_aliasnames));
            sd_debug( 1, "%s(): fetched GPI[%d]: %s", __func__, i, dd->gpevset->gpis[i].name);
          }
        }

        if (dd->gpevset->amountgpos) {
          dd->gpevset->gpos = sdtools_malloc(sizeof(SDGPO_t) * dd->gpevset->amountgpos);
          if (! dd->gpevset->gpos) {
            sd_debug(0,
              "%s(): cannot allocate memory for GPOs. continuing without support for these ...",
              __func__
            );
            dd->gpevset->amountgpos = 0;
          }

          for (i = 0; i < dd->gpevset->amountgpos; i++) {
            char buf_name[50];
            char buf_aliasnames[256];
            char buf_defines[256];

            SERDISP_SRVARG args[] = { { MBUF_ARGINT, &i, 0 },
                                      { MBUF_ARGEND, NULL, 0} }; 
            SERDISP_SRVARG rets[] = { { MBUF_ARGBYTE, &(dd->gpevset->gpos[i].id), 0},
                                      { MBUF_ARGSTR, buf_name, sizeof(buf_name)-1 },
                                      { MBUF_ARGSTR, buf_aliasnames, sizeof(buf_aliasnames)-1 },
                                      { MBUF_ARGBYTE, &(dd->gpevset->gpos[i].type), 0},
                                      { MBUF_ARGBYTE, &(dd->gpevset->gpos[i].mode), 0},
                                      { MBUF_ARGLONG, &(dd->gpevset->gpos[i].minval), 0},
                                      { MBUF_ARGLONG, &(dd->gpevset->gpos[i].maxval), 0},
                                      { MBUF_ARGSTR, buf_defines, sizeof(buf_defines)-1 },
                                      { MBUF_ARGEND, NULL, 0} };

            if( serdisp_srv_sendcmd(con,MBUF_CMD_GPIDESC,args,rets) < 0 ) {
              sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd 'MBUF_CMD_GPIDESC'", __func__);
              serdisp_freeresources (dd);
              return (serdisp_t*) 0;
            }

            dd->gpevset->gpos[i].name = sdtools_malloc( (size_t)(strlen(buf_name)+1) );
            dd->gpevset->gpos[i].aliasnames = sdtools_malloc( (size_t)(strlen(buf_aliasnames)+1) );
            dd->gpevset->gpos[i].defines = sdtools_malloc( (size_t)(strlen(buf_defines)+1) );
            if ( ! (dd->gpevset->gpos[i].name && dd->gpevset->gpos[i].aliasnames && dd->gpevset->gpos[i].defines) ) {
              sd_error(SERDISP_ERUNTIME, "%s(): unable to allocate memory for strings", __func__);
              serdisp_freeresources (dd);
              return (serdisp_t*) 0;
            }
            sdtools_strncpy(dd->gpevset->gpos[i].name, buf_name, strlen(buf_name));
            sdtools_strncpy(dd->gpevset->gpos[i].aliasnames, buf_aliasnames, strlen(buf_aliasnames));
            sdtools_strncpy(dd->gpevset->gpos[i].defines, buf_aliasnames, strlen(buf_aliasnames));
            sd_debug( 1, "%s(): fetched GPO[%d]: %s", __func__, i, dd->gpevset->gpos[i].name);
          }
        }
      }
    }
  }

  sd_debug( 1, "%s(): remote display: w/h/d: %d/%d/%d", __func__, dd->width, dd->height, dd->depth);


  /* serdisp_remote_setupoptions() needs an initialised display: thus already call MBUF_CMD_DDINIT in here */

  /* TODO: This is a hack, as long as I don't know how to handle local options */
  con->mode = CON_BUFFERED;

  if( serdisp_srv_sendcmd(con,MBUF_CMD_DDINIT,NULL,NULL)<0 ) {
    sd_debug(0, "%s(): error while sending cmd", __func__);
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* parse and set options ... */
  if( serdisp_remote_setupoptions(dd,dispname,optionstring) ) {
    serdisp_freeresources (dd);
    return NULL;
  }

  return dd;
}


/*========================================================================*\
    Init a remote display 
    (actually this is a dummy function because display is already initalised, but do some pre-post-init stuff)
\*========================================================================*/
void serdisp_remote_init(serdisp_t* dd) {
  /* SERDISP_SRVCON *con = ((serdisp_srvdev_t*)dd->sdcd->extra)->con; */

}


/*========================================================================*\
    Update a remote display
\*========================================================================*/
void serdisp_remote_update(serdisp_t* dd) {
  SERDISP_SRVCON *con   = ((serdisp_srvdev_t*)dd->sdcd->extra)->con;
  SERDISP_SRVARG args[] = { { MBUF_ARGBYTES, dd->scrbuf, dd->scrbuf_size }, 
                            { MBUF_ARGBYTES, dd->scrbuf_chg, dd->scrbuf_chg_size }, 
                            { MBUF_ARGEND, NULL, 0} };
  sd_debug(2, "%s(): stated", __func__);

  /* in buffered mode we first need to copy over the buffers ... */
  if( con->mode==CON_BUFFERED ) {
    if( serdisp_srv_sendcmd(con,MBUF_CMD_DDPUTBUFFS,args,NULL)<0 )
      sd_error(SERDISP_ERUNTIME, "%s(): cannot send buffers", __func__);
  }

  /* now we do a remote update.  */
  if( serdisp_srv_sendcmd(con,MBUF_CMD_DDUPDATE,NULL,NULL)<0 ) {
    sd_debug(0, "%s(): error while sending cmd", __func__);
  }

  /* clear local change buffer.... */
  memset(dd->scrbuf_chg, 0, dd->scrbuf_chg_size);
}


/*========================================================================*\
    Set option for a remote display
\*========================================================================*/
int serdisp_remote_setoption( serdisp_t* dd, const char* optionname, long value ) {
  SERDISP_SRVCON *con = ((serdisp_srvdev_t*)dd->sdcd->extra)->con;
  int             retval;
  int             stdidx = serdisp_getstandardoptionindex(optionname);
  serdisp_options_t optiondesc;

  /* options starting with SDR are local scope options */
  if (strncasecmp(optionname, "SDR", 3) == 0) {
    return 1;  /* ignore for now and return success */
  }

  SERDISP_SRVARG args[] = { { MBUF_ARGSTR,  (void*)optionname, 0}, 
                            { MBUF_ARGLONG, &value, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &retval, 0}, 
                            { MBUF_ARGEND, NULL, 0} };

  if ( (stdidx != -1) && 
       serdisp_getoptiondescription(dd, optionname, &optiondesc) &&
       (optiondesc.flag & SD_OPTIONFLAG_LOC)
     ) {
    return serdisp_remote_local_setoption(dd, optionname, value);
  }

  if( serdisp_srv_sendcmd(con,MBUF_CMD_DDSETOPTION,args,rets)<0 ) {
    sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd", __func__);
    return -1;
  }


  /* report return code (and ignore it ...) */
  sd_debug( 2, "%s(\"%s=%ld\"): returned %d", __func__, optionname, value, retval );

  /* return success */
  return 1;
}


/*========================================================================*\
    Get option from a remote display
\*========================================================================*/
long serdisp_remote_getoption( serdisp_t* dd, const char* optionname, int* typesize ) {
/*  int stdidx = serdisp_getstandardoptionindex(optionname);
  serdisp_options_t optiondesc;*/

  SERDISP_SRVCON *con = ((serdisp_srvdev_t*)dd->sdcd->extra)->con;
  char           opt_strval[256];
  long           opt_numval;
  int            typesize_temp = 0;
  char*          ret_str;

  /* options starting with SDR are local scope options */
  if (strncasecmp(optionname, "SDR", 3) == 0) {
    int typesize_temp;
    void* valueptr;
    long retval = -1;

    valueptr = serdisp_remote_getvalueptr(dd, optionname, &typesize_temp);
    switch (typesize_temp) {
      case 0:  retval = (typesize) ? (long)(*((char**)valueptr)) : -1; break;  
      case 1:  retval = *((byte*) valueptr); break;
      case 2:  retval = *((short*) valueptr); break;
      case 4:  retval = *((uint32_t*) valueptr); break;
      case 8:  retval = *((long*) valueptr); break;
      default: retval = -1;
    }

    if (typesize)
      *typesize = typesize_temp;

    return retval;
  }

  SERDISP_SRVARG args[] = { { MBUF_ARGSTR,  (char*) optionname, sizeof(optionname)}, 
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGLONG, &opt_numval, 0}, 
                            { MBUF_ARGSTR, opt_strval, -1}, 
                            { MBUF_ARGINT, &typesize_temp, 0}, 
                            { MBUF_ARGEND, NULL, 0} };

  /*typesize = 0;*/  /* ignore strings at the moment */

  if( serdisp_srv_sendcmd(con,MBUF_CMD_DDGETOPTION,args,rets)<0 ) {
    sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd", __func__);
    return -1;
  }

  /* typesize is requested and needs to be set */
  if (typesize) {
    *typesize = typesize_temp;
  }

  if (typesize_temp == 0) {
    if (typesize) {
      ret_str = (char*)sdtools_malloc(strlen(opt_strval)+1);
      sdtools_strncpy(ret_str, opt_strval, strlen(opt_strval));
      return (long)ret_str;
    } else {
      return -1;
    }
  }
  return opt_numval;
}


/*========================================================================*\
    Close a remote display
    Note: the server will simply clear it...
\*========================================================================*/
void serdisp_remote_close(serdisp_t* dd) {
  SERDISP_SRVCON *con = ((serdisp_srvdev_t*)dd->sdcd->extra)->con;

  /* simply send the close command to server ... */
 if( serdisp_srv_sendcmd(con,MBUF_CMD_DDCLOSE,NULL,NULL)<0 ) {
   sd_debug(0, "%s(): error while sending cmd", __func__);
 }

  sd_debug(2, "%s(): done with closing", __func__);
}


/*========================================================================*\
    Set a pixel on the remote display
\*========================================================================*/
void serdisp_remote_setsdpixel( serdisp_t* dd, int x, int y, SDCol sdcol ) {
  SERDISP_SRVCON *con = ((serdisp_srvdev_t*)dd->sdcd->extra)->con;
  sd_debug(2, "%s(%d,%d,%d): started in mode %d", 
               __func__, x, y, sdcol, con->mode );

  /* in buffered and writethrough-mode use the local buffer ... */
  if( con->mode==CON_BUFFERED || con->mode==CON_WRITETHROUGH ) {
    if (dd->colour_spaces & SD_CS_SCRBUFCUSTOM)
      sdtools_generic_setsdpixel_greyhoriz( dd, x, y, sdcol );
    else
      sdtools_generic_setsdpixel( dd, x, y, sdcol );
  }

  /* ... remotely set the pixel in transparent or writethrough mode */
  if( con->mode==CON_TRANSPARENT || con->mode==CON_WRITETHROUGH ) {
    SERDISP_SRVARG args[] = { { MBUF_ARGINT,  &x, 0}, 
                              { MBUF_ARGINT,  &y, 0}, 
                              { MBUF_ARGLONG, &sdcol, 0}, 
                              { MBUF_ARGEND, NULL, 0} }; 
    if( serdisp_srv_sendcmd(con,MBUF_CMD_DDSETSDPIXEL,args,NULL)<0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd", __func__);
      return;
    }
  }
}


/*========================================================================*\
    Get a pixel on the remote display
\*========================================================================*/
SDCol serdisp_remote_getsdpixel( serdisp_t* dd, int x, int y ) {
  SERDISP_SRVCON *con = ((serdisp_srvdev_t*)dd->sdcd->extra)->con;
  SDCol          sdcol;

  /* in buffered mode use the local buffer ... */
  if( con->mode==CON_BUFFERED ) {
    if (dd->colour_spaces & SD_CS_SCRBUFCUSTOM)
      sdcol = sdtools_generic_getsdpixel_greyhoriz( dd, x, y );
    else
      sdcol = sdtools_generic_getsdpixel( dd, x, y );
  }
  /* ... else get pixel from remote peer ... */
  else {
    SERDISP_SRVARG args[] = { { MBUF_ARGINT,  &x, 0}, 
                              { MBUF_ARGINT,  &y, 0}, 
                              { MBUF_ARGEND, NULL, 0} }; 
    SERDISP_SRVARG rets[] = { { MBUF_ARGLONG, &sdcol, 0}, 
                              { MBUF_ARGEND, NULL, 0} }; 
    if( serdisp_srv_sendcmd(con,MBUF_CMD_DDGETSDPIXEL,args,rets)<0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd", __func__);
      return 0;
    }
  }

  sd_debug(2, "%s(%d,%d): done (colour: %d)", __func__, x, y, sdcol);
  return sdcol;
}


/* *********************************
   int serdisp_remote_setupoptions(dd, dispname, optionstring)
   *********************************
   set up options (indiv. one because we can't use generic serdisp_setupoptions())
   *********************************
   dd                  ... display descriptor
   dispname            ... display name
   optionstring        ... string with options set by user / outside
   *********************************
   returns 0 if everything was ok or 1 if an error ocurred
*/
int serdisp_remote_setupoptions(serdisp_t* dd, const char* dispname, const char* optionstring) {
  char* optionptr;
  int optionlen;
  int optionborder;
  char buffer[50];  /* needed for error messages */

  /* ignore empty option string */
  if (strlen(optionstring) < 1) 
    return 0;

  optionptr = (char*) optionstring;
  optionlen = -1;
  optionborder = strlen(optionptr);

  /* parse option string */
  while( (optionptr = sdtools_nextpattern(optionptr, ';', &optionlen, &optionborder)) ) {
    char* valueptr = 0;
    /*int valuelen = 0;*/
    char* idxpos = strchr(optionptr, '=');
    int keylen = optionlen;

    int stdoptidx;   /* index of an option found in standard options */
    int foundstdopt;

    int optidx;      /* index of an option found in driver options array */
    int foundopt;

    char* optvalueptr;
    /*int optvalueptrlen;*/
    int usestdopt;

    serdisp_options_t curroption;
    /*int rc_getopt;*/

    long optvalue;
    int rc_scan;

    /* split key=value */

    /* '=' found and position not outside optionlen? */
    if (idxpos &&  serdisp_ptrstrlen(idxpos, optionptr) < optionlen ) {
      keylen = serdisp_ptrstrlen(idxpos, optionptr);
      valueptr = ++idxpos;
      /*valuelen = optionlen - keylen - 1;*/
    } else {  /* invalid option */
      snprintf(buffer, ( (keylen >= 49) ? 50 : keylen+1), "%s", optionptr);
      sd_error(SERDISP_EINVAL, "error in option string: no value given for option %s", buffer );
      return 1;
    }

    optvalueptr = valueptr;
    /*optvalueptrlen = valuelen;*/

    /* ignore wiring definitions */
    if ( strncasecmp(optionptr, "WIRING=", strlen("WIRING=")) == 0 ||
         strncasecmp(optionptr, "WIRE=", strlen("WIRE=")) == 0
       ) {
      continue;
    }

    /* look if option is found in standard options array */
    stdoptidx = serdisp_getstandardoptionindex(optionptr);
    foundstdopt = (stdoptidx >= 0) ? 1 : 0;

    /* look if option is found in driver options array */
    optidx = serdisp_getoptionindex(dd, optionptr);
    foundopt = (optidx >= 0) ? 1 : 0;

    /* serdisp_getoptiondescription() auto-ignores everything after '=' */
    /* rc_getopt = */
    serdisp_getoptiondescription(dd, optionptr, &curroption);
    /* is option a driver independend standard option? */
    usestdopt = foundstdopt; /*(foundstdopt && (curroption.flag & SD_OPTIONFLAG_STD));*/

    rc_scan = serdisp_scanoptvalue(dd, optionptr, optvalueptr, &optvalue);

    if (rc_scan < 0)
      continue;

    /* only process local options in here, all other are sent to the server */
    if ( (usestdopt && curroption.flag & SD_OPTIONFLAG_LOC) ||
         (foundopt && dd->options[foundopt].flag & SD_OPTIONFLAG_LOC)
       ) {

      if (foundstdopt) {
        /* no string values accepted for standard options*/
        if (rc_scan > 0) {
          snprintf(buffer, ( (optionlen >= 49) ? 50 : optionlen+1), "%s", optionptr);
          sd_error(SERDISP_EINVAL, "string value not allowed for standard option '%s'", buffer );
          return 1;
        }
        if (strcasecmp(curroption.name, "INVERT") == 0) {
          dd->curr_invert = (int)optvalue;
        } else if (strcasecmp(curroption.name, "ROTATE") == 0) {
          dd->curr_rotate = sdtools_rotate_deg2intern(dd, (int)optvalue);
        } else if (strcasecmp(curroption.name, "PROCCMDPORT") == 0) {
          if (dd->gpevset)
            dd->gpevset->cmdproc_port = (int)optvalue;
        }
      }
    } else {
      snprintf(buffer, ( (keylen >= 49) ? 50 : keylen+1), "%s", optionptr);
      serdisp_setoption(dd, buffer, optvalue);
    }
  }

  return 0;
}


int  serdisp_remote_local_setoption(serdisp_t* dd, const char* optionname, long value) {
  int idx = serdisp_getstandardoptionindex(optionname);

  /* non existing option: error */
  if (idx == -1)
    return 0;

  if (idx == serdisp_getstandardoptionindex("ROTATE") ) {
    int oldval = dd->curr_rotate;

    int newval = sdtools_rotate_deg2intern(dd, (int)value);

    /* rotate content only when 180 degree rotation (else clear display) */
    if (oldval != newval) {
      if ((oldval & 0x02) == (newval & 0x02))
        sdtools_generic_rotate(dd);
      else
        serdisp_clear(dd);

      dd->curr_rotate = newval;
    }
  }
  return 1;
}
