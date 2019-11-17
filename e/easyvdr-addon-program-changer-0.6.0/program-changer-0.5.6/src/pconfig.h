#define PFAD_MAX				200
#define BUTTON_MAX				150
//0 wird nicht verwendet 
#define PAGE_MAX				40


//Standardmenü maximal Seite 1-9
#define PAGE_PROGRAMS			1	
//System Programme maximal Seite 10-19
#define PAGE_BOOKMARK			10
//System Programme maximal Seite 20-29
#define PAGE_SYSTEMPROGRAMS		20
#define PAGE_SYSTEMOPTIONS		30


#define	NAME_BUTTON_MAX			70
#define BUTTON_BASH_ARRAY_MAX	BUTTON_MAX
#define PLINE_MAX	220

#define FULL_HD 		1920
#define FULL_HD_HEIGHT 	1080
//#define HD_READY 	1280
//#define	PAL		720

struct stProgramConfig {
 char lirc_config[PFAD_MAX];
 char main_script[PFAD_MAX];
 char switch_media_center_script[PFAD_MAX];
 char setup_cfg[PFAD_MAX];
 char keyboard_config[PFAD_MAX];
 char css_config[PFAD_MAX];
 int  backround_picture_yes;
 int  iButtonHoldTime;
 int  iButtonReleaseTime;
 int  iXSpeed;
 int  iYSpeed;	
 char backround_picture[PFAD_MAX];
 char select_page_picture_button[4][PFAD_MAX];
 char please_wait[PFAD_MAX];
 char title_picture[PFAD_MAX];
 char help_picture[PFAD_MAX];
 char show_hide_menu[PFAD_MAX];
 char default_icon[PFAD_MAX];
 char picture_number[BUTTON_MAX][PFAD_MAX];
 char name_button[BUTTON_MAX][NAME_BUTTON_MAX];
 char picture_button[BUTTON_MAX][PFAD_MAX];
 int  imenu_page_nr_button[BUTTON_MAX];
 int  adjust_alsamixer_Button[BUTTON_MAX];
 int  array_index_button[BUTTON_MAX];
 int  imouse[BUTTON_MAX];	
 int  iIndex__Selected_Button;
 int  iKey_Pchanger;			//Pchager Taste vom Keyboard
 int  iKey_red;
 int  iKey_green;
 int  iKey_yellow;
 int  iKey_blue;
 int  iKey_menu;
 int  iKey_help;
} program_config; 

struct stFileComands {
char Show_Pchanger[PFAD_MAX];
char Exec[PFAD_MAX];
} FileComands;


void find_config_parameter(char *acParameter,char *acData);
void find_hd_config_parameter(char *acParameter,char *acData);
void find_hd_ready_config_parameter(char *acParameter,char *acData);
void find_pal_config_parameter(char *acParameter,char *acData);
int readconfig(char *acFilename);
