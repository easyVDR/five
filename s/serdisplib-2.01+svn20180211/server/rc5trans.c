/*
 *************************************************************************
 *
 * rc5trans.c
 * handles translation of RC5 codes into lirc commands
 * called by serdisplearn, serdispd
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "rc5trans.h"


/*========================================================================*\
    Look up an entry in the translation table.
\*========================================================================*/
char *rc5trans_get( const rc5trans_t *rc5tab, uint32_t rc5code ) {
  int i;

/*------------------------------------------------------------------------*\
    Loop over all entries until one matches or an emty one was seen...
\*------------------------------------------------------------------------*/  
  for( i=0; i<rc5tab->size; i++ ) {
    if( !rc5tab->table[i].rc5code )
      break;
    if( rc5tab->table[i].rc5code==rc5code )
      return rc5tab->table[i].command;
  }
 
/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/  
  return NULL;
}


/*========================================================================*\
    Add or change an entry in the translation table.
\*========================================================================*/
int rc5trans_set( rc5trans_t *rc5tab, uint32_t rc5code, const char *command ) {
  int   i;
  int   newsize;
  void *newtab;

/*------------------------------------------------------------------------*\
    Command
\*------------------------------------------------------------------------*/  
  if( strlen(command)>=sizeof(rc5tab->table->command) ) {
    errno = EINVAL;
    return -1;
  }

/*------------------------------------------------------------------------*\
    Loop over all entries until one matches or an emty one was seen...
\*------------------------------------------------------------------------*/  
  for( i=0; i<rc5tab->size; i++ ) {
    if( !rc5tab->table[i].rc5code || rc5tab->table[i].rc5code==rc5code ) {
      rc5tab->table[i].rc5code = rc5code;
      strcpy( rc5tab->table[i].command, command ); 
      return 0;
    }
  }

/*------------------------------------------------------------------------*\
    Table full -> Allocate new entries...
\*------------------------------------------------------------------------*/  
  newsize = rc5tab->size + RC5TRANS_TABINCR;
  newtab  = realloc(rc5tab->table, newsize*sizeof(*rc5tab->table) );
  if( !newtab )
    return -1;
  rc5tab->size  = newsize;
  rc5tab->table = newtab;
  rc5tab->table[i].rc5code = rc5code;
  strcpy( rc5tab->table[i].command, command ); 
  for( i++; i<newsize; i++ )
    rc5tab->table[i].rc5code = 0;

/*------------------------------------------------------------------------*\
    That's all...
\*------------------------------------------------------------------------*/  
  return 0;
}


/*========================================================================*\
    Read command entries from file
\*========================================================================*/
int rc5trans_read( rc5trans_t *rc5tab, const char *fname, int *linep ) {
  FILE  *fp;
  char   buffer[1024];
  char  *ptr;
  uint32_t  rc5code;
  if( linep )
    *linep = 0;

/*------------------------------------------------------------------------*\
    Try to open the file
\*------------------------------------------------------------------------*/  
  fp = fopen( fname, "r" );
  if( !fp )
    return -1;

/*------------------------------------------------------------------------*\
    Read line, ignore empty lines and comments.
\*------------------------------------------------------------------------*/  
  while( fgets(buffer,sizeof(buffer)-1,fp) ) {
    if( linep )
      (*linep)++;
    for( ptr=buffer; isspace(*ptr); ptr++ );	
    if( *ptr=='#' || *ptr==0 )
      continue;
    if( strchr(buffer,'\n') )
      *strchr(buffer,'\n') = 0;

/*------------------------------------------------------------------------*\
    Interpret and store translation entry...
\*------------------------------------------------------------------------*/
   rc5code = (uint32_t) strtol( ptr, &ptr, 0 );
   if( *ptr!=';' ) {
     errno = EINVAL;
     fclose( fp );
     return -1;
   }
   if( rc5trans_set(rc5tab,rc5code,ptr+1) ) {
     fclose( fp );
     return -1;
   }
  } /* end of: while(...) */
 
/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/
  fclose( fp );
  return 0;
}


/*========================================================================*\
    Count number of command entries
\*========================================================================*/
int rc5trans_count( const rc5trans_t *rc5tab ) {
  int i;
  int entries = 0;

/*------------------------------------------------------------------------*\
    Loop over all entries
\*------------------------------------------------------------------------*/  
  for( i=0; i<rc5tab->size; i++ ) {
    if( !rc5tab->table[i].rc5code )
      continue;
    entries++;
  }
 
/*------------------------------------------------------------------------*\
    That's all
\*------------------------------------------------------------------------*/
  return entries;
}

/* recode rc5 code according to philips definition */
uint32_t rc5trans_recode_rc5 (uint32_t code) {
  /*
   | Input
   | Byte 1                |                       | Byte 0                |                       |
   |  S  | ~C6 |  T  |  A4 |  A3 |  A2 |  A1 |  A0 |  X  |  X  |  C5 |  C4 |  C3 |  C2 |  C1 |  C0 |
   if ~C6 is low: command set is extended to > 63

   | Output
   | Byte 1                |                       | Byte 0                |                       |
   |  0  |  0  |  0  | ~C6 |  0  |  A4 |  A3 |  A2 |  A1 |  A0 |  C5 |  C4 |  C3 |  C2 |  C1 |  C0 |
  */
  uint32_t cmdcode =  code & 0x0000003F;  /* command code w/o ~C6 */
  uint32_t addrcode = code & 0x00005F00;  /* address code + ~C6; toggle bit and 'S'-bit set to 0 */

  return ((addrcode >> 2) | cmdcode) & 0x00001FFF;
}


/*=========================================================================*\
                                    END OF FILE
\*=========================================================================*/
