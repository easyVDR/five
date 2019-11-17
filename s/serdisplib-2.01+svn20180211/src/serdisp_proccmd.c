/** \file    serdisp_proccmd.c
  *
  * \brief   Command processor
  * \date    (C) 2007-2014
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
  */

/*
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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "serdisplib/serdisp_gpevents.h"
#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_proccmd.h"

#define MAX_CMDLEN 50

/* prototypes for functions with local-only scope */
static int SDCMD_proccmd_gpo    (serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode);
static int SDCMD_proccmd_gpi    (serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode);
static int SDCMD_proccmd_option (serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode);
static int print_msgline        (char* buf, int maxsize, int rccode, int show_rccode, int multiline, const char* format, ...);
static void test_eventlistener  (serdisp_t* dd, SDGP_event_t* currevent);



int print_msgline(char* buf, int maxsize, int rccode, int show_rccode, int multiline, const char* format, ...) {
  va_list ap;

  if (buf) {
    char* startbuf = buf;

    if (maxsize < 10)  /* safety */
      return 0;

    if (show_rccode) {
      snprintf(startbuf, 5, "%03d%c", rccode, ((multiline) ? '-' : ' '));
      startbuf = &startbuf[4];
      maxsize -= 4;
    }

    if (maxsize < 4)  /* safety */
      return 0;

    va_start (ap, format);
    vsnprintf(startbuf, maxsize, format, ap);
    va_end(ap);
    return strlen(buf);
  } else {
    if (show_rccode) {
      printf("%03d%c", rccode, ((multiline) ? '-' : ' '));
    }

    va_start (ap, format);
    vprintf(format, ap);
    va_end(ap);
    return -1;
  }
}


void test_eventlistener(serdisp_t* dd, SDGP_event_t* currevent) {
  if (!currevent)
    return;

  if ( SDGPT_GETCATEGORY( currevent->type ) != SDGPT_CATEGORYSTREAM) {
    fprintf( stderr, "\r%d.%06d -> got event type 0x%02x: ", (int)currevent->timestamp.tv_sec, (int)currevent->timestamp.tv_usec, currevent->type);
    switch (currevent->type) {
      case SDGPT_SIMPLETOUCH: {
        SDGP_evpkt_simpletouch_t  simpletouch;
        memcpy(&simpletouch, &currevent->data, sizeof(SDGP_evpkt_simpletouch_t));

        fprintf( stderr, "Norm: XxY = %3dx%3d | Touch: %4d  (raw: XxY = %3dx%3d, touch: %4d\n",
                (short)(simpletouch.norm_x), (short)(simpletouch.norm_y), (short)(simpletouch.norm_touch),
                (short)(simpletouch.raw_x), (short)(simpletouch.raw_y), (short)(simpletouch.raw_touch)     );
      }
      break;
      case SDGPT_GENERICTOUCH: {
        SDGP_evpkt_generictouch_t  generictouch;
        char evtype = ' ';
        memcpy(&generictouch, &currevent->data, sizeof(SDGP_evpkt_generictouch_t));

        switch (generictouch.type) {
          case SDGPT_TOUCHUP:     evtype = 'U'; break;
          case SDGPT_TOUCHDOWN:   evtype = 'D'; break;
          case SDGPT_TOUCHMOVE:   evtype = '.'; break;
          case SDGPT_TOUCHSCROLL: evtype = 'S'; break;
          default:                evtype = '?';
        }
        fprintf( stderr, "%c | XxY = %3dx%3d | Buttons: %c%c%c | Scroll: %c" ,
                evtype, (generictouch.norm_x), (generictouch.norm_y),
                (generictouch.norm_touch & 0x01) ? 'l' : ' ',
                (generictouch.norm_touch & 0x02) ? 'r' : ' ',
                (generictouch.norm_touch & 0x04) ? 'm' : ' ',
                (generictouch.type == SDGPT_TOUCHSCROLL) ? ((generictouch.norm_touch & 0x10) ? '^' : 'v') : ' '
        );
        switch ((generictouch.flags & 0xF0) >> 4) {
          case 1:  /* raw unscaled x/y values and touch information (if binary touch) included */
            if (generictouch.flags & 0x01) { /* binary touch */
              fprintf( stderr, "  (raw: unscaled x/y: %5d/%5d)\n", generictouch.raw_x, generictouch.raw_y);
            } else {
              fprintf( stderr, "  (raw: unscaled x/y: %5d/%5d; touch: %5d, act. low: %c)\n",
                       generictouch.raw_x, generictouch.raw_y,
                       generictouch.raw_touch, (generictouch.raw_actlow) ? 'y' : 'n'
              );
            }
            break;
          default:
            fprintf(stderr, "\n");
        }
      }
      break;
      case SDGPT_BOOL:
        fprintf( stderr, "%s\n", (currevent->value) ? "true" : "false");
      break;
      case SDGPT_INVBOOL:
        fprintf( stderr, "%s\n", (currevent->value) ? "false" : "true");
      break;
      case SDGPT_VALUE:
        fprintf( stderr, "%d\n", (uint32_t)(currevent->value));
      break;
      case SDGPT_SGNVALUE:
        fprintf( stderr, "%d\n", (int32_t)(currevent->value));
      break;
      default:
        fprintf( stderr, "(undefined test)\n");
    }
    fprintf(stderr, "> ");
  }
}


#define PR_MSGLINE(_rccode, _multiline, args...)    \
  print_msgline(((msgbuf) ? &msgbuf[msgbuf_used]:0), (msgbuf_size-msgbuf_used), (_rccode), show_rccode, (_multiline), args)


/**
  * \brief   processes a command line
  *
  * processes a command line (eg: <b><tt>'GPO SET gpo0 1'</tt></b>, <b><tt>'OPTION SET option1 3'</tt></b>)
  *
  * \param   dd            device descriptor
  * \param   cmdline       command line that should be processed
  * \param   msgbuf        message buffer (or \p NULL if not used)
  * \param   msgbuf_size   size of message buffer
  * \param   show_rccode   show or hide return code information
  *
  * \return  return code (one of <tt>SDCMD_RCCODE_*</tt>)
  *
  * \remarks
  *  the answer returned by the command processor can be \em single or \em multi-lined. \n
  *  each line is preceded by a 3-digit return code and either a space if the answer is single-lined or 
  *  a dash if the answer is multi-lined (unless \em show_rccode is set to 0). \n
  *  \n
  *  <b>format of an answer-line</b>\n
  *  <tt>rccode['-'|' ']message</tt> \n
  *  \n
  *  \b msgbuf \n
  *  the answer will be filled into the message buffer \em msgbuf. the max. size of this buffer is given by \em msgbuf_size. \n
  *  answers bigger than \em msgbuf_size will be cropped. \n
  *  if \em msgbuf is \p NULL the answer will be printed to \p stdout. \n
  *  \n
  *  \b show_rccode \n
  *   depending on \em show_rccode the answer will contain return code information. \n \n
  *   the command line <b><tt>GPO HELP</tt></b> may return the following outputs:
  *  \li show_rccode == 1
  * \verbatim
       214-Commands:
       214-  INFO  HELP  SET  TOGGLE  VERSION \endverbatim
     \li show_rccode == 0
  * \verbatim
       Commands:
         INFO  HELP  SET  TOGGLE  VERSION  \endverbatim
  *
  * \since   1.98.0
  */
int SDCMD_proccmd (serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode) {
  int rc;
  int msgbuf_used = 0;

  if (strncasecmp("VERSION", cmdline, 7) == 0) {
    rc = SDCMD_RCCODE_VERSION;
    msgbuf_used += PR_MSGLINE(rc, 1, "serdisplib version: %d.%d\n", SERDISP_VERSION_MAJOR, SERDISP_VERSION_MINOR);
    msgbuf_used += PR_MSGLINE(rc, 1, " command processor: %d.%d\n", SDCMD_VERSION_MAJOR, SDCMD_VERSION_MINOR);
  } else if (strncasecmp("HELP", cmdline, 4) == 0) {
    rc = SDCMD_RCCODE_HELP;
    msgbuf_used += PR_MSGLINE(rc, 1, "supported commands:\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  VERSION\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  INFO\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  HELP\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  GPI\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  GPO\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  O | OPTION\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "use <command> HELP to receive detailed information\n");
  } else if (!msgbuf && strncasecmp("SWALLOWEDHELP", cmdline, 13) == 0) {
    rc = SDCMD_RCCODE_HELP;
    msgbuf_used += PR_MSGLINE(rc, 1, "extended commands (use '<command> HELP' to receive detailed information):\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  version                      print version information\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  info                         print information about the device\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  gpi [<args>...]              deal with GPIs (general purpose inputs)\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  gpo [<args>...]              deal with GPOs (general purpose outputs)\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  o | option [<args>...]       deal with options\n");
  } else if (strncasecmp("INFO", cmdline, 4) == 0) {
    int ratio = serdisp_getpixelaspect(dd);
    rc = SDCMD_RCCODE_INFO;
    msgbuf_used += PR_MSGLINE(rc, 1, "         device name: %s\n", serdisp_getdisplayname(dd));
    msgbuf_used += PR_MSGLINE(rc, 1, "      width x height: %dx%d\n", serdisp_getwidth(dd), serdisp_getheight(dd));
    msgbuf_used += PR_MSGLINE(rc, 1, "               depth: %d\n", serdisp_getdepth(dd));
    msgbuf_used += PR_MSGLINE(rc, 1, "  pixel aspect ratio: %d:%1.4g\n", 1, ((float)ratio/100.0) );
  } else if (strncasecmp("GPO", cmdline, 3) == 0) {
    char* idx = strchr(cmdline, ' ');

    rc = SDCMD_proccmd_gpo(dd, (idx) ? idx : "", msgbuf, msgbuf_size, show_rccode);
  } else if (strncasecmp("GPI", cmdline, 3) == 0) {
    char* idx = strchr(cmdline, ' ');

    rc = SDCMD_proccmd_gpi(dd, (idx) ? idx : "", msgbuf, msgbuf_size, show_rccode);
  } else if (strncasecmp("O", cmdline, 1) == 0 || strncasecmp("OPTION", cmdline, 6) == 0 ||
             strncasecmp("O ", cmdline, 2) == 0 || strncasecmp("OPTION ", cmdline, 7) == 0
            ) {
    char* idx = strchr(cmdline, ' ');

    rc = SDCMD_proccmd_option(dd, (idx) ? idx : "", msgbuf, msgbuf_size, show_rccode);
  } else {
    rc = SDCMD_RCCODE_ERRCMD;
    PR_MSGLINE(rc, 0, "unknown command\n");
  }
  return rc;
}



/* *********************************
   int SDCMD_proccmd_option (dd, cmdline, msgbuf, msgbuf_size, show_rccode)
   *********************************
   process a command line (eg: "SET gpo0 1", "TOGGLE gpo0")
   *********************************
   dd          ... display descriptor
   cmdline     ... command line to process
   msgbuf      ... message buffer
   msgbuf_size ... size of message buffer
   show_rccode ... show or hide rccode
   *********************************
   returns:
     single- or multilined answer or errormessage:
     format: 
     rccode['-'|' ']message
     rcocde: 3-digit. if followed by space: single line, if followed by '-': multiline

     eg:
     cmdline: 
       "HELP"
     return-value (example):
       "214-Commands:"\
       "214-  INFO  HELP  SET  TOGGLE  VERSION"
*/
int SDCMD_proccmd_option(serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode) {
  char* cmd = (char*)0;
  int cmdlen = 0;
  char* id = (char*)0;
  int idlen = 0;
  char* val = (char*)0;
  int vallen = 0;
  int rc = 0;
  char buffer[MAX_CMDLEN];
  int msgbuf_used = 0;

  /* save space for trailing '\0' */
  msgbuf_size--;

  if (!dd)
    return SDCMD_RCCODE_ERRRUNT;

  if (cmdline && strlen(cmdline) > 0) {
    cmd = sdtools_strlefttrim(cmdline);
    if (cmd && strlen(cmd) > 0) {
      id = strchr(cmd, ' ');

      cmdlen = (id) ? (int)(id - cmd) : strlen(cmd);

      if (id && strlen(sdtools_strlefttrim(id)) > 0) {
        id = sdtools_strlefttrim(id);

        val = strchr(id, ' ');

        idlen = (val) ? (int)(val - id) : strlen(id);

        if (val && strlen(sdtools_strlefttrim(val)) > 0) {
          val = sdtools_strlefttrim(val);

          vallen = strlen(val);
        }
      }
    }

    /* safety exit */
    if (cmdlen > MAX_CMDLEN || idlen > MAX_CMDLEN || vallen > MAX_CMDLEN) {
      rc = SDCMD_RCCODE_ERRSIZE;
      msgbuf_used += PR_MSGLINE(rc, 0, "at least one argument exceeded max. supported argument size. safety exit.\n");
      return rc;
    }
  }

  if (!cmdline || (strlen(cmdline) == 0) || (strncasecmp("HELP", cmd, 4) == 0)) {
    if (!id && !val) {
      rc = SDCMD_RCCODE_HELP;
      msgbuf_used += PR_MSGLINE(rc, 1, "syntax:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  OPTION | O <command>\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "supported commands:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  INFO    LIST     HELP     \n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  SET     TOGGLE   INVERT\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "use OPTION HELP <command> to receive detailed information\n");
    } else if (id && !val) {
      if (strncasecmp("LIST", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "OPTION LIST [<mode>]:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  mode:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    ALL         ... list all options\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    RW          ... list only options that are changeable during runtime\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    (empty)     ... like 'RW'\n");
      } else if (strncasecmp("INFO", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "OPTION INFO <option>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <option> .. name, or alias name of option\n");
      } else if (strncasecmp("SET", id, 3) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "OPTION SET <option> <value>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  sets an option to a new value\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <option> .. name, or alias name of option\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <value>  .. new value\n");
      } else if (strncasecmp("TOGGLE", id, 6) == 0 || strncasecmp("INVERT", id, 6) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "OPTION TOGGLE|INVERT <option>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  toggles a binary GPO\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <gpo>    .. name, alias name or numeric ID of GPO\n");
      } else if (strncasecmp("HELP", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "OPTION HELP [<command>]:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <command>   ... show help info for command <command>\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  (empty)     ... show list of supported commands\n");
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: OPTION HELP [<command>]\n");
    }
  } else if (strncasecmp("LIST", cmd, 4) == 0) {
    int showall = 0;
    serdisp_options_t optiondesc;

    if (idlen > 0 && strncasecmp("ALL", id, 3) == 0)
      showall = 1;
    else if (idlen > 0 && strncasecmp("RW", id, 2) == 0)
      showall = 0;
    else
      showall = 0;

    rc = SDCMD_RCCODE_INFO;

    msgbuf_used += PR_MSGLINE(rc, 1, "display name: %s\n", serdisp_getdisplayname(dd));
    msgbuf_used += PR_MSGLINE(rc, 1, "options:\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  name            aliasnames                     mode\n");
    msgbuf_used += PR_MSGLINE(rc, 1, "  ---------------------------------------------------\n");

    optiondesc.name = "";

    while(serdisp_nextoptiondescription(dd, &optiondesc)) {
      if (showall || (optiondesc.flag & SD_OPTIONFLAG_RW))
        msgbuf_used += PR_MSGLINE(rc, 1, 
             "  %-15s %-30s  %2s\n", 
             optiondesc.name, optiondesc.aliasnames, ((optiondesc.flag & SD_OPTIONFLAG_RW) ? "RW" : "RO")
        );
    }
  } else if (strncasecmp("INFO", cmd, 4) == 0) {
    if (id && !val) {
      serdisp_options_t optiondesc;

      sdtools_strncpy(buffer, id, idlen);

      if (serdisp_getoptiondescription(dd, buffer, &optiondesc)) {
        int typesize;
        long curr_value;

        rc = SDCMD_RCCODE_INFO;

        msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %s\n", "name", optiondesc.name);
        if (optiondesc.aliasnames && strlen(optiondesc.aliasnames) > 0)
           msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %s\n", "alias names", optiondesc.aliasnames);
        if (optiondesc.defines && strlen(optiondesc.defines) > 0)
           msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %s\n", "defines", optiondesc.defines);
        if (optiondesc.minval != -1)
           msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %ld\n", "min. value", optiondesc.minval);
        if (optiondesc.maxval != -1)
           msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %ld\n", "max. value", optiondesc.maxval);
        if (optiondesc.modulo != -1 && optiondesc.modulo != 1)
           msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %ld\n", "modulo", optiondesc.modulo);
        msgbuf_used += PR_MSGLINE(rc, 1, 
           "  %15s: %s\n", 
           "mode", (optiondesc.flag & SD_OPTIONFLAG_RW) ? "read/write" : "readonly"
        );
        curr_value = serdisp_getoption(dd, buffer, &typesize);
        if (curr_value == -1)
          msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %s\n", "current value", "(invalid)");
        else if (typesize!=0)
          msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %ld\n", "current value", curr_value);
        else
          msgbuf_used += PR_MSGLINE(rc, 1, "  %15s: %s\n", "current value", (char*)curr_value);
      } else {
        rc = SDCMD_RCCODE_ERRARGS;
        msgbuf_used += PR_MSGLINE(rc, 1, "  *** option '%s' unknown or unsupported\n", buffer);
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: OPTION INFO <option>\n");
    }
  } else if (strncasecmp("SET", cmd, 3) == 0) {
    if (id && val) {
      long value;
      int opttype;

      sdtools_strncpy(buffer, id, idlen);

      opttype = serdisp_isoption(dd, buffer);
      if ( opttype >= 1) {  /* -1: readonly, 0: non existant */
#if 0
        char* tempptr;
        /* test if val contains numeric value */
        value = strtol(val, &tempptr, 0);
        /* verify if val contained a valid number */
        if (! (val == tempptr || ( (*tempptr != '\0') && (tempptr < (val + strlen(val))) ) )) {
          serdisp_setoption(dd, buffer, value);
          rc = SDCMD_RCCODE_SUCCESS;
          msgbuf_used += PR_MSGLINE(rc, 0, "operation SET executed\n");
        } else {
          if (serdisp_checkdefines(dd, buffer, val, &value) == 0) {
            serdisp_setoption(dd, buffer, value);
            rc = SDCMD_RCCODE_SUCCESS;
            msgbuf_used += PR_MSGLINE(rc, 0, "operation SET executed\n");
          } else {
            rc = SDCMD_RCCODE_ERRARGS;
            msgbuf_used += PR_MSGLINE(rc, 0, "invalid option value '%s'\n", val);
          }
        }
#endif
        int rc = serdisp_scanoptvalue(dd, buffer, val, &value);
        if (rc == 0) {
          serdisp_setoption(dd, buffer, value);
          rc = SDCMD_RCCODE_SUCCESS;
          msgbuf_used += PR_MSGLINE(rc, 0, "operation SET executed\n");
        } else {
          rc = SDCMD_RCCODE_ERRARGS;
          msgbuf_used += PR_MSGLINE(rc, 0, "invalid option value '%s'\n", val);
        }
      } else if (opttype == -1) {
        rc = SDCMD_RCCODE_ERRRUNT;
        msgbuf_used += PR_MSGLINE(rc, 0, "OPTION '%s' is read-only\n", buffer);
      } else {
        byte gpoidx;
        if ( /* test if a numeric GPO fits */
          ((gpoidx = SDGPO_search(dd, buffer)) != SDGPT_ERROR) && 
          ( SDGPT_GETCATEGORY( SDGPO_gettype(dd, gpoidx) ) == SDGPT_CATEGORYVALUE)
        )
        {
          char* tempptr;
          int   isvalidnumber = 1;  /* is 'val' a valid number? */
          /* accept base 2, 10, and base 16 values (base 2 if value starts with '%', base 16 if value starts with 0x or 0X) */
          if (strncasecmp(val, "0x", 2) == 0) {
            value = strtol(val, &tempptr, 16);
          } else if (strncasecmp(val, "%", 1) == 0 && strlen(val) > 1) {
            value = strtol(&val[1], &tempptr, 2);
          } else {
            value = strtol(val, &tempptr, 10);
          }
          /* verify if val contained a valid number */
          isvalidnumber = ((val == tempptr) || (*tempptr != '\0') )  ? 0 : 1;

          if (isvalidnumber)
            rc = SDGPO_setvalue(dd, gpoidx, value);

          if (isvalidnumber && !rc) {
            rc = SDCMD_RCCODE_SUCCESS;
            msgbuf_used += PR_MSGLINE(rc, 0, "operation SET executed on numeric GPO\n");
          } else {
            rc = SDCMD_RCCODE_ERRARGS;
            msgbuf_used += PR_MSGLINE(rc, 0, "invalid GPO value '%s'\n", val);
          }
        } else {
          rc = SDCMD_RCCODE_ERRARGS;
          msgbuf_used += PR_MSGLINE(rc, 0, "unknown OPTION '%.*s'\n", idlen, buffer);
        }
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: OPTION SET <option> <value>\n");
    }
  } else if (strncasecmp("TOGGLE", cmd, 6) == 0 || strncasecmp("INVERT", cmd, 6) == 0) {
    if (id && !val) {
      int opttype;

      sdtools_strncpy(buffer, id, idlen);

      opttype = serdisp_isoption(dd, buffer);
      if ( opttype >= 1) {  /* -1: readonly, 0: non existant */
        serdisp_setoption(dd, buffer, SD_OPTION_TOGGLE);
        rc = SDCMD_RCCODE_SUCCESS;
        msgbuf_used += PR_MSGLINE(rc, 0, "operation '%.*s' successful\n", cmdlen, cmd);
      } else {
        byte gpoidx;
        if ( /* test if a numeric GPO fits */
          ((gpoidx = SDGPO_search(dd, buffer)) != SDGPT_ERROR) && 
          ( SDGPT_GETCATEGORY( SDGPO_gettype(dd, gpoidx) ) == SDGPT_CATEGORYVALUE)
        )
        {
          rc = SDGPO_setvalue(dd, gpoidx, 2);
          if (!rc) {
            rc = SDCMD_RCCODE_SUCCESS;
            msgbuf_used += PR_MSGLINE(rc, 0, "operation TOGGLE executed on numeric GPO\n");
          } else {
            rc = SDCMD_RCCODE_ERRARGS;
            msgbuf_used += PR_MSGLINE(rc, 0, "invalid GPO value '%s'\n", val);
          }
        } else {
          rc = SDCMD_RCCODE_ERRARGS;
          msgbuf_used += PR_MSGLINE(rc, 0, "unknown OPTION '%.*s'\n", idlen, buffer);
        }
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: OPTION %.*s <option>\n", cmdlen, buffer);
    }
  } else {
    rc = SDCMD_RCCODE_ERRCMD;
    msgbuf_used += PR_MSGLINE(rc, 0, "erraneous command '%s'\n", cmd);
  }
  return rc;
}





/* *********************************
   int SDCMD_proccmd_gpo (dd, cmdline, msgbuf, msgbuf_size, show_rccode)
   *********************************
   process a command line (eg: "SET gpo0 1", "TOGGLE gpo0")
   *********************************
   dd          ... display descriptor
   cmdline     ... command line to process
   msgbuf      ... message buffer
   msgbuf_size ... size of message buffer
   show_rccode ... show or hide rccode
   *********************************
   returns:
     single- or multilined answer or errormessage:
     format: 
     rccode['-'|' ']message
     rcocde: 3-digit. if followed by space: single line, if followed by '-': multiline

     eg:
     cmdline: 
       "HELP"
     return-value (example):
       "214-Commands:"\
       "214-  INFO  HELP  SET  TOGGLE  VERSION"
*/
int SDCMD_proccmd_gpo(serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode) {
  char* cmd = (char*)0;
  int cmdlen = 0;
  char* id = (char*)0;
  int idlen = 0;
  char* val = (char*)0;
  int vallen = 0;
  int rc = 0;
  char buffer[MAX_CMDLEN+1];
  int msgbuf_used = 0;

  /* save space for trailing '\0' */
  msgbuf_size--;

  if (!dd)
    return SDCMD_RCCODE_ERRRUNT;

  if (!dd->gpevset || !dd->gpevset->gpos) {
    rc = SDCMD_RCCODE_ERRNAVAIL;
    msgbuf_used += PR_MSGLINE(rc, 1, "no GPOs available for device '%s'\n", serdisp_getdisplayname(dd));
    return rc;
  }

  if (cmdline && strlen(cmdline) > 0) {
    cmd = sdtools_strlefttrim(cmdline);
    if (cmd && strlen(cmd) > 0) {
      id = strchr(cmd, ' ');

      cmdlen = (id) ? (int)(id - cmd) : strlen(cmd);

      if (id && strlen(sdtools_strlefttrim(id)) > 0) {
        id = sdtools_strlefttrim(id);

        val = strchr(id, ' ');

        idlen = (val) ? (int)(val - id) : strlen(id);

        if (val && strlen(sdtools_strlefttrim(val)) > 0) {
          val = sdtools_strlefttrim(val);

          vallen = strlen(val);
        }
      }
    }

    /* safety exit */
    if (cmdlen > MAX_CMDLEN || idlen > MAX_CMDLEN || vallen > MAX_CMDLEN) {
      rc = SDCMD_RCCODE_ERRSIZE;
      msgbuf_used += PR_MSGLINE(rc, 0, "at least one argument exceeded max. supported argument size. safety exit.\n");
      return rc;
    }
  }

  if (!cmdline || (strlen(cmdline) == 0) || (strncasecmp("HELP", cmd, 4) == 0)) {
    if (!id && !val) {
      rc = SDCMD_RCCODE_HELP;
      msgbuf_used += PR_MSGLINE(rc, 1, "syntax:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  GPO <command>\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "supported commands:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  LIST    HELP     \n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  SET     TOGGLE   INVERT\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "use GPO HELP <command> to receive detailed information\n");
    } else if (id && !val) {
      if (strncasecmp("LIST", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPO LIST [<mode>]:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  mode:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    ALL         ... list all defined GPOs, not only single and compound ones\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    NOSUMMARY   ... don't show summary of supported GPOs\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    SUMMARYONLY ... show summary of supported GPOs solely\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    (empty)     ... list single and compound GPOs, summary\n");
      } else if (strncasecmp("SET", id, 3) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPO SET <gpo> <value>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  sets a GPO to a new value\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <gpo>    .. name, alias name or numeric ID of GPO\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <value>  .. new value\n");
      } else if (strncasecmp("TOGGLE", id, 6) == 0 || strncasecmp("INVERT", id, 6) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPO TOGGLE|INVERT <gpo>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  toggles a binary GPO\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <gpo>    .. name, alias name or numeric ID of GPO\n");
      } else if (strncasecmp("HELP", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPO HELP [<command>]:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <command>   ... show help info for command <command>\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  (empty)     ... show list of supported commands\n");
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: GPO HELP [<command>]\n");
    }
  } else if (strncasecmp("LIST", cmd, 4) == 0) {
    int i;
    int cnt_c=0, cnt_s=0, cnt_p=0;
    int showall = 0;
    int showdefault = 1;
    int showsummary = 1;

    if (idlen > 0 && strncasecmp("ALL", id, 3) == 0)
      showall = 1;
    else if (idlen > 0 && strncasecmp("NOSUMMARY", id, 9) == 0)
      showsummary = 0;
    else if (idlen > 0 && strncasecmp("SUMMARYONLY", id, 11) == 0)
      showdefault = 0;

    rc = SDCMD_RCCODE_INFO;
    if (showdefault) {
      msgbuf_used += PR_MSGLINE(rc, 1, "list of GPOs available for device '%s'\n", serdisp_getdisplayname(dd) );
      msgbuf_used += PR_MSGLINE(rc, 1, "   id m name            aliasnames                    \n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  --- - ----------------------------------------------\n");
    }
    for (i = 0; i < SDGPO_getamount(dd); i++) {
      switch (dd->gpevset->gpos[i].mode) {
        case 'S': cnt_s++; break;
        case 'C': cnt_c++; break;
        case 'P': cnt_p++; break;
      }
      if (showdefault) {
        if (dd->gpevset->gpos[i].mode == 'S' || dd->gpevset->gpos[i].mode == 'C' ||
            (dd->gpevset->gpos[i].mode == 'P' && showall)  ) {
          msgbuf_used += PR_MSGLINE(rc, 1, "  %3d %c %-15s %-30s\n",
            i, dd->gpevset->gpos[i].mode, dd->gpevset->gpos[i].name, dd->gpevset->gpos[i].aliasnames
          );
        }
      }
    }
    if (showsummary) {
      msgbuf_used += PR_MSGLINE(rc, 1, "amount of GPOs defined for device '%s': %d\n",
        serdisp_getdisplayname(dd), SDGPO_getamount(dd)
      );
      if (SDGPO_getamount(dd))
        msgbuf_used += PR_MSGLINE(rc, 1, "  single (S): %d, compound (C): %d, parts of compounds (P): %d\n",
          cnt_s, cnt_c, cnt_p
        );
    }
  } else if (strncasecmp("SET", cmd, 3) == 0) {
    if (id && val) {
      byte gpid = 0xFF;
      char* tempptr;
      long value;

      sdtools_strncpy(buffer, id, idlen);
      gpid = SDGPO_search(dd, buffer);

      if (gpid != 0xFF) {

        /* test if val contains numeric value */
        value = strtol(val, &tempptr, 0);
        /* verify if val contained a valid number */
        if (! (val == tempptr || ( (*tempptr != '\0') && (tempptr < (val + strlen(val))) ) )) {
          if (SDGPO_setvalue(dd, gpid, value) >= 0) {
            rc = SDCMD_RCCODE_SUCCESS;
            msgbuf_used += PR_MSGLINE(rc, 0, "operation SET successful\n");
          } else {
            rc = SDCMD_RCCODE_ERRRUNT;
            msgbuf_used += PR_MSGLINE(rc, 1, "operation SET unsuccessful\n");
            msgbuf_used += PR_MSGLINE(rc, 1, "cause: %s\n", sd_geterrormsg());
          }
        } else {
          rc = SDCMD_RCCODE_ERRARGS;
          msgbuf_used += PR_MSGLINE(rc, 0, "invalid argument '%s'\n", val);
        }
      } else {
        rc = SDCMD_RCCODE_ERRARGS;
        msgbuf_used += PR_MSGLINE(rc, 0, "unknown GPO '%.*s'\n", idlen, id);
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: GPO SET <gpo> <value>\n");
    }
  } else if (strncasecmp("TOGGLE", cmd, 6) == 0 || strncasecmp("INVERT", cmd, 6) == 0) {
    if (id && !val) {
      byte gpid = 0xFF;

      sdtools_strncpy(buffer, id, idlen);
      gpid = SDGPO_search(dd, buffer);
      if (gpid != 0xFF) {
        if (SDGPO_setvalue(dd, gpid, 2) >= 0) {
          rc = SDCMD_RCCODE_SUCCESS;
          msgbuf_used += PR_MSGLINE(rc, 0, "command '%.*s' successful\n", cmdlen, cmd);
        } else {
          rc = SDCMD_RCCODE_ERRRUNT;
          msgbuf_used += PR_MSGLINE(rc, 0, "command %.*s unsuccessful\n", cmdlen, cmd);
        }
      } else {
        rc = SDCMD_RCCODE_ERRARGS;
        msgbuf_used += PR_MSGLINE(rc, 0, "unknown GPO '%.*s'\n", idlen, id);
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: GPO %.*s <gpo>\n", cmdlen, cmd);
    }
  } else {
    rc = SDCMD_RCCODE_ERRCMD;
    msgbuf_used += PR_MSGLINE(rc, 0, "erraneous command '%s'\n", cmd);
  }
  return rc;
}


/* *********************************
   int SDCMD_proccmd_gpi (dd, cmdline, msgbuf, msgbuf_size, show_rccode)
   *********************************
   process a GPI-specific command line (eg: "LIST", "TEST gpo0")
   *********************************
   dd          ... display descriptor
   cmdline     ... command line to process
   msgbuf      ... message buffer
   msgbuf_size ... size of message buffer
   show_rccode ... show or hide rccode
   *********************************
   returns:
     single- or multilined answer or errormessage:
     format: 
     rccode['-'|' ']message
     rcocde: 3-digit. if followed by space: single line, if followed by '-': multiline

     eg:
     cmdline: 
       "HELP"
     return-value (example):
       "214-Commands:"\
       "214-  INFO  HELP  SET  TOGGLE  VERSION"
*/
int SDCMD_proccmd_gpi(serdisp_t* dd, char* cmdline, char* msgbuf, int msgbuf_size, int show_rccode) {
  int rc = 0;
  int msgbuf_used = 0;
#ifdef HAVE_LIBPTHREAD
  char* cmd = (char*)0;
  int cmdlen = 0;
  char* id = (char*)0;
  int idlen = 0;
  char* val = (char*)0;
  int vallen = 0;
  char buffer[MAX_CMDLEN+1];

  /* save space for trailing '\0' */
  msgbuf_size--;

  if (!dd)
    return SDCMD_RCCODE_ERRRUNT;

  if (!dd->gpevset || !dd->gpevset->gpis) {
    rc = SDCMD_RCCODE_ERRNAVAIL;
    msgbuf_used += PR_MSGLINE(rc, 1, "no GPIs available for device '%s'\n", serdisp_getdisplayname(dd));
    return rc;
  }

  if (cmdline && strlen(cmdline) > 0) {
    cmd = sdtools_strlefttrim(cmdline);
    if (cmd && strlen(cmd) > 0) {
      id = strchr(cmd, ' ');

      cmdlen = (id) ? (int)(id - cmd) : strlen(cmd);

      if (id && strlen(sdtools_strlefttrim(id)) > 0) {
        id = sdtools_strlefttrim(id);

        val = strchr(id, ' ');

        idlen = (val) ? (int)(val - id) : strlen(id);

        if (val && strlen(sdtools_strlefttrim(val)) > 0) {
          val = sdtools_strlefttrim(val);

          vallen = strlen(val);
        }
      }
    }

    /* safety exit */
    if (cmdlen > MAX_CMDLEN || idlen > MAX_CMDLEN || vallen > MAX_CMDLEN) {
      rc = SDCMD_RCCODE_ERRSIZE;
      msgbuf_used += PR_MSGLINE(rc, 0, "at least one argument exceeded max. supported argument size. safety exit.\n");
      return rc;
    }
  }

  if (!cmdline || (strlen(cmdline) == 0) || (strncasecmp("HELP", cmd, 4) == 0)) {
    if (!id && !val) {
      rc = SDCMD_RCCODE_HELP;
      msgbuf_used += PR_MSGLINE(rc, 1, "syntax:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  GPI <command>\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "supported commands:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  LIST    HELP     \n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  ENABLE  DISABLE\n");
      if (!msgbuf)
        msgbuf_used += PR_MSGLINE(rc, 1, "  TEST\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "use GPI HELP <command> to receive detailed information\n");
    } else if (id && !val) {
      if (strncasecmp("LIST", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPI LIST [<mode>]:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  mode:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    NOSUMMARY   ... don't show summary of supported GPIs\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    SUMMARYONLY ... show summary of supported GPIs solely\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "    (empty)     ... list GPIs and summary\n");
      } else if (strncasecmp("ENABLE", id, 6) == 0 || strncasecmp("DISABLE", id, 7) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPI ENABLE | DISABLE <gpi>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  enables or disables a GPI\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <gpi>    .. name, alias name or numeric ID of GPI\n");
      } else if (!msgbuf && strncasecmp("TEST", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPI TEST <gpi>:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  test a GPI\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <gpi>    .. name, alias name or numeric ID of GPI\n");
      } else if (strncasecmp("HELP", id, 4) == 0) {
        rc = SDCMD_RCCODE_HELP;
        msgbuf_used += PR_MSGLINE(rc, 1, "GPI HELP [<command>]:\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  <command>   ... show help info for command <command>\n");
        msgbuf_used += PR_MSGLINE(rc, 1, "  (empty)     ... show list of supported commands\n");
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: GPI HELP [<command>]\n");
    }
  } else if (strncasecmp("LIST", cmd, 4) == 0) {
    int i;
    int showdefault = 1;
    int showsummary = 1;

    if (idlen > 0 && strncasecmp("NOSUMMARY", id, 9) == 0)
      showsummary = 0;
    else if (idlen > 0 && strncasecmp("SUMMARYONLY", id, 11) == 0)
      showdefault = 0;

    rc = SDCMD_RCCODE_INFO;
    if (showdefault) {
      msgbuf_used += PR_MSGLINE(rc, 1, "list of GPIs available for device '%s'\n", serdisp_getdisplayname(dd) );
      msgbuf_used += PR_MSGLINE(rc, 1, "  id  name            aliasnames                    \n");
      msgbuf_used += PR_MSGLINE(rc, 1, "  --- ----------------------------------------------\n");
    }
    for (i = 0; i < SDGPI_getamount(dd); i++) {
      if (showdefault) {
        msgbuf_used += PR_MSGLINE(rc, 1, "  %2d  %-15s %-30s\n", 
                                  i, dd->gpevset->gpis[i].name, dd->gpevset->gpis[i].aliasnames
        );
      }
    }
    if (showsummary) {
      msgbuf_used += PR_MSGLINE(rc, 1, "amount of GPIs defined for device '%s': %d\n",
        serdisp_getdisplayname(dd), SDGPI_getamount(dd)
      );
    }
  } else if (strncasecmp("ENABLE", cmd, 6) == 0 || strncasecmp("DISABLE", cmd, 7) == 0) {
    if (id && !val) {
      byte gpid = 0xFF;

      sdtools_strncpy(buffer, id, idlen);
      gpid = SDGPI_search(dd, buffer);

      if (gpid == 0xFF) {
        rc = SDCMD_RCCODE_ERRARGS;
        msgbuf_used += PR_MSGLINE(rc, 0, "unknown GPI '%s'\n", buffer);
      } else {
        if (strncasecmp("ENABLE", cmd, 6) == 0) {
          int rc_enable = SDGPI_enable(dd, gpid, 1);
          if (rc_enable == 0) {
            rc = SDCMD_RCCODE_SUCCESS;
            msgbuf_used += PR_MSGLINE(rc, 0, "GPI '%s' enabled successfully\n", buffer);
          } else if (rc_enable == 1) {
            rc = SDCMD_RCCODE_ALREADY;
            msgbuf_used += PR_MSGLINE(rc, 0, "GPI '%s' already enabled\n", buffer);
          } else {
            rc = SDCMD_RCCODE_ERRRUNT;
            msgbuf_used += PR_MSGLINE(rc, 0, "enabling of GPI '%s' unsuccessful\n", buffer);
          }
        } else {
          int rc_enable = SDGPI_enable(dd, gpid, 0);
          if (rc_enable == 0) {
            rc = SDCMD_RCCODE_SUCCESS;
            msgbuf_used += PR_MSGLINE(rc, 0, "GPI '%s' disabled successfully\n", buffer);
          } else if (rc_enable == 1) {
            rc = SDCMD_RCCODE_ALREADY;
            msgbuf_used += PR_MSGLINE(rc, 0, "GPI '%s' already disabled\n", buffer);
          } else {
            rc = SDCMD_RCCODE_ERRRUNT;
            msgbuf_used += PR_MSGLINE(rc, 0, "disabling of GPI '%s' unsuccessful\n", buffer);
          }
        }
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: GPI ENABLE | DISABLE <gpo>\n");
    }
  } else if (strncasecmp("TEST", cmd, 4) == 0) {
    if (id && !val) {
      byte gpid = 0xFF;

      sdtools_strncpy(buffer, id, idlen);
      gpid = SDGPI_search(dd, buffer);

      if (msgbuf) {
        rc = SDCMD_RCCODE_ERRNAVAIL;
        msgbuf_used += PR_MSGLINE(rc, 0, "testing of GPIs not available in this mode\n");
      } else if (gpid == 0xFF) {
        rc = SDCMD_RCCODE_ERRARGS;
        msgbuf_used += PR_MSGLINE(rc, 0, "unknown GPI '%s'\n", buffer);
      } else {
        int fd;
        fd_set  fd_read;
        SDGP_event_t event;
        struct timeval     tv;
        int                to = 1000;   /* millisec */
        int                retval;
        char               wheel[] = { '-', '\\', '|', '/' };
        int                wheelstep = 0;

        if (!SDGPI_isenabled(dd, gpid))
          SDGPI_enable(dd, gpid, 1);

        if (SDGPT_GETCATEGORY(SDGPI_gettype(dd, gpid)) == SDGPT_CATEGORYSTREAM) {
          if (( fd = SDGPI_getstreamfd(dd, gpid)) != 0) {
            fprintf(stderr, "  dumping input events for input filedescriptor %d. hit Enter to abort...\n", fd);

            for(;;) {
              FD_ZERO( &fd_read );
              FD_SET( fd, &fd_read );
              FD_SET( 0,  &fd_read );
              tv.tv_sec  = to/1000;
              tv.tv_usec = (to-tv.tv_sec*1000)*1000;
              if( select(fd+1,&fd_read,NULL,NULL,&tv)<0 ) {
                fprintf( stderr, "Select failed: %s\n", strerror(errno) );
                break;
              }
              if(FD_ISSET(fd,&fd_read) ) {
                retval = read( fd, &event, sizeof(event) );
                if( retval == -1 ) {
                  fprintf( stderr, "error when reading fd %d: %s\n", fd, strerror(errno) );
                  break;
                }   
                if( !retval ) {
                  fprintf( stderr, "fd %d not fed any longer...\n", fd );
                  break;
                }   
                if( retval!=sizeof(event) ) {
                  fprintf( stderr, "read from fd %d returned short message %d<%d.\n", fd, retval, (int)sizeof(event) );
                  break;
                }

                /* convert header from network byte order to network byte order */
                SDGPT_event_header_ntoh(&event);

                if (event.type == SDGPT_NORMRCSTREAM) {   /* normalised remote control stream */
                  uint32_t rcstream[] = {0, 0, 0, 0 };

                  retval = read( fd, rcstream, sizeof(rcstream) );
                  if( !retval ) {
                    fprintf( stderr, "fd %d not fed any longer...\n", fd );
                    break;
                  }   
                  if( retval!=sizeof(rcstream) ) {
                    fprintf( stderr, "read from fd %d returned short message %d<%d.\n", fd, retval, (int)sizeof(rcstream) );
                    break;
                  }

                  /* convert payload from network byte order to host byte order */
                  SDGPT_event_payload_ntoh(rcstream, sizeof(rcstream), (byte)sizeof(rcstream[0]));

                  fprintf(stderr, "\r%ld.%06d -> got event type %d, values %08x %08x %08x %08x\n", 
                    (long)event.timestamp.tv_sec, (int)event.timestamp.tv_usec, event.type, 
                    rcstream[0], rcstream[1], rcstream[2], rcstream[3]);
                } else {
                  fprintf(stderr, "\rUnknown event %d occured. ignoring ...\n", event.type);
                  retval = read(fd, NULL, 0);
                }
              } else {
                fprintf(stderr,  "\r %c ", wheel[wheelstep] );
                wheelstep = (wheelstep+1)%sizeof(wheel);
                fflush( stdout );
              }
              if( FD_ISSET(0,&fd_read) ) {
                fprintf(stderr,  "\rbreak!\n" );
                break;
              }
            }
          }
          SDGPI_enable(dd, gpid, 0);
        } else { /* all event types but streams */
          /* is listener already in list -> stop testing (try to delete listener) */
          int rc = SDEVLP_del_listener(dd, gpid, &test_eventlistener);
          if (rc == 1) { /* listener has not been in list: add it */
            fprintf(stderr, "listener added\n");
            SDEVLP_add_listener(dd, gpid, &test_eventlistener);
          } else if (rc == 0) {
            fprintf(stderr, "listener deleted\n");
            /* no more listeners for gpid: disable gpid */
            if (SDEVLP_count_listeners(dd, gpid) == 0)
              SDGPI_enable(dd, gpid, 0);
          }
        }
      }
    } else {
      rc = SDCMD_RCCODE_ERRARGS;
      msgbuf_used += PR_MSGLINE(rc, 1, "invalid arguments:\n");
      msgbuf_used += PR_MSGLINE(rc, 1, "usage: GPI TEST <gpo>\n");
    }
  } else {
    rc = SDCMD_RCCODE_ERRCMD;
    msgbuf_used += PR_MSGLINE(rc, 0, "erraneous command '%s'\n", cmd);
  }
#else /* !defined HAVE_LIBPTHREAD */
  rc = SDCMD_RCCODE_ERRRUNT;
  PR_MSGLINE(rc, 0, "GPIs not supported due to missing support for libpthread\n");
  { /* make gcc ignore unused listener function */
    void* tmp = (void*)test_eventlistener;
    tmp = tmp;
  }
#endif
  return rc;
}
