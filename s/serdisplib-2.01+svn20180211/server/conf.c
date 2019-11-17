/*
 *************************************************************************
 *
 * conf.c
 * handles arguments and configuration for serdispd
 * called by serdispd (main module)
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance:
 * copyright (C) 2006-2008  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *************************************************************************
 *
 * updates:
 * 2006-07-05 MAF: * added LIRC command
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
#include <ctype.h>
#include <errno.h>
#include <syslog.h>

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_srvtools.h"
#include "conf.h"
#include "lcds.h"
#include "connections.h"
#include "lirc.h"


/*=========================================================================*\
	Global symbols
\*=========================================================================*/
/* none */

/*=========================================================================*\
	Private symbols
\*=========================================================================*/

/*------------------------------------------------------------------------*\
   Argument handling
\*------------------------------------------------------------------------*/
typedef struct _argdsc {
  struct _argdsc     *next;
  char               *name;
  char               *sname;
  void               *buffer;
  char               *valname;
  char               *help;
  char               *defval;
} ARGDSC;
/*static ARGDSC *firstarg;*/   /* WA: not used */



/*=========================================================================*\
        Read config file and init displays
\*=========================================================================*/
int readconfig( char *fname )
{
  FILE           *fp;
  int             line = 0;
  char            buffer[2048];
  char           *ptr;
  char           *keyword;

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
    Open (and init) a new display
\*------------------------------------------------------------------------*/
    if( !strcasecmp("Display",keyword) ) {
      char *lcdname  = strtok(NULL," \t\n");
      char *lcddev   = strtok(NULL," \t\n");
      char *lcddisp  = strtok(NULL," \t\n");
      char *lcdopts  = strtok(NULL," \t\n");
      if( !lcdname || !*lcdname || !lcddev || !*lcddev || !lcddisp || !*lcddisp ) {
        sd_srvmsg( LOG_ERR, "%s line %d: sytax error (line ignored)!\n", fname, line );
        continue;
      }
      if( !lcds_open(lcdname,lcddev,lcddisp,lcdopts) ) {
        sd_srvmsg( LOG_ERR, "Exiting on line %d in file %s\n", line, fname );
        fclose( fp );
        lcds_close( NULL );
        return -1;
      }
    }

/*------------------------------------------------------------------------*\
    Add an access controll entry
\*------------------------------------------------------------------------*/
    else if( !strcasecmp("allow",keyword) ) {
      if( add_ipfilter(1,strtok(NULL,"")) )
        return -1;
    }
    else if( !strcasecmp("deny",keyword) ) {
      if( add_ipfilter(-1,strtok(NULL,"")) )
        return -1;
    }

/*------------------------------------------------------------------------*\
    Open lirc socket(s) for displays with input capabilities
\*------------------------------------------------------------------------*/
    else if( !strcasecmp("Lirc",keyword) ) {
      LCDDSC *lcd;
      char *lcdname   = strtok(NULL," \t\n");
      char *trname    = strtok(NULL," \t\n");
      char *socket    = strtok(NULL," \t\n");      
      char *portstr   = strtok(NULL," \t\n");   
      int   port      = -1;
      if( socket==NULL )
        socket = "/dev/lircd";
      if( !lcdname || !trname ) {
        sd_srvmsg( LOG_ERR, "%s line %d: sytax error (line ignored)!\n", fname, line );
        continue;
      }
      lcd = lcd_getbyname( lcdname );
      if( !lcd ) {
        sd_srvmsg( LOG_ERR, "%s line %d: no such display %s!\n", fname, line, lcdname );
        fclose( fp );
        lcds_close( NULL );
        return -1;
      }
      if( portstr ) {
        char *eptr;
        port = strtol( portstr, &eptr, 10 );
        if( *eptr ) {
          sd_srvmsg( LOG_ERR, "%s line %d: bad port number %s (line ignored)!\n", 
                     fname, line, portstr );
          continue;
        }
      }
      if( lirc_initdisplay(lcd,trname,socket,port)<0 ) {
        sd_srvmsg( LOG_ERR, "Exiting on line %d in file %s: %s\n", line, fname, sd_geterrormsg() );
        fclose( fp );
        lcds_close( NULL );
        return -1;
      }
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
