/*
 *************************************************************************
 *
 * lcds.h
 * handles displays managed by serdisplib server
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

#ifndef __LCDS_H
#define __LCDS_H

/*=========================================================================*\
	Includes needed by definitions from this file
\*=========================================================================*/

#include "serdisplib/serdisp.h"


/*------------------------------------------------------------------------*\
   Descriptor of a LCD display
\*------------------------------------------------------------------------*/
typedef struct _lcddsc_t {
  struct _lcddsc_t     *next;
  char                 *name;
  serdisp_t            *dd;
  struct _lirctrans_t  *lirc;  
} LCDDSC;


/*========================================================================*\
   Prototypes
\*========================================================================*/
LCDDSC *lcds_open( const char *lcdname, const char *lcddev, 
                 const char *lcddisp, const char *lcdopts );
int     lcds_close( LCDDSC *dellcd );
LCDDSC *lcd_getbyname( const char *lcdname );
LCDDSC *lcd_getfirst( void );

#endif  /* __LCDS_H */


/*========================================================================*\
                                 END OF FILE
\*========================================================================*/

