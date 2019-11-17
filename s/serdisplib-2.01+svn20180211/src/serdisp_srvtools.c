/*
 *************************************************************************
 *
 * serdisp_srvtools.c
 * tools for client server communication
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance and enhancements:
 * copyright (C) 2006-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *************************************************************************
 *
 * updates:
 * 2006-05-13 WA:  * included #include <netinet/in.h> to make freebsd/pcbsd happy
 *                 * replaced all malloc() through sdtools_malloc() to avoid
 *                    rpl_malloc() hack be applied by bsd-based systems
 *                 * included serdisp_tools.h  to be able to use sdtools_malloc()
 * 2006-06-16 MAF: * can deal now with unaligned buffer payloads by introducing
 *                   btohl() and htobl()
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
#include <syslog.h>
#include <fcntl.h>

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_srvtools.h"

#include "serdisplib/serdisp_fctptr.h"

/*=========================================================================*\
	Global symbols
\*=========================================================================*/


/*=========================================================================*\
	Private symbols
\*=========================================================================*/

/*------------------------------------------------------------------------*\
    Protocol header
\*------------------------------------------------------------------------*/  
#define HEADER_SIZE (3*sizeof(uint32_t))
static void conv_mhdr2b( char *buffer, size_t msglen, mbuf_cmd cmd );
static int conv_b2mhdr( char *buffer, size_t *msglen, mbuf_cmd *cmd );

/*------------------------------------------------------------------------*\
    Protocol payload
\*------------------------------------------------------------------------*/  
static size_t   get_bufsize( SERDISP_SRVARG *args, int *isvar );
static void     dump_buffer( void *buffer, size_t size );
static uint32_t btohl( void* netlongptr );
static void     htobl( void* netlongptr, uint32_t hostlong );

/*========================================================================*\
    Connect to a server
\*========================================================================*/
SERDISP_SRVCON *serdisp_srv_connect( const char *hostname, int port ) {
  SERDISP_SRVCON    *con;
  struct hostent    *hostptr;
  struct sockaddr_in srvaddr;
  int                fd;

/*-------------------------------------------------------------------------*\
	Parse device string
\*-------------------------------------------------------------------------*/
  port = port ? port : SD_SRV_DEFPORT;
 
/*-------------------------------------------------------------------------*\
	Get server address
\*-------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------*\
	Create socket and connect to server
\*-------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------*\
	Set up connection decriptor
\*-------------------------------------------------------------------------*/
  con = calloc( sizeof(SERDISP_SRVCON), 1 );
  if( !con ) {
    sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for connection", __func__);
    return NULL;
  }
  con->fd = fd;
  con->timestamp = time(NULL);

/*-------------------------------------------------------------------------*\
	That's it.
\*-------------------------------------------------------------------------*/
  return con;
}


/*========================================================================*\
    Close a connection
\*========================================================================*/
int serdisp_srv_close( SERDISP_SRVCON *con ) {
  int retval;

/*-------------------------------------------------------------------------*\
	Close socket ...
\*-------------------------------------------------------------------------*/
  retval = close( con->fd );

/*-------------------------------------------------------------------------*\
	... and free decriptor.
\*-------------------------------------------------------------------------*/
  free( con );

/*-------------------------------------------------------------------------*\
	That's it.
\*-------------------------------------------------------------------------*/
  return retval;
}


/*========================================================================*\
    Write from a buffer to a socket (just like send())
    return value is -1 on error or else the number of bytes read
    a buffer is completely submited if its status contains MBUF_COMPLETE
    note: if called with a already complete buffer, an error will be 
          returned and errno is set to EEXIST.
    note: broken connections will result in an ECONNRESET error
\*========================================================================*/
int serdisp_srv_dowritebuf( SERDISP_SRVMBUFFER *buf, int fd )
{
  int      missing;
  int      bytes = -1;
  int      totalbytes = 0;

/*------------------------------------------------------------------------*\
    If buffer is virgin: buffer payload and set it up for the header.
\*------------------------------------------------------------------------*/  
  if( !buf->status || buf->status==MBUF_LOADED ) {
    void *swap   = buf->buffer_; 
    buf->buffer_ = buf->buffer;
    buf->msglen_ = buf->msglen;
    buf->buffer  = swap;
    if( !buf->buffer )
      buf->buffer = sdtools_malloc( HEADER_SIZE );
    if( !buf->buffer ) {
      sd_error( SERDISP_EMALLOC, "%s(): out of memory!", __func__ );
      return -1;
    }
    conv_mhdr2b( buf->buffer, buf->msglen, buf->cmd );
    buf->msglen = HEADER_SIZE;
    buf->index  = 0;
    buf->status = MBUF_HEADER;
  }

/*------------------------------------------------------------------------*\
    Loop over header and payload as long as all bytes were sent
\*------------------------------------------------------------------------*/  
  do {  /* while( bytes==missing && buf->status!=MBUF_COMPLETE ) */

/*------------------------------------------------------------------------*\
    Try to write out missing data
\*------------------------------------------------------------------------*/  
    missing = buf->msglen - buf->index;
    if( !missing )
      errno = EEXIST;
    else
      bytes = fp_send( fd, buf->buffer+buf->index, missing, 0 );
    sd_debug( 2, "%s(): wrote to handle %d: %d missing, %d sent", __func__, fd, missing, bytes );
    if( bytes<0 && errno==EAGAIN )
      return 0;
    if( bytes<0 ) {
      sd_error( SERDISP_ERUNTIME, "%s(): could not write to connection: %s", __func__, strerror(errno) );
      return -1;
    }
    if( sd_getdebuglevel() ) 
      dump_buffer(buf->buffer+buf->index,bytes); 
    buf->index    += bytes;
    buf->bytecntr += bytes;
    totalbytes    += bytes;
    if( bytes )
      buf->timestamp = time(NULL);

/*------------------------------------------------------------------------*\
    Header is complete: setup for payload
\*------------------------------------------------------------------------*/
    if( buf->status==MBUF_HEADER && buf->index==buf->msglen ) {
      void *swap   = buf->buffer_; 
      buf->buffer_ = buf->buffer;
      buf->buffer  = swap;
      buf->msglen  = buf->msglen_;
      buf->status  = MBUF_PAYLOAD;
      buf->index   = 0;
      sd_debug( 2, "%s(): header sent complete for handle %d: cmd %d, length %d", __func__,
                fd, buf->cmd, (int)buf->msglen );
    }

/*------------------------------------------------------------------------*\
    Payload is complete:
\*------------------------------------------------------------------------*/
    if( buf->status==MBUF_PAYLOAD && buf->index==buf->msglen ) {
      buf->status = MBUF_COMPLETE;
      buf->msgcntr++;
      sd_debug( 2, "%s(): payload sent complete for handle %d: cmd %d, length %d", __func__,
              fd, buf->cmd, (int)buf->msglen );
    }
  } while( bytes==missing && buf->status!=MBUF_COMPLETE );

/*------------------------------------------------------------------------*\
    That's all: return bytes written.
\*------------------------------------------------------------------------*/  
  return totalbytes;
}


/*========================================================================*\
    Read from a socket into a buffer
    return value is -1 on error or else the number of bytes read
    a buffer is complete if its status contains MBUF_COMPLETE
    note: if called with a already complete buffer, an error will be 
          returned and errno is set to EEXIST.
    note: if a bad magic number is read in message header (protocol out 
          of sync) an EBADMSG will be issued.
    note: broken connections will - unlike read/recv which would return 0 - 
          result in an ECONNRESET error
    note: if a operation would block a non-blocking connection, no error
          but a byte-count of zero is returned.
\*========================================================================*/
int serdisp_srv_doreadbuf( SERDISP_SRVMBUFFER *buf, int fd )
{
  int      missing;
  int      bytes = -1;
  int      totalbytes = 0;

/*------------------------------------------------------------------------*\
    If buffer is virgin, we set it up for a header.
\*------------------------------------------------------------------------*/  
  if( !buf->status ) {
    buf->msglen = HEADER_SIZE;
    buf->index  = 0;
    buf->status = MBUF_HEADER;
  }

/*------------------------------------------------------------------------*\
    Loop over header and payload as long as all bytes were sent
\*------------------------------------------------------------------------*/  
  do {  /* while( bytes==missing && buf->status!=MBUF_COMPLETE ) */

/*------------------------------------------------------------------------*\
    (Re)allocate buffer for payload.
\*------------------------------------------------------------------------*/  
  if( buf->buffer && buf->msglen>buf->bufsize ) {
    free( buf->buffer );
    buf->buffer = NULL;
  }
  if( !buf->buffer ) {
    buf->bufsize = MAX(buf->msglen, SERDISP_SRV_MINBUFLEN);
    buf->buffer = sdtools_malloc( buf->bufsize );
    if( !buf->buffer ) {
      sd_error( SERDISP_EMALLOC, "%s(): out of memory!", __func__ );
      return -1;
    }
  }

/*------------------------------------------------------------------------*\
    Try to read missing data
\*------------------------------------------------------------------------*/  
    missing = buf->msglen - buf->index;
    if( !missing )
      errno = EEXIST;
    else
      bytes = fp_recv( fd, buf->buffer+buf->index, missing, 0 );
    sd_debug( 2, "%s(): read from handle %d: %d missing, %d read", __func__, fd, missing, bytes );
    if( !bytes ) {
      errno = ECONNRESET;
      return -1;
    }
    if( bytes<0 && errno==EAGAIN )
      return 0;
    if( bytes<0 ) {
      sd_error( SERDISP_ERUNTIME, "%s(): could not read from connection: %s", __func__, strerror(errno) );
      return -1;
    }
    if( sd_getdebuglevel() )
      dump_buffer(buf->buffer+buf->index,bytes); 
    buf->index    += bytes;
    buf->bytecntr += bytes;
    totalbytes    += bytes;
    buf->timestamp = time(NULL);

/*------------------------------------------------------------------------*\
    Header is complete: setup for payload
\*------------------------------------------------------------------------*/
    if( buf->status==MBUF_HEADER && buf->index==buf->msglen ) {
      buf->status = MBUF_PAYLOAD;
      if( conv_b2mhdr(buf->buffer,&buf->msglen,&buf->cmd) ) {
        sd_error( SERDISP_ERUNTIME,  "%s(): invalid message header", __func__ );
        errno = EBADMSG;
        return -1;
      } 
      buf->index = 0;
      sd_debug( 2, "%s(): header read complete for handle %d: cmd %d, length %d", __func__,
                fd, buf->cmd, (int)buf->msglen );
    }

/*------------------------------------------------------------------------*\
    Payload is complete:
\*------------------------------------------------------------------------*/
    if( buf->status==MBUF_PAYLOAD && buf->index==buf->msglen ) {
      buf->status = MBUF_COMPLETE;
      buf->msgcntr++;
      sd_debug( 2, "%s(): payload read complete for handle %d: cmd %d, length %d", __func__,
                fd, buf->cmd, (int)buf->msglen );
    }
  } while( bytes==missing && buf->status!=MBUF_COMPLETE );

/*------------------------------------------------------------------------*\
    That's all: return total bytes read
\*------------------------------------------------------------------------*/  
  return totalbytes;
}


/*========================================================================*\
    Send a command to a server and wait for return values.
\*========================================================================*/
int serdisp_srv_sendcmd( SERDISP_SRVCON *cd, mbuf_cmd cmd, 
                         SERDISP_SRVARG *args, SERDISP_SRVARG *retvals )
{

/*------------------------------------------------------------------------*\
    Reset buffer states...
\*------------------------------------------------------------------------*/  
  sd_debug( 2, "%s(): sending command %d", __func__, cmd );
  cd->writebuf.status = MBUF_VIRGIN;
  cd->readbuf.status  = MBUF_VIRGIN;

/*------------------------------------------------------------------------*\
    Only supported in blocking mode!
\*------------------------------------------------------------------------*/  
  if( serdisp_srv_setblocking(cd,1) )
    return -1;

/*------------------------------------------------------------------------*\
    Fill buffer for arguments
\*------------------------------------------------------------------------*/  
  if( serdisp_srv_fillbuf(&cd->writebuf,args) ) 
    return -1;
  cd->writebuf.cmd = cmd;

/*------------------------------------------------------------------------*\
    Try to send the buffer, we might block!
\*------------------------------------------------------------------------*/  
  do {
    if( serdisp_srv_dowritebuf(&cd->writebuf,cd->fd)<0 ) 
      return -1;
  } while( cd->writebuf.status!=MBUF_COMPLETE );

/*------------------------------------------------------------------------*\
    Try to read the answer
\*------------------------------------------------------------------------*/
  do {
    if( serdisp_srv_doreadbuf(&cd->readbuf,cd->fd)<0 ) 
      return -1; 
  } while( cd->readbuf.status!=MBUF_COMPLETE );

/*------------------------------------------------------------------------*\
    Return command should be equal to sent one ...
\*------------------------------------------------------------------------*/  
  if( cd->readbuf.cmd != cd->writebuf.cmd ) {
    sd_error( SERDISP_ERUNTIME, 
              "%s(): returned command %d unequals sent command %d - check server and client for same version",
              __func__, cd->readbuf.cmd, cd->writebuf.cmd );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Read return values
\*------------------------------------------------------------------------*/  
  if( serdisp_srv_unpackbuf(&cd->readbuf,retvals) ) 
    return -1;

/*------------------------------------------------------------------------*\
    That's all.
\*------------------------------------------------------------------------*/  
  return cd->readbuf.msglen;
}



/*========================================================================*\
    Pack data in a buffer
\*========================================================================*/
int serdisp_srv_fillbuf( SERDISP_SRVMBUFFER *buf, SERDISP_SRVARG *args ) {
  SERDISP_SRVARG *arg;
  size_t          bufsize;
  void           *ptr;

  /* buffer needs to be virgin */
  sd_debug( 2, "%s(%p,%p): starting...", __func__, buf, args );
  if( buf->status!=MBUF_VIRGIN ) {
    sd_error( SERDISP_ERUNTIME,  "%s(): tried to fill non-virgin buffer!", __func__ );
    return -1;
  }

  /* calculate buffer size. */
  buf->msglen = 0;
  bufsize = get_bufsize( args, NULL );
  /*if( bufsize<0 )
    return -1;*/
  if( !bufsize )
    return 0;

  /* (re)allocate buffer for payload. */
  if( buf->buffer && bufsize>buf->bufsize ) {
    free( buf->buffer );
    buf->buffer = NULL;
  }
  if( !buf->buffer ) {
    buf->bufsize = MAX(bufsize,SERDISP_SRV_MINBUFLEN);
    buf->buffer = sdtools_malloc( buf->bufsize );
    if( !buf->buffer ) {
      sd_error( SERDISP_EMALLOC, "%s(): out of memory!", __func__ );
      return -1;
    }
  }

  /* transcript data to buffer */
  for( ptr=buf->buffer,arg=args; arg->type!=MBUF_ARGEND; arg++ ) {
    switch( arg->type ) {
      case MBUF_ARGBYTE:
        htobl( ptr, *(byte*)arg->buffer ); 
        ptr += sizeof(uint32_t);
        sd_debug( 2, "%s(): encoded ARGBYTE=%d", __func__, *(byte*) arg->buffer );
        break; 
      case MBUF_ARGINT:
        htobl( ptr, *(int*)arg->buffer ); 
        ptr += sizeof(uint32_t);
        sd_debug( 2, "%s(): encoded ARGINT=%d", __func__, *(int*) arg->buffer );
        break; 
      case MBUF_ARGLONG:
        htobl( ptr, *(long*)arg->buffer ); 
        ptr += sizeof(uint32_t);
        sd_debug( 2, "%s(): encoded ARGLONG=%ld", __func__, *(long*) arg->buffer );
        break; 
      case MBUF_ARGSTR:
      case MBUF_ARGBYTES:
        htobl( ptr, arg->size );
        ptr += sizeof(uint32_t);
        if (arg->size) {
          memcpy( ptr, arg->buffer, arg->size );
          ptr += arg->size; 
        }
        if( arg->type==MBUF_ARGSTR ) {
          sd_debug( 2, "%s(): encoded ARGSTR=\"%s\"", __func__, (char*) arg->buffer );  
        } else {
          sd_debug( 2, "%s(): encoded ARGBYTES: %d elements", __func__, (int)arg->size );
        }        break;
      case MBUF_ARGINTS:
      case MBUF_ARGLONGS:
        { /* note: ints and longs are transferred equally using 4 bytes (uint32_t) */
          int idx;
          htobl( ptr, arg->size ); 
          ptr += sizeof(uint32_t);
          for (idx = 0; idx < arg->size; idx++) {
            htobl( ptr, ((uint32_t*)arg->buffer)[idx] );
            ptr += sizeof(uint32_t);
          }
          sd_debug( 2, "%s(): encoded %s: %d elements", __func__,
                       ((arg->type == MBUF_ARGINTS) ? "ARGINTS" : "ARGLONGS"), (int)arg->size );
        }
        break;
      default:
        break;
    }
  }
  buf->msglen = (char*)ptr - (char*)buf->buffer;

  return 0;
}


/*========================================================================*\
    Unpack data from a buffer
\*========================================================================*/
int serdisp_srv_unpackbuf( SERDISP_SRVMBUFFER *buf, SERDISP_SRVARG *args ) {
  SERDISP_SRVARG *arg;
  size_t          bufsize;
  size_t          argsize;
  int             isvar;
  void           *ptr;

  /* buffer needs to be complete */
  if( buf->status!=MBUF_COMPLETE  ) {
    sd_error( SERDISP_ERUNTIME, "%s(): tried to interpret non-complete buffer!", __func__ );
    return -1;
  }

  /* calculate expected buffer size. */
  bufsize = get_bufsize( args, &isvar );
  sd_debug( 2, "%s(%p,%p): estimated bufsize: %s=%ld", __func__,
            buf, args, isvar?">":"", (long)bufsize );
  /*if( bufsize<0 )
    return -1;*/
  if( !bufsize )
    return 0;

  /* (re)allocate buffer for payload. */
  if( bufsize>buf->index || (!isvar && bufsize!=buf->index) ) {
    sd_error( SERDISP_ERUNTIME, 
              "%s(): argument size does not fit buffer size", __func__ );
    return -1;
  }

  /* transcript buffer to data */
  for( bufsize=0,ptr=buf->buffer,arg=args; arg->type!=MBUF_ARGEND; arg++ ) {
    switch( arg->type ) {
      case MBUF_ARGBYTE:
        *(byte*) arg->buffer = btohl( ptr ); 
        ptr     += sizeof(uint32_t);
        bufsize += sizeof(uint32_t);
        sd_debug( 2, "%s(): decoded ARGBYTE=%d", __func__, *(byte*) arg->buffer );
        break; 
      case MBUF_ARGINT:
        *(int*) arg->buffer = btohl( ptr ); 
        ptr     += sizeof(uint32_t);
        bufsize += sizeof(uint32_t);
        sd_debug( 2, "%s(): decoded ARGINT=%d", __func__, *(int*) arg->buffer );  
        break; 
      case MBUF_ARGLONG:
        *(long*) arg->buffer = btohl( ptr ); 
        ptr     += sizeof(uint32_t);
        bufsize += sizeof(uint32_t); 
        sd_debug( 2, "%s(): decoded ARGLONG=%ld", __func__, *(long*) arg->buffer );  
        break; 
      case MBUF_ARGSTR:
      case MBUF_ARGBYTES:
        argsize  = btohl( ptr ); 
        ptr     += sizeof(uint32_t);
        bufsize += argsize+sizeof(uint32_t);
        if( argsize>arg->size ) {
          sd_error( SERDISP_ERUNTIME, 
                    "%s(): string buffer length %d too small for message length %d", __func__,
                    (int)arg->size, (int)argsize );
          return -1;
        }

        if( bufsize>buf->msglen ) {
          sd_error( SERDISP_ERUNTIME, 
                    "%s(): read past end of buffer: argsize=%d, bufsize=%d, msglen=%d ", __func__,
                    (int)argsize, (int)bufsize, (int)buf->msglen );
          return -1;
        }
        arg->size = argsize;
        if (arg->size) {
          memcpy( arg->buffer, ptr, arg->size ); 
          ptr += arg->size; 
          if( arg->type==MBUF_ARGSTR && arg->size!=strlen(arg->buffer)+1 ) {
            sd_error( SERDISP_ERUNTIME, 
                      "%s(): string length %d does not match message length %d", __func__,
                      (int)strlen(arg->buffer)+1, (int)arg->size );
            return -1;
          }
        }
        if( arg->type==MBUF_ARGSTR ) {
          sd_debug( 2, "%s(): decoded ARGSTR=\"%s\"", __func__, (char*) arg->buffer );
        } else {
          sd_debug( 2, "%s(): decoded ARGBYTES: %d elements", __func__, (int)arg->size );
        }
        break;
      case MBUF_ARGINTS:
      case MBUF_ARGLONGS: {
          int idx;
          argsize  = btohl( ptr ); 
          ptr     += sizeof(uint32_t);
          bufsize += argsize+sizeof(uint32_t);
          if( argsize > arg->size * ((arg->type == MBUF_ARGINTS)?sizeof(int):sizeof(long)) ) {
            if (arg->type == MBUF_ARGINTS) {
              sd_error( SERDISP_ERUNTIME, 
                        "%s(): buffer length %d*%d=%d too small for message length %d", __func__,
                        (int)arg->size, (int)sizeof(int), ((int)arg->size * (int)sizeof(int)), (int)argsize );
            } else {
              sd_error( SERDISP_ERUNTIME, 
                        "%s(): buffer length %d*%d=%d too small for message length %d", __func__,
                        (int)arg->size, (int)sizeof(long), ((int)arg->size * (int)sizeof(long)), (int)argsize );
            }
            return -1;
          }

          if( bufsize>buf->msglen ) {
            sd_error( SERDISP_ERUNTIME, 
                      "%s(): read past end of buffer: argsize=%d, bufsize=%d, msglen=%d ", __func__,
                      (int)argsize, (int)bufsize, (int)buf->msglen );
            return -1;
          }

          for (idx = 0; idx < arg->size; idx++) {
            if (arg->type == MBUF_ARGINTS)
              ((int*) arg->buffer)[idx] = (int)btohl( ptr ); 
            else
              ((long*) arg->buffer)[idx] = btohl( ptr ); 
            ptr += sizeof(uint32_t);
          }

          sd_debug( 2, "%s(): decoded %s: %d elements", __func__,
                       ((arg->type == MBUF_ARGINTS) ? "ARGINTS" : "ARGLONGS"), (int)arg->size );
        }
        break;
      default:
        break; 
    }
  }

  return 0;
}


/*========================================================================*\
    Set a connection into blocking mode
\*========================================================================*/
int serdisp_srv_setblocking( SERDISP_SRVCON *cd, int value )
{
  int flags;

/*------------------------------------------------------------------------*\
    Get current settings
\*------------------------------------------------------------------------*/  
  flags = fcntl( cd->fd, F_GETFL );
  if( flags==-1 ) {
    sd_error( SERDISP_ERUNTIME, "Could not fcntl socket %d: %s", 
              cd->fd, strerror(errno) );
    return -1;
  }
  
/*------------------------------------------------------------------------*\
    Mask NONblocking flag out or in.
\*------------------------------------------------------------------------*/  
  if( value )
    flags &= ~O_NONBLOCK;
  else
    flags |= O_NONBLOCK;

/*------------------------------------------------------------------------*\
    Get current settings
\*------------------------------------------------------------------------*/  
  flags = fcntl( cd->fd, F_GETFL );
  if( fcntl(cd->fd,F_SETFL,flags) ) {
    sd_error( SERDISP_ERUNTIME, "Could not fcntl socket %d: %s", 
              cd->fd, strerror(errno) );
    return -1;
  }

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Get buffer size needed for an argument list
        If isvar is NULL, strings are expected to be initialised and byte
           array desriptors should carry a valid size field, for all other 
           types, the size field will be set.
        If isvar is not null, the referenced integer will be set, if a
           string or byte array is seen.
        returns -1 on invalid argument types
\*=========================================================================*/
static size_t get_bufsize( SERDISP_SRVARG *arg, int *isvar ) {
  size_t bufsize = 0;
  int    i = 0;

  if( isvar ) *isvar = 0;

  /* loop over all arguments */
  while( arg && arg->type!=MBUF_ARGEND ) { 
    switch( arg->type ) {
      case MBUF_ARGBYTE:
      case MBUF_ARGINT:
      case MBUF_ARGLONG:
        bufsize += sizeof(uint32_t);
        if( !isvar )
          arg->size = sizeof(uint32_t); 
        break;
      case MBUF_ARGSTR:
        if( !isvar )
          arg->size = strlen(arg->buffer)+1;
        else 
          bufsize++;                       /* count terminating \0 anyway */
        /* No break ! */
      case MBUF_ARGBYTES: 
        if( !isvar )
          bufsize += arg->size; 
        else
          *isvar = 1;
        bufsize += sizeof(uint32_t);               /* length header field */
        break; 
      case MBUF_ARGINTS: 
      case MBUF_ARGLONGS: 
        if( !isvar )
          bufsize += arg->size * sizeof(long);
        else
          *isvar = 1;
        bufsize += sizeof(uint32_t);               /* length header field */
        break; 
      default:
        sd_error( SERDISP_ERUNTIME,
                  "%s(): target argument #%d has bad type %d", __func__,
                  i, arg->type );
        return -1;
    }
    arg++;
  }

  return bufsize;
}


/*=========================================================================*\
        Convert message header data from buffer
\*=========================================================================*/
static int conv_b2mhdr( char* buffer, size_t *msglen, mbuf_cmd *cmd )
{
  int magic;

/*------------------------------------------------------------------------*\
    Get and check magic number
\*------------------------------------------------------------------------*/
  magic  = fp_ntohl( *(uint32_t*)(buffer) ); 
  if( magic != SD_SRV_HDRMAGIC ) {
    sd_debug(2, "Magic expexted: %08x, got %08x", SD_SRV_HDRMAGIC, magic );
    return -1;
  }

/*------------------------------------------------------------------------*\
    Get message header length and command code
\*------------------------------------------------------------------------*/
  buffer += sizeof(uint32_t);
  *msglen = fp_ntohl( *(uint32_t*)(buffer) ); 
  buffer += sizeof(uint32_t);
  *cmd    = fp_ntohl( *(uint32_t*)(buffer) ); 

/*------------------------------------------------------------------------*\
    That's it...
\*------------------------------------------------------------------------*/
  return 0;
}


/*=========================================================================*\
        Convert message header data to buffer
\*=========================================================================*/
static void conv_mhdr2b( char* buffer, size_t msglen, mbuf_cmd cmd )
{
  *(uint32_t*)buffer = fp_htonl( SD_SRV_HDRMAGIC );
  buffer += sizeof(uint32_t);
  *(uint32_t*)buffer = fp_htonl( msglen ); 
  buffer += sizeof(uint32_t);
  *(uint32_t*)buffer = fp_htonl( cmd );
}


/*=========================================================================*\
        Get a long integer from payload buffer
        (A form of ntohl dealing with unaligned pointers)
\*=========================================================================*/
static uint32_t btohl( void* netlongptr )
{
  uint32_t aligned_value;

/*------------------------------------------------------------------------*\
    Copy possibly unaligned bytes to local variable ...
\*------------------------------------------------------------------------*/
  memcpy( &aligned_value, netlongptr, sizeof(aligned_value) );

/*------------------------------------------------------------------------*\
    ... and return the byteordered value.
\*------------------------------------------------------------------------*/
  return fp_ntohl( aligned_value ); 
}

/*=========================================================================*\
        Put a long integer to payload buffer
        (A form of htonl dealing with unaligned pointers)
\*=========================================================================*/
static void htobl( void* netlongptr, uint32_t hostlong )
{
  uint32_t netlong = fp_htonl( hostlong );
  memcpy( netlongptr, &netlong, sizeof(netlong) );
}


/*=========================================================================*\
        Dump buffer contents to output
\*=========================================================================*/
static void   dump_buffer( void *buffer, size_t size ) {
  size_t i;
  int    llen   = 8;
  char   hbuff[256], cbuff[256], *hptr=hbuff, *cptr=cbuff;
  sd_debug(2, "Dumping %d bytes starting at %p", (int)size, buffer );
  for( i=0; i<size; i++, buffer++ ) {
    if( !(i%llen) ) {
      if( i )
        sd_debug(2, "0x%04x - %-24s - %s ", (int)((i/llen-1)*llen), hbuff, cbuff );
      hptr = hbuff; cptr = cbuff;
      *hptr = *cptr = 0;
    }
    hptr += sprintf( hptr, "%02x ", *(unsigned char*)buffer );
    cptr += sprintf( cptr, "%c ", isprint(*(char*)buffer)?*(char*)buffer:'.' );
  }
  if( i )
    sd_debug(2, "0x%04x - %-24s - %s", (int)((i/llen)*llen), hbuff, cbuff );
}


/*=========================================================================*\
                                    END OF FILE
\*=========================================================================*/
