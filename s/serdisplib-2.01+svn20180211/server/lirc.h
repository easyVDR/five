/*
 *************************************************************************
 *
 * lirc.h
 * handles lirc functions of the serdisplib server
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

#ifndef __LIRC_H
#define __LIRC_H

/*=========================================================================*\
	Includes needed by definitions from this file
\*=========================================================================*/

#include <stdint.h>

#include "serdisplib/serdisp.h"
#include "rc5trans.h"
#include "lcds.h"

/*=========================================================================*\
	Defines
\*=========================================================================*/

#define LIRC_DEFREMNAME  "serdispd"


/*------------------------------------------------------------------------*\
   Descriptor of a client connection
\*------------------------------------------------------------------------*/
typedef struct _lirccon_t {
  struct _lirccon_t  *next;
  int                 fd;
  char               *name;
  time_t              timestamp;
  int                 messages;
} lirccon_t;

/*------------------------------------------------------------------------*\
   Descriptor of a translation ruleset
\*------------------------------------------------------------------------*/
typedef struct _lirctrans_t {
  int                 in_fd;
  int                 un_fd;
  int                 in_port;
  char               *un_path;
  lirccon_t          *connections;
  int                 input_fd;
  char               *remname;
  rc5trans_t          rc5tab;
  uint32_t            rc5old;
  uint32_t            rc5prev;  /* added 2008-02-23 wastl */
  unsigned char       rc5reps;  /* added 2008-02-23 wastl */
} lirctrans_t;



/*========================================================================*\
   Prototypes
\*========================================================================*/
int lirc_initdisplay      (LCDDSC *lcd, const char *trfile, const char *path, int port );
int lirc_handleinput      (LCDDSC *lcd );

int lirc_stoplistener     (lirctrans_t* lirc /*LCDDSC *lcd*/ );
int lirc_acceptconnection (lirctrans_t* lirc /*LCDDSC *lcd*/, int fd, const char* devname);
int lirc_readmsg          (lirctrans_t* lirc /*LCDDSC *lcd*/, lirccon_t *delcon );
int lirc_closeconnection  (lirctrans_t* lirc /*LCDDSC *lcd*/, lirccon_t *delcon );
int lirc_sendmsg          (lirctrans_t* lirc /*LCDDSC *lcd*/, int fd, const char *msg );

int lirc_processinput     (lirctrans_t* lirc, int fd_in, const char* devname );
int lirc_read_conffile    (lirctrans_t* lirc, const char *fname );
int lirc_read_rcnfile     (lirctrans_t* lirc, const char *fname );

#endif  /* __LIRC_H */


/*========================================================================*\
                                 END OF FILE
\*========================================================================*/

