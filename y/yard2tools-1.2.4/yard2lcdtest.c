/***************************************************************************
 *   (c) Copyright 2013 R. Geigenberger                                    *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "yard2lcdtest.h"
#include "global_config.h"
#include "util.h"

int fd_yard; 							//filedescriptor for yard HW
unsigned char buffer[BUFFER_LENGTH];	//Buffer for socket
int LCD_sock, max_fd, maxcount;
fd_set rfds;

void exit_handler(int sig) {
  cleanup_and_exit(0);
}

void cleanup_and_exit(int exitstate)
{
  close(LCD_sock);
  exit(exitstate);
}


void printusage(void)
{
	printf("\n");
	printf("yard2LCDtest usage:\n");
	printf("Start yard2LCDtest\n");
	printf("Enter Command\n");
	printf("C = Clear - C\n");
	printf("G[line] - G0 (set cursor to line 0)\n");
	printf("W[Text to write] - WHalloWorld\n");
	printf("\n");
	
//	cleanup_and_exit(0);
}

int main(int argc, char *argv[])
{  
  struct sigaction handler;
  struct sockaddr_un srvAddr;
  int srvAddrLen;
  char lcdinput[80];
  int rc,sendlen;
  
	handler.sa_handler = exit_handler;
	if (sigfillset(&handler.sa_mask) <0 )
		cleanup_and_exit(-1);
	handler.sa_flags=0;
	if (sigaction(SIGINT, &handler,0) < 0)
		cleanup_and_exit(-1);

	printusage();

// Establish connection to LIRC server
	bzero( (char *)&srvAddr, sizeof(srvAddr));
	srvAddr.sun_family = AF_UNIX;
	strcpy(srvAddr.sun_path, LCD_SERVER_PATH);
	srvAddrLen = strlen(srvAddr.sun_path) + sizeof(srvAddr.sun_family);
	
	//create socket
	LCD_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (LCD_sock < 0)
	{
		printf("Can not create LCD socket\n");
		cleanup_and_exit(-1);
		return 0;
	}
	
	//connect to lirc
	if (connect(LCD_sock, (struct sockaddr *)&srvAddr, srvAddrLen) < 0)
	{
		printf("Can not connext to yard2srvd LCD socket\n");
		cleanup_and_exit(-1);
		return 0;
	}

	while(1)
	{
		memset(&lcdinput, 0, sizeof(lcdinput)); // reset buffer
		sendlen = 0;
		printf("Enter LCD command: ");
		if (fgets(lcdinput,sizeof(lcdinput),stdin) == NULL)
		{
			printf("Input error!\n");
			cleanup_and_exit(-1);
		}
		printf("Command: %s\n",lcdinput);
		printf("Send command to LCD\n");
		if(lcdinput[0] == 'G') 
		{
			lcdinput[2] = lcdinput[1] - 48; //convert to 0..9 from char
			lcdinput[1] = 0; //only y position, x position always 0
			sendlen = 3;
		}

		if(lcdinput[0] == 'W') 
		{
			sendlen = strlen(lcdinput)-1; //already in right format; get length -2 #10#0
		}

		if(lcdinput[0] == 'C') 
		{
			sendlen = 1; //already in right format; get length
		}

		rc = 0;
		if(sendlen > 0) rc = send(LCD_sock, lcdinput, sendlen, MSG_NOSIGNAL);
		if (rc < 0 )
		{
			printf("Error while send LCD command to yard\n");
		}
	} //while

	cleanup_and_exit(0);
	return 0;
}
