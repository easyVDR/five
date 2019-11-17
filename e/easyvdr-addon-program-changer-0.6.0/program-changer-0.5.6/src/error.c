#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdbool.h>
#include <gtk/gtk.h>
#include <syslog.h>
#include "program-changer.h"


char acInfo[200]; 
void error(char* acError,char *acSyslogError)
{
GtkWidget *dialog;


//Fehler ins Syslog schreiben
setlogmask (LOG_UPTO (LOG_NOTICE));

openlog ("Program-Changer", LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
syslog (LOG_ERR,"%s",acSyslogError);
closelog ();
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(gtkWindow.Main),
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
	openlog ("Program-Changer", LOG_CONS | LOG_NDELAY, LOG_LOCAL1);
	syslog (LOG_NOTICE,"%s",acInfo);
	closelog ();
}
}
