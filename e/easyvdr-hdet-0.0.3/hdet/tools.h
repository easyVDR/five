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

#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdint.h>
#include "hdet.h"
#include "list.h"



/******************************************************************************
 * typedefs && macros.
 *****************************************************************************/
#define DEFAULTPATH "/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/X11R6/bin"

#define SUBUNKNOWN    (uint32_t)0x00000000
#define IEEE1394      (uint32_t)0x31333934
#define IEEE1394_NODE (uint32_t)0x3133394E
#define ACPI          (uint32_t)0x41435049
#define BDI           (uint32_t)0x42444900
#define BLOCK         (uint32_t)0x424C434B
#define BLUETOOTH     (uint32_t)0x424C5545
#define BSG           (uint32_t)0x42534700
#define CPUID         (uint32_t)0x43505549
#define DMI           (uint32_t)0x434D4900
#define DVB           (uint32_t)0x44564200
#define GRAPHICS      (uint32_t)0x47524148
#define HID           (uint32_t)0x48494400
#define HIDRAW        (uint32_t)0x48494452
#define I2C           (uint32_t)0x49324300
#define IDE_PORT      (uint32_t)0x49444550
#define INPUT         (uint32_t)0x494E5055
#define MDIO_BUS      (uint32_t)0x4D434255
#define MEM           (uint32_t)0x4D454D00
#define MISC          (uint32_t)0x4D495343
#define MSR           (uint32_t)0x4D535200
#define NET           (uint32_t)0x4E455400
#define PARPORT       (uint32_t)0x50415250
#define PCI           (uint32_t)0x50434900
#define PCI_BUS       (uint32_t)0x50434942
#define PCI_EXPRESS   (uint32_t)0x50434945
#define PLATFORM      (uint32_t)0x504C4154
#define PNP           (uint32_t)0x504E5000
#define PPDEV         (uint32_t)0x50504445
#define PPP           (uint32_t)0x50505000
#define PRINTER       (uint32_t)0x5052494E
#define RC            (uint32_t)0x52430000
#define RFKILL        (uint32_t)0x52464B49
#define RS232         (uint32_t)0x52533233
#define RTC           (uint32_t)0x52544300
#define SCSI          (uint32_t)0x53430000
#define SCSI_DEVICE   (uint32_t)0x53434445
#define SCSI_DISK     (uint32_t)0x5343444B
#define SCSI_GENERIC  (uint32_t)0x53434745
#define SCSI_HOST     (uint32_t)0x5343484F
#define SERIO         (uint32_t)0x5345494F
#define SOUND         (uint32_t)0x534F554E
#define THERMAL       (uint32_t)0x54484552
#define TTY           (uint32_t)0x54545900
#define USB           (uint32_t)0x55534200
#define USB_DEVICE    (uint32_t)0x55534244
#define USBMON        (uint32_t)0x5553424D
#define VC            (uint32_t)0x56430000
#define VTCONSOLE     (uint32_t)0x56434F4E
#define VIDEO4LINUX   (uint32_t)0x56344C00


int show_device_tree(struct list_head * devices);

const char * usb_class   (uint8_t deviceClass);
const char * usb_subclass(uint8_t deviceclass, uint8_t subclass, uint8_t protocol);

const char * pci_class   (uint8_t deviceClass);
const char * pci_subclass(uint32_t class_id);

const char * input_bustype(uint8_t bus);

uint32_t get_subsystem(const char * path);
void get_short_desc(device_t * d, char * buf);

void rd_link(const char * link, char * buf);

int loadDriver  (const char * driver);
int unloadDriver(const char * driver);

void sort_bus_devices(struct list_head *head);

extern char * fullpath;

#endif
