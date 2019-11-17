/*
 *************************************************************************
 *
 * touchscreen_tool.c
 * tool for touchscreen devices
 *
 *************************************************************************
 *
 * copyright (C) 2013-2014  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
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

#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_gpevents.h"

/* uint16_t, uint32_t, ... */
#include <inttypes.h>

/* version information */
#define TOUCHSCREENTOOL_VERSION_MAJOR    1
#define TOUCHSCREENTOOL_VERSION_MINOR    0

static int16_t    norm_x = 0;              /* coordinate X */
static int16_t    norm_y = 0;              /* coordinate Y */
static int32_t    raw_x = 0;               /* raw coordinate X */
static int32_t    raw_y = 0;               /* raw coordinate Y */
static byte       event_valid = 0;


/* usage, help, ...
   iserr: -1 ... print version information only
           0 ... print help/usage to stdout
           1 ... print error message + help/usage to stderr
*/
void print_usage(int iserr) {
  FILE* f = (iserr > 0) ? stderr : stdout;

  fprintf(f, "  touchscreen_tool version %d.%d   (using serdisplib version %d.%d)\n", 
    TOUCHSCREENTOOL_VERSION_MAJOR, TOUCHSCREENTOOL_VERSION_MINOR,
    SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
    SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
  );
  fprintf(f, "  (C) 2013-2014 by Wolfgang Astleitner\n");

  fprintf(f, "\n");

  if (iserr == -1) return;

  fprintf(f, "Usage: touchscreen_tool [<options>]\n\n");
  fprintf(f, "  Options:\n");
  fprintf(f, "    -n name          display name\n");
  fprintf(f, "    -p dev|port      output device or port\n");
  fprintf(f, "    -o options       options for driver, semicolon-separated key-value pairs\n");
  fprintf(f, "                     eg: -o \"TOUCHDEVICE=/dev/input/touchscreen;TOUCHSWAPY=1\"\n");
  fprintf(f, "    -i               print information about touchscreen and display\n");
  fprintf(f, "    -C               calculate calibration settings\n");
  fprintf(f, "    -v               verbose (-v repeated: <= 2: log to syslog, >= 3: log to stderr, >= 5: log to stdout)\n");
  fprintf(f, "    -V               version information\n");
  fprintf(f, "            \n");
  fprintf(f, "  Examples: \n");
  fprintf(f, "    touchscreen_tool                         # print information about touchscreen and display\n");
  fprintf(f, "    touchscreen_tool -C                      # calculate calibration settings\n");
  fprintf(f, "            \n");
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

/* draw a simple E for error */
void draw_error(serdisp_t* dd) {
  int w = serdisp_getwidth(dd);
  int h = serdisp_getheight(dd);

  int i,j;

  serdisp_clear(dd);
  for (j = h / 10; j <= h - (h / 10); j++) {
    serdisp_setcolour(dd, w / 3, j, 0xFFFF5555);
    serdisp_setcolour(dd, w / 3 + 1, j, 0xFFFF5555);

    if (
         j == (h / 10) || j == ((h / 10) + 1) ||
         j == (h/2 - 1) || j == (h / 2) ||
         j == (h - h/10 - 1) || j == (h - h/10)
    ) {
      for (i = w / 3 + 2; i < (w / 3) * 2; i++) {
        serdisp_setcolour(dd, i, j, 0xFFFF5555);
      }
    }
  }
  serdisp_update(dd);
}

void eventlistener(serdisp_t* dd, SDGP_event_t* currevent) {
  if (!currevent)
    return;

  if ( currevent->type == SDGPT_GENERICTOUCH) {
    SDGP_evpkt_generictouch_t generictouch;

    memcpy(&generictouch, &currevent->data, sizeof(SDGP_evpkt_generictouch_t));
    if ((generictouch.flags & 0x30) == 0x10) {
      norm_x = generictouch.norm_x;
      norm_y = generictouch.norm_y;
      raw_x = generictouch.raw_x;
      raw_y = generictouch.raw_y;
      event_valid = 1;
    }
  }
}


int wait_event(serdisp_t* dd, int16_t* nx, int16_t* ny, int32_t* rx, int32_t* ry, int timeout) {
  uint32_t timeout_max = timeout * 150000;  /* seconds -> microseconds */
  uint32_t wait_sum = 0;

  event_valid = 0;
  SDEVLP_add_listener(dd, 0, &eventlistener);
  SDGPI_enable(dd, 0, 1);

  while (!event_valid && (wait_sum < timeout_max)) {
    usleep(100); wait_sum += 100;
  }

  if (event_valid) {
    *nx = norm_x;
    *ny = norm_y;
    *rx = raw_x;
    *ry = raw_y;
  }
  SDGPI_enable(dd, 0, 0);
  return (event_valid) ? 1 : 0;
}


int main(int argc, char **argv) {
  int ch;
  char sdcdev[51] = "";
  char dispname[51] = "";

  char* optionstring = 0;

  int flag_showinfo = 0;
  int flag_calibrate = 0;

  int tmp_debuglevel = 0;

  int err = 0;

  extern char *optarg;
  extern int optind;
  char* optstring =  "n:hip:o:CvV";

  serdisp_CONN_t* sdcd;
  serdisp_t* dd = 0;

  sd_setdebuglevel(SD_LVL_WARN);

  while ((ch = getopt(argc, argv, optstring)) != -1) {
    switch(ch) {
      case 'h':
        print_usage(0);
        exit(0);
      case 'i':
        flag_showinfo = 1;
      break;
      case 'n':
        sdtools_strncpy(dispname, optarg, 50);
      break;
      case 'p':
        sdtools_strncpy(sdcdev, optarg, 50);
      break;
      case 'C':
        flag_calibrate = 1;
      break;
      case 'o':
        if (optionstring) free(optionstring);
        optionstring = (char*) sdtools_malloc(strlen(optarg)+1);
        sdtools_strncpy(optionstring, optarg, strlen(optarg));
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

  if (strlen(dispname) < 1) {
    fprintf(stderr, "required option -n <display name> missing ...\n\n");
    print_usage(1);
    err = 1;
  }

  if (err) {
    if (optionstring) free(optionstring);
    exit(1);
  }

  /* if no flag given: show info */
  if (! (flag_calibrate) ) {
    flag_showinfo = 1;
  }

  /* if sdcdev is not defined: check if a predefined display device expression is available for this display */
  if (strlen(sdcdev) == 0) {
    const char* defaultdev = serdisp_defaultdevice(dispname);
    if (defaultdev && strlen(defaultdev) > 0) {
      sdtools_strncpy(sdcdev, defaultdev, 50);
    }
  }
  sdcd = SDCONN_open(sdcdev);

  if (sdcd == (serdisp_CONN_t*)0) {
    fprintf(stderr, "Error: Unable to open %s, additional info: %s\n", sdcdev, sd_geterrormsg());
    err = 1;
  }

  dd = serdisp_init(sdcd, dispname, optionstring);

  if (!dd) {
    fprintf(stderr, "Error: Unable to open '%s', additional info: %s\n", dispname, sd_geterrormsg());
    err = 1;
  }

  if ( !serdisp_isoption(dd, "TOUCHDEVICE") ) {
    fprintf(stderr, "Error: Display driver '%s' doesn't support a touchscreen.\n", dispname);
    err = 1;
  }

  if ( SDGPI_search(dd, "TOUCHSCREEN") == 0xFF ) {
    fprintf(stderr, "Error: Touchscreen could not be initialised.\n");
    err = 1;
  }

  if ( !serdisp_isoption(dd, "TOUCHSWAPX") || !serdisp_isoption(dd, "TOUCHSWAPY") ) {
    fprintf(stderr, "Error: Touchscreen doesn't support swapping of axes (TOUCHSWAPX and TOUCHSWAPY).\n");
    err = 1;
  }

  if ( ! serdisp_isoption(dd, "TOUCHMINX") || !serdisp_isoption(dd, "TOUCHMINY") ||
       ! serdisp_isoption(dd, "TOUCHMAXX") || !serdisp_isoption(dd, "TOUCHMAXY")
  ) {
    fprintf(stderr, "Error: Touchscreen doesn't support calibration of touch area (TOUCH[MIN|MAX][X|Y]).\n");
    err = 1;
  }

  if (err) {
    if (optionstring) free(optionstring);
    exit(1);
  }

  if (flag_showinfo) {
    int32_t xo = 0, yo = 0, xs = 0, ys = 0;
    int swapx = 0, swapy = 0;

    printf("touchscreen_tool version: %d.%d\n", TOUCHSCREENTOOL_VERSION_MAJOR, TOUCHSCREENTOOL_VERSION_MINOR);
    printf("serdisplib version:       %d.%d\n",
      SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
      SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
    );
    printf("\nDisplay WidthxHeight:     %dx%d\n", serdisp_getwidth(dd), serdisp_getheight(dd));

    swapx = (int) serdisp_getoption(dd, "TOUCHSWAPX", 0);
    swapy = (int) serdisp_getoption(dd, "TOUCHSWAPY", 0);
    xo = (int32_t) serdisp_getoption(dd, "TOUCHMINX", 0);
    yo = (int32_t) serdisp_getoption(dd, "TOUCHMINY", 0);
    xs = (int32_t) serdisp_getoption(dd, "TOUCHMAXX", 0);
    ys = (int32_t) serdisp_getoption(dd, "TOUCHMAXY", 0);

    printf("Touchscreen Info:\n");
    printf("  Min X/Y:                %d/%d\n", xo, yo);
    printf("  Max X/Y:                %d/%d\n", xs, ys);
    printf("  Swap X/Y:               %c/%c\n\n", (swapx) ? 'Y' : 'N', (swapy) ? 'Y' : 'N');
  }


  if (flag_calibrate) {
    int32_t xo = 0, yo = 0, xs = 0, ys = 0;
    int swapx = 0, swapy = 0;

    int16_t nx1, ny1, nx2, ny2;        /* raw values for both coordinates */
    int32_t x1, y1, x2, y2;            /* normalised values for both coordinates */
    int16_t dnx1, dny1, dnx2, dny2;    /* differences of normalised coordinates to expected positions */

    int inv_swapx = 0, inv_swapy = 0;

    int32_t orig_xs, orig_ys;

    int w = serdisp_getwidth(dd);
    int h = serdisp_getheight(dd);
    int mx = w / 2 - 1;
    int my = h / 2 - 1;

    swapx = (int) serdisp_getoption(dd, "TOUCHSWAPX", 0);
    swapy = (int) serdisp_getoption(dd, "TOUCHSWAPY", 0);
    xo = (int32_t) serdisp_getoption(dd, "TOUCHMINX", 0);
    yo = (int32_t) serdisp_getoption(dd, "TOUCHMINY", 0);
    xs = (int32_t) serdisp_getoption(dd, "TOUCHMAXX", 0);
    ys = (int32_t) serdisp_getoption(dd, "TOUCHMAXY", 0);
    orig_xs = xs;
    orig_ys = ys;

    int drawoff_x = w / 10;
    int drawoff_y = h / 10;
    int drawrad = (drawoff_x + drawoff_y) / 10;

    int draw_nx, draw_ny;

    int timeout = 60;
    int pr = 0;

    if (tmp_debuglevel >= 3)
      fprintf(stderr, "Before calibration:\nMin X/Y: %d/%d, Max X/Y: %d/%d, Swap X/Y: %d/%d\n\n", xo, yo, xs, ys, swapx, swapy);

    memset (dd->scrbuf_chg, 0xFF, dd->scrbuf_chg_size);
    serdisp_clear(dd);

    fprintf(stderr, ">>> tap on first cross ... \n");

    draw_nx = drawoff_x;
    draw_ny = drawoff_y;

    draw_cross(dd, draw_nx, draw_ny, drawrad);
    if (wait_event(dd, &nx1, &ny1, &x1, &y1, timeout)) {
      /* invert swapx? */
      if ( nx1 > mx ) {
        inv_swapx = 1;
        nx1 = w - 1 - nx1;
        if (tmp_debuglevel >= 3)
          fprintf(stderr, "--> swapping x-axis ...\n");
      }
      /* invert swapy? */
      if ( ny1 > my ) {
        inv_swapy = 1;
        ny1 = h - 1 - ny1;
        if (tmp_debuglevel >= 3)
          fprintf(stderr, "--> swapping y-axis ...\n");
      }

      dnx1 = nx1 - draw_nx;
      dny1 = ny1 - draw_ny;

      if (tmp_debuglevel >= 3)
        fprintf(stderr, "... expected %d/%d, got %d/%d, diff: %d/%d\n", draw_nx, draw_ny, nx1, ny1, dnx1, dny1);
    } else {
      err = 1;
      fprintf(stderr, "timeout reached\n");
    }
    if (!err) {
      fprintf(stderr, "\n>>> tap on second cross ... \n");
      serdisp_clear(dd);
      sleep(1);

      draw_nx = w - 1 - drawoff_x;
      draw_ny = h - 1 - drawoff_y;

      draw_cross(dd, draw_nx, draw_ny, drawrad);

      if (wait_event(dd, &nx2, &ny2, &x2, &y2, timeout)) {
        if ( inv_swapx ) {
          nx2 = w - 1 - nx2;
        }
        /* invert swapy? */
        if ( inv_swapy ) {
          ny2 = h - 1 - ny2;
        }

        dnx2 = nx2 - draw_nx;
        dny2 = ny2 - draw_ny;

        if (tmp_debuglevel >= 3)
          fprintf(stderr, "... expected %d/%d, got %d/%d, diff: %d/%d\n", draw_nx, draw_ny, nx2, ny2, dnx2, dny2);
      } else {
        err = 1;
        fprintf(stderr, "timeout reached\n");
      }

      if (!err) {
        /* swapx as expected ? */
        if ( nx2 < mx ) {
          fprintf(stderr, "Error: TOUCHSWAPX would differ between first and second cross!\n");
          err = 1;
        }
        /* swapy as expected ? */
        if ( ny2 < my ) {
          fprintf(stderr, "Error: TOUCHSWAPY would differ between first and second cross!\n");
          err = 1;
        }
      }
    }

    if (!err) {
      double ux1, ux2, uy1, uy2;

      if (tmp_debuglevel >= 3)
        fprintf(stderr, "calculating new calibration ... \n\n");

      /* change swapx/y according to inv_swapx/y */
      swapx = swapx ^ inv_swapx;
      swapy = swapy ^ inv_swapy;

      ux1 = (double)xo + (double)(nx1 * xs) / (double)w;
      ux2 = (double)xo + (double)(nx2 * xs) / (double)w;

      uy1 = (double)yo + (double)(ny1 * ys) / (double)h;
      uy2 = (double)yo + (double)(ny2 * ys) / (double)h;

      /* calculate new xs/ys and xo/yo */
      xs = (int)((ux1 - ux2) * (double)w / ((double)drawoff_x - ((double)w-(double)drawoff_x)) + 0.5);
      ys = (int)((uy1 - uy2) * (double)h / ((double)drawoff_y - ((double)h-(double)drawoff_y)) + 0.5);

      xo = (int)(ux2 - ( ((double)w-(double)drawoff_x) * (double)xs) / (double)w + 0.5);
      yo = (int)(uy2 - ( ((double)h-(double)drawoff_y) * (double)ys) / (double)h + 0.5);


      /* fix out of bounds errors */
      if (xo < 0) {
        fprintf(stderr, "-> TOUCHMINX < 0. setting to 0\n");
        xo = 0;
      }
      if ( xs < 0) {
        fprintf(stderr, "-> TOUCHMAXX < 0. resetting\n");
        xs = orig_xs;
      }
      if ( xs > 0xFFFF) {
        fprintf(stderr, "-> TOUCHMAXX > 0xFFFF. setting to 0xFFFF\n");
        xs = 0xFFFF;
      }
      if (yo < 0) {
        fprintf(stderr, "-> TOUCHMINY < 0. setting to 0\n");
        yo = 0;
      }
      if ( ys > 0xFFFF) {
        fprintf(stderr, "-> TOUCHMAXY > 0xFFFF. setting to 0xFFFF\n");
        ys = 0xFFFF;
      }
      if ( ys < 0) {
        fprintf(stderr, "-> TOUCHMAXY < 0. resetting\n");
        ys = orig_ys;
      }


      serdisp_setoption(dd, "TOUCHSWAPX", swapx);
      serdisp_setoption(dd, "TOUCHSWAPY", swapy);
      serdisp_setoption(dd, "TOUCHMINX", xo);
      serdisp_setoption(dd, "TOUCHMINY", yo);
      serdisp_setoption(dd, "TOUCHMAXX", xs);
      serdisp_setoption(dd, "TOUCHMAXY", ys);

      /* values could have been corrected by serdisp_gpevents.c, so re-read them */
      swapx = (int) serdisp_getoption(dd, "TOUCHSWAPX", 0);
      swapy = (int) serdisp_getoption(dd, "TOUCHSWAPY", 0);
      xo = (int32_t) serdisp_getoption(dd, "TOUCHMINX", 0);
      yo = (int32_t) serdisp_getoption(dd, "TOUCHMINY", 0);
      xs = (int32_t) serdisp_getoption(dd, "TOUCHMAXX", 0);
      ys = (int32_t) serdisp_getoption(dd, "TOUCHMAXY", 0);

      if (tmp_debuglevel >= 3)
        fprintf(stderr, "After calibration:\nMin X/Y: %d/%d, Max X/Y: %d/%d, Swap X/Y: %d/%d\n\n", xo, yo, xs, ys, swapx, swapy);

      fprintf(stderr, ">>> tap on center cross (for verification) ... \n");
      serdisp_clear(dd);
      sleep(1);
      draw_cross(dd, mx, my, drawrad);
      if (wait_event(dd, &nx1, &ny1, &x1, &y1, timeout)) {
        fprintf(stderr, "... expected %d/%d, got %d/%d, diff: %d/%d\n", mx, my, nx1, ny1, nx1-mx, ny1-my);
      } else {
        err = 1;
        fprintf(stderr, "timeout reached\n");
      }
    }
    if (err) {
      draw_error(dd);
      fprintf(stderr, "please restart calibration\n");
    } else {
      serdisp_clear(dd);

      fprintf(stderr, "\nCalibration string to be attached to option parameters:\n\n");
      if (xo || yo) {
        fprintf(stderr, "%cTOUCHMINX=%d;TOUCHMINY=%d", ((pr) ? ';' : '\"'), xo, yo);
        pr = 1;
      }
      if (xs != 0xFFFF && ys != 0xFFFF) {
        fprintf(stderr, "%cTOUCHMAXX=%d;TOUCHMAXY=%d", ((pr) ? ';' : '\"'), xs, ys);
        pr = 1;
      }
      if (swapx) {
        fprintf(stderr, "%cTOUCHSWAPX=1", ((pr) ? ';' : '\"'));
        pr = 1;
      }
      if (swapy) {
        fprintf(stderr, "%cTOUCHSWAPY=1", ((pr) ? ';' : '\"'));
        pr = 1;
      }
      if (pr) {
        fprintf(stderr, "\"\n\n");
      } else {
        fprintf(stderr, "(no option parameters required)\n\n");
      }

    }
    sleep(5);
  }

  if (optionstring) free(optionstring);
  serdisp_quit(dd);
  return 0;
}

