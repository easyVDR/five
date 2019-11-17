extern char acInfo[];
int make_yes_no_dialog_box(char *acMessage);
void error(char* acError,char *acSyslogError);
void info_log(char* acInfo);
void make_info_dialog_box(gchar *acMessage,bool bDestroy);
extern pthread_t thread;
extern GtkWidget *Info_dialog;