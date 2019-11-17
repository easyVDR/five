/***************************************************************************
 *   (c) Copyright 2012 R. Geigenberger / Frank Kohlmann                   *
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

// to get the definition for strptime
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "Command_Defines.h"
#include "yard2wakeup.h"
#include "global_config.h"

unsigned char buffer[BUFFER_LENGTH];
int config_sock;


// Convert binary coded decimal <-> normal decimal numbers
unsigned char decToBcd(int val)
{
  return ( (val/10*16) + (val%10) );
}

unsigned char bcdToDec(int val)
{
  return ( (val/16*10) + (val%16) );
}


void exit_handler(int sig) {
  cleanup_and_exit(0);
}

void cleanup_and_exit(int exitstate)
{
  close(config_sock);
  exit(exitstate);
}


void WriteRTCbyte(unsigned char adr, unsigned char val)
{
	buffer[0] = SYS_RTC_CMOS_W;
	buffer[1] = 255-buffer[0];
	buffer[2] = adr;
	buffer[3] = 1;
	buffer[4] = val;
	send(config_sock, buffer, 5, MSG_NOSIGNAL);
}
void ReadRTCWUTime(void)
{
    int byear = 2000;

    buffer[0] = SYS_RTC_WU_R;
    buffer[1] = 255 - buffer[0];
    buffer[2] = 0;
    buffer[3] = 7;
    send(config_sock, buffer, 4, MSG_NOSIGNAL);
    // wait for ack
    if (read(config_sock,buffer,BUFFER_LENGTH) < 0)
    {
        printf("Communication error!\n");
        cleanup_and_exit(-1);
    }
    byear = byear + bcdToDec(buffer[9]);
    printf ( "Y.A.R.D.2 Next Wakeup Time/Date: %d:%d:%d - %d.%d.%d\n",bcdToDec(buffer[5]), bcdToDec(buffer[4]), bcdToDec(buffer[3]),bcdToDec(buffer[7]), bcdToDec(buffer[8]), byear);
}


void WriteRTCWUTime(time_t newtime)
{
	struct tm *yardtime;
	
	yardtime = localtime( &newtime );

	buffer[0] = SYS_RTC_WU_W;   // I2C
	buffer[1] = 255-buffer[0];   // I2C

	buffer[2]   = 7; //Wakeup adr
	buffer[3]   = 7; //len

	if(newtime > 0)
	{
		buffer[4]  = 0; //no seconds for Wakeup time
		buffer[5]  = decToBcd(yardtime -> tm_min);   //RTC 1 minute
		buffer[6]  = decToBcd(yardtime -> tm_hour);  //RTC 2 hour
		buffer[7]  = decToBcd(yardtime -> tm_wday);  //RTC 3 Dayofweek
		buffer[8]  = decToBcd(yardtime -> tm_mday);  //RTC 4 day
		buffer[9]  = decToBcd(yardtime -> tm_mon + 1); //RTC 5 month; month starts from 0=Jan
		buffer[10] = decToBcd( (yardtime -> tm_year - 100 ) ); //RTC 6 year; years from 1900 --> 2014 = 114 --> 14
	}
	else
	{
		buffer[4]  = 0; //Disbae wakeup
		buffer[5]  = 0;
		buffer[6]  = 0;
		buffer[7]  = 0;
		buffer[8]  = 1;
		buffer[9]  = 1;
		buffer[10] = 0;
	}
	

	send(config_sock, buffer, 11, MSG_NOSIGNAL);
	// wait for ack
	if (read(config_sock,buffer,BUFFER_LENGTH) < 0)
	{
		printf("Communication error!\n");
		cleanup_and_exit(-1);
	}

	if(buffer[0]=='1')	printf("ack %d \n",buffer[0]);
	//start RTC clock 

	WriteRTCbyte(7,16);  //RTC clock setting
}

void printusage(void)
{
	printf("\n");
	printf("yard2wakeup usage:\n");
	printf("yard2wakeup [mode] [Wakeup time] [time adjust] [-Poff] \n");
	printf("yard2wakeup: -C: Read current wake up time / date from Y.A.R.D.2\n");
	printf("Time string: -S \"dd.mm.yyyy hh:mm\"\n");
	printf("Time time_t: -I 1234567890\n");
	printf("\n");
	printf("Example1: yard2wakeup -S \"01.04.2014 17:20\"\n");
	printf("Example2: yard2wakeup -S \"01.01.2000 00:00\" (Disable wakeup)\n");
	printf("Example3: yard2wakeup -I 1396365600\n");
	printf("          Start on Tue Apr 1 17:20:00 2014");
	printf("Example4: yard2wakeup -I 0 (Disable wakeup)\n");
	printf("\n");
	printf("Time adjust: set Wakeup time -x minutes\n");
	printf("Example5: yard2wakeup -S \"01.04.2014 17:20\" 10\n");
	printf("          Start 10 minutes before:   17:10 \n");
	printf("Example6: yard2wakeup -I 1396365600 5\n");
	printf("          Start 5 minutes before: Tue Apr 1 17:20:00 2014\n");
	printf("\n");
	printf("-Poff: shutdown system after wakeup time set. Must be last argument ! (sudo)\n");
	printf("Example6: yard2wakeup -S \"10.06.2012 17:20\" 10 -Poff\n");
	printf("\n");
	printf("VDR -s option mode (start VDR with \"-s yard2wakeup\") - sets also -Poff\n");
	printf("yard2wakeup [nextwakeup UTC] [nextwakeup from now] [channel] [file] [reason] \n");
	printf("\n");
	
	cleanup_and_exit(0);
}

int main(int argc, char *argv[])
{  
  struct sigaction handler;
  struct sockaddr_un srvAddr;
  int srvAddrLen;
  int WUadjust = 0;
  struct tm *argtime;
  time_t yardwutime;	
  time_t currenttime;	
  int ReadNextWakeup = 0;
  int SetNextWUint = 0;
  int SetNextWUstr = 0;
  int SetDirectVDR = 0;
  int PowerOff = 0;
  int i = 0;


	//init argument time
	time(&currenttime);
	time(&yardwutime);
	argtime = localtime(&yardwutime);

	// Set seconds to 0, no sec. in wakeupdate
	argtime->tm_sec = 0;

	handler.sa_handler = exit_handler;
	if (sigfillset(&handler.sa_mask) <0 )
		cleanup_and_exit(-1);
	handler.sa_flags=0;
	if (sigaction(SIGINT, &handler,0) < 0)
		cleanup_and_exit(-1);

	if( (argc==1 ) || (argc > 6) ) printusage();
	
	for(i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-Poff") == 0) PowerOff = 1;
	}


	if( (argc==2) && (strcmp(argv[1],"-C") == 0) ) ReadNextWakeup = 1;
	if( (strcmp(argv[1],"-I") == 0) ) SetNextWUint = 1;
	if( (strcmp(argv[1],"-S") == 0) ) SetNextWUstr = 1;


	//assume that yard2wakeup is called directly from vdr shutdown option "-s yard2wakeup"
	if(argc == 6) // VDR call yard2wakeup with 5 parameters
	{
		SetDirectVDR = 1;
		yardwutime = strtol(argv[1], NULL, 10);
		// remove seconds from given time
		yardwutime = (yardwutime / 60) * 60;
		PowerOff = 1; // shutdown system
	}
	
	if ((ReadNextWakeup + SetNextWUint + SetNextWUstr + SetDirectVDR) != 1) printusage();


	if (SetNextWUint == 1)
	{
		yardwutime = strtol(argv[2], NULL, 10);
		// remove seconds from given time
		yardwutime = (yardwutime / 60) * 60;
	}
	

	if (SetNextWUstr == 1)
	{
		if(strptime(argv[2], "%d.%m.%Y %H:%M", argtime) == 0) printf("o");//printusage();
		if(argtime->tm_year == 100) yardwutime = 0;

		argtime->tm_isdst = -1; //prevent automatic converison
		yardwutime = mktime(argtime); 
	}

//printf("Time: %d\n",yardwutime);
// 1396365600 = 01.04.2014 17:20 //for testing
	
	//modify time 
	if ( ((SetNextWUint + SetNextWUstr + SetDirectVDR) == 1) && (yardwutime != 0) )
	{


		//start x min before given wakeup with default in global_config.h
		yardwutime = yardwutime - GLOBAL_WUADJUST * 60;

		//start x min before given wakeup
		if((argc == 4) && (strcmp(argv[3],"-Poff") != 0))
		{
			WUadjust = atoi(argv[3]);
			yardwutime = yardwutime - WUadjust * 60;
		}

		//Check if new time is in the past if time > 0
		if( (yardwutime > 0) && (yardwutime < currenttime) ) 
		{
			printf ("New wakeup time is in the past: %s\n", ctime (&yardwutime) );
			cleanup_and_exit(-1);
		}

	}

	
// Establish connection to YARD server if time is in right format
	bzero( (char *)&srvAddr, sizeof(srvAddr));
	srvAddr.sun_family = AF_UNIX;
	strcpy(srvAddr.sun_path, CONFIG_SERVER_PATH);
	srvAddrLen = strlen(srvAddr.sun_path) + sizeof(srvAddr.sun_family);
	
	config_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (config_sock < 0)
	{
		printf("Setwakeup: Can't create socket ! \n");
		cleanup_and_exit(-1);
	}
	
	if (connect(config_sock, (struct sockaddr *)&srvAddr, srvAddrLen) < 0)
	{
		printf("Setwakeup: Can't connect to yardsrv !\n");
		cleanup_and_exit(-1);
		return 0;
	}


    //read wake up from yard and exit
	if (ReadNextWakeup == 1)
	{
		ReadRTCWUTime();
        	cleanup_and_exit(0);
	}


	if ((SetNextWUint + SetNextWUstr) == 1)
	{
		if(yardwutime > 0)
			printf ("New wakeup time: %s", ctime (&yardwutime) );
		else
			printf ("Wake up disabled\n");
			
		//printf("%d.%d.%d %d:%d\n",argtime->tm_mday,argtime->tm_mon,argtime->tm_year,argtime->tm_hour,argtime->tm_min);
		WriteRTCWUTime(yardwutime);
	}

	
	//todo how to shutdown vdr, need sudo
	//direct call from VDR -> Shutdown if yardwuwakeup is in the future
	//normally done via sh script
	if( PowerOff == 1)
	{
		if (system("shutdown -h now") == -1)
		{
			printf("Couldn't shutdown!\n");
			cleanup_and_exit(-1);
		}
	}

	cleanup_and_exit(0);

	return 0;  // can't be reached; needed to get rid of a warning
}

