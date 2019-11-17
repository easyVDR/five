/***************************************************************************
 *   Copyright (C) 2009 by M. Feser                                        *
 *   Modified to work with Y.A.R.D.2 USB                                   *
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "Command_Defines.h"
#include "yard2config.h"
#include "util.h"
#include "global_config.h"

int max_fd;
fd_set rfds;

unsigned char buffer[BUFFER_LENGTH];
int config_sock;
int yardtype = 0;
int yardlcd  = 0;
int selectpos=12;
int LCDx, LCDy, LCDtype, LCDmode, LCDBLmin, LCDBLmax, LSmin, LSmax, LCDfont, ENCtype, Mainsetting,Startup,LEDstatus;
int currentmenu = 0;

// Convert binary coded decimal <-> normal decimal numbers


void exit_handler(int sig) {
  cleanup_and_exit(0);
}

void cleanup_and_exit(int exitstate) {
  printf("cleanup ...\n");
  close(config_sock);
  clrscr(2,1);
//  unlink(CONFIG_SERVER_PATH);
  if(exitstate == 255) 
  {
	printf("Run: sudo yard2flash --program [FW file]\n");
	printf("After flashing Y.A.R.D. 2:\n");
	printf("Run: sudo yard2flash --reset\n");
  }
	
  if(exitstate!=100) exit(exitstate);
}

int yardconfig_init(void)
{
	struct sockaddr_un srvAddr;
	int srvAddrLen;


	// Establish connection to YARD server
	bzero( (char *)&srvAddr, sizeof(srvAddr));
	srvAddr.sun_family = AF_UNIX;
	strcpy(srvAddr.sun_path, CONFIG_SERVER_PATH);
	srvAddrLen = strlen(srvAddr.sun_path) + sizeof(srvAddr.sun_family);
	
	config_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (config_sock < 0)
	{
		printf("yardconfig: Can't create socket ! \n");
		cleanup_and_exit(-1);
		return 0;
	}
	
	if (connect(config_sock, (struct sockaddr *)&srvAddr, srvAddrLen) < 0)
	{
		printf("yardconfig: Can't connect to yardsrv !\n");
		cleanup_and_exit(-1);
		return 0;
	}


	return 1;
}

int yardconfig_deinit(void)
{
	// Unregister IR code notification
	// not used in yard2 yard_sendSrvCmd(SRVCMD_IRUNREG);
	
	// Close socket
	close(config_sock);
	config_sock = -1;
	return 1;
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

void WriteEEPROMbyte(unsigned char adr, unsigned char val)
{
	buffer[0] = SYS_EEPROM_W_BYTE;
	buffer[1] = 255-buffer[0];
	buffer[2] = adr;
	buffer[3] = 1;
	buffer[4] = val;
	send(config_sock, buffer, 5, MSG_NOSIGNAL);
}

void WriteEEPROMData(unsigned char adr, unsigned char* val, unsigned char len)
{
	memcpy(&buffer[4],&val[0],len);
	
	buffer[0] = SYS_EEPROM_W;
	buffer[1] = 255-buffer[0];
	buffer[2] = adr;
	buffer[3] = len;

	send(config_sock, buffer,len + 4, MSG_NOSIGNAL);
}


void ReadEEPROM(unsigned char adr, unsigned char len)
{
	if(len==1) buffer[0] = SYS_EEPROM_R_BYTE;
	else       buffer[0] = SYS_EEPROM_R;


	buffer[1] = 255-buffer[0];
	buffer[2] = adr;
	buffer[3] = len;
	send(config_sock, buffer, 4, MSG_NOSIGNAL);
	// wait for ack
	if (read(config_sock, buffer, BUFFER_LENGTH) < 0)
	{
		printf("Communication error!\n");
		cleanup_and_exit(-1);
	}
}

void WriteRTCTime(unsigned char adr, time_t newtime)
{
	struct tm *yardtime;
	
	yardtime = localtime( &newtime );

	if(adr == 0)
	{
		buffer[0] = SYS_RTC_TIME_W;   // I2C
		buffer[4] = decToBcd(yardtime->tm_sec); //RTC 0 seconds
	}
	else
	{
		buffer[0] = SYS_RTC_WU_W;   // I2C
		buffer[4] = 0; //no seconds for Wakeup time
	};

	buffer[1] = 255-buffer[0];
	buffer[2]   = adr; //adr
	buffer[3]   = 7; //len

	buffer[5] = decToBcd(yardtime->tm_min);   //RTC 1 minute
	buffer[6] = decToBcd(yardtime->tm_hour);  //RTC 2 hour
	buffer[7] = decToBcd(yardtime->tm_wday);  //RTC 3 Dayofweek
	buffer[8] = decToBcd(yardtime->tm_mday);  //RTC 4 day
	buffer[9] = decToBcd(yardtime->tm_mon+1); //RTC 5 month
	buffer[10] = decToBcd( (yardtime->tm_year - 100 ) ); //RTC 6 year

	send(config_sock, buffer, 11, MSG_NOSIGNAL);
	if (read(config_sock,buffer,BUFFER_LENGTH) < 0)
	{
		printf("Communication error!\n");
		cleanup_and_exit(-1);
	}
	if(buffer[0]=='1')	printf("ack %d \n",buffer[0]);
	
/*	done now in Firmware
	//start RTC clock 
	WriteRTCbyte(7,16);  //RTC clock setting
*/
}

int Readinput(unsigned char maxvalue)
{
char ivalue[10];	
int newvalue;

	if (fgets(ivalue,sizeof(ivalue),stdin) == NULL)
	{
		printf("Input error!\n");
		cleanup_and_exit(-1);
	}
	newvalue = (unsigned int)strtol(ivalue, NULL, 10);
	if((newvalue >=0) && (newvalue<=maxvalue)) newvalue = newvalue *25; //backlight is stored in range 0..1000 in 25steps
	else newvalue = -1;
	return newvalue;
}


void ReadLS(void)
{
    buffer[0] = SYS_READ_LS;
    buffer[1] = 255-buffer[0];
	send(config_sock, buffer, 2, MSG_NOSIGNAL);
	if (read(config_sock, buffer, BUFFER_LENGTH) < 0)
	{
		printf("Communication error!\n");
		cleanup_and_exit(-1);
	}
}

void SaveYardConfig(void)
{
	unsigned char LCDhigh;
	
	LCDhigh = 0; //Save 10 Bit value of 4 items to 5 bytes
	LCDhigh = LCDhigh + ( (LCDBLmax >> 8) 	& 0x3); 	//00000011
	LCDhigh = LCDhigh + ( (LCDBLmin >> 6) 	& 0xC); 	//00001100
	LCDhigh = LCDhigh + ( (LSmax >> 4) 		& 0x30); 	//00110000
	LCDhigh = LCDhigh + ( (LSmin >> 2) 		& 0xC0); 	//11000000

	buffer[0] = Mainsetting;
	buffer[1] = LCDmode; //Configtype only important for Windows version
	buffer[2] = LCDy;
	buffer[3] = LCDx;
	buffer[4] = LCDBLmax;
	buffer[5] = LCDBLmin;
	buffer[6] = LSmax;
	buffer[7] = LSmin;
	buffer[8] = LCDhigh;
	buffer[9] = LCDtype;
	buffer[10]= 0;		//repealy not implemented in FW now
	buffer[11]= (LCDfont << 4) + ENCtype;
	buffer[12]= LEDstatus;
	
	WriteEEPROMData(0,buffer,13);

}

void ResetYARD(void)
{
	buffer[0] = SYS_RESET;
	buffer[1] = 255-buffer[0];
	buffer[2] = 0x55;
	send(config_sock, buffer,3, MSG_NOSIGNAL);
}

void printmainsettings(void)
{
	gotoxy(0,16);
	printf(" Use LCD 8Bit mode             : "); if(CHECK_BIT(Mainsetting,0)) printf("1\n"); else printf("0\n");
	printf(" Show LCD time in suspend      : "); if(CHECK_BIT(Mainsetting,1)) printf("1\n"); else printf("0\n");
	printf(" Show welcome                  : "); if(CHECK_BIT(Mainsetting,2)) printf("1\n"); else printf("0\n");
	printf(" LCD Testmode                  : "); if(CHECK_BIT(Mainsetting,3)) printf("1\n"); else printf("0\n");
	printf(" No Power on Reset             : "); if(CHECK_BIT(Mainsetting,4)) printf("1\n"); else printf("0\n");
	printf(" Use Lightsensor               : "); if(CHECK_BIT(Mainsetting,5)) printf("1\n"); else printf("0\n");
	printf(" Use IR WakeUp also as shutdown: "); if(CHECK_BIT(Mainsetting,6)) printf("1\n"); else printf("0\n");
}


void printSettingsmenu(void)
{
	gotoxy(0,4);
	clrscr(0,0);
	currentmenu=1;
	selectpos = 6;
	
	printf("Cur. Config:");
	printf(" ------------------------------------------------------------\n");
	if(yardlcd==1) //YARD LCD version
	{
		selectpos=10;
		printf(" A: Use 8Bit mode for LCD (HD44780/KS0073)\n");
		printf(" B: Show time on LCD in suspend/off\n");
		printf(" C: Show own welcome message \n");
		printf(" D: LCD Testmode (Time, IR, Power status\n");
	}
	
	printf(" E: Power on Reset (AC lost)\n");
	
	if(yardlcd==1) //YARD LCD version
	{
		printf(" F: Use lightsensor for auto backlight adjustment\n");
	}
	
	printf(" G: Use IR WakeUp also as shutdown command\n");

	printf(" ------------------------------------------------------------\n");
	printf(" M: Main Menu\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printmainsettings();
}

void printLCDmenu(void)
{
	gotoxy(0,4);
	clrscr(0,0);
	currentmenu=2;
	printf(" Cur. Config: X:%d-Y:%d LCDCont.:%d EncType:%d\n",LCDx,LCDy,LCDtype,ENCtype);
	printf(" Cur. Config: BLmin/max: %d-%d LSmin/max: %d-%d \n",LCDBLmin,LCDBLmax,LSmin,LSmax);
	printf(" ------------------------------------------------------------\n");
	printf(" c: Change Controller (0=HD44780 1=KS0066 2=KS0108 3=T6963)\n");
	printf(" x: Cycle Res.X: 8,16,20,24,27,40,128,160,192,240 \n");
	printf(" y: Cycle Res.Y: 1,2,4,64,128,160\n");
	printf("    LCD backlight:   s: Min value - t: Max value (0..40)\n");
	printf("    Set Lightsensor: u: Min value - v: Max value (Use current value)\n");
	printf(" w: Set Rotary Encoder type (0=disable, 1,2,4(3) events/step)\n");
	printf(" t: Cycle LCD Testmode on/off\n");
	printf(" ------------------------------------------------------------\n");
	printf(" M: Main Menu\n");
	printf("\n");
}

void printLEDmenu(void)
{
	gotoxy(0,4);
	clrscr(0,0);
	currentmenu=3;
	printf(" Current LCD config: %d\n",LEDstatus);
	printf(" ------------------------------------------------------------\n");
	printf(" 0: LED OFF (recommended)\n");
	printf(" 1: LED ON\n");
	printf(" 2: LED 1 second blink\n");
	printf(" 3: Blink if IR WakeUp code received\n");
	printf(" 4: LED ON when System is switched off (StandBy)\n");
	printf(" 5: Blink during IR receive\n");
	printf(" 6: Blink during IR receive (if system is ON)\n");
	printf(" ------------------------------------------------------------\n");
	printf(" M: Main Menu\n");
	printf("\n");
}

void printmainmenu(void)
{
	gotoxy(0,4);
	clrscr(0,0);
	currentmenu=0;
	printf(" Main config (all options case sensitive)\n");
	printf(" ------------------------------------------------------------\n");
	printf(" 1: Show main settings\n");
	if(yardtype != 9)
	{
	printf(" 2: Set time\n");
	}
	printf(" 3: Get IR wakeup code\n");
	printf(" 4: Set IR wakeup code (autosave)\n");
	if(yardlcd == 1)
	{
	printf(" 5: LCD Settings\n");
	}
	printf(" 6: LED Setting\n");
	printf(" 7: Get FORCE OFF IR code\n");
	printf(" 8: Set FORCE OFF IR code (autosave)\n");
	printf(" S: --> After setup --> Save all Settings --> RESET\n");
	printf(" R: RESET Y.A.R.D.2\n");
	printf(" P: Test Power button switch -> system will shutdown !!!\n");
	printf(" U: Enter Update mode -> run: sudo yard2flash --program [filename]\n");
	printf(" 0: Exit\n");
	printf("\n");
}

void yardconfigmenu(void)
{
	char ch  = 0;
	char ch2 = 0;
	//unsigned char buffer[40];
	time_t timevar;
	struct timeval tv = {1, 0};   // sleep for 1 second!
	int i;
	
	selectpos = 15; //is max value
	printmainmenu();
	
	do
	{

		gotoxy(0,selectpos + 4);
		printf("                       ");
		gotoxy(0,selectpos + 4);
		printf(" Select: ");
	
		ch = fgetc(stdin);
		ch2 = fgetc(stdin); //remove 0x10 from stdin
		clrscr(0,0);
		printf("\n");
		
		switch(ch)
		{
			case 'M':
			selectpos = 15;
			printmainmenu();
			break;
		default:break;
		}

		if(currentmenu==0)
		{
			switch(ch)
			{
				case '0':
				//exit program
					cleanup_and_exit(0);
					break;
		
				case '1':
					printSettingsmenu();
					break;

				case '2':
					if(yardtype == 9)
					{
						printf("Time setting not available for MICRO version");
						break;
					}

					time( &timevar );
					WriteRTCTime(0,timevar);
					WriteRTCbyte(7,16);  //RTC clock setting
					printf("Time set");
					break;
					
			
				case '3':
					//get IR wake up code
					ReadEEPROM(24,8); //EE_IRWakeup
		
					printf("IR Wakeup code : ");
					printhex(&buffer[4],8); //4,8
					printf("\n");
					break;
		
				case '4':
					printf("Press IR Button you want to use for Wakeup:\n");
					i = read(config_sock,buffer,BUFFER_LENGTH); //wait for ack
					printf("IR Wakeup code : ");
					printhex(&buffer[1],8);
					printf("\n");

					buffer[i]   = 0;
					buffer[i+1] = 0;
					if( (buffer[1] > 30) || (buffer[1] == 0) ) printf("Incorrect IR Code, try again");
					else WriteEEPROMData(24,&buffer[1],8);

					//empty queue from yard2srvd
					i = 1;
					while(i > 0)
					{				        
				        FD_ZERO(&rfds);
        				FD_SET(config_sock, &rfds);
				    	max_fd = config_sock;
				        i = select(max_fd+1, &rfds, NULL, NULL, &tv);
					   if (FD_ISSET(config_sock, &rfds)) {
					      i = read(config_sock,buffer,BUFFER_LENGTH);
					   }
					}


					break;
		
				case '5':
					if(yardlcd==0)
					{
						printf("LCD settings only for Y.A.R.D. LCD version");
						break;
					}
					
					selectpos=12;
					printLCDmenu();

					break;

				case '6':
					selectpos=11;
					printLEDmenu();
					break;

				case '7':
					//get FORCE OFF IR code
					ReadEEPROM(16,8); //EE_IRWakeup
		
					printf("FORCE OFF IR code : ");
					printhex(&buffer[4],8); //4,8
					printf("\n");
					break;

				case '8':
					printf("Press IR Button you want to use for FORCE OFF:\n");
					i = read(config_sock,buffer,BUFFER_LENGTH); //wait for ack
					printf("FORCE OFF IR code : ");
					printhex(&buffer[1],8);
					printf("\n");

					buffer[i]   = 0;
					buffer[i+1] = 0;
					if( (buffer[1] > 30) || (buffer[1] == 0) ) printf("Incorrect IR Code, try again");
					else WriteEEPROMData(16,&buffer[1],8);

					//empty queue from yard2srvd
					i = 1;
					while(i > 0)
					{				        
				        FD_ZERO(&rfds);
        				FD_SET(config_sock, &rfds);
				    	max_fd = config_sock;
				        i = select(max_fd+1, &rfds, NULL, NULL, &tv);
					   if (FD_ISSET(config_sock, &rfds)) {
					      i = read(config_sock,buffer,BUFFER_LENGTH);
					   }
					}


					break;
		
				case 'P': //Test Power button switch
					printf("System must be configured to shutdown if Power on button is pressed\n");
					printf("If the system will not shutdown, check cable position -> Manual\n");
					printf("WARNING - System will shutdown if you continue !\n"); 
					printf("Continue Y/N:"); 
					ch = fgetc(stdin);
					ch2 = fgetc(stdin); //remove 0x10
					if(ch=='Y')
					{
						buffer[0] = SYS_TestMBSW;
						buffer[1] = 255-buffer[0];
						send(config_sock, buffer, 2, MSG_NOSIGNAL);
					}
					yardconfigmenu();
					break;
				case 'S':
					//Save config to yard
					SaveYardConfig();
					printf("You have to reset Y.A.R.D.2 to activate changes, Press R !\n"); 
					printf("Y.A.R.D daemon and config will shutdown, you have to start it manually !"); 
					break;

				case 'R':
					//Reset yard
					ResetYARD();
					cleanup_and_exit(0);
					break;
					
				case 'U':
					WriteEEPROMbyte(254,0xFF);
					sleep(2);
					ResetYARD();
					cleanup_and_exit(255);
					break;
				default:break;
			}
		}

		else if(currentmenu==1) //Mainsettings
		{
			switch(ch)
			{

				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
					ch2 = ch-65; //ascii A=65 --> convert into 0..7

					if(CHECK_BIT(Mainsetting,ch2))
						CLR_BIT(Mainsetting,ch2);
					else
						SET_BIT(Mainsetting,ch2);

						printSettingsmenu();
					break;
				default:
					printSettingsmenu();
					break;
			}
		}
			
		else if(currentmenu==2) //LCDmenu
		{
			switch(ch)
			{

				/*        LCD Settings */
				case 'c':
					LCDtype++;
					if(LCDtype==4) LCDtype = 0;
					printLCDmenu(); //repaint menu for new value
					break;

				case 's':
					//LCD backlight min
					printf("Enter minimum Backlight:\n");
					i = Readinput(40);
					if(i>0) LCDBLmin = i;
					printLCDmenu();
					break;
					
				case 't':
					//LCD backlight max
					printf("Enter maximum Backlight:\n");
					i = Readinput(40);
					if(i>0) LCDBLmax = i;
					printLCDmenu();
					break;
				case 'u':
					ReadLS();
					LSmin = (buffer[1] << 8) + buffer[2];
					printf("LS min value: %d",LSmin);
					printLCDmenu();
					break;

				case 'v':
					ReadLS();
					LSmax = (buffer[1] << 8) + buffer[2];
					printf("LS max value: %d",LSmax);
					printLCDmenu();
					break;

				case 'w':
					ENCtype++;
					if(ENCtype==4) ENCtype = 0;
					printLCDmenu(); //repaint menu for new value
					break;

				case 'x':
					switch(LCDx)
					{
						case 8  : LCDx = 16;  break;
						case 16 : LCDx = 20;  break;
						case 20 : LCDx = 24;  break;
						case 24 : LCDx = 27;  break;
						case 27 : LCDx = 40;  break;
						case 40 : LCDx = 128; break;
						case 128: LCDx = 160; break;
						case 160: LCDx = 192; break;
						case 192: LCDx = 240; break;
						case 240: LCDx = 8;   break;
						default : break;
					}
					printLCDmenu(); //repaint menu for new value
			
					break;

				case 'y':
					switch(LCDy)
					{
						case 1  : LCDy = 2;   break;
						case 2  : LCDy = 4;   break;
						case 4  : LCDy = 64;  break;
						case 64 : LCDy = 128; break;
						case 128: LCDy = 160; break;
						case 160: LCDy = 1;   break;
						default : break;
					}
					printLCDmenu(); //repaint menu for new value

					break;

				default:
					printLCDmenu(); //repaint menu for new value
					break;
			} //switch
		}

		else if(currentmenu==3) //LED Seting
		{
			switch(ch)
			{

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
					ch2 = ch-48; //ascii 1=48 --> convert into 0..5
					if(ch2<0) ch2 = 0;
					LEDstatus=ch2;
					printLEDmenu();
					break;
				default:
					printLEDmenu();
					break;
			}
		}

		
	} while(ch != 0);
}

int main(int argc, char *argv[])
{  
  struct sigaction handler;
  // struct tm *yardtime;
  // time_t timevar;
  int configbyte=0;

  handler.sa_handler = exit_handler;
  if (sigfillset(&handler.sa_mask) <0 )
    cleanup_and_exit(-1);
  handler.sa_flags=0;
  if (sigaction(SIGINT, &handler,0) < 0)
    cleanup_and_exit(-1);

	
//init all variables to 0
	Mainsetting = 0;	LCDx = 0; 	LCDy = 0;	LCDtype = 0;	LCDmode = 0;	LCDBLmin = 0;	LCDBLmax = 0;	LSmin = 0;	LSmax = 0;	LCDfont = 0;	ENCtype = 0;	Startup = 0;
	
	yardconfig_init();
	clrscr(2,1);
	
//get FW version and type
	buffer[0] = YARD_GET_VERSION;
	buffer[1] = 255 - buffer[0];
	send(config_sock, buffer, 2, MSG_NOSIGNAL);
	// wait for ack
	if (read(config_sock,buffer,BUFFER_LENGTH) < 0)
	{
		printf("Communication error!\n");
		cleanup_and_exit(-1);
	}
	printf("Y.A.R.D.2 Version: %d.%d.%d ", buffer[2], buffer[3], buffer[4]);
	if(buffer[6] != 0) 	printf("Beta %d", buffer[6]);
	if(buffer[5] == 2) 	{printf(" LCD"); 	yardlcd = 1;}  	//2 = lcd
	if(buffer[5] == 3) 	{printf(" LCD SMD"); 	yardlcd = 1;}  	//3 = lcd SMD
	if(buffer[5] == 7) 	{printf(" LCD SMD"); 	yardlcd = 1;}  	//3 = lcd SMD
	if(buffer[5] == 9) 	printf(" MICRO");  	//9 = lcd MICRO
	else				printf(" Mini"); //1 = mini
	yardtype = buffer[5];
	
	if(yardtype==0)
	{
		printf("Error getting Y.A.R.D.2 info");
		cleanup_and_exit(0);
	}
	
/*
	time( &timevar );
	yardtime = localtime( &timevar );
	printf ( "Current  date/time: %s", asctime (yardtime) );
*/

//read startup reason
	buffer[0] = SYS_START_UP;
	buffer[1] = 255-buffer[0];
	send(config_sock, buffer, 2, MSG_NOSIGNAL);
	// wait for ack
	if (read(config_sock,buffer,BUFFER_LENGTH) < 0)
	{
		printf("Communication error!\n");
		cleanup_and_exit(-1);
	}
	printf("  Startup: "); //write startup behind YARD version
	switch(buffer[1])
	{
		case 1:  printf("IR Power on\n");		break;
		case 2:  printf("RTC Wakeup Power on\n");	break;
		case 3:  printf("Power on Reset !\n");		break;
		case 4:  printf("USER RESET or FW UPDATE !\n");	break;
		case 5:  printf("Disabled Power on Reset\n");	break;
		case 6:  printf("Disabled Power on Reset RTC\n");break;
		case 7:  printf("Power on Reset RTC!\n");	break;
		case 8:	 printf("User Power on\n");		break;
		case 10: printf("No new wakeup since last start\n");break;
		case 0:
		case 255:
			printf("ERROR ! NOT SET\n");
		default:
			printf("FW ERROR ! This should not happen!\n");
		break;
	}
	
	if(yardtype != 9) //MICRO
	{
//get RTC time from YARD
	buffer[0] = SYS_RTC_TIME_R;
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
	printf ( "Y.A.R.D.2 Time/Date: %d:%d:%d - %d.%d.20%d\n",bcdToDec(buffer[5]), bcdToDec(buffer[4]), bcdToDec(buffer[3]),bcdToDec(buffer[7]), bcdToDec(buffer[8]), bcdToDec(buffer[9]));
	
	} //yardtype !=9	

	ReadEEPROM(0,13); //get all main settings, read 12 bytes
	Mainsetting 	= buffer[4];
	LCDmode		= buffer[5];
	if(yardlcd==1) //YARD LCD version
	{
		//LCD resolution
		LCDy = buffer[6];
		LCDx = buffer[7];

		//Backlight and Lightsensor settings 10bit value
		configbyte = buffer[12];
	        LCDBLmax = 	((configbyte & 0x3)  << 8) + buffer[8];
	        LCDBLmin = 	((configbyte & 0xC)  << 6) + buffer[9];
	        LSmax = 	((configbyte & 0x30) << 4) + buffer[10];
	        LSmin = 	((configbyte & 0xC0) << 2) + buffer[11];
	
		//Display Controller type
		LCDtype 	= buffer[13];

		// Get Encoder Type
		ENCtype = buffer[15] & 0x0F;
		LCDfont = (buffer[15] & 0xF0) >> 4;

	}

	LEDstatus = buffer[16];
	yardconfigmenu();

	return 0;
}
