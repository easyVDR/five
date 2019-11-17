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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// to add predefined pic mcu's expand from deviceList to definePicType 
const char *deviceList[] = {
	"18F2455",
	"18F2550",
	"18F4455",
	"18F4550",
	""
};
int pic18Fx455(void) {
	MI_PROGRAM_TOP  = 0x5fff;
	return 0;
}
int pic18Fx550(void) {
// default values nothing to do...
	return 0;
}
int definePicType(char *name){
	if(!strcmp(name,"18F2455")) return pic18Fx455();
	if(!strcmp(name,"18F2550")) return pic18Fx550();
	if(!strcmp(name,"18F4455")) return pic18Fx455();
	if(!strcmp(name,"18F4550")) return pic18Fx550();
	fprintf(stderr,"Error: <%s> is not defined in fsusb\n",name);
	printf("   please create configfile for <%s>\n",name);
	printf("   and use option --device <configFile>\n");
	listPicType();
	exit(18);
	return 1;
}
int listPicType(void){
	int i=0;
	printf("list of predefined mcu's in fsusb\n");
	while ( deviceList[i] ) {
		printf("   %s\n",deviceList[i++]);
	} 
	return 0;
}
int createConfigFile(char *fileName){
	FILE *f;

	f=fopen(fileName, "w");

	if(f == NULL) {
		fprintf(stderr,"Error: could not create <%s>\n",fileName); 
		return -1;
	}
	fprintf(f,"[18F4550]\n");
	fprintf(f,"EEPROM_BASE  = 0x%02x\n",MI_EEPROM_BASE);
	fprintf(f,"EEPROM_TOP   = 0x%02x\n",MI_EEPROM_TOP);

	fprintf(f,"PROGRAM_BASE = 0x%04x\n",MI_PROGRAM_BASE);
	fprintf(f,"PROGRAM_TOP  = 0x%04x\n",MI_PROGRAM_TOP);
	fprintf(f,"ID_BASE      = 0x%06x\n",MI_ID_BASE);
	fprintf(f,"ID_TOP       = 0x%06x\n",MI_ID_TOP);

	fprintf(f,"CONFIG_BASE  = 0x%06x\n",MI_CONFIG_BASE);
	fprintf(f,"CONFIG_TOP   = 0x%06x\n",MI_CONFIG_TOP);

	fprintf(f,"DEVID_BASE   = 0x%06x\n",MI_DEVID_BASE);
	fprintf(f,"DEVID_TOP    = 0x%06x\n",MI_DEVID_TOP );
	fclose(f);
	printf("file <%s> created\n",fileName);
	return 0;
}

char *trim(char *str){
	int i = -1;
	int len = 0;
	char z;
	char *start=NULL;
	while ( (z=str[++i]) ) {
		if ( !isspace(z) ) {
			if ( !start ) {
				start = str+i;
			}
			len=i;
		}
	}
	str[len+1] = '\0';
	return start;
}
int csplit(char *str, char c){
	int i = -1;
	int count = 1;
	while ( str[++i] ) {
		if ( str[i] == c ) {
			count++;
			str[i] = '\0';
		}
	}
	return count;
}
char *splitNext(char *str){
	int i = -1;
	while ( (str[++i]) ) {
	}
	while ( !(str[++i]) ) {
	}
	return str+i;
}
#define MAX_LINE_LEN 50
int configPicDevice(char *fileName) {
	int count;
	char *cvalue;
	char *cname;
	FILE *f;
	char line[MAX_LINE_LEN];
	f=fopen(fileName, "r");
	
	if (f == NULL) {
		fprintf(stderr,"Error: could not read <%s>\n",fileName); 
		return -1;
	} 

	while ( fgets(line,MAX_LINE_LEN,f) != NULL ) {
		// example shows what happens with <line>
		// line = " NAME = 0xff   \n"
		count = csplit(line,'=');
		// line = " NAME \0 0xff  \n"
		if ( count > 1) {
			cvalue = trim(splitNext(line));
		}
		// line = " NAME \0 0xff\0 \n"
		cname = trim(line);
		// line = " NAME\0\0 0xff\0 \n"

		if(!strcmp(cname, "EEPROM_BASE"))  sscanf(cvalue, "%x", &MI_EEPROM_BASE); 
		if(!strcmp(cname, "EEPROM_TOP"))   sscanf(cvalue, "%x", &MI_EEPROM_TOP); 
		if(!strcmp(cname, "PROGRAM_BASE")) sscanf(cvalue, "%x", &MI_PROGRAM_BASE); 
		if(!strcmp(cname, "PROGRAM_TOP"))  sscanf(cvalue, "%x", &MI_PROGRAM_TOP); 
		if(!strcmp(cname, "ID_BASE"))      sscanf(cvalue, "%x", &MI_ID_BASE); 
		if(!strcmp(cname, "ID_TOP"))       sscanf(cvalue, "%x", &MI_ID_TOP); 
		if(!strcmp(cname, "CONFIG_BASE"))  sscanf(cvalue, "%x", &MI_CONFIG_BASE); 
		if(!strcmp(cname, "CONFIG_TOP"))   sscanf(cvalue, "%x", &MI_CONFIG_TOP); 
		if(!strcmp(cname, "DEVID_BASE"))   sscanf(cvalue, "%x", &MI_DEVID_BASE); 
		if(!strcmp(cname, "DEVID_TOP"))    sscanf(cvalue, "%x", &MI_DEVID_TOP); 
	}
	fclose(f);

        //createConfigFile("test.conf");
	return 0;
}

/* mi_make_patch: Allocate and initialize a mi_patch
 */
mi_patch *mi_make_patch(unsigned long base, unsigned long top)
{
  mi_patch *pat;



  pat=malloc(sizeof(mi_patch));
  if(pat == NULL) {
    return NULL;
  }


  pat->base=base;
  pat->top=top;



  pat->contents=malloc(sizeof(mi_byte_t)*(top-base));
  if(pat->contents == NULL) {
    free(pat);
    return NULL;
  }

  memset(pat->contents, 0xff, sizeof(mi_byte_t)*(top-base));



  pat->mask=malloc(sizeof(char)*(top-base));
  if(pat->mask == NULL) {
    free(pat->contents);
    free(pat);
    return NULL;
  }

  memset(pat->mask, 0, sizeof(char)*(top-base));



  return pat;
}



/* mi_free_patch: Free a mi_patch and its buffers
 */
void mi_free_patch(mi_patch *p)
{
  if(p == NULL) {
    return;
  }

  free(p->contents);
  free(p->mask);
  free(p);
}



/* mi_free_image: Free a mi_patch and its contents
 */
void mi_free_image(mi_image *i)
{
  if(i == NULL) {
    return;
  }

  mi_free_patch(i->program);
  mi_free_patch(i->id);
  mi_free_patch(i->config);
  mi_free_patch(i->devid);
  mi_free_patch(i->eeprom);

  free(i);
}



/* mi_modify_patch: Modify patch contents, tagging it as changed
 */
void mi_modify_patch(mi_patch *p, int base, int len, mi_byte_t *data)
{
  int i;

  if(p == NULL) {
    return;
  }



  if(base<p->base || base+len-1 > p->top) {
    printf("*** mi_modify_patch(): patch out of range\n");
    return;
  }



  for(i=0;i<len;i++) {
    p->contents[base - p->base + i]=data[i];
    p->mask[base - p->base + i]=0xff;
  }
}



/* mi_image: Create a mi_image from the contents of filename
 */
mi_image *mi_load_hexfile(char *filename)
{
  mi_image *img;
  hex_record *r;
  FILE *f;
  hex_file *hf;



  if(filename == NULL) {
    return NULL;
  }



  f=fopen(filename, "r");
  if(f == NULL) {
    return NULL;
  }



  hf=hex_open(f);
  if(hf == NULL) {
    fclose(f);
    return NULL;
  }



  img=malloc(sizeof(mi_image));
  if(img == NULL) {
    fclose(f);
    free(hf);
    return NULL;
  }



  /* These nulls may not be required, but make me feel safer when
   *  using free_image() on an error
   */
  img->program = NULL;
  img->id = NULL;
  img->config = NULL;
  img->devid = NULL;
  img->eeprom = NULL;



  img->program=mi_make_patch(MI_PROGRAM_BASE, MI_PROGRAM_TOP);
  img->id=mi_make_patch(MI_ID_BASE, MI_ID_TOP);
  img->config=mi_make_patch(MI_CONFIG_BASE, MI_CONFIG_TOP);
  img->devid=mi_make_patch(MI_DEVID_BASE, MI_DEVID_TOP);
  img->eeprom=mi_make_patch(MI_EEPROM_BASE, MI_EEPROM_TOP);

  if(img->program == NULL || img->id == NULL || img->config == NULL
     || img->devid == NULL || img->eeprom == NULL) {
    fclose(f);
    free(hf);
    mi_free_image(img);
    return NULL;
  }



  while((r=hex_read(hf))) {
    if(r->type == 0) {
      /*
      printf("file: %.2i@0x%.8X:\t", r->datlen, r->addr);
      for(i=0;i<r->datlen;i++) {
        printf("%.2x", r->data[i]);
      }
      printf("\n");
      */

      if(r->addr >= MI_PROGRAM_BASE && r->addr <= MI_PROGRAM_TOP) {
        //        printf("Program memory\n");
        mi_modify_patch(img->program, r->addr, r->datlen, r->data);
      }

      if(r->addr >= MI_ID_BASE && r->addr <= MI_ID_TOP) {
        //        printf("ID memory\n");
        mi_modify_patch(img->id, r->addr, r->datlen, r->data);
      }

      if(r->addr >= MI_CONFIG_BASE && r->addr <= MI_CONFIG_TOP) {
        //        printf("Config memory\n");
        mi_modify_patch(img->config, r->addr, r->datlen, r->data);
      }

      if(r->addr >= MI_DEVID_BASE && r->addr <= MI_DEVID_TOP) {
        //        printf("Devid memory\n");
        mi_modify_patch(img->devid, r->addr, r->datlen, r->data);
      }


    }
    free(r);
    //    printf("\n");
  }



  free(hf);
  fclose(f);
  return img;
}

