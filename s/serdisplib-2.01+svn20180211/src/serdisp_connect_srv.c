/*
 *************************************************************************
 *
 * serdisp_connect_srv.c
 * routines for accessing remote devices via a serdispd server
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 * copyright (C) 2006-2010  wolfgang astleitner
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

/*#define DEBUG_SIGNALS 1*/

#include "../config.h"

#include <syslog.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>        /* isspace */

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_srvtools.h"
#include "serdisplib/serdisp_connect_srv.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"


/* *********************************
   serdisp_CONN_t* SDCONNsrv_open(sdcdev)
   *********************************
   opens an remote device for a serdisp lcd
   *********************************
   sdcdev   ... device identifier. format:

        devid = 'SRV:' lcdname '@' srvname [':'port]

      lcdname - Name of LCD as given in server configuration file
      srvname - Server name or address
      port    - Server port (if not default)

               examples:
                "SRV:lcd1@192.168.1.1"
                "SRV:lcd2@localhost"
                "SRV:lcd2@myhost.mydomain:12532"

   *********************************
   returns a serdisp connect descriptor or (serdisp_CONN_t*)0 if operation was unsuccessful

*/
serdisp_CONN_t* SDCONNsrv_open( const char sdcdev[] ) {
  serdisp_CONN_t*    sdcd = 0;
  char              *lcdname = strdup(sdcdev+4);
  char              *hostname;
  char              *idx, *eptr;
  int                port;
  SERDISP_SRVCON    *con; 
  serdisp_srvdev_t  *extra;

  /* check if strdup was successful in allocating memory*/
  if( !lcdname ) {
    sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for device string", __func__);
    return NULL;
  }

/*-------------------------------------------------------------------------*\
	Parse device string
\*-------------------------------------------------------------------------*/
  idx = strchr( lcdname, '@' );
  if( !idx ) {
    sd_error(SERDISP_ENXIO, "%s(): invalid protocol (missing '@')", __func__);
    free( lcdname );
    return NULL;
  }
  *idx = '\0';   /* replace '@' through '\0' */
  hostname = ++idx;
  idx = strchr( hostname, ':' );
  if( idx ) {
    *idx = '\0';   /* replace ':' through '\0' */
    port = (int) strtol( idx+1, &eptr, 10 );
    while( isspace(*eptr) )
    eptr++;
    if( *eptr || port<=0 ) {
      sd_error(SERDISP_ENXIO, "%s(): invalid protocol (bad port)", __func__);
      free( lcdname );
      return NULL;
    }
  } else {
    port = SD_SRV_DEFPORT;
  }

/*-------------------------------------------------------------------------*\
	Connect to server
\*-------------------------------------------------------------------------*/
  con = serdisp_srv_connect( hostname, port );
  if( !con ) {
    free( lcdname );
    return NULL;
  }
  sd_debug( 1, "%s(): connected to %s:%d", __func__, hostname, port );

/*-------------------------------------------------------------------------*\
	Set display
\*-------------------------------------------------------------------------*/
  {
    int  retval;
    char retstr[256];
    SERDISP_SRVARG args[] = { { MBUF_ARGSTR, lcdname, 0}, 
                              { MBUF_ARGEND, NULL, 0} }; 
    SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &retval, 0},
                              { MBUF_ARGSTR, retstr, sizeof(retstr) }, 
                              { MBUF_ARGEND, NULL, 0} };
    if( serdisp_srv_sendcmd(con,MBUF_CMD_ATTACH,args,rets)<0 ) {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot send cmd", __func__);
      free( lcdname );
      return NULL;
    }
    if( retval ) {
      sd_error(SERDISP_ERUNTIME, "%s(): %s", __func__, retstr );
      free( lcdname );
      return NULL;
    }
  }
  free( lcdname );

/*-------------------------------------------------------------------------*\
	Set up connection decriptor
\*-------------------------------------------------------------------------*/
  sdcd = (serdisp_CONN_t*)sdtools_malloc( sizeof(serdisp_CONN_t) );
  if( !sdcd ) {
    sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd", __func__);
    return NULL;
  }
  memset( sdcd, 0, sizeof(serdisp_CONN_t) );
  sdcd->sdcdev       = (char*)sdcdev;
  sdcd->conntype     = SERDISPCONNTYPE_SERVER;
  sdcd->hardwaretype = SDHWT_REMOTE;

  sdcd->extra = (void*)sdtools_malloc(sizeof(serdisp_srvdev_t));
  if( !sdcd->extra ) {
    sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd->extra", __func__);
    free( sdcd );
    return NULL;
  }
  memset( sdcd->extra, 0, sizeof(serdisp_srvdev_t) );
  extra = (serdisp_srvdev_t*)(sdcd->extra);

  extra->con = con;

  return sdcd;
}


/* *********************************
   void SDCONNsrv_close(sdcd)
   *********************************
   close the device occupied by serdisp
   *********************************
   sdcd     ... serdisp connect descriptor
   *********************************
*/
void SDCONNsrv_close(serdisp_CONN_t* sdcd) {
  serdisp_srv_close( ((serdisp_srvdev_t*)sdcd->extra)->con );
  free( sdcd->extra );
}


