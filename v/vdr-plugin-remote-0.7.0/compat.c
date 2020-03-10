/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * compat.h: Compatibility stuff
 *
 * Copyright (C) 2015 Oliver Endriss <o.endriss@gmx.de>
 *
 * cSocketRemote was copied from cSocket (svdrp.c, VDR 2.2.0):
 * Copyright (C) 2000, 2003, 2006, 2008, 2013 Klaus Schmidinger
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
 */

#include <vdr/config.h>
#include "compat.h"

#if VDRVERSNUM >= 20300
// --- cSocket ---------------------------------------------------------------

cSocketRemote::cSocketRemote(int Port, int Queue)
{
  port = Port;
  sock = -1;
  queue = Queue;
}

cSocketRemote::~cSocketRemote()
{
  Close();
}

void cSocketRemote::Close(void)
{
  if (sock >= 0) {
     close(sock);
     sock = -1;
     }
}

bool cSocketRemote::Open(void)
{
  if (sock < 0) {
     // create socket:
     sock = socket(PF_INET, SOCK_STREAM, 0);
     if (sock < 0) {
        LOG_ERROR;
        port = 0;
        return false;
        }
     // allow it to always reuse the same port:
     int ReUseAddr = 1;
     setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ReUseAddr, sizeof(ReUseAddr));
     //
     struct sockaddr_in name;
     name.sin_family = AF_INET;
     name.sin_port = htons(port);
     name.sin_addr.s_addr = SVDRPhosts.LocalhostOnly() ? htonl(INADDR_LOOPBACK) : htonl(INADDR_ANY);
     if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
        LOG_ERROR;
        Close();
        return false;
        }
     // make it non-blocking:
     int oldflags = fcntl(sock, F_GETFL, 0);
     if (oldflags < 0) {
        LOG_ERROR;
        return false;
        }
     oldflags |= O_NONBLOCK;
     if (fcntl(sock, F_SETFL, oldflags) < 0) {
        LOG_ERROR;
        return false;
        }
     // listen to the socket:
     if (listen(sock, queue) < 0) {
        LOG_ERROR;
        return false;
        }
     }
  return true;
}

int cSocketRemote::Accept(void)
{
  if (Open()) {
     struct sockaddr_in clientname;
     uint size = sizeof(clientname);
     int newsock = accept(sock, (struct sockaddr *)&clientname, &size);
     if (newsock > 0) {
        bool accepted = SVDRPhosts.Acceptable(clientname.sin_addr.s_addr);
        if (!accepted) {
           const char *s = "Access denied!\n";
           if (write(newsock, s, strlen(s)) < 0)
              LOG_ERROR;
           close(newsock);
           newsock = -1;
           }
        isyslog("connect from %s, port %hu - %s", inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port), accepted ? "accepted" : "DENIED");
        }
     else if (errno != EINTR && errno != EAGAIN)
        LOG_ERROR;
     return newsock;
     }
  return -1;
}
#endif // VDRVERSNUM >= 20300
