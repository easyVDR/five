#include <gtk/gtk.h>
#include <lirc/lirc_client.h>

bool init_lirc(GtkWidget *Window,char *acProgname,char *acLircConfigFile);
bool start_lirc_callback(GtkWidget *Window);
void deinit_lirc(struct lirc_config *config);
void check_lirc_command(char *config_parameter);
//Timer Starten damit Fenster am Anfang 5s eingeblendet wird
//Wenn keine Button Nummer übergeben wird ist in acCommand der fertige Befehl
void exec_command_and_show_please_wait(int iButton_Nr, char *acCommand);
