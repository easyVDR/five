/* hdet, a simple multimedia hardware detection utility.
 *
 * Copyright (C) 2011 Winfried Koehler
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *                                                             
 * The author can be reached at: handygewinnspiel AT gmx DOT de
 */

#ifndef _HDET_H
#define _HDET_H


/******************************************************************************
 * hdet internal macros and defs.                     
 *****************************************************************************/
#define nlist(name) struct list_head name = { &(name), &(name) }
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
#define getType(A) (uint32_t)(*A << 24 | *(A+1) << 16 | *(A+2) << 8 | *(A+3))
#define local4(X) ntohl(X)
#define local2(X) ntohs(X)        
#define sc2u(Y)  sscanf(pl,"%hX", &db->Y)
#define scstr(Y) db->Y = strdup(pl)
#define scbu(Y)  db->Y = getType(pl),pl=p+1,n++

#define b4u(X) fl-=4*fread(&d4,4,1,i); X=local4(d4)
#define b2u(X) fl-=2*fread(&d2,2,1,i); X=local2(d2)
#define be2(X) htons(X)
#define be4(X) htonl(X)
#define w4u(X) d4=be4(X); fwrite(&d4,1,4,o)
#define w2u(X) d2=be2(X); fwrite(&d2,1,2,o)
#define wstr(X)if(X)len=strlen(X); else len=0; w2u(len); if(len) fwrite(&*X,1,len,o)
#define lat(X) list_add_tail(X, &database)
#define while_entry(X) list_for_each(X, &database)
#define str(X) b2u(len); X=calloc(len+1,1); fl-=fread(&*X,1,len,i)
#define __DE(a,b...) do {if(a<=verbose){fprintf(stderr,b);}}while(0)
#define __DO(a,b...) do {if(a<=verbose){fprintf(stdout,b);}}while(0)
#define __DL(a, b, c...) __DE(a,"%s:%d: " b, __FUNCTION__, __LINE__ , ##c)
#define E(a,b...)  do { __DL(-1, "ERROR: " a , ##b); exit(-1); } while(0)
#define E0(a,b...) do { __DO(-1, "ERROR: " a , ##b); exit(-1); } while(0)
#define W(a,b...)  do { __DL(-1, "WARNING: " a , ##b);} while(0)
#define I(a...) __DO(1,a)
#define V(a...) __DO(2,a)
#define DBG(a...) __DO(3,a)
#define NDB "no database"
#define ndb if(!i)E0(NDB " : %s\n",f);
#define wdb if(v!=magic)E0("invalid database version.\n");

#define HWUNDEF   0x00
#define VGA       0x10
#define AUDIO     0x11
#define MEDIA     0x12
#define ETHERNET  0x13
#define EVENT     0x14

#define MAX_DEVICE_CHILDS 32
struct bus_device;

#include "list.h"

typedef struct {
    uint16_t type;
    uint32_t bus;
    char *name;
    char *caps;
    char *driverpackage;
    char *drivers;
    char *plugin;
    char *comment; 
    struct list_head list;
    union {       
      struct {
        uint16_t dClass;
        uint16_t dSubclass;
        uint16_t dVendor;
        uint16_t dDevice;
        uint16_t dSubvendor;
        uint16_t dSubdevice;
        uint16_t reserved1;
        uint16_t reserved2;
        } pci;
      struct {
        uint16_t dClass;
        uint16_t dSubclass;
        uint16_t dVendor;
        uint16_t dDevice;
        uint16_t dBcdDevice_lo;  
        uint16_t dBcdDevice_hi;
        uint16_t reserved1;
        uint16_t reserved2;
        char *manufacturer;
        char *product;
        char *serial;
        } usb;
      struct {
        uint16_t dClass;
        uint16_t dSubclass;
        uint16_t dVendor;
        uint16_t dDevice;
        uint16_t reserved1;  
        uint16_t reserved2;
        } ieee1394;
      struct {
	uint16_t dVendor;
	uint16_t dDevice;
	uint16_t dVersion;
        } input;
      } u;
} db_t;

typedef struct bus_device {
  uint32_t bus;
  char * devpath;
  char * kernelname;
  char * phys_path;
  char * subsystem;
  char * driver;
  char * driver_module;
  char * firmware_node;
  char * modalias;
  union {       
     struct {
        uint32_t class_id;             // class 0x0c0320
        uint16_t vendor_id;            // vendor 0x8086
        uint16_t device_id;            // device 0x3a3a
        uint16_t subdevice_id;         // subsystem_device
        uint16_t subvendor_id;         // subsystem_vendor 0x1043
        int irq;                       // irq 23
        uint16_t domain;
        uint8_t  busnum;
        uint8_t  slot;
        uint8_t  function;
     } pci;
     struct {
        uint16_t bDeviceClass;         // 00 (Defined at Interface level)
        uint16_t bDeviceSubClass;      // 00
        uint16_t bDeviceProtocol;      // 00
        uint16_t bAlternateSetting;
        uint16_t bInterfaceClass;
        uint16_t bInterfaceSubClass;
        uint16_t bInterfaceNumber;
        uint16_t bInterfaceProtocol;
        uint16_t bNumInterfaces;       // ' 2'
        uint16_t bNumEndpoints;
        uint16_t bcdDevice;            // 0106
        int      busnum;               // bus: bus we're part of
        int      devnum;               // 2                       devnum: device number;address on a USB bus
        int      maxchild;             // 0                       maxchild: number of ports if hub
        int      speed;                // 480                     speed: device speed: high/full/low (or error)
        uint16_t idVendor;             // 0ccd
        uint16_t idProduct;            // 0097
        char * manufacturer;           // NEWMI                   %.100s string, if present (static)
        char * product;                // USB2.0 DVB-T TV Stick   %.100s string, if present (static)
        char * serial;                 // 010101010600001         serial: iSerialNumber string, if present (static)
        char * version;                // ' 2.00'
        char * dev;                    // 189:257                 major:minor. 189 = usb_device;257 = ??
        // guessed.
        const char *bus_name;          // stable id (PCI slot_name etc)
     } usb;
     struct {
        uint32_t class_id;             // class
        uint16_t vendor_id;            // vendor
        uint16_t device_id;            // device
     } ieee1394;
     struct {
        uint16_t bustype;
        uint16_t vendor;
        uint16_t product;
        uint16_t version;
        char * name;
        char * phys;
        char * uniq;
        char * device;
        struct bus_device * busdevice;
     } input;
  }u; 
  struct bus_device * parent;
  struct bus_device * childs[MAX_DEVICE_CHILDS];
  struct list_head list;
  db_t * database;
} device_t;

typedef struct {
   uint16_t type;
   uint32_t bus;
   int load;
   char * plugin;
   char * driver;
   char * package;
   char * capability;
} scantype_t;

extern int verbose;

#endif
