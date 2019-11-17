#!/bin/bash
#Program Changer none-X-Setup
#
#12.03.2016 gb
#01.04.2016 Update easyPortal default select
#05.04.2016 Update read Keyboard Keycode
#17.09.2016 Update Skin_Dialog
#04.01.2019 modify easyVDRV4
#V1.0 
#

##### includes #####
. /usr/share/easyvdr/program-changer/lib/setup-functions
##### definitionen #####
USER=$STANDARD_DESKTOP_USER
HOME_PATH=$(cat /etc/passwd | grep ^$USER: | awk -F: '{print $6}')
SETUP_DIR=/usr/share/easyvdr/setup						#easyVDR Setup Dir
PCH_CONF="/etc/vdr/easyvdr/program-changer/program-changer.conf"               	#Parameter 1: program-changer.conf
PCH_KEY_CONF="/etc/vdr/program-changer/program-changer-keyboard.conf"     	#Pfad für Tastatur Belegung
KODI_HOME="$HOME_PATH/.kodi"                                  	          	#Pfad für KODI Home Verzeichnis
KODI_HOME_USERDATA="$HOME_PATH/.kodi/userdata"                     	  	#Pfad für KODI Home/userdaten Verzeichnis

############################Menü####################################
Setup_Menu()
{
SELECT_MENU=$(dialog --backtitle "${TITLE[$LANG]}" --menu "${MENU_SELECTION[$LANG]}"  0 0 0 \
			"${SKIN_SELECTION[$LANG]}" "${SKIN_STATEMENT[$LANG]}" \
			"${REMOTE_SELECTION[$LANG]}" "${REMOTE_STATEMENT[$LANG]}" \
			"${REMOTE_KEYBOARD_SELECTION[$LANG]}" "${REMOTE_KEYBOARD_STATEMENT[$LANG]}" \
			"${BUTTON_SELECTION[$LANG]}" "${BUTTON_STATEMENT[$LANG]}" \
			"${BUTTON_PORTAL_SELECTION[$LANG]}" "${BUTTON_PORTAL_STATEMENT[$LANG]}"\
			"${EXIT_SELECTION[$LANG]}" "${EXIT_STATEMENT[$LANG]}" \
			3>&1 1>&2 2>&3)
SELECT=$?
case $SELECT in
		1)
		 clear
		;;
		255)
		 Setup_Menu
		;;
esac
for RESULT in $SELECT_MENU; do
    case $RESULT in
        ${SKIN_SELECTION[$LANG]})
		Skin_Dialog
			;;
		${REMOTE_SELECTION[$LANG]})
		remote_mapping
			;;
		${REMOTE_KEYBOARD_SELECTION[$LANG]})
		Keyboard_Code
		Setup_Menu
			;;
		${BUTTON_SELECTION[$LANG]})
		 BUTTON_MENU=0
		 if [ ! $DEFAULT_YESNO ]; then 
		  Default_List_YesNo
		 elif [ $DEFAULT_YESNO -eq 0 ]; then
		  Get_new_Index
		  pch_main_script_parameter
		  Button_Menu
		 elif [ $DEFAULT_YESNO -eq 1 ]; then
		  Get_new_Index
		  Button_Menu
		 else
		  Default_List_YesNo
		 fi
			;;
		${EXIT_SELECTION[$LANG]})
		clear
		;;
		${BUTTON_PORTAL_SELECTION[$LANG]})
		 BUTTON_MENU=1
		 if [ ! $DEFAULT_EASYPORTAL_YESNO ]; then 
		  Default_List_easyPortal_YesNo
		 elif [ $DEFAULT_EASYPORTAL_YESNO -eq 0 ]; then
		Get_new_Index
		pch_main_script_parameter_easyportal
		Button_Menu_easyPortal
		elif [ $DEFAULT_EASYPORTAL_YESNO -eq 1 ]; then
		  Get_new_Index
		  Button_Menu_easyPortal
		 else
		  Default_List_easyPortal_YesNo
		 fi
		;;
	esac
done
}

############################Button Menü easyPortal#############################
Button_Menu_easyPortal()
{
SELECT_MENU=$(dialog --backtitle "${TITLE[$LANG]}" \
			--title "${BUTTON_PORTAL_SELECTION_TITLE[$LANG]}" \
			--menu "${BUTTON_SELECTION_MENU[$LANG]}" 0 0 0 \
			"${BUTTON_SELECTION_1[$LANG]}" "" \
			"${BUTTON_SELECTION_20[$LANG]}" "" \
			"${BUTTON_SELECTION_30[$LANG]}" "" \
			"${BUTTON_SELECTION_BACK[$LANG]}" "${BUTTON_STATEMENT_BACK[$LANG]}" 3>&1 1>&2 2>&3) 
SELECT=$?
case $SELECT in
		1)
		DEFAULT_EASYPORTAL_YESNO=""
		 Setup_Menu
		;;
		 255)
		 Setup_Menu
		;;
esac
for RESULT in $SELECT_MENU; do
 case $RESULT in
        ${BUTTON_SELECTION_1[$LANG]})
            Standard_Programme_Menu
            ;;
        ${BUTTON_SELECTION_20[$LANG]})
            System_Programmme_Menu
            ;;
        ${BUTTON_SELECTION_30[$LANG]})
            Systemsteuerung_Menu
            ;;
        ${BUTTON_SELECTION_BACK[$LANG]})
            Set_Main_Script_Parameter_easyportal
			DEFAULT_EASYPORTAL_YESNO=0
            Setup_Menu
            ;;
 esac
done
}
############################Button Menü#############################
Button_Menu()
{
SELECT_MENU=$(dialog --backtitle "${TITLE[$LANG]}" \
			--title "${BUTTON_PCHANGER_SELECTION_TITLE[$LANG]}" \
			--menu "Auswahl:" 0 0 0 \
			"${BUTTON_SELECTION_1[$LANG]}" "" \
			"${BUTTON_SELECTION_10[$LANG]}" "" \
			"${BUTTON_SELECTION_20[$LANG]}" "" \
			"${BUTTON_SELECTION_30[$LANG]}" "" \
			"${BUTTON_SELECTION_BACK[$LANG]}" "${BUTTON_STATEMENT_BACK[$LANG]}" 3>&1 1>&2 2>&3) 
SELECT=$?
case $SELECT in
		1)
		 DEFAULT_YESNO=""
		 Setup_Menu
		;;
		 255)
		 Setup_Menu
		;;
esac
for RESULT in $SELECT_MENU; do
 case $RESULT in
        ${BUTTON_SELECTION_1[$LANG]})
            Standard_Programme_Menu
            ;;
        ${BUTTON_SELECTION_10[$LANG]})
            Bookmarks_Menu
            ;;
        ${BUTTON_SELECTION_20[$LANG]})
            System_Programmme_Menu
            ;;
        ${BUTTON_SELECTION_30[$LANG]})
            Systemsteuerung_Menu
            ;;
        ${BUTTON_SELECTION_BACK[$LANG]})
            Set_Main_Script_Parameter
			DEFAULT_YESNO=0
            Setup_Menu
            ;;
 esac
done
}
############################Button Auswahl############################
Standard_Programme_Menu()
{
MENU_PAGE_NR=1
MENU_TITLE="${BUTTON_SELECTION_1[$LANG]}"

#Set_Button_List
if [ $BUTTON_MENU -eq 0 ]; then
 Button_List
 Button_Menu
elif [ $BUTTON_MENU -eq 1 ]; then
 easyPortal_Button_List
 Button_Menu_easyPortal
fi
}


Bookmarks_Menu()
{
MENU_PAGE_NR=10
MENU_TITLE="${BUTTON_SELECTION_10[$LANG]}"

#Set_Button_List
if [ $BUTTON_MENU -eq 0 ]; then
 Button_List
 Button_Menu
elif [ $BUTTON_MENU -eq 1 ]; then
 easyPortal_Button_List
 Button_Menu_easyPortal
fi
}


System_Programmme_Menu()
{
MENU_PAGE_NR=20
MENU_TITLE="${BUTTON_SELECTION_20[$LANG]}"

#Set_Button_List
if [ $BUTTON_MENU -eq 0 ]; then
 Button_List
 Button_Menu
elif [ $BUTTON_MENU -eq 1 ]; then
 easyPortal_Button_List
 Button_Menu_easyPortal
fi
}


Systemsteuerung_Menu()
{
MENU_PAGE_NR=30
MENU_TITLE="${BUTTON_SELECTION_30[$LANG]}"

#Set_Button_List
if [ $BUTTON_MENU -eq 0 ]; then
 Button_List
 Button_Menu
elif [ $BUTTON_MENU -eq 1 ]; then
 easyPortal_Button_List
 Button_Menu_easyPortal
fi
}


#####################PChanger Skin X-Setup##########################
Skin_Setup()
{
VT_KONSOLE_OLD=$(fgconsole)         # aktuell aktives Terminal

rm_xauthority
easyvdr-desktop start
#Skin Auswahl Script starten
sleep 3
DISPLAY=$FRONTEND_DISPLAY $PCH_DIR/program-changer-enable-skin.sh
#Auf das Ende des Setups warten
while [ ! -e /tmp/skin-config-ready ]
 do
 sleep 1
done
rm /tmp/skin-config-ready
easyvdr-desktop stop
rm_xauthority
chvt $VT_KONSOLE_OLD               # zurueck zur aufrufenden Konsole
}

########################Remote Mapping##############################
remote_mapping()
{
dialog --backtitle "${TITLE[$LANG]}" --infobox "${REMOTE_MAPPING_INFO[$LANG]}" 5 40; $SETUP_DIR/easyvdr-make-remote-mapping; Setup_Menu
}


####################Abfrage Default Buttons laden####################
Default_List_YesNo()
{
if [ ! -e $MAIN_SCRIPT_PARAMETER ]; then
 DEFAULT_YESNO=0
 Get_new_Index
 Button_Menu
else
 dialog --title "Program-Changer"\
       --backtitle "${TITLE[$LANG]}" \
       --colors \
	   --no-label "${SAVE_SELECTION_NO[$LANG]}"\
	   --yes-label "${SAVE_SELECTION_YES[$LANG]}"\
       --yesno "${SAVE_SELECTION[$LANG]}" 7 40
 DEFAULT_YESNO=$?
 case $DEFAULT_YESNO in
   1) Get_new_Index
      Button_Menu
   ;;
   0) Get_new_Index
      pch_main_script_parameter
	  Button_Menu
   ;;
   255) Default_List_YesNo
   ;;
 esac
fi
}

####################Abfrage Default easyPortal Buttons laden####################
Default_List_easyPortal_YesNo()
{
if [ ! -e $MAIN_SCRIPT_PARAMETER_EASYPORTAL ]; then
 DEFAULT_EASYPORTAL_YESNO=0
 Get_new_Index
 Button_Menu
else
 dialog --title "Program-Changer"\
       --backtitle "${TITLE[$LANG]}" \
       --colors \
	   --no-label "${SAVE_SELECTION_NO[$LANG]}"\
	   --yes-label "${SAVE_SELECTION_YES[$LANG]}"\
       --yesno "${SAVE_SELECTION[$LANG]}" 7 40
 DEFAULT_EASYPORTAL_YESNO=$?
 case $DEFAULT_EASYPORTAL_YESNO in
   1) Get_new_Index
      Button_Menu_easyPortal
   ;;
   0) Get_new_Index
      pch_main_script_parameter_easyportal
	  Button_Menu_easyPortal
   ;;
   255) Default_List_easyPortal_YesNo
   ;;
 esac
fi
}
####################PChanger Keyboardcodes anlernen####################
Keyboard_Code()
{
abbruch=0
TEXT=${SHOW_HIDE_MENU_TEXT[LANG]}
Read_Key_Code
key_show_hide_menu=$(($KEY_CODE+8))
if [ $abbruch -ne 1 ];then 
 TEXT=${KEY_RED_TEXT[LANG]}
 Read_Key_Code
 key_red=$(($KEY_CODE+8))
fi
if [ $abbruch -ne 1 ];then 
 if [ $key_show_hide_menu -ne $key_red ];then
  TEXT=${KEY_GREEN_TEXT[LANG]}
  Read_Key_Code
  key_green=$(($KEY_CODE+8))
 else 
  Read_Key_Code_break
 fi
fi
if [ $abbruch -ne 1 ];then 
 if [ $key_show_hide_menu -ne $key_green ] && [ $key_red -ne $key_green ];then
  TEXT=${KEY_YELLOW_TEXT[LANG]}
  Read_Key_Code
  key_yellow=$(($KEY_CODE+8))
 else 
  Read_Key_Code_break
 fi
fi
if [ $abbruch -ne 1 ];then 
 if [ $key_show_hide_menu -ne $key_yellow ] && [ $key_red -ne $key_yellow ] && [ $key_green -ne $key_yellow ];then
  TEXT=${KEY_BLUE_TEXT[LANG]}
  Read_Key_Code
  key_blue=$(($KEY_CODE+8))
 else 
  Read_Key_Code_break
 fi
fi
if [ $abbruch -ne 1 ];then 
 if [ $key_show_hide_menu -ne $key_blue ] && [ $key_red -ne $key_blue ] && [ $key_green -ne $key_blue ] && [ $key_yellow -ne $key_blue ];then
  TEXT=${KEY_MENU_TEXT[LANG]}
  Read_Key_Code
  key_menu=$(($KEY_CODE+8))
 else 
  Read_Key_Code_break
 fi
fi
if [ $abbruch -ne 1 ];then 
 if [ $key_show_hide_menu -ne $key_menu ] && [ $key_red -ne $key_menu ] && [ $key_green -ne $key_menu ] && [ $key_yellow -ne $key_menu ] && [ $key_blue -ne $key_menu ];then
  Write_Keyboard_Code
 else 
  Read_Key_Code_break
 fi
fi
reset
}
Read_Key_Code_break()
{
dialog --backtitle "${TITLE[$LANG]}" --infobox "${INFO_ERROR[LANG]}" 5 40 && sleep 2 && abbruch=1
}
Read_Key_Code_ok()
{
dialog --backtitle "${TITLE[$LANG]}" --infobox "${INFO_OK[LANG]}" 5 40 && sleep 1 && abbruch=0
}
#####################Skin Dialog > Abfrage ob X gestatet werden soll############################
Skin_Dialog()
{
dialog --backtitle "${TITLE[$LANG]}" --title "${SKIN_SELECTION[$LANG]}" --ok-label "${BUTTON_SELECTION_BACK[$LANG]}" --cancel-label "${BUTTON_SELECTION_CONTINUE[$LANG]}"  --pause "${SKIN_DIALOG_STATEMENT[$LANG]}" 12 65 20
SELECT_BUTTON=$?
case $SELECT_BUTTON in
		1)
		 Skin_Setup
		 Setup_Menu
		;;
		0)
		 Setup_Menu
		;;
		255)
		 Setup_Menu
		;;
esac
}
#######################Script Start########################
Get_Buttons_List
if [ ! -e $DEFAULT_BUTTON ]; then
 dialog --backtitle "${TITLE[$LANG]}" --infobox "${INFO_DEFAULT[LANG]}" 5 40
 DEFAULT_EASYPORTAL_YESNO=0
 DEFAULT_YESNO=0
 sleep 1
 Get_new_Index
 Set_Main_Script_Parameter
 Set_Main_Script_Parameter_easyportal
fi
Setup_Menu

