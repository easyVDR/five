/*
 * carddetect, a program to list dvb devices and pvr cards. Made for easyvdr-0.9.
 * Copyright (C) 2011, 2012  Winfried Koehler
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/ioctl.h>

#include <linux/types.h>
  
#define MAX_DVB_ADAPTERS  8
#define MAX_DVB_FRONTENDS 8
#define MAX_ATV_DEVICES   8

typedef enum { i0, i1, i2, i3 } placeholder;
struct dtv_info {
   char         name[128];
   placeholder  deprecated1;
   __u32        frequency_min;
   __u32        frequency_max;
   __u32        frequency_stepsize;
   __u32        frequency_tolerance;
   __u32        symbol_rate_min;
   __u32        symbol_rate_max;
   __u32        symbol_rate_tolerance;
   __u32        deprecated2;
   __u32        capabilities;
};

struct atv_info {
   __u8         driver[16];
   __u8         name[32];
   __u8         bus_info[32];
   __u32        version;
   __u32        capabilities;
   __u32        reserved[4];
};

typedef enum {
   DVBC_ANNEX_A       = 1,
   DVBC_ANNEX_B       = 2,
   DVBC_ANNEX_C       = 18,
   ISDBC              = 10,
   DVBT               = 3,
   DVBT2              = 16,
   DVBH               = 7,
   ISDBT              = 8,
   DMBTH              = 13,
   DSS                = 4,
   DVBS               = 5,
   DVBS2              = 6,
   TURBO              = 17,
   ISDBS              = 9,
   ATSC               = 11,
   ATSCMH             = 12,
   CMMB               = 14,
   DAB                = 15,
} dtv_delsys;

struct dtv_property {
   __u32 command;
   __u32 reserved0[3];
   union {
      __u32 data;
      struct {
         __u8 data[32];
         __u32 length;
         __u32 reserved1[3];
         void *reserved2;
      } buffer;
   } u;
   int result;
} __attribute__ ((packed));

struct dtv_properties {
   __u32 count;
   struct dtv_property *properties;
};

#define debug(msg...) if (enable_debug) printf(msg) 

int main(int argc, char **argv) {
  char device_node[80];
  int i, j, fd, device_number = 0;
  int init = 0, enable_debug = 0;
  struct dtv_info dtv_info;
  struct atv_info atv_info;
  uint16_t dvbapi = 302;
  uint8_t  delsys[256];
  struct dtv_property p[2];
  struct dtv_properties seq = {.count = 1, .properties = p};

  struct option lo[] = {{"debug", 0 , 0, 'd'},{0,0,0,0}};

  while ((i = getopt_long(argc, argv, "d", lo, &j)) != -1) {
        switch (i) {
               case 'd': enable_debug = 1; break;
               default:;
               }
        }

  for (i = 0; i < MAX_DVB_ADAPTERS; i++) {
      for (j = 0; j < MAX_DVB_FRONTENDS; j++) {
          snprintf (device_node, sizeof(device_node), "/dev/dvb/adapter%i/frontend%i", i, j);
          memset(&delsys[0], 0, sizeof(uint8_t) * 256);
          int k = 0;        

          debug("\tchecking for device %d %-30s... ", device_number, device_node);
          if ((fd = open (device_node, O_RDONLY | O_NONBLOCK)) < 0) {
             debug("[ NO ]\n");
             continue;
             }
          debug("[ YES ]\n");

          if (! init) {
             init = 1;
             p[0].command = 35;
             p[0].u.data  = (dvbapi / 100) << 8 | (dvbapi % 100);

             ioctl(fd, _IOR('o', 83, struct dtv_properties), &seq);
             dvbapi = 100*(p[0].u.data >> 8) + (p[0].u.data & 0xFF);

             debug("\t\tnew dvb api ?       -> %d ... [ %s ]\n", dvbapi, dvbapi<505?"NO":"YES");
             }

          debug("\t\tioctl dtv_info             ... ");
          if (ioctl(fd, _IOR('o', 61, struct dtv_info), &dtv_info) < 0) {
             debug("[ FAIL ]\n");
             close (fd);
             continue;
             }
          debug("[ SUCCESS ]\n");        

          if (dvbapi <= 504) {
             debug("\t\tusing deprecated dtv_info  ... ");
             switch (dtv_info.deprecated1) {
                    case 0:
                         debug("[ DVBS/S2 ]\n");
                         delsys[DVBS] = 1;
                         if ((dtv_info.capabilities & (1 << 28)) == (1 << 28))
                            delsys[DVBS2] = 1;
                         break;                           
                    case 1:
                         debug("[ DVBC ]\n"); 
                         delsys[DVBC_ANNEX_A] = 1;
                         break; 
                    case 2:
                         debug("[ DVBT ]\n"); 
                         delsys[DVBT] = 1;
                         if ((dtv_info.capabilities & (1 << 28)) == (1 << 28))
                            delsys[DVBT2] = 1;
                         break;
                    case 3:
                         debug("[ ATSC ]\n"); 
                         delsys[ATSC] = 1;
                         break;
                    default:
                         debug("[ UNKNOWN ]\n");
                    } 
             }
          else {
             debug("\t\treading delivery systems   ...");              
             p[0].command = 44;
             if (ioctl(fd, _IOR('o', 83, struct dtv_properties), &seq) < 0) {
                debug(" [ FAIL ]\n");
                continue;
                }

             while(p[0].u.buffer.length > 0) {
                  debug(" [ %u ]", p[0].u.buffer.data[k]);
                  delsys[p[0].u.buffer.data[k++]] = 1;
                  p[0].u.buffer.length--;
                  }
             debug("\n");
             }

          close (fd);

          printf("DEVICE_%.2d;%s;%s;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u\n",
                device_number, dtv_info.name, device_node,
                delsys[DVBC_ANNEX_A],
                delsys[DVBC_ANNEX_B],
                delsys[DVBC_ANNEX_C],
                delsys[ISDBC],
                delsys[DVBT],
                delsys[DVBT2],
                delsys[ISDBT],
                delsys[DVBH],
                delsys[DVBS],
                delsys[DVBS2],
                delsys[TURBO],
                delsys[ISDBS],
                delsys[ATSC],
                delsys[ATSCMH],
                delsys[DMBTH],
                delsys[DSS],
                delsys[CMMB],
                delsys[DAB],
                0,0);

           debug("\n\n");
           device_number++;
          }
      }


  for (i = 0; i < MAX_ATV_DEVICES; i++) {
      int pvr = 0, pvrmpeg = 0;
      snprintf (device_node, sizeof(device_node), "/dev/video%i", i);

      debug("\tchecking for device %d %-30s... ", device_number, device_node);
      if ((fd = open(device_node, O_RDONLY)) < 0) {
         debug("[ NO ]\n");
         continue;
         }
      debug("[ YES ]\n");

      debug("\t\tioctl atv_info             ... ");
      if (ioctl(fd, _IOR('V',  0, struct atv_info), &atv_info)!= 0) {
          debug("[ FAIL ]\n");
          close(fd);
          continue;
          }
      debug("[ SUCCESS ]\n");

      debug("\t\tis pvrinput                ... ");
      if (!strncasecmp((char *) atv_info.driver, "ivtv", 4) ||
          !strncasecmp((char *) atv_info.driver, "cx8800", 6) ||
          !strncasecmp((char *) atv_info.driver, "pvrusb2", 7)) {
         debug("[ YES ]\n");
         pvr++;
         if ((atv_info.capabilities & 0x00000200) == 0x00000200)
            pvrmpeg++;
         printf("DEVICE_%.2d;%s;%s;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u;%u\n",
               device_number, atv_info.name, device_node,
               0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
               pvr,pvrmpeg);
         debug("\n\n");
         }
      else
         debug("[ NO ]\n");
      close(fd);
      device_number++;
      }

  return 0;
}
