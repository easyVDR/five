/*
 * main.cc
 * Copyright (C) 2014 Bleifuss <bleifuss2@gmx.net>
 * 
 * make-remote-mapping is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * make-remote-mapping is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <stdio.h>

//#include <cstdio>

#include <termios.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <string>

#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <getopt.h>
#include <syslog.h>

#include <pthread.h>


//Wichtig!! Projekt (rechtes Fenster) rechts anklicken
//Eigenschaften Flags für C- Kompiler -pthread einstellen


#define DEV_LIRCD "lircd"
#define VARRUNDIR "/var/run"
#define PACKAGE "lirc"

#define LIRCD	   VARRUNDIR "/" PACKAGE "/" DEV_LIRCD 



//Parameter Namen aus der vdr Remote.conf
#define LIRC_0_9		"LIRC."
#define	LIRC_UP			"LIRC.Up"
#define	LIRC_Down		"LIRC.Down"
#define LIRC_Menu		"LIRC.Menu"
#define LIRC_Ok			"LIRC.Ok"
#define LIRC_Back		"LIRC.Back"
#define LIRC_Left		"LIRC.Left"
#define LIRC_Right		"LIRC.Right"
#define LIRC_Red		"LIRC.Red"
#define LIRC_Green		"LIRC.Green"
#define LIRC_Yellow		"LIRC.Yellow"
#define LIRC_Blue		"LIRC.Blue"
#define LIRC_Play		"LIRC.Play"
#define	LIRC_Pause		"LIRC.Pause"
#define LIRC_Stop		"LIRC.Stop"
#define LIRC_Record		"LIRC.Record"
#define LIRC_FastFwd	"LIRC.FastFwd"
#define LIRC_FastRew	"LIRC.FastRew"
#define LIRC_Next		"LIRC.Next"
#define LIRC_Prev		"LIRC.Prev"
#define LIRC_Power		"LIRC.Power"
//Power deaktiviert
#define LIRC_Power_D		"#LIRC.Power"
#define LIRC_Channel_P	"LIRC.Channel+"
#define LIRC_Channel_M	"LIRC.Channel-"
#define LIRC_Volume_P	"LIRC.Volume+"
#define LIRC_Volume_M	"LIRC.Volume-"
#define LIRC_Mute		"LIRC.Mute"
#define LIRC_Recordings	"LIRC.Recordings"
#define LIRC_Back		"LIRC.Back"

#define LIRC_AUDIO		"LIRC.Audio"
#define LIRC_Subtitles	"LIRC.Subtitles"

//EPG
#define LIRC_Schedule	"LIRC.Schedule"
#define LIRC_Channels	"LIRC.Channels"
#define LIRC_Timers		"LIRC.Timers"


std::string ProgramName="make-remote-mapping";
std::string RemoteName[3]={" "," "," "}; 

char Lircbuf[128];
bool write_input=false;
int error=0;
bool Init_Lirc=false;
bool Quiet=false;
bool DEBUG=false;

void clear_cin_buffer(void) {
	//Buffer leeren
	std::cin.clear();
	std::cin.ignore(INT_MAX,'\n');
}

//Auf return warten
void wait_key_return(void){
	if(Quiet!=true){
		std::cout << "Return druecken" << std::endl;
		std::cin;
		clear_cin_buffer();
	}
}

//######################################################################
//Infos ins Syslog Schreiben

void info_log(std::string acInfo) {
	if(DEBUG==true){
		openlog ("make-remote-mapping", LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
		syslog (LOG_NOTICE,"%s",acInfo.c_str());
		closelog ();
	}
}
//######################################################################
void * RemoteThread(void *p) {
int i;
int LircSocket;
struct sockaddr_un addr;
	
	//Socket Typ
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, LIRCD);

	//Socket öffnen
	LircSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (LircSocket == -1) {
		perror("socket");
		wait_key_return();		
		exit(errno);
	};

	
	if (connect(LircSocket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("connect lirc socket");
		wait_key_return();		
		exit(errno);
	};

	//Lirc OK
	Init_Lirc=true;
	
	for (;;) {
		i = read(LircSocket, Lircbuf, 128);
		if (i == -1) {
			perror("read");
			wait_key_return();			
			exit(errno);
		};

		
		if (!i)
			exit(0);

		if (write_input==true)
			ssize_t ret=write(STDOUT_FILENO, Lircbuf, i);
		
		
	};

	close(LircSocket);
}

//#############################################################################
void RemoteTest() {
char cSelection;

//Tasten anzeigen
write_input=true;
while ( cSelection !='x' && cSelection !='X') { 		
	//ESC Sequenz clear screen, cursor home
	std::cout << "\033[2J\033[1;1H";

	std::cout << "---------------------------------" << std::endl;
	std::cout << "Fernbedienungs Test\n" << std::endl;
	std::cout << "Beliebige Taste auf der Fernbedienung druecken\n" << std::endl;
	std::cout << "Mit x zurueck zum Hauptmenue\n" << std::endl;
	std::cout << "Taste: " ;
	
	std::cin >> cSelection;
	clear_cin_buffer();
}

//Tasten nicht anzeigen
write_input=false;
}

//#############################################################################
//Aus der Lircmap.xml die Tasten Namen ermitteln
std::string search_kodi_key_name(std::string kodi_lircmap_xml, std::string Lirc_key_name)
{
#define PLINE_MAX 200
FILE *pLircmapFile;
char acString[PLINE_MAX+1];
char acParameter[PLINE_MAX],acTemp[PLINE_MAX];
char acLircmap_Key_Name[PLINE_MAX];


std::string Lircmap_Key="";
	
	pLircmapFile=fopen(kodi_lircmap_xml.c_str(), "r");
	
	if(pLircmapFile==NULL){
		std::cerr << "Datei nicht gefunden: " << kodi_lircmap_xml << std::endl;
		wait_key_return();
		exit(1);
	}
	else {

		while(feof(pLircmapFile)==0 ){

			//Puffer löschen
			memset(acString,0,PLINE_MAX);
			if(fgets(acString,PLINE_MAX,pLircmapFile)!=NULL){
				//Testen ob Kommentar Zeile
				if(memchr(acString,'#',PLINE_MAX)==NULL && strlen(acString) >1){   
					Lircmap_Key=acString;
					//Taste vorhanden?
					if (Lircmap_Key.find(Lirc_key_name) !=std::string::npos ){
						//Original String: <one>LIRC_OK</one>
						//String vom Ende bis zum trenner durchsuchen		
						unsigned found=Lircmap_Key.find_first_of(">");
						//std::string::npos=> String Ende
						if( found  != std::string::npos ){
							Lircmap_Key.erase( found, std::string::npos );
							found=Lircmap_Key.find_first_of("<");
							found++;				//"<" entfernen
							if( found  <= std::string::npos )
								Lircmap_Key.erase( 0,found); //von 0 bis "<" String löschen
							fclose(pLircmapFile);
							return Lircmap_Key;
						}
					}
				}
			}
		}
		fclose(pLircmapFile);		
	}
return "";
}


//#############################################################################
//Aus der remote.conf die Lirc Namen ermitteln
//key_counter -> Einige Keys gibt es mehrmals, hier die Nummer des zu suchenden Keys
//				-1 Key gibt es nicht
std::string search_lirc_key_name(std::string vdr_remote_conf, std::string vdr_remote_key_name,int &key_counter)
{
#define PLINE_MAX 200
FILE *pRemoteFile;
char acString[PLINE_MAX+1];
char acParameter[100];
char acLirc_Key_Name[100];
int counter=0;

std::string Lirc_Key_Name="";
std::string Fc="";

//Erstes Zeichen des such String ermittlen
Fc=*(vdr_remote_key_name.begin());
		
	pRemoteFile=fopen(vdr_remote_conf.c_str(), "r");
	
	if(pRemoteFile==NULL){
		std::cerr << "Datei nicht gefunden: " << vdr_remote_conf << std::endl;
		wait_key_return();
		exit(1);
	}
	else {

		while(feof(pRemoteFile)==0 || key_counter ==5){

			//Puffer löschen
			memset(acString,0,PLINE_MAX);
			//Eine Zeile lesen
			if(fgets(acString,PLINE_MAX,pRemoteFile)!=NULL){

				//Testen ob Kommentar Zeile, ist in Fc auch ein Kommentar dann muss die Zeile verarbeitet werden
				if(memchr(acString,'#',PLINE_MAX)==NULL || Fc.compare("#") ==0 ){
					if(sscanf(acString,"%s %s",acParameter,acLirc_Key_Name)==2){
						std::string vdr_key=acParameter;	
						if (vdr_remote_key_name.compare(vdr_key) ==0 ){
							if (key_counter==counter && Lirc_Key_Name=="" ){
								Lirc_Key_Name=acLirc_Key_Name;
								key_counter++;
							}
							counter++;
							//Wurde die Taste nochmal gefunden?
							//Wenn nicht Datei bis Ende durchsuchen und -1 für fertig
							if(counter>key_counter){
								fclose(pRemoteFile);
								return Lirc_Key_Name;
							}
						}
					}
				}
			}	
		}
		fclose(pRemoteFile);		
	}
	
key_counter=-1;
return Lirc_Key_Name;
}


//#############################################################################
std::string get_remote_button_name(std::string button_name){
	if(Quiet==true)
		return "";
	sleep(1);	
	std::cout << "FB Taste fuer "  << button_name << "druecken" << std::endl;
	std::string ButtonName;
	
	//Puffer Löschen
	memset(Lircbuf,0,sizeof(Lircbuf));
		
	int i=0;
	while (Lircbuf[0]==0){
		usleep (100000);
		i++;
		//7s warten
		if(i>70){
			std::cerr << "Keine FB Taste erkannt" << std::endl;
			sleep (3);
			return "";
		}
	}

	ButtonName=Lircbuf;

//Original String: 000000000000154f 02 KEY_OK TechnoTrend_S2400
	//String vom Ende bis zum trenner durchsuchen
		
	unsigned found=ButtonName.find_last_of(" ");
	//std::string::npos=> String Ende
	if( found  != std::string::npos ){
		ButtonName.erase( found, std::string::npos );

		found=ButtonName.find_last_of(" ");
		found++;				//" " entfernen
		if( found  <= std::string::npos )
			ButtonName.erase( 0,found); //von 0 bis " " String löschen

		std::cout << "Erkannte Taste:" << ButtonName << std::endl;
	}	
return ButtonName;
}
//#############################################################################
void write_key_lirc_conf(std::fstream &f_pchanger_lirc_conf,std::string ProgramName,std::string ButtonName ,int iRepeat,std::string Config){
	f_pchanger_lirc_conf << "begin" << std::endl;
	f_pchanger_lirc_conf << "	prog = " << ProgramName << std::endl;
	f_pchanger_lirc_conf << "	button = " << ButtonName << std::endl;
	f_pchanger_lirc_conf << "	repeat = " << iRepeat << std::endl;
	f_pchanger_lirc_conf << "	config = " << Config << std::endl;
	f_pchanger_lirc_conf << "end" << std::endl;
}


//#############################################################################

void setup_power_off(std::string remote_conf_disable_power_off,std::string program_changer_lirc_conf_enable_power_off,std::string restore_remote_conf_power_off){
//std::string log;
char cSelection=0;


	while ( cSelection!='4') { 	
		int i=1;
		std::cout << "\033[2J\033[1;1H";
		std::cout << "---------------------------------\n" << std::endl;
		std::cout << "1 Software Ausschalten deaktivieren, der VDR wird mit den Schalter vom Board ausgeschaltet." << std::endl << std::endl;
		std::cout << "2 Program-changer steuert das Ausschalten, man kann jederzeit die Powertaste druecken." << std::endl;
		std::cout << "  Der VDR faehrt herunter, auch wenn der Desktop oder Kodi aktiv ist." << std::endl;
		std::cout << "3 Standard wieder herstellen" << std::endl;
		std::cout << "4 Zurueck" << std::endl;

		std::cout << "Bitte 1-4 waehlen: ";

	
		std::cin >> cSelection;
		clear_cin_buffer();

		switch (cSelection) {
			case '1':
				info_log("system:" + remote_conf_disable_power_off);
				i=system(remote_conf_disable_power_off.c_str()); 
				std::cout << "Funktion ausgefuehrt ";
				wait_key_return();
			break;
			case '2':
				info_log("system:" + program_changer_lirc_conf_enable_power_off);
				i=system(program_changer_lirc_conf_enable_power_off.c_str()); 
				std::cout << "Funktion ausgefuehrt ";
				wait_key_return();
			break;
			case '3':
				info_log("system:" + restore_remote_conf_power_off);
				i=system(restore_remote_conf_power_off.c_str());
				std::cout << "Funktion ausgefuehrt ";
				wait_key_return();
			break;
		}
	}

}

//#############################################################################
void program_changer_lirc_conf(std::string pchanger_lirc_conf,std::string vdr_remote_conf){
	std::fstream f_pchanger_lirc_conf;
	std::string ButtonName;
	std::string ProgramChangerButtonName;
	int key_counter=0;
	
	std::cout << "\033[2J\033[1;1H";
	std::cout << "---------------------------------\n" << std::endl;
	std::cout << "Taste fuer \"Start Program-changer\" waehlen" << std::endl << std::endl;
	std::cout << "Die hier gewaehlte Taste darf nur fuer den Program-changer verwendet werden." << std::endl;
	std::cout << "Wird die Taste auch bei VDR/KODI verwendet gibt es Probleme." << std::endl;
	std::cout << "Wenn keine freie Taste vorhanden ist nichts druecken." << std::endl;
	std::cout << "Dann kann der Program-changer mit der Taste Menu aufgerufen werden." << std::endl;
	std::cout << "Die Menu Taste muss >1s gedrueckt werden um den Program-changer zu aktivieren/deaktivieren\n" << std::endl;

	std::cout << "Um zu starten ";

	wait_key_return();

	f_pchanger_lirc_conf.open(pchanger_lirc_conf.c_str(), std::fstream::out);

	if (!f_pchanger_lirc_conf){
		std::cerr << "Datei: " << pchanger_lirc_conf << " kann nicht geoeffnet werden" << std::endl;
		wait_key_return();
		exit(1);
	}


	f_pchanger_lirc_conf << "# program-changer-lirc.conf Version 0.1 01.04.2014 Bleifuss2\n" << std::endl;
	f_pchanger_lirc_conf << "# program-changer-lirc.conf wurde aus VDR remote.conf erzeugt" << std::endl;
	f_pchanger_lirc_conf << "# Bei Button muß der Name der Fernbedienungstaste eingetragen werden (siehe /etc/lirc/lircd.conf" << std::endl;
	f_pchanger_lirc_conf << "# Befehle bei config:" << std::endl;
	f_pchanger_lirc_conf << "# EXECUTE_NO_MOUSE-SkriptName  ->Mit dem Parameter EXECUTE_NO_MOUSE- wird das script nach dem - ausgefuehrt" << std::endl;
	f_pchanger_lirc_conf << "# EXECUTE_MOUSE-SkriptName		->Mit dem Parameter EXECUTE_MOUSE- wird das script nach dem - ausgefuehrt" << std::endl;
	f_pchanger_lirc_conf << "# SHOW_HIDE_MENU				->Menu anzeigen" << std::endl;
	f_pchanger_lirc_conf << "# BUTTON_1 - BUTTON_10			->Standard Skript (program-changer.sh) wird ausgefuehrt" << std::endl;



std::string  PchangerName="Program-Changer";

	ProgramChangerButtonName=get_remote_button_name("Program-Changer ");
	//Pchanger Taste Speichern
	if (ProgramChangerButtonName==""){
		key_counter=0;
		while (key_counter!=-1){
			ProgramChangerButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Menu,key_counter);
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ProgramChangerButtonName ,1,"WAIT_SHOW_HIDE_MENU");
		}
	}
	else
		write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ProgramChangerButtonName ,1,"SHOW_HIDE_MENU");

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Volume_P,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"EXECUTE_VOLUME-/usr/share/easyvdr/program-changer/utilities.sh master_volume_up");
	}

	key_counter=0;
	while (key_counter!=-1){
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Volume_M,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"EXECUTE_VOLUME-/usr/share/easyvdr/program-changer/utilities.sh master_volume_down");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Mute,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"EXECUTE_VOLUME-/usr/share/easyvdr/program-changer/utilities.sh master_volume_mute");
	}
	
	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Back,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"HIDE_MENU");
	}

	key_counter=0;	
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Power,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"EXECUTE_NO_MOUSE-/usr/share/easyvdr/program-changer/program-changer.sh NO_SHUTDOWN");
	}

	key_counter=0;	
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Power_D,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"EXECUTE_NO_MOUSE-/usr/share/easyvdr/program-changer/program-changer.sh NO_SHUTDOWN");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Schedule,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"EXECUTE_ON_SCREEN_KEYBOARD-/usr/share/easyvdr/program-changer/utilities on_screen_keyboard");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_UP,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"POINTER_UP");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Down,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"POINTER_DOWN");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Left,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"POINTER_LEFT");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Right,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"POINTER_RIGHT");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Red,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"RED");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Green,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"GREEN");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Yellow,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"YELLOW");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Blue,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"BLUE");
	}
	
	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Channel_P,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"POINTER_WHEEL_UP");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Channel_M,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"POINTER_WHEEL_DOWN");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_FastFwd,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"POINTER_CLICK_FORWARD");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_FastRew,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"POINTER_CLICK_BACK");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Ok,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,"OK");
	}

	key_counter=0;
	while (key_counter!=-1){	
		ButtonName=search_lirc_key_name(vdr_remote_conf,LIRC_Menu,key_counter);
		if(ButtonName!="" && ButtonName!=ProgramChangerButtonName)
			write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,1,"MENU");
	}


	char acString[100];
	std::string str;

	for(int i=0;i<10;i++)
	{
		sprintf(acString,"%s%d",LIRC_0_9,i);
		key_counter=0;
		while (key_counter!=-1){	
			//LIRC Button Name
			str=acString;

			ButtonName=search_lirc_key_name(vdr_remote_conf,str,key_counter);

			if(ButtonName!="" && ButtonName!=ProgramChangerButtonName){

				char Pchanger_Button_Name[15];
				if (i==0)
					sprintf(Pchanger_Button_Name,"BUTTON_%d",10);
				else
					sprintf(Pchanger_Button_Name,"BUTTON_%d",i);
					
				str=Pchanger_Button_Name;
				write_key_lirc_conf(f_pchanger_lirc_conf,PchangerName,ButtonName ,0,str);
			}
		}
		
	}
	
	f_pchanger_lirc_conf.close();

	std::cout << "\nDatei \"" << pchanger_lirc_conf << "\" erstellt" << std::endl;
//	wait_key_return();
	if(Quiet==true)
    	    exit (0); 
	sleep (3);

}
//#############################################################################
void keymap_xml(std::string kodi_keymap_xml,std::string kodi_lircmap_xml){
	char cSelection;
	std::fstream f_kodi_keymap;
		
	std::cout << "\033[2J\033[1;1H";
	std::cout << "---------------------------------\n" << std::endl;
	std::cout << "1 KODI Bedienung anpassen" << std::endl;
	std::cout << "2 KODI Anpassungen loeschen" << std::endl;
	std::cout << "3 Abbruch\n" << std::endl;

	std::cout << "Bitte 1-3 waehlen: ";

	std::cin >> cSelection;
	clear_cin_buffer();
	
		switch (cSelection) {
			case '2':
				f_kodi_keymap.open(kodi_keymap_xml.c_str(), std::fstream::in);
				if (f_kodi_keymap){ // Datei vorhanden?
					f_kodi_keymap.close();
					std::remove(kodi_keymap_xml.c_str()); 	
				}
				return;
			break;
			case '3':return;
			break;
		}
	
	
	std::cout << "\033[2J\033[1;1H";
	std::cout << "---------------------------------\n" << std::endl;
	std::cout << "1 KODI Bedienung anpassen\n" << std::endl;
	
	//Keymap erzeugen
	std::string ButtonName;
	std::string ButtonDelete;
	std::string ButtonMoveItemUp;
	std::string ButtonMoveItemDown;
	std::string ButtonFullscreen;
	std::string ButtonQueue;
	std::string ButtonPlaylist;
	std::string ButtonNextSubtitle;
	std::string ButtonSkipPrevious;	
	std::string ButtonSkipNext;

	
	std::cout << "Die nachfolgenden Aenderungen sind nur in den Menues:\n" << std::endl;
	std::cout << "Musik, Playlists, Visualization und Video aktiv\n\n\n" << std::endl;
	std::cout << "Bei nicht gewuenschten Funktionen keine Taste druecken\n\n" << std::endl;
	std::cout << "Um zu starten ";

	wait_key_return();
	
	std::cout << "Wenn keine freie Taste vorhanden ist \"0\" waehlen" << std::endl;
	ButtonDelete=get_remote_button_name("Titel aus Playlist entfernen ");
	ButtonDelete=search_kodi_key_name(kodi_lircmap_xml,ButtonDelete);
	std::cout << std::endl;
	
	std::cout << "Wenn keine freie Taste vorhanden ist \"2\" waehlen" << std::endl;
	ButtonMoveItemUp=get_remote_button_name("Titel in Playlist nach oben schieben ");
	ButtonMoveItemUp=search_kodi_key_name(kodi_lircmap_xml,ButtonMoveItemUp);
	std::cout << std::endl;
	
	std::cout << "Wenn keine freie Taste vorhanden ist \"8\" waehlen" << std::endl;
	ButtonMoveItemDown=get_remote_button_name("Titel in Playlist nach unten schieben ");
	ButtonMoveItemDown=search_kodi_key_name(kodi_lircmap_xml,ButtonMoveItemDown);
	std::cout << std::endl;
	

	std::cout << "Wenn keine freie Taste vorhanden ist \"5\" waehlen" << std::endl;
	ButtonFullscreen=get_remote_button_name("Vollbild ");
	ButtonFullscreen=search_kodi_key_name(kodi_lircmap_xml,ButtonFullscreen);
	std::cout << std::endl;
	

	std::cout << "Wenn keine freie Taste vorhanden ist \"1\" waehlen" << std::endl;
	ButtonQueue=get_remote_button_name("Titel in Warteschlange kopieren ");
	ButtonQueue=search_kodi_key_name(kodi_lircmap_xml,ButtonQueue);
	std::cout << std::endl;
	

	std::cout << "Wenn keine freie Taste vorhanden ist \"3\" waehlen" << std::endl;
	ButtonPlaylist=get_remote_button_name("Playlist anzeigen ");
	ButtonPlaylist=search_kodi_key_name(kodi_lircmap_xml,ButtonPlaylist);
	std::cout << std::endl;
	

	std::cout << "Wenn keine freie Taste vorhanden ist \"9\" waehlen" << std::endl;
	ButtonNextSubtitle=get_remote_button_name("Naechster Untertitel ");
	ButtonNextSubtitle=search_kodi_key_name(kodi_lircmap_xml,ButtonNextSubtitle);
	std::cout << std::endl;
	
	std::cout << "Wenn keine freie Taste vorhanden ist \"4\" waehlen" << std::endl;
	ButtonSkipPrevious=get_remote_button_name("Titel zurueck ");
	ButtonSkipPrevious=search_kodi_key_name(kodi_lircmap_xml,ButtonSkipPrevious);
	std::cout << std::endl;
	
	std::cout << "Wenn keine freie Taste vorhanden ist \"6\" waehlen" << std::endl;
	ButtonSkipNext=get_remote_button_name("Titel vor ");
	ButtonSkipNext=search_kodi_key_name(kodi_lircmap_xml,ButtonSkipNext);
	std::cout << std::endl;

	
	f_kodi_keymap.open(kodi_keymap_xml.c_str(), std::fstream::out);

	if (!f_kodi_keymap){
		std::cerr << "Datei: " << kodi_keymap_xml << "kann nicht geoeffnet werden" << std::endl;
		wait_key_return();
		exit(1);
	}
	
	f_kodi_keymap << "<!-- Keymap.xml Version 0.1 25.03.2014 by Bleifuss -->\n" << std::endl;
	f_kodi_keymap << "<!-- Keymap.xml mit " << ProgramName << " generiert -->\n" << std::endl;
	f_kodi_keymap << "<keymap>" << std::endl;
//#####################
	f_kodi_keymap << "<MyMusicPlaylist>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;
	if (ButtonDelete!="")
		f_kodi_keymap << "   <" << ButtonDelete <<">" << "Delete" << "</" << ButtonDelete <<">" << std::endl;
	if (ButtonMoveItemUp!="")
		f_kodi_keymap << "   <" << ButtonMoveItemUp <<">" << "MoveItemUp" << "</" << ButtonMoveItemUp <<">" << std::endl;
	if (ButtonMoveItemDown!="")
		f_kodi_keymap << "   <" << ButtonMoveItemDown <<">" << "MoveItemDown" << "</" << ButtonMoveItemDown <<">" << std::endl;
	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</MyMusicPlaylist>" << std::endl;

//#####################	
	f_kodi_keymap << "<MyMusicPlaylistEditor>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;
	if (ButtonDelete!="")
		f_kodi_keymap << "   <" << ButtonDelete <<">" << "Delete" << "</" << ButtonDelete <<">" << std::endl;
	if (ButtonMoveItemUp!="")
		f_kodi_keymap << "   <" << ButtonMoveItemUp <<">" << "MoveItemUp" << "</" << ButtonMoveItemUp <<">" << std::endl;
	if (ButtonMoveItemDown!="")
		f_kodi_keymap << "   <" << ButtonMoveItemDown <<">" << "MoveItemDown" << "</" << ButtonMoveItemDown <<">" << std::endl;
	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</MyMusicPlaylistEditor>" << std::endl;
//#####################
	f_kodi_keymap << "<MyMusicFiles>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;

	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;
	if (ButtonQueue!="")
		f_kodi_keymap << "   <" << ButtonQueue <<">" << "Queue" << "</" << ButtonQueue <<">" << std::endl;
	if (ButtonPlaylist!="")
		f_kodi_keymap << "   <" << ButtonPlaylist <<">" << "Playlist" << "</" << ButtonPlaylist <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</MyMusicFiles>" << std::endl;

//#####################
	f_kodi_keymap << "<MyMusicLibrary>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;

	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;
	if (ButtonQueue!="")
		f_kodi_keymap << "   <" << ButtonQueue <<">" << "Queue" << "</" << ButtonQueue <<">" << std::endl;
	if (ButtonPlaylist!="")
		f_kodi_keymap << "   <" << ButtonPlaylist <<">" << "Playlist" << "</" << ButtonPlaylist <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</MyMusicLibrary>" << std::endl;
 
//#####################
	f_kodi_keymap << "<MusicOSD>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;

	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;
	if (ButtonPlaylist!="")
		f_kodi_keymap << "   <" << ButtonPlaylist <<">" << "Playlist" << "</" << ButtonPlaylist <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</MusicOSD>" << std::endl;
 
//#####################
	f_kodi_keymap << "<FullscreenVideo>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;

	if (ButtonNextSubtitle!="")
		f_kodi_keymap << "   <" << ButtonNextSubtitle <<">" << "NextSubtitle" << "</" << ButtonNextSubtitle <<">" << std::endl;
	if (ButtonPlaylist!="")
		f_kodi_keymap << "   <" << ButtonPlaylist <<">" << "Playlist" << "</" << ButtonPlaylist <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</FullscreenVideo>" << std::endl;


//#####################
	f_kodi_keymap << "<VideoMenu>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;

	if (ButtonNextSubtitle!="")
		f_kodi_keymap << "   <" << ButtonNextSubtitle <<">" << "NextSubtitle" << "</" << ButtonNextSubtitle <<">" << std::endl;
	if (ButtonPlaylist!="")
		f_kodi_keymap << "   <" << ButtonPlaylist <<">" << "Playlist" << "</" << ButtonPlaylist <<">" << std::endl;
	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;
	
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</VideoMenu>" << std::endl;


//#####################
	f_kodi_keymap << "<Visualisation>" << std::endl;
	f_kodi_keymap << "  <remote>" << std::endl;

	if (ButtonSkipPrevious!="")
		f_kodi_keymap << "   <" << ButtonSkipPrevious <<">" << "SkipPrevious" << "</" << ButtonSkipPrevious <<">" << std::endl;
	if (ButtonSkipNext!="")
		f_kodi_keymap << "   <" << ButtonSkipNext <<">" << "SkipNext" << "</" << ButtonSkipNext <<">" << std::endl;

	if (ButtonPlaylist!="")
		f_kodi_keymap << "   <" << ButtonPlaylist <<">" << "Playlist" << "</" << ButtonPlaylist <<">" << std::endl;
	if (ButtonFullscreen!="")
		f_kodi_keymap << "   <" << ButtonFullscreen <<">" << "Fullscreen" << "</" << ButtonFullscreen <<">" << std::endl;

	                                      
	f_kodi_keymap << "  </remote>" << std::endl;
	f_kodi_keymap << "</Visualisation>" << std::endl;

	f_kodi_keymap << "</keymap>" << std::endl;	
	f_kodi_keymap.close();

	std::cout << "Datei \"" << kodi_keymap_xml << "\" erstellt" << std::endl;
	sleep (3);

	

}
//#############################################################################
//kodi_lircmap_xml Pfad zur kodi lircmap
//vdr_remote_conf Pfad zur vdr remote.conf
void lircmap_xml(std::string kodi_lircmap_xml, std::string vdr_remote_conf){
char cSelection;
int i;

//Funktion:
//FB Name feststellen
//Lircmap generieren

std::cout << "\033[2J\033[1;1H";
std::cout << "---------------------------------\n" << std::endl;

//Testen ob der FB Name übergeben wurde
if (RemoteName[0]==" "){
	//Name der Fernbedienung ermitteln
	std::cout << "Eine Taste auf der Fernbedienung druecken \n" << std::endl;

	//Puffer Löschen
	memset(Lircbuf,0,sizeof(Lircbuf));

	//Auf Taste warten
	i=0;
	while (Lircbuf[0]==0){
		usleep (100000);
		i++;
		//5s warten
		if(i>50){
			std::cerr << "Keine Fernbedienung gefunden" << std::endl;
			wait_key_return();
			return;
		}
	}

	RemoteName[0]=Lircbuf;

	//Original String: 000000000000154f 02 KEY_OK TechnoTrend_S2400
	//String vom Ende bis zum trenner durchsuchen

	unsigned found=RemoteName[0].find_last_of(" ");
	//std::string::npos=> String Ende
	if( found  != std::string::npos ){

		//Trenner wurde gefunden string ab " "kopieren 
		found++;							//Trenner entfernen
		RemoteName[0]=RemoteName[0].substr(found,std::string::npos);
		//New Line suchen und löschen
		RemoteName[0].erase ( RemoteName[0].rfind("\n") );
		std::cout << "Aktive Fernbedienung: " << RemoteName[0] << std::endl;
	}
}

//Lircmap erzeugen
std::fstream f_kodi_lircmap;
std::string ButtonName;

	f_kodi_lircmap.open(kodi_lircmap_xml.c_str(), std::fstream::out);
	if (!f_kodi_lircmap){
		std::cerr << "Datei: " << kodi_lircmap_xml << "kann nicht geoeffnet werden" << std::endl;
		wait_key_return();
		exit(1);
	}

	
	f_kodi_lircmap << "<!-- Lircmap.xml Version 0.1 25.03.2014 by Bleifuss -->\n" << std::endl;
	f_kodi_lircmap << "<!-- Lircmap.xml aus VDR remote.conf mit make-remote-mapping generiert -->\n" << std::endl;
	f_kodi_lircmap << "<lircmap>" << std::endl;


	//Tasten anlernen Info
	std::cout << "\n\nBei nicht gewuenschten Funktionen warten\n"<< std::endl;
	std::cout << "Um zu starten ";
	wait_key_return();

	
	int Remote=1;
	//Anzahl der FB's feststellen
	if (RemoteName[1]!=" ")
		Remote=2;

	for(int i=0;i<Remote;i++){ 

		f_kodi_lircmap << "  <remote device=" << "\"" << RemoteName[i] << "\""<< ">" << std::endl;

		std::cout << "Tasten fuer die Fernbedienung: " << RemoteName[i] << "festlegen" << std::endl;

		ButtonName=get_remote_button_name("Kontextmenue (rechte Maustaste)");
		if (ButtonName!="")
			f_kodi_lircmap << "  <title>" << ButtonName << "</title>" << std::endl;

		int key_counter=0;

		while (key_counter!=-1)
			f_kodi_lircmap << "  <up>" << search_lirc_key_name(vdr_remote_conf,LIRC_UP,key_counter) << "</up>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)
			f_kodi_lircmap << "  <down>" << search_lirc_key_name(vdr_remote_conf,LIRC_Down,key_counter) << "</down>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)
			f_kodi_lircmap << "  <left>" << search_lirc_key_name(vdr_remote_conf,LIRC_Left,key_counter) << "</left>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)
			f_kodi_lircmap << "  <right>" << search_lirc_key_name(vdr_remote_conf,LIRC_Right,key_counter) << "</right>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <red>" << search_lirc_key_name(vdr_remote_conf,LIRC_Red,key_counter) << "</red>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <green>" << search_lirc_key_name(vdr_remote_conf,LIRC_Green,key_counter) << "</green>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <yellow>" << search_lirc_key_name(vdr_remote_conf,LIRC_Yellow,key_counter) << "</yellow>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <blue>" << search_lirc_key_name(vdr_remote_conf,LIRC_Blue,key_counter) << "</blue>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <pageplus>" << search_lirc_key_name(vdr_remote_conf,LIRC_Channel_P,key_counter) << "</pageplus>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <pageminus>" << search_lirc_key_name(vdr_remote_conf,LIRC_Channel_M,key_counter) << "</pageminus>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <forward>" << search_lirc_key_name(vdr_remote_conf,LIRC_FastFwd,key_counter) << "</forward>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <reverse>" << search_lirc_key_name(vdr_remote_conf,LIRC_FastRew,key_counter) << "</reverse>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <select>" << search_lirc_key_name(vdr_remote_conf,LIRC_Ok,key_counter) << "</select>" << std::endl;
	
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <menu>" << search_lirc_key_name(vdr_remote_conf,LIRC_Menu,key_counter) << "</menu>" << std::endl;
	
		key_counter=0;
		while (key_counter!=-1)		
			f_kodi_lircmap << "  <back>" << search_lirc_key_name(vdr_remote_conf,LIRC_Back,key_counter) << "</back>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <one>" << search_lirc_key_name(vdr_remote_conf,"LIRC.1",key_counter) << "</one>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <two>" << search_lirc_key_name(vdr_remote_conf,"LIRC.2",key_counter) << "</two>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <three>" << search_lirc_key_name(vdr_remote_conf,"LIRC.3",key_counter) << "</three>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <four>" << search_lirc_key_name(vdr_remote_conf,"LIRC.4",key_counter) << "</four>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)		
			f_kodi_lircmap << "  <five>" << search_lirc_key_name(vdr_remote_conf,"LIRC.5",key_counter) << "</five>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <six>" << search_lirc_key_name(vdr_remote_conf,"LIRC.6",key_counter) << "</six>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <seven>" << search_lirc_key_name(vdr_remote_conf,"LIRC.7",key_counter) << "</seven>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <eight>" << search_lirc_key_name(vdr_remote_conf,"LIRC.8",key_counter) << "</eight>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <nine>" << search_lirc_key_name(vdr_remote_conf,"LIRC.9",key_counter) << "</nine>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <zero>" << search_lirc_key_name(vdr_remote_conf,"LIRC.0",key_counter) << "</zero>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <play>" << search_lirc_key_name(vdr_remote_conf,LIRC_Play,key_counter) << "</play>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <pause>" << search_lirc_key_name(vdr_remote_conf,LIRC_Pause,key_counter) << "</pause>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <stop>" << search_lirc_key_name(vdr_remote_conf,LIRC_Stop,key_counter) << "</stop>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <record>" << search_lirc_key_name(vdr_remote_conf,LIRC_Record,key_counter) << "</record>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <volumeplus>" << search_lirc_key_name(vdr_remote_conf,LIRC_Volume_P,key_counter) << "</volumeplus>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <volumeminus>" << search_lirc_key_name(vdr_remote_conf,LIRC_Volume_M,key_counter) << "</volumeminus>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <mute>" << search_lirc_key_name(vdr_remote_conf,LIRC_Mute,key_counter) << "</mute>" << std::endl;

//	f_kodi_lircmap << "  <power>" << search_lirc_key_name(vdr_remote_conf,LIRC_Power) << "</power>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <skipplus>" << search_lirc_key_name(vdr_remote_conf,LIRC_Next,key_counter) << "</skipplus>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <skipminus>" << search_lirc_key_name(vdr_remote_conf,LIRC_Prev,key_counter) << "</skipminus>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <language>" << search_lirc_key_name(vdr_remote_conf,LIRC_AUDIO,key_counter) << "</language>" << std::endl;
		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <subtitle>" << search_lirc_key_name(vdr_remote_conf,LIRC_Recordings,key_counter) << "</subtitle>" << std::endl;

		key_counter=0;
		while (key_counter!=-1)	
			f_kodi_lircmap << "  <recordedtv>" << search_lirc_key_name(vdr_remote_conf,LIRC_Subtitles,key_counter) << "</recordedtv>" << std::endl;


		f_kodi_lircmap << "  </remote>" << std::endl;
	}
	f_kodi_lircmap << "</lircmap>" << std::endl;	
	f_kodi_lircmap.close();


	std::cout << "\nDatei \"" << kodi_lircmap_xml << "\" erstellt" << std::endl;
	if(Quiet==true)
    	    exit (0); 
	sleep (3);

}
//#############################################################################
void print_help (){
	std::cout << std::endl << std::endl;
	std::cout << "---------------------------------\n" << std::endl;
	std::cout << "KODI/Pchanger Fernbedienungs Setup Parameter:" << std::endl << std::endl;
	std::cout << "-r vdr remote.conf, Datei wird nur gelesen" << std::endl;
	std::cout << "-n Name der Fernbedienung fuer KODI Lircmap konfiguration" << std::endl;
	std::cout << "-e Name der zweiten Fernbedienung fuer KODI Lircmap konfiguration" << std::endl;
	std::cout << "-l Lircmap.xml, Datei wird erzeugt" << std::endl;
	std::cout << "-k Keymap.xml, Datei wird erzeugt" << std::endl;
	std::cout << "-p program-changer-lirc.conf, Datei wird erzeugt" << std::endl;
	std::cout << "-d Skript um Software Poweroff abzuschalten" << std::endl;
	std::cout << "-s Skript fuer program-Changer shutdown aktivierung" << std::endl;
	std::cout << "-w Skript fuer Software Poweroff aktivierung ohne Program-Changer" << std::endl;
	std::cout << "-q Keine Fragen" << std::endl;
	std::cout << "-i info in das syslog schreiben" << std::endl;
	std::cout << "-m  \"0\" Alle Menu Punkte, \"1\" Pchanger Menu \"2\", KODI Menu" << std::endl << std::endl;
	std::cout << "Beispiel:" << std::endl;
	std::cout << "make-remote-mappping -r /etc/vdr/remote.conf -l /home/vdr/.kodi/userdata/Lircmap.xml -k /home/vdr/.kodi/userdata/keymaps/keymap.xml -p /etc/program-changer-lrc.conf" << std::endl;
}


//#############################################################################
main(int argc, char *argv[]){

pthread_t thread;

char cSelection;
int i;

std::cout << "Start";
	//Lirc Thread starten
	if(pthread_create(&thread, NULL, RemoteThread, NULL) != 0) {
		perror("pthread_create");
		wait_key_return();
		exit (EXIT_FAILURE);
	}
	sleep (1);

	//Lirc erfolgreich initialisiert?
	if(Init_Lirc!=true)
		exit (EXIT_FAILURE);

	//Pfad zur vdr remote.conf
	std::string vdr_remote_conf;
	std::string pchanger_lirc_conf;
	std::string kodi_lircmap_xml;
	std::string kodi_keymap_xml;
	std::string remote_conf_disable_power_off;
	std::string program_changer_lirc_conf_enable_power_off;
	std::string restore_remote_conf_power_off;

	int Menu=0;
	
	//Parameter einlesen
	int c;
	while ((c = getopt(argc, argv, "r:l:p:k:m:n:e:d:s:w:hqi")) != -1) {
		switch (c) {
			case 'r':
				vdr_remote_conf = optarg;
			break;
			case 'l':
				kodi_lircmap_xml = optarg;
			break;
			case 'p':
				pchanger_lirc_conf = optarg;
			break;
			case 'k':
				kodi_keymap_xml = optarg;
			break;
			case 'n':
				RemoteName[0] = optarg;
			break;
			case 'e':
				RemoteName[1] = optarg;
			break;
			case 'm':
				Menu = atoi( optarg );
			break;
			case 'q':
				Quiet=true;
			break;
			case 'd':
				remote_conf_disable_power_off = optarg;
			break;
			case 's':
				program_changer_lirc_conf_enable_power_off = optarg;
			break;
			case 'w':
				restore_remote_conf_power_off = optarg;
			break;
			case 'h':
				print_help(); //Hilfe aufrufen
				return 0;
			break;
			case 'i':
				DEBUG=true; 
			break;
			default: //Hilfe aufrufen
				print_help(); //Hilfe aufrufen
				wait_key_return();
				exit(EXIT_FAILURE);
		}
	}
	


	//##################
	info_log("vdr remote.conf:" + vdr_remote_conf);
	info_log("kodi lircmap xml:" + kodi_lircmap_xml);
	info_log("pchanger lirc.conf:" + pchanger_lirc_conf);

	info_log("remote.conf disable power off remote.conf:" + remote_conf_disable_power_off);
	info_log("pchanger enable power off:" + program_changer_lirc_conf_enable_power_off);
	info_log("restore remote.conf power off:" + restore_remote_conf_power_off);
	//#################

	
	while ( cSelection!='6') { 	
		int i=1;
		//Bildschirm löschen
		//std::system("clear");
		//ESC Sequenz clear screen, cursor home
		std::cout << "\033[2J\033[1;1H";

		if(Quiet!=true && (Menu==2 || Menu==0)){ //0 Alles, 2 Nur KODI
			std::cout << "---------------------------------" << std::endl;
			std::cout << "Hauptmenue\n" << std::endl;
			if(Menu==0){
				std::cout << "1 Fernbedienung testen" << std::endl;
				std::cout << "2 Program-Changer Taste anlernen" << std::endl;
			}
			std::cout << "3 KODI Fernbedienung einrichten" << std::endl;
			std::cout << "4 KODI Erweiterte Fernbedienungs Einstellungen" << std::endl;
			std::cout << "5 Einstellungen fuer das ausschalten mit der Fernbedienung" << std::endl;
			std::cout << "6 Ende" << std::endl;
			std::cout << "---------------------------------" << std::endl;
			i=Menu+1;
			std::cout << "Bitte " << i << "-5 waehlen: ";

			std::cin >> cSelection;
			clear_cin_buffer();

			//Falsche Menu Anwahl abfangen
			if(Menu==2)  //KODI Menu
				if( cSelection <'3' )
					cSelection = '0';
		}
		else
			if(Menu==2)
			cSelection='3';//KODI FB einrichten
				else
			cSelection='2';//Pchanger Key anlerenen 


		switch (cSelection) {
			case '1':RemoteTest(); 
			break;
			case '2':program_changer_lirc_conf(pchanger_lirc_conf,vdr_remote_conf); 
			break;
			case '3':lircmap_xml(kodi_lircmap_xml,vdr_remote_conf);
			break;
			case '4':keymap_xml(kodi_keymap_xml,kodi_lircmap_xml);
			break;
			case '5':setup_power_off(remote_conf_disable_power_off,program_changer_lirc_conf_enable_power_off,restore_remote_conf_power_off);
			break;
		}
	}

return error;
}
