#define PLINE_MAX					220
#define PFAD_MAX_SETUPPRG			PLINE_MAX+1
#define BUTTON_MAX_SETUPPRG			200
#define	NAME_BUTTON_MAX_SETUPPRG	70
#define SIZE_REMOTE_NAME			100
#define SIZE_MAX_YES_NO				10

#define NO_ERROR						0
#define CREATE_CFG_FILE_ERROR			12
#define PARAMETER_ERROR					13
#define ERROR_READ_PCHANGER_CONF_FILE	14
#define ERROR_READ_PCHANGER_SH_FILE		15
#define CREATE_KEYBOARD_CONF_FILE_ERROR 18
#define ERROR_READ_PROGRAM_DB			19
#define CREATE_SCRIPT_FILE_ERROR 		20

#define MENU_PAGE_STANDARD_PROGRAM		1
#define MENU_PAGE_INTERNET_BOOKMARKS	10
#define MENU_PAGE_SYSTEM_PROGRAM		20
#define MENU_PAGE_SYSTEMOPTIONS			30



struct stPConf {
 char acVersion[30];	//10 Byte für File Version		
 char lirc_config[PFAD_MAX_SETUPPRG];
 char main_script[PFAD_MAX_SETUPPRG];
 char easyportal_script_parameter[PFAD_MAX_SETUPPRG];
 char setup_conf[PFAD_MAX_SETUPPRG];
 char keyboard_config[PFAD_MAX_SETUPPRG];
 char css_config[PFAD_MAX_SETUPPRG];
	
 char default_icon[PFAD_MAX_SETUPPRG];
	
 char name_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char picture_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char enable_button[BUTTON_MAX_SETUPPRG][SIZE_MAX_YES_NO];
 char enable_button_easyportal[BUTTON_MAX_SETUPPRG][SIZE_MAX_YES_NO];
 char mouse_emulation_button[BUTTON_MAX_SETUPPRG][SIZE_MAX_YES_NO];
 char adjust_alsamixer_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char shutdown_program_button[BUTTON_MAX_SETUPPRG][SIZE_MAX_YES_NO];
 char pre_start_program_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char start_program_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];	
 char start_program_button_easyportal[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];	
 char stop_program_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char kill_program_button[BUTTON_MAX_SETUPPRG][SIZE_MAX_YES_NO];
 char root_rights_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char vdr_remote_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];
 char install_apt_get_prg_button[BUTTON_MAX_SETUPPRG][PFAD_MAX_SETUPPRG];	
 char menu_page_nr_button[BUTTON_MAX_SETUPPRG][SIZE_MAX_YES_NO];
 
 	
 int  iButtons_at_Page[BUTTON_MAX_SETUPPRG];
 int  iIndex__Selected_Button;

 int  iReserve;	//Diese Variable wird nicht verwendet
 int  iKey_Pchanger;			//Pchager Taste vom Keyboard
 int  iKey_red;
 int  iKey_green;
 int  iKey_yellow;
 int  iKey_blue;
 int  iKey_menu;
 int  iKey_help;
} ; 
