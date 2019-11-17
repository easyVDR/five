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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "lirctest.h"
#include "global_config.h"
#include "util.h"

//int fd_yard; 							//filedescriptor for yard HW
unsigned char buffer[BUFFER_LENGTH];
int lirc_sock,max_fd,maxcount;
fd_set rfds;


void exit_handler(int sig) {
  cleanup_and_exit(0);
}

void cleanup_and_exit(int exitstate)
{
  close(lirc_sock);
  exit(exitstate);
}

void printusage(void)
{
	printf("\n");
	printf("lirctest usage:\n");
	printf("Just start lirctest to see IR commands from lirc\n");
	printf("\n");
}

int main(int argc, char *argv[])
{  
  struct sigaction handler;
  struct sockaddr_un srvAddr;
  int srvAddrLen;
  int ret;


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
	strcpy(srvAddr.sun_path, LIRC_OWNSERVER);
	srvAddrLen = strlen(srvAddr.sun_path) + sizeof(srvAddr.sun_family);
	
	//create socket
	lirc_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (lirc_sock < 0)
	{
		printf("Can not create socket\n");
		cleanup_and_exit(-1);
		return 0;
	}
	
	//connect to lirc
	if (connect(lirc_sock, (struct sockaddr *)&srvAddr, srvAddrLen) < 0)
	{
		printf("Can not connext to lirc\n");
		cleanup_and_exit(-1);
		return 0;
	}

	//Test lirc commands: VERSION, LIST, LIST YARD2USB
	//get Version from Lirc Server
	send(lirc_sock, "VERSION\n", 8 , MSG_NOSIGNAL);

	while(1)
	{
		//Build rfds structure
        FD_ZERO(&rfds);
        //FD_SET(fd_yard, &rfds);
        if (lirc_sock != -1) FD_SET(lirc_sock, &rfds);
	  
	    max_fd = lirc_sock;
	  
        ret = select(max_fd+1, &rfds, NULL, NULL, NULL);
        maxcount++;
        printf("Maxcount %d, Ret %d \n",maxcount,ret); 

        if (ret <= 0)
	{
	  printf("Error during receive\n");
	  cleanup_and_exit(0);
	  /* Error or Signal (CTRL-C) */
	  break;
	}

        if (FD_ISSET(lirc_sock, &rfds))
	{
	  ret = read(lirc_sock,buffer,BUFFER_LENGTH);
	  if (ret > 0)
	    {
	      buffer[ret] = 0;
	      // Get a lirc event
	      printf("--> LIRC IR event \n"); 
	      printf("LIRC CMD:\n%s\n",buffer);

		//get RC List from Lirc Server
          if(strstr(buffer, "VERSION\n") != NULL){send(lirc_sock, "LIST\n", 5 , MSG_NOSIGNAL);}
		//get IR List from Lirc Server of YARD2USB

          if(strstr(buffer, "LIST\n") != NULL){send(lirc_sock, "LIST YARD2USB\n", 14 , MSG_NOSIGNAL);}
	    }
	  else
	    {
	      // socket closed
	      //close lirc socket and wait for new connect
	      printf("Socket closed \n"); 
	      close(lirc_sock);
	      lirc_sock = -1;
  	      cleanup_and_exit(0);
	    }

    }

	} //while
	


	cleanup_and_exit(0);
	return 0;
}
