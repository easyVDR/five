/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Peter Reyinger 2013 <easyvdr@gmx.net>
 * 
 * Program-Changer is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Program-Changer is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
* 
 * X Key handlig Template from Wei-Ning Huang (AZ) <aitjcize@gmail.com>
*/
//Bei Projekt konfigurieren diese Optionen hinzufügen -D_REENTRANT -lpthread 
// 'CFLAGS=-D_REENTRANT -lpthread -g ....
//Flag für Compiler -lpthread
//Wichtig beim Projekt anlegen Internationalisierung abschalten

///!!!!  UNREF Prüfen !!!xbmc

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#include <glib/gi18n.h>
#include <syslog.h>
#include <pthread.h>
#include "parameter.h"
#include "message_and_error_log.h"
#include "make_lirc_conf_files.h"
#include "hotkey.h"
#include "tools.h"

#define LIRC_CONFIG					"lirc-config"
#define KEYBOARD_CONFIG				"keybord-config"
#define MAIN_SCRIPT					"main-skript"
#define EASYPORTAL_SCRIPT_PARAMETER	"easyportal-script-parameter"
#define SETUP_CONF					"setup-conf"
#define CCS_CONFIG_FILE				"css-setup-config"
#define LIST_OF_PROGRAMS			"list-of-programs"
#define MAIN_SCRIPT_PARAMETER       "main-script-parameter"

//Programm Parameter
#define PCHANGER_CONF				"-pchanger-conf"
#define REMOTE_MAPPING_SCRIPT		"-remote-mapping-script"
#define ENABLE_SKIN_SCRIPT			"-enable-skin-script"
//Skript Array Daten
#define NAME_BUTTON					"name_button"
#define ENABLE_BUTTON				"enable_button"
#define MENUE_PAGE_NR_BUTTON		"menu_page_nr_button"


#define CHANGE_MENU 1001
#define END_MENU	1002

int readconfig(char *acFilename);
void quit_prog(int iExit_Code);
void read_string(char *acString,char *acParameter,char *acData);


struct stPConf *program_config;

char acFilename_remote_mapping_script[300];
char acFilename_change_skin_script[PFAD_MAX_SETUPPRG];

int PchangerKey=FALSE;


char acList_of_programs_File[PFAD_MAX_SETUPPRG];
char acMain_ScriptParameter_File[PFAD_MAX_SETUPPRG];
int iCheckPchangerButton=FALSE;
pthread_t thread;	


GtkWidget *gtkMainWindow,*menu_dialog;
GtkWidget *check_button[BUTTON_MAX_SETUPPRG],*ok_button;
GtkWidget *keyboard_conf_button;
GtkWidget *Label[23];
GtkWidget *gdkGrid;

int iButton_Index_Start=0,iButton_Index_End=0;
int iDEBUG=FALSE;
int iReturn;
int iSelected_Menu=0;


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

//Skript Array erzeugen
int make_program_changer_script_parameter(char *acFilename)
{
char acString[PFAD_MAX_SETUPPRG];
FILE *pFile;
int i,iArrayIndex=0;
	
	pFile=fopen(acFilename,"w");
	if(pFile==NULL)
	{
		sprintf(acString,"\n  make_program_changer_script_parameter: \n Datei kann nicht gespeichert werden:\n %s \nKeine Berechtigung?",acFilename);
		sprintf(acInfo,"make_program_changer_script_parameter: Error saving File, wrong Permissions? : %s",acFilename);
		error(acString,acInfo);
		return (CREATE_SCRIPT_FILE_ERROR);
	}
	else
	{
		for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
		{
			if(strcasecmp("yes",program_config->enable_button[i]) == 0)
			{
				fprintf(pFile,"#################################################\n");
				fprintf(pFile,"name_button[%d]=%s\n",iArrayIndex,program_config->name_button[i]);
				fprintf(pFile,"picture_button[%d]=%s\n",iArrayIndex,program_config->picture_button[i]);
				fprintf(pFile,"enable_button[%d]=%s\n",iArrayIndex,program_config->enable_button[i]);
				fprintf(pFile,"enable_mouse_button[%d]=%s\n",iArrayIndex,program_config->mouse_emulation_button[i]);	        
				fprintf(pFile,"menu_page_nr_button[%d]=%s\n",iArrayIndex,program_config->menu_page_nr_button[i]);
				fprintf(pFile,"adjust_alsamixer_button[%d]=%s\n",iArrayIndex,program_config->adjust_alsamixer_button[i]);
				fprintf(pFile,"shutdown_program_button[%d]=%s\n",iArrayIndex,program_config->shutdown_program_button[i]);
				fprintf(pFile,"pre_start_program_button[%d]=%s\n",iArrayIndex,program_config->pre_start_program_button[i]);
				fprintf(pFile,"start_program_button[%d]=%s\n",iArrayIndex,program_config->start_program_button[i]);
				fprintf(pFile,"stop_program_button[%d]=%s\n",iArrayIndex,program_config->stop_program_button[i]);
				fprintf(pFile,"kill_program_button[%d]=%s\n",iArrayIndex,program_config->kill_program_button[i]);        
				fprintf(pFile,"root_rights_button[%d]=%s\n",iArrayIndex,program_config->root_rights_button[i]);
				fprintf(pFile,"vdr_remote_button[%d]=%s\n",iArrayIndex,program_config->vdr_remote_button[i]);	        
				fprintf(pFile,"install_apt_get_prg_button[%d]=%s\n",iArrayIndex,program_config->install_apt_get_prg_button[i]);
				fprintf(pFile,"\n");
				iArrayIndex++;				        
			}
		}
		fclose(pFile);
	}
return TRUE;
}
//****************************************************************
//Skript Array erzeugen
int make_easyportal_script_parameter(char *acFilename)
{
char acString[PFAD_MAX_SETUPPRG];
FILE *pFile;
int i,iArrayIndex=0;
	
	pFile=fopen(acFilename,"w");
	if(pFile==NULL)
	{
		sprintf(acString,"\n  make_easyportal_script_parameter: \n Datei kann nicht gespeichert werden:\n %s \nKeine Berechtigung?",acFilename);
		sprintf(acInfo,"make_easyportal_script_parameter: Error saving File, wrong Permissions? : %s",acFilename);
		error(acString,acInfo);
		return (CREATE_SCRIPT_FILE_ERROR);
	}
	else
	{
		for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
		{
			if(strcasecmp("yes",program_config->enable_button_easyportal[i]) == 0)
			{
				fprintf(pFile,"#################################################\n");
				fprintf(pFile,"name_button[%d]=%s\n",iArrayIndex,program_config->name_button[i]);
				fprintf(pFile,"picture_button[%d]=%s\n",iArrayIndex,program_config->picture_button[i]);
				fprintf(pFile,"enable_button[%d]=%s\n",iArrayIndex,program_config->enable_button_easyportal[i]);
				fprintf(pFile,"enable_mouse_button[%d]=%s\n",iArrayIndex,program_config->mouse_emulation_button[i]);        
				fprintf(pFile,"menu_page_nr_button[%d]=%s\n",iArrayIndex,program_config->menu_page_nr_button[i]);
				fprintf(pFile,"adjust_alsamixer_button[%d]=%s\n",iArrayIndex,program_config->adjust_alsamixer_button[i]);
				fprintf(pFile,"shutdown_program_button[%d]=%s\n",iArrayIndex,program_config->shutdown_program_button[i]);
				fprintf(pFile,"pre_start_program_button[%d]=%s\n",iArrayIndex,program_config->pre_start_program_button[i]);
				fprintf(pFile,"start_program_button[%d]=%s\n",iArrayIndex,program_config->start_program_button_easyportal[i]);
				fprintf(pFile,"stop_program_button[%d]=%s\n",iArrayIndex,program_config->stop_program_button[i]);
				fprintf(pFile,"kill_program_button[%d]=%s\n",iArrayIndex,program_config->kill_program_button[i]);        
				fprintf(pFile,"root_rights_button[%d]=%s\n",iArrayIndex,program_config->root_rights_button[i]);
				fprintf(pFile,"vdr_remote_button[%d]=%s\n",iArrayIndex,program_config->vdr_remote_button[i]);	        
				fprintf(pFile,"install_apt_get_prg_button[%d]=%s\n",iArrayIndex,program_config->install_apt_get_prg_button[i]);
				fprintf(pFile,"\n");
				iArrayIndex++;				        
			}
		}
		fclose(pFile);
	}
return TRUE;
}
//******************************************



//******************************************
//Array Speichern
int save_config(char *acFilename)
{
FILE *pFile;
char acString[300];
int i;
	
	pFile=fopen(acFilename,"w");
	if(pFile==NULL)
	{
		sprintf(acString,"\n  save_config: \n Datei kann nicht gespeichert werden:\n %s \nKeine Berechtigung?",acFilename);
		sprintf(acInfo,"save_config: Error saving File, wrong Permissions? : %s",acFilename);
		error(acString,acInfo);
		return (CREATE_CFG_FILE_ERROR);
	}

	for(i=1;i<BUTTON_MAX_SETUPPRG-1;i++)
	{
		if(strcasecmp(program_config->enable_button[i],"yes")==0)
			fprintf(pFile,"PCH_Button %d\n",i-1); //-1 Index beginnt von 0
	}

	for(i=1;i<BUTTON_MAX_SETUPPRG-1;i++)
	{
		if(strcasecmp(program_config->enable_button_easyportal[i],"yes")==0)
			fprintf(pFile,"Portal_Button %d\n",i-1);//-1 Index beginnt von 0
	}
	
	fclose(pFile);
	return(TRUE);
}

//*********************************************
int read_user_config(char *acFilename)
{
FILE *pFile;
char acParameter[50];
char acString[PLINE_MAX];
int i;

		
		//Testen ob eine css Datei vorhanden ist
	if(check_file_exist(acFilename)==TRUE)
	{
		if(make_yes_no_dialog_box("Soll die aktuell Konfiguration verwendet werden?\nBei nein wird die default Konfiguration verwendet.")!=TRUE)
		{
			return(TRUE);
		}

		pFile=fopen(acFilename,"r");
		if(pFile==NULL)
		{
			return(TRUE);
		}

		//Array rücksetzen
		for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
		{
			sprintf(program_config->enable_button[i],"no");
			sprintf(program_config->enable_button_easyportal[i],"no");
		}
	}
        else
         return(TRUE);

	while(feof(pFile)==0)
	{
		//Puffer löschen
		memset(acString,0,PLINE_MAX);
		//Eine Zeile lesen
		if(fgets(acString,PLINE_MAX,pFile)!=NULL)
			//Testen ob Komentar Zeile
			if(memchr(acString,'#',PLINE_MAX)==NULL)
			{
				if(sscanf(acString,"%s %d",acParameter,&i)==2)
				{
					//Button Index Prüfen
					if(strcasecmp(acParameter,"PCH_Button")==0)
					{
						if(i<BUTTON_MAX_SETUPPRG && i>=0) 
							sprintf(program_config->enable_button[i+1],"yes");//+1 Buttons in den Dialogen fangen ab 1 an
					}
					else
					//Button Index Prüfen
					if(strcasecmp(acParameter,"Portal_Button")==0)
						if(i<BUTTON_MAX_SETUPPRG && i>=0) 
							sprintf(program_config->enable_button_easyportal[i+1],"yes");//+1 Buttons in den Dialogen fangen ab 1 an
				}
			}
	}
	fclose(pFile);
		
return(TRUE);
}		


//**************************************************
gboolean callback_key_press(GtkWidget *widget, GdkEventKey *pKey, gpointer user_data)
{
	//sprintf(acInfo,"Key %d",pKey->keyval);
	//fprintf(stderr,"Key %d",pKey->keyval);
	//info_log(acInfo);

	if (pKey->type == GDK_KEY_PRESS)
	{

		//fprintf(stderr," GTK HW  Key d %d \n",pKey->hardware_keycode);//,detail);

/*		
		if(Key_Red==0)
			Key_Red=pKey->keyval;
		else
		if(Key_Green==0)		   
			Key_Green=pKey->keyval;
		else
		if(Key_Yellow==0)		   
			Key_Yellow=pKey->keyval;
		else
		if(Key_Blue==0)		   
			Key_Blue=pKey->keyval;
*/
	}

return FALSE; 
}

//**********************************************************
void check_easyportal_buttons_selected(void)
{
int i;

	for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
	{
		if(strlen(program_config->name_button[i])>2 && check_button[i]!=NULL)
		{
			if(gtk_toggle_button_get_active((GTK_TOGGLE_BUTTON(check_button[i])))==TRUE)
			{   
				sprintf(program_config->enable_button_easyportal[i],"yes");
			}
			else
				sprintf(program_config->enable_button_easyportal[i],"no");
		}
		else
			sprintf(program_config->enable_button_easyportal[i],"no");
	}
}
//**********************************************************
void check_buttons_selected(int iPage_Nr)
{
int i;

for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
{
	if( atoi(program_config->menu_page_nr_button[i])== iPage_Nr )
	{	//Testen ob Variable belegt ist
		if(strlen(program_config->name_button[i])>2 && check_button[i]!=NULL)
		{
			if(gtk_toggle_button_get_active((GTK_TOGGLE_BUTTON(check_button[i])))==TRUE)
			{   
				sprintf(program_config->enable_button[i],"yes");
			}
			else
				sprintf(program_config->enable_button[i],"no");
		}
		else
			sprintf(program_config->enable_button[i],"no");
	}
}
/*
//Testen ob mindestens 1 Button gewählt wurde
if(iButtonCounter<2)
{   //2 Buttons anwählen
			sprintf(program_config->enable_button[0],"no");
	//program_config->array_index_button[1]=TRUE;
}*/
}

//************************************************************************************************
void button_change_skin_callback(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(menu_dialog);


	char acString[300];
	sprintf(acString,"roxterm -e %s",acFilename_change_skin_script);
//	make_info_dialog_box(acFilename_remote_mapping_script,TRUE);
	
	system(acString);
}

//************************************************************************************************
void button_make_lirc_conf_callback(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(menu_dialog);


	char acString[300];
	sprintf(acString,"roxterm -e %s",acFilename_remote_mapping_script);
//	make_info_dialog_box(acFilename_remote_mapping_script,TRUE);
	
	system(acString);
}


//************************************************************************************************
//Grid mit Buttons erzeugen
void show_menu_dialog(int iPage_Nr)
{
int i;
char acString[200];	
gdkGrid = gtk_grid_new ();
GtkWidget *box, *button_menu_dialog;

	//Speicher löschen
	memset(check_button,0,sizeof(check_button));

	button_menu_dialog = gtk_dialog_new_with_buttons ("Bitte Funktionen wählen",
                   		NULL,
                   		GTK_DIALOG_MODAL,
                  		"Ok",END_MENU,
                  		 NULL);

	box = gtk_dialog_get_content_area( GTK_DIALOG( button_menu_dialog ) ); 
	
	
	//************** Texte erzeugen
	Label[0]=gtk_label_new (" ");
	Label[1]=gtk_label_new ("Bitte gewünschte Programme wählen");
	gtk_widget_set_name(GTK_WIDGET(Label[1]), "Title_Label");
	Label[2]=gtk_label_new (" ");
	Label[3]=gtk_label_new ("Die Programme werden der Reihe nach den Buttons zugeordnet");
	Label[4]=gtk_label_new ("Die Reihenfolge kann nur im Skript geändert werden");
	Label[5]=gtk_label_new (" ");


	switch (iPage_Nr)
	{
		case MENU_PAGE_STANDARD_PROGRAM:
			Label[6]=gtk_label_new ("Standard Programme");
			Label[7]=gtk_label_new (" ");
		break;
		case MENU_PAGE_SYSTEMOPTIONS:
			Label[6]=gtk_label_new ("Systemsteuerung");
			Label[7]=gtk_label_new (" ");
		break;
		case MENU_PAGE_INTERNET_BOOKMARKS:
			Label[6]=gtk_label_new ("Internet Bookmarks");
			Label[7]=gtk_label_new (" ");
		break;
		case MENU_PAGE_SYSTEM_PROGRAM:
			Label[6]=gtk_label_new ("System Programme");
			Label[7]=gtk_label_new (" ");
		break;
	}

	for(i=5;i<8;i++)
	{
		gtk_widget_set_name(GTK_WIDGET(Label[i]), "Title_Label");
	}

	for(i=0;i<8;i++)
	{
		gtk_grid_attach (GTK_GRID (gdkGrid), Label[i], 0, i,5,1 );
	}

#define START_ROW 7+3 
int iRow,iColumn=0;
iRow=START_ROW;


	for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
	{
		if(strlen(program_config->name_button[i])>2 && atoi(program_config->menu_page_nr_button[i])== iPage_Nr)
		{
			sprintf(acString,"%s",program_config->name_button[i]);
			//String abkürzen
			acString[18]=0;
			check_button[i]=gtk_check_button_new_with_label( acString );
			gtk_grid_attach (GTK_GRID (gdkGrid), check_button[i],iColumn , iRow, 1, 1 );
		
			iColumn++;
			if(iColumn==5)
			{
				iColumn=0;
				iRow++;
			}
			if( strlen(program_config->name_button[i])>2 && strcasecmp("yes",program_config->enable_button[i]) == 0)
			{
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_button[i]),TRUE);
			}
			else
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_button[i]),FALSE);
		}
		else
		{
			check_button[i]=NULL;
		}
	}

    gtk_container_add (GTK_CONTAINER (box), gdkGrid);

	gtk_widget_show_all( button_menu_dialog );
	
	gtk_dialog_run(GTK_DIALOG(button_menu_dialog));

	//Auswahl übernehmen
	check_buttons_selected(iPage_Nr);
	
	gtk_widget_destroy(button_menu_dialog);
	
}

//************************************************************************************************
//Grid mit Buttons erzeugen
void show_easyportal_menu_dialog()
{
int i;
GdkScreen* gdk_screen = NULL;
GdkRectangle rectangle;
GdkRectangle* prec = &rectangle;
GdkDisplay *gdk_display;
GdkMonitor *primary;
GdkMonitor *monitor; 
GdkSeat *gdk_seat_display;
	

char acString[200];	
gdkGrid = gtk_grid_new ();
GtkWidget *box, *button_menu_dialog,*scrolled_window;


	//Speicher löschen
	memset(check_button,0,sizeof(check_button));
	
	button_menu_dialog = gtk_dialog_new_with_buttons ("Bitte Easyportal Funktionen wählen",
                   		NULL,
                   		GTK_DIALOG_MODAL,
                  		"Ok",END_MENU,
                  		 NULL);

	//Bildschirmauflösung  vom primären Bildschirm feststellen
	gdk_screen = gtk_window_get_screen(GTK_WINDOW(gtkMainWindow));
	gdk_display = gdk_screen_get_display (gdk_screen);
	primary = gdk_display_get_primary_monitor (gdk_display);

	if (primary)
	
		monitor = gdk_display_get_monitor (gdk_display, get_monitor_num (primary));

	else
		monitor = 0;
 
	gdk_monitor_get_geometry (monitor, prec);
	
	scrolled_window = gtk_scrolled_window_new(NULL,NULL);
	//Breite - 5%
	gtk_scrolled_window_set_min_content_width( GTK_SCROLLED_WINDOW (scrolled_window),(gint) prec->width-((prec->width/100)*5));
	//Höhe - 20%	
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW (scrolled_window),(gint) prec->height-((prec->height/100)*20));
	box = gtk_dialog_get_content_area( GTK_DIALOG( button_menu_dialog ) ); 

	
	//************** Texte erzeugen
	Label[0]=gtk_label_new (" ");
	Label[1]=gtk_label_new ("Bitte gewünschte Programme wählen");
	gtk_widget_set_name(GTK_WIDGET(Label[1]), "Title_Label");
	Label[2]=gtk_label_new (" ");
	Label[3]=gtk_label_new ("Die Programme werden der Reihe nach den Buttons zugeordnet");
	Label[4]=gtk_label_new ("Die Reihenfolge kann nur im Skript geändert werden");
	Label[5]=gtk_label_new (" ");

	for(i=5;i<5;i++)
	{
		gtk_widget_set_name(GTK_WIDGET(Label[i]), "Title_Label");
	}

	for(i=0;i<5;i++)
	{
		gtk_grid_attach (GTK_GRID (gdkGrid), Label[i], 0, i,5,1 );
	}

#define START_ROW 7+3 
int iRow,iColumn=0;
iRow=START_ROW;


	for(i=0;i<BUTTON_MAX_SETUPPRG-1;i++)
	{
		if(strlen(program_config->name_button[i])>2 )
		{
			sprintf(acString,"%s",program_config->name_button[i]);
			//String abkürzen
			acString[18]=0;
			check_button[i]=gtk_check_button_new_with_label( acString );
			gtk_grid_attach (GTK_GRID (gdkGrid), check_button[i],iColumn , iRow, 1, 1 );
		
			iColumn++;
			if(iColumn==5)
			{
				iColumn=0;
				iRow++;
			}
			if( strlen(program_config->name_button[i])>2 && strcasecmp("yes",program_config->enable_button_easyportal[i]) == 0)
			{
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_button[i]),TRUE);
			}
			else
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_button[i]),FALSE);
		}
		else
		{
			check_button[i]=NULL;
		}
	}

	gtk_container_add (GTK_CONTAINER (box), scrolled_window);
	gtk_container_add (GTK_CONTAINER (scrolled_window), gdkGrid);
	gtk_widget_show_all( button_menu_dialog );
	
	gtk_dialog_run(GTK_DIALOG(button_menu_dialog));

	//Auswahl übernehmen
	check_easyportal_buttons_selected();
	
	gtk_widget_destroy(button_menu_dialog);
	
}
//********************************************************************************

//********************************************************************************

void button_standard_menu_callback(GtkWidget *widget, gpointer data)
{
	//************** Fenster mit Buttons ****************
	show_menu_dialog(MENU_PAGE_STANDARD_PROGRAM);   

}
	
void button_internet_bookmarks_callback(GtkWidget *widget, gpointer data)
{
	//************** Fenster mit Buttons ****************
	show_menu_dialog(MENU_PAGE_INTERNET_BOOKMARKS);   

}


void button_system_program_callback(GtkWidget *widget, gpointer data)
{
	//************** Fenster mit Buttons ****************
	show_menu_dialog(MENU_PAGE_SYSTEM_PROGRAM);   

}

void button_system_options_callback(GtkWidget *widget, gpointer data)
{
	//************** Fenster mit Buttons ****************
	show_menu_dialog(MENU_PAGE_SYSTEMOPTIONS);   

}

//************************ Keyboard **********************
void button_make_keyboard_conf_callback(GtkWidget *widget, gpointer data)
{

	//Thread für PchangerKey erzeugen
	pthread_create(&thread, NULL, intercept_key_thread, gtkMainWindow);

	gtk_widget_hide(menu_dialog);

	Key_Blue=Key_Yellow=Key_Green=Key_Red=Key_Pchanger=0;

	if(check_file_exist(program_config->keyboard_config)==TRUE)
	{
		//Sollen die Daten vom Setup übernommen werden
		if(make_yes_no_dialog_box("Soll die vorhandene Datei Tastatur Konfiguration überschrieben werden?")==TRUE)
		{
			info_log("Make keyboard-conf ");
			if(make_keyboard_conf("Program-Changer")==TRUE)
				make_info_dialog_box("Tastatur Konfiguration gespeichert\n \n",TRUE);
		}   
	}
	else
	{
		info_log("Make keyboard-conf ");
		if(make_keyboard_conf("Program-Changer")==TRUE)
			make_info_dialog_box("Tastatur Konfiguration gespeichert\n \n",TRUE);
	}

stop_intercept_key_thread();
}

//************************ Easyportal **********************
void button_easyportal_callback(GtkWidget *widget, gpointer data)
{
	//************** Fenster mit Buttons ****************   
	show_easyportal_menu_dialog();
}


//**********************************
GtkWidget* create_window (char *acProgName)
{
	GtkWidget *window;
    
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), acProgName);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_keep_above(GTK_WINDOW(window),FALSE );
	//g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (callback_key_press), NULL);
	return window;
}

//*************
void quit_prog(int iExit_Code)
{
	free(program_config);	
	exit(iExit_Code);
}


int check_argv(int argc, char *argv[],char *acParameter)
{
int i;
//Übergabe Parameter auswerten
for(i=1;i<argc;i++)
{
	if(strcasecmp(argv[i],acParameter) == 0)
	{
		return i;

	}
}
return -1;
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

   PangoContext * context = gtk_widget_get_pango_context ( gtkMainWindow ) ;
 
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
	while((iScreenWidth/5)-(iScreenWidth/30)>iTextWidth)
	{
		gettextwdht("Ubuntu",iFont_Size,PANGO_WEIGHT_NORMAL,PANGO_STYLE_NORMAL,"Frontend-Softhddev",
	            &iTextWidth,&iTextHeight);
		iFont_Size++;
		//fprintf (stderr,"Font_size %d\n",iTextWidth);
	}

	iFont_Size_Big=iFont_Size+(iFont_Size/4);
	
	//fprintf (stderr,"Font_size %d",iFont_Size);

	pFile=fopen(acFilename,"w");
	if(pFile!=NULL)
	{
		fprintf(pFile,"@define-color backround_color           rgb(230,230,230);\n");
		fprintf(pFile,"@define-color text_color                rgb(0,0,0);\n");
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
		fprintf(pFile,"	font: Ubuntu %d;\n",iFont_Size);
		fprintf(pFile,"	}\n");


		fprintf(pFile,"\nGtkButton, GtkEntry {\n");   
		fprintf(pFile,"	-GtkWidget-focus-line-width: 5;\n");
		fprintf(pFile,"	}\n");

		fprintf(pFile,"\nGtkLabel#Title_Label {\n");	//Überschriften
		fprintf(pFile,"	color: @text_color;\n"); 
		fprintf(pFile,"	background-color: @backround_color;\n");
		fprintf(pFile,"	font: Ubuntu %d;\n",iFont_Size_Big);
		fprintf(pFile,"	}\n");

		fprintf(pFile,"\nGtkButton:hover {\n");
		fprintf(pFile,"	background-color: rgb(0,140,200);\n");/* Farbe wenn mit Maus angewählt */ 
		fprintf(pFile,"	-GtkWidget-focus-line-width: 5;\n");
		fprintf(pFile,"	}\n");

		fclose(pFile);
	}
}
//*******************************************************

					 
//**************** Start Menü anzeigen *************
int start_menu()
{
GtkWidget *box;		       
GtkWidget *button_change_skin,*button_make_lirc_conf,*button_make_keyboard_conf;
GtkWidget *button_easyportal;
	
menu_dialog = gtk_dialog_new_with_buttons ("Program-Changer Setup, Bitte wählen",
                   		NULL,
                   		GTK_DIALOG_MODAL,
						"Menü anpassen",CHANGE_MENU,                                
                  		"Ende",END_MENU,
                  		 NULL);

	button_change_skin=gtk_button_new_with_label("Aussehen wählen");
	button_make_lirc_conf=gtk_button_new_with_label("Fernbedienung konfigurieren");
	button_make_keyboard_conf=gtk_button_new_with_label("Tastatur konfigurieren");
    button_easyportal=gtk_button_new_with_label("Easyportal Pchanger Menü konfigurieren");


	g_signal_connect( G_OBJECT( button_change_skin ),
                     "clicked",
                     G_CALLBACK( button_change_skin_callback ),NULL );

	g_signal_connect( G_OBJECT( button_make_lirc_conf ),
                     "clicked",
                     G_CALLBACK( button_make_lirc_conf_callback ),NULL );

	g_signal_connect( G_OBJECT( button_make_keyboard_conf ),
                     "clicked",
                     G_CALLBACK( button_make_keyboard_conf_callback ),NULL );

	g_signal_connect( G_OBJECT( button_easyportal ),
                     "clicked",
                     G_CALLBACK( button_easyportal_callback ),NULL );
	
	box = gtk_dialog_get_content_area( GTK_DIALOG( menu_dialog ) ); 


	gtk_box_pack_start( GTK_BOX( box ), button_change_skin, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX( box ), button_make_lirc_conf, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX( box ), button_make_keyboard_conf, TRUE, TRUE, 0 );
	gtk_box_pack_start( GTK_BOX( box ), button_easyportal, TRUE, TRUE, 0 );
        
    gtk_widget_show_all( menu_dialog );
	
gint result=gtk_dialog_run(GTK_DIALOG(menu_dialog));

	gtk_widget_hide( menu_dialog );
	gtk_widget_destroy(menu_dialog);

	
	switch (result)
	{
		case CHANGE_MENU:
			return CHANGE_MENU;
			break;
		case END_MENU:
			return END_MENU;
			break;
	}
return 1;
}

//**************** Menü Seite wählen anzeigen ****************
int start_select_menu()
{
GtkWidget *box;		       
GtkWidget *button[4];
int i;
	menu_dialog = gtk_dialog_new_with_buttons ("Bitte Menü wählen",
                   		NULL,
                   		GTK_DIALOG_MODAL,
                  		"Ende",END_MENU,
                  		 NULL);


	button[0]=gtk_button_new_with_label("Standard Programme");
	button[1]=gtk_button_new_with_label("Internet Bookmarks");
	button[2]=gtk_button_new_with_label("System Programme");
	button[3]=gtk_button_new_with_label("Systemsteuerung");


	g_signal_connect( G_OBJECT( button[0] ),
                     "clicked",
                     G_CALLBACK( button_standard_menu_callback ),NULL );
	g_signal_connect( G_OBJECT(  button[1] ),
                     "clicked",
                     G_CALLBACK( button_internet_bookmarks_callback ),NULL );

	g_signal_connect( G_OBJECT(  button[2] ),
                     "clicked",
                     G_CALLBACK( button_system_program_callback  ),NULL );

	g_signal_connect( G_OBJECT(  button[3] ),
                     "clicked",
                     G_CALLBACK( button_system_options_callback  ),NULL );

	box = gtk_dialog_get_content_area( GTK_DIALOG( menu_dialog ) ); 


	for(i=0;i<4;i++)
		gtk_box_pack_start( GTK_BOX( box ), button[i], TRUE, TRUE, 0 );
        
    gtk_widget_show_all( menu_dialog );
	
	gtk_dialog_run(GTK_DIALOG(menu_dialog));

	gtk_widget_destroy(menu_dialog);

	return END_MENU;
}

//***********************************************
//************ css
void create_css_provider()
{
FILE *pFile;
GtkCssProvider *css_provider;
css_provider = gtk_css_provider_new ();

GdkRectangle rectangle;
GdkRectangle* prec = &rectangle;
GdkScreen* gdk_screen = NULL;
GdkDisplay *gdk_display;
GdkMonitor *primary;
GdkMonitor *monitor; 
GdkSeat *gdk_seat_display;

int iScreenWidth;

//Bildschirmauflösung  vom primären Bildschirm feststellen
	gdk_screen = gtk_window_get_screen(GTK_WINDOW(gtkMainWindow));
	gdk_display = gdk_screen_get_display (gdk_screen);
	primary = gdk_display_get_primary_monitor (gdk_display);

	if (primary)
	
		monitor = gdk_display_get_monitor (gdk_display, get_monitor_num (primary));

	else
		monitor = 0;
 
	gdk_monitor_get_geometry (monitor, prec);

	iScreenWidth = prec->width;



	//Testen ob eine css Datei vorhanden ist
	if(check_file_exist(program_config->css_config)!=TRUE)
	{
		create_css_config(program_config->css_config,iScreenWidth);
	}

	//Css Datei laden
	pFile=fopen(program_config->css_config,"r");
	if(pFile!=NULL)
	{
		if(gtk_css_provider_load_from_path (css_provider,program_config->css_config,NULL)==TRUE)
		{

			GdkDisplay *display = gdk_display_get_default ();
			GdkScreen *screen = gdk_display_get_default_screen (display);
			gtk_style_context_add_provider_for_screen (screen,
											GTK_STYLE_PROVIDER (css_provider),
											GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

		}
	fclose(pFile);
	}

	g_object_unref (css_provider);
}



//******************************************** Main *******************************
int main (int argc, char *argv[])
{
FILE *pFile;

int i;
char acString[255];
char acFilename[300];
	
char acProgname[50];
sprintf(acProgname,"Program-Changer-Setup");

info_log("Setup start");
	
program_config=(struct stPConf *) malloc(sizeof(struct stPConf));

                       
//GTK Initialisieren	
gtk_init (&argc, &argv);
	
	
	//Haupt Fenster
	gtkMainWindow= create_window (acProgname);
	

	//************** Parameter auswerten
	i=check_argv(argc, argv,PCHANGER_CONF);
	if(i!=-1) 
	{
		//Index auf nächsten Parameter setzen
		i++;
		if(argc>=i)
			sprintf(acFilename,"%s",argv[i]);
		else
		{
			sprintf(acInfo,"Error: %s \"no Filename\" Example: %s /etc/vdr/program-changer.conf",PCHANGER_CONF,PCHANGER_CONF);
			sprintf(acString,"Dateiname fehlt: %s \"Datei Name\" \n Beispiel: %s /etc/vdr/program-changer.conf",PCHANGER_CONF,PCHANGER_CONF); 
			error(acString,acInfo);
			quit_prog(PARAMETER_ERROR);
		}
	}
	else
	{
		sprintf(acInfo,"Missing parameter  Example: \" %s  /etc/vdr/program-changer.conf\"",PCHANGER_CONF);
		sprintf(acString,"Parameter fehlt \n Beispiel: \" %s  /etc/vdr/program-changer.conf\"",PCHANGER_CONF); 
		error(acString,acInfo);
		quit_prog(PARAMETER_ERROR);
	}

	//************** Parameter auswerten
	i=check_argv(argc, argv,REMOTE_MAPPING_SCRIPT);
	if(i!=-1) 
	{
		//Index auf nächsten Parameter setzen
		i++;
		if(argc>=i)
			sprintf(acFilename_remote_mapping_script,"%s",argv[i]);
		else
		{
			sprintf(acInfo,"Error: %s \"no Filename\" Example: %s /usr/share/easyvdr/setup/easyvdr-make-remote-mapping",REMOTE_MAPPING_SCRIPT,REMOTE_MAPPING_SCRIPT);
			sprintf(acString,"Dateiname fehlt: %s \"Datei Name\" \n Beispiel: %s /usr/share/easyvdr/setup/easyvdr-make-remote-mapping",REMOTE_MAPPING_SCRIPT,REMOTE_MAPPING_SCRIPT); 
			error(acString,acInfo);
			quit_prog(PARAMETER_ERROR);
		}
	}
	else
	{
		sprintf(acInfo,"Missing parameter  Example: \" %s  /usr/share/easyvdr/setup/easyvdr-make-remote-mapping\"",REMOTE_MAPPING_SCRIPT);
		sprintf(acString,"Parameter fehlt \n Beispiel: \" %s  /usr/share/easyvdr/setup/easyvdr-make-remote-mapping\"",REMOTE_MAPPING_SCRIPT); 
		error(acString,acInfo);
		quit_prog(PARAMETER_ERROR);
	}


	i=check_argv(argc, argv,ENABLE_SKIN_SCRIPT);
	if(i!=-1) 
	{
		//Index auf nächsten Parameter setzen
		i++;
		if(argc>=i)
			sprintf(acFilename_change_skin_script,"%s",argv[i]);
		else
		{
			sprintf(acInfo,"Error: %s \"no Filename\" Example: %s /usr/share/easyvdr/program-changer/program-changer-enable-skin.sh",ENABLE_SKIN_SCRIPT,ENABLE_SKIN_SCRIPT);
			sprintf(acString,"Dateiname fehlt: %s \"Datei Name\" \n Beispiel: %s /usr/share/easyvdr/program-changer/program-changer-enable-skin.sh",ENABLE_SKIN_SCRIPT,ENABLE_SKIN_SCRIPT); 
			error(acString,acInfo);
			quit_prog(PARAMETER_ERROR);
		}
	}
	else
	{
		sprintf(acInfo,"Missing parameter  Example: \" %s  /usr/share/easyvdr/program-changer/program-changer-enable-skin.sh\"",ENABLE_SKIN_SCRIPT);
		sprintf(acString,"Parameter fehlt \n Beispiel: \" %s  /usr/share/easyvdr/program-changer/program-changer-enable-skin.sh\"",ENABLE_SKIN_SCRIPT); 
		error(acString,acInfo);
		quit_prog(PARAMETER_ERROR);
	}

//*********************************
	


//program-changer-conf & Programm DB Daten laden
if(readconfig(acFilename)!=TRUE)
{
	quit_prog(iReturn);
}
//**************
create_css_provider();



sprintf(acString,"\nProgram-Changer Setup \n\nFür das Setup wird eine Maus empfohlen.\n\
Das Pchanger Setup kann mit dem Easyvdr Tool erneut gestartet werden.");

make_info_dialog_box(acString,TRUE);

//Vom Anweder erzeugte Konfiguration laden
read_user_config(program_config->setup_conf);


//Testen ob eine plirc.conf vorhanden ist
if(check_file_exist(program_config->lirc_config)!=TRUE)
{

//FB Programm starten ####################################
//make_lirc_conf("Program-Changer",acVDR_Remote_conf);
}

//Startmenü anzeigen
while(i)
{
	iSelected_Menu=start_menu();

	if(iSelected_Menu>=CHANGE_MENU)
		break;
}
//Button Menü anzeigen
if(iSelected_Menu==CHANGE_MENU)
{
	//Wahl der Menüseite anzeigen
	while(iSelected_Menu)
	{
		iSelected_Menu=start_select_menu();

		if(iSelected_Menu>=CHANGE_MENU)
			break;
	}	
}

//Auswahl speichern
pFile=fopen(acMain_ScriptParameter_File,"r");
if(pFile!=NULL)
{
	fclose(pFile);
	//Daten vom Setup übernehmen?
	if(make_yes_no_dialog_box("Soll die vorhandene Datei setup.conf überschrieben werden?")==TRUE)
	{
		iReturn=save_config(program_config->setup_conf);
		make_program_changer_script_parameter(acMain_ScriptParameter_File);
		make_easyportal_script_parameter(program_config->easyportal_script_parameter);
	}
		
}
else
{
	iReturn=save_config(program_config->setup_conf);
	make_program_changer_script_parameter(acMain_ScriptParameter_File);
	make_easyportal_script_parameter(program_config->easyportal_script_parameter);
}

if(iReturn!=TRUE)
{
	quit_prog(iReturn);
}

quit_prog(NO_ERROR);
info_log("Setup end");
return (0);
}
//************************************

void DebugPrintConfig(void)
{
int i;
fprintf(stderr,"\n \n Config Parameter \n");
fprintf(stderr,"CSS File: %s \n ",program_config->css_config);
		
//for(i=0;i<BUTTON_MAX_SETUPPRG;i++)
for(i=0;i<5;i++)	
{
	fprintf(stderr,"\n Button  %d: %s",i,program_config->name_button[i]);
	fprintf(stderr,"\n Button  %d: %s",i,program_config->picture_button[i]);	
	fprintf(stderr,"\n Button  %d:enable_button %s",i,program_config->enable_button[i]);
	fprintf(stderr,"\n Button  %d:Maus Emulation: %s",i,program_config->mouse_emulation_button[i]);
	fprintf(stderr,"\n Button  %d:adjust_alsamixer_button: %s",i,program_config->adjust_alsamixer_button[i]);
	fprintf(stderr,"\n Button  %d:shutdown_program_button: %s",i,program_config->shutdown_program_button[i]);
	fprintf(stderr,"\n Button  %d:pre_start_program_button: %s",i,program_config->pre_start_program_button[i]);
	fprintf(stderr,"\n Button  %d:stop_program_button: %s",i,program_config->stop_program_button[i]);
	fprintf(stderr,"\n Button  %d:kill_program_button: %s",i,program_config->kill_program_button[i]);
	fprintf(stderr,"\n Button  %d:root_rights_button: %s",i,program_config->root_rights_button[i]);
	fprintf(stderr,"\n Button  %d:vdr_remote_button: %s",i,program_config->vdr_remote_button[i]);
	fprintf(stderr,"\n Button  %d:install_apt_get_prg_button: %s",i,program_config->install_apt_get_prg_button[i]);
	fprintf(stderr,"\n Button  %d:menu_page_nr_button: %s",i,program_config->menu_page_nr_button[i]);	


	fprintf(stderr,"\n ");
}	
fprintf(stderr,"\n  \n");

}


//*************************** Konfiguration laden ***********************
//Alle Programme einlesen
void ReadProgramList(char *acFilename)
{
	FILE *pFile=NULL;
	char acString[PLINE_MAX+1];

	char acParameter[100];
	char acData[PFAD_MAX_SETUPPRG+1];

	int iButton_NR=0;
	

	pFile=fopen(acFilename, "r");
	if(pFile==NULL)
	{
		sprintf(acString,"\n  ReadProgramList \n Keine Berechtigung oder  \n  Datei nicht gefunden: \n  %s  ",acFilename);
		sprintf(acInfo,"ReadProgramList: File not found or wrong Permissions?: %s",acFilename);
		error(acString,acInfo);
		quit_prog(ERROR_READ_PROGRAM_DB); 
	}
	else
	{
		while(feof(pFile)==0)
		{ 
			//Puffer löschen
			memset(acString,0,sizeof(acString));
			//Eine Zeile lesen
			if(fgets(acString,PLINE_MAX,pFile)!=NULL)
			{
				//Testen ob Komentar Zeile
				if(memchr(acString,'#',PLINE_MAX)==NULL)
				{  
					sscanf(acString,"%s",acParameter);
					//Version?
					if(strcasecmp("Version",acParameter) == 0)
						sprintf(program_config->acVersion,"%s",acData);

					//Button Konfiguration ?
					if(strcasecmp("start-config",acParameter) == 0)
					{
						iButton_NR++;
						while(feof(pFile)==0 && strcasecmp("end-config",acParameter)!=0)
						{
							//Puffer löschen
							memset(acString,0,sizeof(acString));
							//Eine Zeile lesen
							if(fgets(acString,PLINE_MAX,pFile)!=NULL)
							//Testen ob Komentar Zeile oder Ende
								if(memchr(acString,'#',PLINE_MAX)==NULL )
								{
									if(memchr(acString,'=',PLINE_MAX)!=NULL)
									{
										read_string(acString,acParameter,acData);
																				
										if(strcasecmp("button-name",acParameter) == 0)
											sprintf(program_config->name_button[iButton_NR],"%s",acData);
										else
										if(strcasecmp("button-picture",acParameter) == 0)
											sprintf(program_config->picture_button[iButton_NR],"%s",acData);
										else
										if(strcasecmp("default-button-easyportal",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->enable_button_easyportal[iButton_NR],"%s",acData);
										}
										else
										if(strcasecmp("default-button",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->enable_button[iButton_NR],"%s",acData);
										}
										else
										if(strcasecmp("mouse-emulation",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->mouse_emulation_button[iButton_NR],"%s",acData);
										}
										else
										if(strcasecmp("menu-page",acParameter) == 0)
											sprintf(program_config->menu_page_nr_button[iButton_NR],"%s",acData);
										else
										if(strcasecmp("adjust-alsamixer",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->adjust_alsamixer_button[iButton_NR],"%s",acData);
										}
										else
										if(strcasecmp("quit-program",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->shutdown_program_button[iButton_NR],"%s",acData);
										}
										else
										if(strcasecmp("pre-start",acParameter) == 0)
											sprintf(program_config->pre_start_program_button[iButton_NR],"%s",acData);
										else
										if(strcasecmp("start-program",acParameter) == 0)
											sprintf(program_config->start_program_button[iButton_NR],"%s",acData);
										else
										if(strcasecmp("start-program-easyportal",acParameter) == 0)
											sprintf(program_config->start_program_button_easyportal[iButton_NR],"%s",acData);
										else
										if(strcasecmp("stop-program",acParameter) == 0)
											sprintf(program_config->stop_program_button[iButton_NR],"%s",acData);
										else
										if(strcasecmp("kill-program",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->kill_program_button[iButton_NR],"%s",acData);
										}							
										else
										if(strcasecmp("root-rights",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->root_rights_button[iButton_NR],"%s",acData);
										}
										else								
										if(strcasecmp("vdr-remote-on",acParameter) == 0)
										{
											//String begrenzen 
											acData[SIZE_MAX_YES_NO-1]=0;
											sprintf(program_config->vdr_remote_button[iButton_NR],"%s",acData);
										}
										else								
										if(strcasecmp("install-pakage",acParameter) == 0)
										{
											sprintf(program_config->install_apt_get_prg_button[iButton_NR],"%s",acData);
										}
									}
									else//Ende Kennung
										sscanf(acString,"%s",acParameter);
								}
						}
					
					}
				}		
			}
		}
	}
	fclose(pFile);
}

//******************************

void read_string(char *acString,char *acParameter,char *acData)
{
int i=0,j=0;
char *acP;
	
	i=search_no_space(acString);
//fprintf(stderr,"%s,",acString);
	
	//Parameter vor dem '=' lesen
	while(acString[i]!=' ' && acString[i]!=0 && acString[i]!='\n' && acString[i]!='\t')
	{
		acParameter[j]=acString[i];
		j++;
		i++;   
	}
	//String terminieren
	acParameter[j]=0;
//	fprintf(stderr,"%s,",acParameter);

	acP=memchr(acString,'=',PLINE_MAX);
	//' ' Zeichen überspringen
	acP++;
	i=search_no_space(acP);

	j=0;
	//Ab hier Daten
	while(acP[i]!=0 && acP[i]!='\n') 
	{
		acData[j]=*(acP+i);
		i++;
		j++;
	
	}
	//String terminieren
	acData[j]=0;
	//fprintf(stderr,"%s,%d\n",acData,j);
}

//****************************
int readconfig(char *acFilename)
{
	FILE *pFile=NULL;
	char acString[PLINE_MAX+1];

	char acParameter[100];
	char acData[PFAD_MAX_SETUPPRG];

	char acEq[2];
	
	//Puffer löschen
	memset(program_config,0,sizeof(struct stPConf));

	pFile=fopen(acFilename, "r");
	if(pFile==NULL)
	{
		sprintf(acString,"\n  Readconfig \n Keine Berechtigung oder  \n  Datei nicht gefunden: \n  %s  ",acFilename);
		sprintf(acInfo,"Readconfig: File not found or wrong Permissions?: %s",acFilename);
		error(acString,acInfo);
		return ERROR_READ_PCHANGER_CONF_FILE;
	}
	else
	{
		while(feof(pFile)==0)
		{
			//Puffer löschen
			memset(acString,0,sizeof(acString));
			//Eine Zeile lesen
			if(fgets(acString,PLINE_MAX,pFile)!=NULL)
			{
				//Testen ob Kommentar Zeile
				if(memchr(acString,'#',PLINE_MAX)==NULL)
				{
					if(sscanf(acString,"%s %c %s",acParameter,acEq,acData)==3)
					{
						if(strcasecmp(CCS_CONFIG_FILE,acParameter) == 0)
						{
						//css Config Datei
							strcpy(program_config->css_config,acData);
						}
						else
						if(strcasecmp(LIRC_CONFIG,acParameter) == 0)
						{
						//Lirc Config Datei
							strcpy(program_config->lirc_config,acData);
						}
						else
						if(strcasecmp(KEYBOARD_CONFIG,acParameter) == 0)
						{
							//Keyboard Config Datei
							strcpy(program_config->keyboard_config,acData);
						}
						else
						if(strcasecmp(MAIN_SCRIPT,acParameter) == 0)
						{
							//script
							strcpy(program_config->main_script,acData);
						}
						else
						if(strcasecmp(MAIN_SCRIPT_PARAMETER,acParameter) == 0)
						{
							//easyportal script
							strcpy(acMain_ScriptParameter_File,acData);
						}
						else
						if(strcasecmp(EASYPORTAL_SCRIPT_PARAMETER,acParameter) == 0)
						{
							//easyportal script
							strcpy(program_config->easyportal_script_parameter,acData);
						}
						else
						if(strcasecmp(SETUP_CONF,acParameter) == 0)
						{
							//Setup Config Datei
							strcpy(program_config->setup_conf,acData);
						}
						else
						if(strcasecmp(LIST_OF_PROGRAMS,acParameter) == 0)
						{
							//Liste mit allen möglichen Funktionen 
							strcpy(acList_of_programs_File,acData);
						}
					}
				}		
			}
		}
		fclose(pFile);
	}

	//Button Parameter Laden
	ReadProgramList(acList_of_programs_File);
	
	if(iDEBUG==TRUE)
		DebugPrintConfig();	
	
return TRUE;
}
