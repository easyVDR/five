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
#include <getopt.h>
#include <ctype.h>
#include "yard2record.h"
#include "global_config.h"
#include "util.h"

int fd_yard; 							//filedescriptor for yard HW
unsigned char currentir[18];			//IR code send 
unsigned char lastir[18];				//Last IR code for comparing
unsigned char lastchangedir[18]; 		//Last IR code tat was changed or added
unsigned char buffer[BUFFER_LENGTH];	//Buffer for socket
unsigned char btnname[80];	//Button name
unsigned char keybid[20];	//keyboard ID from input.h
int lirc_sock,max_fd,maxcount;
fd_set rfds;
struct irmap myirmap[MAX_IRMAP_SIZE];
char *homedir;
char mapfile[255];
int itemsmodified;
char *opt_yardIrMap = 0;
char y2mode  = 0;
char VDRtype  = 0;

struct x {
	const char *name;
	int event_id;
};

struct x array[] = {

{"KEY_RESERVED", 0},{"KEY_ESC", 1},{"KEY_1", 2},{"KEY_2", 3},{"KEY_3", 4},{"KEY_4", 5},{"KEY_5", 6},{"KEY_6", 7},{"KEY_7", 8},{"KEY_8", 9},{"KEY_9", 10},{"KEY_0", 11},
{"KEY_MINUS", 12},{"KEY_EQUAL", 13},{"KEY_BACKSPACE", 14},{"KEY_TAB", 15},{"KEY_Q", 16},{"KEY_W", 17},{"KEY_E", 18},{"KEY_R", 19},{"KEY_T", 20},{"KEY_Y", 21},{"KEY_U", 22},
{"KEY_I", 23},{"KEY_O", 24},{"KEY_P", 25},{"KEY_LEFTBRACE", 26},{"KEY_RIGHTBRACE", 27},{"KEY_ENTER", 28},{"KEY_LEFTCTRL", 29},{"KEY_A", 30},{"KEY_S", 31},{"KEY_D", 32},
{"KEY_F", 33},{"KEY_G", 34},{"KEY_H", 35},{"KEY_J", 36},{"KEY_K", 37},{"KEY_L", 38},{"KEY_SEMICOLON", 39},{"KEY_APOSTROPHE", 40},{"KEY_GRAVE", 41},{"KEY_LEFTSHIFT", 42},
{"KEY_BACKSLASH", 43},{"KEY_Z", 44},{"KEY_X", 45},{"KEY_C", 46},{"KEY_V", 47},{"KEY_B", 48},{"KEY_N", 49},{"KEY_M", 50},{"KEY_COMMA", 51},{"KEY_DOT", 52},{"KEY_SLASH", 53},
{"KEY_RIGHTSHIFT", 54},{"KEY_KPASTERISK", 55},{"KEY_LEFTALT", 56},{"KEY_SPACE", 57},{"KEY_CAPSLOCK", 58},{"KEY_F1", 59},{"KEY_F2", 60},{"KEY_F3", 61},{"KEY_F4", 62},{"KEY_F5", 63},
{"KEY_F6", 64},{"KEY_F7", 65},{"KEY_F8", 66},{"KEY_F9", 67},{"KEY_F10", 68},{"KEY_NUMLOCK", 69},{"KEY_SCROLLLOCK", 70},{"KEY_KP7", 71},{"KEY_KP8", 72},{"KEY_KP9", 73},{"KEY_KPMINUS", 74},
{"KEY_KP4", 75},{"KEY_KP5", 76},{"KEY_KP6", 77},{"KEY_KPPLUS", 78},{"KEY_KP1", 79},{"KEY_KP2", 80},{"KEY_KP3", 81},{"KEY_KP0", 82},{"KEY_KPDOT", 83},{"KEY_ZENKAKUHANKAKU", 85},
{"KEY_102ND", 86},{"KEY_F11", 87},{"KEY_F12", 88},{"KEY_RO", 89},{"KEY_KATAKANA", 90},{"KEY_HIRAGANA", 91},{"KEY_HENKAN", 92},{"KEY_KATAKANAHIRAGANA", 93},{"KEY_MUHENKAN", 94},
{"KEY_KPJPCOMMA", 95},{"KEY_KPENTER", 96},{"KEY_RIGHTCTRL", 97},{"KEY_KPSLASH", 98},{"KEY_SYSRQ", 99},{"KEY_RIGHTALT", 100},{"KEY_LINEFEED", 101},{"KEY_HOME", 102},{"KEY_UP", 103},
{"KEY_PAGEUP", 104},{"KEY_LEFT", 105},{"KEY_RIGHT", 106},{"KEY_END", 107},{"KEY_DOWN", 108},{"KEY_PAGEDOWN", 109},{"KEY_INSERT", 110},{"KEY_DELETE", 111},{"KEY_MACRO", 112},{"KEY_MUTE", 113},
{"KEY_VOLUMEDOWN", 114},{"KEY_VOLUMEUP", 115},{"KEY_POWER", 116},{"KEY_KPEQUAL", 117},{"KEY_KPPLUSMINUS", 118},{"KEY_PAUSE", 119},{"KEY_SCALE", 120},{"KEY_KPCOMMA", 121},{"KEY_HANGEUL", 122},
{"KEY_HANGUEL", 122},{"KEY_HANJA", 123},{"KEY_YEN", 124},{"KEY_LEFTMETA", 125},{"KEY_RIGHTMETA", 126},{"KEY_COMPOSE", 127},{"KEY_STOP", 128},{"KEY_AGAIN", 129},{"KEY_PROPS", 130},
{"KEY_UNDO", 131},{"KEY_FRONT", 132},{"KEY_COPY", 133},{"KEY_OPEN", 134},{"KEY_PASTE", 135},{"KEY_FIND", 136},{"KEY_CUT", 137},{"KEY_HELP", 138},{"KEY_MENU", 139},{"KEY_CALC", 140},
{"KEY_SETUP", 141},{"KEY_SLEEP", 142},{"KEY_WAKEUP", 143},{"KEY_FILE", 144},{"KEY_SENDFILE", 145},{"KEY_DELETEFILE", 146},{"KEY_XFER", 147},{"KEY_PROG1", 148},{"KEY_PROG2", 149},
{"KEY_WWW", 150},{"KEY_MSDOS", 151},{"KEY_COFFEE", 152},{"KEY_SCREENLOCK", 152},{"KEY_DIRECTION", 153},{"KEY_CYCLEWINDOWS", 154},{"KEY_MAIL", 155},{"KEY_BOOKMARKS", 156},{"KEY_COMPUTER", 157},
{"KEY_BACK", 158},{"KEY_FORWARD", 159},{"KEY_CLOSECD", 160},{"KEY_EJECTCD", 161},{"KEY_EJECTCLOSECD", 162},{"KEY_NEXTSONG", 163},{"KEY_PLAYPAUSE", 164},{"KEY_PREVIOUSSONG", 165},{"KEY_STOPCD", 166},
{"KEY_RECORD", 167},{"KEY_REWIND", 168},{"KEY_PHONE", 169},{"KEY_ISO", 170},{"KEY_CONFIG", 171},{"KEY_HOMEPAGE", 172},{"KEY_REFRESH", 173},{"KEY_EXIT", 174},{"KEY_MOVE", 175},{"KEY_EDIT", 176},
{"KEY_SCROLLUP", 177},{"KEY_SCROLLDOWN", 178},{"KEY_KPLEFTPAREN", 179},{"KEY_KPRIGHTPAREN", 180},{"KEY_NEW", 181},{"KEY_REDO", 182},{"KEY_F13", 183},{"KEY_F14", 184},{"KEY_F15", 185},
{"KEY_F16", 186},{"KEY_F17", 187},{"KEY_F18", 188},{"KEY_F19", 189},{"KEY_F20", 190},{"KEY_F21", 191},{"KEY_F22", 192},{"KEY_F23", 193},{"KEY_F24", 194},{"KEY_PLAYCD", 200},{"KEY_PAUSECD", 201},
{"KEY_PROG3", 202},{"KEY_PROG4", 203},{"KEY_DASHBOARD", 204},{"KEY_SUSPEND", 205},{"KEY_CLOSE", 206},{"KEY_PLAY", 207},{"KEY_FASTFORWARD", 208},{"KEY_BASSBOOST", 209},{"KEY_PRINT", 210},
{"KEY_HP", 211},{"KEY_CAMERA", 212},{"KEY_SOUND", 213},{"KEY_QUESTION", 214},{"KEY_EMAIL", 215},{"KEY_CHAT", 216},{"KEY_SEARCH", 217},{"KEY_CONNECT", 218},{"KEY_FINANCE", 219},{"KEY_SPORT", 220},
{"KEY_SHOP", 221},{"KEY_ALTERASE", 222},{"KEY_CANCEL", 223},{"KEY_BRIGHTNESSDOWN", 224},{"KEY_BRIGHTNESSUP", 225},{"KEY_MEDIA", 226},{"KEY_SWITCHVIDEOMODE", 227},{"KEY_KBDILLUMTOGGLE", 228},
{"KEY_KBDILLUMDOWN", 229},{"KEY_KBDILLUMUP", 230},{"KEY_SEND", 231},{"KEY_REPLY", 232},{"KEY_FORWARDMAIL", 233},{"KEY_SAVE", 234},{"KEY_DOCUMENTS", 235},{"KEY_BATTERY", 236},{"KEY_BLUETOOTH", 237},
{"KEY_WLAN", 238},{"KEY_UWB", 239},{"KEY_UNKNOWN", 240},{"KEY_VIDEO_NEXT", 241},{"KEY_VIDEO_PREV", 242},{"KEY_BRIGHTNESS_CYCLE", 243},{"KEY_BRIGHTNESS_ZERO", 244},{"KEY_DISPLAY_OFF", 245},
{"KEY_WIMAX", 246},{"KEY_RFKILL", 247},{"KEY_MICMUTE", 248},{"KEY_OK", 0x160},{"KEY_SELECT", 0x161},{"KEY_GOTO", 0x162},{"KEY_CLEAR", 0x163},{"KEY_POWER2", 0x164},{"KEY_OPTION", 0x165},
{"KEY_INFO", 0x166},{"KEY_TIME", 0x167},{"KEY_VENDOR", 0x168},{"KEY_ARCHIVE", 0x169},{"KEY_PROGRAM", 0x16a},{"KEY_CHANNEL", 0x16b},{"KEY_FAVORITES", 0x16c},{"KEY_EPG", 0x16d},{"KEY_PVR", 0x16e},
{"KEY_MHP", 0x16f},{"KEY_LANGUAGE", 0x170},{"KEY_TITLE", 0x171},{"KEY_SUBTITLE", 0x172},{"KEY_ANGLE", 0x173},{"KEY_ZOOM", 0x174},{"KEY_MODE", 0x175},{"KEY_KEYBOARD", 0x176},{"KEY_SCREEN", 0x177},
{"KEY_PC", 0x178},{"KEY_TV", 0x179},{"KEY_TV2", 0x17a},{"KEY_VCR", 0x17b},{"KEY_VCR2", 0x17c},{"KEY_SAT", 0x17d},{"KEY_SAT2", 0x17e},{"KEY_CD", 0x17f},{"KEY_TAPE", 0x180},{"KEY_RADIO", 0x181},
{"KEY_TUNER", 0x182},{"KEY_PLAYER", 0x183},{"KEY_TEXT", 0x184},{"KEY_DVD", 0x185},{"KEY_AUX", 0x186},{"KEY_MP3", 0x187},{"KEY_AUDIO", 0x188},{"KEY_VIDEO", 0x189},{"KEY_DIRECTORY", 0x18a},
{"KEY_LIST", 0x18b},{"KEY_MEMO", 0x18c},{"KEY_CALENDAR", 0x18d},{"KEY_RED", 0x18e},{"KEY_GREEN", 0x18f},{"KEY_YELLOW", 0x190},{"KEY_BLUE", 0x191},{"KEY_CHANNELUP", 0x192},{"KEY_CHANNELDOWN", 0x193},
{"KEY_FIRST", 0x194},{"KEY_LAST", 0x195},{"KEY_AB", 0x196},{"KEY_NEXT", 0x197},{"KEY_RESTART", 0x198},{"KEY_SLOW", 0x199},{"KEY_SHUFFLE", 0x19a},{"KEY_BREAK", 0x19b},{"KEY_PREVIOUS", 0x19c},{"KEY_DIGITS", 0x19d},
{"KEY_TEEN", 0x19e},{"KEY_TWEN", 0x19f},{"KEY_VIDEOPHONE", 0x1a0},{"KEY_GAMES", 0x1a1},{"KEY_ZOOMIN", 0x1a2},{"KEY_ZOOMOUT", 0x1a3},{"KEY_ZOOMRESET", 0x1a4},{"KEY_WORDPROCESSOR", 0x1a5},{"KEY_EDITOR", 0x1a6},
{"KEY_SPREADSHEET", 0x1a7},{"KEY_GRAPHICSEDITOR", 0x1a8},{"KEY_PRESENTATION", 0x1a9},{"KEY_DATABASE", 0x1aa},{"KEY_NEWS", 0x1ab},{"KEY_VOICEMAIL", 0x1ac},{"KEY_ADDRESSBOOK", 0x1ad},{"KEY_MESSENGER", 0x1ae},
{"KEY_DISPLAYTOGGLE", 0x1af},{"KEY_SPELLCHECK", 0x1b0},{"KEY_LOGOFF", 0x1b1},{"KEY_DOLLAR", 0x1b2},{"KEY_EURO", 0x1b3},{"KEY_FRAMEBACK", 0x1b4},{"KEY_FRAMEFORWARD", 0x1b5},{"KEY_CONTEXT_MENU", 0x1b6},
{"KEY_MEDIA_REPEAT", 0x1b7},{"KEY_10CHANNELSUP", 0x1b8},{"KEY_10CHANNELSDOWN", 0x1b9},{"KEY_IMAGES", 0x1ba},{"KEY_DEL_EOL", 0x1c0},{"KEY_DEL_EOS", 0x1c1},{"KEY_INS_LINE", 0x1c2},{"KEY_DEL_LINE", 0x1c3},
{"KEY_FN", 0x1d0},{"KEY_FN_ESC", 0x1d1},{"KEY_FN_F1", 0x1d2},{"KEY_FN_F2", 0x1d3},{"KEY_FN_F3", 0x1d4},{"KEY_FN_F4", 0x1d5},{"KEY_FN_F5", 0x1d6},{"KEY_FN_F6", 0x1d7},{"KEY_FN_F7", 0x1d8},
{"KEY_FN_F8", 0x1d9},{"KEY_FN_F9", 0x1da},{"KEY_FN_F10", 0x1db},{"KEY_FN_F11", 0x1dc},{"KEY_FN_F12", 0x1dd},{"KEY_FN_1", 0x1de},{"KEY_FN_2", 0x1df},{"KEY_FN_D", 0x1e0},{"KEY_FN_E", 0x1e1},
{"KEY_FN_F", 0x1e2},{"KEY_FN_S", 0x1e3},{"KEY_FN_B", 0x1e4},{"KEY_BRL_DOT1", 0x1f1},{"KEY_BRL_DOT2", 0x1f2},{"KEY_BRL_DOT3", 0x1f3},{"KEY_BRL_DOT4", 0x1f4},{"KEY_BRL_DOT5", 0x1f5},{"KEY_BRL_DOT6", 0x1f6},
{"KEY_BRL_DOT7", 0x1f7},{"KEY_BRL_DOT8", 0x1f8},{"KEY_BRL_DOT9", 0x1f9},{"KEY_BRL_DOT10", 0x1fa},{"KEY_NUMERIC_0", 0x200},{"KEY_NUMERIC_1", 0x201},{"KEY_NUMERIC_2", 0x202},{"KEY_NUMERIC_3", 0x203},
{"KEY_NUMERIC_4", 0x204},{"KEY_NUMERIC_5", 0x205},{"KEY_NUMERIC_6", 0x206},{"KEY_NUMERIC_7", 0x207},{"KEY_NUMERIC_8", 0x208},{"KEY_NUMERIC_9", 0x209},{"KEY_NUMERIC_STAR", 0x20a},{"KEY_NUMERIC_POUND", 0x20b},
{"KEY_CAMERA_FOCUS", 0x210},{"KEY_WPS_BUTTON", 0x211},{"KEY_TOUCHPAD_TOGGLE", 0x212},{"KEY_TOUCHPAD_ON", 0x213},{"KEY_TOUCHPAD_OFF", 0x214},{"KEY_CAMERA_ZOOMIN", 0x215},{"KEY_CAMERA_ZOOMOUT", 0x216},
{"KEY_CAMERA_UP", 0x217},{"KEY_CAMERA_DOWN", 0x218},{"KEY_CAMERA_LEFT", 0x219},{"KEY_CAMERA_RIGHT", 0x21a},{"KEY_MIN_INTERESTING", 113},{"KEY_MAX", 0x2ff},
{NULL, 0}
};

void exit_handler(int sig) {
  cleanup_and_exit(0);
}

void cleanup_and_exit(int exitstate)
{
  writeirmap(); //save ir map to file
  printf("\n - IMPORTANT - \n");
  printf("Please copy the file: %s to /etc/yard2/ (sudo cp) \n",mapfile);
  printf("Restart yard2srvd or system to activate the changes\n");
  
  close(lirc_sock);
  exit(exitstate);
}

BOOL ClearSocket()
{

    BOOL             res;
    fd_set          sready;
    struct timeval  nowait;

    FD_ZERO(&sready);
    FD_SET(lirc_sock,&sready);
    memset((char *)&nowait,0,sizeof(nowait));

    res = select(lirc_sock+1,&sready,NULL,NULL,&nowait);
	if(res==1) 
	{
		// clear buffer
		if (read(lirc_sock,buffer,BUFFER_LENGTH) < 0)
		{
			printf("Communication error!\n");
			cleanup_and_exit(-1);
		}
		return TRUE;
	}
	else return FALSE; //nothing was in socket
	
/*
	if( FD_ISSET(lirc_sock,&sready) )
        res = TRUE;
    else
        res = FALSE;


    return res;
	*/
}

int getinput_ID(char* keyname)
{
 struct x *i;
 for (i = array; i->name; i++) 
 {
   if(strcmp(i->name,keyname) == 0) return i->event_id;
//   printf("%s:%s %d 0x%x\n", i->name,keyname, i->event_id, i->event_id);
 }
return 0;
}

void getinput_keyname(int ID, char* keyname)
{
 struct x *i;
 for (i = array; i->name; i++) 
 {
   if(ID == i->event_id) 
   {
	strcpy(keyname,i->name);
	return;
   }
 }
}

void writeirmap()
{
int i;
FILE *fh;
char line[255];
  
  if(itemsmodified == 0) return; // no need to write the file

  printf("\n");
  //printf("Open file %s\n",mapfile);
  if ( ( fh = fopen(mapfile, "w") ) == NULL )
  {
    printf("Error open irmap file %s !\n", mapfile);
    return;
  }

  i = 0;   
  while( (myirmap[i].IRcode != 0) && (strlen(myirmap[i].btn_name) != 0) && (i < MAX_IRMAP_SIZE)  )
  {
   sprintf(line,"%s@%s@%d@\n",myirmap[i].IRcode_str,myirmap[i].btn_name,myirmap[i].keybord_map);
   printf("Write: %s", line);
   fwrite(line,1,strlen(line),fh); 
   i++;
  } // while

  fclose(fh);
  return;
}

void printusage(void)
{
	printf("\n");
	printf("yard2record usage:\n");
	printf("Start yard2record\n");
	printf("Press the button of your remote control\n");
	printf("IR Code must be received twice before button name can be entered\n");
	printf("Enter the name of the button\n");
	printf("If a IR code is already in use you get an info to replace or not\n");
	printf("\n");
	printf("Press first button ...\n");
	printf("\n");
	
//	cleanup_and_exit(0);
}
void readbtnname(unsigned char id)
{

	memset(&btnname[0], 0, sizeof(btnname));
	itemsmodified++;

	strcpy(lastchangedir,currentir); //save the last changed IR code

	if((y2mode == '1') || (y2mode == '2'))	
	{
		printf("Enter button name: ");
		if (fgets(btnname,sizeof(btnname),stdin) == NULL)
		{
			printf("Input error!\n");
			cleanup_and_exit(-1);
		}
		btnname[strlen(btnname)-1] = 0; //Remove \n from string

		// get ID from keyname for y2mode 2
		if(y2mode == '2') sprintf(keybid, "%d",getinput_ID(btnname));
	}

	if((y2mode == '1') || (y2mode == '3'))	
	{
		printf("Enter Keyboard ID/Event (from input.h) - 0=skip: ");
		if (fgets(keybid,sizeof(keybid),stdin) == NULL)
		{
			printf("Input error!\n");
			cleanup_and_exit(-1);
		}

		if(y2mode == '3') getinput_keyname(atoi(keybid),btnname);
	}

	gotoxy(0,18);
        clrscr(0,0);
	printf("Entry for IR: %s - BtnName: %s - Keyboard ID: %s\n",currentir,btnname,keybid);

	myirmap[id].IRcode = (unsigned long long)strtoull(currentir, NULL, 16);
	strcpy(myirmap[id].IRcode_str,currentir); //copy ir to myirmap
	strcpy(myirmap[id].btn_name,btnname);  //copy ir to myirmap
	myirmap[id].keybord_map = (unsigned int)strtol(keybid, NULL, 10);
}

void printVDRdefault (void)
{
if(VDRtype == '1')
	{
	if(y2mode == '2') printf(" YaVDR / XBMC default - Keyname & IDs - ENTER KEYNAME !!!\n");
	if(y2mode == '3') printf(" YaVDR / XBMC default - Keyname & IDs - ENTER ID !!!\n");
	printf("------------------------------------------------------------------------------\n");
	printf("KEY_UP:    103 | KEY_DOWN:  108 | KEY_MENU:   139 | KEY_ESC:        1\n");
	printf("KEY_LEFT:  105 | KEY_RIGHT: 106 | KEY_NEXT:   407 | KEY_OK:       352\n");
	printf("KEY_ENTER:  28 | KEY_POWER: 116 | KEY_SPACE:  57  | KEY_BACKSPACE: 14\n");
	printf("KEY_EPG:   365 | KEY_MUTE:  113 | KEY_FN:     464 | KEY_TEXT:     388\n");
	printf("KEY_RED:   398 | KEY_GREEN: 399 | KEY_YELLOW: 400 | KEY_BLUE:     401\n");
	printf("KEY_PLAY:  207 | KEY_PAUSE: 119 | KEY_STOP:   128 | KEY_RECORD:   167\n");
	printf("KEY_SETUP: 141 | KEY_TIME:  359 | KEY_MODE:   373 | KEY_SCREEN:   375\n");
	printf("KEY_PROG1: 148 | KEY_PROG2: 149 | KEY_PROG3:  202 | KEY_PROG4:    203\n");
	printf("KEY_PVR:   366 | KEY_AUDIO: 392 | KEY_VIDEO:  393 | KEY_IMAGES:   442\n");
	printf("KEY_CHANNELUP:   402 | KEY_CHANNELDOWN: 403       | KEY_CHANNEL:  336\n");
	printf("KEY_VOLUMEUP:    115 | KEY_VOLUMEDOWN:  114       | KEY_POWER2:   356\n");
	printf("KEY_FASTFORWARD: 208 | KEY_REWIND:      168       | KEY_PREVIOUS: 412\n");
	printf("KEY_FAVORITES:   365 | KEY_SUBTITLE:     370      | KEY_BACK:     158\n");
	printf("KEY_1: 2  | _2: 3  | _3: 4  | _4: 5 | _5: 6 | ... | _9: 10  |  _0: 11\n");
	printf("------------------------------------------------------------------------------\n");
	}

if(VDRtype == '2')
	{
	if(y2mode == '2') printf(" easyVDR default - Keyname & IDs - ENTER KEYNAME !!!\n");
	if(y2mode == '3') printf(" easyVDR default - Keyname & IDs - ENTER ID !!!\n");
	printf("------------------------------------------------------------------------------\n");
	printf("KEY_UP:    103 | KEY_DOWN: 108 | KEY_LEFT:     105 | KEY_RIGHT:      106\n"); 
	printf("KEY_M(enu): 50 | KEY_ENTER: 28 | KEY_BACKSPACE: 14 | KEY_ESC:          1\n");
	printf("KEY_I(nfo): 23 | KEY_SPACE(Pause): 57 | KEY_P(ower): 25\n");
	printf("KEY_F1(red):59 | KEY_F2(gr):60 | KEY_F3(yellow):   61 | KEY_F4(blue): 62\n");
	printf("KEY_F5(FR): 63 | KEY_F6(FF):64 | KEY_F7(U7):       65 | KEY_F8(U8):   66\n");
	printf("KEY_F9(U9): 67 | KEY_F10(Mute): 68 | KEY_F11(Vol-):87 | KEY_F12(Vol+):88\n");
	printf("KEY_1:       2 | KEY_2:      3 | KEY_3: 4 | KEY_4:  5 | KEY_5: 6\n");
	printf("KEY_6:       7 | KEY_7:      8 | KEY_8: 9 | KEY_9: 10 | KEY_0: 11\n");
	printf("KEY_KPMINUS(Vol-): 74 | KEY_KPPLUS(Vol+):  78\n");
	printf("------------------------------------------------------------------------------\n");
	}

}

void usage( void )
{
puts ("\
  yard2record [option]\n\n\
  options:\n\
    -i <file>, --irmap=<file>:\n\
                  The IR map filename (with path).\n\
    -h, --help:   This text\
");
exit (-1);
}

int main(int argc, char *argv[])
{  
  struct sigaction handler;
  struct sockaddr_un srvAddr;
  int srvAddrLen;
  int ret, c;
  int i;
  BOOL foundinlist;
  char yesno[80];
  unsigned long long currentircode;
  int long_index = 0;


  // Specifying the expected options
  static struct option long_options[] =
  {
	{"help",	no_argument,		0,	'h' },
	{"irmap",	required_argument,	0,	'i' },
	{0,0,0,0}
  };

  opterr = 0;
  while ((c = getopt_long(argc, argv, "hi:", long_options, &long_index)) != -1)
  {
	  switch (c)
      {
      case 'i':
    	  opt_yardIrMap = optarg;
    	  break;
      case '?':
    	  switch (optopt)
    	  {
    	  	  case 'i':
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

  itemsmodified = 0;
  
  if ( opt_yardIrMap )
  {
	  strcpy(mapfile, opt_yardIrMap);
  }
  else
  {
	  homedir = getenv("HOME");
  	  #ifdef DEBUG
	    if (homedir != NULL) printf("Home dir in enviroment: %s\n\n", homedir);
  	  #endif
	  sprintf(mapfile,"%s%s",homedir,yardirmap);
  }

	handler.sa_handler = exit_handler;
	if (sigfillset(&handler.sa_mask) <0 )
		cleanup_and_exit(-1);
	handler.sa_flags=0;
	if (sigaction(SIGINT, &handler,0) < 0)
		cleanup_and_exit(-1);

	printusage();
	readirmap(mapfile,myirmap);

// Establish connection to LIRC server
	bzero( (char *)&srvAddr, sizeof(srvAddr));
	srvAddr.sun_family = AF_UNIX;
	strcpy(srvAddr.sun_path, CONFIG_SERVER_PATH);
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
		printf("Can not connext to yard2srvd\n");
		cleanup_and_exit(-1);
		return 0;
	}

	// Let user select which mode to use
	//gotoxy(0,1);
	//clrscr(0,0);
        printf("yard2record mode select (recommended: 3) \n"); 
        printf("1: manual mode (Enter Keyname & ID)\n"); 
        printf("-- 2 and 3 can select VDR Version default keymap (Keyname/ID must match input.h)\n"); 
        printf("2: Enter keyname -> auto get ID\n"); 
        printf("3: Enter ID      -> auto get keyname\n"); 
        printf("Select: "); 
 	y2mode = fgetc(stdin);
	fgetc(stdin); //remove 0x10 from stdin
	if((y2mode != '1') && (y2mode != '2') && (y2mode != '3') ) y2mode = '3';

        printf("Which VDR release do yo use\n"); 
        printf("1: VDR / yaVDR / (XMBC)\n"); 
        printf("2: easyVDR\n"); 
        printf("Select: "); 
 	VDRtype = fgetc(stdin);
	fgetc(stdin); //remove 0x10 from stdin
	if((VDRtype != '1') && (VDRtype != '2') ) VDRtype = '1';
	gotoxy(0,1);
	clrscr(0,0);

	if(y2mode != '1') printVDRdefault();



	while(1)
	{
	//Build rfds structure
	if(y2mode != '1') gotoxy(0,19);


        FD_ZERO(&rfds);
        FD_SET(fd_yard, &rfds);
        if (lirc_sock != -1) FD_SET(lirc_sock, &rfds);
	  
		max_fd = lirc_sock;
	  
        printf("Wait for IR event\n"); 
        ret = select(max_fd+1, &rfds, NULL, NULL, NULL);
        maxcount++;
    #ifdef DEBUG
		printf("Maxcount %d, Ret %d \n",maxcount,ret); 
    #endif

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
	      //printf("--> LIRC IR event \n"); 
		
		currentircode = 0;
  		currentircode = buffer[1];
		
		for(i=2;i<9;i++) currentircode = (currentircode << 8) + buffer[i];

		sprintf(currentir, "%016llX",currentircode);
		printf("IR CODE:%s\n",currentir);
          
		  if((strcmp(currentir,lastir) == 0) && (strcmp(lastir,lastchangedir) != 0)) // IR code was send twice, ask for btn name
		  {
			//printf("IR CODE MATCH WITH LAST IR CODE\n");
			//Check if IR code already in irmap list
			foundinlist = FALSE;
			i=0;
			while( (myirmap[i].IRcode != 0) && (i < MAX_IRMAP_SIZE))
			{
				if(strcmp(myirmap[i].IRcode_str,currentir)==0)
				{
					foundinlist = TRUE;
					break;
				}
			i++;
			}

			if(foundinlist == TRUE)
			{
				printf("IR Code & Button name already in list - Button name: %s \n",myirmap[i].btn_name);
				printf("Update entry [y,n]? :");
				if (fgets(yesno,sizeof(yesno),stdin) == NULL)
				{
					printf("Input error!\n");
					cleanup_and_exit(-1);
				}
				//update list if wanted by user
				if(yesno[0]=='y'){readbtnname(i);}
			}
			else
			{	//add new entry
				readbtnname(i);
			}
		  }
		  //save lastircode fo comparing.	
		  strncpy(lastir,currentir,16);
		  gotoxy(0,19);
		  clrscr(0,0);

	     }
	     else
	     {
	      /* socket closed */
	      //close lirc socket and wait for new connect
	      printf("Socket closed \n"); 
	      close(lirc_sock);
	      lirc_sock = -1;
  	      cleanup_and_exit(0);
	     }

        }//If FDSET
	//remove all LIRC events in TCP queue
	ClearSocket();

	} //while

	cleanup_and_exit(0);
	return 0;
}
