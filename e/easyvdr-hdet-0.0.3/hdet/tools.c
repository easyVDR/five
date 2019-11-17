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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <netinet/in.h>
#include "tools.h"

char * fullpath;

const char * usb_subclass(uint8_t deviceclass, uint8_t subclass, uint8_t protocol) {
  uint32_t i = deviceclass<<16 | subclass<<8 | protocol;
  switch(i) {
        case 0x010100 ... 0x0101ff:  return "Control Device"; 
        case 0x010200 ... 0x0102ff:  return "Streaming";
        case 0x010300 ... 0x0103ff:  return "MIDI Streaming";  
        case 0x020100 ... 0x0201ff:  return "Direct Line";
        case 0x020200:               return "Modem"; 
        case 0x020201:               return "v.25 AT Modem";  
        case 0x020202:               return "PCCA101 AT Modem"; 
        case 0x020203:               return "PCCA101 AT Modem w. wakeup"; 
        case 0x020204:               return "GSM Modem";
        case 0x020205:               return "3G Modem";
        case 0x020206:               return "CDMA Modem";
        case 0x0202fe:               return "Modem";
        case 0x0202ff:               return "Modem";
        case 0x020300 ... 0x0203ff:  return "Telephone"; 
        case 0x020400 ... 0x0204ff:  return "Multi-Channel";
        case 0x020500 ... 0x0205ff:  return "CAPI Control"; 
        case 0x020600 ... 0x0206ff:  return "Ethernet Networking"; 
        case 0x020700 ... 0x0207ff:  return "ATM Networking"; 
        case 0x020800 ... 0x0208ff:  return "Wireless Handset Control"; 
        case 0x020900 ... 0x0209ff:  return "Device Management"; 
        case 0x020a00 ... 0x020aff:  return "Mobile Direct Line"; 
        case 0x020b00 ... 0x020bff:  return "OBEX"; 
        case 0x020c00 ... 0x020cff:  return "Ethernet Emulation";
        case 0x030000:               return "none";
        case 0x030001:               return "Keyboard";
        case 0x030002:               return "Mouse";
        case 0x030100:               return "Boot Interface";
        case 0x030101:               return "Boot Interface/Keyboard";
        case 0x030102:               return "Boot Interface/Mouse";
        case 0x060101:               return "Still Image Capture";
        case 0x070101:               return "Unidirectional";
        case 0x070102:               return "Bidirectional";
        case 0x070103:               return "bidir IEEE1284.4 compatible";
        case 0x0701ff:               return "Vendor Specific";
        case 0x080100 ... 0x0801ff:  return "RBC";
        case 0x080200 ... 0x0802ff:  return "MMC-2/ATAPI";
        case 0x080300 ... 0x0803ff:  return "QIC-157"; 
        case 0x080400 ... 0x0804ff:  return "Floppy";  
        case 0x080500 ... 0x0805ff:  return "SFF-8070i"; 
        case 0x080600 ... 0x0806ff:  return "SCSI";
        case 0x090000:               return "root hub";
        case 0x090001:               return "hi-speed with single TT";
        case 0x090002:               return "hi-speed hub with multiple TTs";
        case 0x0a0030:               return "I.430 ISDN BRI"; 
        case 0x0a0031:               return "HDLC"; 
        case 0x0a0032:               return "Transparent"; 
        case 0x0a0050:               return "Q.921M";  
        case 0x0a0051:               return "Q.921"; 
        case 0x0a0052:               return "Q.921TM"; 
        case 0x0a0090:               return "V.42bis"; 
        case 0x0a0091:               return "Q.932 EuroISDN"; 
        case 0x0a0092:               return "V.120 V.24 rate ISDN"; 
        case 0x0a0093:               return "CAPI 2.0"; 
        case 0x0a00fd:               return "Host Based Driver"; 
        case 0x0a00fe:               return "CDC PUF"; 
        case 0x0a00ff:               return "Vendor specific"; 
        case 0x0e0000 ... 0x0e00ff:  return "Undefined";
        case 0x0e0100 ... 0x0e01ff:  return "Video Control"; 
        case 0x0e0200 ... 0x0e02ff:  return "Video Streaming"; 
        case 0x0e0300 ... 0x0e03ff:  return "Video Interface Collection";
        case 0xe00101:               return "Bluetooth"; 
        case 0xe00102:               return "UWB Radio"; 
        case 0xe00103:               return "remote NDIS";
        case 0xe00104:               return "Bluetooth AMP"; 
        case 0xe00201:               return "host wire";
        case 0xe00202:               return "device wire";
        case 0xe00203:               return "device wire isochronous";
        default:  return "unknown";
        }
}

const char * usb_class(uint8_t deviceClass) {
  const char * classes[] = {
        "Defined at Interface level",            // 0
        "Audio Device",                          // 1  
        "Communication  and CDC Control Device", // 2
        "HID (Human Interface Device)",          // 3
        "Physical Interface Device",             // 5
        "Imaging Device",                        // 6
        "Printer",                               // 7 
        "Mass Storage Device",                   // 8  
        "Hub",                                   // 9
        "CDC-Data",                              // a
        "SmartCard",                             // b  
        "Content Security Device",               // d  
        "Video Device",                          // e
        "Personal Healthcare Device",            // f
        "Diagnostic Device",                     // dc
        "Wireless Controller",                   // e0
        "Misc Device",                           // ef
        "Application Specific Device",           // fe
        "Vendor Specific Device",                // ff
        "(undefined device class)",              // wtf..?
        };
  switch(deviceClass) {
        case 0 ... 3:     return classes[deviceClass];
        case 5 ... 0xb:   return classes[deviceClass-1];
        case 0xd ... 0xf: return classes[deviceClass-2];
        case 0xdc:        return classes[14];
        case 0xe0:        return classes[15];
        case 0xef:        return classes[16];
        case 0xfe:        return classes[17];
        case 0xff:        return classes[18];
        default:          return classes[19];
        }
}

const char * pci_subclass(uint32_t class_id) {
  switch(class_id) {
        case 0x000000 ... 0x0000ff:  return "PCI-1.x Non-VGA"; 
        case 0x000100 ... 0x0001ff:  return "PCI-1.x VGA"; 
        case 0x010000 ... 0x0100ff:  return "SCSI";
        case 0x010100 ... 0x0101ff:  return "IDE";
        case 0x010200 ... 0x0102ff:  return "Floppy disk";
        case 0x010300 ... 0x0103ff:  return "IPI bus";
        case 0x010400 ... 0x0104ff:  return "RAID bus";
        case 0x010500 ... 0x0105ff:  return "ATA";
        case 0x010600:               return "SATA";
        case 0x010601:               return "SATA (AHCI)";
        case 0x010602 ... 0x0106ff:  return "SATA";
        case 0x010700 ... 0x0107ff:  return "Serial Attached SCSI";
        case 0x018000 ... 0x0180ff:  return "Other";
        case 0x020000 ... 0x0200ff:  return "Ethernet";
        case 0x020100 ... 0x0201ff:  return "Token ring";
        case 0x020200 ... 0x0202ff:  return "FDDI";
        case 0x020300 ... 0x0203ff:  return "ATM";
        case 0x020400 ... 0x0204ff:  return "ISDN";
        case 0x020500 ... 0x0205ff:  return "WorldFip";
        case 0x020600 ... 0x0206ff:  return "PICMG";
        case 0x028000 ... 0x0280ff:  return "Other";
        case 0x030000:               return "VGA";
        case 0x030001:               return "VGA/8514";
        case 0x030100 ... 0x0301ff:  return "XGA";
        case 0x030200 ... 0x0302ff:  return "3D";
        case 0x038000 ... 0x0380ff:  return "Other";
        case 0x040000 ... 0x0400ff:  return "Video";
        case 0x040100 ... 0x0401ff:  return "Audio";
        case 0x040200 ... 0x0402ff:  return "Telephony";
        case 0x040300 ... 0x0403ff:  return "Audio";
        case 0x048000 ... 0x0480ff:  return "Other";
        case 0x050000 ... 0x0500ff:  return "RAM";
        case 0x050100 ... 0x0501ff:  return "FLASH";
        case 0x058000 ... 0x0580ff:  return "Other";
        case 0x060000 ... 0x0600ff:  return "Host";
        case 0x060100 ... 0x0601ff:  return "ISA";
        case 0x060200 ... 0x0602ff:  return "EISA";
        case 0x060300 ... 0x0603ff:  return "Micro Channel";
        case 0x060400:               return "PCI";
        case 0x060401:               return "PCI Subtractive decode";
        case 0x060402 ... 0x0604ff:  return "PCI";
        case 0x060500 ... 0x0605ff:  return "PCMCIA";
        case 0x060600 ... 0x0606ff:  return "NuBus";
        case 0x060700 ... 0x0607ff:  return "CardBus";
        case 0x060800:               return "transparent RACEway";
        case 0x060801:               return "RACEway Endpoint";
        case 0x060802 ... 0x0608ff:  return "RACEway";
        case 0x060900 ... 0x0609ff:  return "Semi-transparent PCI-PCI";
        case 0x060a00 ... 0x060aff:  return "InfiniBand to PCI host";
        case 0x068000 ... 0x0680ff:  return "Other";
        case 0x070000:               return "Serial XT-compat.";
        case 0x070001:               return "Serial 16450";
        case 0x070002:               return "Serial 16550";
        case 0x070003:               return "Serial 16650";
        case 0x070004:               return "Serial 16750";
        case 0x070005:               return "Serial 16850";
        case 0x070006:               return "Serial 16950";
        case 0x070007 ... 0x0700ff:  return "Serial";
        case 0x070100:               return "SPP Parport";
        case 0x070101:               return "bi-dir Parport";
        case 0x070102:               return "ECP Parport";
        case 0x070103:               return "IEEE 1284 Parport";
        case 0x070104 ... 0x0701fd:  return "Parport";
        case 0x0701fe:               return "IEEE 1284 Target Parport";
        case 0x0701ff:               return "Parport";
        case 0x070200:               return "Multiport serial";
        case 0x070300:               return "Modem";
        case 0x070301:               return "16450 Modem";
        case 0x070302:               return "16550 Modem";
        case 0x070303:               return "16650 Modem";
        case 0x070304:               return "16750 Modem";
        case 0x070305 ... 0x0703ff:  return "Modem";
        case 0x070400 ... 0x0704ff:  return "GPIB";
        case 0x070500 ... 0x0705ff:  return "Smard Card";
        case 0x078000 ... 0x0780ff:  return "Other";
        case 0x080000:               return "8259 PIC";
        case 0x080001:               return "ISA PIC";
        case 0x080002:               return "EIS APIC";
        case 0x080003 ... 0x08000f:  return "PIC";
        case 0x080010:               return "IO-APIC";
        case 0x080011 ... 0x08001f:  return "PIC";
        case 0x080020:               return "IOx-APIC";
        case 0x080021 ... 0x0800ff:  return "PIC";
        case 0x080100:               return "8237 DMA";
        case 0x080101:               return "ISA DMA";
        case 0x080102:               return "EISA DMA";
        case 0x080103 ... 0x0801ff:  return "DMA";
        case 0x080200:               return "8254 Timer";
        case 0x080201:               return "ISA Timer";
        case 0x080202:               return "EISA Timer";
        case 0x080203 ... 0x0802ff:  return "Timer";
        case 0x080300:               return "RTC";
        case 0x080301:               return "ISA RTC";
        case 0x080302 ... 0x0803ff:  return "RTC";
        case 0x080400 ... 0x0804ff:  return "PCI Hot-Plug";
        case 0x080500 ... 0x0805ff:  return "SD Host";
        case 0x088000 ... 0x0880ff:  return "Other";
        case 0x090000 ... 0x0900ff:  return "Keyboard";
        case 0x090100 ... 0x0901ff:  return "Digitizer Pen";
        case 0x090200 ... 0x0902ff:  return "Mouse";
        case 0x090300 ... 0x0903ff:  return "Scanner";
        case 0x090400 ... 0x0904ff:  return "Gameport";
        case 0x098000 ... 0x0980ff:  return "Other";
        case 0x0a0000 ... 0x0affff:  return "Docking Station";
        case 0x0b0000 ... 0x0bffff:  return "Processor";
        case 0x0c0000 ... 0x0c000f:  return "FireWire";
        case 0x0c0010:               return "FireWire OHCI";
        case 0x0c0011 ... 0x0c00ff:  return "FireWire";
        case 0x0c0100 ... 0x0c01ff:  return "ACCESS Bus";
        case 0x0c0200 ... 0x0c02ff:  return "SSA";
        case 0x0c0300:               return "UHCI USB-1.x";
        case 0x0c0301 ... 0x0c030f:  return "USB";
        case 0x0c0310:               return "OHCI USB-1.x";
        case 0x0c0311 ... 0x0c031f:  return "USB";
        case 0x0c0320:               return "EHCI USB-2.0";
        case 0x0c0321 ... 0x0c032f:  return "USB";
        case 0x0c0330:               return "XHCI USB-3.0";
        case 0x0c0331 ... 0x0c03fd:  return "USB";
        case 0x0c03fe:               return "USB Device";
        case 0x0c03ff:               return "USB";
        case 0x0c0400 ... 0x0c04ff:  return "Fibre Channel";
        case 0x0c0500 ... 0x0c05ff:  return "SMBus";
        case 0x0c0600 ... 0x0c06ff:  return "InfiniBand";
        case 0x0c0700 ... 0x0c07ff:  return "IPMI SMIC";
        case 0x0c0800 ... 0x0c08ff:  return "SERCOS";
        case 0x0c0900 ... 0x0c09ff:  return "CANBUS";
        case 0x0d0000 ... 0x0d00ff:  return "IRDA";
        case 0x0d0100 ... 0x0d01ff:  return "Consumer IR";
        case 0x0d0200 ... 0x0d0fff:  return "Wireless";
        case 0x0d1000 ... 0x0d10ff:  return "RF";
        case 0x0d1100 ... 0x0d11ff:  return "Bluetooth";
        case 0x0d1200 ... 0x0d12ff:  return "Broadband";
        case 0x0d1300 ... 0x0d1fff:  return "Wireless";
        case 0x0d2000 ... 0x0d20ff:  return "802.1a";
        case 0x0d2100 ... 0x0d21ff:  return "802.1b";
        case 0x0d2200 ... 0x0dffff:  return "Wireless";
        case 0x0e0000 ... 0x0effff:  return "Intelligent IO";
        case 0x0f0100 ... 0x0f01ff:  return "TV";
        case 0x0f0200 ... 0x0f02ff:  return "Audio";
        case 0x0f0300 ... 0x0f03ff:  return "Voice";
        case 0x0f0400 ... 0x0f04ff:  return "Data";
        case 0x100000 ... 0x1000ff:  return "Network/Computing";
        case 0x101000 ... 0x1010ff:  return "Entertainment";
        case 0x110000 ... 0x1100ff:  return "DPIO module";
        case 0x110100 ... 0x1101ff:  return "Performance counters";
        case 0x111000 ... 0x1110ff:  return "Communication synchronizer";
        case 0x112000 ... 0x1120ff:  return "Signal processing management";
        case 0x118000 ... 0x1180ff:  return "Signal processing";
        default: return "unknown";
        }
}

const char * pci_class(uint8_t deviceClass) {
  const char * classes[] = {
        "Unclassified device",                   // 00
        "Mass storage controller",               // 01
        "Network controller",                    // 02
        "Display controller",                    // 03
        "Multimedia controller",                 // 04
        "Memory controller",                     // 05
        "Bridge device",                         // 06
        "Communication controller",              // 07
        "System peripheral",                     // 08
        "Input device controller",               // 09
        "Docking station",                       // 0A
        "Processor",                             // 0B
        "Serial bus controller",                 // 0C
        "Wireless controller",                   // 0D
        "Intelligent controller",                // 0E
        "Satellite communications controller",   // 0F
        "Encryption controller",                 // 10
        "Signal processor",                      // 11
        };
  if (deviceClass <= 0x11)
     return classes[deviceClass];
  else
     return "(undefined device class)";
}

const char * input_bustype(uint8_t bus) {
  const char * busses[] = {
        "PCI",                                   // 01
        "ISAPNP",                                // 02
        "USB",                                   // 03
        "HIL",                                   // 04
        "BLUETOOTH",                             // 05
        "VIRTUAL",                               // 06
        "ISA",                                   // 10
        "I8042",                                 // 11
        "XTKBD",                                 // 12
        "RS232",                                 // 13
        "GAMEPORT",                              // 14
        "PARPORT",                               // 15
        "AMIGA",                                 // 16
        "ADB",                                   // 17
        "I2C",                                   // 18
        "HOST",                                  // 19
        "GSC",                                   // 1A
        "ATARI",                                 // 1B
        };
  switch (bus) {
     case 0x01 ... 0x06:
          return busses[bus-1];
     case 0x10 ... 0x1B:
          return busses[bus-10];
     default:
          return "(undefined bus)";
     }
}


uint32_t get_subsystem(const char * path) {
  char * subsystem_path;
  char * p;
  uint32_t system = SUBUNKNOWN;

  asprintf(&subsystem_path, "%s/%s", path, "subsystem");
    
  p = calloc(256,1);
  rd_link(subsystem_path, p);

  if (*p) {
  #define SUBSYS(a,A) else if (!strcasecmp(p, a)) system = A
  if      (!strcasecmp(p, "unknown"))      system = SUBUNKNOWN;
  
  SUBSYS("usb"               , USB);
  SUBSYS("pci"               , PCI);
  SUBSYS("ieee1394"          , IEEE1394);
  SUBSYS("ieee1394_node"     , IEEE1394_NODE);
  SUBSYS("pci_bus"           , PCI_BUS);
  SUBSYS("pci_express"       , PCI_EXPRESS);
  SUBSYS("i2c"               , I2C);
  SUBSYS("input"             , INPUT);
  SUBSYS("sound"             , SOUND);
  SUBSYS("usb_device"        , USB_DEVICE);
  SUBSYS("scsi"              , SCSI);
  SUBSYS("scsi_host"         , SCSI_HOST);
  SUBSYS("scsi_disk"         , SCSI_DISK);
  SUBSYS("scsi_device"       , SCSI_DEVICE);
  SUBSYS("scsi_generic"      , SCSI_GENERIC);
  SUBSYS("block"             , BLOCK);
  SUBSYS("bsg"               , BSG);
  SUBSYS("net"               , NET);
  SUBSYS("hid"               , HID);
  SUBSYS("hidraw"            , HIDRAW);
  SUBSYS("dvb"               , DVB);
  SUBSYS("rc"                , RC);
  SUBSYS("video4linux"       , VIDEO4LINUX);
  SUBSYS("acpi"              , ACPI);
  SUBSYS("pnp"               , PNP);
  SUBSYS("rtc"               , RTC);
  SUBSYS("tty"               , TTY);
  SUBSYS("ide_port"          , IDE_PORT);
  SUBSYS("platform"          , PLATFORM);
  SUBSYS("serio"             , SERIO);
  SUBSYS("vtconsole"         , VTCONSOLE);
  SUBSYS("msr"               , MSR);
  SUBSYS("bdi"               , BDI);
  SUBSYS("mem"               , MEM);
  SUBSYS("vc"                , VC);
  SUBSYS("misc"              , MISC);
  SUBSYS("cpuid"             , CPUID);
  SUBSYS("graphics"          , GRAPHICS);
  SUBSYS("thermal"           , THERMAL);
  SUBSYS("mdio_bus"          , MDIO_BUS);
  SUBSYS("dmi"               , DMI);
  SUBSYS("ppp"               , PPP);
  SUBSYS("usbmon"            , USBMON);
  SUBSYS("bluetooth"         , BLUETOOTH);
  SUBSYS("rfkill"            , RFKILL);
  SUBSYS("ppdev"             , PPDEV);
  SUBSYS("printer"           , PRINTER);

  else DBG("%s %d: unknown subsystem %s\n", __FUNCTION__, __LINE__, p);
  }
  free(p);
  return system;
}



void rd_link(const char * link, char * buf) {
  char * buffer = calloc(256,1);
  struct stat os;
  char * p = 0;

  if (buf) *buf = 0;

  if (! link || ! *link) {
     free(buffer);
     return;
     }

  // DBG("%s %d: link = '%s'\n", __FUNCTION__, __LINE__, link);

  if (lstat(link, &os) >= 0 &&  readlink(link,buffer,256) > 0) {
     // DBG("%s %d: buffer = '%s'\n", __FUNCTION__, __LINE__, buffer);
     p = (strrchr(buffer, '/'));
     if (p) {
        p = p + 1;
        if (p && *p) {
           // DBG("%s %d: p = '%s'\n", __FUNCTION__, __LINE__, p);
           strcpy(buf,p);
           }
        }
     }
  free(buffer);
}

/* path_expansion: expand path for executing shell to 'DEFAULTPATH:$(pwd)'
 * no_hang       : dont suspend execution of the calling thread if status is not immediately available 
 */
int run_in_shell(const char * command, int path_expansion, int no_hang, int warn) {
  pid_t pid = fork();
  char * full_cmd;
  int retval = 0;

  if (path_expansion)
     asprintf(&full_cmd,"%s;%s",fullpath,command);
  else
     asprintf(&full_cmd,"%s",command);

  if (pid < 0) { // oops -> no child
     free(full_cmd);
     E("no child: %s\n", strerror(errno));
     }
  else if (pid > 0) { // executed in main programs context, waiting for child
     int child_status = 0;

     if (waitpid(pid, &child_status, no_hang) < 0) {
        free(full_cmd);
        E("waitpid failed: %s\n", strerror(errno));
        }
     if (WIFEXITED(child_status)) {
        retval = WEXITSTATUS(child_status);
        if (retval && warn)
           W("process terminated normally with return value %d\n", retval);
        }
     if (WIFSIGNALED(child_status)) {
        retval = -1;
        if (warn)
           W("child process terminated due to signal %d\n", WTERMSIG(child_status)); 
        }
     if (WIFSTOPPED(child_status)) {
        retval = -1;
        if (warn)
           W("child process is stopped by signal %d\n",WSTOPSIG(child_status)); 
        } 
     if (WIFCONTINUED(child_status)) {
        retval = -1; 
        if (warn)
           W("child process continued from a job control stop\n");
        }
     }
  else { // executed in child process
    if (execl("/bin/sh", "sh", "-c", full_cmd, NULL) == -1)
       E("%s:%s\n", command, strerror(errno));
    exit(retval);
    }
  free(full_cmd);
  return retval;
}

int loadDriver(const char * driver) {
  char * cmdline = calloc(strlen("modprobe --quiet ") + strlen(driver) + 1, 1);
  int result = 0;

  sprintf(cmdline, "%s %s", "modprobe --quiet", driver);
  result = run_in_shell(cmdline, 0, 0, 0);
  free(cmdline);
  return result;
}

int unloadDriver(const char * driver) {
  char * cmdline = calloc(strlen("modprobe --quiet --remove") + strlen(driver) + 1, 1);
  int result = 0;

  sprintf(cmdline, "%s %s", "modprobe --quiet --remove", driver);
  result = run_in_shell(cmdline, 0, 0, 0);
  free(cmdline);
  return result;
}

void get_short_desc(device_t * d, char * buf) {
      switch(d->bus) {
            case PCI:
                 sprintf(buf, "PCI %s %s %04X:%04X %04X:%04X",
                   pci_subclass(d->u.pci.class_id), pci_class((d->u.pci.class_id >> 16) & 0xff), 
                   d->u.pci.vendor_id, d->u.pci.device_id, d->u.pci.subvendor_id, d->u.pci.subdevice_id);
                 break;
            case USB:
                 sprintf(buf, "USB %s %s %04X:%04X",
                   usb_subclass(d->u.usb.bDeviceClass, d->u.usb.bDeviceSubClass, d->u.usb.bDeviceProtocol),
                   usb_class(d->u.usb.bDeviceClass),
                   d->u.usb.idVendor, d->u.usb.idProduct);
            default:;
            }
}


static void ltree(device_t * d, int col) {
  char * spaces = calloc(3*col + 1, 1);
  memset(spaces, 32, 3*col);
  char * buf = calloc(256,1);
  int i = 0;

  get_short_desc(d,buf);

  I("%s/%s %s\n", spaces, d->kernelname, buf);

  while (d->childs[i] && i < MAX_DEVICE_CHILDS) {
        // dont print some items to avoid loosing readability.
        if (! strcasecmp((d->childs[i])->kernelname,"power") ||
            ! strcasecmp((d->childs[i])->kernelname,"capabilities") ||
            ! strcasecmp((d->childs[i])->kernelname,"statistics") ||
            ! strncasecmp((d->childs[i])->kernelname,"ep_",3) ||
            ! strncasecmp((d->childs[i])->kernelname,"queue",5) ) {
           i++;
           continue;
           }
        I("   %s|\n", spaces);
        ltree(d->childs[i],col + 1);
        i++;
        }
  free(spaces);
  free(buf);
}

int show_device_tree(struct list_head * devices) {
   struct list_head *p;
   device_t *d;
   int items = 0;
   
   list_for_each(p, devices) {
      d = list_entry(p, device_t, list);

      items++;
      if (d->parent)
         continue;

      ltree(d,0);
      }

  I("%d items\n", items);
}




/*
 * Combine final list merge with restoration of standard doubly-linked
 * list structure.  This approach duplicates code from merge(), but
 * runs faster than the tidier alternatives of either a separate final
 * prev-link restoration pass, or maintaining the prev links
 * throughout.
 */
static void merge_and_restore_back_links(void *priv,
				int (*cmp)(void *priv, struct list_head *a,
					struct list_head *b),
				struct list_head *head,
				struct list_head *a, struct list_head *b)
{
	struct list_head *tail = head;

	while (a && b) {
		/* if equal, take 'a' -- important for sort stability */
		if ((*cmp)(priv, a, b) <= 0) {
			tail->next = a;
			a->prev = tail;
			a = a->next;
		} else {
			tail->next = b;
			b->prev = tail;
			b = b->next;
		}
		tail = tail->next;
	}
	tail->next = a ? : b;

	do {
		/*
		 * In worst cases this loop may run many iterations.
		 * Continue callbacks to the client even though no
		 * element comparison is needed, so the client's cmp()
		 * routine can invoke cond_resched() periodically.
		 */
		(*cmp)(priv, tail->next, tail->next);

		tail->next->prev = tail;
		tail = tail->next;
	} while (tail->next);

	tail->next = head;
	head->prev = tail;
}


/*
 * COPYRIGHT NOTE:
 *    the source code of these functions is taken from linux-2.6.38.6 (www.kernel.org),
 *    files 'lib/list_sort.c' and 'lib/list.h':
 *
 *     i)   merge
 *     ii)  list_sort
 *     iii) merge_and_restore_back_links
 *
 * (GNU GENERAL PUBLIC LICENSE Version 2, June 1991)
 *
 * for copyright on those functions please refer to the copyright informations of the
 * original authors (Linus Torvalds and others).
 */

#define MAX_LIST_LENGTH_BITS 20

/*
 * Returns a list organized in an intermediate format suited
 * to chaining of merge() calls: null-terminated, no reserved or
 * sentinel head node, "prev" links not maintained.
 */
static struct list_head *merge(void *priv,
				int (*cmp)(void *priv, struct list_head *a,
					struct list_head *b),
				struct list_head *a, struct list_head *b)
{
	struct list_head head, *tail = &head;

	while (a && b) {
		/* if equal, take 'a' -- important for sort stability */
		if ((*cmp)(priv, a, b) <= 0) {
			tail->next = a;
			a = a->next;
		} else {
			tail->next = b;
			b = b->next;
		}
		tail = tail->next;
	}
	tail->next = a?:b;
	return head.next;
}

/**
 * list_sort - sort a list
 * @priv: private data, opaque to list_sort(), passed to @cmp
 * @head: the list to sort
 * @cmp: the elements comparison function
 *
 * This function implements "merge sort", which has O(nlog(n))
 * complexity.
 *
 * The comparison function @cmp must return a negative value if @a
 * should sort before @b, and a positive value if @a should sort after
 * @b. If @a and @b are equivalent, and their original relative
 * ordering is to be preserved, @cmp must return 0.
 */
void list_sort(void *priv, struct list_head *head,
		int (*cmp)(void *priv, struct list_head *a,
			struct list_head *b))
{
	struct list_head *part[MAX_LIST_LENGTH_BITS+1]; /* sorted partial lists
						-- last slot is a sentinel */
	int lev;  /* index into part[] */
	int max_lev = 0;
	struct list_head *list;

	if (head->next == head)
		return;

	memset(part, 0, sizeof(part));

	head->prev->next = NULL;
	list = head->next;

	while (list) {
		struct list_head *cur = list;
		list = list->next;
		cur->next = NULL;

		for (lev = 0; part[lev]; lev++) {
			cur = merge(priv, cmp, part[lev], cur);
			part[lev] = NULL;
		}
		if (lev > max_lev) {
			if (lev >= (sizeof(part) / sizeof(part[0]))-1) {
				W("list passed to list_sort() too long for efficiency\n");
				lev--;
			}
			max_lev = lev;
		}
		part[lev] = cur;
	}

	for (lev = 0; lev < max_lev; lev++)
		if (part[lev])
			list = merge(priv, cmp, part[lev], list);

	merge_and_restore_back_links(priv, cmp, head, part[max_lev], list);
}


static inline uint64_t bus_sort_order(const uint32_t bus) {
  switch(bus) {
        case PCI:         return 0xFFFF00000000ULL | bus;
        case USB:         return 0xFFFE00000000ULL | bus;
        case INPUT:       return 0xFFFD00000000ULL | bus;
        case DVB:         return 0xFFFC00000000ULL | bus;
        case VIDEO4LINUX: return 0xFFFB00000000ULL | bus;
        case GRAPHICS:    return 0xFFFA00000000ULL | bus;
        default:          return 0x000000000000ULL | bus;
        }
}

int bus_device_cmp(void * somedata, struct list_head * a, struct list_head * b) {
  device_t *Ad, *Bd;

  uint64_t bus_a;
  uint64_t bus_b;
  
  Ad = list_entry(a, device_t, list);
  Bd = list_entry(b, device_t, list);

  bus_a = bus_sort_order(Ad->bus);
  bus_b = bus_sort_order(Bd->bus);

  if (bus_a > bus_b) return -1;
  if (bus_a < bus_b) return  1;

  return(strcasecmp(Ad->kernelname,Bd->kernelname));
}

void sort_bus_devices(struct list_head *head) {
  return list_sort(0, head, bus_device_cmp);
}
