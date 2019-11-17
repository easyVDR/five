/*
 *************************************************************************
 *
 * serdispd.c
 * main module for the serdisplib server
 * called by operating system (daemon)
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
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_srvtools.h"
#include "args.h"
#include "conf.h"
#include "lcds.h"
#include "connections.h"

#define SERDISPD_VERSION      1.0


/* private symbols */
static volatile int stop_signal;
static void         sig_handler( int sig );


/* custom signal handler */
static void  sig_handler( int sig ) {
  /* what sort of signal is to be processed ? */
  switch( sig ) {
    case SIGINT:
    case SIGTERM:
      stop_signal = sig;
    break;
  }
}


/* main function */
int main( int argc, char *argv[] ) {
  int       help_flag    = 0;
  int       fg_flag      = 0;
  char*     cfg_fname    = "/etc/serdispd.cfg";
  char*     pid_fname    = "/var/run/serdispd.pid";
  char*     port_arg     = "15243";
  char*     verb_arg     = "0";
  int       verb_level   = 0;
  char*     vers_flag    = NULL;
  char*     eptr;
  int       port;
  int       cpid;
  int       fd;
  int       fd_lst;
  FILE*     fp;
  int       retval = 0;

  /* set up commandline switches */
  addarg( "--help",    "-h",  &help_flag,   NULL,       "Show this help message and quit" );
  addarg( "--config",  "-f",  &cfg_fname,   "filename", "Set name of configuration file" );
  addarg( "--port",    "-p",  &port_arg,    "port",     "Port for listener" );
  addarg( "--fg",      "-F",  &fg_flag,     NULL,       "Run in foreground (no detach)" );
  addarg( "--pfile",   "-pid",&pid_fname,   "filename", "Filename to store process ID" );
  addarg( "--verbose", "-v",  &verb_arg,    "level",    "Set verbosity level" );
  addarg( "--version", "-V",  &vers_flag,   NULL,       "Show program version" );

  /* parse the arguments */
  if( getargs(argc,argv) ) {
    usage( argv[0], 1 );
    return 1;
  }

  if( vers_flag ) {
    printf( "%s version %g protocol %d.%d  (using serdisplib version %d.%d)\n", 
      argv[0], SERDISPD_VERSION, SD_SRV_PROTOVERS_MAJOR, SD_SRV_PROTOVERS_MINOR,
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
    verb_level = (int) strtol( verb_arg, &eptr, 10 );
    while( isspace(*eptr) )
      eptr++;
    if( *eptr ) {
      fprintf( stderr, "Bad verbosity level: '%s'\n", verb_arg );
      usage( argv[0], 1 );
      return 1;
    }
  }
  port = (int) strtol( port_arg, &eptr, 10 );
  while( isspace(*eptr) )
    eptr++;
  if( *eptr || port<=0 ) {
    fprintf( stderr, "Bad port number: '%s'\n", port_arg );
    usage( argv[0], 1 );
    return 1;
  }

  /* init logging of serdisplib ... */
  sd_setdebuglevel( verb_level );
  sd_setlogmedium( fg_flag ? SD_LOG_STDERR : SD_LOG_SYSLOG );

  /* goto background */
  if( ! fg_flag ) {
    cpid = fork();
    if( cpid==-1 ) {
      perror( "Could not fork" );
      return -2;
    }
    if( cpid )   /* Parent process exits ... */
      return 0;
    if( setsid()==-1 ) {
      perror( "Could not create new session" );
      return -2;
    }
    fd = open( "/dev/null", O_RDWR, 0 );
    if( fd!=-1) {
      dup2(fd, STDIN_FILENO);
      dup2(fd, STDOUT_FILENO);
      dup2(fd, STDERR_FILENO);
    }
  } /* end of: if( ! fg_flag )*/

  /* setup PID file, ignore errors... */
  fp = fopen( pid_fname, "w" );
  if( fp ) {
    fprintf( fp, "%d\n", getpid() );
    fclose( fp );
  }

  /* read configuration and init display descriptors */
  if( readconfig(cfg_fname) )
    return -1;

  /* OK, from here on we catch some terminating signals and ignore others */
  signal( SIGINT,  sig_handler );
  signal( SIGTERM, sig_handler );
  signal( SIGPIPE, SIG_IGN );

  /* start up listener socket ... */
  /*  fd_lst = make_listener( port );*/
  fd_lst = create_portlistener( port );
  if( fd_lst<0 )
    return -1;

  /* main loop:  */
  while( !stop_signal ) {

   /* accept new connections and get pending client requests */
    retval = do_select( fd_lst, 1000 );
    if( retval )
      break;

    /* read client requests and handle them */
    retval = handle_requests( );
    if( retval )
      break;

  } /* end of: while( !stop_signal ) */

  /* stop signal received: stop program */
  if( stop_signal )
    sd_debug( 1, "Exiting due to signal %d ...", stop_signal );

  /* close listener and shutdown all connections */
  close( fd_lst );
  close_connection( NULL );

  /* cleanup all displays and PID file */
  lcds_close( NULL );
  unlink( pid_fname );

  return retval;
}
