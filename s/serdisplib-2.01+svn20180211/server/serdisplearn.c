/*
 *************************************************************************
 *
 * serdisplearn.c
 * tool to generate LIRC translations
 * called by operating system
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance:
 * copyright (C) 2006-2017  wolfgang astleitner
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
/*#include <sys/uio.h>*/
#include <sys/select.h>

#if defined(__linux__)
#include <getopt.h>
#endif

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_gpevents.h"

#include <sys/time.h>

#define SERDISPLEARN_VERSION  0.99

#include "args.h"
#include "rc5trans.h"

rc5trans_t rc5tab;
void  rc5trans_list( const rc5trans_t *rc5tab );
int   rc5trans_write( const rc5trans_t *rc5tab, const char *fname );


/*========================================================================*\
    Main function
\*========================================================================*/
int main( int argc, char *argv[] )
{
  int                 help_flag    = 0;
  int                 vers_flag    = 0;
  char               *fname        = NULL;
  char               *dname        = NULL;
  char               *port         = NULL;
  char               *options      = "";
  char               *verb_arg     = NULL;
  int                 verblevel    = 0;
  char               *eptr;
  int                 retval;
  int                 retcode      = 0;
  serdisp_CONN_t     *sdcd;
  serdisp_t          *dd = 0;
/* orig code: maf
  serdisp_gpievent_t  event;
  serdisp_gpievent_t  oldevent;
*/  
/* adapted code: wastl */
  SDGP_event_t        event;
  uint32_t            rcstream[] = {0, 0, 0, 0};
  uint32_t            oldrcstream[]= {0, 0, 0, 0};
  byte                gpid = 0xFF;
/* --- */
  uint32_t            rc5code;
  fd_set              fd_read;
  int                 line;
  int                 fd;
  int                 prompt;
  char                c;
  char               *lircstr;
  char                buffer[128];

/*-------------------------------------------------------------------------*\
	Set up commandline switches
\*-------------------------------------------------------------------------*/
  addarg( "--help",    "-h",  &help_flag,   NULL,       "Show this help message and quit" );
  addarg( "--file",    "-f",  &fname,       "filename", "Name of file containing rc-codes" );
  addarg( "--display", "-n",  &dname,       "name",     "Display name" );
  addarg( "--port",    "-p",  &port,        "dev|port", "Output device or port" );
  addarg( "--option",  "-o",  &options,     "options",  "Option string" );
  addarg( "--verbose", "-v",  &verb_arg,    "level",    "Set verbosity level" );
  addarg( "--version", "-V",  &vers_flag,   NULL,       "Show program version" );
  
/*-------------------------------------------------------------------------*\
	Parse the arguments
\*-------------------------------------------------------------------------*/
  if( getargs(argc,argv) ) {
    usage( argv[0], 1 );
    return 1;
  }
  if( vers_flag ) {
    printf( "%s version %g  (using serdisplib version %d.%d)\n", 
      argv[0], SERDISPLEARN_VERSION, 
      SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
      SERDISP_VERSION_GET_MINOR(serdisp_getversioncode()) );
    printf( "(c) 2006 by //MAF (add. enhancements / add-ons: (c) 2008-2010 by Wolfgang Astleitner)\n\n" );
    return 0;
  }
  if( help_flag ) {
    usage( argv[0], 0 );
    return 0;
  }
  if( verb_arg ) {
    verblevel = (int) strtol( verb_arg, &eptr, 10 );
    while( isspace(*eptr) )
      eptr++;
    if( *eptr ) {
      fprintf( stderr, "Bad verbosity level: '%s'\n", verb_arg );
      usage( argv[0], 1 );
      return 1;
    }
  }
  if( !dname || !port ) {
    fprintf( stderr, "Please supply display name and device or port!\n" );
    usage( argv[0], 1 );
    return 1;
  }

/*------------------------------------------------------------------------*\
    Init logging of serdisplib ...
\*------------------------------------------------------------------------*/  
  sd_setdebuglevel( verblevel );
  sd_setlogmedium( SD_LOG_STDERR );

/*------------------------------------------------------------------------*\
    Try to read existing translation file
\*------------------------------------------------------------------------*/
  if( fname ) {
    if( rc5trans_read(&rc5tab,fname,&line) ) {
      if( errno!=ENOENT ) {
        fprintf( stderr, "File '%s' line %d: %s\n", fname, line, strerror(errno) );
        return -1;
      }
      printf( "File '%s' will be created on termination...\n", fname );
    } 
    else
      printf( "Found %d entries in file '%s'\n", rc5trans_count(&rc5tab), fname );
  }

/*------------------------------------------------------------------------*\
    Open and init display
\*------------------------------------------------------------------------*/
  sdcd = SDCONN_open( port );
  if( !sdcd ) {  
    fprintf( stderr, "Error: Unable to open %s, additional info: %s\n", 
             port, sd_geterrormsg());
    return -1;
  }
  dd = serdisp_init( sdcd, dname, options );
  if( !dd ) {          
    fprintf( stderr, "Error: Unknown display or unable to open %s, additional info: %s\n", 
             dname, sd_geterrormsg());
    SDCONN_close( sdcd );
    return -1;
  }
  serdisp_clear(dd);

/*------------------------------------------------------------------------*\
    Init input handler
\*------------------------------------------------------------------------*/


/* orig code: maf
  if( !dd->fp_recvctrl ) {
*/
/* adapted code: wastl */
  gpid = SDGPI_search(dd, "IR");
  if (gpid == 0xFF) {
/* --- */  
    fprintf( stderr, "Input not supported by this display!\n" );
    serdisp_close( dd );
    SDCONN_close( sdcd );
    return -1;
  }

/* orig code: maf
  if( serdisp_recvctrl(dd,SERDISP_RECVSTART)<0 ) {
*/
/* adapted code: wastl */
  if (!SDGPI_isenabled(dd, gpid)) {
    if (SDGPI_enable(dd, gpid, 1) < 0) {
/* --- */  
      fprintf( stderr, "Could not start input handler: %s\n", sd_geterrormsg() );
      serdisp_close( dd );
      SDCONN_close( sdcd );
      return -1;
    }
  }
/* orig code: maf
  fd = serdisp_recvgetfd( dd );
*/
/* adapted code: wastl */
  fd = SDGPI_getstreamfd(dd, gpid);
  if (!fd) {
    fprintf( stderr, "unable to get file descriptor for reading pipe\n");
    serdisp_close( dd );
    SDCONN_close( sdcd );
    return -1;
  }
/* --- */
  if( verblevel>0 )
    printf( "Type vector for display is 0x%04x\n\n", 
/* orig code: maf
            serdisp_recvctrl(dd,SERDISP_RECVTYPE) );
*/
/* adapted code: wastl */
            (int)(SDGPI_gettype(dd, gpid)) );
/* --- */          
/*------------------------------------------------------------------------*\
    Main loop
\*------------------------------------------------------------------------*/
  memset( oldrcstream, 0, sizeof(oldrcstream) );
  prompt = 1;
  for(;;) {
    FD_ZERO( &fd_read );
    FD_SET( fd, &fd_read );
    FD_SET( 0,  &fd_read );

/*------------------------------------------------------------------------*\
    Wait for an input event
\*------------------------------------------------------------------------*/
    if( prompt )
      printf( "\nHit a key on remote, <L+Enter> to list entries or <Ctrl-D> to quit...\n" );
    if( select(fd+1,&fd_read,NULL,NULL,NULL)<0 ) {
      fprintf( stderr, "Select failed: %s\n", strerror(errno) );
      retcode = -1;
      break;
    }
    prompt = 0;

/*------------------------------------------------------------------------*\
    Keybord input via stdin?
\*------------------------------------------------------------------------*/
    if( FD_ISSET(0,&fd_read) ) {
      if( !read(0,&c,1) ) {
        printf( "Break! Shutting down...\n" );
        break;
      }
      if( tolower(c)=='l' )
        rc5trans_list(&rc5tab);
      if( c=='\n' )
        prompt = 1;
      continue;
    }

/*------------------------------------------------------------------------*\
    Retry if no IR-event is pending ...    
\*------------------------------------------------------------------------*/
    if( !FD_ISSET(fd,&fd_read) )
      continue;

/*------------------------------------------------------------------------*\
    Read in IR event   
\*------------------------------------------------------------------------*/
    retval = read( fd, &event, sizeof(event) );
    if( retval == -1 ) {
      fprintf( stderr, "error when reading fd %d: %s\n", fd, strerror(errno) );
      break;
    }
    if( !retval ) {
      fprintf( stderr, "fd %d not fed any longer...\n", fd );
      retcode = -1;
      break;
    }
    if( retval!=sizeof(event) ) {
      fprintf( stderr, "Read returned short message %d<%d.\n", retval, (int)sizeof(event) );
      retcode = -1;
      break;
    }

    /* convert header from network byte order to network byte order */
    SDGPT_event_header_ntoh(&event);

    if (event.type == SDGPT_NORMRCSTREAM) {   /* normalised remote control stream */
      retval = read( fd, rcstream, sizeof(rcstream) );
      if( retval == -1 ) {
        fprintf( stderr, "error when reading fd %d: %s\n", fd, strerror(errno) );
        break;
      }
      if( !retval ) {
        fprintf( stderr, "fd %d not fed any longer...\n", fd );
        retcode = -1;
        break;
      }
      if( retval!=sizeof(rcstream) ) {
        fprintf( stderr, "Read returned short message %d<%d.\n", retval, (int)sizeof(event) );
        retcode = -1;
        break;
      }

      /* convert payload from network byte order to host byte order */
      SDGPT_event_payload_ntoh(rcstream, sizeof(rcstream), (byte)sizeof(rcstream[0]));

      if( verblevel>1 )
        printf( "%d.%06d -> got event type %d, values %08x %08x %08x %08x\n", 
                (int)event.timestamp.tv_sec, (int)event.timestamp.tv_usec, event.type, 
                rcstream[0], rcstream[1], rcstream[2], rcstream[3]);

      /* ignore if same key is still pressed (toggle bit is not masked in event) */
      if( rcstream[0] == oldrcstream[0] )
        continue;
      prompt = 1;
      memcpy( oldrcstream, rcstream, sizeof(oldrcstream) );
      rc5code = rc5trans_recode_rc5(rcstream[0]); /* & ~0x00002000;  */      /* ... ignore toggle bit */
      printf( "Received RC5 code %04x, ", rc5code );

      /* if code is already assigned ask for what to do... */
      lircstr = rc5trans_get( &rc5tab, rc5code );
      if( lircstr ) {
        printf( "found translation entry: %s\n", lircstr );
        do {
          printf( "Change [N|y|q]? " );
          fflush( stdout );
          if( scanf("%c%*[^\n]",&c)==EOF )
            c = 'q';
          else
            getchar();
          c = tolower( c );
        } while( !strchr("ynq",c) );
        if( c=='q' ) {
          printf( "Break! Shutting down...\n" );
          break;
        }
        if( c=='n' )
          continue;
      }

      /* read and store command string */
      printf( " please enter command string: " );
      fflush( stdout );
      if( !fgets(buffer,sizeof(buffer)-1,stdin) )
        break;
      if( strchr(buffer,'\n') )
        *strchr(buffer,'\n') = 0;
      if( !rc5trans_set(&rc5tab,rc5code,buffer) )
        printf( "Command '%s' stored for RC5 code %08x.\n", buffer, rc5code );
      else
        fprintf( stderr, "Error storing command '%s': %s\n", buffer, strerror(errno) );
    } else {
      fprintf(stderr, "\rUnknown event 0x%02x occured. ignoring ...\n", event.type);
      read(fd, NULL, 0);
    }
  } /* end of: for(;;) */

/*------------------------------------------------------------------------*\
    Store translation entries
\*------------------------------------------------------------------------*/  
  if( !rc5trans_count(&rc5tab) )
    printf( "Ignoring empty command table (no output generated)...\n" );
  else if( fname ) {
    if( rc5trans_write(&rc5tab,fname) ) {
      printf( "Tranlation table follows:\n" );
      rc5trans_list(&rc5tab);
      retcode = -1;
    }
  } else {
    printf( "Tranlation table follows (use -f to store in file):\n" );
    rc5trans_list(&rc5tab);
  }

/*------------------------------------------------------------------------*\
    Shut down display
\*------------------------------------------------------------------------*/  
/* orig code: maf
  if( serdisp_recvctrl(dd,SERDISP_RECVSTOP)<0 )
*/
/* adapted code: wastl */
  if ( SDGPI_enable(dd, gpid, 0) < 0 )
/* --- */  
    fprintf( stderr, "Could not stop input handler: %s\n", sd_geterrormsg() );
  serdisp_close( dd );
  SDCONN_close( sdcd );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return retcode;
}


/*========================================================================*\
    List command entries
\*========================================================================*/
void rc5trans_list( const rc5trans_t *rc5tab ) {
  int i;
  int entries = 0;

/*------------------------------------------------------------------------*\
    Loop over all entries
\*------------------------------------------------------------------------*/  
  for( i=0; i<rc5tab->size; i++ ) {
    if( !rc5tab->table[i].rc5code )
      continue;
    printf( "%04x %s\n", rc5tab->table[i].rc5code, rc5tab->table[i].command );
    entries++;
  }
  printf( "%d commands found\n", entries );
 
/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/
}


/*========================================================================*\
    Write command entries to file
\*========================================================================*/
int rc5trans_write( const rc5trans_t *rc5tab, const char *fname ) {
  FILE  *fp;
  int    i;
  char   entries = 0;

/*------------------------------------------------------------------------*\
    Try to open the file
\*------------------------------------------------------------------------*/  
  fp = fopen( fname, "w" );
  if( !fp ) {
    fprintf( stderr, "Could not open '%s' for writing: %s\n", 
             fname, strerror(errno) );
    return -1;
  } 

/*------------------------------------------------------------------------*\
    Loop over all entries
\*------------------------------------------------------------------------*/  
  for( i=0; i<rc5tab->size; i++ ) {
    if( !rc5tab->table[i].rc5code )
      continue;
    fprintf( fp, "0x%04x;%s\n", 
             rc5tab->table[i].rc5code, rc5tab->table[i].command );
    entries++;
  }
 
/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/
  if( fclose(fp) ) {
    fprintf( stderr, "Could not close '%s': %s\n", fname, strerror(errno) );
    return -1;
  }

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/
  printf( "Wrote %d entries to file '%s'\n", entries, fname );
  return 0;
}


/*=========================================================================*\
                                    END OF FILE
\*=========================================================================*/
