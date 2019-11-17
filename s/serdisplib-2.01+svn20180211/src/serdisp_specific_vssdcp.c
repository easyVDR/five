/*
 *************************************************************************
 *
 * serdisp_specific_vssdcp.c
 * routines for controlling displays using protocol 'VSSDCP' (created by me)
 *
 *************************************************************************
 *
 * copyright (C) 2014-2018  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>     /* write() */

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"

#define DEBUG

/* #define OPT_USEOLDUPDATEALGO */

#define MAX_ACK_RETRY 20


/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_VSSDCP        1

serdisp_options_t serdisp_vssdcp_options[] = {
   /*  name       aliasnames        min    max mod int defines  */
   {  "BAUDRATE",   "BAUD",          0,    18,  1, 0,
     "AUTO=0,9600=1,19200=2,38400=3,57600=4,115200=5,230400=6,460800=7,500000=8,576000=9,921600=10,1000000=11,1152000=12,1500000=13,2000000=14,2500000=15,3000000=16,3500000=17,4000000=18"}
  ,{  "ACKTYPE",    "ACK",           0,     2,  1, 1,  "OFF=0,ACKONLY=1,ALL=2"}
  ,{  "EVENTS",     "",              0,     1,  1, 1,  "OFF=0,ON=1,NO=0,YES=1"}
  ,{  "CONTRAST",   "",              0,    10,  1, 1,  ""}
  ,{  "BACKLIGHT",  "",              0,     1,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",              0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
};


/* internal typedefs and functions */

static void serdisp_vssdcp_init      (serdisp_t*);
static void serdisp_vssdcp_update    (serdisp_t*);
static int  serdisp_vssdcp_setoption (serdisp_t*, const char*, long);
static void serdisp_vssdcp_clear     (serdisp_t*);
static void serdisp_vssdcp_close     (serdisp_t*);

static void write_byte               (serdisp_t*, byte);
static size_t write_bytes              (serdisp_t*, byte[], int);
static void getACK                   (serdisp_t*);

typedef struct serdisp_vssdcp_specific_s {
  char     name[32];

  byte     coldepth[4];          /* depth for {alpha, red, green, blue}  */
  byte     colorder[4];          /* channel order for {alpha, red, green, blue}, if unused > 3 */
  byte     colLUT[4];
  byte     colchannels;

  byte     vers_maj;             /* major version of VSSDCP-protocol */
  byte     vers_min;             /* minor version of VSSDCP-protocol */

  byte     clflag_acktype;       /* acknowledge type */
  byte     clflag_events;        /* events yes (1), no (0) */

  byte     feature_brightness;   /* max. brightness that can be set. 0: no support for brightness */
  byte     feature_immedUpdate;  /* commit changes: 0: extra command needed, 1: drawing commands update immediately */
  byte     feature_gpis;         /* amount of supported GPIs */
  byte     feature_gpos;         /* amount of supported GPOs */

  uint16_t payload_size;         /* max. payload size - 1  (0 = 1 byte, 65535 = 64k) */
  byte     pixel_dependency;     /* single pixel depenency to neighbour pixels (only relevant for greyscale displays):
                                    0: no pixel dependency (single pixel can be set autonomously)
                                    1: horizontal pixel dep. (single pixel cannot be set without sibling pixels)
                                    2: vertical pixel dep. (single pixel cannot be set without pixels from same page)
                                 */
} serdisp_vssdcp_specific_t;


static serdisp_vssdcp_specific_t* serdisp_vssdcp_internal_getStruct(serdisp_t* dd) {
  return (serdisp_vssdcp_specific_t*)(dd->specific_data);
}



/* callback-function for setting non-standard options */
static void* serdisp_vssdcp_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "BAUDRATE")) {
    *typesize = sizeof(unsigned int);
    return &(dd->sdcd->rs232.baudrate);
  } else if (serdisp_compareoptionnames(dd, optionname, "ACKTYPE")) {
    *typesize = sizeof(byte);
    return &(serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype);
  } else if (serdisp_compareoptionnames(dd, optionname, "EVENTS")) {
    *typesize = sizeof(byte);
    return &(serdisp_vssdcp_internal_getStruct(dd)->clflag_events);
  }
  return 0;
}

void write_byte (serdisp_t* dd, byte item) {
  write(dd->sdcd->fd, &item, 1);
  /*send(dd->sdcd->fd, &item, 1, 0);*/
}

size_t write_bytes (serdisp_t* dd, byte buf[], int len) {
  size_t rv = write(dd->sdcd->fd, buf, len);
  /*size_t rv = send(dd->sdcd->fd, buf, len, 0);*/
  return rv;
}


void getACK(serdisp_t* dd) {
  char tmp, id;
  int cnt = 0;

  if (serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype == 0)
    return;

  sdtools_nsleep(100);

  do {
    if (cnt)
      sdtools_nsleep(100);
    SDCONN_readstream(dd->sdcd, (byte*) &id, 1);

    if (id == 'X') {
      SDCONN_readstream(dd->sdcd, (byte*) &tmp, 1);
    } else if (id == 'D') {
      fprintf(stderr, "DEBUG: ");
      do {
        SDCONN_readstream(dd->sdcd, (byte*) &tmp, 1);
        fprintf(stderr, "%c", (tmp != 0) ? tmp : '\n');
      } while (tmp != '\0');
    } else {
      cnt ++;
    }
  } while ( (id != 'X' && id != 'D') && (cnt < MAX_ACK_RETRY));
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_vssdcp_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_vssdcp_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;
  byte infobuffer[128];
  int i;
  char* patternptr;
  int patternlen;
  int patternborder;
  int valid = 1;


  if (! sdcd ) {
    sd_error(SERDISP_EMALLOC, "%s(): output device not open", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));


  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("VSSDCP", dispname))
    dd->dsp_id = DISPID_VSSDCP;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_vssdcp.c", dispname);
    return (serdisp_t*)0;
  }

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_vssdcp_specific_t)) )) {
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_RS232 | SERDISPCONNTYPE_INET;

  /* force device-check at this point because _setup already does some communication with the display */
  if (! (dd->connection_types & dd->sdcd->conntype)) {
    sd_error(SERDISP_EDEVNOTSUP, "'%s' only supports 'RS232' as connection type (try using 'RS232:<device>')", dispname);
    free(dd->specific_data);
    free(dd);
    return (serdisp_t*)0;
  }

  /* per display settings */

  /* pre-init */
  dd->feature_contrast  = 0;
  dd->feature_invert    = 0;
  dd->feature_backlight = 0;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 6;

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported colour spaces */
  /*  dd->colour_spaces     = SD_CS_SELFEMITTING | SD_CS_RGB565 | SD_CS_TRUECOLOUR;*/
  dd->colour_spaces     = SD_CS_GREYSCALE | SD_CS_RGB565 | SD_CS_TRUECOLOUR;

  dd->fp_init           = &serdisp_vssdcp_init;
  dd->fp_update         = &serdisp_vssdcp_update;
  dd->fp_clear          = &serdisp_vssdcp_clear;
  dd->fp_close          = &serdisp_vssdcp_close;
  dd->fp_setoption      = &serdisp_vssdcp_setoption;
  dd->fp_getvalueptr    = &serdisp_vssdcp_getvalueptr;

  /* pre-init some options before calling setup */
  serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype = 0; /* no acks by default. for arduino: set at least acktype=1 */
  serdisp_vssdcp_internal_getStruct(dd)->clflag_events = 0;  /* no events by default */


  serdisp_setupstructinfos(dd, 0, 0, serdisp_vssdcp_options);
  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  /* contains numeric id after serdisp_setupstructinfos() -> convert to constant */
  switch(dd->sdcd->rs232.baudrate) {
    case  1: dd->sdcd->rs232.baudrate =    B9600; break;
    case  2: dd->sdcd->rs232.baudrate =   B19200; break;
    case  3: dd->sdcd->rs232.baudrate =   B38400; break;
    case  4: dd->sdcd->rs232.baudrate =   B57600; break;
    case  5: dd->sdcd->rs232.baudrate =  B115200; break;
    case  6: dd->sdcd->rs232.baudrate =  B230400; break;
    case  7: dd->sdcd->rs232.baudrate =  B460800; break;
    case  8: dd->sdcd->rs232.baudrate =  B500000; break;
    case  9: dd->sdcd->rs232.baudrate =  B576000; break;
    case 10: dd->sdcd->rs232.baudrate =  B921600; break;
    case 11: dd->sdcd->rs232.baudrate = B1000000; break;
    case 12: dd->sdcd->rs232.baudrate = B1152000; break;
    case 13: dd->sdcd->rs232.baudrate = B1500000; break;
    case 14: dd->sdcd->rs232.baudrate = B2000000; break;
    case 15: dd->sdcd->rs232.baudrate = B2500000; break;
    case 16: dd->sdcd->rs232.baudrate = B3000000; break;
    case 17: dd->sdcd->rs232.baudrate = B3500000; break;
    case 18: dd->sdcd->rs232.baudrate = B4000000; break;
    default: dd->sdcd->rs232.baudrate =       B0;
  }

  dd->sdcd->rs232.c_cstopb = 0;  /* one stop bit */
  dd->sdcd->rs232.c_rtscts = 0;  /* no flow control */
  dd->sdcd->rs232.c_cread  = 1;   /* enable receiver */
  dd->sdcd->rs232.c_local  = 0;   /* enable local mode */

  dd->sdcd->rs232.c_set_vmin = 1;
  dd->sdcd->rs232.c_set_vtime = 1;

  dd->sdcd->rs232.c_cc_vmin = 0;
  dd->sdcd->rs232.c_cc_vtime = 10;


  serdisp_vssdcp_internal_getStruct(dd)->payload_size = 63;  /* default max. payload size: 64 byte */

  /* send some dummy bytes to stabilise communication (without this initial communication sometimes fails) */
  for (i = 0; i < 5; i++) {
    infobuffer[0] = 0;
    write_bytes (dd, infobuffer, 1);
    sdtools_nsleep(100);
  }

#if 0
  fprintf(stderr, "sending 'Q'");
  infobuffer[0] = 'Q';
  write_bytes (dd, infobuffer, 1);
  sdtools_nsleep(1000);

  fprintf(stderr, ": ");
  i = 0;
  do {
    infobuffer[i] = SDCONN_read(dd->sdcd, 0);
    fprintf(stderr, "[%d]: %02x (%c) ", i, infobuffer[i], infobuffer[i]);
  } while ((infobuffer[i] != 0) && (infobuffer[i] != 'X' ) && (i++ < 127) );

  if (i < 2) {
    sdtools_nsleep(100000);
  }

  fprintf(stderr, "XX: i=%d, infobuffer[i-1]=%02x\n", i, infobuffer[i]);
  if (i == 0 && (infobuffer[0] == 'X')) {
    infobuffer[0] = SDCONN_read(dd->sdcd, 0);
  }
  fprintf(stderr, ".\n");
#endif

  /* send command to get display / module information */
  infobuffer[0] = 'I';
  infobuffer[1] =
    (serdisp_vssdcp_internal_getStruct(dd)->clflag_events << 2) |
    (serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype);
  write_bytes (dd, infobuffer, 2);

  i = 0;
  do {
    infobuffer[i] = SDCONN_read(dd->sdcd, 1);
  } while ((infobuffer[i++] != 0) && i < 128);

  fprintf(stderr, "[len=%d]: %s\n", (int)strlen((char*)infobuffer), (char*)infobuffer);

  patternptr = (char*)infobuffer;
  patternlen = -1;
  patternborder = strlen(patternptr);
  while( (patternptr = sdtools_nextpattern(patternptr, ';', &patternlen, &patternborder)) ) {
    char *valueptr = &patternptr[2];
    int  valuelen  = patternlen - 2;
    long  tempval;
    int  rc;
    switch(patternptr[0]) {
      case 'N':
        sdtools_strncpy( serdisp_vssdcp_internal_getStruct(dd)->name, valueptr, (valuelen > 31) ? 31 : valuelen);
        break;
      case 'V': {
        double tempdouble;
        rc = sdtools_strtosd(valueptr, ';', &tempdouble);
        if (rc) {
          serdisp_vssdcp_internal_getStruct(dd)->vers_maj = (int)tempdouble;
          serdisp_vssdcp_internal_getStruct(dd)->vers_min = (int)(tempdouble * 10.0) % 10;
        } else {
//fprintf(stderr, "invalid: V\n");
          valid = 0;
        }
        break;
      }
      case 'W':
        rc = sdtools_strtol(valueptr, ';', 10, &tempval);
        if (rc) {
          if (tempval >=0 && tempval < 4096) {
            dd->width = tempval;
          }
        } else {
//fprintf(stderr, "invalid: W\n");
          valid = 0;
        }
        break;
      case 'H':
        rc = sdtools_strtol(valueptr, ';', 10, &tempval);
        if (rc) {
          if (tempval >=0 && tempval < 4096) {
            dd->height = tempval;
          }
        } else {
//fprintf(stderr, "invalid: H\n");
          valid = 0;
        }
        break;
      case 'P':
        rc = sdtools_strtol(valueptr, ';', 10, &tempval);
        if (rc) {
          if (tempval >=0 && tempval <= 32) {
            dd->depth = tempval;
          }
        } else {
//fprintf(stderr, "invalid: P\n");
          valid = 0;
        }
        break;
      case 'C': {  /* colour format */
        int i = 0;
        byte skip_depths = 0;

        serdisp_vssdcp_internal_getStruct(dd)->colchannels = 0;

        for (i = 0; i <=3 ; i++) {
          serdisp_vssdcp_internal_getStruct(dd)->colorder[i] = 0xFF;
          serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] = 0xFF;
        }
        i = 0;
        while ( valid && (! skip_depths) && (i <= 3) && (i < valuelen) ) {
//fprintf(stderr, "C valueptr: %s\n", valueptr);
          switch (valueptr[i]) {
            case 'A':
//fprintf(stderr, "C case A\n");
              serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] = 0;
              serdisp_vssdcp_internal_getStruct(dd)->colorder[0] = i++;
              serdisp_vssdcp_internal_getStruct(dd)->colchannels ++;
              break;
            case 'R':
//fprintf(stderr, "C case R\n");
              serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] = 1;
              serdisp_vssdcp_internal_getStruct(dd)->colorder[1] = i++;
              serdisp_vssdcp_internal_getStruct(dd)->colchannels ++;
              break;
            case 'G':
//fprintf(stderr, "C case G\n");
              serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] = 2;
              serdisp_vssdcp_internal_getStruct(dd)->colorder[2] = i++;
              serdisp_vssdcp_internal_getStruct(dd)->colchannels ++;
              break;
            case 'B':
//fprintf(stderr, "C case B\n");
              serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] = 3;
              serdisp_vssdcp_internal_getStruct(dd)->colorder[3] = i++;
              serdisp_vssdcp_internal_getStruct(dd)->colchannels ++;
              break;
            case 'K': /* b/w or grayscale: use red order position */
//fprintf(stderr, "C case K\n");
              if (i == 0) {
                serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] = 1;
                serdisp_vssdcp_internal_getStruct(dd)->colorder[1] = i++;
                serdisp_vssdcp_internal_getStruct(dd)->colchannels ++;
                skip_depths = 1;
              } else {
//fprintf(stderr, "invalid: C0\n");
                valid = 0;
              }
              break;
            default:
              if ( (valueptr[i] >= '1' && valueptr[i] <= '9') || (valueptr[i] >= 'a' || valueptr[i] <= 'f') ) {
                skip_depths = 1;
              } else {
//fprintf(stderr, "invalid: C1\n");
                valid = 0;
              }
          }
        }
        /* expression requires amount of colour channels times 2 (RGB888 ->  3 * 2, ARGB8888 -> 4 * 2) */
        if ((serdisp_vssdcp_internal_getStruct(dd)->colchannels * 2) < valuelen) {
//fprintf(stderr, "invalid: C2: valuelen: %d, colchannels: %d\n", valuelen, serdisp_vssdcp_internal_getStruct(dd)->colchannels  );
          valid = 0;
        }
        if (valid) {
          i = 0;
          while (valid && i < serdisp_vssdcp_internal_getStruct(dd)->colchannels ) {
            byte curr_val = valueptr[serdisp_vssdcp_internal_getStruct(dd)->colchannels + i];
            if ( (curr_val >= '1' && curr_val <= '9') || (curr_val >= 'a' && curr_val <= 'f') ) {
              serdisp_vssdcp_internal_getStruct(dd)->coldepth[ serdisp_vssdcp_internal_getStruct(dd)->colLUT[i] ]
              =
              (isdigit(curr_val)) ? curr_val - '0' : (curr_val - 'a') + 0xa;
            } else {
//fprintf(stderr, "invalid: C3\n");
              valid = 0;
            }
            i++;
          }
        }
#if 0
        for (i = 0; i < 4; i++) {
          if (serdisp_vssdcp_internal_getStruct(dd)->colorder[i] <= 3)
            fprintf(stderr, "COL: [%d]: %d -> %x\n", i, serdisp_vssdcp_internal_getStruct(dd)->colorder[i], serdisp_vssdcp_internal_getStruct(dd)->coldepth[i]);
        }
#endif

        break;
      }
      case 'B':  /* max. payload size */
        rc = sdtools_strtol(valueptr, ';', 10, &tempval);
        if (rc) {
          if (tempval >=0 && tempval <= 65535) {
            serdisp_vssdcp_internal_getStruct(dd)->payload_size = tempval;
          }
        } else {
//fprintf(stderr, "invalid: B\n");
          valid = 0;
        }
        break;
      case 'F': { /* features */
        int pos1 = 0;
        int pos2 = 0;
        int tmppos;
        int rc;
        long rv;

        if (valuelen < 2) {
//fprintf(stderr, "invalid: F valuelen\n");
          valid = 0;
        }

        while (valid && pos2 > -1) {
          tmppos = pos2 + 1;
          while ((tmppos < valuelen) && (tmppos > pos2)) {
            if (((valueptr[tmppos] >= 'A') && (valueptr[tmppos] <= 'Z')) || ((valueptr[tmppos] >= 'a') && (valueptr[tmppos] <= 'z'))) {
              pos1 = pos2;
              pos2 = tmppos;
            } else {
              tmppos ++;
            }
          }
          if (tmppos >= valuelen) {
            pos2 = -1;
          }
          rc = sdtools_strtol( &valueptr[pos1+1], (pos2 == -1) ? ';' : valueptr[pos2], 10, &rv);
          if (rc) {
            switch (valueptr[pos1]) {
              case 'A':
                serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype = (rv >= 0 && rv <= 2) ? (byte)rv : 0;
                break;
              case 'b':
                dd->feature_backlight = ((byte)rv == 1) ? 1 : 0;
                break;
              case 'c':
                dd->feature_contrast = ((byte)rv == 1) ? 1 : 0;
                break;
              case 'e':
                if (rv == 1) {
                  dd->colour_spaces |= SD_CS_SELFEMITTING;
                }
                break;
              case 'i':
                dd->feature_invert = ((byte)rv == 1) ? 1 : 0;
                break;
              case 'p':
                serdisp_vssdcp_internal_getStruct(dd)->pixel_dependency = (rv >= 0 && rv <= 2) ? (byte)rv : 0;
                break;
              case 'B':
                serdisp_vssdcp_internal_getStruct(dd)->feature_brightness = (rv >= 0 && rv <= 255) ? (byte)rv : 0;
                break;
              case 'u':
                serdisp_vssdcp_internal_getStruct(dd)->feature_immedUpdate = (rv >= 0 && rv <= 1) ? (byte)rv : 0;
                break;
              case 'I':
                serdisp_vssdcp_internal_getStruct(dd)->feature_gpis = (rv >= 0 && rv <= 255) ? (byte)rv : 0;
                break;
              case 'O':
                serdisp_vssdcp_internal_getStruct(dd)->feature_gpos = (rv >= 0 && rv <= 255) ? (byte)rv : 0;
                break;
            }
          } else {
//fprintf(stderr, "invalid: F else\n");
            valid = 0;
          }
          pos1 = pos2;
        }
        break;
      }
    }
    fprintf(stderr, "PATTERN: %.*s (len=%d, border=%d, valid=%d)\n", patternlen, patternptr, patternlen, patternborder, valid);
  }

  fprintf(stderr, "FEATURES\n");
  fprintf(stderr, "  clflag_acktype:       %d\n", serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype);
  fprintf(stderr, "  feature_backlight:    %d\n", dd->feature_backlight);
  fprintf(stderr, "  feature_contrast:     %d\n", dd->feature_contrast);
  fprintf(stderr, "  feature_selfemitting: %d\n", (dd->colour_spaces & SD_CS_SELFEMITTING) ? 1 : 0 );
  fprintf(stderr, "  feature_invert:       %d\n", dd->feature_invert);
  fprintf(stderr, "  payload_size:         %d\n", serdisp_vssdcp_internal_getStruct(dd)->payload_size + 1);
  fprintf(stderr, "  pixel_dependency:     %d\n", serdisp_vssdcp_internal_getStruct(dd)->pixel_dependency);
  fprintf(stderr, "  feature_brightness:   %d\n", serdisp_vssdcp_internal_getStruct(dd)->feature_brightness);
  fprintf(stderr, "  feature_immedUpdate:  %d\n", serdisp_vssdcp_internal_getStruct(dd)->feature_immedUpdate);
  fprintf(stderr, "  feature_gpis:         %d\n", serdisp_vssdcp_internal_getStruct(dd)->feature_gpis);
  fprintf(stderr, "  feature_gpos:         %d\n", serdisp_vssdcp_internal_getStruct(dd)->feature_gpos);

  if (! valid) {
    sd_error(SERDISP_ERUNTIME, "'%s': error when processing VSSDCP information", dispname);
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  if ( dd->depth < 8 ) {
    if ( serdisp_vssdcp_internal_getStruct(dd)->pixel_dependency != 2 ) {
      dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
      dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
    }
    dd->colour_spaces     = SD_CS_GREYSCALE;
  }

  sd_debug(
    2, "%s(): Name: %s, WxHxD: %dx%dx%d, protocol version: %d.%d", __func__,
    serdisp_vssdcp_internal_getStruct(dd)->name,
    dd->width, dd->height, dd->depth,
    serdisp_vssdcp_internal_getStruct(dd)->vers_maj, serdisp_vssdcp_internal_getStruct(dd)->vers_min
  );

  sdtools_nsleep(2000);

/*  serdisp_setupstructinfos(dd, 0, 0, serdisp_vssdcp_options); */

  return dd;
}


/* *********************************
   void serdisp_vssdcp_init(dd)
   *********************************
   initialise an RS232 based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_vssdcp_init(serdisp_t* dd) {
  if (serdisp_vssdcp_internal_getStruct(dd)->feature_brightness)
    serdisp_setoption(dd, "BRIGHTNESS", dd->curr_dimming);
  sd_debug(2, "%s(): done with initialising", __func__);
}


/* *********************************
   void serdisp_vssdcp_update(dd)
   *********************************
   updates a generic RS232 based display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_vssdcp_update(serdisp_t* dd) {
  int i,j;
  int max_payload_size = serdisp_vssdcp_internal_getStruct(dd)->payload_size + 1;
  byte buf[ max_payload_size];
  SDCol col;
  int w = dd->width;
  int h = dd->height;
  int maxChunk =  ((((int)( max_payload_size * 8)) / dd->depth) * dd->depth ) / 8;

  int xt = 0;
  int yt = 0;
  int xb = w - 1;
  int yb = h - 1;
  int cw;
  int ch;
  int rc = 0;

  int total = 0;
  int pos = 0;
  int curr = 0;
  byte pbb = (dd->depth < 8) ? (8 / dd->depth) : 1;
  int p;

  /* get bounding box for changed pixels */
#ifdef OPT_USEOLDUPDATEALGO
  rc = 1;
#else
  rc = sdtools_calc_bbox (dd, &xt, &yt, &xb, &yb);
#endif /* OPT_USEOLDUPDATEALGO */

  cw = xb - xt + 1;
  ch = yb - yt + 1;

#ifdef DEBUG
fprintf(stderr, ">bbox, unaligned: rc=%d, x/y: %d/%d - %d/%d, w/h: %d/%d\n", rc, xt, yt, xb, yb, cw, ch);
#endif
  if (rc == 1) {
    int origrot = dd->curr_rotate;  /* store rotation */

    if (dd->depth < 8) { /* aligning */
      switch (serdisp_vssdcp_internal_getStruct(dd)->pixel_dependency) {
        case 1: {
          xt = ((uint16_t)(xt / pbb )) * pbb;
          xb = ((uint16_t)( ( (xb+1 + pbb-1) / pbb )) * pbb ) - 1;
          break;
        }
        case 2: {
          yt = ((uint16_t)(yt / pbb )) * pbb;
          yb = ((uint16_t)((yb + (pbb-1)) / pbb )) * pbb;
          break;
        }
        // else: no aligning required
      }
      cw = xb - xt + 1;
      ch = yb - yt + 1;
    }

#ifdef DEBUG
fprintf(stderr, ">bbox,   aligned: rc=%d, x/y: %d/%d - %d/%d, w/h: %d/%d\n", rc, xt, yt, xb, yb, cw, ch);
#endif
    write_byte(dd, 'D');       /* send image clip: d <x start> <y start> <width> <height> <data ...> */
    buf[0] = (xt & 0xFF00) >> 8;  buf[1] = (xt & 0x00FF);
    buf[2] = (yt & 0xFF00) >> 8;  buf[3] = (yt & 0x00FF);
    buf[4] = (cw & 0xFF00) >> 8;  buf[5] = (cw & 0x00FF);
    buf[6] = (ch & 0xFF00) >> 8;  buf[7] = (ch & 0x00FF);
#ifdef DEBUG
fprintf(stderr, ">update writing\n");
#endif
    rc = write_bytes(dd, buf, 8);

#ifdef DEBUG
fprintf(stderr, ">rc: %d\n", rc);
#endif

/*    sdtools_nsleep(1000); */

    dd->curr_rotate = 0;  /* get current colour via sdtools_generic_getsdpixel(), but unrotated */

    if (dd->depth < 8) {
      switch( serdisp_vssdcp_internal_getStruct(dd)->pixel_dependency ) {
        case 1:
          total = ( (cw + pbb - 1) / pbb ) * ch ;
          break;
        case 2:
          total = cw * ( ( ch + pbb - 1 ) / pbb ) ;
          break;
        default:
          total = ( cw * ch + (pbb - 1) ) / pbb ;
      }

#ifdef DEBUG
        fprintf(stderr, "write_bytes: total=%d\n", total);
#endif

      i = xt;
      j = yt;

      for (curr = 0; curr < total; curr++) {
        buf[pos] = 0;

        switch( serdisp_vssdcp_internal_getStruct(dd)->pixel_dependency ) {
          case 1:
            for (p = 0; p < pbb; p++) {
              if (i < w)
                buf[pos] |= dd->fp_getsdpixel(dd, i, j) << (p * dd->depth);
              i++;
            }
            if (i > xb || i >= w) {
              i = xt;
              j++;
            }
            break;
          case 2:
            for (p = 0; p < pbb; p++) {
              if (j < h)
                buf[pos] |= dd->fp_getsdpixel(dd, i, j) << (p * dd->depth);
              j++;
            }
            if (j > yb || j >= h) {
              j = yt;
              i++;
            }
            break;
          default:
            for (p = 0; p < pbb; p++) {
              buf[pos] |= dd->fp_getsdpixel(dd, i, j) << (p * dd->depth);
              i++;
              if (i > xb || i >= w) {
                i = xt;
                j++;
              }
            }
        }
        if ((! dd->feature_invert) && (dd->curr_invert)) {
           buf[pos] = buf[pos] ^ 0xFF;
        }
        pos++;

        if ( (pos == maxChunk) || (curr == (total - 1)) ) {
#ifdef DEBUG
          fprintf(stderr, "[pos=%d, curr=%d, i/j: %03d/%03d]\t", pos, curr, i, j);
#endif
          write_bytes(dd, buf, pos);
          if ((curr != (total - 1)) && (serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype > 1)) {
            getACK(dd);
          }
          pos = 0;
        }
      }
#ifdef DEBUG
      fprintf(stderr, "\n");
#endif
#ifdef DEBUG
fprintf(stderr, ">update write done: x/y/w/h: %d/%d %dx%d // %d\n", xt, yt, cw, ch, total);
#endif
    } else {
      total = cw * ch * (dd->depth/8);

      for (j = yt; j <= yb; j ++) {
        for (i = xt; i <= xb; i ++) {
          col = dd->fp_getsdpixel(dd, i, j);

          if ((! dd->feature_invert) && (dd->curr_invert)) {
            col = col ^ 0x00FFFFFF;
          }

          if (dd->depth == 24) {
            buf[pos++] = (col & 0xFF0000) >> 16;
          }
          if (dd->depth >= 16) {
            buf[pos++] = (col & 0x00FF00) >> 8;
          }
          buf[pos++] = (col & 0x0000FF);
          curr += (dd->depth/8);

          if ( (pos == maxChunk) || (curr == total) ) {
#ifdef DEBUG
            fprintf(stderr, "write_bytes [i/j=%d/%d]: pos=%d .. ", i, j, pos);
#endif
            rc = write_bytes(dd, buf, pos);
#ifdef DEBUG
            fprintf(stderr, "rc=%d\n", rc);
#endif
            if ((curr != total) && (serdisp_vssdcp_internal_getStruct(dd)->clflag_acktype > 1)) {
#ifdef DEBUG
            fprintf(stderr, "get ack ... ");
#endif
              getACK(dd);
#ifdef DEBUG
            fprintf(stderr, "ok\n");
#endif
            }
            pos = 0;
          }
        }
      }
    }
    dd->curr_rotate = origrot;  /* reset real rotation */
    getACK(dd);

#ifdef DEBUG
    fprintf(stderr, "<update\n");
#endif
    if (! serdisp_vssdcp_internal_getStruct(dd)->feature_immedUpdate) {
      write_byte(dd, 'U');       /* force update */
      getACK(dd);
    }

    memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
  }

}


/* *********************************
   int serdisp_vssdcp_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_vssdcp_setoption(serdisp_t* dd, const char* option, long value) {
  byte buf[3];

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT" )) {
    if (value < 2)
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;

    buf[0] = 'F'; /* set feature value */
    buf[1] = 'i'; /* invert */
    buf[2] = (dd->curr_invert) ? 1 : 0;
    write_bytes(dd, buf, 3);
    getACK(dd);
  } else if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    if (value < 2)
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;

    buf[0] = 'F'; /* set feature value */
    buf[1] = 'b'; /* backlight */
    buf[2] = (dd->curr_backlight) ? 1 : 0;
    write_bytes(dd, buf, 3);
    getACK(dd);
  } else if (serdisp_compareoptionnames(dd, option, "CONTRAST") || serdisp_compareoptionnames(dd, option, "BRIGHTNESS" ) ) {
    int dimmed_contrast;

    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
    } else {
      dd->curr_dimming = 100 - (int)value;
    }

    dimmed_contrast = (((dd->curr_contrast - dd->min_contrast) * (100 - dd->curr_dimming)) / 100) + dd->min_contrast;

    buf[0] = 'F'; /* set feature value */
    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      buf[1] = 'C'; /* contrast */
      buf[2] = dimmed_contrast;
    } else {
      buf[1] = 'B'; /* brightness */
      buf[2] = ((dd->curr_dimming) * serdisp_vssdcp_internal_getStruct(dd)->feature_brightness) / 100;
    }
    write_bytes(dd, buf, 3);
    getACK(dd);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}


/* *********************************
   void serdisp_vssdcp_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_vssdcp_clear(serdisp_t* dd) {
  write_byte (dd, 'C');
  getACK(dd);
  if (! serdisp_vssdcp_internal_getStruct(dd)->feature_immedUpdate) {
    write_byte(dd, 'U');       /* force update */
    getACK(dd);
  }
}


/* *********************************
   void serdisp_vssdcp_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_vssdcp_close(serdisp_t* dd) {
  write_byte (dd, 'C');
  getACK(dd);
  if (! serdisp_vssdcp_internal_getStruct(dd)->feature_immedUpdate) {
    write_byte(dd, 'U');       /* force update */
    getACK(dd);
  }
}

