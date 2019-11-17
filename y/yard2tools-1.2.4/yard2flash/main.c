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


#include "memimg.h"
#include <stdio.h>
#include <stdlib.h>
#include "fsusb.h"
#include <string.h>

#define NOT_VERIFIED 99 

picdem_handle *usbdev;


typedef int scan_callback_t(int,int,mi_byte_t*,char*);

int program_flash(int addr, int len, mi_byte_t *data, char *mask)
{
  if(len != 64) {
    printf("*** Tried to program flash not 64 bytes at a time!\n");
    printf("*** Is flash size divisible by 64?\n");
    return -1;
  }

  //  printf("writing program block at %.8x\n", addr);
  rjl_write_block(usbdev, addr, data);

  return 0;
}

int program_config(int addr, int len, mi_byte_t *data, char *mask)
{
  int i;

  printf("writing size-%i config block at %.8x:\n", len, (addr/64)*64);

  for(i=0;i<64;i++) {
    printf("%.2x", data[i]);
  }
  printf("\n");


  rjl_write_config_block(usbdev, addr, len, data);

  return 0;
}

int verify_flash(int addr, int len, mi_byte_t *data, char *mask)
{
  int mb;
  int i;
  bl_packet bp;
  int bad=0;
  //  printf("verifying %i bytes at %.8x\n", len, addr);
  /*
  printf("data is ");
  for(i=0;i<len;i++) {
    printf("%.2x", data[i]);
  }
  printf("\nmask is ");
  for(i=0;i<len;i++) {
    printf("%.2x", (mask[i])?0xff:0x00);
  }

  printf("\n");
  */

  for(mb=0;mb<64 && mb<len;mb+=32) {
    //    printf("doing rjl_request_flash(usbdev, %.8x, %i, %.8x);\n",
    //           addr+mb, (len-mb>=32)?32:len-mb, &bp);
    rjl_request_flash(usbdev, addr+mb, (len-mb>=32)?32:len-mb, &bp);

    for(i=0;i<32 && mb+i<len;i++) {
      if(mask[mb+i] && data[mb+i] != bp.data[i]) {
        //        printf("mismatch!\n");
        printf("mismatch in %i-byte chunk at 0x%.8x:\n",
               (len-mb>=32)?32:len-mb, addr+mb);
        printf("File:  ");
        for(i=0;i<32 && mb+i<len;i++) {
          if(mask[mb+i]) {
            printf("%.2x", data[mb+i]);
          } else {
            printf("##");
          }
        }
        printf("\nDevice:");
        for(i=0;i<32 && mb+i<len;i++) {
          printf("%.2x", bp.data[i]);
        }
        printf("\n");
	return NOT_VERIFIED;

      }
    }


  }

  return bad;
}

int scanpatch(mi_patch *p, scan_callback_t sc)
{
  int b,i,active;
  int retval=0;
  int callback_ret;

  for(b=0;b<=p->top - p->base;b+=64) {
    active=0;
    for(i=0;i<64 && b+i <= p->top - p->base;i++) {
      if(p->mask[i+b]) {
        active=1;
      }
    }

    if(active) {
      if((callback_ret=sc(b+p->base, (b+63+p->base > p->top)?p->top-p->base-b+1:64,
            p->contents+b, p->mask+b))) {
        retval=callback_ret;
        //        printf("*** Something bad happened!\n");
      }
      //      printf("active %s block at %.8lx\n", ttt, b+p->base);
    }


  }

  return retval;
}

void show_usage(void)
{
  printf("yard2flash: Software for YARD2 USB Firmware update\n");
  printf("       modified by RG [2012] YARD2USB\n");
  printf("       modified by <sig@sigvdr.de> [2011] to version fsusb-0.2\n");
  printf("       written by <rickluddy@gmail.com> version fsusb-0.1.11\n\n");

  printf("yard2flash command [<file>] [options]\n\n");
  printf("commands:\n");	
  printf("yard2flash --program <file> [options]  program board with <file> and verify\n");
  printf("yard2flash --verify <file> [options]   verify board against <file>\n");
//  printf("fsusb --read <file> [options]     read board, saving result in <file>\n");
  printf("yard2flash --reset [options]           reset board\n");
//  printf("fsusb --pic-list                  list of mcu's (for option <pic>)\n");
//  printf("fsusb --create <configFile>       create a default configFile for 18F4550, modify this\n");
  printf("\noptions:\n");
  printf("None available for YARD2:\n");
//  printf("      --usb-id <[VENDOR:]PRODUCT>  define usb hardware id -(check id with <lsusb>)\n");
//  printf("                                   default: 04d8:ff0b\n");
//  printf("      --pic <type of mcu>             default: 18Fx550\n");
//  printf("      --device <configFile>       other mcu's than <--pic-list> need to be defined\n");
}


int verify_file(char *file)
{
  mi_image *img;

  usbdev=rjl_fsusb_open();
  img=mi_load_hexfile(file);

  int retval=scanpatch(img->program, verify_flash);

  if(retval) 
  {
    printf("Program memory contains errors!\n");
  } 
  else 
  {
    printf("Program memory validated\n");
  }

  // don't bother checking config memory, it's write-protected anyway
  //  scanpatch(img->id, verify_flash);
  //  scanpatch(img->config, verify_flash);
  //  scanpatch(img->devid, verify_flash);

  return retval;
}

int program_file(char *file)
{
  mi_image *img;
  int retval=0;
//  int r;
//  byte buf[1];
//  buf[0] = 0xFF;	
//  unsigned char buffer[7];

  usbdev=rjl_fsusb_open();
  img=mi_load_hexfile(file);

  if(scanpatch(img->program, program_flash)) 
  {
    printf("Writing program memory unsuccessful\n");
    retval=1;
  }
  else 
  {
    printf("Writing program memory successful\n");
  }

  // don't bother with config memory, it's write-protected
  //  scanpatch(img->id, program_config);
  //  scanpatch(img->config, program_config);

  // devid is read-only, don't program it
  //  scanpatch(img->devid, program_config); 

  // don't bother checking config memory, it's write-protected anyway
  //  scanpatch(img->id, verify_flash);
  //  scanpatch(img->config, verify_flash);
  //  scanpatch(img->devid, verify_flash);

   if(retval == 0)
   {
     if(scanpatch(img->program, verify_flash)) 
     {
       printf("Program memory contains errors!\n");
       retval=1;
     } 
     else
     {
       printf("Program memory validated\n");
       printf("Reset Y.A.R.D.2 ...\n");
       reset2(usbdev);
     }
   }

  return retval;

}

int write_range(int base, int top, FILE *f)
{
  bl_packet bp;
  int addr;
  int i;
  unsigned char checksum;
  int doit;


  fprintf(f, ":02000004%.2X%.2X%.2X\n",
         (base >> 16) & 0xff,
         (base >> 24) & 0xff,
         (unsigned char)
         (-((char)((2+4+((base >> 16) & 0xff)+((base >> 24) & 0xff))%256))));

  for(addr=base;addr<=top;addr+=16) {
    rjl_request_flash(usbdev, addr,
                      (top-addr+1>=16)?16:top-addr+1, &bp);

    doit=0;
    for(i=0;i<16 && i+addr < top;i++) {
      if(bp.data[i] != 0xff) {
        doit=1;
      }
    }
    if(doit==0) { // This section is all 0xff, i.e. unprogrammed
      continue;
    }

    // :
    fprintf(f, ":");
    checksum=0;

    // ll
    checksum += (top-addr+1>=16)?16:top-addr+1;
    fprintf(f, "%.2X", (top-addr+1>=16)?16:top-addr+1);

    // aaaa
    checksum += (addr & 0xffff) / 256;
    fprintf(f, "%.2X", (addr & 0xffff) / 256);
    checksum += (addr & 0xffff) % 256;
    fprintf(f, "%.2X", (addr & 0xffff) % 256);

    // tt
    fprintf(f, "00");

    // dd...
    for(i=0;i<16 && i+addr<=top;i++) {
      checksum += bp.data[i];
      fprintf(f, "%.2X", bp.data[i]);
    }

    // cc
    fprintf(f, "%.2X\n", (unsigned char)(-((char)checksum)));
  }

  return 0;
}


int read_to_file(char *file)
{
  usbdev=rjl_fsusb_open();
  FILE *f;

  f=fopen(file, "w");

  if(f == NULL) {
    return -1;
  }

  write_range(MI_PROGRAM_BASE, MI_PROGRAM_TOP, f);
  write_range(MI_ID_BASE, MI_ID_TOP, f);
  write_range(MI_CONFIG_BASE, MI_CONFIG_TOP, f);
  write_range(MI_DEVID_BASE, MI_DEVID_TOP, f);
  fprintf(f, ":00000001FF\n");

  printf("Finished reading\n");

  return 0;
}


int main(int argc, char *argv[])
{
  int i;	
  if(argc < 2 /*|| argc > 3*/) {
    show_usage();
    exit(1);
  }
  // parse options  
  for ( i = 1; i < argc; i++ ) {
     if(!strcmp(argv[i],"--usb-id")) {
	setUsbId(argv[i+1]);
	}
     if(!strcmp(argv[i],"--pic")) {
	definePicType(argv[i+1]);
	}
     if(!strcmp(argv[i],"--device")) {
	configPicDevice(argv[i+1]);
	}
  }	

  if(!strcmp(argv[1], "--reset")) {
    exit( reset() );
  }

//  if(!strcmp(argv[1], "--pic-list")) {
//    exit( listPicType() );
//  }

  // parse command 
  if(argc > 2) {
    if(!strcmp(argv[1], "--verify")) {
      exit(verify_file(argv[2]));
    }

    if(!strcmp(argv[1], "--program")) 
    {
	exit(program_file(argv[2]));
    }

//    if(!strcmp(argv[1], "--read")) {
//      exit(read_to_file(argv[2]));
//    }

//    if(!strcmp(argv[1], "--create")) {
//      exit(createConfigFile(argv[2]));
//    }

    fprintf(stderr,"Error: Unknown command %s\n", argv[1]);
    show_usage();
    exit(1);
  }


  show_usage();
  return 1;
}
