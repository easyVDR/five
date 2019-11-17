/*
 *************************************************************************
 *
 * lcds.c
 * handles displays managed by serdisplib server
 * called by serdispd
 *
 *************************************************************************
 *
 * copyright (C) 2006       //MAF
 *
 * additional maintenance:
 * copyright (C) 2006-2008  wolfgang astleitner
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

#include <stdlib.h>
#include <string.h>

#include "lirc.h"


/*=========================================================================*\
	Private symbols
\*=========================================================================*/
static LCDDSC *lcds;  /* List of LCD displays */ 


/*=========================================================================*\
        Init displays
\*=========================================================================*/
LCDDSC *lcds_open( const char *lcdname, const char *lcddev, 
                 const char *lcddisp, const char *lcdopts ) {
  serdisp_CONN_t *sddev;
  serdisp_t      *dd;
  char           *name;
  LCDDSC         *lcd;
  
  char*          lcdname_stat;
  char*          lcddev_stat;
  char*          lcddisp_stat;
  char*          lcdopts_stat;

/*------------------------------------------------------------------------*\
    Name already used?
\*------------------------------------------------------------------------*/  
  if( lcd_getbyname(lcdname) ) {
    sd_srvmsg( LOG_ERR, "LCD name \"%s\" already used!\n", lcdname );
    return NULL;
  } 


 /* allocate memory for non-heapspace strings or else they will be overwritten later on */
 lcdname_stat = (char*)sdtools_malloc(strlen(lcdname) + 1);
 lcddev_stat  = (char*)sdtools_malloc(strlen(lcddev) + 1);
 lcddisp_stat = (char*)sdtools_malloc(strlen(lcddisp) + 1);
 lcdopts_stat = (char*)sdtools_malloc((lcdopts) ? (strlen(lcdopts) + 1) : 1);
 
 if (lcdname_stat && lcddev_stat && lcddisp_stat && lcdopts_stat) {
   sdtools_strncpy(lcdname_stat, lcdname, strlen(lcdname));
   sdtools_strncpy(lcddev_stat,  lcddev,  strlen(lcddev));
   sdtools_strncpy(lcddisp_stat, lcddisp, strlen(lcddisp));
   if (lcdopts)
     sdtools_strncpy(lcdopts_stat, lcdopts, strlen(lcdopts));
    else
     sdtools_strncpy(lcdopts_stat, "", 0);
 } else {
   return NULL;
 }

fprintf(stderr, "name: %s   dev: %s    disp: %s   opts: %s\n", lcdname_stat, lcddev_stat, lcddisp_stat, lcdopts_stat);
/*------------------------------------------------------------------------*\
    Create a serdisplib device
\*------------------------------------------------------------------------*/  
  sddev = SDCONN_open( lcddev_stat );
  if( !sddev ) {
    sd_srvmsg( LOG_ERR, "Unable to open %s, additional info: %s (check config file)\n", 
               lcddev_stat, sd_geterrormsg());
    return NULL;
  }
fprintf(stderr, "calling init\n");
  dd = serdisp_init( sddev, lcddisp_stat, lcdopts_stat ? lcdopts_stat : "" );
  if( !dd ) {
    sd_srvmsg( LOG_ERR, "Unknown display or unable to open %s, additional info: %s (check config file)\n",
               lcddisp_stat, sd_geterrormsg() );
    SDCONN_close( sddev );
    return NULL;
  }
  serdisp_clear( dd );

/*------------------------------------------------------------------------*\
    Create and prepend a new entry...
\*------------------------------------------------------------------------*/  
  lcd    = calloc( sizeof(LCDDSC), 1 );
  name   = strdup( lcdname );
  if( !lcd || !name ) {
    sd_srvmsg( LOG_ERR, "out of memory!" );
    return NULL;
  }
  lcd->next = lcds;
  lcds      = lcd;

/*------------------------------------------------------------------------*\
    ... and init it.
\*------------------------------------------------------------------------*/  
  lcd->name       = name;	
  lcd->dd         = dd;

/*------------------------------------------------------------------------*\
    That's it...
\*------------------------------------------------------------------------*/  
  return lcd;
}


/*=========================================================================*\
        Close one or all displays
\*=========================================================================*/
int lcds_close( LCDDSC *dellcd )
{
  LCDDSC         *lcd;
  LCDDSC         *prev = NULL;

/*------------------------------------------------------------------------*\
    Close all?
\*------------------------------------------------------------------------*/  
  if( !dellcd ) {
    while( lcds )
      lcds_close( lcds );
    return 0;
  }

/*------------------------------------------------------------------------*\
    Find entry in list and unlink it
\*------------------------------------------------------------------------*/  
  for( lcd=lcds; lcd&&lcd!=dellcd; prev=lcd,lcd=lcd->next );
  if( prev )
    prev->next = dellcd->next;
  else
    lcds = dellcd->next;

/*------------------------------------------------------------------------*\
    Close display and free buffer
\*------------------------------------------------------------------------*/  
  serdisp_quit( dellcd->dd );
  
  /* if-block added by wastl 2007-01-07 */
  /* if lirc available: close lirc-specific things */
  if (dellcd->lirc) {
    lirc_closeconnection( dellcd->lirc, NULL );
    lirc_stoplistener( dellcd->lirc );
  }
  free( dellcd );

/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return 0;
}


/*=========================================================================*\
        Get display descriptor by name
\*=========================================================================*/
LCDDSC *lcd_getbyname( const char *lcdname ) {
  LCDDSC         *lcd;

  for( lcd=lcds; lcd && strcmp(lcd->name,lcdname); lcd=lcd->next );
  return lcd;
}


/*=========================================================================*\
        Get first display descriptor of linked list
\*=========================================================================*/
LCDDSC *lcd_getfirst( void ) {
  return lcds;
}

/*=========================================================================*\
                                    END OF FILE
\*=========================================================================*/
