void show_window (bool exec_script, bool show_please_wait);
void hide_window (bool exec_script);
void select_menu_page(int iPage);
void Menu_Page_Up();
void Menu_Page_Down();
void show_please_wait();
void show_menu_page(int iPageNew);
bool switch_window_state();
int search_button_index_From_Page(int iButton_Nr);

extern int iScreenWidth,iScreenHeight;
extern int iDEBUG;
extern pthread_t thread;
extern bool PchangerKey;


struct stgtkWindow {
 GtkWidget   *Main;
 GtkWidget   *Event;
} ;

extern struct stgtkWindow gtkWindow;