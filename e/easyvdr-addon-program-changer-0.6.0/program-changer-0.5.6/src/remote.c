
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "remote.h"
#include "error.h"
#include "program-changer.h"
#include "mouse.h"
#include "pconfig.h"
#include "tools.h"

#define LIRC_COMMAND_SHOW_HIDE_MENU			"SHOW_HIDE_MENU"
#define LIRC_COMMAND_SHOW_HIDE_MENU_DEFAULT		"WAIT_SHOW_HIDE_MENU"
#define LIRC_COMMAND_BACK				"HIDE_MENU"
#define LIRC_COMMAND_SWITCH_MEDIA_CENTER		"SWITCH_MEDIA_CENTER"
#define LIRC_COMMAND_BUTTON				"BUTTON_"
#define LIRC_COMMAND_EXECUTE				"EXECUTE_NO_MOUSE-"
#define LIRC_COMMAND_EXECUTE_MOUSE			"EXECUTE_MOUSE-"


#define LIRC_COMMAND_MOUSE_ON				"POINTER_ON"
#define LIRC_COMMAND_MOVE_POINTER_RIGHT			"POINTER_RIGHT"
#define LIRC_COMMAND_MOVE_POINTER_LEFT			"POINTER_LEFT"
#define LIRC_COMMAND_MOVE_POINTER_DOWN			"POINTER_DOWN"
#define LIRC_COMMAND_MOVE_POINTER_UP			"POINTER_UP"
#define LIRC_COMMAND_POINTER_CLICK_LEFT			"OK"
#define LIRC_COMMAND_POINTER_CLICK_RIGHT		"BLUE"
//#define LIRC_COMMAND_POINTER_CLICK				
#define LIRC_COMMAND_POINTER_CLICK_BACK			"POINTER_CLICK_BACK"
#define LIRC_COMMAND_POINTER_CLICK_FORWARD		"POINTER_CLICK_FORWARD"

#define LIRC_COMMAND_POINTER_WHEEL_UP			"POINTER_WHEEL_UP"
#define LIRC_COMMAND_POINTER_WHEEL_DOWN			"POINTER_WHEEL_DOWN"

#define LIRC_COMMAND_MENU						"RED"
#define LIRC_COMMAND_BOOKMARKS					"GREEN"
#define LIRC_COMMAND_SYSTEM_PROGRAMME_MENU		"YELLOW"
#define LIRC_COMMAND_SYSTEMOPIONS_MENU			"BLUE"

#define LIRC_COMMAND_VOLUME				"EXECUTE_VOLUME-"

#define BUTTON_POINTER_LEFT			 1
#define BUTTON_POINTER_RIGHT		 3
#define BUTTON_POINTER_WHEEL_UP		 4
#define BUTTON_POINTER_WHEEL_DOWN	 5
#define BUTTON_POINTER_WHEEL_LEFT	 6
#define BUTTON_POINTER_WHEEL_RIGHT	 7
#define BUTTON_POINTER_BACK			 8
#define BUTTON_POINTER_FORWARD		 9


#define BUTTON_POINTER_DOWN			10
#define BUTTON_POINTER_UP			11

#define POINTER_FAST_ON				2

static gboolean lirc_data_callback(GIOChannel *source, GIOCondition condition, gpointer data);

static int file_socket = -1;
extern int bMouse;
static int bButton;
int iXFaktor,iYFaktor;
static double dStart_time;	
bool bAlsaMixer=FALSE;
char acSysCommand[220];

static pthread_t threadProgramChangerScript;	

struct lirc_config *config = NULL;


//Der Button Please Wait wird nur gezeichnet wenn das Programm 
//in der gtkmain schleife ist


static gboolean time_handler_wait_program_changer_script_ready(gpointer data)
{
//Die Funktion wird 2x je sekunde aufgerufen
#define MAX_TIME 20*2
static int i;

	//Aufrufe zählen
	if(acSysCommand[0]==0)
	{
		i=0;
		hide_window(TRUE);
		return FALSE;
	}
	else
		i++;

	//Skript hängt Programm beenden und per upstart neu starten
	if (i>=MAX_TIME)
	    exit (1);

return TRUE;
}

void *thread_exec_command_and_please_wait(void *P)
{

int ret;	
		 		
	//Script ausführen
	ret=system(acSysCommand);

	//Skript gelaufen
	acSysCommand[0]=0;

	//Soll Mausfunktion geprüft werden?
	if(program_config.iIndex__Selected_Button!=-1)
	{	//Maus einschalten?
		if(program_config.imouse[program_config.iIndex__Selected_Button]==TRUE)
			init_mouse();
		else
			deinit_mouse();

		//Lautstärke veränderbar?
		if(program_config.adjust_alsamixer_Button[program_config.iIndex__Selected_Button]==TRUE)
			bAlsaMixer=TRUE;
		else
			bAlsaMixer=FALSE;
	}
return NULL;
}

//Wenn keine Button Nummer übergeben wird ist in acCommand der fertige Befehl
void exec_command_and_show_please_wait(int iButton_Nr, char *acCommand)
{
int ret;
if(iButton_Nr>-1)
{
	//Button von 0-9 Button 0 ist Taste 1 Button 9 ist Taste 0
	sprintf(acSysCommand,"%s %d",program_config.main_script,program_config.array_index_button[search_button_index_From_Page(iButton_Nr-1)]);
	program_config.iIndex__Selected_Button=(search_button_index_From_Page(iButton_Nr-1));
}
else
	sprintf(acSysCommand,"%s",acCommand);

if(acSysCommand[0]==0)
	return ;

sprintf(acInfo,"execute command: %s",acSysCommand);
info_log(acInfo);


show_please_wait();

if(acSysCommand[0]!=0)
{
	//Bash Skript ausführen
	ret=pthread_create(&threadProgramChangerScript,NULL, thread_exec_command_and_please_wait, NULL);
	//Thread lösen, beim thread ende wird der Speicher freigegeben
	if (ret == 0)
	{
		pthread_detach(threadProgramChangerScript);
	}

}

//1000 1 sec
g_timeout_add(500, (GSourceFunc) time_handler_wait_program_changer_script_ready, NULL);

}

//True Fast 
void move_speed(int bFast)
{
static float x,y;
	if(bFast==TRUE)			
	{

		iXFaktor=program_config.iXSpeed+(int)x;
 		iYFaktor=program_config.iYSpeed+(int)y;

		if(x==0)
		{
			x=(float)program_config.iXSpeed;
			y=(float)program_config.iYSpeed;
		}
		
		if(x<(program_config.iXSpeed*10))
		{
			x*=1.9;
			y*=1.9;
		}
		
	}
	else
	{
		x=y=0;
		iXFaktor= program_config.iXSpeed;
 		iYFaktor=program_config.iYSpeed;
	}
}


//	Messen wie lange ein Knopf von der Fernbedienung gedrückt wird
//Und die Mouse Geschwindichkeit einstellen
void set_mouse_speed(int iButtonName,int iTimeHold,int iReleaseTime)
{
static double dTime_last_event;
static double dFB_IMP_Speed=20000;	

	//Feststellen ob die Taste gehalten wird
	if(bButton!=iButtonName)
	{
	
		bButton=iButtonName;						
		dTime_last_event=dStart_time=getTime_ms();
		//slow
		move_speed(FALSE);
		return ;
	}
	else
	{

		static double  dSpeed=0;
		double dTime=0;

		dTime=getTime_ms();

		//Testen ob die Taste losgelassen wurde
		if(dTime-dTime_last_event>500)
		{
			//Taste wurde losgelassen, von vorne anfangen
			bButton=0;
			//slow
			move_speed(FALSE);
		}
		else
		{
			//Taste länger als 450ms gedrückt?
			if(dTime-dStart_time>program_config.iButtonHoldTime)
			{
				//Fast
				move_speed(TRUE);
			}
		}

		dTime_last_event=dTime;
	}
}

void check_mouse_click()
{
	static double Starttime=0;

		if(Starttime==0)
		{
			Starttime=getTime_ms();
			mouse_click(BUTTON_POINTER_LEFT);
			return;
		}
		else
		if(getTime_ms()-Starttime > 1500)
		{
			Starttime=0;
			return;
		}
		else
		//Taste ist länger gedrückt -> Doppelklick 
		if(getTime_ms()-Starttime>500)
		{
			Starttime=0;
			mouse_click_double();			
		}
}


void check_lirc_command(char *config_parameter)
{
char acString[200];
int i;
int ret;	
//FILE *pFile=NULL;	
info_log("check_lirc_command");
//************************************************************************
//Funktionen die immer aktiv sind	
	if(bMouse==TRUE &&  gtk_widget_get_sensitive(gtkWindow.Main)!=TRUE)
	{
		if (strcasecmp (config_parameter, LIRC_COMMAND_MOVE_POINTER_RIGHT) == 0) 
		{
			set_mouse_speed(BUTTON_POINTER_RIGHT,program_config.iButtonHoldTime,program_config.iButtonReleaseTime);
			mouse_move(iXFaktor,0);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_MOVE_POINTER_LEFT) == 0)
		{
		 	set_mouse_speed(BUTTON_POINTER_LEFT,program_config.iButtonHoldTime,program_config.iButtonReleaseTime);
			mouse_move(-iXFaktor,0);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_MOVE_POINTER_UP) == 0) 
		{
		 	set_mouse_speed(BUTTON_POINTER_UP,program_config.iButtonHoldTime,program_config.iButtonReleaseTime);
			mouse_move(0,-iYFaktor);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_MOVE_POINTER_DOWN) == 0) 
		{
		 	set_mouse_speed(BUTTON_POINTER_DOWN,program_config.iButtonHoldTime,program_config.iButtonReleaseTime);
			mouse_move(0,iYFaktor);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_CLICK_LEFT) == 0) 
		{
			check_mouse_click();
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_CLICK_RIGHT) == 0) 
		{
			mouse_click(BUTTON_POINTER_RIGHT);
			return;
		}
		else
/*
			if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_CLICK_DOUBLE) == 0) 
		{
			mouse_click_double();
			return;
		}
		else
*/		
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_WHEEL_UP) == 0) 
		{
			mouse_click(BUTTON_POINTER_WHEEL_UP);
			//Gleich nochmal, sonst ist es zu langsam
			mouse_click(BUTTON_POINTER_WHEEL_UP);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_WHEEL_DOWN) == 0) 
		{
			mouse_click(BUTTON_POINTER_WHEEL_DOWN);
			//Gleich nochmal, sonst ist es zu langsam
			mouse_click(BUTTON_POINTER_WHEEL_DOWN);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_CLICK_BACK) == 0) 
		{
			mouse_click(BUTTON_POINTER_BACK);
			return;
		}
		else			
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_CLICK_FORWARD) == 0) 
		{
			mouse_click(BUTTON_POINTER_FORWARD);
			return;
		}
		else			
			bButton=0;
	}
//************ Ende Maus Steuerung *************

	//Testen ob direkt ein Skript gestartet werden soll
	if ( (strncasecmp (config_parameter, LIRC_COMMAND_EXECUTE,(strlen(LIRC_COMMAND_EXECUTE))) == 0) || (strncasecmp (config_parameter, LIRC_COMMAND_EXECUTE_MOUSE,(strlen(LIRC_COMMAND_EXECUTE_MOUSE) )) == 0) )  
	{

		if( strncasecmp (config_parameter, LIRC_COMMAND_EXECUTE_MOUSE,(strlen(LIRC_COMMAND_EXECUTE_MOUSE) ) ) == 0)
		{
			init_mouse();
								
			//Den EXECUTE- Teil entfernen in acString ist jetzt der Skript Befehl
			//-1 String Terminierung abschneiden   
			strcpy(acString,(config_parameter+strlen(LIRC_COMMAND_EXECUTE_MOUSE)));
			}
			else
			{
				deinit_mouse();
				//Den EXECUTE- Teil entfernen in acString ist jetzt der Skript Befehl
				//-1 String Terminierung abschneiden   
				strcpy(acString,(config_parameter+strlen(LIRC_COMMAND_EXECUTE)));
			}
/*
			//Ist das Skript vorhanden?
			if(acString==NULL)
			   error("LIRC_COMMAND_EXECUTE-  Syntax Fehler / kein Skriptname",
			         "LIRC_COMMAND_EXECUTE- Syntax Error / no Scriptname");
			pFile=fopen(acString, "r");
			if(pFile!=NULL)
			{
				fclose(pFile);
	*/
				//Timer für Button Anzeige starten
				program_config.iIndex__Selected_Button=-1;
				exec_command_and_show_please_wait(-1,acString);
				return;
/*
			}
			else
			{
				sprintf(acInfo,"Keine Berechtigung oder \nDatei nicht gefunden: \n %s",acString);
				sprintf(acString,"File not found or Permission error: %s",acString);
				error(acInfo,acString);			
			}*/
		}

//**********************************************************************************
	//Testen ob die Lautstärke verändert werden soll
	if( bAlsaMixer==TRUE && gtk_widget_get_sensitive(gtkWindow.Main)!=TRUE )
	{  	
		if ( strncasecmp (config_parameter, LIRC_COMMAND_VOLUME,strlen(LIRC_COMMAND_VOLUME) ) == 0)
		{

			static double  dTime=0; 
			if(dTime==0)
				dTime=getTime_ms();
			else
			//0,3s Warten 
			if( (getTime_ms()-dTime)<250 )
				return;

			bool static bRekursiv=FALSE;
			if(bRekursiv==FALSE)
			{
				bRekursiv=TRUE;
				//Den EXECUTE- Teil entfernen in acString ist jetzt der Skript Befehl
				strcpy(acString,(config_parameter+strlen(LIRC_COMMAND_VOLUME)));
				ret=system(acString);
				bRekursiv=FALSE;
				dTime=getTime_ms();
				return ;
			}
			else
			return;
		}
	}
	
	if (strcasecmp (config_parameter, LIRC_COMMAND_SHOW_HIDE_MENU_DEFAULT) == 0) 
	{
		//Taste muss mind 2 s gedrückt sein
		static double Starttime=0;
		double dTime;
		static double dRepeat_time;
	
		if(Starttime==0)
		{
			Starttime=dRepeat_time=getTime_ms();
			return;
		}
		else
		{
			dTime=getTime_ms();
			//Testen ob die Taste losgelassen wurde
			if(dTime-dRepeat_time>800)
			{
				Starttime=0;
				return;
			}
			dRepeat_time=dTime;
			//Taste ist länger gedrückt 
			if(dTime-Starttime>1500)
			{
				Starttime=0;			
				if(PchangerKey==FALSE)
					PchangerKey=TRUE;
				else
					PchangerKey=FALSE;
			}
			return;
		}
	}
	else			
	if (strcasecmp (config_parameter, LIRC_COMMAND_SHOW_HIDE_MENU) == 0) 
	{
		//Taste muss mind 2 s gedrückt sein
		static double Starttime=0;
		double dTime;
		static double dRepeat_time;
		static bool bswitch_media_center_run=FALSE;
		info_log("PCH Taste gedrückt");
		dTime=getTime_ms();
		//Testen ob die Taste losgelassen wurde
		if(dTime-dRepeat_time>800)
		{
			info_log("PCH Taste nach Pause erneut gedrückt");
			Starttime=0;
		
		}
		//Taste losgelassen?
		if(Starttime!=0 && bswitch_media_center_run==TRUE)
		{
			info_log("PCH Taste gedrückt Swtitch Skript läuft noch abbruch");
			dRepeat_time=dTime;
			return;
		}
		if(Starttime==0)
		{
			info_log("PCH Taste gedrückt Aktion startet und PCH Fenster ein oder aus");
			Starttime=dRepeat_time=getTime_ms();
			bswitch_media_center_run=FALSE;
			if(PchangerKey==FALSE)
				PchangerKey=TRUE;
			else
				PchangerKey=FALSE;
			return;
		}
		else
		{
			dRepeat_time=dTime;
			//Taste ist länger gedrückt 
			if(dTime-Starttime>600)
			{
				if(PchangerKey==FALSE)
				{
				    info_log("PCH Taste länger gedrückt PCH anzeigen");
					PchangerKey=TRUE;
				}
			}
			if(dTime-Starttime>1300 && bswitch_media_center_run!=TRUE)
			{
				    info_log("Skript Switch media center");

				bswitch_media_center_run=TRUE;
				deinit_mouse();
				program_config.iIndex__Selected_Button=-1;//Mausstuerung deaktivieren
				exec_command_and_show_please_wait(-1,program_config.switch_media_center_script);
				info_log("Skript Ende");
			}
			return;
		}
	}
	else
	if (strcasecmp (config_parameter, LIRC_COMMAND_SWITCH_MEDIA_CENTER) == 0) 
	{
		    info_log("Skript Switch media center ausführen");
		exec_command_and_show_please_wait(-1,program_config.switch_media_center_script);
		return;
	}
	else
	//Ist das Fenster sichtbar?
	if( gtk_widget_get_sensitive(gtkWindow.Main)==TRUE)
	{
		sprintf(acInfo,"lirc.conf Parameter %s",config_parameter);
		info_log(acInfo);
		if (strcasecmp (config_parameter, LIRC_COMMAND_BACK) == 0) 
		{
			PchangerKey=FALSE;
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_MOUSE_ON) == 0) 
		{   
			if(bMouse==FALSE)
				init_mouse();
			else
				deinit_mouse();

			return;
		}
		else
		//Haupt Menü 
		if (strcasecmp (config_parameter, LIRC_COMMAND_MENU) == 0) 
		{  
			show_menu_page(PAGE_PROGRAMS);
			return;
		}
		else
		//System Programme Menü
		if (strcasecmp (config_parameter, LIRC_COMMAND_SYSTEM_PROGRAMME_MENU) == 0) 
		{   
			show_menu_page(PAGE_SYSTEMPROGRAMS);
			return;
		}
		else
		//Systemsteuerung 
		if (strcasecmp (config_parameter, LIRC_COMMAND_SYSTEMOPIONS_MENU) == 0) 
		{   
			show_menu_page(PAGE_SYSTEMOPTIONS);
			return;
		}
		else
		if (strcasecmp (config_parameter, LIRC_COMMAND_BOOKMARKS) == 0) 
		{
			show_menu_page(PAGE_BOOKMARK);
			return;
		}
		else
		//Menü Seiten Durchblättern ?
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_WHEEL_UP) == 0) 
		{   
			Menu_Page_Up();
			return;
		}
		else
		//Menü Seiten Durchblättern ?
		if (strcasecmp (config_parameter, LIRC_COMMAND_POINTER_WHEEL_DOWN) == 0) 
		{   
			Menu_Page_Down();
			return;
		}
		else
		//Testen ob Standard Funktion aktiviert wird
		//Testen ob und wenn ja welcher Button gedrückt wurde
		for(i=1;i<BUTTON_MAX+1;i++)
		{
			sprintf(acString,"%s%d",LIRC_COMMAND_BUTTON,i);

			//Testen Ob Button gedrückt
			if ( strcasecmp (config_parameter, acString) == 0  )
			{
				//Timer für Button Anzeige starten
				exec_command_and_show_please_wait(i,NULL);
				return;
			}
		}
			
	}
	else
	if (strcasecmp (config_parameter, LIRC_COMMAND_SWITCH_MEDIA_CENTER) == 0) 
	{
		mouse_click(BUTTON_POINTER_FORWARD);
		return;
	}

}

//Lirc Initialisieren und Config file laden
//File Zugriffsmodus setzen
bool init_lirc(GtkWidget *Window,char *acProgname,char *acLircConfigFile)
{
static int iError=0;
acSysCommand[0]=0;
	
	if( (file_socket=lirc_init(acProgname,1)) ==-1)
	{   
		if(iError<=0)
		{
			info_log("error Lirc initialization");
			iError=1800;
		}
		iError--;
		return FALSE;
	}

	if (lirc_readconfig (acLircConfigFile, &config, NULL) != 0) 
	{
		//Bei Fehler nur Tastaturbedienung, keine Meldung
//		char acString[PLINE_MAX+1];
//		sprintf(acString,"File not found or Permission error: %s",acLircConfigFile);
//		sprintf(acInfo,"Datei nicht gefunden: \n %s",acLircConfigFile);
//		error(acInfo,acString);
		lirc_deinit();
    	file_socket = -1;
		return FALSE;
	}
	else  
	{
	//Modus umstellen damit lirc_nextcode() nicht auf nächsten event wartet 
		fcntl (file_socket, F_SETFL, O_NONBLOCK);
	}
return TRUE;
}

//Lirc beenden
void deinit_lirc(struct lirc_config *config)
{
	lirc_freeconfig(config);
	lirc_deinit();
}
	
//Lirc Callback einrichten 
bool start_lirc_callback(GtkWidget *Window)
{
static int iError=0;
GIOChannel *ioc;

	if (file_socket<0)
	{
		if(iError<=0)
		{		
			error("Start_lirc_callback file_socket error!!","Start_lirc_callback file_socket error!!");
			iError=1800;
		}
		iError--;
		return(FALSE);
	}
	ioc = g_io_channel_unix_new(file_socket);
	g_io_add_watch(ioc, G_IO_IN, lirc_data_callback, NULL);
	return (TRUE);
}



//Lirc Callbackfunktion
//Tastendrücke mit config Datei vergleichen und wenn passend lirc_command aufrufen 
static gboolean lirc_data_callback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	//config_parameter: String mit Text aus dem configfile vom Parameter config 
	char *code, *config_parameter;
	int bReturn = -1;
  
	//Wenn kein Fehler und ein Zeichen vorhanden is dann Schleife
	while (lirc_nextcode (&code) == 0 && code != NULL) 
	{
		bReturn = 0;
		//Lirc Lib mit config Datei 
		if (config)
		{
			while (lirc_code2char (config, code, &config_parameter) == 0 && config_parameter != NULL) 
			{
				check_lirc_command(config_parameter);
			}
		}
		else
		{
		;
//Lirc Strings von Hand dekodieren 
//			find(code);
		}	
		free (code);
	}

	//Fehler
	if (bReturn == -1) 
	{
		info_log("error Lirc initialization");		
		return FALSE;
	}
	return TRUE;
}
