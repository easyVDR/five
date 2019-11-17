/***************************************************************************
 *   Copyright (C) 2012 R. Geigenberger                                    *
 *   Y.A.R.D.2 USB                                                         *
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
#ifndef GLOBAL_H
#define GLOBAL_H

/* USER section */

//Define the repeat delay. Only IR signals will be send out to lirc which are over this value in ms. Default: 150
//#define IR_REPEATDELAY 150

// Define the default wakeup adjust. This value in minutes will be substrct from given wake up time
#define GLOBAL_WUADJUST 0

// Define the default device locations
#define YARDDEVICE "/dev/ttyYARD2"
#define YARDDEVICE_2 "/dev/ttyACM0"

// Define if you want to use the own lirc server implemenation or original lirc
//#define USEOWNLIRC

/***************************************************************************
*                                                                          *  
*                  DO NOT MAKE CHANGES BELOW !!!                           *
*                                                                          *
****************************************************************************/
//Path for yardirmap in your user folder: home/[user]/...
#define yardirmap "/yardirmap.txt"
//#define yardirmappath "/yardirmap.txt"

#define YARD_PIDFILE "/tmp/yard2srvd.pid"

#define MAX_LIRC_CONNECTIONS 8 //maximum connections to the own lirc server
struct connections {
 int used;
 int socket;
};

#define MAX_IRMAP_SIZE 80 //maximum keys in IR map
struct irmap {
 unsigned char IRcode_str[80]; //IR code has always 16 char. 64Bit value in Hex, avoid wrong string > buffer 80
 unsigned long long IRcode;
 unsigned char btn_name[80];   //max length for btn name
 unsigned int keybord_map;   //Number according input.h to use with uinput 16Bit value
};

struct yardconfig {
 unsigned char yardtype;
 unsigned char FWverison_main;
 unsigned char FWverison_sub;
 unsigned char Startupreason;
 unsigned char LCDtype;
 unsigned char LCDconfig;
 unsigned char LCD_X;
 unsigned char LCD_Y;
};

//Socket names
#define LIRC_SERVER_PATH    "/tmp/yardsrv_sock"   /* original Lirc, socket to communicate between yard2srvd and Lirc*/
#define CONFIG_SERVER_PATH  "/tmp/configserver"   /* config socket*/
#define LCD_SERVER_PATH     "/tmp/lcdserver"      /* lcd socket */
#define LIRC_OWNSERVER_DIR  "/var/run/lirc"
#define LIRC_OWNSERVER      LIRC_OWNSERVER_DIR "/lircd" /* Lirc socket, use own lirc implementaion */
//#define LIRC_OWNSERVER      "/tmp/lircd" /* Lirc socket, use own lirc implementaion */

/* Debug section */
//#define DEBUG
//#define DEBUGSHOWHEX
//#define DEBUGSHOWTIME

// Global Defines
#define BUFFER_LENGTH 1024  // used to read from yard or socket
#define FALSE 0
#define TRUE 1
#define LOGSYSLOG_IDENT "YARD2"
#define UINPUT_DEV_NAME "YARD2USB"

typedef unsigned char BOOL;
typedef unsigned char YARD_IRCODE[6];

#endif

