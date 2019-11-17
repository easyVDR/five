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
#include "program-changer-setup.h"

char acInfo[200];
GtkWidget *Info_dialog;

//Dialog Box 
int make_yes_no_dialog_box(char *acMessage)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(NULL,
				       GTK_DIALOG_MODAL,
				       GTK_MESSAGE_INFO,
				       GTK_BUTTONS_NONE,
				       "%s",acMessage);				       

	gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                      "Ja", 100,
                      "Nein", 101,
                       NULL);

	gint result=gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	switch (result)
	{
    case 100:
		return (TRUE);
        break;
    case 101:
  		return (FALSE);
        break;
	}
return FALSE;
}

 
void error(char* acError,char *acSyslogError)
{
GtkWidget *dialog;

//Fehler ins Syslog Schreiben
setlogmask (LOG_UPTO (LOG_NOTICE));

openlog ("Program-Changer Setup", LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
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


//Infos ins Syslog Schreiben
void info_log(char* acInfo)
{
if(iDEBUG==TRUE)
{
	openlog ("Program-Changer Setup", LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE,"%s",acInfo);
	closelog ();
}
}



//Dialog Box mit Text anzeigen
void make_info_dialog_box(gchar *acMessage, bool bDestroy)
{
	Info_dialog = gtk_message_dialog_new(NULL,
				       GTK_DIALOG_MODAL,
				       GTK_MESSAGE_INFO,
				       GTK_BUTTONS_OK,
				       "%s",acMessage);				       
    gtk_dialog_run(GTK_DIALOG(Info_dialog));
	if(bDestroy==TRUE)
		gtk_widget_destroy(Info_dialog);
}
