/*
 *************************************************************************
 *
 * commands.h
 * command module for the serdisplib server
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

#ifndef __COMMANDS_H
#define __COMMANDS_H

/*=========================================================================*\
	Includes needed by definitions from this file
\*=========================================================================*/

#include "serdisplib/serdisp.h"
#include "connections.h"


/*========================================================================*\
   Prototypes
\*========================================================================*/
int cmd_execute( CONDSC *con );




#endif  /* __COMMANDS_H */


/*========================================================================*\
                                 END OF FILE
\*========================================================================*/

