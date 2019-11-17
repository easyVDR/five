/*
 *************************************************************************
 *
 * args.c
 * handles command line arguments
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
 * 2006-07-21 MAF: * separated from conf.[ch]
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
#include "args.h"


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
static ARGDSC *firstarg;



/*=========================================================================*\
  Add an argument
\*=========================================================================*/
int addarg( char *name, char *sname, void *buffer, char *valname, char *help )
{
  ARGDSC *newarg,
         *walk;

/*-------------------------------------------------------------------------*\
    OK, allocate a new argument descriptor and link it
\*-------------------------------------------------------------------------*/
  newarg = calloc( 1, sizeof(*newarg) );
  if( !firstarg )
    firstarg = newarg;
  else {
    for( walk=firstarg; walk->next; walk=walk->next );
    walk->next = newarg;
  }

/*-------------------------------------------------------------------------*\
    Store the values.
\*-------------------------------------------------------------------------*/
  newarg->name    = name;
  newarg->sname   = sname;
  newarg->buffer  = buffer;
  newarg->valname = valname;
  newarg->help    = help;
  newarg->defval  = valname?strdup(*(char**)buffer?*(char**)buffer:"<none>"):NULL;

/*-------------------------------------------------------------------------*\
    That's all ...
\*-------------------------------------------------------------------------*/
  return 0;
}


/*=========================================================================*\
  Give some hints about the usage
\*=========================================================================*/
void usage( char *prgname, int iserr )
{
  ARGDSC *arg;
  FILE* f = iserr ? stderr : stdout;

/*-------------------------------------------------------------------------*\
    Print header
\*-------------------------------------------------------------------------*/
  fprintf( f, "Usage  : %s [option [value]]\n", prgname ); 
  fprintf( f, "Options:\n" );

/*-------------------------------------------------------------------------*\
    Print each option
\*-------------------------------------------------------------------------*/
  for( arg=firstarg; arg; arg=arg->next ) {
    fprintf( f, " %-10s %-5s %-10s - %s", arg->name,
                                          arg->sname?arg->sname:"",
                                          arg->valname?arg->valname:"",
                                          arg->help );
    if( arg->defval )
      fprintf( f, " [default: \"%s\"]\n", arg->defval );
    else
      fprintf( f, "\n" );
  }

/*-------------------------------------------------------------------------*\
    That's all.
\*-------------------------------------------------------------------------*/
}


/*=========================================================================*\
  Parse the command line parameters
\*=========================================================================*/
int getargs( int argc, char *argv[] )
{
  int      arg;
  ARGDSC  *argptr = NULL;

/*-------------------------------------------------------------------------*\
  Loop over all arguments
\*-------------------------------------------------------------------------*/
  arg = 0;
  while( ++arg<argc ) { 

/*-------------------------------------------------------------------------*\
  Does this switch match ?
\*-------------------------------------------------------------------------*/
      for( argptr=firstarg; argptr; argptr=argptr->next )  {
        if( !strcmp(argv[arg],argptr->name) || 
            (argptr->sname && !strcmp(argv[arg],argptr->sname)) ) {

/*-------------------------------------------------------------------------*\
  Switch without value?
\*-------------------------------------------------------------------------*/
          if( !argptr->valname )
            (*(int*)argptr->buffer)++;

/*-------------------------------------------------------------------------*\
  Switch with value?
\*-------------------------------------------------------------------------*/
          else if( ++arg==argc )
            return -1;
          else
            *(char **)argptr->buffer = argv[arg];

/*-------------------------------------------------------------------------*\
  No need to test the rest.
\*-------------------------------------------------------------------------*/
          break;
        }
      } /* end of:  for( argptr=firstarg; argptr; argptr=argptr->next )  */

/*-------------------------------------------------------------------------*\
  Unknown option ?
\*-------------------------------------------------------------------------*/
      if( !argptr && *argv[arg] == '-' ) {
        fprintf( stderr, "%s: Unknown option '%s'!\n",
                 argv[0], argv[arg] );
        return 1;
      }

  } /* endof:  while( ++arg<argc ) */

/*-------------------------------------------------------------------------*\
        That's all.
\*-------------------------------------------------------------------------*/
  return 0;
}




/*=========================================================================*\
                                    END OF FILE
\*=========================================================================*/
