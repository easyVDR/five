/*
 *************************************************************************
 *
 * l4m320t_tool.c
 * tool for linux4media l4m320t (a.k.a. digital devices dd320t) display module
 *
 *************************************************************************
 *
 * copyright (C) 2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 * contributions:
 * markus bauernfeind, fast-lta:
 * + draft code for uploading firmware
 * linux4media:
 * + C# code as reference for certain operations and calculations
 *
 *************************************************************************
 *
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
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#ifdef HAVE_GD2_GD_H
  #include "gd2/gd.h"
  #include "gd2/gdfontt.h"
  #include "gd2/gdfonts.h"
  #include "gd2/gdfontmb.h"
  #include "gd2/gdfontl.h"
  #include "gd2/gdfontg.h"
#elif defined(HAVE_GD_GD_H)
  #include "gd/gd.h"
  #include "gd/gdfontt.h"
  #include "gd/gdfonts.h"
  #include "gd/gdfontmb.h"
  #include "gd/gdfontl.h"
  #include "gd/gdfontg.h"
#elif defined(HAVE_GD_H)
  #include "gd.h"
  #include "gdfontt.h"
  #include "gdfonts.h"
  #include "gdfontmb.h"
  #include "gdfontl.h"
  #include "gdfontg.h"
#else
  #define LIBGD_SUPPORT_OFF 1
#endif

#if defined(__linux__)
#include <getopt.h>
#endif

#include "common.h"
#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_connect_usb.h"
#include "serdisplib/serdisp_gpevents.h"

/* uint16_t, uint32_t, ... */
#include <inttypes.h>

/* default device */
#define L4M320T_DEFOLDDEVICE         "USB:4243/ee20"
#define L4M320T_DEFAULTDEVICE        "USB:4243/ee21"

/* firmware size */
#define L4M320T_FIRMWARESIZE         16384

/* version information */
#define L4M320TTOOL_VERSION_MAJOR    1
#define L4M320TTOOL_VERSION_MINOR    0

#ifdef HAVE_LIBPTHREAD
static int16_t    raw_x = 0;               /* raw coordinate X */
static int16_t    raw_y = 0;               /* raw coordinate Y */
static byte       event_valid = 0;
#endif

/* usage, help, ...
   iserr: -1 ... print version information only
           0 ... print help/usage to stdout
           1 ... print error message + help/usage to stderr
*/
void print_usage(int iserr) {
  FILE* f = (iserr > 0) ? stderr : stdout;

  fprintf(f, "  l4m320t_tool version %d.%d   (using serdisplib version %d.%d)\n", 
    L4M320TTOOL_VERSION_MAJOR, L4M320TTOOL_VERSION_MINOR,
    SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
    SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
  );
  fprintf(f, "  (C) 2010 by Wolfgang Astleitner\n");

  fprintf(f, "\n");

  if (iserr == -1) return;

  fprintf(f, "Usage: l4m320t_tool [<options>]\n\n");
  fprintf(f, "  Options: (default values in squared brackets)\n");
  fprintf(f, "    -p dev|port      output device or port, default: '%s'\n", L4M320T_DEFAULTDEVICE);
  fprintf(f, "    -i               print firmware version information and serial number\n");
  fprintf(f, "    -B id        [0] show a stored bootlogo; id=[0, 2]\n");
  fprintf(f, "    -c value     [5] contrast / brightness; value=[0, 10]\n");
  fprintf(f, "    -a \"HH:MM\"       set alarm time\n");
  fprintf(f, "    -d \"%%6543210\"    set alarm days (bit field, to be entered as hexadecimal or decimal)\n");
  fprintf(f, "                       bit 6: saturday   bit 5: friday\n");
  fprintf(f, "                       bit 4: thursday   bit 3: wednesday\n");
  fprintf(f, "                       bit 2: tuesday    bit 1: monday\n");
  fprintf(f, "                       bit 0: sunday\n");
  fprintf(f, "                     examples:\n");
  fprintf(f, "                       -d 0x7F: %%1111111 -> all days\n");
  fprintf(f, "                       -d 0x41: %%1000001 -> saturday and sunday\n");
  fprintf(f, "                       -d 0:    de-activate alarm\n");
#ifndef LIBGD_SUPPORT_OFF
  fprintf(f, "    -f imagefile(s)  change up to three bootlogos (240x320, readable by libgd)\n");
#else
  fprintf(f, "    -f imagefile(s)  not supported (tool has been compiled without support for libgd)\n");
#endif
  fprintf(f, "    -u firmwarefile  update firmware (.hex or .bin)\n");
#ifdef HAVE_LIBPTHREAD
  fprintf(f, "    -C               calibrate touchscreen\n");
#else
  fprintf(f, "    -C               calibrating touchscreen is not supported (serdisplib compiled w/o libpthread)\n");
#endif
  fprintf(f, "    -y               enforce certain actions (e.g. overwrite newer firmware version with older one\n");
  fprintf(f, "    -1               display module with firmware version < '90D'\n");
  fprintf(f, "    -v               verbose (-v repeated: <= 2: log to syslog, >= 3: log to stderr, >= 5: log to stdout)\n");
  fprintf(f, "    -V               version information\n");
  fprintf(f, "            \n");
  fprintf(f, "  Examples: \n");
  fprintf(f, "    l4m320t_tool                             # show default (=first) bootlogo\n");
  fprintf(f, "    l4m320t_tool -B 1                        # show second bootlogo\n");
#ifndef LIBGD_SUPPORT_OFF
  fprintf(f, "    l4m320t_tool -f bootlogo.png             # change first bootlogo\n");
  fprintf(f, "    l4m320t_tool -f bl0.png bl1.png bl2.png  # change all three bootlogos\n");
#endif
  fprintf(f, "    l4m320t_tool -a \"23:30\" -d 0x7F        # set alarm at 23:30, all days\n");
  fprintf(f, "            \n");
}


int cmd_readstream (serdisp_CONN_t* sdcd, byte* readbuf, int readcnt, byte* cmdbuf, int cmdcnt) {
  int retry = 0;
  int rc = 0;
  int i;
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);
  int default_read_timeout = usbitems->read_timeout;

  do {  /* until valid stream is read or max. retries exceeded */
    for (i = 0; i < cmdcnt; i++)
      SDCONN_write(sdcd, cmdbuf[i], 0);
    if (cmdcnt < 64)
      SDCONN_write(sdcd, 0, 0); /* add paranoia byte */
    SDCONN_commit(sdcd);

    usbitems->read_timeout >>= 4;  /* temp. decrease timeout */
    rc = SDCONN_readstream(sdcd, readbuf, readcnt);
    usbitems->read_timeout = default_read_timeout;
    /* if (rc < 0) fprintf(stderr, ".");*/
    retry++;
  } while (retry < 10 && (rc < 0 || (readbuf[0] != cmdbuf[0])));

  if (retry >= 10) {
    readbuf[0] = 0;
    rc = -1;
  }
  return rc;
}


int read_flashpages (serdisp_t* dd, byte* fw, int start, int amount) {
  int rc = 0;
  int page, chunk, i, cnt;
  byte buffer[64];
  byte cmdbuf[4];
  int pos = 0;

  for (page = start; page < start+amount; page++) {
    for (chunk = 0; chunk <= 4; chunk++) {
      cmdbuf[0] = 0xB2;
      cmdbuf[1] = (page & 0xFF00) >> 8;
      cmdbuf[2] = (page & 0x00FF);
      cmdbuf[3] = chunk * 60;
      rc = cmd_readstream(dd->sdcd, buffer, 64, cmdbuf, 4);

      if (rc < 0)
        return 0;

      cnt = ( chunk < 4 ) ? 60 : 16;
      for (i=0; i < cnt; i++)
        fw[pos++] = buffer[i+1];
    }
  }
  return 1;
}


int write_flashpages (serdisp_t* dd, byte* fw, int start, int amount) {
  int page, chunk, i, cnt;
  int pos = 0;

  for (page = start; page < start+amount ; page++) {
    for (chunk = 0; chunk <= 4; chunk++) {
      SDCONN_write(dd->sdcd, 0xb3, 0);
      SDCONN_write(dd->sdcd, (page & 0xFF00) >> 8, 0);
      SDCONN_write(dd->sdcd, (page & 0x00FF)     , 0);
      SDCONN_write(dd->sdcd, chunk*60, 0);
      cnt = ( chunk < 4 ) ? 60 : 16;
      for (i = 0; i < cnt; i++)
        SDCONN_write(dd->sdcd, fw[pos++], 0);
      SDCONN_commit(dd->sdcd);
    }
  }
  return 1;
}


uint32_t read_uint32(byte* fw) {
  return ( (fw[0] << 24) | (fw[1] << 16) | (fw[2] <<  8) | fw[3] );
}


uint16_t read_uint16(byte* fw) {
  return ( (fw[0] << 8) | fw[1] );
}


/* first image page == 68, size of each image in pages: 320*240*2 / 256 */
uint16_t get_image_pagestart(int id) {
  return 68 + ((320*240)/(256/2)) * id;
}


uint32_t firmware_version(byte* fw) {
  return read_uint32( &fw[0x39F0] );
/*  return ( (fw[0x39F0] << 24) | (fw[0x39F1] << 16) | (fw[0x39F2] <<  8) | fw[0x39F3] );*/
}

int check_image_entry(byte* fw, int id) {
  int saddr = (id+1)*32;
  /*
  fprintf(stderr, "id: %d, w/h: %d/%d, page: %d, size: %d\n",
          fw[saddr+2]-2, read_uint16(&fw[saddr+3]), read_uint16(&fw[saddr+5]),
          read_uint16(&fw[saddr+10]), read_uint32(&fw[saddr+13]));
  */
  return (
    ((fw[saddr+2]-2) == id) && (read_uint32(&fw[saddr+13]) == (240*320*2) ) &&
    (read_uint16(&fw[saddr+3]) == 240) && (read_uint16(&fw[saddr+5]) == 320)
  );
}



int read_imagedata(byte* imagedata, char* imagefile) {
#ifndef LIBGD_SUPPORT_OFF
  gdImagePtr im_file = 0;
  FILE* fin;
  int orig_w = 0, orig_h = 0;
  int colour;
  int pos = 0;
  byte r, g, b;
  int i, j;

  fin = fopen(imagefile, "rb");
  if (!fin) {
    fprintf(stderr, "image file '%s' cannot be opened\n", imagefile);
    return 0;
  }

  if (strcasecmp(strrchr(imagefile, '.'), ".png") == 0) {
    im_file = gdImageCreateFromPng(fin);
    orig_w = gdImageSX(im_file); orig_h = gdImageSY(im_file);
#if HAVE_GDIMAGECREATEFROMGIF
  } else if (strcasecmp(strrchr(imagefile, '.'), ".gif") == 0) {
    im_file = gdImageCreateFromGif(fin);
    orig_w = gdImageSX(im_file); orig_h = gdImageSY(im_file);
#endif
  } else if (strncasecmp(strrchr(imagefile, '.'), ".jp", 3) == 0) {
    sd_debug(2, "JPEG\n");
    im_file = gdImageCreateFromJpeg(fin);
    orig_w = gdImageSX(im_file); orig_h = gdImageSY(im_file);
  } else {
    fprintf(stderr, "unsupported extension for image file %s.\n", imagefile);
    fclose(fin);
    return 0;
  }
  fclose(fin);
  /*fprintf(stderr, "orig_w/h: %d/%d\n", orig_w, orig_h);*/

  if (orig_w != 240 || orig_h != 320) {
    fprintf(stderr, "wrong format of image %s. image format must be: 240x320\n", imagefile); 
    gdImageDestroy(im_file);
    return 0;
  }

  pos = 0;
  for (j = 0; j < orig_h; j++) {
    for (i = 0; i < orig_w; i++) {
      colour = gdImageGetPixel(im_file, i, j);
      r = (byte)gdImageRed(im_file, colour);
      g = (byte)gdImageGreen(im_file, colour);
      b = (byte)gdImageBlue(im_file, colour);

      /* clip least significant bits from colour channels and shift them to the right places */
      /* BBBB BGGG | GGGR RRRR */
      imagedata[pos++] = (b & 0xF8) | ((g & 0xE0) >> 5);
      imagedata[pos++] = ((g & 0x1C) << 3) | ((r & 0xF8) >> 3);
    }
  }
  gdImageDestroy(im_file);

  return 1;
#else
  return 0;
#endif /* !defined LIBGD_SUPPORT_OFF */
}


void clear_region(serdisp_t* dd, int x, int y, int w, int h) {
  int i,j;
  for (j = y; j < y+h; j++)
    for (i = x; i < x+w; i++)
      serdisp_setcolour(dd, i, j, 0xFF000000);
  serdisp_update(dd);
}


void draw_cross(serdisp_t* dd, int x, int y, int scale) {
  int i,j;
  for (j = -scale; j <= scale; j++) {
    for (i = -scale; i <= scale; i++) {
      if ( (i == j || (-i == j ) ) && (i != 0 && j != 0) )
        serdisp_setcolour(dd, x+i, y+j, 0xFFFFFFFF);
    }
  }
  serdisp_update(dd);
}

#ifdef HAVE_LIBPTHREAD
void eventlistener(serdisp_t* dd, SDGP_event_t* currevent) {
  if (!currevent)
    return;

  if ( currevent->type == SDGPT_SIMPLETOUCH ) {
    SDGP_evpkt_simpletouch_t  simpletouch;
    memcpy(&simpletouch, &currevent->data, sizeof(SDGP_evpkt_simpletouch_t));
    raw_x = simpletouch.raw_x;
    raw_y = simpletouch.raw_y;
    event_valid = 1;
  }
}
#endif


#ifdef HAVE_LIBPTHREAD
int wait_event(serdisp_t* dd, int* rx, int* ry, int timeout) {
  uint32_t timeout_max = timeout * 150000;  /* seconds -> microseconds */
  uint32_t wait_sum = 0;

  event_valid = 0;
  SDEVLP_add_listener(dd, 0, &eventlistener);
  SDGPI_enable(dd, 0, 1);

  while (!event_valid && (wait_sum < timeout_max)) {
    usleep(100); wait_sum += 100;
  }

  if (event_valid) {
    *rx = raw_x;
    *ry = raw_y;
  }
  SDGPI_enable(dd, 0, 0);
  return (event_valid) ? 1 : 0;
}
#endif


int main(int argc, char **argv) {
  int ch;
  char sdcdev[51] = "";

#ifndef LIBGD_SUPPORT_OFF
  char* bootlogofile[3] = {(char*)0, (char*)0, (char*)0};
  int   bootlogofiles_set = 0;
#endif
  char* firmwarefile = (char*)0;
  char  serialnumber[17];
  byte  newfirmware[L4M320T_FIRMWARESIZE];
  byte  currfirmware[L4M320T_FIRMWARESIZE];

  char optionstring[101] = "";

  int flag_alarmtime = 0;
  int flag_alarmdays = 0;
  int flag_setbootlogo = 0;
  int flag_setfirmware = 0;
  int flag_showbootlogo = 0;
  int flag_showinfo = 0;
#ifdef HAVE_LIBPTHREAD
  int flag_calibrate = 0;
#endif
  int flag_yes = 0;
  int flag_oldfirmware = 0;

  int contrast = 5;
  int bootlogo_id = 0;

  unsigned short alarm_days   = 0;
  unsigned char  alarm_hour   = 0;
  unsigned char  alarm_minute = 0;

  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)0;

  int tmp_debuglevel = 0;

  extern char *optarg;
  extern int optind;
  char* optstring =  "1a:B:c:Cd:f:hip:u:vVy";

  serdisp_CONN_t* sdcd;
  serdisp_t* dd = 0;

  sd_setdebuglevel(SD_LVL_WARN);

  while ((ch = getopt(argc, argv, optstring)) != -1) {
    switch(ch) {
      case 'h':
        print_usage(0);
        exit(0);
      case '1':
        flag_oldfirmware = 1;
      break;
      case 'i':
        flag_showinfo = 1;
      break;
      case 'y':
        flag_yes = 1;
      break;
      case 'p':
        sdtools_strncpy(sdcdev, optarg, 50);
      break;
      case 'C':
#ifdef HAVE_LIBPTHREAD
        flag_calibrate = 1;
#else
        fprintf(stderr,
          "Error: calibration is not supported because serdisplib has been compiled without libpthread.\n"
        );
        exit (1);
#endif
      break;
      case 'c':
        {
          char* tempptr;
          int isvalidnumber = 0;

          if (strncasecmp(optarg, "0x", 2) == 0) {
            contrast = (int)strtol(optarg, &tempptr, 16);
          } else {
            contrast = (int)strtol(optarg, &tempptr, 10);
          }

          /* verify if optvalueptr contained a valid number */
          isvalidnumber = ( (optarg == tempptr) || ( (*tempptr != '\0')  ) ) ? 0 : 1;

          if (!isvalidnumber || contrast < 0 || contrast > 10) {
            fprintf(stderr, "Error: Invalid value for option -c: %s\n", optarg);
            exit (1);
          }
        }
      break;
      case 'B':
        {
          char* tempptr;
          int isvalidnumber = 0;

          flag_showbootlogo = 1;
          bootlogo_id = (int)strtol(optarg, &tempptr, 10);

          /* verify if optvalueptr contained a valid number */
          isvalidnumber = ( (optarg == tempptr) || ( (*tempptr != '\0')  ) ) ? 0 : 1;

          if (!isvalidnumber || bootlogo_id < 0 || bootlogo_id > 2) {
            fprintf(stderr, "Error: Invalid value for option -B: %s\n", optarg);
            exit (1);
          }
        }
      break;
      case 'f':
        {
#ifndef LIBGD_SUPPORT_OFF
          int rc;
          int opterr = 0;

          bootlogofile[bootlogofiles_set] = (char*)sdtools_malloc(strlen(optarg)+2);
          sdtools_strncpy(bootlogofile[bootlogofiles_set++], optarg, strlen(optarg));
          flag_setbootlogo = 1;

          while ( optind < argc && argv[optind][0] != '-' ) {
            if (bootlogofiles_set < 3) {
              bootlogofile[bootlogofiles_set] = (char*)sdtools_malloc(strlen(argv[optind])+2);
              sdtools_strncpy(bootlogofile[bootlogofiles_set++], argv[optind], strlen(argv[optind]));
            } else {
              opterr = 1;
            }
            optind++;
          }

          for (rc = 0; rc < bootlogofiles_set; rc++)
            fprintf(stderr, "boot logo %d: '%s'\n", rc, bootlogofile[rc]);

          if (opterr) {
            int i;
            fprintf(stderr, "Error: more than 3 bootlogo files given.\n");
            for (i = 0; i < bootlogofiles_set; i++)
              free(bootlogofile[i]);
            exit (1);
          }
#else
          fprintf(stderr,
          "Error: changing of bootlogo images is not supported because l4m320t_tool has been compiled without libgd.\n"
          );
          exit (1);
#endif /* !defined LIBGD_SUPPORT_OFF */
        }
      break;
      case 'u':
        {
          firmwarefile = (char*)sdtools_malloc(strlen(optarg)+2);
          sdtools_strncpy(firmwarefile, optarg, strlen(optarg));
          flag_setfirmware = 1;
        }
      break;
      case 'a':
        {
          char* tempptr;
          long value;
          int isvalidnumber;

          flag_alarmtime = 1;

          value = strtol(optarg, &tempptr, 10);

          /* verify if optvalueptr contained a valid number */
          isvalidnumber = /*( (optarg == tempptr) || (*/ (*tempptr != ':') /*) ) */ ? 0 : 1;

          if (isvalidnumber) {
            char* optarg_shift = tempptr+1;

            alarm_hour = (unsigned char) value;

            value = strtol(optarg_shift, &tempptr, 10);

            /* verify if optvalueptr contained a valid number */
            isvalidnumber = ( (optarg_shift == tempptr) || ( (*tempptr != '\0')) ) ? 0 : 1;
            if (isvalidnumber) {
              alarm_minute = (unsigned char) value;
            }
          }

          if (!isvalidnumber) {
            fprintf(stderr, "Error: Invalid value for option -a: %s\n", optarg);
            exit (1);
          }
        }
      break;
      case 'd':
        {
          char* tempptr;
          long value;
          int isvalidnumber;

          flag_alarmdays = 1;

          if ( (strlen(optarg) == 3) && ( ( strncasecmp(optarg, "off", 3) == 0) || (strncasecmp(optarg, "all", 3) == 0) ) ) {
            value = ( strncasecmp(optarg, "off", 3) == 0) ? 0 : 0x7F;
          } else {
            /* accept base 10 and base 16 values (base 16 if value starts with 0x or 0X) */
            if (strncasecmp(optarg, "0x", 2) == 0) {
              value = strtol(optarg, &tempptr, 16);
            } else {
              value = strtol(optarg, &tempptr, 10);
            }

            /* verify if optvalueptr contained a valid number */
            isvalidnumber = ( (optarg == tempptr) || ( (*tempptr != '\0')  ) ) ? 0 : 1;

            if (!isvalidnumber) {
              fprintf(stderr, "Error: Invalid value for option -d: %s\n", optarg);
              exit (1);
            }
          }

          alarm_days = (unsigned short) value;

          /* if -d 0  (deactive alarm): no -a required */
          if (alarm_days == 0) {
            flag_alarmtime = 1;
          }
        }
      break;
      case 'v':
        tmp_debuglevel++;
      break;
      case 'V':
        print_usage(-1);
        exit(0);
      break;
    }
  }


  if (tmp_debuglevel) {
    if (tmp_debuglevel > 6) tmp_debuglevel = 6;
    sd_setlogmedium( (tmp_debuglevel <= 2) ? SD_LOG_SYSLOG : ((tmp_debuglevel <= 4) ? SD_LOG_STDERR : SD_LOG_STDOUT));
    if (tmp_debuglevel > 2) sd_setdebuglevel( (tmp_debuglevel % 2) ? 1 : 2);
    else sd_setdebuglevel(tmp_debuglevel);
  }

  if (strlen(sdcdev) == 0) {
    sdtools_strncpy(sdcdev, (flag_oldfirmware) ? L4M320T_DEFOLDDEVICE : L4M320T_DEFAULTDEVICE, 50);
  }

  if (flag_setbootlogo && (flag_alarmtime || flag_alarmdays)) {
    fprintf(stderr, "Error: Changing boot logo (-f) and setting alarm (-a, -d) may not be used altogether\n");
    exit (1);
  }

  /* if no flag given: show bootlogo */
  if (! (flag_setbootlogo || flag_alarmtime || flag_alarmdays) ) {
    flag_showbootlogo = 1;
  }

  if (flag_alarmdays ^ flag_alarmtime) {
    fprintf(stderr, "Error: Both -a and -d are required for setting alarm\n");
    exit (1);
  }

  if (alarm_hour > 23 || alarm_minute > 59 || alarm_days > 0x7f) {
    fprintf(stderr, "Error: Alarm time or bit field for alarm days out of bounds\n");
    exit (1);
  }

  sdcd = SDCONN_open(sdcdev);

  if (sdcd == (serdisp_CONN_t*)0) {
    fprintf(stderr, "Error: Unable to open %s, additional info: %s\n", sdcdev, sd_geterrormsg());
    exit (1);
  }

  if (flag_alarmtime) {
    snprintf(optionstring, 99, "ALARMHOUR=%02d;ALARMMINUTE=%02d;ALARMDAYS=0x%02x;CONTRAST=%d",
                               alarm_hour, alarm_minute, alarm_days,contrast);
    sd_debug(1, "optionstring: %s", optionstring);
  } else if (flag_setbootlogo || flag_showbootlogo) {
    snprintf(optionstring, 99, /*"RESMODE=1"*/ "CONTRAST=%d;ROTATE=0", contrast);
    sd_debug(1, "optionstring: %s", optionstring);
  }


  dd = serdisp_init(sdcd, "L4M320T", optionstring);

  if (!dd) {
    fprintf(stderr, "Error: Unable to open L4M320T, additional info: %s\n", sd_geterrormsg());
    exit(1);
  }

  usbitems = (serdisp_usbdev_t*)(sdcd->extra);

  /* sometimes the first read fails -> dummy read -> subsequent read commands should work reliabely then */
  {
    int default_in_ep = usbitems->in_ep;
    int default_read_timeout = usbitems->read_timeout;
    byte buffer[64];

    usbitems->in_ep=0x83;
    usbitems->read_timeout = 1;
    SDCONN_write(dd->sdcd, 0x32, 0);
    SDCONN_write(dd->sdcd,    0, 0);
    SDCONN_commit(dd->sdcd);
    SDCONN_readstream(dd->sdcd, buffer, 64);
    usbitems->read_timeout = default_read_timeout;
    usbitems->in_ep = default_in_ep;
  }

  if (flag_showinfo) {
    int default_in_ep = usbitems->in_ep;
    byte buffer[64];
    byte cmdbuf[1];
    int rc = 0;
    uint32_t fwversion1 = 0; /* firmware version taken from firmware data */
    uint32_t fwversion2 = 0; /* firmware version received from command 0x35 */

    usbitems->in_ep=0x83;

    cmdbuf[0] = 0x32;

    rc = cmd_readstream(dd->sdcd, buffer, 64 /*1 + 32*/, cmdbuf, 1);

    if (buffer[0] != 0x32) {
      fprintf(stderr, "Error: Reading serial number failed\n");
      serdisp_quit(dd);
      exit(1);
    }

    if ( buffer[1] == 0x20 ) {
      printf("serial number not set (will be auto-generated when updating firmware)\n");
    } else {
      for (rc = 0; rc < 16; rc ++)
        serialnumber[rc]=buffer[(rc<<1)+1];
      serialnumber[16]='\0';

      printf("serial number: %s\n", serialnumber);
    }

    /* firmware is stored in pages 4 to 67 */
    rc = read_flashpages(dd, currfirmware, 4, 64);

    if (rc) {
      fwversion1 = firmware_version(currfirmware);
    } else {
      fprintf(stderr, "error when reading firmware version\n");
    }

    cmdbuf[0] = 0x35;
    rc = cmd_readstream(dd->sdcd, buffer, 64 /*1 + 4*/, cmdbuf, 1);
    fwversion2 = read_uint32(&buffer[1]);

    if (rc && (fwversion1 == fwversion2)) {
      printf("firmware version: %08x\n", fwversion1);
    } else if (rc && (fwversion1 != fwversion2)) {
      fprintf(stderr, "mismatching firmware version information: %08x != %08x\n", fwversion1, fwversion2);
    } else {
      fprintf(stderr, "error when reading firmware version\n");
    }

    cmdbuf[0] = 0x4E;
    rc = cmd_readstream(dd->sdcd, buffer, 64 /*1 + 8*/, cmdbuf, 1);

    if (rc > 0 && (buffer[0] == 0x4E)) {
      int xo = (buffer[1] << 8) | buffer[2];
      int xs = (buffer[3] << 8) | buffer[4];
      int yo = (buffer[5] << 8) | buffer[6];
      int ys = (buffer[7] << 8) | buffer[8];
      printf("calibration coordinates: offset x/y: %d/%d,  scale x/y: %d/%d\n", xo, yo, xs, ys);
    } else {
      fprintf(stderr, "error when reading calibration coordinates\n");
    }

    /* restore default in_ep */
    usbitems->in_ep = default_in_ep;
  }

  if (flag_setbootlogo) {
#ifndef LIBGD_SUPPORT_OFF
    byte directory[1024];
    int i;
    int default_in_ep = usbitems->in_ep;
    int flok = 1;
    byte* imagedata[3] = {(byte*)0, (byte*)0, (byte*)0};

    /* read boot logo images */
    i = 0;
    while (flok && i <= 2) {
      if (bootlogofile[i]) {
        fprintf(stderr, "reading image %d ... ", i);
        imagedata[i] = (byte*)sdtools_malloc(240*320*2);
        if (imagedata[i]) {
          flok = read_imagedata(imagedata[i], bootlogofile[i]);
        } else {
          flok = 0;
        }
        if (flok) fprintf(stderr, "OK\n");
        if (flok == 0 && imagedata[i]) {
          free(imagedata[i]);
          imagedata[i] = (byte*)0;
        }
      }
      i++;
    }

    if (!flok) {
      fprintf(stderr, "error when reading bootlogo image.\n");
    } else {
      /* if all boot logo files are given: re-init whole directory */
      if (bootlogofiles_set == 3) {
        memset(directory, 0, 1024);
        for (i = 0; i < 1024; i += 32) {
          directory[i] = 0x55;
          directory[i + 31] = 0xFF-0x55;
        }
        for (i = 0; i < 3; i++) {
          int s;
          uint16_t w = 240;
          uint16_t h = 320;

          uint32_t size = w*h*2;

          uint16_t le = (240 - w) / 2;
          uint16_t ri = le + w - 1;
          uint16_t to = (320 - h) / 2;
          uint16_t bo = to + h - 1;

          uint16_t page = get_image_pagestart(i);

          uint16_t csum = 0;

          /* w/h of boot image */
          directory[ (i+1)*32 +  2 ] = 0x02 + i;
          directory[ (i+1)*32 +  3 ] = (w & 0xFF00) >> 8;
          directory[ (i+1)*32 +  4 ] = (w & 0x00FF);
          directory[ (i+1)*32 +  5 ] = (h & 0xFF00) >> 8;
          directory[ (i+1)*32 +  6 ] = (h & 0x00FF);
          /* flash address */
          directory[ (i+1)*32 +  9 ] = 0;
          directory[ (i+1)*32 + 10 ] = (page & 0xFF00) >> 8;
          directory[ (i+1)*32 + 11 ] = (page & 0x00FF);
          directory[ (i+1)*32 + 12 ] = 0;
          /* image size in byte */
          directory[ (i+1)*32 + 13 ] = (size & 0x00FF0000) >> 24;
          directory[ (i+1)*32 + 14 ] = (size & 0x00FF0000) >> 16;
          directory[ (i+1)*32 + 15 ] = (size & 0x0000FF00) >>  8;
          directory[ (i+1)*32 + 16 ] = (size & 0x000000FF);
          /* window 0/0 - 239/319 */
          directory[ (i+1)*32 + 17 ] = (le & 0xFF00) >> 8;
          directory[ (i+1)*32 + 18 ] = (le & 0x00FF);
          directory[ (i+1)*32 + 19 ] = (ri & 0xFF00) >> 8;
          directory[ (i+1)*32 + 20 ] = (ri & 0x00FF);
          directory[ (i+1)*32 + 21 ] = (to & 0xFF00) >> 8;
          directory[ (i+1)*32 + 22 ] = (to & 0x00FF);
          directory[ (i+1)*32 + 23 ] = (bo & 0xFF00) >> 8;
          directory[ (i+1)*32 + 24 ] = (bo & 0x00FF);

          for (s = 0; s < 30; s++) {
            csum += directory[ (i+1)*32 + s ];
            if (csum >= 256) csum -= 255;
          }
          directory[ (i+1)*32 + 31 ] = (0xFF - (byte)csum);
        }
        fprintf(stderr, "writing directory to flash ... ");
        usbitems->in_ep = 0x83;
        flok = write_flashpages(dd, directory, 0, 4);
        usbitems->in_ep = default_in_ep;
        fprintf(stderr, "%s\n", ((flok) ? "OK" : "error"));
      } else {  /* only 1 to 2 boot logo images given: check dir entries and re-use if valid */
        int rc;

        fprintf(stderr, "reading directory from module ... ");
        usbitems->in_ep = 0x83;
        rc = read_flashpages(dd, directory, 0, 4);
        usbitems->in_ep = default_in_ep;
        fprintf(stderr, "done\n");

        fprintf(stderr, "checking empty entries ... ");
        i = 0;  /* dir. entries 1-3 contain boot images ( i = [32 - 128[ ) */
        while (flok && i < 1024) {
          flok = ( (i >= 32 && i < 128) || ((directory[i] == 0x55) && (directory[i+31] == (0xFF - 0x55))) );
          i += 32;
        }
        fprintf(stderr, "%s\n", ((flok) ? "OK" : "invalid"));

        i = 0;
        while (flok && i <= 2) {
          fprintf(stderr, "checking directory entry for image %d ... ", i);
          flok = check_image_entry(directory, i);
          fprintf(stderr, "%s\n", ((flok) ? "OK" : "invalid"));
          i++;
        }
      }
    }

    /* write images to flash */
    if (flok) {
      int pages = (320*240)/(256/2);

      usbitems->in_ep = 0x83;
      i = 0;
      while (flok && i <= 2) {
        if (imagedata[i]) {
          fprintf(stderr, "writing bootlogo image %d to flash ... ", i);
          flok = write_flashpages(dd, imagedata[i], get_image_pagestart(i), pages);
          fprintf(stderr, "%s\n", ((flok) ? "OK" : "error"));
        }
        i++;
      }
      usbitems->in_ep = default_in_ep;
      if (!flok) {
        fprintf(stderr, "error when writing bootlogo image(s).\n");
      }
    }

    /* free image data */
    for (i = 0; i < 3; i++) {
      if ( imagedata[i] ) {
        free (imagedata[i]);
        imagedata[i] = (byte*) 0;
      }
    }
#endif /* !defined LIBGD_SUPPORT_OFF */
  }

#ifdef HAVE_LIBPTHREAD
  if (flag_calibrate) {
    int err = 0;
    int x1, y1, x2, y2, x3, y3;
    int xo = 0, yo = 0, xs = 1300, ys = 1300;
    double ux1, ux2, uy1, uy2;
    byte buffer[64];
    byte cmdbuf[1];
    int rc = 0;
    int default_in_ep = usbitems->in_ep;

    int drawoff = 10;
    int drawrad = drawoff - 0;

    fprintf(stderr, "resetting calibration ... ");
    SDCONN_write(dd->sdcd, 0x4f, 0);
    SDCONN_write(dd->sdcd, (xo & 0xFF00) >> 8, 0);
    SDCONN_write(dd->sdcd, (xo & 0x00FF)     , 0);
    SDCONN_write(dd->sdcd, (xs & 0xFF00) >> 8, 0);
    SDCONN_write(dd->sdcd, (xs & 0x00FF)     , 0);
    SDCONN_write(dd->sdcd, (yo & 0xFF00) >> 8, 0);
    SDCONN_write(dd->sdcd, (yo & 0x00FF)     , 0);
    SDCONN_write(dd->sdcd, (ys & 0xFF00) >> 8, 0);
    SDCONN_write(dd->sdcd, (ys & 0x00FF)     , 0);
    SDCONN_commit(dd->sdcd);
    fprintf(stderr, "done\n");

    memset (dd->scrbuf_chg, 0xFF, dd->scrbuf_chg_size);
    clear_region(dd, 0, 0, 239, 319);

    /* internally read 2nd cross before 1st one (looks better when clearing/redrawing the screen) */
    fprintf(stderr, ">>> tap on first cross ... ");
    draw_cross(dd, 239-drawoff, drawoff, drawrad);
    if (wait_event(dd, &x2, &y2, 10)) {
      fprintf(stderr, "raw x/y: %d/%d\n", x2, y2);
    } else {
      err = 1;
    }
    if (!err) {
      fprintf(stderr, ">>> tap on second cross ... ");
      clear_region(dd, 239-drawoff-drawrad-1, drawoff-drawrad-1, drawrad*2+2, drawrad*2+2);
      sleep(1);
      draw_cross(dd, drawoff, 319-drawoff, drawrad);
      if (wait_event(dd, &x1, &y1, 10)) {
        fprintf(stderr, "raw x/y: %d/%d\n", x1, y1);
      } else {
        err = 1;
      }
    }

    if (err) {
      fprintf(stderr, "timeout reached!\nplease restart calibration\n");
    } else {
      fprintf(stderr, "reading current internal calibration ... ");
      usbitems->in_ep=0x83;
      cmdbuf[0] = 0x4E;
      rc = cmd_readstream(dd->sdcd, buffer, 64 /*1 + 8*/, cmdbuf, 1);
      usbitems->in_ep=default_in_ep;

      if (buffer[0] == 0x4e) {
        xo = (buffer[1] << 8) | buffer[2];
        xs = (buffer[3] << 8) | buffer[4];
        yo = (buffer[5] << 8) | buffer[6];
        ys = (buffer[7] << 8) | buffer[8];
        fprintf(stderr, "done\n");
      } else {
        fprintf(stderr, "error when reading internal calibration (0x4e != 0x%02x, bytes read: %d)\n", buffer[0], rc);
      }

      /* calculations taken from linux4media source */
      ux1 = (double)xo + (double)(x1 * xs) / 240.0;
      ux2 = (double)xo + (double)(x2 * xs) / 240.0;
      uy1 = (double)yo + ((double)(319-y1) * (double)ys) / 320.0;
      uy2 = (double)yo + ((double)(319-y2) * (double)ys) / 320.0;

      /* calculate new xs/ys and xo/yo */
      /* 20/20 == center position of cross (rel, to border) */
      xs = (int)((ux1 - ux2) * 240.0 / ((double)drawoff - (239.0-(double)drawoff)) + 0.5);
      ys = (int)((uy1 - uy2) * 320.0 / ((double)drawoff - (319.0-(double)drawoff)) + 0.5);

      xo = (int)(ux2 - ( (239.0-(double)drawoff) * (double)xs) / 240.0 + 0.5);
      yo = (int)(uy2 - ( (319.0-(double)drawoff) * (double)ys) / 320.0 + 0.5);

      fprintf(stderr, "writing new calibration ... ");
      SDCONN_write(dd->sdcd, 0x4f, 0);
      SDCONN_write(dd->sdcd, (xo & 0xFF00) >> 8, 0);
      SDCONN_write(dd->sdcd, (xo & 0x00FF)     , 0);
      SDCONN_write(dd->sdcd, (xs & 0xFF00) >> 8, 0);
      SDCONN_write(dd->sdcd, (xs & 0x00FF)     , 0);
      SDCONN_write(dd->sdcd, (yo & 0xFF00) >> 8, 0);
      SDCONN_write(dd->sdcd, (yo & 0x00FF)     , 0);
      SDCONN_write(dd->sdcd, (ys & 0xFF00) >> 8, 0);
      SDCONN_write(dd->sdcd, (ys & 0x00FF)     , 0);
      SDCONN_commit(dd->sdcd);
      fprintf(stderr, "done\nnew calibration: offset x/y: %d/%d, scale x/y: %d/%d\n", xo, yo, xs, ys);
    }

    if (!err) {
      fprintf(stderr, ">>> tap on center cross (for verification) ... ");
      clear_region(dd, drawoff-drawrad-1, 319-drawoff-drawrad-1, drawrad*2+2, drawrad*2+2);
      sleep(1);
      draw_cross(dd, 119, 159, drawrad);
      if (wait_event(dd, &x3, &y3, 10)) {
        fprintf(stderr, "x/y: %d/%d, diff to center: %d/%d\n", x3, y3, (x3-119), (y3-159));
      } else {
        err = 1;
      }
      clear_region(dd, 119-drawrad-1, 159-drawrad-1, drawrad*2+2, drawrad*2+2);
      sleep(2);
    }
    if (err) {
      fprintf(stderr, "timeout reached\n");
    }
  }
#endif

  if (flag_setfirmware) {
    int default_in_ep = usbitems->in_ep;
    byte buffer[64];
    byte cmdbuf[1];
    int rc = 0;
    int page, i;
    int pos = 0;
    int fw_hex = 1;

    usbitems->in_ep=0x83;

    if( strlen(firmwarefile) <= 4) {
      fprintf(stderr, "invalid firmware filename\n");
      exit(-1);
    }

    if (strncasecmp(".hex", strrchr(firmwarefile, '.'), 4) == 0 ) {
      fw_hex = 1;
    } else if (strncasecmp(".bin", strrchr(firmwarefile, '.'), 4) == 0 ) {
      if (flag_yes) {
        fw_hex = 0;
      } else {
        fprintf(stderr, "binary firmware files cannot be checked for data and checksum errors.\n");
        fprintf(stderr, "add -y to force using .bin files\n");
        exit(-1);
      }
    } else {
      fprintf(stderr, "unknown file-ending for firmware filename\n");
      exit(-1);
    }

    /* read current firmware from module */
    rc = read_flashpages(dd, currfirmware, 4, 64);

    if (!rc) {
      fprintf(stderr, "error when reading current firmware version\n");
      exit(-1);
    }

    /* read new firwmare from file */
    if (fw_hex)
      rc = read_firmware_intelhex (firmwarefile, newfirmware, L4M320T_FIRMWARESIZE);
    else
      rc = read_firmware_binary   (firmwarefile, newfirmware, L4M320T_FIRMWARESIZE);

    if (rc) {
      fprintf(stderr, "error reading firmware file: %s\n", sd_geterrormsg());
      exit(-1);
    }

    if ( !flag_yes && (firmware_version(currfirmware) >= firmware_version(newfirmware)) ) {
      fprintf(stderr, "current firmware version newer or equal to version of firmware file: %08x >= %08x\n",
                      firmware_version(currfirmware), firmware_version(newfirmware)
      );
      fprintf(stderr, "use -y to force update\n");
      exit(-1);
    }

    fprintf(stderr, "old firmware version: %08x, new firmware version: %08x\n",
                    firmware_version(currfirmware), firmware_version(newfirmware)
    );

    cmdbuf[0] = 0x32;
    rc = cmd_readstream(dd->sdcd, buffer, 64 /*1 + 32*/, cmdbuf, 1);

    if (buffer[0] != 0x32) {
      fprintf(stderr, "Error: Reading serial number failed\n");
      serdisp_quit(dd);
      exit(1);
    }

    if (buffer[1]  != 0x20 ) {
      for (rc = 0; rc < 16; rc ++)
        serialnumber[rc]=buffer[(rc<<1)+1];
      serialnumber[16] = '\0';

      fprintf(stderr, "serial number: %s\n", serialnumber);
    } else {
      FILE* fp_rand = NULL;

      fprintf(stderr, "serial number was not set, generating new (random) one: ");

      serialnumber[0] = 0x20; /* start with empty (invalidated) serial number */

      fp_rand = fopen("/dev/random", "r");
      if (fp_rand) {
        int i, j;
        byte rand80[10];  /* 80 bit random number */
        char b32[] = "0ABCDEFGHIJKLMNOPQRSTUVWXYZ12345";  /* permitted character set */

        /* read 10 bytes (= 80 bit) from /dev/random */
        fread(rand80, 1, sizeof(rand80), fp_rand);
        fclose(fp_rand);

        /* generation of serial number taken from linux4media sources */
        /* copied the calculations from their code w/o trying or wanting to fully understand these ;-) */
        serialnumber[0] = 'R';
        i = 0;
        j = 1;
        while (j < 17) {
          serialnumber[j++] = b32[ (rand80[i] >> 3) & 0x1F ];
          serialnumber[j++] = b32[ ((rand80[i] << 2) & 0x1C) | ((rand80[i + 1] >> 6) & 0x03) ];
          i++;
          serialnumber[j++] = b32[ ((rand80[i] >> 1) & 0x1F) ];
          serialnumber[j++] = b32[ ((rand80[i] << 4) & 0x10) | (( rand80[i + 1] >> 4) & 0x0F) ];
          i++;
          serialnumber[j++] = b32[ ((rand80[i] << 1) & 0x1E) | (( rand80[i + 1] >> 7) & 0x01) ];
          i++;
          serialnumber[j++] = b32[ ((rand80[i] >> 2) & 0x1F)];
          serialnumber[j++] = b32[ ((rand80[i] << 3) & 0x18) | (( rand80[i + 1] >> 5) & 0x07) ];
          i++;
          serialnumber[j++] = b32[ ((rand80[i]) & 0x1F) ];
          i++;
        }
        serialnumber[16] = '\0';
        fprintf(stderr, "%s\n", serialnumber);
      } else {
        fprintf(stderr, "error when opening /dev/random, continuing w/o applying serial number\n");
      }
    }

    if (serialnumber[0] != 0x20) {
      fprintf(stderr, "applying serial number to new firmware ... ");
      pos = (newfirmware[0x39FE] << 8) | newfirmware[0x39FF];

      if (pos > 0x100 && pos < 0x3000) {
        for (i = 0; i < strlen(serialnumber); i++)
          newfirmware[pos + i * 2] = serialnumber[i];
        fprintf(stderr, "OK\n");
      } else {
        fprintf(stderr, "patch position invalid, continuing w/o serial number\n");
      }
    }

    fprintf(stderr, "erasing firmware flash pages ... ");
    for (page = 4; page < 68; page++) {
      SDCONN_write(dd->sdcd, 0xb1, 0); /* clear page command */
      SDCONN_write(dd->sdcd, 0x00, 0);
      SDCONN_write(dd->sdcd, page, 0);
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_commit(dd->sdcd);
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "writing new firmware into flash ... ");
    /* write new firmware into flash */
    rc = write_flashpages(dd, newfirmware, 4, 64);
    fprintf(stderr, "%s\n", ((rc) ? "OK" : "invalid"));

    fprintf(stderr, "verifying ... ");
    /* compare */
    rc = read_flashpages(dd, currfirmware, 4, 64);

    if (!rc) {
      fprintf(stderr, "error when reading firmware\n");
      exit(-1);
    }

    i = 0;
    while (i < L4M320T_FIRMWARESIZE) {
      if (currfirmware[i] != newfirmware[i]) {
        fprintf(stderr, "verification error at position %d (expected: %02x, got: %02x)\n",
                        i, newfirmware[i], currfirmware[i]);
        exit(-1);
      }
      i++;
    }
    fprintf(stderr, "OK\n");


    fprintf(stderr, "starting flash process, display will reset itself after some seconds ...\n");
    /* start the flash process */
    SDCONN_write(dd->sdcd, 0x10000 | 0x34, 0);

    /* restore default in_ep */
    usbitems->in_ep = default_in_ep;

    exit(0);
  }


  if (flag_setbootlogo || flag_showbootlogo) {
    SDCONN_write(dd->sdcd, 0x03, 0);
    SDCONN_write(dd->sdcd, bootlogo_id, 0);
    SDCONN_commit(dd->sdcd);
    serdisp_close(dd);
  } else {
    serdisp_quit(dd);
  }
  return 0;
}

