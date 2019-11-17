/***************************************************************************
 *   Copyright (C) 2012 R. Geigenberger / Frank Kohlmann                   *
 *   Y.A.R.D.2 USB daemon                                                  *
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

//#define openelec

#include "release.h"
#include "Command_Defines.h"
#include "util.h"
#include "yard2srvd.h"
#include "inih/ini.h"
#include "global_config.h"
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <getopt.h>
#include <ctype.h>
#include <linux/input.h>
#include <linux/uinput.h>

#ifndef openelec
	#include <bsd/libutil.h>
#endif

/* global */
char *homedir;
char mapfile[255];
int fd_yard = -1; 						//filedescriptor for yard HW
int fd_uinput = -1;
int lirc_sock, lirc_listen_sock;
int lirc_ownserver_sock_listen;		    //own lirc server socket
int config_sock, config_listen_sock;	//config sockets
int lcd_sock, lcd_listen_sock;			//lcd sockets
struct connections lirc_connections[MAX_LIRC_CONNECTIONS];
struct irmap myirmap[MAX_IRMAP_SIZE];
struct yardconfig myyardconfig;
unsigned char LCDbuffer[BUFFER_LENGTH];
int amask;
int repeatdelay1st;
int repeatdelay2nd;
struct pidfh *pfh;

#define YARD_DEVICE_NAME_MAX  80
char yardDeviceName[YARD_DEVICE_NAME_MAX];
char *opt_yardDevName = 0;
char *opt_yardIrMap = 0;

BOOL clientConnected = FALSE;
unsigned long long lasttickcount,currenttickcount;
unsigned long long currentircode,lastircode;
unsigned char irrepeats;
int max_fd,ret;
fd_set rfds;
int maxcount;

/* define the INI file config struct */
typedef struct {
    #define CFG(s, n, default) char *s##_##n;
    #include "yard2srvd.ini.def"
} init_file_config_t;

/* create one and fill in its default values */
init_file_config_t InitConfig = {
    #define CFG_UNDEF "undef"
    #define CFG(s, n, default) default,
    #include "yard2srvd.ini.def"
};

#define getCFG(s, n) InitConfig.s##_##n
#define isCFG(s, n) ( strcmp(getCFG(s, n), CFG_UNDEF) != 0 )
#define isCFG_VAL(s, n, v) ( strcmp(getCFG(s, n), v) == 0 )
// #define checkCFG_VAL(s, n, v) ( isCFG(s, n) && isCFG_VAL(s, n, v) )

/* process a line of the INI file and store valid values */
static int cfg_handler(void *user, const char *section, const char *name,
					   const char *value)
{
	init_file_config_t *cfg = (init_file_config_t *)user;

    if (0) ;
    #define CFG(s, n, default) else if (strcmp(section, #s)==0 && \
        strcmp(name, #n)==0) cfg->s##_##n = strdup(value);
	#include "yard2srvd.ini.def"

    return 1;
}

#ifdef DEBUGSHOWCONFIG
/* print all the variables in the config, one per line */
static void dump_config(init_file_config_t *cfg)
{
    #define CFG(s, n, default) printf("%s_%s = %s\n", #s, #n, cfg->s##_##n);
	#include "yard2srvd.ini.def"
}
#define DUMP_CONFIG(cfg) dump_config(cfg);
#else
#define DUMP_CONFIG(cfg) do {} while(0)
#endif

static void close_yard_device(void)
{
	if (fd_yard != -1)
	{
		close(fd_yard);
		fd_yard = -1;
		sleep(2); // be sure that yard handle is closed
	}
}

/*
 * DESCRIPTION:     closes the yard device, if open and opens the given device name
 *                  and prints an error message, if open fails
 * TODO:            -
 */
static int open_yard_device(char* dev_name)
{
	close_yard_device();

	if (dev_name)
	{
		fd_yard = open(dev_name, O_NOCTTY | O_RDWR | O_DSYNC | O_RSYNC );//| O_NOCTTY | O_NONBLOCK);
		if (fd_yard == -1)
		{
			logging(LL_ERROR, "Cannot open yard device file %s: %d - %s\n", dev_name, errno, strerror(errno));
		}
	}
	return fd_yard;
}

static void exit_handler(int sig)
{
  cleanup_and_exit(0);
}

void cleanup_and_exit(int exitstate)
{
  logging(LL_INFO, "cleanup ...\n");
  
  if (fd_yard != -1)
  {
	  //Update RTC time
	  UpdateRTCTime();
  }

  close_yard_device();

  /* close all client the listening socket and unlink its file */

  if (fd_uinput != -1)
  {
	  ioctl(fd_uinput, UI_DEV_DESTROY);
	  close(fd_uinput);
	  fd_uinput = -1;
  }

  if ((lirc_sock != 1) || (lirc_listen_sock != -1))
  {
	  close(lirc_sock);
	  close(lirc_listen_sock);
	  unlink(LIRC_SERVER_PATH);
  }

  if (lirc_ownserver_sock_listen != -1)
  {
	  int i;

	  close(lirc_ownserver_sock_listen);
	  for ( i = 0; i < MAX_LIRC_CONNECTIONS; i++)
	  {
		if(lirc_connections[i].used)
		{
			close(lirc_connections[i].socket);
		}
	  }
	  unlink(LIRC_OWNSERVER);
  }

  close(config_sock);
  close(config_listen_sock);
  unlink(CONFIG_SERVER_PATH);

  close(lcd_sock);
  close(lcd_listen_sock);
  unlink(LCD_SERVER_PATH);

#ifndef openelec
  if (pfh != NULL)
  {
	  pidfile_remove(pfh);
	  pfh = NULL;
  }
#endif

  logclosesyslog();

  if(exitstate!=100) exit(exitstate);
}

#ifndef openelec
int open_pidfile()
{
	pid_t otherpid;
	int ret=0;

	pfh = pidfile_open(YARD_PIDFILE, 0666, &otherpid);
	if (pfh == NULL)
	{
		if (errno == EEXIST)
		{
			logging(LL_ERROR, "yard2srvd already running, pid:%d\n", otherpid);
			ret=-1;
		}
		else
		{
			logging(LL_WARNING, "Cannot open or create pidfile (start yard2srvd with sudo)\n");
		}
	}
	else
	{
		logging(LL_DEBUG, "Create pidfile " YARD_PIDFILE "\n");

		/* write the parent PID, so no other process can be started */
		pidfile_write(pfh);
	}
	return ret;
}
#endif

#ifdef openelec
int CheckProcess()
{
struct sockaddr_un srvAddr;
int srvAddrLen;
int test_sock;

	// Check if Socket already available
bzero( (char *)&srvAddr, sizeof(srvAddr));
srvAddr.sun_family = AF_UNIX;
strcpy(srvAddr.sun_path, CONFIG_SERVER_PATH);
srvAddrLen = strlen(srvAddr.sun_path) + sizeof(srvAddr.sun_family);

test_sock = socket(AF_UNIX, SOCK_STREAM, 0);
if (test_sock < 0)
{
	//Can not create socket -> someting wrong
	return 2;
}
else
   {
	if (connect(test_sock, (struct sockaddr *)&srvAddr, srvAddrLen) < 0)
	{
		logging(LL_DEBUG, "yard2srvd not running \n");
		return 0;
	}
	else
	{
		//Socket available --> yard2srvd running		
		logging(LL_DEBUG, "yard2srvd already running --> Exit \n");
		close(test_sock);
		test_sock = -1;
		return 1;
	}

   }
}
#endif


void StartDaemon()
{
  /* Our process ID and Session ID */
  pid_t pid, sid;

  /* pidfile have o be opened, when daemonizing */
#ifndef openelec
  if (pfh == NULL) exit(EXIT_FAILURE);
#endif

  /* Fork off the parent process */
  pid = fork();
  if (pid < 0) 
  {
	  logging(LL_WARNING, "Cannot daemonize\n");
#ifndef openelec
	  pidfile_remove(pfh);
#endif
	  exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then
     we can exit the parent process. */
  if (pid > 0)
  {
	  logging(LL_ALWAYS, "yard2srvd daemon with pid %d started successful\n", pid);
#ifndef openelec
	  pidfile_close(pfh);
#endif
	  exit(EXIT_SUCCESS);
  }

#ifndef openelec
  /* write now the child PID */
  pidfile_write(pfh);
#endif

  /* Change the file mode mask */
  umask(0);

  /* Open any logs here */        
                
  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0) 
  {
       cleanup_and_exit(EXIT_FAILURE);
  }

  /* Change the current working directory */
  if ((chdir("/")) < 0) 
  {
      cleanup_and_exit(EXIT_FAILURE);
  }

  /* Close out the standard file descriptors */
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

//Read EEprom data from yard
static void ReadEEPROM(unsigned char adr, unsigned char len)
{
 unsigned char buffer[4];

	if(len==1) buffer[0] = SYS_EEPROM_R_BYTE;
	else       buffer[0] = SYS_EEPROM_R;

	buffer[1] = 255-buffer[0];
	buffer[2] = adr;
	buffer[3] = len;
	send_data_to_yard(buffer, 4);
}

void UpdateRTCTime(void)
{
	struct tm *yardtime;
	time_t timevar;
	unsigned char buffer[20];	

	logging(LL_DEBUG, "Update RTC time\n");
	time( &timevar );
	yardtime = localtime( &timevar );

	buffer[0] = SYS_RTC_TIME_W;   // I2C
	buffer[1] = 255-buffer[0];
	buffer[2] = 0;
	buffer[3] = 7;
	buffer[4] = decToBcd(yardtime->tm_sec); 	//RTC 0 seconds
	buffer[5] = decToBcd(yardtime->tm_min);   	//RTC 1 minute
	buffer[6] = decToBcd(yardtime->tm_hour);  	//RTC 2 hour
	buffer[7] = decToBcd(yardtime->tm_wday);  	//RTC 3 Dayofweek
	buffer[8] = decToBcd(yardtime->tm_mday);  	//RTC 4 day
	buffer[9] = decToBcd(yardtime->tm_mon+1); 	//RTC 5 month
	buffer[10] = decToBcd( (yardtime->tm_year - 100 ) ); //RTC 6 year

	send_data_to_yard(buffer,11);
	logging(LL_DEBUG, "Update complete\n");
}


/*
 * DESCRIPTION:     Init listening socket ...
 *                  Wait for client connections ...
 *                  Return client socket on connection
 * 
 * SIDE EFFECTS:    calls cleanup_and_exit(...) on any error.
 *                    -> program termination
 * 
 * RETURNS:         Socket file descriptor of the client connection
 * TODO: description... constructive error handling
 */
int start_listen_socket(int id)
{
  int local_listen_sock, rc;
  struct sockaddr_un serv_addr;
  /*
   * create the server socket */

  logging(LL_DEBUG, "Create socket id=%d\n", id);
  amask = umask(0000); //change permission of sockets and folders

  local_listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (local_listen_sock < 0)
    {
      logging(LL_ERROR, "ERROR opening listen socket: %s", strerror(errno));
      cleanup_and_exit(-1);
    }
  
  /* init serv_addr struct */
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  if(id==0) strcpy(serv_addr.sun_path, LIRC_SERVER_PATH);
  if(id==1) strcpy(serv_addr.sun_path, CONFIG_SERVER_PATH);
  if(id==2) strcpy(serv_addr.sun_path, LCD_SERVER_PATH);
  if(id==3) strcpy(serv_addr.sun_path, LIRC_OWNSERVER);
  
  /* bind socket */

  rc = bind(local_listen_sock, (struct sockaddr *)&serv_addr, SUN_LEN(&serv_addr));
  if ( rc < 0)
    {
      logging(LL_ERROR, "ERROR on binding listen socket: %s", strerror(errno));
      cleanup_and_exit(-1);
    }
  
  /* set socket in listen state */
  rc = listen(local_listen_sock,1);
  if (rc < 0 )
    {
	  logging(LL_ERROR, "ERROR on listening: %s", strerror(errno));
      cleanup_and_exit(-1);
    }
  
  /* return the socket yard2srvd is listening */
  umask(amask); //change permission of sockets and folders
  return local_listen_sock;
}

/*
 * DESCRIPTION:     Signal handler for yard2 data received
 * 
 * TODO:            - Move SIGINT, SIGTERM to separated functions
 *                  - Implement better error handling
 */
void signal_handler_yard( int status )
{
  int res,i;
  unsigned char buffer[BUFFER_LENGTH];

  res = read(fd_yard,buffer,BUFFER_LENGTH); /* TODO: check for errors */
  if (res == -1)
    {
	  logging(LL_ERROR, "ERROR while reading from yard (%d, %s (%d)\n", res, strerror(errno), errno);
      cleanup_and_exit(-1);
    }
    
  /* TODO: move the rest to a separated function 
   * this function (signal_handler_yard) is 
   * designed to read data from the hw device */
    
//#ifdef DEBUGSHOWHEX
  logging(LL_VERBOSE, "read YARD_RES: %i: ",res);
  loghex(LL_VERBOSE, buffer, res);
  logging(LL_VERBOSE, "\n");
  //#endif
    
  /* case differentiation based on the first byte */
  if (res>0)
    {
      unsigned char com = buffer[0];
      switch (com)
	{
	case YARD_ACK:
	case YARD_GET_VERSION:
	case SYS_RTC_TIME_R:
	case SYS_RTC_WU_R:
	case SYS_RTC_CMOS_R:
	case SYS_EEPROM_R:
	case SYS_EEPROM_R_BYTE:
	case SYS_READ_LS:
	case SYS_START_UP:
	  if(config_sock != -1) send_data_to_config(buffer,res);
	  else
	  {
		if(com == YARD_ACK )	logging(LL_VERBOSE, "YARD2 ACK signal\n");
		if(com == SYS_START_UP ){myyardconfig.Startupreason = buffer[1]; ReadEEPROM(0,10);} //Store date and read eeprom
		if(com == YARD_GET_VERSION ){ 	myyardconfig.FWverison_main = buffer[2]; myyardconfig.FWverison_sub = buffer[3];}
		if(com == SYS_EEPROM_R ) //yard2srvd reads from adr 0 5 bytes; EEData starts at buffer[4]
		{
			if(buffer[2]  == 0)
			{
				myyardconfig.LCDtype 	= buffer[4+9];
				myyardconfig.LCDconfig 	= buffer[4+1];
				myyardconfig.LCD_X 	= buffer[4+3];
				myyardconfig.LCD_Y 	= buffer[4+2];
			}
		} //if com
		logging(LL_DEBUG, "YARD Config: Start:%d, V:%d.%d\n",myyardconfig.Startupreason,myyardconfig.FWverison_main,myyardconfig.FWverison_sub);
		logging(LL_DEBUG, "LCD  Config: Type:%d, Config:%d X:%d Y: %d\n",myyardconfig.LCDtype,myyardconfig.LCDconfig,myyardconfig.LCD_X,myyardconfig.LCD_Y);
	  } // else
	  break;

	case SYS_WAKEUP_IR:
			
	  //remove first byte to have same structure as SYS_IR_REC
	  for(i=0;i<res;i++) buffer[i] = buffer[i+1]; 
	  res--;
	  //no break !!
	case SYS_IR_REC:
	  /* send ir data to lirc*/
	  //send to config program if connected
	  if(config_sock != -1)
	    {
	      send_data_to_config(buffer,res);	
	    }
	  else
	    {
	      send_ir_data_to_lirc(buffer, res, 0);
	    };
			
	  break;
          
	case SYS_BUTTON:
	case SYS_SPINBUTTON:
	  //Button and Spinbutton only have 1 Byte on data; bring it in the same format as IR signal = 8Bytes
	  for(i=2;i<9;i++) buffer[i] = 0;
	  send_ir_data_to_lirc(buffer, 9, 0);
	  break;

          
	default:
          logging(LL_WARNING, "Sorry, this command is not supported yet. [%02X]\n", com);
          break;
	}
    }
}// end of function

/*
 * DESCRIPTION:     Send config data to yard
 * TODO:            -
 */
void send_data_to_config(unsigned char* buf, int len)
{
  logging(LL_DEBUG, "Send Data to config app\n");
  logging(LL_DEBUG, "buffer: ");
  loghex(LL_DEBUG, buf, len);
  logging(LL_DEBUG, "\n");

  if(config_sock != -1) send(config_sock, buf, len, MSG_NOSIGNAL);
}

void send_data_to_yard(unsigned char* buf, int len)
{
  int t=1;

  if (write(fd_yard,buf,len) < 0)
  {
    logging(LL_ERROR, "Error: Send to yard failed\n");
  }
  else
  {
    logging(LL_DEBUG, "SENDDATA TO YARD\n");
  }

  //got a RESET, shutdown program after sending RESET to Y.A.R.D.
  if( (buf[0] == SYS_RESET) /*&& (buf[2]==0x55)*/ ) 
  {
	sleep(3); //Wait to undock from USB
// Todo automatic reset and conneting
	close_yard_device();
    logging(LL_VERBOSE, "Wait 300 seconds:\n");
	while((t>0) && (t < 150) )
	{
      logging(LL_VERBOSE, "Try to reconnect to YARD2 %d - %d\n", t, fd_yard);
      open_yard_device(yardDeviceName);
	  if (fd_yard != -1)
	  {
 		logging(LL_VERBOSE, "YARD2 reconnected %d\n", fd_yard);
		t=0;  // terminate loop
	  }
	  else 
	  {
 	   t++;
 	   sleep(2); // Wait 2 seconds
 	  }

	}

	if(t==150) cleanup_and_exit(0); // No reconnet --> Exit;
	else open_yard(); //Openyard
  }
}

/*
 * DESCRIPTION:     get Keybord ID (in input.h) for received IR code for uinput
 * TODO:            -
 */
int getirkeyb(unsigned long long IR)
{
int i;

   i=0;
   while( (myirmap[i].IRcode != 0) && (i < MAX_IRMAP_SIZE))
   {
	if (myirmap[i].IRcode == IR)
	{
	 return myirmap[i].keybord_map;
	 break;
	}
	else i++;
   }

   return 0; //nothing found
}

/*
 * DESCRIPTION:     get Button name of received IR code for own lirc server
 * TODO:            -
 */
void getirmapname(unsigned char *mapname, unsigned long long IR)
{
int i;

   //Clear mapname
   mapname[0] = '\0';

   i=0;
   while( (myirmap[i].IRcode != 0) && (i < MAX_IRMAP_SIZE))
   {
	//printf("%lld - %lld\n", IR, myirmap[i].IRcode);
	if (myirmap[i].IRcode == IR)
	{
	//printf("Found IR map %s\n",myirmap[i].btn_name);
	sprintf(mapname, "%s", myirmap[i].btn_name);
	break;
	}
	else i++;
   }

  //no map is found return the IR as hex
  if (mapname[0] == '\0') sprintf(mapname, "%016llX", IR);
}

/*
 * DESCRIPTION:     Send keyboard event to uinput 
 * TODO:            -
 */
void send_key_event(int fd, unsigned int keycode, int keyvalue)
{
struct input_event event;

 event.type = EV_KEY;
 event.code = keycode;
 event.value = keyvalue;

 if (write(fd, &event, sizeof(event)) < 0) 
 {
   logging(LL_ERROR, "Error: Write faild to simulate key 1\n");
 }
 else
 {
   logging(LL_DEBUG, "simulate key %d, %d\n", keycode, keyvalue);
 }

 event.type = EV_SYN;
 event.code = SYN_REPORT;
 event.value = 0;
 if (write(fd, &event, sizeof(event)) < 0) 
 {
   logging(LL_ERROR, "Error: Write faild to simulate key 2\n");
 }
 else 
 {
   logging(LL_DEBUG, "simulate key %d, %d\n", keycode, keyvalue);
 }
}


/*
 * DESCRIPTION:     Send IR date to lirc or to clients if own lirc is used
 * TODO:            -
 */
void send_ir_data_to_lirc(unsigned char* buf, int len, int socket)
{
  int i;
  int repeatdelay;
  
  currenttickcount = GetTickCount();

  //Build 64Bit value from buffer
  currentircode = 0;
  currentircode = buf[1];
  for(i=2;i<9;i++) currentircode = (currentircode << 8) + buf[i];

  // socket is != 0 only, in case of ownlirc mode
  if (socket != 0) //send unmodified data to requesting lirc socket; Events: LIST, VERSION, LIST YARD2USB
  {
 	send(socket, buf,(int)strlen(buf), MSG_NOSIGNAL);
	return;
  }


#ifdef DEBUGSHOWTIME
  logging(LL_DEBUG, "Currentircode: %llu\n",currentircode);
  logging(LL_DEBUG, "Lastircode:    %llu\n",lastircode);
  logging(LL_DEBUG, "currenttickcount:    %llu\n",currenttickcount);
  logging(LL_DEBUG, "lasttickcount   :    %llu\n",lasttickcount);
#endif

  if (irrepeats == 0)   repeatdelay = repeatdelay1st;
  else			repeatdelay = repeatdelay2nd;


  //Do not send every IR code to lirc -> too many repeats
  if ( (currenttickcount < lasttickcount + 100 + repeatdelay) && 
       (currentircode == lastircode ))
    {
      logging(LL_DEBUG, "IR code not send - Too many IR repeats\n\n");
      return;
    }


//Check if it is a repeat IR or not
  if (lastircode == currentircode)
  {
      if (currenttickcount < (lasttickcount + 200 + repeatdelay))
	  {
		if (irrepeats == 255) irrepeats = 0;
		else irrepeats = irrepeats + 1;
	  }
	  else
	  {
		irrepeats = 0;
	  }
  }
  else
  {
    irrepeats = 0;
  }
	
  //Save last time and IRcode if ready to send
  lasttickcount = currenttickcount; 
  lastircode = currentircode;
  
  //if Uinput is used, send the key to uinput
  if (fd_uinput != -1)
  {
	  int irkey = getirkeyb(currentircode);
	  if (irkey != 0)
	  {
		  send_key_event(fd_uinput, irkey, 1);
		  send_key_event(fd_uinput, irkey, 0);
	  }
  }

  // dedicated lirc mode
  if (lirc_ownserver_sock_listen == -1)
  {
	  int rc = 6;
	  YARD_IRCODE ircode;

	  logging(LL_DEBUG, "lirc_sock: %d\n",lirc_sock);
	  logging(LL_DEBUG, "buffer: ");
	  loghex(LL_DEBUG, buf+1, 6);
	  logging(LL_DEBUG, "\n");

	  /* TODO: check if the socket is initialized and clients are connected */
	  //LIRC (hw_yard2) only accept 6 Bytes of Data !!
	  memcpy(&ircode[0],&buf[1],sizeof(YARD_IRCODE));

	  logging(LL_DEBUG, "sending %d bytes\n",rc);

	  if(lirc_sock != -1)
	  {
		  rc = send(lirc_sock, ircode, sizeof(YARD_IRCODE), MSG_NOSIGNAL);
		  logging(LL_DEBUG, "finished sending %d bytes\n",rc);
	  }
	  else
	  {
		  logging(LL_DEBUG, "No client\n");
	  }
	

	  if (rc < 0 )
	  {
		  logging(LL_ERROR, "send() to lirc failed: %s", strerror(errno));
		  clientConnected = FALSE;
	  }
  }
  else
  {
    // Own lirc server implementation
	unsigned char lircstr[80];
	unsigned char mapname [80];

	// send to all
	//Get btnname for IR code			
	getirmapname(mapname,currentircode);

	//Build sendstring lirc format	012345678912345 00 Map_name YARD2USB	
	sprintf(lircstr, "%016llX %02X %s YARD2USB\n", currentircode, (int)irrepeats, mapname);

	logging(LL_DEBUG, "lircstr: %s", lircstr);
	logging(LL_DEBUG, "length : %d\n", (int)strlen(lircstr));

	for ( i = 0; i < MAX_LIRC_CONNECTIONS; i++) //send to all active connections
	{
		if(lirc_connections[i].used)
		{	
			send(lirc_connections[i].socket, lircstr,(int)strlen(lircstr), MSG_NOSIGNAL);
		}
	}
  }

  //   printf("clientConnected: %d\n",clientConnected);
}

/*
 * DESCRIPTION:     opens the given device name and stores the name, if open was possible
 * TODO:            -
 */
static int set_yard_device( char* dev_name)
{
	
	if (open_yard_device(dev_name) != -1)
	{
		// the device is usable, save the name for later use
		strncpy( yardDeviceName, dev_name, YARD_DEVICE_NAME_MAX );
		yardDeviceName[ YARD_DEVICE_NAME_MAX-1 ] = '\0';
	}
	return fd_yard;
}

/*
 * DESCRIPTION:     init an dopen YARD2 Hardware
 * TODO:            -
 */
void open_yard(void)
{
  struct termios settings;
  int status, t;

  // first open attempt to given yard device (cmd option)
  if (opt_yardDevName)
  {
	  // a given device name have to be existing
	  if (set_yard_device(opt_yardDevName) == -1)
	  {
		  cleanup_and_exit(-1);
	  }

	  // don't check it again
	  opt_yardDevName = 0;
  }

  // determine default YARD device name
  if (yardDeviceName[ 0 ] == '\0')
  {
/* org code !!!
	 if ((set_yard_device(YARDDEVICE) == -1) && (set_yard_device(YARDDEVICE_2) == -1))
	  {
		  cleanup_and_exit(-1);
	  }
*/

	  
//------------------------
	t=1;
 	while ((set_yard_device(YARDDEVICE) == -1) && (set_yard_device(YARDDEVICE_2) == -1) && (t<11) )
	{ 
    	// try another 20 times, perhaps driver was not ready
	    logging(LL_VERBOSE, "Try to connect to YARD2 %d\n", t);
	    t++;
	    sleep(2); // Wait 1 seconds
	  }

	  if(t>10)
	  {
	     logging(LL_VERBOSE, "Could not connect to YARD2, tried %d \n",t-1);
	     cleanup_and_exit(0); // No reconnet --> Exit;
	  }

//------------------------
  }

/*
//close again and open afterwards
  close_yard_device();
  //init yard
  if (open_yard_device(yardDeviceName) == -1)
  {
	  cleanup_and_exit(-1);
  }
*/
  usleep(500*1000); //wait for init 500ms

  status = tcgetattr(fd_yard, &settings);
  if (status < 0)
    {
      logging(LL_ERROR, "Failed to read device attributes: %s", strerror(errno));
      cleanup_and_exit(-1);
    }
  
  // set baud rate, need for sime systems
  status = cfsetspeed(&settings, B115200);
  if (status < 0)
    {
      logging(LL_ERROR, "Failed to set device speed: %s", strerror(errno));
      cleanup_and_exit(-1);
    }
  // set input modes
  settings.c_iflag = IGNBRK;
  // set output modes
  settings.c_oflag = 0;
  // set control modes
  settings.c_cflag = CS8 | HUPCL | CREAD | CLOCAL;
  // set local modes
  settings.c_lflag = 0;
  status = tcsetattr(fd_yard, TCSAFLUSH, &settings);
  if (status < 0)
    {
      logging(LL_ERROR, "Failed to set device attributes: %s", strerror(errno));
      cleanup_and_exit(-1);
    }
}

static int activate_uinput()
{
	int ret, j;
	int uinputEventNr;
	char uinputEvent[50];

	//open uinput to setup a new event
	//Check uinput locations
	fd_uinput = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if(fd_uinput < 0)
	{
		//check if uinput is perhaps in /dev/input/ instead of /dev/
		fd_uinput = open("/dev/input/uinput", O_WRONLY | O_NONBLOCK);
		if(fd_uinput < 0)
		{
			logging(LL_ERROR, "error opening uinput device %d (%s)\n", errno, strerror(errno));
			logging(LL_ERROR, "If you want to use uinput you have to start yard2srvd with sudo\n");
			return(-1);
		}
	}

	//get our ID, hopefully
	uinputEventNr = GetNextFreeEvent();

	//enable keyboard for this event
	ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_KEY);
	if (ret < 0)
	{
		logging(LL_ERROR, "Error in uinput enable keybord\n");
		return -1;
	}
	ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_SYN);
	if (ret < 0)
	{
		logging(LL_ERROR, "Error in uinput enable keybord\n");
		return -1;
	}

	//enable vaild keys to receive by uinput
	//get all valid keys from irmap
	j=0;
	while((myirmap[j].IRcode != 0) && (j < MAX_IRMAP_SIZE))
	{
		ret = ioctl(fd_uinput, UI_SET_KEYBIT, myirmap[j].keybord_map);
		if (ret < 0) logging(LL_ERROR, "Error in uinput - adding buttons to uinput line: %d, code:%d, name:%s",j+1
							,myirmap[j].keybord_map,myirmap[j].btn_name);
		j++;
	}

	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));

	//Setup new event device
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, UINPUT_DEV_NAME);
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x2;
	uidev.id.product = 0x2;
	uidev.id.version = 1;
	ret = write(fd_uinput, &uidev, sizeof(uidev));
	if (ret < 0)
	{
		logging(LL_ERROR, "Error in uinput Setup Device\n");
		return -1;
	}

	//Create the device finally --> new event should be in /dev/input/event[x]
	ret = ioctl(fd_uinput, UI_DEV_CREATE);
	if (ret < 0)
	{
		logging(LL_ERROR, "Error in uinput Create Device\n");
		return -1;
	}

	//Set permission to every one can read from this event
	usleep(300); // wait some time that event is created.
	sprintf(uinputEvent,"/dev/input/event%d",uinputEventNr);
	logging(LL_INFO, "Generated yard2srvd Event: %s\n",uinputEvent);
	if (chmod (uinputEvent,S_IRWXU|S_IRGRP|S_IROTH) < 0)
	{
		  logging(LL_ERROR, "error in chmod(%s) - %d (%s)\n", uinputEvent, errno, strerror(errno));
		  return -1;
	}

	logging(LL_INFO, "This is a keyboard event which send keystrokes to any application\n");
	return 0;
}

static int activate_lirc()
{
	unlink(LIRC_SERVER_PATH);

	/* init listen socket for patched lircd */
	lirc_listen_sock = start_listen_socket(0);
	if (lirc_listen_sock == -1) logging(LL_ERROR, "Cannot create lircd communication socket.");

	return lirc_listen_sock;
}

static int activate_ownlirc()
{
	int i;

	unlink(LIRC_OWNSERVER);

	//Change permission of lirc socket, so external programs can read/write

	amask = umask(0000); //change permission of sockets and folders
	mkdir(LIRC_OWNSERVER_DIR, 0777);//S_IRWXU | S_IRWXG | S_IRWXO | S_IWOTH);
	if(errno == EACCES)
	{
		logging(LL_ERROR, "error creating directory %s : %d (%s)\n", LIRC_OWNSERVER_DIR, errno, strerror(errno));
		logging(LL_ERROR, "If you want to use Ownlirc Server you have to start once yard2srvd with sudo\n");
		return(-1);
	}


	//FIXME JASMIN: was tut das hier?
	//sprintf(buffer,"%s%s",homedir,yardirmappath);
	//mkdir(buffer, 0777);//S_IRWXU | S_IRWXG | S_IRWXO | S_IWOTH);

	lirc_ownserver_sock_listen = start_listen_socket(3);
	if (lirc_ownserver_sock_listen == -1) logging(LL_ERROR, "Cannot create own lirc socket.");


	char mode[] = "0666";
	i = strtol(mode, 0, 8);
	if (chmod (LIRC_OWNSERVER,i) < 0)
	{
	    logging(LL_ERROR, "Error in chmod(%s, %s) - %d (%s)\n", LIRC_OWNSERVER, mode, errno, strerror(errno));
	    return -1;
	}

	umask(amask); //Reset Mask

	return lirc_ownserver_sock_listen;
}

static void usage( void )
{
puts ("\
  yard2srvd [option]\n\n\
  options:\n\
    -c <file>, --config=<file>:\n\
                  The config filename (with path).\n\
    -d, --daemon: Start in daemon mode.\n\
    -D <device>, --device=<device>:\n\
                  The tty device for yard2. Default: /dev/ttyYARD2\n\
                  or /dev/ttyACM0, checked in this order.\n\
    -I, --input:  Use the Linux input system to deliver the key codes.\n\
    -i <file>, --irmap=<file>:\n\
                  The IR map filename (with path).\n\
    -L, --lirc:   Use lircd server with yard2 patch.\n\
    -O, --ownlirc:\n\
                  Use own lirc implementation.\n\
    -l <val>, --loglevel=<val> :\n\
                  Loglevel: 1:error, 2:warning, 3:info,\n\
                  4:verbose, 5:debug\n\
    -s, --syslog: Use syslog for logging.\n\
    -1, --rdelay1:\n\
                  Set repeat delay 1 in ms. Default 250, accepted\n\
                  values 50..1000\n\
    -2, --rdelay2:\n\
                  Set repeat delay 2 in ms. Default 100, accepted\n\
                  values 50..1000\n\
    -h, --help:   This text.\n\
    -V, --version:\n\
                  Print driver version.\n\
\n\
	Options -I, -L and -O are mutual exclusive.\n\
");
exit(-1);
}

static void opt_loglevel( const char *val )
{
	loglevel_t ll = strtol(val, 0, 10);
	if ((ll < LL_ERROR) || (ll > LL_DEBUG))
	{
		  fprintf (stderr, "Invalid Loglevel entered\n");
		  usage();
	}
	else
	{
		  loglevel_current = ll;
	}
}

static void opt_repeatdelay( const char *val, int *repeatdelay, const char *txt )
{
	int rdelay = strtol(val, 0, 10);
	if ((rdelay < 50) || (rdelay > 1000))
	{
		  fprintf (stderr, "Invalid %s entered\n", txt);
		  usage();
	}
	*repeatdelay = rdelay;
}

static inline void opt_repeatdelay1( const char *val )
{
	opt_repeatdelay( val, &repeatdelay1st, "rdelay1" );
}

static inline void opt_repeatdelay2( const char *val )
{
	opt_repeatdelay( val, &repeatdelay2nd, "rdelay2" );
}

/*
 * MAIN
 */
int main(int argc, char *argv[])
{  
  struct sigaction handler;
  unsigned char buffer[BUFFER_LENGTH];
  int ret, c;
  int opt_daemon = 0;
  int opt_use_input = 0;
  int opt_use_lirc = 0;
  int opt_use_ownlirc = 0;
  int opt_syslog = 0;
  int opt_config = 0;
  int long_index = 0;
  int i, j;

  // we need to clear the name now, to detect if a name entered
  yardDeviceName[0] = '\0';

  //init repeatdelay
  repeatdelay1st = 250;
  repeatdelay2nd = 100;

  // Specifying the expected options
  static struct option long_options[] =
  {
	{"config",	required_argument,	0,	'c' },
	{"daemon",	no_argument,		0,	'd' },
	{"device",	required_argument,	0,	'D' },
	{"help",	no_argument,		0,	'h' },
	{"input",	no_argument,		0,	'I' },
	{"irmap",	required_argument,	0,	'i' },
	{"lirc",	no_argument,		0,	'L' },
	{"loglevel",required_argument,	0,	'l' },
	{"ownlirc",	no_argument,		0,	'O' },
	{"rdelay1",	required_argument,	0,	'1' },
	{"rdelay2",	required_argument,	0,	'2' },
	{"syslog",	no_argument,		0,	's' },
	{"version",	no_argument,		0,	'V' },
	{0,0,0,0}
  };

  opterr = 0;
  while ((c = getopt_long(argc, argv, "c:dD:hIi:Ll:O1:2:sV", long_options, &long_index)) != -1)
  {
	switch (c)
	{
	  case 'c':
		  if (opt_config)
		  {
			  printf("Option '-c' allowed only once!\n");
			  exit(-1);
		  }

		  opt_config = 1;
		  if (ini_parse(optarg, cfg_handler, &InitConfig) < 0)
		  {
			  printf("Cannot load ini file '%s': %s\n", optarg, strerror(errno));
			  exit(-1);
		  }

		  DUMP_CONFIG(&InitConfig);

		  if (isCFG(yard2srvd, daemon)) opt_daemon = isCFG_VAL(yard2srvd, daemon, "1");
		  if (isCFG(yard2srvd, device)) opt_yardDevName = getCFG(yard2srvd, device);
		  if (isCFG(yard2srvd, irmapfile)) opt_yardIrMap = getCFG(yard2srvd, irmapfile);
		  if (isCFG(yard2srvd, rdelay1)) opt_repeatdelay1(getCFG(yard2srvd, rdelay1));
		  if (isCFG(yard2srvd, rdelay2)) opt_repeatdelay2(getCFG(yard2srvd, rdelay2));

		  if (isCFG(yard2srvd, input))
			  opt_use_input = isCFG_VAL(yard2srvd, input, "1") ? 1 : 0;
		  if (isCFG(yard2srvd, lirc))
			  opt_use_lirc = isCFG_VAL(yard2srvd, lirc, "1") ? 1 : 0;
		  if (isCFG(yard2srvd, ownlirc))
			  opt_use_ownlirc = isCFG_VAL(yard2srvd, ownlirc, "1") ? 1 : 0;

		  if (isCFG(yard2srvd, loglevel)) opt_loglevel(getCFG(yard2srvd, loglevel));
		  if (isCFG(yard2srvd, syslog)) opt_syslog = isCFG_VAL(yard2srvd, syslog, "1");
		  break;
	  case 'd':
		  opt_daemon = 1;
		  break;
	  case 'D':
		  opt_yardDevName = optarg;
		  break;
	  case 'I':
		  opt_use_input = 1;
		  break;
	  case 'i':
		  opt_yardIrMap = optarg;
		  break;
	  case 'L':
		  opt_use_lirc = 1;
		  break;
	  case 'l':
		  opt_loglevel(optarg);
		  break;
	  case 'O':
		  opt_use_ownlirc = 1;
		  break;
	  case '1':
		  opt_repeatdelay1(optarg);
		  break;
	  case '2':
		  opt_repeatdelay2(optarg);
		  break;
	  case 's':
		  opt_syslog = 1;
		  break;
	  case 'V':
		  printf("yard2srvd V%s%s\n", PKG_VERSION, GIT_VERSION );
		  exit (0);
		  break;
	  case '?':
		  switch (optopt)
		  {
		  	  case 'c':
			  case 'D':
			  case 'i':
			  case 'l':
			  case '1':
			  case '2':
				  fprintf (stderr, "Option '-%c' requires an argument.\n", optopt);
				  break;
			  default:
				  if (isprint (optopt))
					  fprintf (stderr, "Unknown option '-%c'.\n", optopt);
				  else
					  fprintf (stderr, "Unknown option character '\\x%2.2x'.\n", optopt);
				  break;
		  }
		  // NO_BREAK
	  // 'h'
	  default:
		  usage ();
	  }
  }

  
// we have currently no arguments
  if (optind != argc)
  {
	  fprintf (stderr, "Unknown argument '%s'.\n", argv[optind]);
	  usage();
  }

#ifdef DEBUGSHOWCONFIG
  printf("daemon: %d\n", opt_daemon);
  printf("device: %s\n", opt_yardDevName);
  printf("irmapfile: %s\n", opt_yardIrMap);
  printf("rdelay1: %d\n", repeatdelay1st);
  printf("rdelay2: %d\n", repeatdelay2nd);
  printf("input: %d\n", opt_use_input);
  printf("lirc: %d\n", opt_use_lirc);
  printf("ownlirc: %d\n", opt_use_ownlirc);
  printf("loglevel: %d\n", loglevel_current);
  printf("syslog: %d\n", opt_syslog);
#endif

  if (opt_syslog) logtosyslog(LOGSYSLOG_IDENT);

  if (opt_yardIrMap)
  {
	  strcpy(mapfile, opt_yardIrMap);
  }
  else
  {
	  homedir = getenv("HOME");
	  if (homedir == NULL) logging(LL_WARNING, "Cannot get home dir in enviroment\n");
	  sprintf(mapfile,"%s%s",homedir,yardirmap);
  }

  if ((opt_use_input + opt_use_lirc + opt_use_ownlirc) > 1)
  {
	  fprintf (stderr, "Options -I, -L and -O are mutual exclusive.\n");
	  usage();
  }

  if ((opt_use_input + opt_use_lirc + opt_use_ownlirc) == 0)
  {
	  logging(LL_ERROR, "Info: No usage mode specified: -I -O -L\n      Only Y.A.R.D.2 Configuration possible\n");
  }

#ifndef openelec
  /* do not start a second instance of yard2srvd */
  if(open_pidfile() != 0) exit(EXIT_FAILURE);
#endif

#ifdef openelec
  if(CheckProcess() == 1)  exit(EXIT_FAILURE);
#endif

  maxcount = 0;

  logging(LL_ALWAYS, "yard2srvd Version %s%s started\n", PKG_VERSION, GIT_VERSION);
  if (opt_daemon) StartDaemon();

  /* install the signal handler before making the device asynchronous */
  handler.sa_handler = exit_handler;
  if (sigfillset(&handler.sa_mask) <0 )
    cleanup_and_exit(-1);
  handler.sa_flags=0;
  if (sigaction(SIGINT, &handler,0) < 0)
    cleanup_and_exit(-1);

  open_yard();

  memset(&myirmap, 0, sizeof(myirmap)); //reset structure	
  readirmap(mapfile,myirmap);//reading config file if exists

  lirc_sock   = -1;
  config_sock = -1;
  lcd_sock    = -1;
  lirc_ownserver_sock_listen = -1;

  if (opt_use_input)
  {
	  if (activate_uinput() < 0) cleanup_and_exit(-1);
  }

  if (opt_use_lirc)
  {
	  if (activate_lirc() < 0) cleanup_and_exit(-1);
  }

  if (opt_use_ownlirc)
  {
	  if (activate_ownlirc() < 0) cleanup_and_exit(-1);
  }

  //remove old sockets if exists
  unlink(CONFIG_SERVER_PATH);
  unlink(LCD_SERVER_PATH);

  /* init communication sockets */
  config_listen_sock = start_listen_socket(1); //Config needs sudo -> OK
  lcd_listen_sock    = start_listen_socket(2);
  chmod (LCD_SERVER_PATH,S_IRWXU|S_IRWXG|S_IRWXO);

  if ((lcd_listen_sock == -1) || (config_listen_sock==-1))
  {
	  logging(LL_ERROR, "Cannot create communication sockets.");
	  cleanup_and_exit(-1);
  }

  sleep(1);

  //Get Status main config from yard
  buffer[0] = SYS_START_UP;
  buffer[1] = 255-buffer[0];
  send_data_to_yard(buffer, 2); //Get Startip reason

  while(1) 
  {
	//Build rfds structure
      FD_ZERO(&rfds);

      if (fd_yard != -1) FD_SET(fd_yard, &rfds);

      if (opt_use_lirc)
      {
    	  // either wait for server or client socket
    	  if (lirc_sock == -1)  FD_SET(lirc_listen_sock, &rfds);
    	  else                  FD_SET(lirc_sock, &rfds);
      }

      if (opt_use_ownlirc)
      {
    	  FD_SET(lirc_ownserver_sock_listen, &rfds);
    	  for (i = 0; i < MAX_LIRC_CONNECTIONS; i++)
    	  {
    		  if (lirc_connections[i].used)
    		  {
    			  FD_SET(lirc_connections[i].socket, &rfds);
    		  }
    	  }
      }

	  // either wait for server or client socket
      if (config_sock == -1) FD_SET(config_listen_sock, &rfds);
      else                   FD_SET(config_sock, &rfds);

      // either wait for server or client socket
      if (lcd_sock == -1) FD_SET(lcd_listen_sock, &rfds);
      else                FD_SET(lcd_sock, &rfds);

      max_fd = 0;

      if (max_fd < fd_yard) max_fd = fd_yard;

      if (opt_use_lirc)
      {
    	  if (max_fd < lirc_sock)        max_fd = lirc_sock;
    	  if (max_fd < lirc_listen_sock) max_fd = lirc_listen_sock;
      }

      if (opt_use_ownlirc)
      {
    	  if (max_fd < lirc_ownserver_sock_listen) max_fd = lirc_ownserver_sock_listen;
    	  for (i = 0; i < MAX_LIRC_CONNECTIONS; i++)
    	  {
    		  if ((lirc_connections[i].used) && (max_fd < lirc_connections[i].socket))
    			  max_fd = lirc_connections[i].socket;
    	  }
      }

      if (max_fd < config_sock)        max_fd = config_sock;
      if (max_fd < config_listen_sock) max_fd = config_listen_sock;

      if (max_fd < lcd_sock)        max_fd = lcd_sock;
      if (max_fd < lcd_listen_sock) max_fd = lcd_listen_sock;

      //wait for activity
				
      logging(LL_VERBOSE, "Wait in select for new activity\n");
      // logging(LL_VERBOSE, "SOCKETS: y:%d, l:%d, ll:%d, c:%d, cl:%d, d:%d, dl:%d, M:%d\n",
      // 		  fd_yard, lirc_sock, lirc_listen_sock, config_sock, config_listen_sock,
      // 	  lcd_sock, lcd_listen_sock, max_fd );

      ret = select(max_fd+1, &rfds, NULL, NULL, NULL);
      if (ret <= 0)
      {
    	  logging(LL_ERROR, "Error in Select\n");
    	  cleanup_and_exit(0);
    	  /* FEHLER oder Signal (CTRL-C) */
    	  break;
      }
      maxcount++;
      logging(LL_DEBUG, "Maxcount %d, Ret %d \n",maxcount,ret);

      /* minimum 1 FD activity */
      if ( (fd_yard != -1) && FD_ISSET(fd_yard, &rfds)) //yard activity
      {
    	  signal_handler_yard(0);
      }

      if (opt_use_lirc)
      {
    	//Listen sockets events of original lirc
    	if ((lirc_listen_sock > 0) && FD_ISSET(lirc_listen_sock, &rfds))
		{
		  /* accept behandeln */
		  lirc_sock = accept(lirc_listen_sock,NULL,NULL); /* wird nicht blockieren */
		  close(lirc_listen_sock); //only one lirc socket accepted, close lirc listen socket
		  lirc_listen_sock = -1;
		  unlink(LIRC_SERVER_PATH);
		}
      }

      if (opt_use_ownlirc)
      {
		//our own LIRC Server accept connections
		if ((lirc_ownserver_sock_listen >= 0) && FD_ISSET(lirc_ownserver_sock_listen, &rfds))
		{
			/* accept behandeln */
			for ( i = 0; i < MAX_LIRC_CONNECTIONS; i++)
			{

				if (! lirc_connections[i].used)
				{
					lirc_connections[i].used = 1;
					lirc_connections[i].socket = accept(lirc_ownserver_sock_listen,NULL,NULL);
					break;
				}
				if (i == MAX_LIRC_CONNECTIONS)
				{
					/* ignoring incoming connection */
					i = accept(lirc_ownserver_sock_listen,NULL,NULL);
					close(i);
				}
			}
		}
      }

	if ((config_listen_sock > 0) && FD_ISSET(config_listen_sock, &rfds))
	{
	  /* accept behandeln */
	  config_sock = accept(config_listen_sock,NULL,NULL); /* wird nicht blockieren */
	  close(config_listen_sock); //only one config socket accepted, close lirc listen socket
	  config_listen_sock = -1;
	  unlink(CONFIG_SERVER_PATH);

	}

    if ((lcd_listen_sock > 0) && FD_ISSET(lcd_listen_sock, &rfds))
	{
	  /* accept behandeln */
	  lcd_sock = accept(lcd_listen_sock,NULL,NULL); /* wird nicht blockieren */
	  close(lcd_listen_sock); //only one lcd socket accepted, close lirc listen socket
	  lcd_listen_sock = -1;
	  unlink(LCD_SERVER_PATH);
	}

    if (opt_use_lirc)
    {
    	//Communication sockets events
    	//original lirc
		if ((lirc_sock > 0) && FD_ISSET(lirc_sock, &rfds))
		{
		  ret = read(lirc_sock,buffer,BUFFER_LENGTH);
		  if (ret > 0)
		  {
			  // write to YARD for IR Send
			  logging(LL_VERBOSE, "-->LIRC IR SEND event \n");
			  logging(LL_VERBOSE,"buffer: ");
			  loghex(LL_VERBOSE,buffer,ret);
			  logging(LL_VERBOSE,"\n\n");

			  send_data_to_yard(buffer,ret);
		  }
		  else
		  {
			  /* socket closed */
			  //close lirc socket and wait for new connect
			  close(lirc_sock);
			  lirc_sock = -1;
			  lirc_listen_sock = start_listen_socket(0);
		  }
		}
    }

    if (opt_use_ownlirc)
    {
      //own lirc server
  	  for ( i = 0; i < MAX_LIRC_CONNECTIONS; i++) 
	  {
			if (lirc_connections[i].used && FD_ISSET(lirc_connections[i].socket, &rfds))
			{
				//do something
				ret = read(lirc_connections[i].socket,buffer,BUFFER_LENGTH);
				logging(LL_DEBUG, "-->LIRC receive event \n");
				if (ret!=0) // received cmd from lirc client
				{
					  logging(LL_DEBUG, "buffer: %s HEX:",buffer);
					  loghex(LL_DEBUG, buffer,ret);
					  logging(LL_DEBUG, "\n");
				}
				else
				{
					logging(LL_DEBUG, "Connection closed from socket ID %d \n\n",i);
				}

				if (ret > 0)
				{
					buffer[ret] = 0; //mark end of string

					//handle lirc protocol LIST and Version !! not complete implemented yet
					if (strcmp(buffer,"LIST\n")==0) //list available IRs; only one availabe
					{
						sprintf(buffer,"BEGIN\nLIST\nSUCCESS\nDATA\n1\nYARD2USB\nEND\n");
						
						logging(LL_DEBUG, "Send RC List string to client:\n %s: ",buffer);
						logging(LL_DEBUG, "Socket %d\n\n",lirc_connections[i].socket);

						send_ir_data_to_lirc(buffer,strlen(buffer),lirc_connections[i].socket);
					}
					
					else if (strcmp(buffer,"LIST YARD2USB\n")==0) //list of all known IR codes
					{
						logging(LL_DEBUG, "Send IR List to client: %s\n",buffer);
						logging(LL_DEBUG, "Socket %d\n\n",lirc_connections[i].socket);

						//Send Header						
						j=0;
						while((myirmap[j].IRcode != 0) && (j < MAX_IRMAP_SIZE)) j++;
						sprintf(buffer,"BEGIN\nLIST YARD2USB\nSUCCESS\nDATA\n%d\n",j);
						printf("Socket %d\n",lirc_connections[i].socket);
						send_ir_data_to_lirc(buffer,strlen(buffer),lirc_connections[i].socket);
						usleep(10000); //need a little time between send cmds.

						j=0;
						while((myirmap[j].IRcode != 0) && (j < MAX_IRMAP_SIZE))
						{
						sprintf(buffer, "%016llX %s\n",myirmap[j].IRcode, myirmap[j].btn_name);
						send_ir_data_to_lirc(buffer,strlen(buffer),lirc_connections[i].socket);
						j++;
						usleep(10000);//need a little time between send cmds.
						}

						sprintf(buffer,"END\n");
						send_ir_data_to_lirc(buffer,strlen(buffer),lirc_connections[i].socket);
						
					}

					else if (strcmp(buffer,"VERSION\n")==0)
					{
						sprintf(buffer,"BEGIN\nVERSION\nSUCCESS\nDATA\n1\nYARD2USB 0.50\nEND\n");
						
						logging(LL_DEBUG, "Send Version string to client:\n %s: ",buffer);
						logging(LL_DEBUG, "Socket %d\n\n",lirc_connections[i].socket);

						send_ir_data_to_lirc(buffer,strlen(buffer),lirc_connections[i].socket);
					}

					else //unsopported Command request
					{
						unsigned char buffer2[BUFFER_LENGTH];
						snprintf(buffer2, BUFFER_LENGTH, "BEGIN\n%sERROR\nDATA\n1\nUNSUPPORTED COMMAND\nEND\n",buffer);
						buffer2[BUFFER_LENGTH-1]='\0';
						
						logging(LL_DEBUG, "Send ERROR back to client:\n %s: ",buffer);
						logging(LL_DEBUG, "Socket %d\n\n",lirc_connections[i].socket);

						send_ir_data_to_lirc(buffer,strlen(buffer),lirc_connections[i].socket);
					}
					
				}
				else
				{ /* socket closed */
					lirc_connections[i].used = 0;
					close(lirc_connections[i].socket);
				}
			}
	  }
    }

    if ((config_sock > 0) && FD_ISSET(config_sock, &rfds))
	{
	  ret = read(config_sock,buffer,BUFFER_LENGTH);
	  //ret = recv(config_sock,buffer,BUFFER_LENGTH,0);
	  if (ret > 0)
	    {
	      logging(LL_VERBOSE, "configsock received:%d\n", ret);
	      logging(LL_VERBOSE,"buffer: ");
      	  loghex(LL_VERBOSE,buffer,ret);
	      logging(LL_VERBOSE,"\n\n");

	      //send(config_sock, "1", 1, MSG_NOSIGNAL);
	      send_data_to_yard(buffer,ret);

	    }
	  else
	    {
	      /* socket closed */
	      //close lirc socket and wait for new connect
	      close(config_sock);
	      config_sock = -1;

	      config_listen_sock = start_listen_socket(1);
	    }
	}

    if ((lcd_sock > 0) && FD_ISSET(lcd_sock, &rfds))
	{
	  ret = read(lcd_sock,buffer,BUFFER_LENGTH);
	  if (ret > 0)
	    {
		logging(LL_VERBOSE,"LCD Recv:%d, CMD: %s\n",ret,buffer);
		logging(LL_DEBUG, "LCD CMD received: %d: ",ret);
		loghex(LL_DEBUG, buffer, ret);
		logging(LL_DEBUG, "\n");

	      // write to YARD for LCD output
			switch (buffer[0])
			{
				case 'L': 
 						logging(LL_DEBUG, "LCD: Get Config\n");
						LCDbuffer[0] = 'C'; //Send C for Config
						LCDbuffer[1] = myyardconfig.LCD_X;
						LCDbuffer[2] = myyardconfig.LCD_Y;
						LCDbuffer[3] = myyardconfig.LCDtype;

						send(lcd_sock, LCDbuffer,4, MSG_NOSIGNAL); //repond to LCD program; Command received
						break;
				//Clear Display
				case 'C': 
						logging(LL_DEBUG, "LCD: Clear LCD\n");
						LCDbuffer[0] = SYS_LCD_CLEAR; 	//SYS_LCD_CLEAR=clear all; SYS_LCD_CLEAR_T=clear only testarea; SYS_LCD_CLEAR_G=Clear only graphic area
						LCDbuffer[1] = 255 - LCDbuffer[0]; //invert of 0 to check in FW if command is from yard app
						LCDbuffer[2] = 1; // Controller 1 oder 2 on hd44780
						ret = 3;
						break;
				//Set Backlight (0..1023)
				case 'B': 
						logging(LL_DEBUG, "LCD: Set Backlight\n");
						LCDbuffer[0] = SYS_LCD_BL;  
						LCDbuffer[1] = 255 - LCDbuffer[0]; 	//invert of 0 to check in FW if command is from yard app
						LCDbuffer[2] = buffer[1];			// high byte of 16Bit value
						LCDbuffer[3] = buffer[2]; 			// low  byte of 16Bit value
						ret = 4;
						break;
				//Goto x,y on Display
				case 'G': 
 						logging(LL_DEBUG, "LCD: Goto X:%d Y:%d\n",buffer[1],buffer[2]);
						LCDbuffer[0] = SYS_LCD_GOTO_XY;  
						LCDbuffer[1] = 255 - LCDbuffer[0]; 	//invert of 0 to check in FW if command is from yard app
						LCDbuffer[2] = 1;             		// Controller 1 oder 2
						LCDbuffer[3] = buffer[1]; 		// X
						LCDbuffer[4] = buffer[2];		// Y
						ret = 5;
						break;
				//Init Custom Char
				case 'I':
 						logging(LL_DEBUG, "LCD: Set Custom Char\n");
						LCDbuffer[0] = SYS_LCD_SET_CC;    
						LCDbuffer[1] = 255 - LCDbuffer[0];	// invert of 0 to check in FW if command is from yard app
						LCDbuffer[2] = 1;					// Controller 1 oder 2
						LCDbuffer[3] = buffer[1];			// custom char position 0..7
						memcpy(&LCDbuffer[4],&buffer[2],8);	// copy custom char to LCDbuffer, always 8 bytes from top to bottom
						ret = 12;
						break;
				//Write Text on Display
				case 'W':
						logging(LL_DEBUG, "LCD: Write Text %s\n",buffer);
						if(ret>41)  // to long for LCD yard buffer
						{
							logging(LL_WARNING, "Buffer to long for Y.A.R.D.2. MAX=40");
							break;
						}
						
						LCDbuffer[0] = SYS_LCD_W;			// LCD Write CMD
						LCDbuffer[1] = 255 - LCDbuffer[0];	// invert of 0 to check in FW if command is from yard app
						LCDbuffer[2] = 1;					// Controller 1 oder 2
						LCDbuffer[3] = ret-1;
	
						memcpy(&LCDbuffer[4],&buffer[1],ret-1);

						ret = (ret - 1) + 4; //ret-1=Stringlength
						break;
				default:
						logging(LL_WARNING, "Sorry, this command is not supported yet. [%02X]\n",
								buffer[0]);
					    break;
			}

			if(buffer[0] != 'L')
			{			
				send_data_to_yard(LCDbuffer,ret); //Send buffer to yard
				send(lcd_sock, "ROK\0",4, MSG_NOSIGNAL); //repond to LCD program; Command received
			}

	      }
		
	  else
	    {
	      /* socket closed */
	      //close lirc socket and wait for new connect
	      close(lcd_sock);
	      lcd_sock = -1;
	      lcd_listen_sock = start_listen_socket(2);
	    }
	}
  }//while


  cleanup_and_exit(0);
  return 0;
}


