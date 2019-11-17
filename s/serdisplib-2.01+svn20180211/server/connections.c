/*
 *************************************************************************
 *
 * connections.c
 * handles connections of the serdisplib server
 * called by serdispd (main module)
 * calls UNIX IP functions
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance:
 * copyright (C) 2006-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *************************************************************************
 *
 * updates:
 * 2006-05-13 WA:  * included #include <netinet/in.h> to make freebsd/pcbsd happy
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
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "serdisplib/serdisp.h"

#include "serdisplib/serdisp_fctptr.h"

#include "serdisplib/serdisp_srvtools.h"
#include "serdisplib/serdisp_gpevents.h"
#include "connections.h"
#include "commands.h"
#include "lcds.h"
#include "lirc.h"



/*=========================================================================*\
	Global symbols
\*=========================================================================*/


/*=========================================================================*\
	Private symbols
\*=========================================================================*/
typedef struct _ipfilter {
  struct _ipfilter *next;
  uint32_t          mask;
  uint32_t          value;
  int               mode;
} IPFILTER;

/*------------------------------------------------------------------------*\
    Some symbols
\*------------------------------------------------------------------------*/  
static IPFILTER *ipfilters;      /* linked list of filter entries      */
static CONDSC   *connections;    /* root of linked list of connections */
/* static int       fd_lst;    */     /* the listener                       */


/*=========================================================================*\
        Add an entry to the IP filter list
\*=========================================================================*/
int add_ipfilter( int mode, char *name ) {
  int        a, b, c, d, e;
  uint32_t   value;
  uint32_t   mask = 0;
  IPFILTER  *fltr, 
            *newfltr;

/*------------------------------------------------------------------------*\
    Format a.b.c.d/d
\*------------------------------------------------------------------------*/  
  if( sscanf(name,"%d.%d.%d.%d/%d",&a,&b,&c,&d,&e)==5 ) {
    if( a<0 || a>255 || b<0 || b>255 || c<0 || c>255 || d<0 || d>255 || e<0 || e>32 )
      return -1;
    value = (a<<24) + (b<<16) + (c<<8) + d;
    while( e-- )
      mask = 0x80000000 | (mask>>1);
  }

/*------------------------------------------------------------------------*\
    Format a.b.c.d
\*------------------------------------------------------------------------*/  
  else if( sscanf(name,"%d.%d.%d.%d",&a,&b,&c,&d)==4 ) {
    if( a<0 || a>255 || b<0 || b>255 || c<0 || c>255 || d<0 || d>255 )
      return -1;
    value = (a<<24) + (b<<16) + (c<<8) + d;
    mask = 0xffffffff;
  } 

/*------------------------------------------------------------------------*\
    unknown format
\*------------------------------------------------------------------------*/  
  else {
    sd_srvmsg( LOG_ERR, "Invalid adress mask format \"%s\"", name );  
    return -1;
  }

/*------------------------------------------------------------------------*\
    Create and fill new filter descriptor ...
\*------------------------------------------------------------------------*/
  newfltr = calloc( 1, sizeof(IPFILTER) );
  if( !newfltr ) {
    sd_srvmsg( LOG_ERR, "out of memory!" );
    return -1;
  }
  newfltr->mask  = mask;
  newfltr->value = value & mask;
  newfltr->mode  = mode;

/*------------------------------------------------------------------------*\
    ... and link it to the end of the list.
\*------------------------------------------------------------------------*/
  if( !ipfilters )
    ipfilters = newfltr;
  else {
    for( fltr=ipfilters; fltr->next; fltr=fltr->next );
    fltr->next = newfltr;
  }

/*------------------------------------------------------------------------*\
    Some debugging info and that's it ...
\*------------------------------------------------------------------------*/
  sd_debug( 2, "Added IP filter %c 0x%08x / 0x%08x", 
               (mode==CONALLOW)?'+':'-', newfltr->value, mask );  
  return 0;
}


/*=========================================================================*\
        Check if a client fits the IP filter
\*=========================================================================*/
int check_ipfilter( struct sockaddr_in *addr_in ) {
  uint32_t   addr;
  IPFILTER  *fltr;

/*------------------------------------------------------------------------*\
    Get address in local byte order from socket address
\*------------------------------------------------------------------------*/
  addr = fp_ntohl( addr_in->sin_addr.s_addr );

/*------------------------------------------------------------------------*\
    Walk trough the list and return mode for first matching entry
\*------------------------------------------------------------------------*/
  for( fltr=ipfilters; fltr; fltr=fltr->next ) {
    if( (addr&fltr->mask) == fltr->value )
      return fltr->mode;
  }

/*------------------------------------------------------------------------*\
    Default: No fit
\*------------------------------------------------------------------------*/  
  return CONDENY;
}



/* *********************************
   int create_portlistener (port)
   *********************************
   create a port-based listener
   *********************************
   port     ... port number
   *********************************
   returns >= 0 if success or < 0 if failed
*/
int create_portlistener( int port ) {
  int sock;
  int on = 1;
  struct sockaddr_in addr;

  /* Get a socket */
  sd_debug( 1, "%s(): creating listener on socket %d", __func__, port);
  sock = fp_socket( AF_INET, SOCK_STREAM, 0 );
  if (sock < 0) {
    sd_srvmsg( LOG_ERR, "%s(): error opening listener socket: %s", __func__, strerror(errno) );
    return -1;
  }

  /* Enable address reuse (helps on restarting crashed servers) */
  if( fp_setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on)) < 0 )
    sd_srvmsg( LOG_ERR, "%s(): error in setsockopt(): %s", __func__, strerror(errno) );

  /* Bind a port to this socket */
  memset( &addr, 0, sizeof(addr) );
  addr.sin_family = AF_INET;
  addr.sin_port   = fp_htons( port );
  if( fp_bind(sock,(struct sockaddr *)&addr,sizeof(addr)) ) {
    sd_srvmsg( LOG_ERR, "%s(): could not bind to port %d: %s", __func__, port, strerror(errno) );
    return -1;
  }

  /* and now listen ... */
  if( fp_listen(sock,16) < 0 ) {
    sd_srvmsg( LOG_ERR, "%s(): error in listen(): %s", __func__, strerror(errno) );
    return -1;
  }

  return sock;
}



/* *********************************
   int create_devicelistener (path)
   *********************************
   create a device file based listener
   *********************************
   path     ... device file name
   *********************************
   returns >= 0 if success or < 0 if failed
*/
int create_devicelistener( const char* path ) {
  mode_t             oldmask;
  int                sock;
  struct sockaddr_un addr;

  /* safety exit */
  if (strlen(path) <= 0)
    return -1;

  /* Get a socket */
  sd_debug( 1, "%s(): creating listener on socket '%s'", __func__, path);

  unlink( path );
  sock = fp_socket( AF_UNIX, SOCK_STREAM, 0);
  if ( sock < 0 ) {
    sd_srvmsg( LOG_ERR, "%s(): error opening listener socket: %s", __func__, strerror(errno) );
    return -1;
  }

  if( strlen(path) >= sizeof(addr.sun_path) ) {
    sd_srvmsg( LOG_ERR, "%s(): pathname for socket too long (max. %d chars): %s",
               __func__, (int)sizeof(addr.sun_path), path );
    return -1;
  }

  memset( &addr, 0, sizeof(addr) );
  addr.sun_family = AF_UNIX;
  strcpy( addr.sun_path, path );

  oldmask = umask( 0 );
  if( fp_bind(sock,(struct sockaddr *)&addr,sizeof(addr)) ) {
    sd_srvmsg( LOG_ERR, "%s(): could not bind to socket '%s': %s", __func__, path, strerror(errno) );
    umask( oldmask );
    return -1;
  }
  umask( oldmask );

  if( fp_listen(sock,16) < 0 ) {
    sd_srvmsg( LOG_ERR, "%s(): error in listen(): %s", __func__, strerror(errno) );
    return -1;
  }

  return sock;
}




/*=========================================================================*\
        Listen on open sockets
\*=========================================================================*/
int do_select( int fd_lst, int ms ) {
  struct timeval timeout;
  fd_set     readfds;
  fd_set     writefds;
  fd_set     exceptfds;
  CONDSC    *con;
  LCDDSC    *lcd;
  lirccon_t *lcon, *lconnext;
  int        fd_max = fd_lst;

/*------------------------------------------------------------------------*\
    Init timeout structure
\*------------------------------------------------------------------------*/  
  timeout.tv_sec  = ms / 1000;
  timeout.tv_usec = ms - 1000*timeout.tv_sec;
  
/*------------------------------------------------------------------------*\
    Init filedescriptor sets with listeners, connections and input handlers
\*------------------------------------------------------------------------*/  
  FD_ZERO( &readfds);             
  FD_ZERO( &writefds);
  FD_ZERO( &exceptfds);
  FD_SET( fd_lst, &readfds);
  for( con=connections; con; con=con->next ) {
    if( con->con.fd>fd_max )
      fd_max = con->con.fd;    
    if( con->con.writebuf.status && con->con.writebuf.status!=MBUF_COMPLETE ) 
      FD_SET( con->con.fd, &writefds );
    else
      FD_SET( con->con.fd, &readfds );
    FD_SET( con->con.fd, &exceptfds );
  }
  for( lcd=lcd_getfirst(); lcd; lcd=lcd->next ) {
    int fd;
    
    if( !lcd->lirc )
      continue;
/* original code: maf
    if( serdisp_recvgetfd(lcd->dd)>=0 ) {
      FD_SET( serdisp_recvgetfd(lcd->dd), &readfds);
      if( serdisp_recvgetfd(lcd->dd)>fd_max )
        fd_max = serdisp_recvgetfd( lcd->dd );
    }
*/    
/* adapted code: wastl */
    fd = SDGPI_getstreamfd(lcd->dd, SDGPI_search(lcd->dd, "IR"));  /* get input stream */
    if( fd >=0 ) {
      FD_SET( fd, &readfds);
      if( fd > fd_max )
        fd_max = fd;
    }
/* --- */        
    if( lcd->lirc->in_fd>=0 ) {
      FD_SET( lcd->lirc->in_fd, &readfds);
      if( lcd->lirc->in_fd>fd_max )
        fd_max = lcd->lirc->in_fd;
    }
    if( lcd->lirc->un_fd>=0 ) {
      FD_SET( lcd->lirc->un_fd, &readfds);
      if( lcd->lirc->un_fd>fd_max )
        fd_max = lcd->lirc->un_fd;
    }
    for( lcon=lcd->lirc->connections; lcon; lcon=lcon->next ) {
      FD_SET( lcon->fd, &readfds );
      if( lcon->fd>fd_max )
        fd_max = lcon->fd;
    }
  }

/*------------------------------------------------------------------------*\
    now do the select 
\*------------------------------------------------------------------------*/
  /* sd_debug( 2, "do_select(): select with timeout of %dms", ms ); */ 
  if( select(fd_max+1,&readfds,&writefds,&exceptfds,&timeout)<0 ) {
    sd_srvmsg( LOG_ERR, "Error in select: %s", strerror(errno) );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Record flags for existing connections
\*------------------------------------------------------------------------*/  
  for( con=connections; con; con=con->next ) {
    con->con.status  = FD_ISSET(con->con.fd,&readfds)  ? CON_SELRD   : 0;
    con->con.status |= FD_ISSET(con->con.fd,&writefds) ? CON_SELWRT  : 0;
    con->con.status |= FD_ISSET(con->con.fd,&exceptfds)? CON_SELEXPT : 0;
    sd_debug( 2, "so_select(): fd %d, flags 0x%02x", con->con.fd, con->con.status );
  }

/*------------------------------------------------------------------------*\
    New connections or lirc messages comming in?
\*------------------------------------------------------------------------*/  
  if( FD_ISSET(fd_lst,&readfds) ) {
    if( accept_connection(fd_lst) )
      return -1;
  }
  for( lcd=lcd_getfirst(); lcd; lcd=lcd->next ) {
    if( !lcd->lirc )
      continue;
    if( lcd->lirc->in_fd>=0 ) 
      if( FD_ISSET(lcd->lirc->in_fd,&readfds) )
 	if( lirc_acceptconnection(lcd->lirc,lcd->lirc->in_fd, lcd->name) )
          return -1;
    if( lcd->lirc->un_fd>=0 ) 
      if( FD_ISSET(lcd->lirc->un_fd,&readfds) )
 	if( lirc_acceptconnection(lcd->lirc,lcd->lirc->un_fd, lcd->name) )
          return -1;
    for( lcon=lcd->lirc->connections; lcon; lcon=lconnext ) {
      lconnext = lcon->next;          /* We might loose con in loop body! */
      if( FD_ISSET(lcon->fd,&readfds) )
        if( lirc_readmsg(lcd->lirc,lcon) )
          return -1;
    }
/* original code: maf
    if( serdisp_recvgetfd(lcd->dd)>=0 ) {
      if( FD_ISSET(serdisp_recvgetfd(lcd->dd),&readfds) )
        if( lirc_handleinput(lcd) )
          return -1;
    }
*/

    
/* adapted code: wastl */
    /* WA: anonymous block */ 
    {    
      int fd;
      fd = SDGPI_getstreamfd(lcd->dd, SDGPI_search(lcd->dd, "IR"));  /* get input stream */

      if( fd >= 0 ) {
        if( FD_ISSET(fd,&readfds) )
          if( lirc_handleinput(lcd) )
            return -1;
      }
    }
/* --- */
  }

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Accept a new connection
\*=========================================================================*/
int accept_connection( int fd_lst ) {
  struct sockaddr rmadr;
  socklen_t       rmadrl = sizeof(rmadr);
  char            rmname[1024];
  CONDSC         *con;
  int             fd;
  int             flags;

/*------------------------------------------------------------------------*\
    Accept connection
\*------------------------------------------------------------------------*/  
  fd = fp_accept( fd_lst, &rmadr, &rmadrl );
  if( fd<0 ) {
    sd_srvmsg( LOG_ERR, "Error in accept: %s", strerror(errno) );
    return -1;
  }
  if( fd>=FD_SETSIZE ) {
    sd_srvmsg( LOG_ERR, "File descriptor %d to large (too many connections?)", fd );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Get identity of peer
\*------------------------------------------------------------------------*/
  if( fp_getnameinfo(&rmadr,rmadrl,rmname,sizeof(rmname),NULL,0,0) ) {
    sd_srvmsg( LOG_ERR, "Could not get peer name: %s", strerror(errno) );
    return -1;
  }
  sd_srvmsg( LOG_NOTICE, "Connection initiated from %s", rmname );

/*------------------------------------------------------------------------*\
    Check for address mask
\*------------------------------------------------------------------------*/
  if( check_ipfilter((struct sockaddr_in*)&rmadr)!=CONALLOW ) {
    sd_srvmsg( LOG_NOTICE, "Rejected connection from %s", rmname );
    close( fd );
    return 0;
  }

/*------------------------------------------------------------------------*\
    Create, fill and link new connection descriptor
\*------------------------------------------------------------------------*/
  con = calloc( 1, sizeof(CONDSC) );
  if( !con ) {
    sd_srvmsg( LOG_ERR, "out of memory!" );
    return -1;
  }
  con->con.fd       = fd;
  con->con.name     = strdup(rmname);
  con->con.timestamp= time(NULL);
  con->next         = connections;
  connections       = con;

/*------------------------------------------------------------------------*\
    enable nonblocking ...
\*------------------------------------------------------------------------*/  
  flags = fcntl( fd, F_GETFL );
  fcntl( fd, F_SETFL, flags|O_NONBLOCK );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  sd_debug( 2, "New connection from %s with handle %d", rmname, fd );
  return 0;
}


/*=========================================================================*\
        Close one or all connections
\*=========================================================================*/
int close_connection( CONDSC *delcon ) {
  CONDSC *con;
  CONDSC *prev = NULL;

/*------------------------------------------------------------------------*\
    Close all?
\*------------------------------------------------------------------------*/  
  if( !delcon ) {
    while( connections )
      close_connection( connections );
    return 0;
  }

/*------------------------------------------------------------------------*\
    Find entry in list and unlink it
\*------------------------------------------------------------------------*/  
  for( con=connections; con&&con!=delcon; prev=con,con=con->next );
  if( prev )
    prev->next = delcon->next;
  else
    connections = delcon->next;

/*------------------------------------------------------------------------*\
    Close socket and free buffers
\*------------------------------------------------------------------------*/  
  sd_debug( 2, "Closing connection %d", delcon->con.fd );
  close( delcon->con.fd );
  Sfree( delcon->con.readbuf.buffer );
  Sfree( delcon->con.writebuf.buffer );
  free( delcon );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Handle incomming requests
\*=========================================================================*/
int handle_requests( void ) {
  CONDSC *con;
  CONDSC *next;
  int     requests = 0;

/*------------------------------------------------------------------------*\
    Loop over all connections and handle communication requests.
    Also handle closed connections
\*------------------------------------------------------------------------*/  
  con = connections; 
  while( con ) {
    next = con->next;          /* We could loose con in the loop body ... */
    if( con->con.status&CON_SELRD ) {
      int bytes = serdisp_srv_doreadbuf( &con->con.readbuf, con->con.fd );
      /* Mask blocking: Could happen, if exactly the header was read and 
                      then the payload is missing (socket is nonblocking) */
      if( bytes<0 && errno!=EAGAIN ) {
        if( errno!=ECONNRESET )
          sd_srvmsg( LOG_ERR, "Error reading buffer for handle %d: %s\n", 
                     con->con.fd, sd_geterrormsg() );
        if( close_connection(con) )
          return -1;
      }
    }
    if( con->con.status&CON_SELWRT ) {
      int bytes = serdisp_srv_dowritebuf( &con->con.writebuf, con->con.fd );
      /* Mask blocking */
      if( bytes<0 && errno!=EAGAIN ) {
        if( errno!=ECONNRESET )
          sd_srvmsg( LOG_ERR, "Error writing buffer for handle %d: %s\n", 
                     con->con.fd, sd_geterrormsg() );
        if( close_connection(con) )
          return -1;
      }
    }
    con = next;
  } /* End of: while( con ) */

/*------------------------------------------------------------------------*\
    Loop over all connections and handle complete input buffers
\*------------------------------------------------------------------------*/  
  con = connections;
  while( con ) {
    next = con->next;          /* We could loose con in the loop body ... */
    if( con->con.readbuf.status==MBUF_COMPLETE  ) {
      if( cmd_execute(con) )
        return -1;
      con->con.readbuf.status = MBUF_VIRGIN;
    }
    con = next;
  } /* End of: while( con ) */

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  if( requests )
    sd_debug( 2, "Processed %d requests", requests );
  return 0;
}


/*=========================================================================*\
        Attach a display to a connection
\*=========================================================================*/
int attach_display( CONDSC *con, const char *lcdname ) {
  LCDDSC *lcd;
  CONDSC *c;

/*------------------------------------------------------------------------*\
    Display already defined?
\*------------------------------------------------------------------------*/  
  if( con->lcd ) {
    sd_error( SERDISP_ERUNTIME, "Display already defined for this connection" );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Get display descriptor ...
\*------------------------------------------------------------------------*/  
  lcd = lcd_getbyname( lcdname );
  if( !lcd ) {
    sd_error( SERDISP_ERUNTIME, "No display of this name" );
    return -1;
  }

/*------------------------------------------------------------------------*\
    ... check if already in use ...
\*------------------------------------------------------------------------*/  
  for( c=connections; c; c=c->next ) {
    if( c->lcd==lcd ) {
      sd_error( SERDISP_ERUNTIME, "Display already in use" );
      return -1;
    }
  }

/*------------------------------------------------------------------------*\
    ... and attach it to current connection. That's all ...
\*------------------------------------------------------------------------*/
  con->lcd = lcd;
  return 0;
}


/* **  overruled code ** */

#if 0
/* original listener creation function by //MAF: */
/*=========================================================================*\
        Start up listener
\*=========================================================================*/
int make_listener( int port ) {
  int sock;
  int on = 1;
  struct sockaddr_in addr;

/*------------------------------------------------------------------------*\
    Get a socket ...
\*------------------------------------------------------------------------*/  
  sd_debug( 1, "Making listener on socket %d", port );
  sock = socket( AF_INET, SOCK_STREAM, 0);
  if( sock<0 ) {
    sd_srvmsg( LOG_ERR, "Error opening listener socket: %s", strerror(errno) );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Enable address reuse (helps on restarting crashed servers)
\*------------------------------------------------------------------------*/  
  if( setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on))<0 )
    sd_srvmsg( LOG_ERR, "setsockopt: %s", strerror(errno) );

/*------------------------------------------------------------------------*\
    Bind a port to this socket 
\*------------------------------------------------------------------------*/  
  memset( &addr, 0, sizeof(addr) );
  addr.sin_family = AF_INET;
  addr.sin_port   = htons( port );
  if( bind(sock,(struct sockaddr *)&addr,sizeof(addr)) ) {
    sd_srvmsg( LOG_ERR, "Could not bind to port %d: %s", port, strerror(errno) );
    return -1;
  }

/*------------------------------------------------------------------------*\
    And now listen ...
\*------------------------------------------------------------------------*/  
  if( listen(sock,16)<0 ) {
    sd_srvmsg( LOG_ERR, "listen: %s", strerror(errno) );
    return -1;
  }

/*------------------------------------------------------------------------*\
    That's it.
\*------------------------------------------------------------------------*/  
/*  fd_lst = sock; */
  return sock;
}
#endif
