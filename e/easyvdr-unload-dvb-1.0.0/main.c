/*
 * A simple tool for unloading all dvb drivers, mentioned in modules.dep
 * Copyright (C) 2014 Winfried Koehler 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * The author can be reached at: handygewinnspiel AT gmx DOT de
 *
 * The project's page is http://wirbel.htpc-forum.de
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/utsname.h>
#include <sys/errno.h>
#include <syscall.h>
#include <fcntl.h>

typedef struct {
  char * name;
  int size;
  int refcount;
}lm;

struct utsname ubuf;
lm linux_module;


void parse_mod(const char * buf) {
  int i=0;
  int j=0;
  int idx;
  char numbuf[16+1];

  for (i=0; i<strlen(buf); i++) {
      if ((buf[i]==' ') || (buf[i]=='\t')){
         if (linux_module.name && *linux_module.name) {
            free(linux_module.name);
            linux_module.name=NULL;
            }
         linux_module.name = (char *) calloc(i+1,1);
         strncpy(linux_module.name, buf, i);
         for (j=i; j<strlen(buf); j++)
             if (buf[j] != ' ')
                break;
         for (idx=0; j<strlen(buf); j++)
             if (buf[j] >= '0' && buf[j]<= '9')
                numbuf[idx++]=buf[j];
             else
                break;
         numbuf[idx]=0;
         linux_module.size=atoi(numbuf);

         for (; j<strlen(buf); j++)
             if (buf[j] != ' ')
                break;
         for (idx=0; j<strlen(buf); j++)
             if (buf[j] >= '0' && buf[j]<= '9')
                numbuf[idx++]=buf[j];
             else
                break;
         numbuf[idx]=0;
         linux_module.refcount=atoi(numbuf);
          
         break;
         }
      }
}

int main(void ) {
  uname(&ubuf);
  char * modules_dep_path;
  char * modulename;
  FILE * modules_dep = NULL;
  FILE * proc_modules = NULL;
  int err;
  int i;
  int last_module_counter = -1, module_counter = 0;

  linux_module.name=NULL;

  char procbuf[4096];
  char depsbuf[4096];

  printf("kernel release  = %s (%s)\n", ubuf.release, ubuf.machine);
  err = asprintf(&modules_dep_path, "/lib/modules/%s/modules.dep", ubuf.release);
//printf("reading \"%s\"\n", modules_dep_path);

  modules_dep = fopen(modules_dep_path, "rt");
  if (modules_dep == NULL) {
     printf("Error: \"%s\" does not exist.\n", modules_dep_path);
     return -1;
     }

  proc_modules = fopen("/proc/modules", "rt");
  if (proc_modules == NULL) {
     printf("Error: \"%s\" does not exist.\n", "/proc/modules");
     fclose(modules_dep);
     return -1;
     }

REDO:

  while (fgets(&procbuf[0], sizeof(procbuf) - 1, proc_modules)) {
     parse_mod(procbuf);

     // trying to find this mod.
     fseek(modules_dep,0,SEEK_SET);

     while (fgets(&depsbuf[0], sizeof(depsbuf) - 1, modules_dep)) {
        modulename = NULL;

        if (strstr(&depsbuf[0], "kernel/drivers/media/") == &depsbuf[0]) {
           modulename = &depsbuf[sizeof("kernel/drivers/media/")-1];
           for (i=sizeof("kernel/drivers/media/")-1; i<sizeof(depsbuf); i++) {
               if (depsbuf[i] == ':') {
                  depsbuf[i]=0;
                  break;
                  }
               }
           }
        else if (strstr(&depsbuf[0], "kernel/drivers/staging/media/") == &depsbuf[0]) {
           modulename = &depsbuf[sizeof("kernel/drivers/staging/media/")-1];
           for (i=sizeof("kernel/drivers/staging/media/")-1; i<sizeof(depsbuf); i++) {
               if (depsbuf[i] == ':') {
                  depsbuf[i]=0;
                  break;
                  }
               }
           }        
        else if (strstr(&depsbuf[0], "updates/media/") == &depsbuf[0]) {
           modulename = &depsbuf[sizeof("updates/media/")-1];
           for (i=sizeof("update/media/")-1; i<sizeof(depsbuf); i++) {
               if (depsbuf[i] == ':') {
                  depsbuf[i]=0;
                  break;
                  }
               }
           } 
        else
           continue;

        for (i=0; i<strlen(modulename); i++)
           if (modulename[i] == '-')
              modulename[i] = '_';

        if (strrchr(modulename, '/') != NULL)
           modulename = 1 + strrchr(modulename, '/');
        modulename[strlen(modulename)-3]=0;

      //printf("modulename \"%s\" <-> linux_module.name \"%s\" ? \n", modulename, linux_module.name);
        if (strcasecmp(modulename, linux_module.name) == 0)
           break;
        }

   //printf("modulename \"%s\" <-> linux_module.name \"%s\" ? \n", modulename, linux_module.name);

     // this module doesnt belong to media.
     if (modulename == NULL)
        continue;
     if (strcasecmp(modulename, linux_module.name) != 0)
        continue;

     module_counter++;

     if (linux_module.refcount == 0) {
        printf("trying to unload module %s\n", linux_module.name);
        if (syscall(SYS_delete_module, linux_module.name, O_NONBLOCK) < 0) {
           printf("Error unloading kernel module. The error code returned was:\n");
           switch(errno) {
              case EBUSY:  printf("  The module is not \"live\" (i.e., it is still being initialized\n"
                                  "  or is already marked for removal); or, the module has an init\n"
                                  "  function but has no exit function, and O_TRUNC was not specified in flags.\n");
                           break;
              case EFAULT: printf("  name refers to a location outside the process's accessible address space.\n");
                           break;
              case ENOENT: printf("  No module by that name exists.\n");
              case EPERM:  printf("  The caller was not privileged (did not have the CAP_SYS_MODULE capability),\n"
                                  "  or module unloading is disabled\n"
                                  "   (see /proc/sys/kernel/modules_disabled in proc(5)).\n");
                           break;
              case EWOULDBLOCK:
                           printf("  Other modules depend on this module; or, O_NONBLOCK was specified in flags,\n"
                                  "  but the reference count of this module is nonzero and O_TRUNC was not\n"
                                  "  specified in flags.\n");
                           break;
              default:     printf("  unknown error code.. (not mentioned in \"delete_module\" man page.)\n)");
              }
           }
        else
           usleep(500000); 
        }
     }

//printf("module_counter= %d\n", module_counter);
  
  if (module_counter != last_module_counter) {
     last_module_counter = module_counter;
   //printf("modules = %d\n", module_counter);
     module_counter = 0;
     fseek(proc_modules,0,SEEK_SET);
     goto REDO;
     }

  fclose(proc_modules);
  free(modules_dep_path);
  if (linux_module.name != NULL)
     free(linux_module.name);
  fclose(modules_dep);
  return 0;
}