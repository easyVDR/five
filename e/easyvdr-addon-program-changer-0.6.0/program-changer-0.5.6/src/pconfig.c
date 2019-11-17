#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdbool.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pconfig.h"
#include "error.h"
#include "program-changer.h"

#define CSS_CONFIG				"css-config"
#define LIRC_CONFIG				"lirc-config"
#define SHOW_HIDE_MENU				"show-hide-menu"
#define MAIN_SCRIPT				"main-script"
#define SWITCH_MEDIA_CENTER_SCRIPT		"switch-media-center-script"
#define SETUP_CFG				"setup-cfg"
#define KEYBOARD_CONFIG				"keybord-config"
#define BUTTON_DEFAULT_ICON			"default-icon"
#define BACKROUND_PICTURE			"backround-picture"
#define BUTTON_HOLD_TIME			"button-hold-time"
#define BUTTON_RELEASE_TIME			"button-release-time"

#define TITEL_PICTURE				"titel-picture"
#define PLEASE_WAIT_PICTURE			"please-wait-picture"
#define HELP_PICTURE				"help-picture"
#define BACKROUND_PICTURE			"backround-picture"
#define NUMBER_PICTURE				"number-picture-"
#define ENABLE_BACKROUND_PICTURE		"enable-backround-picture"
#define STANDARD_MENU_PICTURE			"standard-menu-button-picture"
#define BOOKMARK_MENU_PICTURE			"bookmark-menu-button-picture"
#define SYSTEM_PROGRAM_MENUE			"system-program-menu-button-picture"
#define SYSTEM_OPTIONS_MENU_PICTURE		"system-options-menu-button-picture"


//####### Daten im Bash Skript
//Skript Array Daten
#define NAME_BUTTON				"name_button"
#define PICTURE_BUTTON				"picture_button"
#define ENABLE_BUTTON				"enable_button"
#define ENABLE_MOUSE_BUTTON			"enable_mouse_button"
#define TMP_FILE_SHOW_PCHANGER			"TMP_FILE_SHOW_PCHANGER"
#define TMP_FILE_PCH_EXEC			"TMP_FILE_PCH_EXEC"
#define MENUE_PAGE_NR_BUTTON			"menu_page_nr_button" 
#define ADJUST_ALSAMIXER_BUTTON			"adjust_alsamixer_Button" 
//####### 


#define MOUSE_SPEED_H				"mouse-speed-horizontal"
#define MOUSE_SPEED_V				"mouse-speed-vertical"


//Tastatur Belegung
#define KEY_PCHANGER					"key-show-hide-menu"
#define KEY_RED						"key-red"
#define KEY_GREEN					"key-green"
#define KEY_YELLOW					"key-yellow"
#define KEY_BLUE					"key-blue"
#define KEY_MENU					"key-menu"
#define KEY_HELP					"key-help"



void DebugPrintConfig(void)
{
if(iDEBUG!=TRUE)
	return;
		
int i;
info_log("Config Parameter");   
sprintf(acInfo,"Css Config File: %s",program_config.css_config);
info_log(acInfo);	
sprintf(acInfo,"Lirc Config File: %s",program_config.lirc_config);
info_log(acInfo);	
sprintf(acInfo,"Script program-changer.sh: %s",program_config.main_script);
info_log(acInfo);
sprintf(acInfo,"Script switch-media-center: %s",program_config.switch_media_center_script);
info_log(acInfo);
sprintf(acInfo,"Script Show_hide_menu: %s",program_config.show_hide_menu);
info_log(acInfo);
sprintf(acInfo,"Title: %s",program_config.title_picture);
info_log(acInfo);
sprintf(acInfo,"Default icon: %s",program_config.default_icon);
info_log(acInfo);
sprintf(acInfo,"Backroundpicture: %s",program_config.backround_picture);	
info_log(acInfo);
sprintf(acInfo,"Temp File Show-Pchanger: %s",	FileComands.Show_Pchanger);	
info_log(acInfo);

sprintf(acInfo,"Temp File PCH Exec: %s",	FileComands.Exec);
info_log(acInfo);

for(i=0;i<10;i++)
{
	sprintf(acInfo,"Picture Number: %d File Name: %s",i,program_config.picture_number[i]);
	info_log(acInfo);
}
		
for(i=0;i<BUTTON_MAX;i++)
{
	sprintf(acInfo,"Button  %d: %s",i,program_config.name_button[i]);
	info_log(acInfo);
	sprintf(acInfo,"Button  %d: %s",i,program_config.picture_button[i]);	
	info_log(acInfo);
	sprintf(acInfo,"Bash Index: %d",program_config.array_index_button[i]);
	info_log(acInfo);
	sprintf(acInfo,"Page number: %d",program_config.imenu_page_nr_button[i]);
	info_log(acInfo);
	sprintf(acInfo,"Alsa Volume change: %d",program_config.adjust_alsamixer_Button[i]);
	info_log(acInfo);
	sprintf(acInfo,"Mouse: %d",program_config.imouse[i]);
	info_log(acInfo);
}	

sprintf(acInfo,"Show Backround Picture: %d",program_config.backround_picture_yes);
info_log(acInfo);
sprintf(acInfo,"Time for change speed fast: %d",program_config.iButtonHoldTime);
info_log(acInfo);
sprintf(acInfo,"Time for fast off: %d",(program_config.iButtonReleaseTime-program_config.iButtonHoldTime));
info_log(acInfo);
sprintf(acInfo,"Mouse X Faktor: %d",program_config.iXSpeed);
info_log(acInfo);
sprintf(acInfo,"Mouse Y Faktor: %d",program_config.iYSpeed);
info_log(acInfo);
sprintf(acInfo,"Pchanger Keyborad Keys");
info_log(acInfo);
sprintf(acInfo,"Pchanger Key: %d",program_config.iKey_Pchanger);
info_log(acInfo);
sprintf(acInfo,"Key Red: %d",program_config.iKey_red);
info_log(acInfo);
sprintf(acInfo,"Key Green: %d",program_config.iKey_green);
info_log(acInfo);
sprintf(acInfo,"Key Yellow: %d",program_config.iKey_yellow);
info_log(acInfo);
sprintf(acInfo,"Key Blue: %d",program_config.iKey_blue);
info_log(acInfo);
info_log("End Config");
}


#define PFAD_MAX_SETUPPRG			200
#define BUTTON_MAX_SETUPPRG			70
#define	NAME_BUTTON_MAX_SETUPPRG	100



int search_parameter(char *acString,char *acParameter,char *acData)
{
int i;

	//Testen ob Text vohanden ist
	if(strstr(acString,acParameter) != 0)
	{
		//Parameter suchen  
		for(i=0;i<strlen(acString)-1;i++)
		{
			if(acString[i]=='=')
			{
				sscanf(acString+i+1,"%s",acData);
				return (TRUE);
			}
		}
	}
return (FALSE);
}

int search_bash_array_index(char *acString,char *acParameterName,int iArray_Max,char *acData)
{
int i;
char acParameter[100];
	
	//Testen ob Kommentar Zeile
			if(memchr(acString,'#',PLINE_MAX)==NULL)
			{

				for(i=0 ;i<iArray_Max;i++)
				{
					sprintf(acParameter,"%s[%d]",acParameterName,i);
					
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{	
						//Index Nummer vom Bash Array
						return(i);
					}
				}
			}
				
return(-1);
}


int readconfig(char *acFilename)
{
	FILE *pFile=NULL;
	char acString[PLINE_MAX+1];

	char acParameter[100];
	char acEq[2];
	char acData[PFAD_MAX];
	int i,iBashArrayIndex,iFound=0;

	
	//Puffer löschen
	memset(&program_config,0,sizeof(program_config));

	pFile=fopen(acFilename, "r");
	if(pFile==NULL)
	{
		sprintf(acInfo,"Readconfig \n Keine Berechtigung oder  \n  Datei nicht gefunden: \n  %s  ",acFilename);
		sprintf(acString,"Readconfig: File not found or Permission error: %s",acFilename);
		error(acInfo,acString);
		return FALSE;
	}
	else
	{
		while(feof(pFile)==0)
		{
			//Puffer löschen
			memset(acString,0,PLINE_MAX);
			//Eine Zeile lesen
			if(fgets(acString,PLINE_MAX,pFile)!=NULL)
				//Testen ob Komentar Zeile
				if(memchr(acString,'#',PLINE_MAX)==NULL)
				{
					if(sscanf(acString,"%s %c %s",acParameter,acEq,acData)==3)
					{
						//Allgemeine Daten Laden
						find_config_parameter(acParameter,acData);
					}
				}		
		}
			fclose(pFile);
	}

	
//Button Parameter von Skript auslesen
	//Feststellen welche Buttons angezeigt werden sollen
	if(program_config.main_script==NULL)
	{
		
		sprintf(acString,"Readconfig: File not found or Permission error: %s",program_config.main_script);
		error("Fehler in conf Datei\n main-skript = Kein Skriptname"
		      ,"Error in conf File, main-skript = no Scriptname");

	}

	pFile=fopen(program_config.main_script, "r");
	if(pFile==NULL)
	{
		sprintf(acInfo,"Readconfig \n Keine Berechtigung oder  \n  Datei nicht gefunden: \n  %s  ",program_config.main_script);
		sprintf(acString,"Readconfig: File not found or Permission error: %s",program_config.main_script);
		error(acInfo,acString);
		return FALSE;
	}
	else
	{	
		//Array auf illegalen wert setzen
		for(i=0;i<BUTTON_MAX;i++)
			program_config.array_index_button[i]=-1;
		
		i=0;
		while(feof(pFile)==0 && strstr(acString,"function ")==NULL)
		{
			//Puffer löschen
			memset(acString,0,PLINE_MAX);
			
			//Eine Zeile lesen
			if(fgets(acString,PLINE_MAX,pFile)!=NULL)
				//Testen ob Komentar Zeile oder bash funktion
				if(memchr(acString,'#',PLINE_MAX)==NULL)
				{
					//TMP Dateiname suchen
					memset(acData,0,sizeof(acData));

					sprintf(acParameter,"%s",TMP_FILE_SHOW_PCHANGER);
						
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{
		
						sprintf(FileComands.Show_Pchanger,"%s",acData);
					}

					//TMP Exec Filename suchen
					memset(acData,0,sizeof(acData));

					sprintf(acParameter,"%s",TMP_FILE_PCH_EXEC);
						
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{
		
						sprintf(FileComands.Exec,"%s",acData);
					}

					if(search_parameter(acString,ENABLE_BUTTON,acData)==TRUE)
					{	   
						memset(acData,0,sizeof(acData));
						iBashArrayIndex=search_bash_array_index(acString,ENABLE_BUTTON,BUTTON_BASH_ARRAY_MAX,acData);
						if(iBashArrayIndex!=-1 || iBashArrayIndex >=BUTTON_MAX_SETUPPRG)
						{
							if( strstr(acData,"yes") || strstr(acData,"Yes") || strstr(acData,"YES"))
							{   //Button gefunden
								program_config.array_index_button[i]=iBashArrayIndex;
								//Menü Seite des Pchanger
								//program_config.imenu_page_nr_button[i]=setup_program_config->menu_page_nr_button[iBashArrayIndex];
								i++;
									if(i==BUTTON_MAX)
									break;
							}
						}
					}
				}
		}
		
	fclose(pFile);
	}		

//Bilder / Mausfunktion einlesen
	pFile=fopen(program_config.main_script, "r");
	
	while(feof(pFile)==0)
	{
		//Puffer löschen
		memset(acString,0,PLINE_MAX);
		memset(acData,0,sizeof(acData));
		
		//Eine Zeile lesen
		if(fgets(acString,PLINE_MAX,pFile)!=NULL)
		{
			//Testen ob Kommentar Zeile
			if(memchr(acString,'#',PLINE_MAX)==NULL)
			for(i=0;i<BUTTON_MAX;i++)
			{
				//Testen ob ein Array Index gefunden wurde
				if(program_config.array_index_button[i]!=-1)
				{

					//Programm/Button Namen
					sprintf(acParameter,"%s[%d]",NAME_BUTTON,program_config.array_index_button[i]);
					
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{   
						if(strlen(acParameter)>=NAME_BUTTON_MAX-1)
						{   //Name zu lang
							sprintf(program_config.name_button[i],"Name to long");
						}
						else
						{
							//Button Name gefunden
							sprintf(program_config.name_button[i],"%s",acData);	
							iFound=1;
						}
					}
		
					//Bild Namen & Pfad suchen
					sprintf(acParameter,"%s[%d]",PICTURE_BUTTON,program_config.array_index_button[i]);
		
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{   
						//Button gefunden
						sprintf(program_config.picture_button[i],"%s",acData);	
						iFound++;
					}

					//Maus ein/aus suchen
					memset(acData,0,sizeof(acData));

					sprintf(acParameter,"%s[%d]",ENABLE_MOUSE_BUTTON,program_config.array_index_button[i]);
						
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{
						iFound++;
						if( strstr(acData,"yes") || strstr(acData,"Yes") || strstr(acData,"YES") )
						{   //Button gefunden
							program_config.imouse[i]=TRUE;	
						}
					}
					//Menü Seiten Nummer suchen
					memset(acData,0,sizeof(acData));
					sprintf(acParameter,"%s[%d]",MENUE_PAGE_NR_BUTTON,program_config.array_index_button[i]);
						
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{
						program_config.imenu_page_nr_button[i]=(atoi(acData));
						iFound++;
					}
				    //Lautstärke ändern ja nein suchen
					
					memset(acData,0,sizeof(acData));

					sprintf(acParameter,"%s[%d]",ADJUST_ALSAMIXER_BUTTON,program_config.array_index_button[i]);
						
					if(search_parameter(acString,acParameter,acData)==TRUE)
					{
						if( strstr(acData,"yes") || strstr(acData,"Yes") || strstr(acData,"YES") )
						{   //Button gefunden
							program_config.adjust_alsamixer_Button[i]=TRUE;	
						}
						iFound++;
					}
					//Parameter gefunden ?
					if(iFound==5)
					{
						break;
					}
					iFound=0;
				}
			}
		}
	}
	fclose(pFile);

//Tastenbelegung einlesen
	pFile=fopen(program_config.keyboard_config, "r");
	if(pFile==NULL)
	{
		sprintf(acInfo,"Readconfig \n Keine Berechtigung oder  \n  Datei nicht gefunden: \n  %s  ",program_config.keyboard_config);
		sprintf(acString,"Readconfig: File not found or Permission error: %s",program_config.keyboard_config);
		error(acInfo,acString);
		return FALSE;
	}
	else
	while(feof(pFile)==0)
	{
		//Puffer löschen
		memset(acString,0,PLINE_MAX);
		memset(acData,0,sizeof(acData));
		
		//Eine Zeile lesen
		if(fgets(acString,PLINE_MAX,pFile)!=NULL)
		{
			//Testen ob Kommentar Zeile
			if(memchr(acString,'#',PLINE_MAX)==NULL)
			{
				//Pchanger Taste?
				if(search_parameter(acString,KEY_PCHANGER,acData)==TRUE)
					program_config.iKey_Pchanger=atoi(acData);   
				else
				if(search_parameter(acString,KEY_RED,acData)==TRUE)
					program_config.iKey_red=atoi(acData);   
				else
				if(search_parameter(acString,KEY_GREEN,acData)==TRUE)
					program_config.iKey_green=atoi(acData);   
				else
				if(search_parameter(acString,KEY_YELLOW,acData)==TRUE)
					program_config.iKey_yellow=atoi(acData);   
				else
				if(search_parameter(acString,KEY_BLUE,acData)==TRUE)
					program_config.iKey_blue=atoi(acData);   
				else
				if(search_parameter(acString,KEY_MENU,acData)==TRUE)
					program_config.iKey_menu=atoi(acData);   
				else
				if(search_parameter(acString,KEY_HELP,acData)==TRUE)
					program_config.iKey_help=atoi(acData);   

			}
		}
	}
	fclose(pFile);
	
DebugPrintConfig();	
return TRUE;
}

//Allgemeine Daten Laden
void find_config_parameter(char *acParameter,char *acData)
{
int i;
char acString[100];
	if(strcasecmp("DEBUG",acParameter) == 0)
	{
 			iDEBUG=atoi(acData);
	}
	else
	if(strcasecmp(CSS_CONFIG,acParameter) == 0)
	{
		//css Config Datei
		strcpy(program_config.css_config,acData);
	}
	if(strcasecmp(LIRC_CONFIG,acParameter) == 0)
	{
		//Lirc Config Datei
		strcpy(program_config.lirc_config,acData);
	}
	else
	if(strcasecmp(MAIN_SCRIPT,acParameter) == 0)
	{
		//Main Skript Datei
		strcpy(program_config.main_script,acData);
	}
	else
	if(strcasecmp(SWITCH_MEDIA_CENTER_SCRIPT,acParameter) == 0)
	{
		//Keyborad config Datei
		strcpy(program_config.switch_media_center_script,acData);
	}
	else
	if(strcasecmp(KEYBOARD_CONFIG,acParameter) == 0)
	{
		//Keyborad config Datei
		strcpy(program_config.keyboard_config,acData);
	}
	else
	if(strcasecmp(BUTTON_DEFAULT_ICON,acParameter) == 0)
	{
		//Keyborad config Datei
		strcpy(program_config.default_icon,acData);
	}
	else
	if(strcasecmp(SETUP_CFG,acParameter) == 0)
	{
		//Setup.cfg Datei
		strcpy(program_config.setup_cfg,acData);
	}
	else		
	if(strcasecmp(SHOW_HIDE_MENU,acParameter) == 0)
	{
		//Show Hide Skript
		strcpy(program_config.show_hide_menu,acData);
	}
	else
	if(strcasecmp(BACKROUND_PICTURE,acParameter) == 0)
	{
		//Hintergrund Bild
		strcpy(program_config.backround_picture,acData);
	}
	else
	if(strcasecmp(TITEL_PICTURE,acParameter) == 0)
	{
		//Überschrift
 		strcpy(program_config.title_picture,acData);
	}
	else
	if(strcasecmp(ENABLE_BACKROUND_PICTURE,acParameter) == 0)
	{
		if(strcasecmp(acData,"yes") == 0)
		{
			//Hintergrund Bild Ja
 			program_config.backround_picture_yes=TRUE;
		}
		else
		program_config.backround_picture_yes=FALSE;
	}
	else
	if(strcasecmp(HELP_PICTURE,acParameter) == 0)
	{
		//Hilfe Button
 		strcpy(program_config.help_picture,acData);
	}
	else
	if(strcasecmp(PLEASE_WAIT_PICTURE,acParameter) == 0)
	{
		//Bitte Warten Button
 		strcpy(program_config.please_wait,acData);
	}
	else
	if(strcasecmp(STANDARD_MENU_PICTURE,acParameter) == 0)
	{
		//Bitte Warten Button
 		strcpy(program_config.select_page_picture_button[0],acData);
	}
	else
	if(strcasecmp(BOOKMARK_MENU_PICTURE,acParameter) == 0)
	{
		//Bitte Warten Button
 		strcpy(program_config.select_page_picture_button[1],acData);
	}
	else
	if(strcasecmp(SYSTEM_PROGRAM_MENUE,acParameter) == 0)
	{
		//Bitte Warten Button
 		strcpy(program_config.select_page_picture_button[2],acData);
	}
	else
	if(strcasecmp(SYSTEM_OPTIONS_MENU_PICTURE,acParameter) == 0)
	{
		//Bitte Warten Button
 		strcpy(program_config.select_page_picture_button[3],acData);
	}
	else
	if(strcasecmp(BUTTON_HOLD_TIME,acParameter) == 0)
	{
 			program_config.iButtonHoldTime=atoi(acData);
			if(program_config.iButtonHoldTime<=10)
				program_config.iButtonHoldTime=300;
	}
	else
	if(strcasecmp(BUTTON_RELEASE_TIME,acParameter) == 0)
	{
 			program_config.iButtonReleaseTime=atoi(acData);
			
			if(program_config.iButtonReleaseTime<=program_config.iButtonHoldTime)
				program_config.iButtonReleaseTime=program_config.iButtonHoldTime+200;
	}
	else
	//Maus Speed
	if(strcasecmp(MOUSE_SPEED_H,acParameter) == 0)
	{
		program_config.iXSpeed=atoi(acData);
                program_config.iXSpeed=(int)(((float)iScreenWidth/(float)FULL_HD)*(float)program_config.iXSpeed);
 	}	
	else
	if(strcasecmp(MOUSE_SPEED_V,acParameter) == 0)
	{
		program_config.iYSpeed=atoi(acData);
		program_config.iYSpeed=(int)(((float)iScreenHeight/FULL_HD_HEIGHT)*(float)program_config.iYSpeed) ;
 	}	
	else
	for(i=0;i<10;i++)
	{
		sprintf(acString,"%s%d",NUMBER_PICTURE,i);
		if(strcasecmp(acString,acParameter) == 0)
		{
			//Zahl
	 		strcpy(program_config.picture_number[i],acData);
			break;
		}
	}
		
}

