/*
 *************************************************************************
 *
 * connections.h
 * handles connections of the serdisplib server
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

#ifndef __CONNECTIONS_H
#define __CONNECTIONS_H

/*=========================================================================*\
	Includes needed by definitions from this file
\*=========================================================================*/

#include <netinet/in.h>

#include "lcds.h"
#include "serdisplib/serdisp_srvtools.h"
#include "serdisplib/serdisp_messages.h"


/*------------------------------------------------------------------------*\
   Descriptor of a client connection
\*------------------------------------------------------------------------*/
typedef struct _condsc {
  struct _condsc     *next;
  SERDISP_SRVCON      con;
  LCDDSC             *lcd;
} CONDSC;


/*------------------------------------------------------------------------*\
   Definitions for IP filter
\*------------------------------------------------------------------------*/
#define CONALLOW 1
#define CONDENY  -1


/*========================================================================*\
   Prototypes
\*========================================================================*/

int create_portlistener   (int port);
int create_devicelistener (const char* path);
/*int make_listener( int port );*/
int do_select( int fd_lst, int ms );
int accept_connection( int fd_lst );
int close_connection( CONDSC *delcon );
int handle_requests( void );
int attach_display( CONDSC *con, const char *lcdname );
int add_ipfilter( int mode, char *name );
int check_ipfilter( struct sockaddr_in *addr_in );


#endif  /* __CONNECTIONS_H */


/*========================================================================*\
                                 END OF FILE
\*========================================================================*/

