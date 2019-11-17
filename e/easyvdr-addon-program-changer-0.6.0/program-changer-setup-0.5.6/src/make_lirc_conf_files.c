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
#include "program-changer-setup.h"
#include "parameter.h"
#include "message_and_error_log.h"



//Tastatur Belegung
#define	KEY_PCHANGER				"key-show-hide-menu"
#define KEY_RED						"key-red"
#define KEY_GREEN					"key-green"
#define KEY_YELLOW					"key-yellow"
#define KEY_BLUE					"key-blue"
#define KEY_MENU					"key-menu"




int Key_Pchanger=0;
int Key_Red=0;
int Key_Green=0;
int Key_Yellow=0;
int Key_Blue=0;
int Key_Menu=0;

bool bcheck_keypchanger=FALSE,bcheck_Key_Red=FALSE,bcheck_Key_Green=FALSE,bcheck_Key_Yellow=FALSE,bcheck_Key_Blue=FALSE;
bool bcheck_Key_Menu=FALSE;

static gboolean time_handler_learn_keys(gpointer data)
{

bool bMessage=FALSE;

	if(Key_Pchanger!=0 && bcheck_keypchanger==TRUE) 
	{
		bMessage=TRUE;
		bcheck_keypchanger=FALSE;
		gtk_widget_destroy(Info_dialog);
	}
	else
	if(Key_Red!=0 && bcheck_Key_Red==TRUE) 
	{
		bMessage=TRUE;
		bcheck_Key_Red=FALSE;
		gtk_widget_destroy(Info_dialog);
	}
	else
	if(Key_Green!=0 && bcheck_Key_Green==TRUE) 
	{
		bMessage=TRUE;
		bcheck_Key_Green=FALSE;
		gtk_widget_destroy(Info_dialog);
	}
	else
	if(Key_Yellow!=0 && bcheck_Key_Yellow==TRUE) 
	{
		bMessage=TRUE;
		bcheck_Key_Yellow=FALSE;
		gtk_widget_destroy(Info_dialog);
	}
	else
	if(Key_Blue!=0 && bcheck_Key_Blue==TRUE) 
	{
		bMessage=TRUE;
		bcheck_Key_Blue=FALSE;
		gtk_widget_destroy(Info_dialog);		
	}
	else
	if(Key_Menu!=0 && bcheck_Key_Menu==TRUE) 
	{
		bMessage=TRUE;
		bcheck_Key_Menu=FALSE;
		gtk_widget_destroy(Info_dialog);		
	}

	if(bMessage==TRUE)
		make_info_dialog_box("Taste zum aktivieren wurde gespeichert.",TRUE);
	return TRUE;
}


//************************
//Timer Starten für Tasten Prüfung
void learn_keys()
{
	//1000 1 sec
	g_timeout_add(100, (GSourceFunc) time_handler_learn_keys, (gpointer) gtkMainWindow);

}



void print_info_keyboard_parameter(FILE *fpFile,char *acProgramName)
{
	fprintf(fpFile,"# program-changer-keybord.conf Version 0.7 04.11.2012 by Bleifuss2\n\n");
	fprintf(fpFile,"\n\n");
}


//*******************************************************************
int make_keyboard_conf(char *acProgramName)
{
FILE *pFile=NULL;
char acString[PLINE_MAX+1];

//Timer für Tastaturanlernen starten
learn_keys();
		
info_log("Make program-changer-keyboard.conf ");
	

//***************** Pchanger Taste ermitteln *****************************

bcheck_keypchanger=TRUE;
	
make_info_dialog_box("Die gewünschte Taste für das aktivieren des Programchangers betätigen\n\
Wenn kein Eingabe erfolgt wird die Tastaur deaktiviert.\n\
Voreinstellung ist die Windows Taste\n \n",FALSE);

	
if(bcheck_keypchanger==FALSE && Key_Pchanger !=0)
{
	Key_Red=0;
	bcheck_Key_Red=TRUE;
	make_info_dialog_box("Bitte die Taste für Standardmenü (Rot) betätigen.",FALSE);
}
else
{
	bcheck_keypchanger=FALSE;
	Key_Pchanger=0;
	gtk_widget_destroy(Info_dialog);
	make_info_dialog_box("Abbruch, Taste wurde nicht erkannt \nVorgang Bitte wiederholen",TRUE);
	return TRUE;
}

if(bcheck_Key_Red==FALSE && Key_Red!=0)
{
	Key_Green=0;
	bcheck_Key_Green=TRUE;
	make_info_dialog_box("Bitte die Taste für Anwendermenü (Grün) betätigen.",FALSE);
}
else
{
	Key_Red=0;
	bcheck_Key_Red=FALSE;
	gtk_widget_destroy(Info_dialog);
	make_info_dialog_box("Abbruch, Taste wurde nicht erkannt \nVorgang Bitte wiederholen",TRUE);
	return TRUE;
}


if(bcheck_Key_Green==FALSE && Key_Green!=0)		
{
	Key_Yellow=0;
	bcheck_Key_Yellow=TRUE;
	make_info_dialog_box("Bitte die Taste für Systemprogramme (Gelb) betätigen.",FALSE);
}
else
{
	Key_Green=0;
	bcheck_Key_Green=FALSE;
	make_info_dialog_box("Abbruch, Taste wurde nicht erkannt \nVorgang Bitte wiederholen",TRUE);
	return TRUE;
}

if(bcheck_Key_Yellow==FALSE && Key_Yellow!=0)
{
	Key_Blue=0;
	bcheck_Key_Blue=TRUE;
	make_info_dialog_box("Bitte die Taste für Systemsteuerung (Blau) betätigen.",FALSE);
}
else
{
	Key_Yellow=0;
	bcheck_Key_Yellow=FALSE;
	make_info_dialog_box("Abbruch, Taste wurde nicht erkannt \nVorgang Bitte wiederholen",TRUE);
	return TRUE;
}


if(bcheck_Key_Blue==FALSE && Key_Blue!=0)
{
	Key_Menu=0;
	bcheck_Key_Menu=TRUE;
	make_info_dialog_box("Bitte die Taste für Menü (m) betätigen.",FALSE);
}
else
{
	Key_Blue=0;
	bcheck_Key_Blue=FALSE;
	make_info_dialog_box("Abbruch, Taste wurde nicht erkannt \nVorgang Bitte wiederholen",TRUE);
	return TRUE;
}


if(bcheck_Key_Menu!=FALSE || Key_Menu==0)
{
	Key_Menu=0;
	bcheck_Key_Menu=FALSE;
	make_info_dialog_box("Abbruch, Taste wurde nicht erkannt \nVorgang Bitte wiederholen",TRUE);
	return TRUE;
}

	
	pFile=fopen(program_config->keyboard_config, "w");
	if(pFile==NULL)
	{
		sprintf(acString,"\n make_keyboard_conf \n Datei kann nicht erzeugt werden: \n  %s  ",program_config->lirc_config);
		sprintf(acInfo,"make_keyboard_conf: File cannot be created : %s",program_config->lirc_config);
		error(acString,acInfo);
		return CREATE_KEYBOARD_CONF_FILE_ERROR;
	}
	else
	{
			//File Info und Pchanger Taste speichern
			print_info_keyboard_parameter(pFile,acProgramName);
			fprintf(pFile,"%s = %d\n",KEY_PCHANGER,Key_Pchanger);
			fprintf(pFile,"%s = %d\n",KEY_RED,Key_Red);
			fprintf(pFile,"%s = %d\n",KEY_GREEN,Key_Green);
			fprintf(pFile,"%s = %d\n",KEY_YELLOW,Key_Yellow);
			fprintf(pFile,"%s = %d\n",KEY_BLUE,Key_Blue);
			fprintf(pFile,"%s = %d\n",KEY_MENU,Key_Menu);
	
	fclose(pFile);
	}
return (TRUE);
}			

//****************************************************************************

