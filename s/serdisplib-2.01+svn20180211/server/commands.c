/*
 *************************************************************************
 *
 * commands.c
 * command module for the serdisplib server
 * called by serdispd (communication part)
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
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_srvtools.h"
#include "serdisplib/serdisp_gpevents.h"

#include "connections.h"
#include "commands.h"


/* private symbols */
static int fillret( SERDISP_SRVCON *cd, mbuf_cmd cmd, SERDISP_SRVARG *retvals );

static int cmd_protovers    ( CONDSC *con );
static int cmd_sdlibvers    ( CONDSC *con );
static int cmd_attach       ( CONDSC *con );
static int cmd_ddinfo       ( CONDSC *con );
static int cmd_ddgettables  ( CONDSC *con );
static int cmd_ddinit       ( CONDSC *con );
static int cmd_ddclose      ( CONDSC *con );
static int cmd_ddsetoption  ( CONDSC *con );
static int cmd_ddgetoption  ( CONDSC *con );
static int cmd_ddupdate     ( CONDSC *con );
static int cmd_ddsetsdpixel ( CONDSC *con );
static int cmd_ddgetsdpixel ( CONDSC *con );

static int cmd_optionamount ( CONDSC *con );
static int cmd_optiondesc   ( CONDSC *con );

static int cmd_gpioamount   ( CONDSC *con );
static int cmd_gpodesc      ( CONDSC *con );
static int cmd_gpidesc      ( CONDSC *con );

static int cmd_ddputbuffs   ( CONDSC *con );
static int cmd_ddgetbuffs   ( CONDSC *con );


/*=========================================================================*\
        Execute remote command

  Parameters      : a connectioon with complete read buffer

  Returncode      : non-zero on error
\*=========================================================================*/
int cmd_execute( CONDSC *con ) {
  int retval   = 0;
  *sd_errormsg = 0;               /* reset error string to save bandwidth */

  /* command handler */
  switch( con->con.readbuf.cmd ) {
    case MBUF_CMD_PROTOVERS:    retval = cmd_protovers(con);      break;
    case MBUF_CMD_SDLIBVERS:    retval = cmd_sdlibvers(con);      break;
    case MBUF_CMD_ATTACH:       retval = cmd_attach(con);         break;
    case MBUF_CMD_DDINFO:       retval = cmd_ddinfo(con);         break;
    case MBUF_CMD_DDGETTABLES:  retval = cmd_ddgettables(con);    break;
    case MBUF_CMD_DDINIT:       retval = cmd_ddinit(con);         break;
    case MBUF_CMD_DDCLOSE:      retval = cmd_ddclose(con);        break;
    case MBUF_CMD_DDUPDATE:     retval = cmd_ddupdate(con);       break;
    case MBUF_CMD_DDGETOPTION:  retval = cmd_ddgetoption(con);    break;
    case MBUF_CMD_DDSETOPTION:  retval = cmd_ddsetoption(con);    break;
    case MBUF_CMD_DDSETSDPIXEL: retval = cmd_ddsetsdpixel(con);   break;
    case MBUF_CMD_DDGETSDPIXEL: retval = cmd_ddgetsdpixel(con);   break;

    case MBUF_CMD_OPTIONAMOUNT: retval = cmd_optionamount(con);   break;
    case MBUF_CMD_OPTIONDESC:   retval = cmd_optiondesc(con);     break;

    case MBUF_CMD_GPIOAMOUNT:   retval = cmd_gpioamount(con);     break;

    case MBUF_CMD_GPODESC:      retval = cmd_gpodesc(con);        break;
    case MBUF_CMD_GPIDESC:      retval = cmd_gpidesc(con);        break;

    case MBUF_CMD_DDPUTBUFFS:   retval = cmd_ddputbuffs(con);     break;
    case MBUF_CMD_DDGETBUFFS:   retval = cmd_ddgetbuffs(con);     break;

    default:
      sd_srvmsg( 0, "Received unknown command %d on handle %d", 
                 con->con.readbuf.cmd, con->con.fd );
      if( fillret(&con->con,MBUF_CMD_UNKNOWN,NULL) ) {
        retval = -1;
      }
  }
  return retval;
}


/* *** commands executed by cmd_execute *** */


/* *********************************
   int cmd_protovers (con)
   *********************************
   receives client protocol version and sends server protocol version
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_protovers ( CONDSC *con ) {
  int  client_protovers_major;
  int  client_protovers_minor;
  int  server_protovers_major;
  int  server_protovers_minor;

  SERDISP_SRVARG args[] = { { MBUF_ARGINT, &client_protovers_major, 0 },
                            { MBUF_ARGINT, &client_protovers_minor, 0 },
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &server_protovers_major, 0 },
                            { MBUF_ARGINT, &server_protovers_minor, 0 },
                            { MBUF_ARGEND, NULL, 0} };

  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  sd_srvmsg(1, "Client protocol version info: %d.%d", client_protovers_major, client_protovers_minor);

  server_protovers_major = SD_SRV_PROTOVERS_MAJOR;
  server_protovers_minor = SD_SRV_PROTOVERS_MINOR;

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_sdlibvers (con)
   *********************************
   receives client serdisplib version and sends server serdisplib version
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_sdlibvers ( CONDSC *con ) {
  int  client_sdlibvers_major;
  int  client_sdlibvers_minor;
  int  client_sdlibvers_subminor;
  int  server_sdlibvers_major;
  int  server_sdlibvers_minor;
  int  server_sdlibvers_subminor;
  char* server_sdlibvers_extra = SERDISP_VERSION_EXTRA;

  SERDISP_SRVARG args[] = { { MBUF_ARGINT, &client_sdlibvers_major, 0 },
                            { MBUF_ARGINT, &client_sdlibvers_minor, 0 },
                            { MBUF_ARGINT, &client_sdlibvers_subminor, 0 },
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &server_sdlibvers_major, 0},
                            { MBUF_ARGINT, &server_sdlibvers_minor, 0 },
                            { MBUF_ARGINT, &server_sdlibvers_subminor, 0 },
                            { MBUF_ARGEND, NULL, 0} };

  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  sd_srvmsg( 0, "Client serdisplib version: serdisplib %d.%d.%d",
                 client_sdlibvers_major, client_sdlibvers_minor, client_sdlibvers_subminor
  );

  server_sdlibvers_major = SERDISP_VERSION_MAJOR;
  server_sdlibvers_minor = SERDISP_VERSION_MINOR;
  server_sdlibvers_subminor = atoi(server_sdlibvers_extra);  /* in theory _EXTRA can be a string */

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_optionamount (con)
   *********************************
   sends amount of options which can be read and/or modified by the client
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_optionamount ( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  int                amountoptions;

  SERDISP_SRVARG rets[] = { { MBUF_ARGINT,  &amountoptions, 0},
                            { MBUF_ARGEND,  NULL, 0} };

  amountoptions = 0;
  while (amountoptions < dd->amountoptions && (! (dd->options[amountoptions].flag & SD_OPTIONFLAG_LOC)) )
    amountoptions++;

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_optiondesc (con)
   *********************************
   sends full option description to option index opt_idx
   opt_idx is the index in the driver specific option array
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_optiondesc ( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  int opt_idx;
  serdisp_options_t optiondesc;

  SERDISP_SRVARG args[] = { { MBUF_ARGINT, &opt_idx, 0},
                            { MBUF_ARGEND, NULL, 0} };
  SERDISP_SRVARG rets[] = { { MBUF_ARGSTR, NULL, 0},
                            { MBUF_ARGSTR, NULL, 0},
                            { MBUF_ARGLONG, NULL, 0},
                            { MBUF_ARGLONG, NULL, 0},
                            { MBUF_ARGLONG, NULL, 0},
                            { MBUF_ARGBYTE, NULL, 0},
                            { MBUF_ARGSTR, NULL, 0},
                            { MBUF_ARGEND, NULL, 0} };

  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  if (opt_idx < 0 || opt_idx >= dd->amountoptions)
    return -1;

  if (!serdisp_getoptiondescription(dd, dd->options[opt_idx].name, &optiondesc)) {
    /* options like contrast or backlight may fail: return dummy */
    rets[0].buffer = dd->options[opt_idx].name;       rets[0].size = strlen(dd->options[opt_idx].name)+1;
    rets[1].buffer = dd->options[opt_idx].aliasnames; rets[1].size = strlen(dd->options[opt_idx].aliasnames)+1;
    rets[2].buffer = &(dd->options[opt_idx].minval);
    rets[3].buffer = &(dd->options[opt_idx].maxval);
    rets[4].buffer = &(dd->options[opt_idx].modulo);
    rets[5].buffer = &(dd->options[opt_idx].flag);
    rets[6].buffer = dd->options[opt_idx].defines;    rets[6].size = strlen(dd->options[opt_idx].defines)+1;
  } else {
    rets[0].buffer = optiondesc.name;       rets[0].size = strlen(optiondesc.name)+1;
    rets[1].buffer = optiondesc.aliasnames; rets[1].size = strlen(optiondesc.aliasnames)+1;
    rets[2].buffer = &(optiondesc.minval);
    rets[3].buffer = &(optiondesc.maxval);
    rets[4].buffer = &(optiondesc.modulo);
    rets[5].buffer = &(optiondesc.flag);
    rets[6].buffer = optiondesc.defines;    rets[6].size = strlen(optiondesc.defines)+1;
  }

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_gpioamount (con)
   *********************************
   sends amount of GPIs and GPOs
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_gpioamount ( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  byte               amountgpis;
  byte               amountgpos;

  SERDISP_SRVARG rets[] = { { MBUF_ARGBYTE, &amountgpis, 0 },
                            { MBUF_ARGBYTE, &amountgpos, 0 },
                            { MBUF_ARGEND,  NULL, 0} };

  if (dd->gpevset) {
    amountgpis = dd->gpevset->amountgpis;
    amountgpos = dd->gpevset->amountgpos;
  } else {
    amountgpis = 0;
    amountgpos = 0;
  };

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_gpodesc (con)
   *********************************
   sends full gpo description
   gpid is the index in the driver specific gpo array
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_gpodesc ( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  byte gpid;

  SERDISP_SRVARG args[] = { { MBUF_ARGBYTE, &gpid, 0},
                            { MBUF_ARGEND, NULL, 0} };
  SERDISP_SRVARG rets[] = { { MBUF_ARGBYTE, NULL, 0},  /* id */
                            { MBUF_ARGSTR, NULL, 0},   /* name */
                            { MBUF_ARGSTR, NULL, 0},   /* aliasnames */
                            { MBUF_ARGBYTE, NULL, 0},  /* type */
                            { MBUF_ARGBYTE, NULL, 0},  /* mode */
                            { MBUF_ARGLONG, NULL, 0},  /* minval */
                            { MBUF_ARGLONG, NULL, 0},  /* maxval */
                            { MBUF_ARGSTR, NULL, 0},   /* defines */
                            { MBUF_ARGEND, NULL, 0} };

  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  if (!dd->gpevset || gpid == 0xFF || gpid >= dd->gpevset->amountgpos)
    return -1;

  rets[0].buffer = &(dd->gpevset->gpos[gpid].id);   /* redundant .. maybe needs improvement */
  rets[1].buffer = dd->gpevset->gpos[gpid].name;       rets[1].size = strlen(dd->gpevset->gpos[gpid].name)+1;
  rets[2].buffer = dd->gpevset->gpos[gpid].aliasnames; rets[2].size = strlen(dd->gpevset->gpos[gpid].aliasnames)+1;
  rets[3].buffer = &(dd->gpevset->gpos[gpid].type);
  rets[4].buffer = &(dd->gpevset->gpos[gpid].mode);
  rets[5].buffer = &(dd->gpevset->gpos[gpid].minval);
  rets[6].buffer = &(dd->gpevset->gpos[gpid].maxval);
  rets[7].buffer = dd->gpevset->gpos[gpid].defines;    rets[7].size = strlen(dd->gpevset->gpos[gpid].defines)+1;

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_gpidesc (con)
   *********************************
   sends full gpi description
   gpid is the index in the driver specific gpo array
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_gpidesc ( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  byte gpid;
  long dummy = 0;

  SERDISP_SRVARG args[] = { { MBUF_ARGBYTE, &gpid, 0},
                            { MBUF_ARGEND, NULL, 0} };
  SERDISP_SRVARG rets[] = { { MBUF_ARGBYTE, NULL, 0},  /* id */
                            { MBUF_ARGSTR, NULL, 0},   /* name */
                            { MBUF_ARGSTR, NULL, 0},   /* aliasnames */
                            { MBUF_ARGBYTE, NULL, 0},  /* type */
                            { MBUF_ARGINT, NULL, 0},   /* enabled */
                            { MBUF_ARGLONG, NULL, 0},  /* union { value / fd } -> init. with 0 */
                            { MBUF_ARGEND, NULL, 0} };

  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  if (!dd->gpevset || gpid == 0xFF || gpid >= dd->gpevset->amountgpis)
    return -1;

  rets[0].buffer = &(dd->gpevset->gpis[gpid].id);   /* redundant .. maybe needs improvement */
  rets[1].buffer = dd->gpevset->gpis[gpid].name;       rets[1].size = strlen(dd->gpevset->gpis[gpid].name)+1;
  rets[2].buffer = dd->gpevset->gpis[gpid].aliasnames; rets[2].size = strlen(dd->gpevset->gpis[gpid].aliasnames)+1;
  rets[3].buffer = &(dd->gpevset->gpis[gpid].type);
  rets[4].buffer = &(dd->gpevset->gpis[gpid].enabled);
  rets[5].buffer = &(dummy);  /* init with 0 */

  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_attach (con)
   *********************************
   attaches a display defined in the server configuration to a connection
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_attach( CONDSC *con ) {
  char lcdname[256];
  int  retval;

  SERDISP_SRVARG args[] = { { MBUF_ARGSTR, lcdname, sizeof(lcdname)}, 
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &retval, 0},
                            { MBUF_ARGSTR, sd_errormsg, 0}, 
                            { MBUF_ARGEND, NULL, 0} };

  /* get arguments */
  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  /* attach display to this connection */
  retval = attach_display( con, lcdname );

  if (!retval) {
    /* reset some options to defaults after (re-)attaching the display */
    serdisp_setoption(con->lcd->dd, "INVERT", 0);
  }

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddinfo (con)
   *********************************
   sends device informations
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddinfo( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  int                xreloctab_size = (dd->xreloctab) ? (dd->width + dd->xcolgaps) : 0;
  int                yreloctab_size = (dd->yreloctab) ? (dd->height + dd->ycolgaps) : 0;
  int                ctable_size    = (dd->ctable) ? serdisp_getcolours(dd) : 0;
  SERDISP_SRVARG rets[] = { { MBUF_ARGINT,  &dd->width, 0},
                            { MBUF_ARGINT,  &dd->height, 0}, 
                            { MBUF_ARGINT,  &dd->depth, 0}, 
                            { MBUF_ARGINT,  &dd->curr_rotate, 0}, 
                            { MBUF_ARGINT,  &dd->curr_invert, 0}, 
                            { MBUF_ARGINT,  &dd->feature_contrast, 0}, 
                            { MBUF_ARGINT,  &dd->feature_backlight, 0}, 
                            { MBUF_ARGINT,  &dd->scrbuf_size, 0}, 
                            { MBUF_ARGINT,  &dd->scrbuf_chg_size, 0}, 
                            { MBUF_ARGBYTE, &dd->scrbuf_bits_used, 0}, 
                            { MBUF_ARGLONG, &dd->colour_spaces, 0}, 
                            { MBUF_ARGLONG, &dd->dsparea_width, 0},
                            { MBUF_ARGLONG, &dd->dsparea_height, 0},
                            { MBUF_ARGLONG, &dd->default_bgcolour, 0},
                            { MBUF_ARGINT,  &dd->startxcol, 0}, 
                            { MBUF_ARGINT,  &dd->startycol, 0}, 
                            { MBUF_ARGINT,  &dd->xcolgaps, 0}, 
                            { MBUF_ARGINT,  &dd->ycolgaps, 0}, 
                            { MBUF_ARGINT,  &xreloctab_size, 0}, 
                            { MBUF_ARGINT,  &yreloctab_size, 0}, 
                            { MBUF_ARGINT,  &ctable_size, 0}, 
                            { MBUF_ARGEND,  NULL, 0} }; 

  /* don't expect arguments */
  if( con->con.readbuf.msglen ) {
    sd_srvmsg( 0, "Received unexpected args for command %d on handle %d", 
               con->con.readbuf.cmd, con->con.fd );
    return -1;
  }

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddgettables (con)
   *********************************
   sends relocation and colour tabs
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddgettables( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  SERDISP_SRVARG vals[] = { { MBUF_ARGINTS,  dd->xreloctab, (dd->xreloctab) ? (dd->width + dd->xcolgaps) : 0 },
                            { MBUF_ARGINTS,  dd->yreloctab, (dd->yreloctab) ? (dd->height + dd->ycolgaps) : 0 },
                            { MBUF_ARGLONGS, dd->ctable,    (dd->ctable) ? serdisp_getcolours(dd) : 0 },
                            { MBUF_ARGEND,  NULL, 0} }; 

  /* don't expect arguments */
  if( con->con.readbuf.msglen ) {
    sd_srvmsg( 0, "Received unexpected args for command %d on handle %d", 
               con->con.readbuf.cmd, con->con.fd );
    return -1;
  }

  /* this is a void function ... */

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,vals) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddinit (con)
   *********************************
   initialises device
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddinit( CONDSC *con ) {
  /* don't expect arguments */
  if( con->con.readbuf.msglen ) {
    sd_srvmsg( 0, "Received unexpected args for command %d on handle %d", 
               con->con.readbuf.cmd, con->con.fd );
    return -1;
  }

  /* this is a void function ... */

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,NULL) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddclose (con)
   *********************************
   closes a display
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddclose( CONDSC *con ) {
  /* don't expect arguments */
  if( con->con.readbuf.msglen ) {
    sd_srvmsg( 0, "Received unexpected args for command %d on handle %d", 
               con->con.readbuf.cmd, con->con.fd );
    return -1;
  }

  /* this is void function ... */

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,NULL) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddsetoption (con)
   *********************************
   sets the value of a display device option
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddsetoption( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  char option[256];
  long value;
  int  retval = 0;

  SERDISP_SRVARG args[] = { { MBUF_ARGSTR,  option, sizeof(option)}, 
                            { MBUF_ARGLONG, &value, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGINT, &retval, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 

  /* get arguments */
  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  /* set this option locally */
  serdisp_setoption( dd, option, value );

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddgetoption (con)
   *********************************
   gets the value of a display device option
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddgetoption( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  char option[256];
  char opt_strval[256];
  long opt_numval;
  int  typesize;

  SERDISP_SRVARG args[] = { { MBUF_ARGSTR,  option, sizeof(option)}, 
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGLONG, &opt_numval, 0}, 
                            { MBUF_ARGSTR, opt_strval, -1}, 
                            { MBUF_ARGINT, &typesize, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 

  /* get arguments */
  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  /* set this option locally */
  opt_numval = serdisp_getoption( dd, option, &typesize );

  if (opt_numval == -1) {
    sd_srvmsg( 1, "Option '%s' is unknown", option );
    typesize = -1;
  } else if (typesize == 0) {
    if (opt_numval) {
      int len = strlen( (char*) opt_numval);
      sdtools_strncpy(opt_strval, (char*)opt_numval, (len > sizeof(opt_strval)-1) ? sizeof(opt_strval)-1 : len);
      /* adjust size field in rets[]-entry for opt_strval) */
      rets[1].size = strlen(opt_strval)+1;
    } else {
      opt_strval[0] = '\0';
      rets[1].size = 0;
    }
    opt_numval = 0;
  }

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddupdate (con)
   *********************************
   updates a display device
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddupdate( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;

  /* don't expect arguments */
  if( con->con.readbuf.msglen ) {
    sd_srvmsg( 0, "Received unexpected args for command %d on handle %d", 
               con->con.readbuf.cmd, con->con.fd );
    return -1;
  }

  /* update display */
  serdisp_update( dd );

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,NULL) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddputbuffs (con)
   *********************************
   receives display buffers from client
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddputbuffs( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  SERDISP_SRVARG args[] = { { MBUF_ARGBYTES, dd->scrbuf, dd->scrbuf_size }, 
                            { MBUF_ARGBYTES, dd->scrbuf_chg, dd->scrbuf_chg_size }, 
                            { MBUF_ARGEND, NULL, 0} };

  /* get arguments */
  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,NULL) )
    return -1;

  return 0;
}


/* *********************************
   int cmd_ddgetbuffs (con)
   *********************************
   sends display buffers to client
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddgetbuffs( CONDSC *con ) {
  serdisp_t*         dd = con->lcd->dd;
  SERDISP_SRVARG rets[] = { { MBUF_ARGBYTES, dd->scrbuf, dd->scrbuf_size }, 
                            { MBUF_ARGBYTES, dd->scrbuf_chg, dd->scrbuf_chg_size }, 
                            { MBUF_ARGEND, NULL, 0} };

  /* don't expect arguments */
  if( con->con.readbuf.msglen ) {
    sd_srvmsg( 0, "Received unexpected args for command %d on handle %d", 
               con->con.readbuf.cmd, con->con.fd );
    return -1;
  }

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}




/* *********************************
   int cmd_ddsetsdpixel (con)
   *********************************
   sets a pixel value
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddsetsdpixel( CONDSC *con ) {
  int  x, y;
  SDCol sdcol;
  serdisp_t*         dd = con->lcd->dd;
  SERDISP_SRVARG args[] = { { MBUF_ARGINT,  &x, 0}, 
                            { MBUF_ARGINT,  &y, 0}, 
                            { MBUF_ARGLONG, &sdcol, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 

  /* get arguments */
  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  /* set pixel */
  serdisp_setsdpixel( dd, x, y, sdcol );

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,NULL) )
    return -1;

  return 0;
}

/* *********************************
   int cmd_ddgetsdpixel (con)
   *********************************
   gets a pixel value
   *********************************
   con  ...  connection handler
   *********************************
   0 ... success, <> 0 ... error
*/
int cmd_ddgetsdpixel( CONDSC *con ) {
  int  x, y;
  SDCol sdcol;
  serdisp_t*         dd = con->lcd->dd;
  SERDISP_SRVARG args[] = { { MBUF_ARGINT,  &x, 0}, 
                            { MBUF_ARGINT,  &y, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 
  SERDISP_SRVARG rets[] = { { MBUF_ARGLONG, &sdcol, 0}, 
                            { MBUF_ARGEND, NULL, 0} }; 

  /* get arguments */
  if( serdisp_srv_unpackbuf(&con->con.readbuf,args) )
    return -1;

  /* get pixel color */
  sdcol = serdisp_getsdpixel( dd, x, y );

  /* place return values */
  if( fillret(&con->con,con->con.readbuf.cmd,rets) )
    return -1;

  return 0;
}


/*========================================================================*\
    Fill return buffer with return values
\*========================================================================*/
int fillret( SERDISP_SRVCON *cd, mbuf_cmd cmd, SERDISP_SRVARG *retvals ) {

  /* buffer needs to be virgin or completed */
  if( cd->writebuf.status!=MBUF_VIRGIN && cd->writebuf.status!=MBUF_COMPLETE ) {
    sd_srvmsg( 0, "Tried to fill non-virgin or non-completed buffer!" );
    return -1;
  }
  cd->writebuf.status = 0;

  /* fill buffer for arguments */
  if( serdisp_srv_fillbuf(&cd->writebuf,retvals) ) 
    return -1;
  cd->writebuf.cmd    = cmd;
  cd->writebuf.status = MBUF_LOADED;

  return 0;
}
