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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <assert.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <netinet/in.h>

#include "db_version.h"
#include "hdet.h"
#include "tools.h"
int verbose = 2;

static nlist(bus_devices);
static nlist(database);

#define DLOCAL "database.bin"
#define DINST (DATADIR "/hdet/" DLOCAL)

#define SYSFS "/sys"

#define isKernelObject    S_ISDIR(s.st_mode)
#define isObjectAttribute S_ISREG(s.st_mode)
#define isObjectRelated   S_ISLNK(s.st_mode)


static void getdb(const char *f) {
  assert(f);
  FILE *i = fopen(f, "rb");
  struct stat s;
  db_t *d;
  uint16_t len, d2;
  uint32_t d4, v;
  int64_t fl;

  ndb;stat(f,&s),fl=s.st_size,b4u(v);wdb;

  while (!feof(i) && fl>0) {
        d = calloc(sizeof(db_t),1);
        b4u(d->bus);
        b2u(d->type);
        str(d->name);
        str(d->caps);
        str(d->driverpackage);
        str(d->drivers);
        str(d->plugin);
        str(d->comment);
        switch(d->bus) {
              case PCI:
                 {
                 b2u(d->u.pci.dClass);
                 b2u(d->u.pci.dSubclass);
                 b2u(d->u.pci.dVendor);
                 b2u(d->u.pci.dDevice);
                 b2u(d->u.pci.dSubvendor);
                 b2u(d->u.pci.dSubdevice);
                 }
                 break;
              case USB:
                 {
                 b2u(d->u.usb.dClass);
                 b2u(d->u.usb.dSubclass);
                 b2u(d->u.usb.dVendor);
                 b2u(d->u.usb.dDevice);
                 b2u(d->u.usb.dBcdDevice_lo);
                 b2u(d->u.usb.dBcdDevice_hi);
                 str(d->u.usb.manufacturer);
                 str(d->u.usb.product);
                 str(d->u.usb.serial);
                 }
                 break;
              case IEEE1394:
                 {
                 b2u(d->u.ieee1394.dClass);
                 b2u(d->u.ieee1394.dSubclass);
                 b2u(d->u.ieee1394.dVendor);
                 b2u(d->u.ieee1394.dDevice);
                 }
                 break;
              default:return;
              }
        lat(&d->list);
        }
  fclose(i);
}

const char * dbp() {
  struct stat buf;
  if (!stat(DLOCAL,&buf)) return DLOCAL;
  if (!stat(DINST,&buf))  return DINST;
  E0(NDB "\n");
}

void associate_device(device_t * d) {
  struct list_head *p;
  db_t *db;

  if (! d->bus) return;
  d->database = 0;

  while_entry(p) {
    db = list_entry(p, db_t, list);
    if (d->bus == db->bus) {
       switch(d->bus) {
             case PCI:
                  if ((PCI == db->bus) &&
                      (d->u.pci.vendor_id == db->u.pci.dVendor) &&
                      (db->u.pci.dDevice    == 0xffff || (d->u.pci.device_id    == db->u.pci.dDevice)) &&
                      (db->u.pci.dSubvendor == 0xffff || (d->u.pci.subvendor_id == db->u.pci.dSubvendor)) &&
                      (db->u.pci.dSubdevice == 0xffff || (d->u.pci.subdevice_id == db->u.pci.dSubdevice)) &&
                      (db->u.pci.dClass    == 0xff    || (((d->u.pci.class_id >> 16) & 0xFF) == db->u.pci.dClass)) /*&&
                      (db->u.pci.dSubclass == 0xff    || (((d->u.pci.class_id >> 8 ) & 0xFF) == db->u.pci.dClass))*/ )
                     d->database = db;
                  break;
             case USB:
                  if ((USB == db->bus) &&
                      (d->u.usb.idVendor == db->u.usb.dVendor) &&
                      (d->u.usb.idProduct == db->u.usb.dDevice) &&
                      (db->u.usb.dBcdDevice_lo == 0xffff || (d->u.usb.bcdDevice >= db->u.usb.dBcdDevice_lo)) &&
                      (db->u.usb.dBcdDevice_hi == 0xffff || (d->u.usb.bcdDevice <= db->u.usb.dBcdDevice_hi)))
                     d->database = db;
                  break;
             case IEEE1394:
                  if ((IEEE1394 == db->bus) &&
                      (d->u.ieee1394.vendor_id == db->u.ieee1394.dVendor) &&
                      (d->u.ieee1394.device_id == db->u.ieee1394.dDevice))
                     d->database = db;
                  break;
             case INPUT:
                  // associate only standalone devices here, i.e. ATI X10.                 
                  if (db->bus != INPUT) continue;
                  switch(d->u.input.bustype) {
                        case BUS_PCI:
                            if ((PCI == db->bus) &&
                                (d->u.input.vendor == db->u.pci.dVendor) &&
                                (d->u.input.product == db->u.pci.dDevice))
                               d->database = db;
                            break;
                        case BUS_USB:
                            if ((USB == db->bus) &&
                                (d->u.input.vendor == db->u.usb.dVendor) &&
                                (d->u.input.product == db->u.usb.dDevice) &&
                                (db->u.usb.dBcdDevice_lo == 0xffff || (d->u.input.version >= db->u.usb.dBcdDevice_lo)) &&
                                (db->u.usb.dBcdDevice_hi == 0xffff || (d->u.input.version <= db->u.usb.dBcdDevice_hi)))
                               d->database = db;
                            break;
                        case BUS_RS232:
                        case BUS_I2C:
                        case BUS_PARPORT:
                            //FIXME: add matching conditions for those device types as soon
                            //       as some of those are defined in db.                            
                        default:;
                        }
             default:
                  E("No association defined for bus %04x\n", d->bus);
             }
       }

    if (d->database)
       break;
    }
}

int rd_attr(const char * f, uint32_t *u, uint16_t *us, int *i, char *s) {
  FILE * sysfs;
  char buf[256];
  unsigned _us;

  memset(&*buf,0,256);
  if (!(sysfs = fopen(f,"r"))) {
     W("couldnt open %s\n",f);
     return -1;
     }
  if (!(fgets(buf,256,sysfs))) {
     W("couldnt read %s\n",f);
     fclose(sysfs);
     return -1;
     }
  if (u)  sscanf(buf,"%x",u);
  if (us) sscanf(buf,"%x",&_us), *us=_us;
  if (i)  sscanf(buf,"%d",i);
  if (s)  {char *p;if ((p=strchr(buf,10))) *p=0;strcpy(s,buf);}
  fclose(sysfs);
  return 0;
}

void associate_input_device(struct list_head *head, device_t * dev, int level) {
  struct list_head *p;
  device_t *d;

  if (! dev->u.input.device)
     return;

  list_for_each(p, &bus_devices) {
      d = list_entry(p, device_t, list);
      if (! d->kernelname)
         continue;

      if (! strcasecmp(dev->u.input.device, d->kernelname)) {
         dev->u.input.busdevice = d;
         break;
         }
      }

  if (!strncasecmp((dev->u.input.busdevice)->kernelname,"input",5) &&
      !strncasecmp(dev->kernelname,"event",5)) {
      dev->u.input.name = strdup((dev->u.input.busdevice)->u.input.name);
      dev->u.input.phys = strdup((dev->u.input.busdevice)->u.input.phys);
      dev->u.input.uniq = strdup((dev->u.input.busdevice)->u.input.uniq);

      // hmmm..., but otherwise i will not find the /dev/input/eventX
      free(dev->devpath);
      dev->devpath = strdup((dev->u.input.busdevice)->devpath);
      }
}

void associate_input_devices(struct list_head *head) {
  struct list_head *p;
  device_t *d;

  list_for_each(p, head) {
      d = list_entry(p, device_t, list);
      if (d->bus != INPUT)
         continue;

      associate_input_device(head, d, 0);
      }
}

int sysfs_rd_driver_module(const char * path, device_t * dev) {
   char * f;
   struct stat s;

   asprintf(&f, "%s/%s", path, "driver/module");

   if (!lstat(f, &s)) {
      char * buf = calloc(256,1);
      rd_link(f, buf);
      dev->driver_module=strdup(buf);
      free(buf);
      }
   free(f);
   return 0;
}

#define isAttr(X) !strcasecmp(attr->d_name,X)
#define isAttrN(X,N) !strncasecmp(attr->d_name,X,N)
#define RUL(X) ulp=&dev->X
#define RU(X)  up =&dev->X
#define RI(X)  ip =&dev->X
#define RS(X)  rd_attr(f,0,0,0,buf), dev->X = strdup(buf)

int rd_sys_devices(const char * path, device_t * dev) {
   DIR * sysfs;
   struct dirent * attr = 0;
   struct stat s;
   char * rpath = 0;

   if (!path || !dev) {
      dev = calloc(sizeof(device_t),1);

      dev->kernelname = strdup("devices");
      dev->devpath = strdup("/devices");
      dev->parent = 0;
      list_add_tail(&dev->list,&bus_devices);

      rpath = calloc(strlen(SYSFS) + strlen(dev->devpath) + 1, 1);
      asprintf(&rpath, "%s%s", SYSFS, dev->devpath);
      sysfs = opendir(rpath);
      }
   else {
      stat(path, &s);
      if (! isKernelObject) {
         I("\t%s: %s is a not dir..\n", __FUNCTION__, path);
         return -1;
         }
      sysfs = opendir(path);
      }

   while ((attr = readdir(sysfs))) {
         char * f;
         uint32_t * ulp = 0;
         uint16_t * up = 0;
         int * ip = 0;
         char * buf;

         if (*attr->d_name == 46) continue;

         buf = calloc(256,1);
         asprintf(&f,"%s/%s", path?path:rpath, attr->d_name);
         lstat(f, &s);

         if (isObjectAttribute) {
            // attribute: a regular file.
            if (isAttr("modalias"))  RS(modalias);            
            switch(dev->bus) {
                  case PCI:
                     if      (isAttr("class"))              RUL(u.pci.class_id);
                     else if (isAttr("vendor"))             RU(u.pci.vendor_id);
                     else if (isAttr("device"))             RU(u.pci.device_id);
                     else if (isAttr("subsystem_device"))   RU(u.pci.subdevice_id);
                     else if (isAttr("subsystem_vendor"))   RU(u.pci.subvendor_id);
                     else if (isAttr("irq"))                RI(u.pci.irq);
                     break;
                  case USB:
                     if      (isAttr("bDeviceClass"))       RU(u.usb.bDeviceClass);
                     else if (isAttr("bDeviceSubClass"))    RU(u.usb.bDeviceSubClass);
                     else if (isAttr("bDeviceProtocol"))    RU(u.usb.bDeviceProtocol);
                     else if (isAttr("bAlternateSetting"))  RU(u.usb.bAlternateSetting);
                     else if (isAttr("bInterfaceClass"))    RU(u.usb.bInterfaceClass);
                     else if (isAttr("bInterfaceSubClass")) RU(u.usb.bInterfaceSubClass);
                     else if (isAttr("bInterfaceNumber"))   RU(u.usb.bInterfaceNumber);
                     else if (isAttr("bInterfaceProtocol")) RU(u.usb.bInterfaceProtocol);
                     else if (isAttr("bNumInterfaces"))     RU(u.usb.bNumInterfaces);
                     else if (isAttr("bNumEndpoints"))      RU(u.usb.bNumEndpoints);
                     else if (isAttr("idVendor"))           RU(u.usb.idVendor);
                     else if (isAttr("idProduct"))          RU(u.usb.idProduct);
                     else if (isAttr("bcdDevice"))          RU(u.usb.bcdDevice);
                     else if (isAttr("busnum"))             RI(u.usb.busnum);
                     else if (isAttr("devnum"))             RI(u.usb.devnum);
                     else if (isAttr("maxchild"))           RI(u.usb.maxchild);
                     else if (isAttr("speed"))              RI(u.usb.speed);
                     else if (isAttr("dev"))                RS(u.usb.dev);
                     else if (isAttr("manufacturer"))       RS(u.usb.manufacturer);
                     else if (isAttr("product"))            RS(u.usb.product);
                     else if (isAttr("serial"))             RS(u.usb.serial);
                     else if (isAttr("version"))            RS(u.usb.version);
                     break;
                  case INPUT:
                     if      (isAttr("name"))               RS(u.input.name);
                     else if (isAttr("phys"))               RS(u.input.phys);
                     else if (isAttr("uniq"))               RS(u.input.uniq);
                     break;
                  }
            if (ulp)     rd_attr(f,ulp,0,0,0);
            else if (up) rd_attr(f,0,up,0,0);
            else if (ip) rd_attr(f,0,0,ip,0); 
            }
         else if (isObjectRelated) {
            // related: a symbolic link.
            char * buf = calloc(256,1);
            rd_link(f, buf);
            if (*buf) {
               if      (isAttrN("firmware_node",13)) dev->firmware_node = strdup(buf); 
               else if (isAttrN("subsystem",9))      dev->subsystem     = strdup(buf);
               else if (isAttrN("driver",6))         dev->driver        = strdup(buf);
               switch(dev->bus) {
                     case INPUT:
                        if (isAttr("device")) dev->u.input.device = strdup(buf);
                        break;
                     default:;
                     }
               }
            else
               W("could not read link %s\n", f);
            free(buf);
            }
         else if (isKernelObject) {
            // object: a directory.
            device_t * d = calloc(sizeof(device_t),1);
            int i = 0;

            list_add_tail(&d->list,&bus_devices);
            for (i=0; i<MAX_DEVICE_CHILDS;i++)
                if (!dev->childs[i]) {
                   dev->childs[i]=d;
                   break;
                   }

            d->parent = dev;
            d->kernelname = strdup(attr->d_name);
            asprintf(&d->devpath, "%s/%s", dev->devpath, d->kernelname);

            // we need to know which kind ob object were reading next,
            // since different objects share the same var names with different type.
            // Actually, this should be improved later..
            d->bus = get_subsystem(f);
            switch (d->bus) {
                   case PCI: {
                      unsigned dom, bus, slot, func;
                      d->u.pci.class_id = d->u.pci.vendor_id = d->u.pci.device_id = 0xFFFF;
                      d->u.pci.subvendor_id = d->u.pci.subdevice_id = 0xFFFF;
               
                      if (sscanf(d->kernelname, "%04x:%02x:%02x.%d", &dom, &bus, &slot, &func) == 4) {               
                         d->u.pci.domain = dom;
                         d->u.pci.busnum = bus;
                         d->u.pci.slot = slot;
                         d->u.pci.function = func;
                         }
                      }
                      break;
                   default:;
                   }
            rd_sys_devices(f, d);
            sysfs_rd_driver_module(f, d);
            associate_device(d);
            }
         free(buf);
         }
   closedir(sysfs);
   if (rpath)
      free(rpath);
   return 0;
}


static inline int hasToken(const char * tokens, const char * token, uint16_t len) {
  char next[256] = { 0 };
  const char *src  = tokens;
  char *dest = &*next;
  unsigned i;
 
  if (!tokens || !*tokens || !token || !*token)
     return 0;
  for(i=0;i<strlen(tokens)+1;i++) {
       if (*next && (*src == 44 || *src == 0)) {
          *(dest+1) = 0;
          if (!strncasecmp(next,token,len))
             return 1;
          memset(&next,0,256);
          dest = &*next;
          src++;
          }
       *dest++ = *src++;
       }  
  return 0;
}

#define hasBus(a,b)              ((!b) || (a == b))
#define hasType(a,b)             ((!b) || (a == b))
#define hasCapability(a,b)       ((!b || !*b) || hasToken(a,b,slen))
#define hasPackage(a,b)          ((!b || !*b) || hasToken(a,b,slen))
#define hasManufacturer(a,b)     ((!b || !*b) || hasToken(a,b,slen))
#define hasProduct(a,b)          ((!b || !*b) || hasToken(a,b,slen))
#define hasSerial(a,b)           ((!b || !*b) || hasToken(a,b,slen))
#define hasPlugin(a,b)           ((!b || !*b) || hasToken(a,b,slen))
#define hasDriver(a,b)           ((!b || !*b) || hasToken(a,b,slen))

int list_devices(const scantype_t * type, uint16_t slen) {
   struct list_head *p;
   device_t *d;
   int items = 0;
  
   list_for_each(p, &bus_devices) {
      d = list_entry(p, device_t, list);

      if (type) {
         db_t * db = d->database;
         if (! d->database ||
             ! hasBus(db->bus,type->bus) ||             
             ! hasType(db->type,type->type) ||
             ! hasCapability(db->caps,type->capability) ||
             ! hasDriver(db->drivers,type->driver) ||
             ! hasPackage(db->driverpackage,type->package) ||
             ! hasPlugin(db->plugin,type->plugin))
            continue;

         if (type->load && db->drivers) {
            char * p = strtok (db->drivers,",");

            while (p) {
                  I("%s %s\n", type->load > 0?"loading":"unloading", p);

                  if (type->load > 0)
                     loadDriver(p);
                  else
                     unloadDriver(p);
                 
                  p = strtok (0, ",");
                  }
            }
         }


      switch(d->bus) {
            case SUBUNKNOWN:
                 // any non identified device or root_device
                 break;
            case PCI:
                 {
                 char * buf = calloc(256,1);
                 get_short_desc(d, buf);

                 V("PCI %-12s -> %s\n", d->kernelname, buf);
                 free(buf);

                 DBG("\t\tclass_id            %06x %s (%s)\n",d->u.pci.class_id,
                                               pci_class((d->u.pci.class_id >> 16) & 0xff),
                                               pci_subclass(d->u.pci.class_id));
                 DBG("\t\tkernelname          %s\n",d->kernelname?d->kernelname:"(NULL)");
                 DBG("\t\tsubsystem           %s\n",d->subsystem?d->subsystem:"(NULL)");
                 DBG("\t\tphys_path           %s\n",d->phys_path?d->phys_path:"(NULL)");
                 DBG("\t\tdriver              %s\n",d->driver?d->driver:"(NULL)");
                 DBG("\t\tdriver_module       %s\n",d->driver_module?d->driver_module:"(NULL)");
                 DBG("\t\tphys_path           %s\n",d->phys_path?d->phys_path:"(NULL)");
                 DBG("\t\tdomain              %04x\n",d->u.pci.domain);
                 DBG("\t\tbusnum              %02x\n",d->u.pci.busnum);
                 DBG("\t\tslot                %02x\n",d->u.pci.slot);
                 DBG("\t\tfunction            %d\n",d->u.pci.function);
                 DBG("\t\tvendor_id           %04x\n",d->u.pci.vendor_id);
                 DBG("\t\tdevice_id           %04x\n",d->u.pci.device_id);
                 DBG("\t\tsubdevice_id        %04x\n",d->u.pci.subdevice_id);
                 DBG("\t\tsubvendor_id        %04x\n",d->u.pci.subvendor_id);
                 if (d->u.pci.irq)
                    DBG("\t\tirq                 %d\n",d->u.pci.irq);
                 }
                 break;
            case USB:
                 {
                 char * buf = calloc(256,1);
                 get_short_desc(d, buf);

                 V("USB %-12s -> %s\n", d->kernelname, buf);
                 free(buf);

                 DBG("\t\tbDeviceClass        %02x%02x %s (%s)\n",
                                               d->u.usb.bDeviceClass,
                                               d->u.usb.bDeviceSubClass,
                                               usb_class(d->u.usb.bDeviceClass),
                                               usb_subclass(d->u.usb.bDeviceClass,
                                                            d->u.usb.bDeviceSubClass,
                                                            d->u.usb.bDeviceProtocol));
                 DBG("\t\tkernelname          %s\n",d->kernelname?d->kernelname:"(NULL)");
                 DBG("\t\tsubsystem           %s\n",d->subsystem?d->subsystem:"(NULL)");
                 DBG("\t\tphys_path           %s\n",d->phys_path?d->phys_path:"(NULL)");
                 DBG("\t\tdriver              %s\n",d->driver?d->driver:"(NULL)");
                 DBG("\t\tdriver_module       %s\n",d->driver_module?d->driver_module:"(NULL)");
                 DBG("\t\tbDeviceProtocol     %02x\n",d->u.usb.bDeviceProtocol);
                 DBG("\t\tidVendor            %04x\n",d->u.usb.idVendor);
                 DBG("\t\tidProduct           %04x\n",d->u.usb.idProduct);
                 DBG("\t\tmanufacturer        %s\n",d->u.usb.manufacturer?d->u.usb.manufacturer:"(NULL)");
                 DBG("\t\tproduct             %s\n",d->u.usb.product?d->u.usb.product:"(NULL)");
                 DBG("\t\tserial              %s\n",d->u.usb.serial?d->u.usb.serial:"(NULL)");
                 DBG("\t\tversion             %s\n",d->u.usb.version?d->u.usb.version:"(NULL)");
                 DBG("\t\tdev                 %s\n",d->u.usb.dev?d->u.usb.dev:"(NULL)");
                 DBG("\t\tbAlternateSetting   %02x\n",d->u.usb.bAlternateSetting);
                 DBG("\t\tbInterfaceClass     %02x\n",d->u.usb.bInterfaceClass);
                 DBG("\t\tbInterfaceSubClass  %02x\n",d->u.usb.bInterfaceSubClass);
                 DBG("\t\tbInterfaceNumber    %02x\n",d->u.usb.bInterfaceNumber);
                 DBG("\t\tbInterfaceProtocol  %02x\n",d->u.usb.bInterfaceProtocol);
                 DBG("\t\tbNumInterfaces      %02x\n",d->u.usb.bNumInterfaces);
                 DBG("\t\tbNumEndpoints       %02x\n",d->u.usb.bNumEndpoints);
                 DBG("\t\tbcdDevice           %04x\n",d->u.usb.bcdDevice);
                 DBG("\t\tbusnum              %d\n",d->u.usb.busnum);
                 DBG("\t\tdevnum              %d\n",d->u.usb.devnum);
                 DBG("\t\tdevpath             %s\n",d->devpath?d->devpath:"(NULL)");
                 DBG("\t\tmaxchild            %d\n",d->u.usb.maxchild);
                 DBG("\t\tspeed               %d\n",d->u.usb.speed);
                 DBG("\t\tmodalias            %s\n",d->modalias?d->modalias:"(NULL)");
                 DBG("\t\tbus_name            %s\n",d->u.usb.bus_name?d->u.usb.bus_name:"(NULL)");
                 }
                 break;
            case INPUT:
                 if (strncasecmp(d->kernelname,"event",5))
                    continue;
                 V("INP %-12s -> %s\n",        d->kernelname, d->u.input.name);
                 DBG("\t\tname                 %s\n",d->u.input.name?d->u.input.name:"(NULL)");
                 DBG("\t\tdevpath              %s\n",d->devpath);
                 DBG("\t\tphys                 %s\n",d->u.input.phys?d->u.input.phys:"(NULL)");
                 DBG("\t\tuniq                 %s\n",d->u.input.uniq?d->u.input.uniq:"(NULL)");
                 DBG("\t\tdevice               %s\n",d->u.input.device?d->u.input.device:"(NULL)");
                 DBG("\t\tbustype              %02x (%s)\n", d->u.input.bustype,input_bustype(d->u.input.bustype));
                 DBG("\t\tvendor               %04x\n", d->u.input.vendor);
                 DBG("\t\tproduct              %04x\n", d->u.input.product);
                 DBG("\t\tversion              %04x\n", d->u.input.version);

                 if (d->u.input.busdevice) {
                    DBG("\t\tkernelname           %s\n",(d->u.input.busdevice)->kernelname?(d->u.input.busdevice)->kernelname:"(NULL)");
                    //V("\t\tphys_path            %s\n",dev->phys_path?dev->phys_path:"(NULL)");
                    }
                 break;
            case ACPI:
            case BDI:
            case IEEE1394:
            case IEEE1394_NODE:
            case BLOCK:
            case BLUETOOTH:
            case BSG:
            case CPUID:
            case DMI:
            case DVB:
            case GRAPHICS:
            case HID:
            case HIDRAW:
            case I2C:
            case IDE_PORT:
            case MDIO_BUS:
            case MEM:
            case MISC:
            case MSR:
            case NET:
            case PARPORT:
            case PCI_BUS:
            case PCI_EXPRESS:
            case PLATFORM:
            case PNP:
            case PPDEV:
            case PPP:
            case PRINTER:
            case RC:
            case RFKILL:
            case RS232:
            case RTC:
            case SCSI:
            case SCSI_DEVICE:
            case SCSI_DISK:
            case SCSI_GENERIC:
            case SCSI_HOST:
            case SERIO:
            case SOUND:
            case THERMAL:
            case TTY:
            case USB_DEVICE:
            case USBMON:
            case VC:
            case VTCONSOLE:
            case VIDEO4LINUX:
                 break;
            default:;
                 DBG("unknown bus %x", d->bus);
            }
      if (d->database) {
         db_t * db = d->database;
         items++;
         V("\t\tname                %s\n",db->name?db->name:"(NULL)");
         V("\t\tcaps                %s\n",db->caps?db->caps:"(NULL)");
         V("\t\tdriverpackage       %s\n",db->driverpackage?db->driverpackage:"(NULL)");
         V("\t\tdrivers             %s\n",db->drivers?db->drivers:"(NULL)");
         V("\t\tplugin              %s\n",db->plugin?db->plugin:"(NULL)");
         }
      }
   return items;
}

static const struct option long_options[] = {     
  { "config",       1, 0, 'C' },
  { "showtree",     0, 0, 'T' },
  { "quite",        0, 0, 'q' },
  { "bus",          1, 0, 'b' },
  { "capability",   1, 0, 'c' },
  { "driver",       1, 0, 'd' },
  { "loaddriver",   0, 0, 'l' },
  { "package",      1, 0, 'p' },
  { "plugin",       1, 0, 'P' },
  { "strlen",       1, 0, 's' },
  { "type",         1, 0, 't' },
  { "unloaddriver", 0, 0, 'u' },
  { "verbose",      0, 0, 'v' },
  { "help",         0, 0, 'h' },
  { 0, 0, 0, 0 }
};

void show_help(const char * opt) {
  if (opt)
     I("ERROR: bad usage of option '%s'\n", opt);

  I("%s\n", "usage: hdet [options...]");
  I("%s\n", "  -C <CONFIG>, --config <CONFIG>");
  I("%s\n", "                       pass path and name of hdet database");
  I("%s\n", "                       i.e. -C /usr/share/hdet/database.bin");
  I("%s\n", "  -b <BUS>, --bus <BUS>");
  I("%s\n", "                       scan only bus <BUS>, where BUS is one of");
  I("%s\n", "                       USB");
  I("%s\n", "                       PCI");
  I("%s\n", "                       INPUT");
  I("%s\n", "  -c <CAPABILITY>, --capability <CAPABILITY>");
  I("%s\n", "                       define capabilities to scan for");
  I("%s\n", "  -d <DRIVER>, --driver <DRIVER>");
  I("%s\n", "                       scan for hardware supported by DRIVER");
  I("%s\n", "  -l, --loaddriver");
  I("%s\n", "                       auto load driver(s)");
  I("%s\n", "  -p <PACKAGE>, --package <PACKAGE>");
  I("%s\n", "                       scan for driver package");                   
  I("%s\n", "  -P <PLUGIN>, --plugin <PLUGIN>");
  I("%s\n", "                       scan for hardware supported by VDR plugins");
  I("%s\n", "  -s N, --strlen N");
  I("%s\n", "                       compare only the first N characters for");
  I("%s\n", "                       pattern matching");
  I("%s\n", "  -t <TYPE>, --type <TYPE>");
  I("%s\n", "                       define hardware type");
  I("%s\n", "                       VGA      : VGA cards");
  I("%s\n", "                       AUDIO    : Audio devices");
  I("%s\n", "                       MEDIA    : Video and DVB devices");
//I("%s\n", "                       ETH      : Ethernet adapters  (FORESEEN, BUT NOT YET IMPLEMENTED)");
  I("%s\n", "  -u, --unloaddriver");
  I("%s\n", "                       auto unload driver(s)");
  I("%s\n", "  -T, --showtree       show device tree");  
  I("%s\n", "  -v, --verbose        increase verbosity");
  I("%s\n", "  -q, --quite          reduce verbosity");
  I("%s\n", "  -h, --help           view help");
}

int main(int argc, char *argv[]) {
  char c;
  char * db = 0;
  scantype_t * scantype = 0;
  uint16_t slen = 256;
  int items = 0;
  int showtree = 0;

  if (asprintf(&fullpath,"PATH=%s:%s", DEFAULTPATH, get_current_dir_name()) <= 0)
     W("couldnt read $PATH\n");

  while ((c = getopt_long(argc, argv, "b:c:d:lp:s:t:uqvhC:P:T", long_options, NULL)) != EOF) {
      switch(c) {
            /* lower case */
            case 'b':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                if     (! strcasecmp(optarg, "USB"))   scantype->bus = USB;
                else if(! strcasecmp(optarg, "PCI"))   scantype->bus = PCI;
                else if(! strcasecmp(optarg, "INPUT")) scantype->bus = INPUT;
                else E("unknown bus '%s'\n", optarg);
                break;
            case 'c':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                scantype->capability = strdup(optarg);
                break;
            case 'd':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                scantype->driver = strdup(optarg);
                break;
            case 'l':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                scantype->load = 1;
                break;
            case 'p':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                scantype->package = strdup(optarg);
                break;
            case 's':
                slen = atoi(optarg);
                break;
            case 't':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                if     (! strcasecmp(optarg, "VGA"))   scantype->type = VGA;
                else if(! strcasecmp(optarg, "AUDIO")) scantype->type = AUDIO;
                else if(! strcasecmp(optarg, "MEDIA")) scantype->type = MEDIA;
                else if(! strcasecmp(optarg, "ETH"))   scantype->type = ETHERNET;
                else { show_help("type"); free(scantype); return -1; }
                break;
            case 'u':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                scantype->load = -1;
                break;
            case 'q':
                if (verbose > 0)
                   verbose--;
                break;
            case 'v':
                verbose++;
                break;
            case 'h':
                show_help(0);
                if(db) free(db);
                return 0;
            /* upper case */
            case 'C':
                if (db) free(db), db = strdup(optarg);
                break;
            case 'P':
                if (!scantype) scantype = calloc(1,sizeof(scantype_t));
                scantype->plugin = strdup(optarg);
                break;
            case 'T':
                showtree = 1;
                break;
            default:
                show_help(0);
                if(db) free(db);
                return -1;
            }
      }

  if (db) {
     getdb(db);
     free(db);
     }
  else
     getdb(dbp());

  rd_sys_devices(0,0);

  if (showtree)
     show_device_tree(&bus_devices);
  else {
     associate_input_devices(&bus_devices);
     // danger: sorting breaks child and parent pointers.
     sort_bus_devices(&bus_devices);
     items = list_devices(scantype, slen);
     }

  return items;
}

