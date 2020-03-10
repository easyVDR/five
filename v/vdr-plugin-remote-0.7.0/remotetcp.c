/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * remotetcp.c: tcp/telnet remote control
 *
 * Copyright (C) 2002-2015 Oliver Endriss <o.endriss@gmx.de>
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


#include "remote.h"
#include "remotetcp.h"


cTcpRemote::cTcpRemote(const char *name, int f, char *d)
    : cRemoteDevTty(name, f, d)
{
    cstatus = NULL;
    csock = NULL;
    Start();
}


cTcpRemote::~cTcpRemote()
{
    Cancel();

    if (cstatus)
    {
        delete cstatus;
        cstatus = NULL;
    }

    if (fh != -1)
    {
        close(fh);
        fh = -1;
    }

    if (csock)
    {
        delete csock;
        csock = NULL;
    }
}		  


uint64_t cTcpRemote::getKey(void)
{
    if (csock == NULL)
    {
        int port;
        sscanf (device, "tcp:%d", &port);
        csock = new cSocketRemote(port);
        if (! csock)
        {
            esyslog("error creating socket");
            Cancel();
        }

        if (! csock->Open())
        {
            esyslog("error opening socket");
            Cancel();
        }
    }


    while (fh < 0)
    {
        usleep(100000);
        if (csock == NULL)
        	return INVALID_KEY;

        fh = csock->Accept();
        if (fh >= 0)
        {
            char str[80];
            // hack for telnet
            sprintf(str, "%c%c%c", 255,251,1); // WILL ECHO
            write(fh, str, strlen(str));
            sprintf(str, "%c%c%c", 255,251,3); // WILL SGA
            write(fh, str, strlen(str));

            sprintf(str, "VDR Remote Plugin - %s\n\r", device);
            write(fh, str, strlen(str));
            // skip telnet stuff
            read(fh, str, sizeof str);

            cstatus = new cTtyStatus(fh);
        }
    }


    uint64_t key = cRemoteDevTty::getKey();

    if (key == INVALID_KEY)
    {
        if (cstatus)
        {
            delete cstatus;
            cstatus = NULL;
        }
        close(fh);
        fh = -1;
    }

    return key;
}
