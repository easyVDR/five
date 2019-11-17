/*
 *************************************************************************
 *
 * rc5trans.h
 * handles translation of RC5 codes into lirc commands
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

#ifndef __RC5TRANS_H
#define __RC5TRANS_H

#include <stdint.h>

/*=========================================================================*\
   Some definitions
\*=========================================================================*/

/*------------------------------------------------------------------------*\
   Descriptor of a LIRC translation table
\*------------------------------------------------------------------------*/
typedef struct {
  uint32_t  rc5code;
  char      command[32];
} rc5transentry_t;

typedef struct {
  int              size;
  rc5transentry_t *table;
} rc5trans_t;

#define RC5TRANS_TABINCR  10

/*========================================================================*\
   Prototypes
\*========================================================================*/
char *rc5trans_get( const rc5trans_t *rc5tab, uint32_t rc5code );
int   rc5trans_set( rc5trans_t *rc5tab, uint32_t rc5code, const char *command );
int   rc5trans_read( rc5trans_t *rc5tab, const char *fname, int *linep );
int   rc5trans_count( const rc5trans_t *rc5tab );
/* recode rc5 code according to philips definition */
uint32_t rc5trans_recode_rc5 (uint32_t code);

#endif  /* __RC5TRANS_H */


/*========================================================================*\
                                 END OF FILE
\*========================================================================*/

