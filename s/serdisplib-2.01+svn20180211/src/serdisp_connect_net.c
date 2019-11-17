/*
 *************************************************************************
 *
 * serdisp_connect_net.c
 * routines for accessing displays over a network-based protocol
 *
 *************************************************************************
 *
 * heavily based on serdisp_connect_srv.c and serdisp_srvtools.c (//MAF)
 *
 * copyright (C) 2011-2018   wolfgang astleitner
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
//#include <ctype.h>        /* isspace */
#include <sys/types.h>
#include <sys/socket.h>


#include "serdisplib/serdisp_connect.h"
//#include "serdisplib/serdisp_srvtools.h"
#include "serdisplib/serdisp_connect_net.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"

#include "serdisplib/serdisp_fctptr.h"


/* *********************************
   serdisp_CONN_t* SDCONNnet_open(sdcdev)
   *********************************
   opens an remote device for a serdisp lcd
   *********************************
   sdcdev   ... device identifier. format:

        devid = 'NET:' srvname [':'port]

      srvname - Server name or address
      port    - Server port (if not given: default port)

               examples:
                "NET:192.168.1.1"
                "NET:localhost"
                "NET:myhost.mydomain:12532"

   *********************************
   returns a serdisp connect descriptor or (serdisp_CONN_t*)0 if operation was unsuccessful

*/
serdisp_CONN_t* SDCONNnet_open( const char sdcdev[] ) {
  serdisp_CONN_t*    sdcd = 0;
  char*              hostname;
  char*              idx;
  int                port = SD_NET_DEFPORT;
  //serdisp_srvdev_t  *extra;
  struct hostent    *hostptr;
  struct sockaddr_in srvaddr;
  int                fd;
  int                rc;

  /* parse device string */
  idx = strchr( &sdcdev[4], ':' );
  if( idx ) {
    hostname = sdtools_malloc(idx - &sdcdev[4] + 1);
    if (hostname) {
      long tempport;
      sdtools_strncpy(hostname, &sdcdev[4], idx - &sdcdev[4]);
      rc = sdtools_strtol(idx+1, '\0', 10, &tempport);
      port = (int)tempport;
      /* verify if port containes a valid number */
      if ((port <= 0) || (rc != 1)) {
        sd_error(SERDISP_ENXIO, "%s(): invalid protocol (bad port)", __func__);
        free( hostname );
        return NULL;
      }
    } else {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate memory for hostname", __func__);
      return NULL;
    }
  } else {
    hostname = sdtools_malloc(strlen(&sdcdev[4])+1);
    if (hostname) {
      sdtools_strncpy(hostname, &sdcdev[4], strlen(&sdcdev[4]) );
      port = SD_NET_DEFPORT;
    } else {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate memory for hostname", __func__);
      return NULL;
    }
  }

  /* connect to server */

  /* get server address */
  memset( &srvaddr, 0, sizeof(srvaddr) );
  sd_debug( 2, "%s(): resolving host '%s'", __func__, hostname );
  hostptr = fp_gethostbyname( hostname );
  if( !hostptr ) {
    sd_error(SERDISP_ENXIO, "%s(): cannot get host by name", __func__);
    return NULL;
  }
  bcopy( hostptr->h_addr, (char*)&srvaddr.sin_addr, hostptr->h_length );
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port   = fp_htons( port );

  /* create socket and connect to server */
  sd_debug( 2, "%s(): connecting to %s:%d", __func__, hostname, port );
  fd = fp_socket( AF_INET, SOCK_STREAM, 0 );
  if( fd<0 ) {
    sd_error(SERDISP_ENXIO, "%s(): could not open socket", __func__);
    return NULL;
  }
  if( fp_connect(fd,(struct sockaddr*)&srvaddr,sizeof(srvaddr)) )  {
    sd_error(SERDISP_ENXIO, "%s(): could not connect", __func__);
    perror("connect");
    close(fd);
    return NULL;
  }
  sd_debug( 1, "%s(): connected to %s:%d", __func__, hostname, port );


  /* set up connection decriptor */
  sdcd = (serdisp_CONN_t*)sdtools_malloc( sizeof(serdisp_CONN_t) );
  if( !sdcd ) {
    sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd", __func__);
    return NULL;
  }
  memset( sdcd, 0, sizeof(serdisp_CONN_t) );
  sdcd->sdcdev       = (char*)sdcdev;
  sdcd->conntype     = SERDISPCONNTYPE_INET;
  sdcd->hardwaretype = SDHWT_INET;
  /*fprintf(stderr, "assigning fd: %d\n", fd);*/
  sdcd->fd = fd;

#if 0
  sdcd->extra = (void*)sdtools_malloc(sizeof(serdisp_srvdev_t));
  if( !sdcd->extra ) {
    sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd->extra", __func__);
    free( sdcd );
    return NULL;
  }
  memset( sdcd->extra, 0, sizeof(serdisp_srvdev_t) );
  extra = (serdisp_srvdev_t*)(sdcd->extra);

  extra->con = con;
#endif

  return sdcd;
}


/* *********************************
   void SDCONNnet_close(sdcd)
   *********************************
   close the device occupied by serdisp
   *********************************
   sdcd     ... serdisp connect descriptor
   *********************************
*/
void SDCONNnet_close(serdisp_CONN_t* sdcd) {
  close( sdcd->fd );
  /*free( sdcd->extra );*/
}


