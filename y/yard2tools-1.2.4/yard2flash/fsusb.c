/*
** This file is part of fsusb_picdem
**
** fsusb_picdem is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License as
** published by the Free Software Foundation; either version 2 of the
** License, or (at your option) any later version.
**
** fsusb_picdem is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with fsusb_picdem; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301, USA
*/

/*
** portions from usb_pickit by Orion Sky Lawlor, olawlor@acm.org
*/

#include <usb.h> /* libusb header */
#include <unistd.h> /* for geteuid */
#include <stdio.h>
#include <string.h>
#include "bootload.h"
#include "fsusb.h"


static int fsusb_vendorID=0x04d8; // Microchip, Inc
static int fsusb_productID=0xF544;  // YARDUSB
static int fsusb_productID2=0xFF0b; // YARDUSB
const static int fsusb_configuration=1; /* 1: bootloader
                                         * ### may change in future firmware versions
                                         */
const static int fsusb_interface=0;
const static int  fsusb_endpoint_write=1; /* first endpoint for everything */
//  fsusb_endpoint_read=0x81 for Windows use //81
const static int  fsusb_endpoint_read=0x81; /* first endpoint for everything
                                    * ### may change in future firmware versions
                                    */
const static int fsusb_timeout=2000; /* timeout in ms */



void bad(const char *why)
{
  fprintf(stderr,"Fatal error> %s\n",why);
  exit(17);
}

void setUsbId(const char *newId){
// possible values for newId are
// newId = VENDOR:PRODUCT  04d8:000b
// newId = PRODUCT         000b  
   char *pos;
   pos = strchr(newId, ':');
   if (pos) {
      pos[0] = '\0';
      sscanf(newId, "%x", &fsusb_vendorID);
      sscanf(pos+1, "%x", &fsusb_productID);
    } else {	
      sscanf(newId,"%x",&fsusb_productID);
   }
      //fprintf(stderr,"Vendor:  0x%04x\n",fsusb_vendorID);
      //fprintf(stderr,"Product: 0x%04x\n",fsusb_productID);
}

void recv_usb(picdem_handle *d, int len, byte *dest) {
  int r;

  r=usb_bulk_read(d,  fsusb_endpoint_read, (char*)dest, len, fsusb_timeout);
  //printf("read %i bytes\n", r);

  if (r!=len) {
    perror("usb PICDEM read");
    bad("USB read failed");
  }
  //  printf("read %i bytes\n", r);
}

int reset2(picdem_handle *d) 
{
 // picdem_handle *usbdev;
  int r;
  byte buf[1];
  buf[0] = 0xFF;	

  unsigned char buffer[7];

  //Reset softawre reset command
  buffer[0] = WRITE_EEDATA;
  buffer[1] = 2;            // länge 1 Byte
  buffer[2] = 0xFE;          // low
  buffer[3] = 0;            // high
  buffer[4] = 0;            // upper
  buffer[5] = 0;            // 1. Byte für 0xFE
  buffer[6] = 0;            // 2. Byte für 0xFF
  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)buffer, 7, fsusb_timeout);

  if(r != 7) {
    perror("usb_bulk_write WRITE_EEDATA ");
    bad("rjl_request_version(): USB write failed WRITE_EEDATA");
  }
  recv_usb(d,1,buffer);

  sleep(1);

 //reset device
  // ### "\0\0\0\0\0" may not be correct in future firmware versions
  r = usb_bulk_write(d,  fsusb_endpoint_write, (char*)buf, 1, fsusb_timeout);
  if(r != 1) {
    perror("usb_bulk_write ");
    bad("rjl_request_version(): USB write failed");
  }
  printf("Y.A.R.D.2 reset successful\n");
  return 0;
}

int reset(void) 
{
  picdem_handle *usbdev;
  usbdev=rjl_fsusb_open();
  return reset2(usbdev);
}


void rjl_request_version(picdem_handle *d, unsigned char *ret)
{
  int r;
  byte buf[4];

  // ### "\0\0\0\0\0" may not be correct in future firmware versions
  r=usb_bulk_write(d,  fsusb_endpoint_write, "\0\0\0\0\0", 5, fsusb_timeout);
  if(r != 5) {
    perror("usb_bulk_write");
    bad("rjl_request_version(): USB write failed");
  }
   //printf("write %i bytes\n", r);

  // command, len, minor, major
  recv_usb(d,4,buf);
  ret[0]=buf[3];
  ret[1]=buf[2];
}



void rjl_request_flash(picdem_handle *d, int offset, int len, bl_packet *pack)
{
  int r;
  bl_packet p;


  p.command=READ_FLASH;
  p.address.low=(offset & 0xff)>>0;
  p.address.high=(offset & 0xff00)>>8;
  p.address.upper=(offset & 0xf0000)>>16;
  p.len=len;


  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5, fsusb_timeout);
  if(r != 5) {
    perror("usb_bulk_write");
    bad("rjl_request_flash(): USB write failed");
  }


  recv_usb(d,len+5,(byte*)pack);
}



/* write in 16-byte boundary-aligned blocks only in this version of
 * the bootloader
 */
void rjl_write_flash(picdem_handle *d, int offset, int len, byte *data, bl_packet *pack)
{
  int r;
  bl_packet p;
  int i;
  byte retbuf[5];



  if(offset & 0x0f) {
    printf("*** WARNING: not boundary-aligned\n");
    return;
  }
  if(len != 16) {
    printf("*** WARNING: not 16 bytes\n");
    return;
  }



  p.command=WRITE_FLASH;
  p.address.low=(offset & 0xff)>>0;
  p.address.high=(offset & 0xff00)>>8;
  p.address.upper=(offset & 0xf0000)>>16;
  p.len=len;
  for(i=0;i<len;i++) {
    p.data[i]=data[i];
  }


  printf(".");
  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5+len, fsusb_timeout);
  if(r != 5+len) {
    perror("usb_bulk_write");
    bad("rjl_write_flash(): USB write failed");
  }

  recv_usb(d,1,retbuf);
  //  printf("write reply is %x\n", retbuf[0]);
}



/* write on 64-byte boundaries only in blocks of 64 bytes.
 *  It's a feature.
 */
void rjl_write_block(picdem_handle *d, int offset, byte *data)
{
  int r;
  bl_packet p;
  byte retbuf[5];
  int subblock=0;


  if(offset & 0x3f) {
    printf("*** WARNING: not boundary-aligned\n");
    return;
  }


  p.command=ERASE_FLASH;
  p.address.low=(offset & 0xff)>>0;
  p.address.high=(offset & 0xff00)>>8;
  p.address.upper=(offset & 0xf0000)>>16;
  p.len=1;

  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5, fsusb_timeout);
  if(r != 5) {
    perror("usb_bulk_write");
    bad("rjl_write_block(): USB write failed");
  }


  recv_usb(d,1,retbuf);
  //  printf("erase reply is %x\n", retbuf[0]);


  for(subblock=0;subblock<4;subblock++) {
    p.command=WRITE_FLASH;
    p.address.low=((offset+16*subblock) & 0xff)>>0;
    p.address.high=((offset+16*subblock) & 0xff00)>>8;
    p.address.upper=((offset+16*subblock) & 0xf0000)>>16;
    p.len=16;
    memcpy(p.data, data+(subblock*16), 16);


    r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5+16, fsusb_timeout);
    if(r != 5+16) {
      perror("usb_bulk_write");
      bad("rjl_write_block(): USB write failed");
    }


    recv_usb(d,1,retbuf);
    //  printf("write reply is %x\n", retbuf[0]);
  }
}



// 59ish bytes max
void rjl_write_config_block(picdem_handle *d, int offset, int len, byte *data)
{
  int r;
  bl_packet p;
  //  int i;
  byte retbuf[5];


  if(len>=BL_DATA_LEN) {
    printf("*** ERROR: config block too big\n");
    return;
  }



  /* The firmware clips the erase to a 64-byte block, which
   *  we don't worry about because in any real device
   *  the config starts on a 64-byte boundary.
   */
  p.command=ERASE_FLASH;
  p.address.low=(offset & 0xff)>>0;
  p.address.high=(offset & 0xff00)>>8;
  p.address.upper=(offset & 0xf0000)>>16;
  p.len=1;


  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5, fsusb_timeout);
  if(r != 5) {
    perror("usb_bulk_write");
    bad("rjl_write_config_block(): USB write failed");
  }


  recv_usb(d,1,retbuf);
  //  printf("erase reply is %x\n", retbuf[0]);


  // config writes have no alignment restriction
  p.command=WRITE_CONFIG;
  p.address.low=(offset & 0xff)>>0;
  p.address.high=(offset & 0xff00)>>8;
  p.address.upper=(offset & 0xf0000)>>16;
  p.len=len;
  memcpy(p.data, data, len);


  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5+len, fsusb_timeout);
  if(r != 5+len) {
    perror("usb_bulk_write");
    bad("rjl_write_config_block(): USB write failed");
  }


  recv_usb(d,1,retbuf);
  //  printf("write reply is %x\n", retbuf[0]);
}



// write on 64-byte boundaries only in blocks of 64 bytes
void rjl_erase_block(picdem_handle *d, int offset)
{
  int r;
  bl_packet p;
  byte retbuf[5];


  if(offset & 0x3f) {
    printf("*** WARNING: not boundary-aligned\n");
    return;
  }



  p.command=ERASE_FLASH;
  p.address.low=(offset & 0xff)>>0;
  p.address.high=(offset & 0xff00)>>8;
  p.address.upper=(offset & 0xf0000)>>16;
  p.len=1;


  r=usb_bulk_write(d,  fsusb_endpoint_write, (char*)&p, 5, fsusb_timeout);
  if(r != 5) {
    perror("usb_bulk_write");
    bad("rjl_erase_block(): USB write failed");
  }


  recv_usb(d,1,retbuf);
  //  printf("erase reply is %x\n", retbuf[0]);
}



/* Find the first USB device with this vendor and product.
 *  Exits on errors, like if the device couldn't be found. -osl
 *
 * This function is heavily based upon Orion Sky Lawlor's
 *  usb_pickit program, which was a very useful reference
 *  for all the USB stuff.  Thanks!
 */
picdem_handle *rjl_fsusb_open(void)
{
  struct usb_device *device;
  struct usb_bus* bus;
  unsigned char buf[2];

/*
  if (geteuid()!=0) {
    bad("This program must be run as root, or made setuid root");
  }
*/
#ifdef USB_DEBUG
  usb_debug=4; 
#endif

  printf("Locating Y.A.R.D.2 bootloader (vendor 0x%04x/(product 0x%04x or 0x%04x))\n",
  	fsusb_vendorID,fsusb_productID,fsusb_productID2);
  /* (libusb setup code stolen from John Fremlin's cool "usb-robot") -osl */
  usb_init();
  usb_find_busses();
  usb_find_devices();

  for (bus=usb_busses;bus!=NULL;bus=bus->next) {
    struct usb_device* usb_devices = bus->devices;
    for(device=usb_devices;device!=NULL;device=device->next) {


      if (device->descriptor.idVendor == fsusb_vendorID
          && ( (device->descriptor.idProduct == fsusb_productID) || 
               (device->descriptor.idProduct == fsusb_productID2) ) )
	{

        usb_dev_handle *d;
        printf( "Found Y.A.R.D.2 USB Bootloader as device '%s' on USB bus %s\n",
                device->filename,
                device->bus->dirname);

        d=usb_open(device);


        if (d) { /* This is our device-- claim it */
          if (usb_set_configuration(d,fsusb_configuration)) {
            bad("Error setting USB configuration. Run with sudo ? \n");
          }

          if (usb_claim_interface(d,fsusb_interface)) {
            bad("Claim failed-- the USB PICDEM is in use by another driver.\n"
                "Do a `dmesg` to see which kernel driver has claimed it--\n"
                "You may need to `rmmod hid` or patch your kernel's hid driver.\n");
          }

          rjl_request_version(d, buf);

          printf("Communication established.  Onboard Bootloader firmware version is %d.%d\n",
                 (int)buf[0],(int)buf[1]);

          if (buf[0] > 0x02u) {
            bad("This Bootloader version is too new (only support version 1.x !)\n");
          }

          return d;
        } else 
          bad("Open failed for USB device");
      }


      /* else some other vendor's device-- keep looking... -osl*/
    }
  }

  bad("Could not find USB Y.A.R.D.2 Bootloader device--\n"
      "you might try lsusb to see if it's actually there.");

  return NULL;
}
