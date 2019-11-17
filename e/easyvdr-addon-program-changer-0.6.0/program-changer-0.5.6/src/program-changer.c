
//http://zetcode.com/tutorials/gtktutorial/gtktextview/
//http://www.bravegnu.org/gtktext/index.html#AEN32
//http://pronix.linuxdelta.de/C/gtk/gtk_C_Kurs_kapitel10.shtml#10_2
//http://www.pro-linux.de/artikel/1/29/gtk.html

//libgtk-3-dev libgtk-3-0 liblircclient-dev liblirc-client0 lirc libxtst-dev libxtst6 libtool
//Vielleicht :sudo apt-get install libxxf86vm-dev


//Wichtig!! Projekt program_changer (rechtes Fenster) rechts anklicken
//Eigenschaften Flags für C- Kompiler -pthread einstellen

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <pthread.h>
#include "hotkey.h"

#include <glib/gi18n.h>
#include <lirc/lirc_client.h>
#include "error.h"
#include "remote.h"
#include "pconfig.h"
#include "tools.h"

#define ERROR_BACKROUND_FILE_NOT_FOUND  2
#define ERROR_SHOW_HIDE_MENU_FILE_NOT_FOUND  3


//#define Y_Button_Offset 400
#define BUTTON_WIDTH_MAX 300
#define BUTTON_HEIGHT_MAX 170
#define NUMBER_WIDTH_MAX  25
#define NUMBER_HEIGHT_MAX 30
#define NUMBER_X_OFFSET 4
#define NUMBER_Y_OFFSET (BUTTON_HEIGHT_MAX-NUMBER_HEIGHT_MAX-3)
#define TITLE_WIDTH_MAX 940
#define TITLE_HEIGHT_MAX 115
#define COLOR_BUTTON_IMAGE_WIDTH 400
#define COLOR_BUTTON_IMAGE_HEIGHT 45


#define MENU_TITEL_CSS  "menu-title-label"

#define PLEASE_WAIT_WIDTH_MAX 408
#define PLEASE_WAIT_HEIGHT_MAX 57


extern struct lirc_config *config;
extern struct stProgramConfig program_config;


char acProgname[]={"Program-Changer"};
int load_image_gtk(char *acFilename,GtkWidget **gdkImage,int iwidth,int iheight,int iUnrefPixpuf,GdkPixbuf **gdkHD_PixbufDesty);
int create_buttons(void);
//Buttons positionieren
void set_pos_buttons(int iPage_Nr);
void put_buttons_to_fixedbox();
void calc_button_position(int iButtons_at_Page);
void show_menu_page(int iPageNew);
bool switch_window_state(void);

struct stCalcButtonPosition { 
 float fButtonZeile;
 float fxOffset;
 float iZeile;
 float fgWidth;
 float fScreenW;
} CalcButtonPosition;

struct stgtkWindow {
 GtkWidget   *Main;
 GtkWidget   *Event;
} ;

//Variabeln eines Buttons
struct stgtkButton {
 GtkWidget *ButtonLabel;
 GtkWidget *Button;
 GtkWidget *ButtonImage;
};	


struct stgtkMenuLabel {
 GtkWidget *Label;
 GtkWidget *LabelName; 
 };

struct stgBookMark {
 GtkWidget   *NameLabel[10];
	
};

struct stThread_Menu_Start {
bool run;
bool ready;
}Thread_Menu_Start;


struct stgtkWindow gtkWindow;

struct stgtkButton Button[BUTTON_MAX+1];
struct stgtkButton Please_wait_Button;

struct stgtkButton PrgTitleButton;

struct stgtkButton SelectPageButton[4];


GtkWidget   *MenuPage_Programs_Label;
GtkWidget   *MenuPage_Bookmarks_Label;
GtkWidget   *MenuPage_SystemProgramms_Label;
GtkWidget   *MenuPage_SysteOptions_Label;

GtkWidget   *MenuPage_fixedbox;

bool Lirc_OK=FALSE;
bool PchangerKey=FALSE;
GtkWidget	*number_image[10];

//Hintergrund Bild
GtkWidget   *image;

static GdkScreen *gdk_screen = NULL;
static GdkDevice *gdk_pointer;


GdkPixbuf *gdkHD_PixbufDesty;

pthread_t thread,threadBashScript;	

int iMenu_Page=1;
int iScreenWidth,iScreenHeight;
int iButtonMax[PAGE_MAX];
int iDEBUG=1;

//*************************************************************
static int get_monitor_num (GdkMonitor *monitor)
{
GdkDisplay *display;
int n_monitors, i;

	display = gdk_monitor_get_display (monitor);
	n_monitors = gdk_display_get_n_monitors (display);

	for (i = 0; i < n_monitors; i++)
	{
	if (gdk_display_get_monitor (display, i) == monitor)
		return i;
	}
    return -1;
}
//**************************************************
void Menu_Page_Up()
{
static double  dTime=0; 
int i,j;
	
	if(dTime==0)
	   dTime=getTime_ms();
	else
	//0,3s Warten 
	if( (getTime_ms()-dTime)<250 )
		return;

	//Suchen ob es die Seite vorhanden ist
	for(j=1;j<PAGE_MAX;j++)
	{
		iMenu_Page++;
		for(i=0;i<BUTTON_MAX;i++)
		{
			if(iMenu_Page<=PAGE_MAX)
			{
				if(program_config.name_button[i][0]!=0 && program_config.imenu_page_nr_button[i]==iMenu_Page) 
					break;
			}
			else
			{
				iMenu_Page=1;
				break;
			}
		}
		//Seite gefunden?
		if(i<BUTTON_MAX)
			break;
	}
	
	show_menu_page(iMenu_Page);
	
	dTime=getTime_ms();
}

void Menu_Page_Down()
{
static double  dTime=0; 
int i,j;
	if(dTime==0)
	   dTime=getTime_ms();
	else
	//0,3s Warten 
	if( (getTime_ms()-dTime)<250 )
		return;

	//Suchen ob es die Seite vorhanden ist
	for(j=1;j<PAGE_MAX;j++)
	{
		iMenu_Page--;
		for(i=0;i<BUTTON_MAX;i++)
		{
			if(iMenu_Page>0)
			{
				if(program_config.name_button[i][0]!=0 && program_config.imenu_page_nr_button[i]==iMenu_Page) 
					break;
			}
			else
			{
				iMenu_Page=1;
				break;
			}
		}
		//Seite gefunden?
		if(i<BUTTON_MAX)
			break;
	}

	show_menu_page(iMenu_Page);

	dTime=getTime_ms();
}

//**************************************************
void menu_buttons_callback(GtkWidget *widget, gpointer data)
{
	char acSysCommand[300];
	//Button von 0-9 Button 0 ist Taste 1 Button 9 ist Taste 0
	sprintf(acSysCommand,"%s %d",program_config.main_script,program_config.array_index_button[GPOINTER_TO_INT(data)]);
	program_config.iIndex__Selected_Button=GPOINTER_TO_INT(data);

	exec_command_and_show_please_wait(-1,acSysCommand);
}
//**************************************************
void select_page_callback(GtkWidget *widget, gpointer data)
{
	show_menu_page(GPOINTER_TO_INT(data));
}
//**************************************************
void callback_button_press(GtkWidget *widget,GdkEventButton  *pButton,gpointer user_data)
{
	if(pButton->button==4)
		Menu_Page_Up();
	else
	if(pButton->button==5)
		Menu_Page_Down();		
}
//**************************************************
gboolean callback_key_press(GtkWidget *widget, GdkEventKey *pKey, gpointer user_data)
{
//	sprintf(acInfo,"Key %d",pKey->keyval);
//	fprintf(stderr,"Key %d",pKey->keyval);
//	info_log(acInfo);

	if (pKey->type == GDK_KEY_PRESS)
	{
		switch (pKey->keyval)
		{
			case GDK_KEY_1:
				exec_command_and_show_please_wait(1,NULL);
			break;
			case GDK_KEY_2 :
				exec_command_and_show_please_wait(2,NULL);
			break;
			case GDK_KEY_3 :
				exec_command_and_show_please_wait(3,NULL);
			break;
			case GDK_KEY_4 :
				exec_command_and_show_please_wait(4,NULL);
			break;
			case GDK_KEY_5 :
				exec_command_and_show_please_wait(5,NULL);
			break;
			case GDK_KEY_6 :
				exec_command_and_show_please_wait(6,NULL);
			break;
			case GDK_KEY_7 :
				exec_command_and_show_please_wait(7,NULL);
			break;
			case GDK_KEY_8 :
				exec_command_and_show_please_wait(8,NULL);
			break;
			case GDK_KEY_9 :
				exec_command_and_show_please_wait(9,NULL);
			break;
			case GDK_KEY_0 :
				exec_command_and_show_please_wait(10,NULL);
			break;
			case GDK_KEY_Up :
				Menu_Page_Up();
			break;
			case GDK_KEY_Down :
				Menu_Page_Down();
			break;
				
				
		}

		if(pKey->hardware_keycode==program_config.iKey_help)
		{
			char acSysCommand[250];
			sprintf(acSysCommand,"%s %d",program_config.main_script,program_config.array_index_button[8]);
			program_config.iIndex__Selected_Button=8;
			exec_command_and_show_please_wait(-1,acSysCommand);
		}
		else
		if(pKey->hardware_keycode==program_config.iKey_red)
			show_menu_page(PAGE_PROGRAMS);
		else
		if(pKey->hardware_keycode==program_config.iKey_green)   
			show_menu_page(PAGE_BOOKMARK);
		else
		if(pKey->hardware_keycode==program_config.iKey_yellow)	
			show_menu_page(PAGE_SYSTEMPROGRAMS);		
		else
		if(pKey->hardware_keycode==program_config.iKey_blue)	
			show_menu_page(PAGE_SYSTEMOPTIONS);
		else
		if(pKey->hardware_keycode==program_config.iKey_menu)	
			show_menu_page(1);

	//	if(pKey->hardware_keycode==program_config.iKey_help)	
		;//	show_menu_page(1);

	}

return FALSE; 
}

//2 Fenster erzeugen ein Nachrichtenfenster
//und ein Popup Fenster das immer im Vordergrund ist (es kann keine Nachrichten empfangen)
void create_window (char *acProgName,struct stgtkWindow *Window)
{
	
	GdkColor color;
    color.red = 0x0000;
    color.green = 0x0000;
    color.blue = 0x0000;
	char acName[]="-Events";
	char acString[strlen(acProgName)+sizeof(acName)];
	sprintf(acString,"%s%s",acProgName,acName);

	Window->Event = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (Window->Event), acString);
	gtk_window_set_position(GTK_WINDOW(Window->Event), GTK_WIN_POS_CENTER);
	gtk_window_set_decorated( (GTK_WINDOW(Window->Event)),FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(Window->Event),TRUE );
	g_signal_connect (G_OBJECT (Window->Event), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	g_signal_connect (G_OBJECT (Window->Event), "key_press_event", G_CALLBACK (callback_key_press), NULL);
	gtk_window_set_default_size(GTK_WINDOW(Window->Event), 1, 1);	
	gtk_widget_set_can_focus (Window->Event,TRUE);
//	g_signal_connect (G_OBJECT (Window->Event), "button-press-event", G_CALLBACK (callback_button_press), NULL);
//	gtk_widget_set_events(Window->Event,GDK_BUTTON_PRESS_MASK);
	
	

	//Programm Fenster
	Window->Main = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_title (GTK_WINDOW (Window->Main), acProgName);
	gtk_window_set_position(GTK_WINDOW(Window->Main), GTK_WIN_POS_CENTER);
	gtk_window_set_decorated( (GTK_WINDOW(Window->Main)),FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(Window->Main),TRUE );
	gtk_window_set_resizable( (GTK_WINDOW(Window->Main)),TRUE);
	/* Exit when the window is closed */
	g_signal_connect (G_OBJECT (Window->Main), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	gtk_widget_set_events(Window->Main,GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK );
	g_signal_connect (G_OBJECT (Window->Main), "button-press-event", G_CALLBACK (callback_button_press), NULL);
	g_signal_connect (G_OBJECT (Window->Main), "scroll-event", G_CALLBACK (callback_button_press), NULL);
	g_signal_connect (G_OBJECT (Window->Main), "key_press_event", G_CALLBACK (callback_key_press), NULL);
	

}
//***********************************************************
void *exec_bash_script_menu_start(void *P)
{ 
char acBashScript[200];

	if(Thread_Menu_Start.run==TRUE)
		return NULL;

	Thread_Menu_Start.run=TRUE;

  	sprintf(acBashScript,"%s -menu-start",program_config.show_hide_menu);
	
	system(acBashScript);

	sprintf(acInfo,"Execute Command: %s",(char*) acBashScript);
	info_log(acInfo);

	Thread_Menu_Start.run=FALSE;
	Thread_Menu_Start.ready=TRUE;
	return NULL;
}


void *exec_bash_script_menu_end(void *P)
{ 
char acBashScript[200];

	if(Thread_Menu_Start.run==TRUE)
	{
	int i;
		for(i=0;i<20;i++)
		{
			if(Thread_Menu_Start.run==FALSE)
				break;
			else
			 usleep(100000);//0,1s
		}
	}
	
	if(Thread_Menu_Start.ready!=TRUE)
		return NULL;

		
	sprintf(acBashScript,"%s -menu-end",program_config.show_hide_menu);

	
	system(acBashScript);
	sprintf(acInfo,"Execute Command: %s",(char*) acBashScript);
	info_log(acInfo);

	Thread_Menu_Start.ready=FALSE;
	return NULL;
}

//Fenster anzeigen 
//Exec_Script	   -> True Bash Script ausführen, False nur Fenster anzeigen 
//show_please_wait -> True Please Wait anzeigen, False nicht anzeigen
void show_window (bool exec_script, bool show_please_wait)
{
int ret;
	// char acString[strlen(program_config.show_hide_menu)+30];

	//Maus außerhalb des Bildschirms setzen
     gdk_device_warp(gdk_pointer, gdk_screen,  iScreenWidth, iScreenHeight);
	     
	//Menü Anzeigen
	gtk_widget_show(gtkWindow.Main);
	gtk_widget_set_sensitive(gtkWindow.Main,TRUE);

	//Überschrift
	gtk_widget_show(PrgTitleButton.Button);
	
	show_menu_page(PAGE_PROGRAMS);
	
	if(show_please_wait!=TRUE)
		gtk_widget_hide(Please_wait_Button.Button);
	else
		gtk_widget_show(Please_wait_Button.Button);
	
	if(exec_script==TRUE)
	{
		//Bash Skript ausführen
		ret=pthread_create(&threadBashScript,NULL, exec_bash_script_menu_start, NULL);
		//Thread lösen, beim thread ende wird der Speicher freigegeben
		if (ret == 0)
		{
			pthread_detach(threadBashScript);
		}
	}

	
//gtk_window_set_modal (GTK_WINDOW(gtkWindow.Event),TRUE);
//gtk_window_set_transient_for (GTK_WINDOW(gtkWindow.Event),GTK_WINDOW(gtkWindow.Main));	



// Event Window anzeigen (Für Tastatur Events)
gtk_widget_show_all(gtkWindow.Event);
gtk_window_activate_focus(GTK_WINDOW(gtkWindow.Event));
gtk_window_activate_default(GTK_WINDOW(gtkWindow.Event));
	
	
	//	info_log("Show Program-Changer");
}

//******************************************
//Fenster verstecken
//Exec_Script	   -> True Bash Script ausführen, False nur Fenster anzeigen
void hide_window (bool exec_script)
{
int ret;
//Menü Verstecken

 gtk_widget_hide(gtkWindow.Event);
	
 gtk_widget_set_sensitive(gtkWindow.Main,FALSE);			
 gtk_widget_hide(gtkWindow.Main );
	
 if(exec_script==TRUE)
 {
	//Bash Skript ausführen
	ret=pthread_create(&threadBashScript, NULL, exec_bash_script_menu_end, NULL);
	//Thread lösen, beim thread ende wird der Speicher freigegeben
	if (ret == 0)
	{
		pthread_detach(threadBashScript);
	}
 }

// info_log("Hide Program-Changer");
}

//*************************
//Wechsel zwischen Fenster sichtbar / unsichtbar + Bash Skript 
bool switch_window_state()
{
	if(gtk_widget_get_sensitive(gtkWindow.Main)==FALSE)
	{
		show_window (TRUE, FALSE);
		return TRUE;
	}
	else
	{
		hide_window (TRUE);
		return FALSE;
	}
	return FALSE;
}
//*************************
//Prüft ob Temp File vohanden oder Taste gedrückt, wenn ja dann Pchanger anzeigen
static gboolean time_handler_show_main_window(gpointer data)
{
static bool PchangerKeyOld=FALSE;
static int CheckFileCount=0;
static bool File_Show_Pchanger=FALSE;

	CheckFileCount++;

	if(PchangerKeyOld!=PchangerKey)
	{
		PchangerKeyOld=PchangerKey=switch_window_state();
		return TRUE;
	}

	if (CheckFileCount > 5 )
	{
		CheckFileCount=0;
		//Wenn sensitive=FALSE Fenster minimiert
		if(gtk_widget_get_sensitive(gtkWindow.Main)!=TRUE)  
		{   
			//Datei Befehle prüfen und ausführen
			if(check_file_exist(FileComands.Exec)==TRUE )
			{
				program_config.iIndex__Selected_Button=-1;
				//Die Parameter werden im Bashskript aus dem TMP File geholt
				exec_command_and_show_please_wait(0, NULL);
			}
			//Testen ob Datei vorhanden ist
			if(check_file_exist(FileComands.Show_Pchanger)==TRUE ) 
			{
				show_window(TRUE,FALSE);
				File_Show_Pchanger=TRUE;
			}
		}
                else
                if(File_Show_Pchanger==TRUE)
                {
			if(check_file_exist(FileComands.Show_Pchanger)!=TRUE ) 
			{
                                File_Show_Pchanger=FALSE;
                                hide_window (TRUE);
			}
                }
	}
	PchangerKeyOld=PchangerKey;
return TRUE;
}

//Timer starten für Menü auf zu starten
void check_show_main_window()
{
	//1000 1 sec
	g_timeout_add(200, (GSourceFunc) time_handler_show_main_window, NULL);

}
//**************************	
static gboolean time_handler_show_window_5s(gpointer data)
{
static int iTimer=0; 
//Wenn sensitive=FALSE Fenster minimiert
iTimer++;
if( (gtk_widget_get_sensitive(gtkWindow.Main)!=TRUE) && (iTimer<3) )
{
	show_window(TRUE,FALSE);
	return TRUE;
}
else
if(iTimer>60)
{
	hide_window(TRUE);
	return FALSE;
}
return TRUE;
}
//**************************	
static gboolean time_handler_init_lirc(gpointer data)
{
static bool bRekursiv=FALSE;
//info_log("Lirc Check start");
	if(bRekursiv==TRUE)
		return TRUE;
	bRekursiv=TRUE;
//info_log("Init Lirc");
	
	if(init_lirc(gtkWindow.Main,acProgname,program_config.lirc_config)==FALSE)
	{
		bRekursiv=FALSE;
		return TRUE; //TRUE Timer läuft weiter
	}
	else
	if(start_lirc_callback(gtkWindow.Main)==FALSE)
	{
		bRekursiv=FALSE;
		return TRUE;
	}
	//LIRC gefunden ? Timer abschalten
	Lirc_OK=TRUE;
	info_log("Lirc initialization ready");
	return FALSE;
}


//************************
//Timer Starten damit Fenster am Anfang 5s eingeblendet wird
void show_window_on_start()
{
	//1000 1 sec
	g_timeout_add(100, (GSourceFunc) time_handler_show_window_5s, (gpointer) gtkWindow.Main);

}
//**********************
void show_please_wait()
{
	gtk_widget_show(Please_wait_Button.Button);
}
//**********************
//Buttons der gewählen Menü Seite anzeigen
void show_menu_page(int iPageNew)
{
	int i;
	iMenu_Page=iPageNew;

	//Menü Überschrift anzeigen
	if(iMenu_Page>=PAGE_PROGRAMS && iMenu_Page<PAGE_BOOKMARK)
	{
		gtk_widget_show(MenuPage_Programs_Label);
		gtk_widget_hide(MenuPage_Bookmarks_Label);
		gtk_widget_hide(MenuPage_SystemProgramms_Label);
		gtk_widget_hide(MenuPage_SysteOptions_Label);
	}
	else
	//Menü Überschrift anzeigen
	if(iMenu_Page>=PAGE_BOOKMARK && iMenu_Page<PAGE_SYSTEMPROGRAMS)
	{
		gtk_widget_hide(MenuPage_Programs_Label);
		gtk_widget_show(MenuPage_Bookmarks_Label);
		gtk_widget_hide(MenuPage_SystemProgramms_Label);
		gtk_widget_hide(MenuPage_SysteOptions_Label);
	}
	else
	//Menü Überschrift anzeigen
	if(iMenu_Page>=PAGE_SYSTEMPROGRAMS && iMenu_Page<PAGE_SYSTEMOPTIONS)
	{
		gtk_widget_hide(MenuPage_Programs_Label);
		gtk_widget_hide(MenuPage_Bookmarks_Label);
		gtk_widget_show(MenuPage_SystemProgramms_Label);
		gtk_widget_hide(MenuPage_SysteOptions_Label);
	}
	else
	//Menü Überschrift anzeigen
	if(iMenu_Page>=PAGE_SYSTEMOPTIONS && iMenu_Page<PAGE_SYSTEMOPTIONS+10)
	{
		gtk_widget_hide(MenuPage_Programs_Label);
		gtk_widget_hide(MenuPage_Bookmarks_Label);
		gtk_widget_hide(MenuPage_SystemProgramms_Label);
		gtk_widget_show(MenuPage_SysteOptions_Label);
	}

 
	//Die Buttons der jeweiligen Seite anzeigen
	for(i=0;i<BUTTON_MAX;i++)
	{
		if(program_config.name_button[i][0]!=0 ) 
		{
			//Buttons verstecken
			if(program_config.imenu_page_nr_button[i]!=iMenu_Page)
			{
				gtk_widget_hide(Button[i].Button);
				gtk_widget_hide(Button[i].ButtonLabel);
			}
			else
			{
				gtk_widget_show(Button[i].Button);
				gtk_widget_show(Button[i].ButtonLabel);
			}
		}
	}

}
//***********************************************************


//Bashskript Index Nummer suchen von Button
int search_button_index_From_Page(int iButton_Nr)
{
int i,j;
j=0;
	for(i=0;i<BUTTON_MAX;i++)
	{
		if(program_config.imenu_page_nr_button[i]==iMenu_Page)
		{
			if(j==iButton_Nr)
				return i;
			j++;
		}
	}
	return 0;//Nichts gefunden
}


//******************************
//https://mail.gnome.org/archives/gtk-list/2009-September/msg00081.html
void gettextwdht ( char * family , int ptsize , int weight , bool normalstyle ,
                   char * stringtomeasure ,
                   int * wdret , int * htret )
   { 
   PangoFontDescription * fd = pango_font_description_new ( );

   pango_font_description_set_family (fd, family );
   pango_font_description_set_style (fd, normalstyle ? PANGO_STYLE_NORMAL :
                                                       PANGO_STYLE_ITALIC );
   pango_font_description_set_variant (fd, PANGO_VARIANT_NORMAL);
   pango_font_description_set_weight (fd, (PangoWeight)weight );
   pango_font_description_set_stretch (fd, PANGO_STRETCH_NORMAL);
   pango_font_description_set_size (fd, ptsize * PANGO_SCALE);

   PangoContext * context = gtk_widget_get_pango_context ( gtkWindow.Main ) ;
 
   PangoLayout * layout = pango_layout_new ( context );
   pango_layout_set_text ( layout, stringtomeasure, -1 );
   pango_layout_set_font_description ( layout, fd );
   pango_layout_get_pixel_size (layout, wdret , htret );
   g_object_unref ( layout );
   }
//******************************
//CSS File erzeugen
void create_css_config(char *acFilename,int iScreenWidth)
{
FILE *pFile;
int iFont_Size=5;
int iFont_Size_Big;
int iTextWidth=0;
int iTextHeight;


//Font Größe berechnen
	while((iScreenWidth/5)-(iScreenWidth/35)>iTextWidth)
	{
		gettextwdht("Ubuntu",iFont_Size,PANGO_WEIGHT_NORMAL,PANGO_STYLE_NORMAL,"Frontend-Softhddevice",
	            &iTextWidth,&iTextHeight);
		iFont_Size++;
		//fprintf (stderr,"Font_size %d\n",iTextWidth);
	}

	iFont_Size_Big=iFont_Size+(iFont_Size/4);
	
	//fprintf (stderr,"Font_size %d",iFont_Size);

	pFile=fopen(acFilename,"w");
	if(pFile!=NULL)
	{
		fprintf(pFile,"@define-color backround_color           rgba(0,0,0,0.5);\n");
		fprintf(pFile,"@define-color text_color                rgb(170,170,175);\n");
		fprintf(pFile,"@define-color foreground_color          rgb(0,0,0);\n");
		fprintf(pFile,"@define-color button-backround_color    rgb(0,0,0);\n");
		
		fprintf(pFile,"\nGtkWindow {\n");
		fprintf(pFile,"	color: @text_color;\n");
		fprintf(pFile,"	background-color: @backround_color;\n");
		fprintf(pFile,"	border-color: @text_color;\n");
		fprintf(pFile,"	}\n");
		
		fprintf(pFile,"\nGtkLabel {\n");	//Allgemeine Text Parameter
		fprintf(pFile,"	color: @text_color;\n"); 
		fprintf(pFile,"	background-color: @backround_color;\n");
		fprintf(pFile,"	font: %d Ubuntu;\n",iFont_Size);
		fprintf(pFile,"	}\n");

	
		fprintf(pFile,"\nGtkButton, GtkEntry {\n");   
		fprintf(pFile,"	color: @text_color;\n"); 
		fprintf(pFile,"	background-color: @button-backround_color;\n");
		fprintf(pFile,"	font: %d Ubuntu;\n",iFont_Size);
		fprintf(pFile,"	}\n");

		fprintf(pFile,"\nGtkLabel#menu-title-label {\n");
		fprintf(pFile,"	color: @text_color;\n");
		fprintf(pFile,"	font: %d Ubuntu;\n",iFont_Size_Big); 
		fprintf(pFile,"	}\n");

		fclose(pFile);
	}
}


int main (int argc, char *argv[])
{
	
int i;
GdkRectangle rectangle;
GdkRectangle* prec = &rectangle;
GdkDisplay *gdk_display;
GdkMonitor *primary;
GdkMonitor *monitor; 
GdkSeat *gdk_seat_display;
	

Thread_Menu_Start.ready=Thread_Menu_Start.run=FALSE;

//Wenn mehrere Sprachen verwendet werden muss noch geprüft werden
/*
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
*/
	info_log("start up Version 5.6");
	 
	//GTK Initialisieren	
	gtk_init (&argc, &argv);

	//Haupt Fenster
	create_window (acProgname,&gtkWindow);

	//Bildschirmauflösung  vom primären Bildschirm feststellen
	gdk_screen = gtk_window_get_screen(GTK_WINDOW(gtkWindow.Main));
	gdk_display = gdk_screen_get_display (gdk_screen);
	primary = gdk_display_get_primary_monitor (gdk_display);

	if (primary)
	
		monitor = gdk_display_get_monitor (gdk_display, get_monitor_num (primary));

	else
		monitor = 0;
 
	gdk_monitor_get_geometry (monitor, prec);


	iScreenWidth = prec->width;
	iScreenHeight = prec->height;

	//Default Screen muß nicht der sein auf dem der Program-changer läuft
	gdk_display = gdk_display_get_default ();
	   
	gdk_seat_display=gdk_display_get_default_seat (gdk_display);
	gdk_pointer=gdk_seat_get_pointer (gdk_seat_display);

	sprintf(acInfo,"Screen Resolution: Width: %d Height %d",iScreenWidth,iScreenHeight);
	info_log(acInfo);

	
	
//************** Parameter Testen ************
	if(argc>2)
	{
		error("Zu viele Parameter \n 1 Parameter erwartet: \n  program-canger.conf",
		      "Wrong Parameter 1 Parameter conf Filename : program-canger.conf");
		exit(EXIT_FAILURE);
	}
	else
	if(argc==1)
	{
		error("Parameter erwartet: \n  program-canger.conf",
		      "Wrong Parameter 1 Parameter conf Filename : program-canger.conf");
		exit(EXIT_FAILURE);
	}


	//Konfiguration laden
	readconfig(argv[1]);

	//************ css
	GtkCssProvider *css_provider;
	css_provider = gtk_css_provider_new ();

	//Testen ob eine css Datei vorhanden ist
	if(check_file_exist(program_config.css_config)!=TRUE)
		create_css_config(program_config.css_config,iScreenWidth);

	//Testen ob eine css Datei vorhanden ist
	if(check_file_exist(program_config.css_config)==TRUE)
		if(gtk_css_provider_load_from_path (css_provider,program_config.css_config,NULL)==TRUE)
		{
			GdkDisplay *display = gdk_display_get_default ();
			GdkScreen *screen = gdk_display_get_default_screen (display);
			gtk_style_context_add_provider_for_screen (screen,
											GTK_STYLE_PROVIDER (css_provider),
											GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
		}
	g_object_unref (css_provider);

//*********** Testen ob die Dateien vorhanden sind **********  
	if(program_config.show_hide_menu==NULL)
	   error("Show_hide_menu Syntax Fehler / kein Skriptname",
			 "Show_hide_menu Syntax Syntax Error / no Scriptname");

	if(check_file_exist(program_config.show_hide_menu)!=TRUE)
	{
		char acText[]="Show_hide_menu: File not found or Permission error:";
		char acString[strlen(program_config.show_hide_menu)+sizeof(acText)];
		sprintf(acInfo,"Show_hide_menu \nKeine Berechtigung oder  \nDatei nicht gefunden: \n  %s  ",program_config.show_hide_menu);
		sprintf(acString,"%s %s",acText,program_config.show_hide_menu);
		error(acInfo,acString);
		exit(EXIT_FAILURE);
	}
		
	
//Fenster Größe auf maximalen Bildschirm setzen (muss vor dem ersten show aufruf sein)
	gtk_window_set_default_size(GTK_WINDOW(gtkWindow.Main), iScreenWidth, iScreenHeight);	
//Button mit Grafik im Fenstermanager einschalten
	GtkSettings *default_settings = gtk_settings_get_default();
	g_object_set(default_settings, "gtk-button-images", TRUE, NULL);


//Buttons erzeugen 
	if(create_buttons()==-1)
		exit(EXIT_FAILURE);

	MenuPage_fixedbox=gtk_fixed_new();
	

	//*********** Testen ob die Dateien vorhanden sind **********  
	//Hintergrund Bild	
	if(program_config.backround_picture_yes==TRUE)
	{
		if( (load_image_gtk(program_config.backround_picture,&image,iScreenWidth,iScreenHeight,TRUE,&gdkHD_PixbufDesty)) ==TRUE)
		{
			gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), image,0,0);
		}
	}

	put_buttons_to_fixedbox();
	
	//Box zum Fenster hinzufügen
	gtk_container_add(GTK_CONTAINER(gtkWindow.Main),MenuPage_fixedbox);

	//Fenster einmal anzeigen dann werden die variablen für die Button Größe von GTK gesetzt
	gtk_widget_show_all(gtkWindow.Main);

	//Fenster nicht anzeigen Initialisierung Beendet.
	gtk_widget_hide(gtkWindow.Main);

	//Buttons berechnen
	for(i=0;i<PAGE_MAX;i++)
		set_pos_buttons(i);
 
		
		
//************** Lirc testen und initialisieren *********
	//Prüfen ob Lirc eingerichtet ist
	if(check_file_exist(program_config.lirc_config)==TRUE)
	{
		//Alle 2 Sekunden prüfen ob Lirc bereit ist
		g_timeout_add_seconds (2, (GSourceFunc) time_handler_init_lirc, (gpointer) gtkWindow.Main);
	}

//Den Program-changer für 4s anzeigen
	//show_window_on_start();

	//Variable Fenster nicht sichtbar setzen
	hide_window(FALSE);

	//Timer starten der prüft ob das Fenster angezeigt werden soll 
	check_show_main_window();

	//Thread für PchangerKey (Hotkey) erzeugen
	pthread_create(&thread, NULL, intercept_key_thread, gtkWindow.Event);
  
	gtk_main ();

//************* Lirc beenden *****************
	if(Lirc_OK==TRUE)
		deinit_lirc(config);	

	stop_intercept_key_thread();

	info_log("exit");
	
return 0;
}



//*********** Bilder laden und skalieren

//iUnrefPixpuf == True Buffer löschen und Image erzeugen

int load_image_gtk(char *acFilename,GtkWidget **gdkImage,int iwidth,int iheight,int iUnrefPixpuf,GdkPixbuf **gdkHD_PixbufDesty)
{
GError *gerror=NULL;
GdkPixbuf *gdkHD_Pixbuf=NULL; 

//Datei prüfen
if(check_file_exist(acFilename)==FALSE)
{
	char acString[300];
	sprintf(acInfo,"load_image_gtk \n Keine Berechtigung oder  \n  Datei nicht gefunden: \n  %s  ",acFilename);
	sprintf(acString,"load_image_gtk: File not found or Permission error: %s",acFilename);
	error(acInfo,acString);
	return(FALSE);
}

//HD Bild laden
gdkHD_Pixbuf=gdk_pixbuf_new_from_file(acFilename,&gerror);

if(gdkHD_Pixbuf==NULL)
{
	error("load_image_gtk: Error gdk_pixbuf_new_from_file",
	      "load_image_gtk: Error gdk_pixbuf_new_from_file");
	return(FALSE);
}

//Bild skalieren ?
if(iwidth>0 || iheight >0)
{
	*gdkHD_PixbufDesty=gdk_pixbuf_scale_simple(gdkHD_Pixbuf,iwidth,iheight,GDK_INTERP_BILINEAR);
	if(*gdkHD_PixbufDesty==NULL)
	{
		error("load_image_gtk: Error gdk_pixbuf_scale_simple",
			  "load_image_gtk: Error gdk_pixbuf_scale_simple");
		return(FALSE);
	}
}
else
{
	if(iUnrefPixpuf==TRUE)
	{
		*gdkImage= gtk_image_new_from_pixbuf(gdkHD_Pixbuf);
	}
	else
	{
		*gdkHD_PixbufDesty=gdkHD_Pixbuf;
		return (TRUE);
	}
}

	//Testen ob Pixelbuffer noch benötigt wird
	//Wenn der Pixbuf verwendet wird kein image erzeugen
	if(iUnrefPixpuf==TRUE)
	{
		*gdkImage= gtk_image_new_from_pixbuf(*gdkHD_PixbufDesty);
		g_object_unref(*gdkHD_PixbufDesty);
		*gdkHD_PixbufDesty=NULL;
	}



g_object_unref(gdkHD_Pixbuf);

return (TRUE);
}



//*********************************************
//Buttons für Fenster erzeugen
int create_buttons()
{
int bReturn=0;
int i=0,i2=0,j,iButton_Nuber[PAGE_MAX];
float fFaktor,f2Faktor;
int iButtonWidth,iButtonHeight;
int iNumberWidth,iNumberHeight;
int	iXNumber, iYNumber;
GdkPixbuf *gdkHD_Pixbuf;


memset(iButtonMax,0,sizeof(iButtonMax));
memset(iButton_Nuber,0,sizeof(iButton_Nuber));


//Größe berechnen
fFaktor=((float)FULL_HD)/((float)TITLE_WIDTH_MAX);

//Größe berechnen
f2Faktor=((float)FULL_HD_HEIGHT)/((float)TITLE_HEIGHT_MAX);

	
//Feststellen wie viele Buttons konfiguriert sind
for(j=0;j<PAGE_MAX;j++)
{
	for(i=0;i<BUTTON_MAX;i++)
	{
		if(program_config.name_button[i][0]!=0 && program_config.imenu_page_nr_button[i]==j) 
		{
			iButtonMax[j]++;
			if(iButtonMax[j]>10)
			{   //Button auf nächste Seite
				program_config.imenu_page_nr_button[i]++;
				iButtonMax[j]--;
			}
		}
		
	}
}

//Menü Name
MenuPage_Programs_Label=gtk_label_new("Standard Menü");
MenuPage_Bookmarks_Label=gtk_label_new("Lesezeichen Seite");
MenuPage_SystemProgramms_Label=gtk_label_new("System Programme");
MenuPage_SysteOptions_Label=gtk_label_new("System Einstellungen");


gtk_widget_set_name(GTK_WIDGET(MenuPage_Programs_Label), MENU_TITEL_CSS);
gtk_widget_set_name(GTK_WIDGET(MenuPage_Bookmarks_Label), MENU_TITEL_CSS);
gtk_widget_set_name(GTK_WIDGET(MenuPage_SystemProgramms_Label), MENU_TITEL_CSS);
gtk_widget_set_name(GTK_WIDGET(MenuPage_SysteOptions_Label), MENU_TITEL_CSS);

//Buttons für Menü Anwahl erzeugen
//Größe des Button Icons berechnen
fFaktor=((float)FULL_HD)/((float)COLOR_BUTTON_IMAGE_WIDTH);
iButtonWidth=((float)iScreenWidth)/fFaktor;

//Größe berechnen
fFaktor=((float)FULL_HD_HEIGHT)/((float)COLOR_BUTTON_IMAGE_HEIGHT);
iButtonHeight=((float)iScreenHeight)/fFaktor;


for(i=0;i<4;i++)
{
	gint Page;	
	switch (i)
	{
		case 0:
				Page=PAGE_PROGRAMS;
		break;

		case 1:
				Page=PAGE_BOOKMARK;
		break;
		case 2:
				Page=PAGE_SYSTEMPROGRAMS;
		break;
		case 3:
				Page=PAGE_SYSTEMOPTIONS;
		break;
	}
	SelectPageButton[i].Button=gtk_button_new();	
			g_signal_connect( G_OBJECT( SelectPageButton[i].Button ),"clicked",
	        G_CALLBACK( select_page_callback ),GINT_TO_POINTER(Page) );

	if(check_file_exist(program_config.select_page_picture_button[i])==FALSE)
		sprintf(program_config.select_page_picture_button[i],"%s",program_config.default_icon);

	//Button Größe ermitteln
//	if( (load_image_gtk(program_config.select_page_picture_button[i],&SelectPageButton[i].ButtonImage,0,0,FALSE,&gdkHD_PixbufDesty)) ==TRUE)
//	{
//		iButtonWidth=gdk_pixbuf_get_width(gdkHD_PixbufDesty);
//		iButtonHeight=gdk_pixbuf_get_height(gdkHD_PixbufDesty);
//		g_object_unref(gdkHD_PixbufDesty);
//	}
		
	
//	//Zielgröße berechnen
//	fFaktor=((float)FULL_HD)/((float)iButtonWidth);
//	iButtonWidth=((float)iScreenWidth)/fFaktor;

	//Buttons Größe berechnen
//	fFaktor=((float)FULL_HD_HEIGHT)/((float)iButtonHeight);
//	iButtonHeight=((float)iScreenHeight)/fFaktor;

	//Bild laden und skalieren
	if((load_image_gtk(program_config.select_page_picture_button[i],&SelectPageButton[i].ButtonImage,iButtonWidth,iButtonHeight,TRUE,&gdkHD_PixbufDesty)) ==TRUE)
	{
		gtk_button_set_image(GTK_BUTTON(SelectPageButton[i].Button), SelectPageButton[i].ButtonImage);
		//gtk_button_set_image_position((GtkButton *)SelectPageButton[i].Button, GTK_POS_LEFT);
	}
}

//Programm Buttons

//Buttons Größe berechnen
fFaktor=((float)FULL_HD)/((float)BUTTON_WIDTH_MAX);
iButtonWidth=((float)iScreenWidth)/fFaktor;

//Buttons Größe berechnen
fFaktor=((float)FULL_HD_HEIGHT)/((float)BUTTON_HEIGHT_MAX);
iButtonHeight=((float)iScreenHeight)/fFaktor;

//Nummer Größe berechnen
fFaktor=((float)FULL_HD)/((float)NUMBER_WIDTH_MAX);
iNumberWidth=((float)iScreenWidth)/fFaktor;

//Nummer Größe berechnen
fFaktor=((float)FULL_HD_HEIGHT)/((float)NUMBER_HEIGHT_MAX);
iNumberHeight=((float)iScreenHeight)/fFaktor;

iXNumber=(int)((float)iScreenWidth)/ ((float)((float)FULL_HD)/((float)NUMBER_X_OFFSET));
iYNumber=(int)((float)iScreenHeight)/((float)((float)FULL_HD_HEIGHT)/((float)NUMBER_Y_OFFSET));

i2=0;
j=1;

	for(i=0;i<BUTTON_MAX;i++)
	{
		if(program_config.imenu_page_nr_button[i]>=PAGE_PROGRAMS && program_config.imenu_page_nr_button[i]<=PAGE_SYSTEMOPTIONS+10)
		{
			Button[i].Button=gtk_button_new();	
			g_signal_connect( G_OBJECT( Button[i].Button ),"clicked",
	        G_CALLBACK( menu_buttons_callback ),GINT_TO_POINTER(i) );
		
			//Button Beschriftung
			Button[i].ButtonLabel=gtk_label_new (program_config.name_button[i]);
		
			if(program_config.name_button[i][0]!=0)
			{
				if(check_file_exist(program_config.picture_button[i])==FALSE)
					sprintf(program_config.picture_button[i],"%s",program_config.default_icon);
			
				if( (load_image_gtk(program_config.picture_button[i],&Button[i].ButtonImage,iButtonWidth,iButtonHeight,FALSE,&gdkHD_PixbufDesty)) ==TRUE)
				{
					if(program_config.imenu_page_nr_button[i]>0)
						iButton_Nuber[program_config.imenu_page_nr_button[i]]++;
	
					//Bild Zahl laden
					if(iButton_Nuber[program_config.imenu_page_nr_button[i]]>9)
					{
						iButton_Nuber[program_config.imenu_page_nr_button[i]]=0;
					}
					i2=iButton_Nuber[program_config.imenu_page_nr_button[i]];
					if( (load_image_gtk(program_config.picture_number[i2],&number_image[i],iNumberWidth,iNumberHeight,FALSE,&gdkHD_Pixbuf)) ==TRUE)
					{
						//Zahlen zum Button Bild hinzufügen
						gdk_pixbuf_composite(  gdkHD_Pixbuf,gdkHD_PixbufDesty,  /* src, dst */
           	        	    iXNumber , iYNumber,             			/* destx, desty */
         			  		iNumberWidth,iNumberHeight,							/* destwdth, destheight */
                            iXNumber , iYNumber,              			/* offset_x, offset_y */
                            1, 1,                    					/* scale_x, scale_y */
                		    GDK_INTERP_BILINEAR, 255 );					/* interp_type, overall_alpha*/ 

						Button[i].ButtonImage= gtk_image_new_from_pixbuf(gdkHD_PixbufDesty);
						gtk_button_set_image(GTK_BUTTON(Button[i].Button), Button[i].ButtonImage);
	
						//Buffer wieder freigeben
						g_object_unref(gdkHD_Pixbuf);
					}
					g_object_unref(gdkHD_PixbufDesty);
					gdkHD_PixbufDesty=NULL;
				}
			}
		}	
	}


	//Größe berechnen
	fFaktor=((float)FULL_HD)/((float)PLEASE_WAIT_WIDTH_MAX);

	//Größe berechnen
	f2Faktor=((float)FULL_HD_HEIGHT)/((float)PLEASE_WAIT_HEIGHT_MAX);


	//Überschrift
	PrgTitleButton.Button=gtk_button_new();
	
	//BildPrgTitleButton für Button laden	
	if( (load_image_gtk(program_config.title_picture,&PrgTitleButton.ButtonImage,((float)iScreenWidth)/fFaktor,((float)iScreenHeight)/f2Faktor,TRUE,&gdkHD_PixbufDesty)) ==TRUE)
	{
		gtk_button_set_image(GTK_BUTTON(PrgTitleButton.Button), PrgTitleButton.ButtonImage);
	}


	//Bitte warten
	Please_wait_Button.Button=gtk_button_new();


	//Bild für Button laden	
	if( (load_image_gtk(program_config.please_wait,&Please_wait_Button.ButtonImage,((float)iScreenWidth)/fFaktor,((float)iScreenHeight)/f2Faktor,TRUE,&gdkHD_PixbufDesty)) ==TRUE)
	{
			gtk_button_set_image(GTK_BUTTON(Please_wait_Button.Button), Please_wait_Button.ButtonImage);
	}

return bReturn;
}	

void put_buttons_to_fixedbox()
{
int i;
	//Genaue Position wird später ermittelt	
	for(i=0;i<BUTTON_MAX;i++)
	{
		if(program_config.imenu_page_nr_button[i]>=PAGE_PROGRAMS && program_config.imenu_page_nr_button[i]<=PAGE_SYSTEMOPTIONS+10)
		{
			gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), Button[i].Button,0,0);
			gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox),Button[i].ButtonLabel,0,0);
		}
	}
		

	gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), PrgTitleButton.Button,0,0);
	//Überschrift
	gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), MenuPage_Programs_Label,0,0);
	gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), MenuPage_Bookmarks_Label,0,0);
	gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), MenuPage_SystemProgramms_Label,0,0);
	gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), MenuPage_SysteOptions_Label,0,0);

	gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), Please_wait_Button.Button,0,0);

	for(i=0;i<4;i++)
	{
		gtk_fixed_put(GTK_FIXED(MenuPage_fixedbox), SelectPageButton[i].Button,0,0);
	}
}


//***************************************************
//Buttons positionieren
void set_pos_buttons(int iPage_Nr)
{
int i,iz,i1;

float fxPos=0;
int static iyPos,iyPosSelectPageButton=0;
int iyOffset,ixOffset;
int ixTOffset,iyTOffset;	

	
GtkAllocation allocation;
gint igWidth,igHeight,igTWidth,igTHeight;
	
	//Programm Überschrift
	//Größe der Bitmap ermitteln
	gtk_widget_get_allocation(PrgTitleButton.ButtonImage,&allocation);
	igWidth=allocation.width;
	igHeight=allocation.height;	

	iyOffset=((float)iScreenHeight/8.3f);
	
	
	gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), PrgTitleButton.Button,( (iScreenWidth-igWidth ) /2),iyOffset);

	iyOffset=igHeight+((float)iScreenHeight/6.0f);
			
	//Menü Überschriften
	//Größer des Text Labels ermitteln
	gtk_widget_get_allocation(MenuPage_Programs_Label,&allocation);
	igWidth=allocation.width;
	igHeight=allocation.height;	

	gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), MenuPage_Programs_Label,( (iScreenWidth-igWidth ) /2),iyOffset);


	//Größer des Text Labels ermitteln
	gtk_widget_get_allocation(MenuPage_Bookmarks_Label,&allocation);
	igWidth=allocation.width;
	igHeight=allocation.height;	

	gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), MenuPage_Bookmarks_Label,( (iScreenWidth-igWidth ) /2),iyOffset);


	//Größer des Text Labels ermitteln
	gtk_widget_get_allocation(MenuPage_SystemProgramms_Label,&allocation);
	igWidth=allocation.width;
	igHeight=allocation.height;	

	gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), MenuPage_SystemProgramms_Label,( (iScreenWidth-igWidth ) /2),iyOffset);

	//Größer des Text Labels ermitteln
	gtk_widget_get_allocation(MenuPage_SysteOptions_Label,&allocation);
	igWidth=allocation.width;
	igHeight=allocation.height;	

	gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), MenuPage_SysteOptions_Label,( (iScreenWidth-igWidth ) /2),iyOffset);


	
	//Bitte warten Button
	//Größe der Bitmap ermitteln
	gtk_widget_get_allocation(Please_wait_Button.ButtonImage,&allocation);
	igWidth=allocation.width;
	igHeight=allocation.height;	

	gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), Please_wait_Button.Button,( (iScreenWidth-igWidth ) /2),(iScreenHeight-igHeight)/2);


//*****************************************************************

	if(iButtonMax[iPage_Nr]>0)
	{
		
		switch(iButtonMax[iPage_Nr])
		{
		case 10:
			CalcButtonPosition.fButtonZeile=5;
		break;
		case 9:
 		  CalcButtonPosition.fButtonZeile=5;
		 break;
		 case 8:
			CalcButtonPosition.fButtonZeile=4;
		break;
		case 7:
 			CalcButtonPosition.fButtonZeile=4;
		break;
		case 6:
 			CalcButtonPosition.fButtonZeile=3;
		break;
		case 5:
 			CalcButtonPosition.fButtonZeile=3;
		break;
		case 4:
  			CalcButtonPosition.fButtonZeile=2;
		break;
		default:
   			CalcButtonPosition.fButtonZeile=iButtonMax[iPage_Nr];
		}

		//Button von dieser Seite suchen 
		i=0;
		while(program_config.imenu_page_nr_button[i]!=iPage_Nr)
			i++;
	
		//Größe der Bitmap ermitteln
		gtk_widget_get_allocation(Button[i].Button,&allocation);
		igWidth=allocation.width;
		igHeight=allocation.height;	

		
		CalcButtonPosition.fgWidth=(float)igWidth;

		calc_button_position(iButtonMax[iPage_Nr]);	
		fxPos=CalcButtonPosition.fxOffset;
		
		iyOffset=((float)iScreenHeight/8.6f);
		iyPos=(int)(((float)iScreenHeight/8.6f)*3.0f);

		iz=i1=0;
		for(i=0;i<BUTTON_MAX;i++)
		{
			if(program_config.imenu_page_nr_button[i]==iPage_Nr)
			{
				gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), Button[i].Button,(int)fxPos,iyPos);		

				//Postiton für Farb Buttons merken

				if( (iyPos+igHeight) > iyPosSelectPageButton )
				{
					iyPosSelectPageButton=(iyPos+igHeight);
				}
				//Größer des Text Labels ermitteln
				gtk_widget_get_allocation(Button[i].ButtonLabel,&allocation);
				//10 Pixel ist der Rand (5 links & 5 rechts)
				igTWidth=allocation.width+10;
				igTHeight=allocation.height;	
				//Text Position berechnen
				ixTOffset=(igWidth-igTWidth)/2;
				iyTOffset=-igTHeight;//igHeight+(igTHeight/3);

				gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), Button[i].ButtonLabel,(int)fxPos+ixTOffset,iyPos+iyTOffset);
				
				fxPos+=CalcButtonPosition.fxOffset+(float)igWidth;
				i1++;
			}
			//Zeile fertig?
			if(i1>=((int)CalcButtonPosition.fButtonZeile))
			{
				i1=0;
				iz++;
				fxPos=CalcButtonPosition.fxOffset;
				iyPos+=igHeight+iyOffset;
				CalcButtonPosition.fButtonZeile=iButtonMax[iPage_Nr]-(int)CalcButtonPosition.fButtonZeile;
			}
			if(iz>=CalcButtonPosition.iZeile)
				break;
		}
	}
//**** Menü Buttons berechnen

	int iButton_size=0;
	for(i=0;i<4;i++)
	{
		//Größe der Buttons ermitteln
		gtk_widget_get_allocation(SelectPageButton[i].Button,&allocation);
		iButton_size+=allocation.width;

	}
	ixOffset=(iScreenWidth-iButton_size)/5;
	for(i=0;i<4;i++)
	{
		//Größe der Buttons ermitteln
		gtk_widget_get_allocation(SelectPageButton[i].Button,&allocation);
		fxPos+=ixOffset;
		//in iyPosSelectPageButton unterkante der Buttons
		iyPos=((iScreenHeight-iyPosSelectPageButton)/2)+iyPosSelectPageButton-((int) ((float)((float)allocation.height/10.0f)*6.0f));;
		gtk_fixed_move(GTK_FIXED(MenuPage_fixedbox), SelectPageButton[i].Button,fxPos,iyPos);
		fxPos+=allocation.width;
	}	
//exit(EXIT_FAILURE);
}

void calc_button_position(int iButtons_at_Page)
{

#define  RAND 40.0f
	CalcButtonPosition.fScreenW=iScreenWidth-RAND;
	
	if( (CalcButtonPosition.fButtonZeile*CalcButtonPosition.fgWidth) > CalcButtonPosition.fScreenW)
	{
		//Soviel wie in 1 Reihe passt zeichnen
		CalcButtonPosition.fButtonZeile=CalcButtonPosition.fScreenW/CalcButtonPosition.fgWidth;
		CalcButtonPosition.fxOffset=(((float)iScreenWidth)-(CalcButtonPosition.fgWidth * CalcButtonPosition.fButtonZeile )) /(CalcButtonPosition.fButtonZeile+1.0f);
		if(CalcButtonPosition.fButtonZeile<iButtons_at_Page)
			CalcButtonPosition.iZeile=2;
		else
			CalcButtonPosition.iZeile=1;
	}
	else
	{
		// 2 Symetrische Reihen
		CalcButtonPosition.fxOffset=( ((float)iScreenWidth)-(CalcButtonPosition.fgWidth * CalcButtonPosition.fButtonZeile ) ) /(CalcButtonPosition.fButtonZeile+1.0f);
		CalcButtonPosition.iZeile=2;
	}
}
