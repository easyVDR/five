/*
 * dvdswitch plugin for VDR (C++)
 *
 * (C) 2010 Andreas Brachold <vdr07 AT deltab de>
 *
 * This dvdswitch plugin is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, version 2 of the License.
 *
 * See the files README and COPYING for details.
 *
 */

#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vdr/tools.h>

int volname(const char* device, char* volname, size_t volsize)
{
  int fd;
  int status;
  char buffer[PATH_MAX];

  bzero(volname,volsize); 

  if(!realpath(device, buffer)) {
    return errno;
  }

  fd = open(device, O_RDONLY);
  if (fd == -1) {
    return errno;
  }

  status = lseek(fd, 32808, SEEK_SET);
  if (status == -1) {
    status = errno;
    close(fd);
    return status;
  }

  status = read(fd, volname, volsize > 32 ? 32 : volsize);
  if (status == -1) {
    status = errno;
    close(fd);
    return status;
  }

  close(fd);

  stripspace(volname);

  return 0;
}
