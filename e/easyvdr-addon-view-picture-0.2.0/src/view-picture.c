/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Version 0.2 28.01.13
 * view-picture.c
 * Copyright (C) Peter Reyinger 2013 <easyvdr@reyinger.com>
 * 
 * view-picture is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * view-picture is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <syslog.h>

int iScreenWidth,iScreenHeight;
GtkWidget   *gtkMainWindow;
GtkWidget   *image,*fixedbox;


int iDEBUG=FALSE;
int bExit=TRUE; 

char acProgname[40];


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

//*************************************************************
char acInfo[200];
void error(char* acError,char *acSyslogError)
{
GtkWidget *dialog;

	
//Fehler ins Syslog schreiben
setlogmask (LOG_UPTO (LOG_NOTICE));

openlog (acProgname, LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
syslog (LOG_ERR,"%s",acSyslogError);
closelog ();
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(gtkMainWindow),
				       GTK_DIALOG_MODAL,
				       GTK_MESSAGE_ERROR,
				       GTK_BUTTONS_OK,
				       "%s",acError);				       
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

//Infos ins Syslog schreiben
void info_log(char* acInfo)
{
if(iDEBUG==TRUE)
{
	openlog (acProgname, LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE,"%s",acInfo);
	closelog ();
}
}
//*************************************************************

gboolean callback_key_press(GtkWidget *widget, GdkEventKey *pKey, gpointer user_data)
{

	if (pKey->type == GDK_KEY_PRESS)
	{
		if(bExit==TRUE)  
			bExit=FALSE;
		else
			gtk_main_quit();
	}

return FALSE; 
}


static gboolean time_handler_show_window_xs(GtkWidget *widget)
{
if(bExit==TRUE)
	gtk_main_quit();
return FALSE;
}


//*********** Bilder laden und skalieren
//iUnrefPixpuf == True Buffer löschen und Image erzeugen

int load_image_gtk(char *acFilename,GtkWidget **gdkImage,int iwidth,int iheight,int iUnrefPixpuf,GdkPixbuf **gdkHD_PixbufDesty)
{
GError *gerror=NULL;
GdkPixbuf *gdkHD_Pixbuf=NULL; 
FILE *pFile=NULL;
char acString[300];

pFile=NULL;
//Datei prüfen
pFile=fopen(acFilename, "r");
if(pFile!=NULL)
{
	fclose(pFile);
}
else
{
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

//Bild skalieren
*gdkHD_PixbufDesty=gdk_pixbuf_scale_simple(gdkHD_Pixbuf,iwidth,iheight,GDK_INTERP_BILINEAR);
if(*gdkHD_PixbufDesty==NULL)
{
	error("load_image_gtk: Error gdk_pixbuf_scale_simple",
	      "load_image_gtk: Error gdk_pixbuf_scale_simple");
	return(FALSE);
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

//******************************************
GtkWidget* create_window (char *acProgName)
{
	GtkWidget *window;
    GdkColor color;
    
	window = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_title (GTK_WINDOW (window), acProgName);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_decorated( (GTK_WINDOW(window)),FALSE);
	gtk_window_set_keep_above(GTK_WINDOW(window),TRUE );
	/* Exit when the window is closed */
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	return window;
}


int
main (int argc, char *argv[])
{
char acString[200];
char acString2[200];
sprintf(acProgname,"view-picture");

GdkScreen* screen = NULL;	
GdkDisplay *display;
GdkMonitor *primary;
GdkMonitor *monitor; 

GdkRectangle rectangle;
GdkRectangle* prec = &rectangle;

	
openlog (acProgname, LOG_CONS | LOG_NDELAY, LOG_LOCAL1);	
syslog (LOG_NOTICE,"start up");	
closelog ();
		
//GTK Initialisieren		
gtk_init (&argc, &argv);

	
//Haupt Fenster
gtkMainWindow = create_window (acProgname);

//Bildschirmauflösung  vom primären Bildschirm feststellen
screen = gtk_window_get_screen(GTK_WINDOW(gtkMainWindow));
display = gdk_screen_get_display (screen);
primary = gdk_display_get_primary_monitor (display);

if (primary)
	
	monitor = gdk_display_get_monitor (display, get_monitor_num (primary));

else
	monitor = 0;
 
gdk_monitor_get_geometry (monitor, prec);


//iScreenWidth = gdk_screen_get_width(screen);
//iScreenHeight = gdk_screen_get_height(screen);

iScreenWidth = prec->width;
iScreenHeight = prec->height;
//sprintf(acInfo,"Screen Resolution: Width: %d Height %d",iScreenWidth,iScreenHeight);
//info_log(acInfo);

	
//************** Parameter Testen ************
	if(argc>3)
	{
		error("Zu viele Parameter \n 2 Parameter erwartet: \n  \"Bildname\"  \"Aufblendzeit in s\" \nBeispiel: /home/nobody/test.jpg 4",
		      "2 Parameter expected: \"picturename\" \"Splash Screen time\" \n example: /home/nobody/test.jpg  4");
		exit(EXIT_FAILURE);
	}
	else
	if(argc<3)
	{
		error("2 Parameter erwartet: \n\"Bildname\"  \"Aufblendzeit in s\" \nBeispiel: /home/nobody/test.jpg 4",
		      "2 Parameter expected: \"picturename\" \"Splash Screen time\" \n example: /home/nobody/test.jpg  4");
		exit(EXIT_FAILURE);
	}

	if(strlen(argv[1])<4)
	{
		sprintf(acString,"1 Parameter falsch: %s \nBildname erwartet",argv[1]);
		sprintf(acString2,"1 Parameter wrong: %s \nno Picturename",argv[1]);
		error(acString,acString2);

	}

	if( atoi(argv[2])==0 || atoi(argv[2])>3600)
	{
		sprintf(acString,"2 Parameter falsch: %s \nAufblendzeit in s erwartet (1s - 3600s)",argv[2]);
		sprintf(acString2,"2 Parameter wrong: %s \nSplash Screen time no valid time (1s - 3600s)",argv[2]);
		error(acString,acString2);
	}
	   

	
	//Neue Box erstellen
	fixedbox=gtk_fixed_new();  
	
//*********** Testen ob die Dateien vorhanden sind **********  
	GdkPixbuf *gdkHD_PixbufDesty;
	//Hintergrund Bild	
	if( (load_image_gtk(argv[1],&image,iScreenWidth,iScreenHeight,TRUE,&gdkHD_PixbufDesty)) ==TRUE)
	{
		gtk_fixed_put(GTK_FIXED(fixedbox), image,0,0);
	}
	else
		exit(EXIT_FAILURE);


//Box zum Fenster hinzufügen
	gtk_container_add(GTK_CONTAINER(gtkMainWindow),fixedbox);


//Fenster Größe auf maximalen Bildschirm setzen (muss vor dem ersten show aufruf sein)
	gtk_window_set_default_size(GTK_WINDOW(gtkMainWindow), iScreenWidth, iScreenHeight);	

	
	gtk_widget_show_all(gtkMainWindow);
	
//Fenster anzeigen
	//1000 1 sec
	g_timeout_add((atoi(argv[2])*1000), (GSourceFunc) time_handler_show_window_xs, (gpointer) gtkMainWindow);

	
	gtk_main ();


openlog (acProgname, LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
syslog (LOG_NOTICE,"exit");
closelog ();	
	
return 0;
}
