/*
 *************************************************************************
 *
 * lirc.c
 * handles lirc functions of the serdisplib server
 * called by serdispd (main module)
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance:
 * copyright (C) 2006-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
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
#include "lcds.h"
#include "lirc.h"

/*=========================================================================*\
	Global symbols
\*=========================================================================*/


/*=========================================================================*\
	Private symbols
\*=========================================================================*/
/*------------------------------------------------------------------------*\
    Some symbols
\*------------------------------------------------------------------------*/  
static int lirc_makelistener( lirctrans_t* lirc /*LCDDSC *lcd*/, const char *path, int port );
/*static int lirc_readtrfile( LCDDSC *lcd, const char *fname );*/


/*=========================================================================*\
        Start up lirc procedures for a display
\*=========================================================================*/
int lirc_initdisplay( LCDDSC *lcd, const char *conffile, const char *path, int port ) 
{
/* adapted code: wastl */
   byte gpid = SDGPI_search(lcd->dd, "IR");

/*------------------------------------------------------------------------*\
    Can display handle inputs?
\*------------------------------------------------------------------------*/  
/* orig code: maf ::  if( serdisp_recvctrl(lcd->dd,SERDISP_RECVTYPE)<0 ) */
/* adapted code: wastl */
  if( gpid == 0xFF )   /* could a GPI called 'IR' be found? */
    return -1;

/*------------------------------------------------------------------------*\
    Make up descriptor.
\*------------------------------------------------------------------------*/  
  if( lcd->lirc ) {
    sd_error( SERDISP_ERUNTIME, "lirc_initdisplay: display already initialised!" );
    return -1;
  }
  lcd->lirc = calloc( sizeof(lirctrans_t), 1 );
  if( !lcd->lirc ) {
    sd_error( SERDISP_ERUNTIME, "lirc_initdisplay: %s", strerror(errno) );
    return -1;
  }
  lcd->lirc->in_fd    = -1;
  lcd->lirc->un_fd    = -1;
  lcd->lirc->input_fd = -1;

  /* added 2008-02-23 wastl */
  lcd->lirc->rc5prev  =  0;
  lcd->lirc->rc5reps  =  0;


/*------------------------------------------------------------------------*\
    Read configuration file
\*------------------------------------------------------------------------*/  
  if( lirc_read_conffile(lcd->lirc,conffile) )
    return -1;

/*------------------------------------------------------------------------*\
    Make listeners
\*------------------------------------------------------------------------*/  
  if( lirc_makelistener(lcd->lirc,path,port) )
    return -1;

/*------------------------------------------------------------------------*\
    Start up input thread if not already running
\*------------------------------------------------------------------------*/  
/* orig code: maf ::  if( serdisp_recvctrl(lcd->dd,SERDISP_RECVSTART)<0 ) */
/* adapted code: wastl */
  if (SDGPI_enable(lcd->dd, gpid, 1) < 0)
    return -1;
  

/*------------------------------------------------------------------------*\
    That's it.
\*------------------------------------------------------------------------*/  
  return 0;
}




/*=========================================================================*\
        Start up listeners
\*=========================================================================*/
static int lirc_makelistener( lirctrans_t* lirc /*LCDDSC *lcd*/, const char *path, int port ) {
  int sock;

  if( path && strlen(path) > 0 ) {
    sock = create_devicelistener(path);

    if (sock >= 0) {
      lirc->un_fd   = sock;
      lirc->un_path = strdup( path );
    }
  }

  if( port > 0 ) {
    sock = create_portlistener(port);

    if (sock >= 0) {
      lirc->in_fd   = sock;
      lirc->in_port = port;
    }
  }
#if 0
/*------------------------------------------------------------------------*\
    Make up listener on named socket:
\*------------------------------------------------------------------------*/
/*  wastl: port -> path  2007-01-07 */
/*  if( port ) {   */
  if( path && strlen(path) > 0 ) {
    struct sockaddr_un addr;
    mode_t             oldmask;

    sd_debug( 1, "Making lirc listener for %s on socket '%s'", lcd->name, path );

    unlink( path );
    sock = socket( AF_UNIX, SOCK_STREAM, 0);
    if( sock<0 ) {
      sd_srvmsg( LOG_ERR, "Error opening lirc listener socket: %s", strerror(errno) );
      return -1;
    }

    if( strlen(path) >= sizeof(addr.sun_path) ) {
      sd_srvmsg( LOG_ERR, "Pathname for lirc socket too long (max. %d chars): %s", 
                 (int)sizeof(addr.sun_path), path );
      return -1;
    }
    memset( &addr, 0, sizeof(addr) );
    addr.sun_family = AF_UNIX;
    strcpy( addr.sun_path, path );

    oldmask = umask( 0 );
    if( bind(sock,(struct sockaddr *)&addr,sizeof(addr)) ) {
      sd_srvmsg( LOG_ERR, "Could not bind to socket %s: %s", path, strerror(errno) );
      umask( oldmask );
      return -1;
    }
    umask( oldmask );

    if( listen(sock,16)<0 ) {
      sd_srvmsg( LOG_ERR, "listen: %s", strerror(errno) );
      return -1;
    }
    lcd->lirc->un_fd   = sock;
    lcd->lirc->un_path = strdup( path );

/*------------------------------------------------------------------------*\
    Make up listener on IP port:
\*------------------------------------------------------------------------*/
  if( port>0 ) {   
    struct sockaddr_in addr;
    int                on = 1;

    sd_debug( 1, "Making lirc listener for %s on port '%d'", lcd->name, port );

    sock = socket( AF_INET, SOCK_STREAM, 0);
    if( sock<0 ) {
      sd_srvmsg( LOG_ERR, "Error opening lirc listener port: %s", strerror(errno) );
      return -1;
    }

    if( setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on))<0 )
      sd_srvmsg( LOG_ERR, "setsockopt: %s", strerror(errno) );

    memset( &addr, 0, sizeof(addr) );
    addr.sin_family = AF_INET;
    addr.sin_port   = htons( port );
    if( bind(sock,(struct sockaddr *)&addr,sizeof(addr)) ) {
      sd_srvmsg( LOG_ERR, "Could not bind to port %d: %s", port, strerror(errno) );
      return -1;
    }

    if( listen(sock,16)<0 ) {
      sd_srvmsg( LOG_ERR, "listen: %s", strerror(errno) );
      return -1;
    }

    lcd->lirc->in_fd   = sock;
    lcd->lirc->in_port = port;
  }
#endif

  return 0;
}


/*=========================================================================*\
        Stop listeners
\*=========================================================================*/
int lirc_stoplistener( lirctrans_t* lirc /* LCDDSC *lcd */ ) {

  /* adaptecd code by wastl 2007-01-07 */
/*  if (!lcd->lirc)
    return 0;
*/
  
  if( lirc->in_fd>=0 ) {
    close( lirc->in_fd );
    lirc->in_port = 0;
  }   

  if( lirc->un_fd>=0 ) {
    close( lirc->un_fd );
    Sfree( lirc->un_path );
  }   

  return 0;
}


/*=========================================================================*\
        Accept a new connection
\*=========================================================================*/
int lirc_acceptconnection( lirctrans_t* lirc /* LCDDSC *lcd */, int fd, const char* devname ) {
  struct sockaddr rmadr;
  socklen_t       rmadrl = sizeof(rmadr);
  char            rmname[1024];
  lirccon_t      *con;
  int             flags;

/*------------------------------------------------------------------------*\
    Accept connection
\*------------------------------------------------------------------------*/  
  fd = fp_accept( fd, &rmadr, &rmadrl );
  if( fd<0 ) {
    sd_srvmsg( LOG_ERR, "Error in accept: %s", strerror(errno) );
    return -1;
  }
  if( fd>=FD_SETSIZE ) {
    sd_srvmsg( LOG_ERR, "File descriptor %d to large (too many connections?)", fd );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Get identity of IP peers and check if accepted 
\*------------------------------------------------------------------------*/
  if( rmadr.sa_family==AF_INET ) {
    if( fp_getnameinfo(&rmadr,rmadrl,rmname,sizeof(rmname),NULL,0,0) ) {
      sd_srvmsg( LOG_ERR, "Could not get peer name: %s", strerror(errno) );
      return -1;
    }
    if (devname) {
      sd_srvmsg( LOG_NOTICE, "Lirc IP connection initiated from %s for device '%s'",
                 rmname, devname );
    } else {
      sd_srvmsg( LOG_NOTICE, "Lirc IP connection initiated from %s", rmname );
    }
    if( check_ipfilter((struct sockaddr_in*)&rmadr)!=CONALLOW ) {
      sd_srvmsg( LOG_NOTICE, "Rejected lirc connection from %s", rmname );
      close( fd );
      return 0;
    }
  } 
  else {
    if (devname) {
      sd_srvmsg( LOG_NOTICE, "Lirc connection on named socket for device '%s' accepted.", devname );
    } else {
      sd_srvmsg( LOG_NOTICE, "Lirc connection on named socket accepted." );
    }
  }


/*------------------------------------------------------------------------*\
    Create, fill and link new connection descriptor
\*------------------------------------------------------------------------*/
  con = calloc( 1, sizeof(lirccon_t) );
  if( !con ) {
    sd_srvmsg( LOG_ERR, "out of memory!" );
    return -1;
  }
  con->fd                = fd;
  con->name              = (rmadr.sa_family==AF_INET) ? strdup(rmname) : NULL;
  con->timestamp         = time(NULL);
  con->next              = lirc->connections;
  lirc->connections = con;

/*------------------------------------------------------------------------*\
    enable nonblocking ...
\*------------------------------------------------------------------------*/  
  flags = fcntl( fd, F_GETFL );
  fcntl( fd, F_SETFL, flags|O_NONBLOCK );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/
  if (devname) {
    sd_debug( 2, "New lirc connection for device '%s' from %s with handle %d", 
              devname, (rmadr.sa_family==AF_INET)?rmname:"named socket", fd );
  } else {
    sd_debug( 2, "New lirc connection from %s with handle %d", 
              (rmadr.sa_family==AF_INET)?rmname:"named socket", fd );
  }
  return 0;
}


/*=========================================================================*\
        Close one or all lirc connections of a display
\*=========================================================================*/
int lirc_closeconnection( lirctrans_t* lirc /* LCDDSC *lcd */, lirccon_t *delcon ) {
  lirccon_t *con;
  lirccon_t *prev = NULL;

  /* adaptecd code by wastl 2007-01-07 */
/*  if (!lcd->lirc)
    return 0;
*/
  
/*------------------------------------------------------------------------*\
    Close all?
\*------------------------------------------------------------------------*/  
  if( !delcon ) {
    while( lirc->connections )
      lirc_closeconnection( lirc, lirc->connections );
    return 0;
  }

/*------------------------------------------------------------------------*\
    Find entry in list and unlink it
\*------------------------------------------------------------------------*/  
  for( con=lirc->connections; con&&con!=delcon; prev=con,con=con->next );
  if( prev )
    prev->next = delcon->next;
  else
    lirc->connections = delcon->next;

/*------------------------------------------------------------------------*\
    Close socket and free buffers
\*------------------------------------------------------------------------*/  
  sd_debug( 1, "Closing lirc connection %d", delcon->fd );
  Sfree( delcon->name );
  close( delcon->fd );
  free( delcon );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Read and interpret a message from a client
\*=========================================================================*/
int lirc_readmsg( lirctrans_t* lirc /* LCDDSC *lcd */, lirccon_t *con ) {
  int bytes;
  char buffer[1024];

/*------------------------------------------------------------------------*\
    Read pending message...
\*------------------------------------------------------------------------*/  
  bytes = read( con->fd, &buffer, 1023 );
  if( bytes<0 ) {
    sd_srvmsg( LOG_ERR, "Error reading form lirc fd %d: %s", con->fd, strerror(errno) );
    return -1;
  }
  if( !bytes ) {
    if( lirc_closeconnection(lirc,con) )
      return -1;
    return 0;
  }
  buffer[bytes] = 0;
  sd_srvmsg( LOG_NOTICE, "Ignoring incomming message form lirc fd %d: %s", con->fd, buffer );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Handle input from a client
\*=========================================================================*/
int lirc_handleinput( LCDDSC *lcd ) {
/* orig code: maf
  int                 fd = serdisp_recvgetfd( lcd->dd );
  serdisp_gpievent_t  event;
*/
/* adapted code: wastl */
  int                 fd_in;

/* adapted code: wastl */
  byte gpid = SDGPI_search(lcd->dd, "IR");
  
  if (gpid == 0xFF || ((fd_in = SDGPI_getstreamfd(lcd->dd, gpid) ) == 0) ) {
    return -1;
  }
/* --- */
  return lirc_processinput( lcd->lirc, fd_in, lcd->name );
}


#if 0
/* orig code: maf ::  bytes = read( fd, &event, sizeof(event) ); */
  bytes = read( fd_in, &event, sizeof(event) );
  if( !bytes ) {
/* orig code: maf
    srvmsg( LOG_ERR, "Input fd %d not fed any longer.", fd );
    serdisp_recvctrl( lcd->dd, SERDISP_RECVSTOP );
*/
/* adapted code: wastl */
    srvmsg( LOG_ERR, "Input fd %d not fed any longer.", fd_in );
    /* serdisp_recvctrl( lcd->dd, SERDISP_RECVSTOP ); */  /* *** WA: need to fix stopping *** */
/* --- */
    return -1;
  }   
  if( bytes!=sizeof(event) ) {
    srvmsg( LOG_ERR, "Input fd %d returned short message %d<%d.\n", 
            fd_in, bytes, sizeof(event) );
/*            fd, bytes, sizeof(event) );*/
    return -1;
  }
#endif

int lirc_processinput( lirctrans_t* lirc, int fd_in, const char* devname ) {
  int                 bytes;
  SDGP_event_t        event;

  /* read raw data */
  bytes = read( fd_in, &event, sizeof(event) );

  if( bytes == -1 ) {
    if ( devname ) {  /* called from lcd->lirc context */
      fprintf( stderr, "error when reading event header on fd %d: %s\n", fd_in, strerror(errno) );
      return -1;
    } else { /* called from other context: erraneous read is no errno */
      return -2;
    }
  }
  if( !bytes ) {
    fprintf( stderr, "fd %d not fed any longer ...\n", fd_in );
    return -3;
  }
  if( bytes!=sizeof(event) ) {
    fprintf( stderr, "read from fd %d returned short message %d<%d.\n", fd_in, bytes, (int)sizeof(event) );
    return -1;
  }

  /* convert header from network byte order to network byte order */
  SDGPT_event_header_ntoh(&event);

  if (event.type == SDGPT_NORMRCSTREAM) {   /* normalised remote control stream */
    uint32_t            rcstream[4] = {0,0,0,0};
    uint32_t            rc5code;
    char                buffer[256];
    char*               lircstr;
    int                 retry;

    /* read payload (retry reading if the delay between sending of header and payload was too big) */
    retry = 0;
    do {
      bytes = read( fd_in, rcstream, sizeof(rcstream) );
      if (bytes == -1) usleep(100);
      retry++;
    } while (bytes == -1 && retry < 3);

    if( bytes == -1 ) {
      if ( devname ) {  /* called from lcd->lirc context */
        fprintf( stderr, "error when reading payload on fd %d: %s\n", fd_in, strerror(errno) );
        return -1;
      } else { /* called from other context: erraneous read is no errno */
        return -2;
      }
    }
    if( !bytes ) {
      fprintf( stderr, "fd %d not fed any longer...\n", fd_in );
      return -3;
    }
    if( bytes!=sizeof(rcstream) ) {
      fprintf( stderr, "read from fd %d returned short message %d<%d.\n", fd_in, bytes, (int)sizeof(event) );
      return -1;
    }

    /* convert payload from network byte order to host byte order */
    SDGPT_event_payload_ntoh(rcstream, sizeof(rcstream), (byte)sizeof(rcstream[0]));

    /* look up LIRC command ... */

    /* orig code: maf ::  rc5code = event.lvalue & ~0x00002000;      */
    rc5code = rc5trans_recode_rc5(rcstream[0]); /* & ~0x00002000 ;  */  /* ... ignore toggle bit */
    lircstr = rc5trans_get( &lirc->rc5tab, rc5code );
    if( !lircstr ) {
      if( rc5code!=lirc->rc5old ) {
        if (devname) {
          sd_srvmsg( LOG_INFO, "Device '%s': No command defined for RC5 code 0x%04x.",
                    devname, rc5code );
        } else {
          sd_srvmsg( LOG_INFO, "No command defined for RC5 code 0x%04x.", rc5code );
        }
      }
      lirc->rc5old = rc5code;
      return 0;
    }
    lirc->rc5old = rc5code;

    /* repeat counter - added 2008-02-23 wastl */
    if (rcstream[0] == lirc->rc5prev) {
      lirc->rc5reps++;
    } else {
      lirc->rc5reps = 0;
      lirc->rc5prev = rcstream[0];
    }

    /* handle data and send messages to clients */
    sprintf( buffer, "%08x%08x %02x %s %s\n",
            0, rc5code, 
            lirc->rc5reps, 
            lircstr, lirc->remname?lirc->remname:LIRC_DEFREMNAME );
    if( lirc_sendmsg(lirc,-1,buffer) )
      return -1;
  } else {
    fprintf(stderr, "\rUnknown event 0x%02x occured. ignoring ...\n", event.type);
    bytes = read(fd_in, NULL, 0);
    return -2;
  }

  return 0;
}

 
/*=========================================================================*\
        Send a message to all clients connected to a display
\*=========================================================================*/
int lirc_sendmsg( lirctrans_t* lirc /* LCDDSC *lcd */, int fd, const char *msg ) {
  lirccon_t *con, 
            *next;
  size_t   len = strlen( msg );
  int      bytes;

/*------------------------------------------------------------------------*\
    Loop over all connections, if fd is set filter only this one...
\*------------------------------------------------------------------------*/  
  for( con=lirc->connections; con; con=next ) {
    next = con->next;                    /* Could loose con in loop body! */
    if( fd!=-1 && fd!=con->fd )
      continue;

/*------------------------------------------------------------------------*\
    Try to send message, close on error (broken or full pipes)
\*------------------------------------------------------------------------*/
    bytes = write( con->fd, msg, len );
    if( bytes!=len ) {
      if( bytes<0 ) 
        sd_srvmsg( LOG_ERR, "Error writing to lirc fd %d: %s", fd, strerror(errno) );
      lirc_closeconnection( lirc, con );
    }
    else
      con->messages++;  
  }

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Read a file that contains normalised rc-codes
\*=========================================================================*/
int lirc_read_rcnfile( lirctrans_t* lirc, const char *fname ) {
  int             line = 0;

  if( rc5trans_read(&lirc->rc5tab,fname,&line) ) {
    sd_error( SERDISP_ERUNTIME, "File '%s' (line %d): %s\n", 
              fname+4, line, strerror(errno) );
    return -1;
  }
  sd_debug( 1, "Read %d entries for RC5/LIRC translation from %s", 
            rc5trans_count(&lirc->rc5tab), fname );
  return 0;
}

/*=========================================================================*\
        Read a lirc rules file
\*=========================================================================*/
int lirc_read_conffile( lirctrans_t* lirc /*LCDDSC *lcd*/, const char *fname ) {
  FILE           *fp;
  int             line = 0;
  char            buffer[2048];
  char           *ptr;
  char           *keyword;

/*------------------------------------------------------------------------*\
    Short form?
\*------------------------------------------------------------------------*/  
  if( !strncasecmp(fname,"RC5:",4) ) {
    if( rc5trans_read(&lirc->rc5tab,fname+4,&line) ) {
      sd_error( SERDISP_ERUNTIME, "File '%s' (line %d): %s\n", 
                fname+4, line, strerror(errno) );
      return -1;
    }
    sd_debug( 1, "Read %d entries for RC5/LIRC translation from %s", 
              rc5trans_count(&lirc->rc5tab), fname+4 );
    return 0;
  }

/*------------------------------------------------------------------------*\
    Try to open file
\*------------------------------------------------------------------------*/  
  fp = fopen( fname,"r" );
  if( !fp ) {
    sd_srvmsg( LOG_ERR, "Could not open %s: %s", fname, strerror(errno) );
    return 1;
  }

/*------------------------------------------------------------------------*\
    Read line, ignore empty lines and comments, get keyword and 
    dispatch into handlers
\*------------------------------------------------------------------------*/  
  while( fgets(buffer,sizeof(buffer)-1,fp) ) {
    line++;
    for( ptr=buffer; isspace(*ptr); ptr++ );	
    if( *ptr=='#' || *ptr==0 )
      continue;
    keyword  = strtok(ptr," \t\n");

/*------------------------------------------------------------------------*\
    Set lirc name of remote control
\*------------------------------------------------------------------------*/
    if( !strcasecmp("RemoteName",keyword) ) {
      char *remname  = strtok(NULL," \t\n");
      char *tail     = strtok(NULL," \t\n");
      if( !remname || !*remname || tail ) {
        sd_srvmsg( LOG_ERR, "%s line %d: sytax error (line ignored)!\n", 
                   fname, line );
        continue;
      }
      if( lirc->remname ) {
        sd_error( SERDISP_ERUNTIME, "%s line %d: Remote name already defined (%s)\n", 
                  fname, line, lirc->remname );
        fclose( fp );
        return -1;
      }
    }

/*------------------------------------------------------------------------*\
    Read translation entries
\*------------------------------------------------------------------------*/
    else if( !strcasecmp("RC5file",keyword) ) {
      char *fname     = strtok(NULL," \t\n");
      char *tail      = strtok(NULL," \t\n");
      if( !fname || tail ) {
        sd_srvmsg( LOG_ERR, "%s line %d: sytax error!\n", fname, line );
        fclose( fp );
        return -1;
      } 
      if( rc5trans_read(&lirc->rc5tab,fname,&line) ) {
        sd_error( SERDISP_ERUNTIME, "File '%s' (line %d): %s\n", 
                  fname+4, line, strerror(errno) );
        fclose( fp );
        return -1;
      }
      sd_debug( 1, "Read %d entries for RC5/LIRC translation from %s", 
                rc5trans_count(&lirc->rc5tab), fname );
      return 0;
    }

/*------------------------------------------------------------------------*\
    Unknown keyword
\*------------------------------------------------------------------------*/
    else {
      sd_srvmsg( LOG_ERR, "%s line %d: unknown keyword %s (line ignored)!\n", 
                 fname, line, keyword );
    }
  } /* endof:  while( fgets(buffer,sizeof(buffer)-1,fp) ) */

/*------------------------------------------------------------------------*\
    That's it...
\*------------------------------------------------------------------------*/
  fclose( fp );	  
  return 0;
}


/*=========================================================================*\
                                    END OF FILE
\*=========================================================================*/
