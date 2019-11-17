#!/bin/bash
#
#  initial              Bleifuss2
#  v 0.2     16.06.12   Captain_Beefheart
#  v 0.3     10.07.12   Bleifuss2 add XBMC Home Directory check
#  v 0.3     25.09.12   Bleifuss2 add inputlirc & for lircmap.xml
#  V 0.4     26.11.13   Bleifuss2 add $USER for home directrory
#  V 0.5     26.08.14   Bleifuss2 change remote mapping
#  V 0.5     17.10.15   Bleifuss2 remove XBMC support, adding KODI
#  V 0.6     17.02.16   Bleifuss2 new version
#Setup Return Codes
#define NO_ERROR				0
#define NO_REMOTE_CON				10
#define CREATE_CFG_FILE_ERROR			12
#define PARAMETER_ERROR				13
#define ERROR_READ_PCHANGER_CONF_FILE		14
#define ERROR_READ_PCHANGER_SH_FILE		15


##### includes #####
. /usr/lib/vdr/easyvdr-config-loader
. /usr/lib/vdr/functions/easyvdr-functions-lib
. /usr/share/easyvdr/setup/easyvdr-setup-defines
. /usr/share/easyvdr/setup/easyvdr-setup-functions

##### definitionen #####

USER=$STANDARD_DESKTOP_USER
HOME_PATH=$(cat /etc/passwd | grep ^$USER: | awk -F: '{print $6}')

PCH_DIR="/usr/share/easyvdr/program-changer"                              #PChanger directory
PCH_ETC_CONF="/etc/vdr/easyvdr/program-changer/program-changer.conf"              #Parameter 1: program-changer.conf
PCH_ETC_CONF_DIR="/etc/vdr/program-changer"                               #Parameter 1: program-changer.conf
PCH_KEY_CONF="/etc/vdr/program-changer/program-changer-keyboard.conf"     #Pfad für Tastatur Belegung
PCH_REMOTE_MAPPING="/usr/share/easyvdr/setup/easyvdr-make-remote-mapping"
PCH_ENABLE_SCRIPT="$PCH_DIR/program-changer-enable-skin.sh"
PCH_TEMPLATES="/usr/share/easyvdr/program-changer/templates"
PCH_CONF="/usr/share/easyvdr/program-changer/config"
KODI_HOME="$HOME_PATH/.kodi"                                  	          #Pfad für KODI Home Verzeichnis
KODI_HOME_USERDATA="$HOME_PATH/.kodi/userdata"                     	  #Pfad für KODI Home/userdaten Verzeichnis
TIMEOUT=10                                                                # 10 sec timeout

##### functions  ####

#####

Run_Pch_Setup()
{
CFG_FILES_EXIST="false"; 
[ -f $PCH_ETC_CONF ] && CFG_FILES_EXIST="true"


if [[ $CFG_FILES_EXIST == "true" ]]
  then
    if [ ! -d $KODI_HOME ]; then
     mkdir $KODI_HOME
      if [ ! -d $KODI_HOME_USERDATA ]; then
       mkdir $KODI_HOME_USERDATA
       chown $USER:$USER $KODI_HOME_USERDATA
       chmod -R 777 $KODI_HOME
     fi
    fi
 
    if [ ! -e $PCH_CONF/version ];then
     grep -i Version $PCH_TEMPLATES/list-of-programs-conf > $PCH_CONF/version
    fi

    #Püfen ob Version passt 
    grep -i Version $PCH_TEMPLATES/list-of-programs-conf | diff -  $PCH_CONF/version 

    if [  $? -ne "0" ] ;then
     cp $PCH_TEMPLATES/program-changer-main-script-parameter $PCH_CONF/program-changer-main-script-parameter
     if [ -f $PCH_CONF/setup.conf ];then
      rm $PCH_CONF/setup.conf
     fi
     if [ -f $PCH_CONF/version ];then
      rm $PCH_CONF/version
     fi
    fi

    #Testen ob User Konfiguration existiert
    if [ ! -e $PCH_CONF/list-of-programs-user.conf ]; then
     cp $PCH_TEMPLATES/list-of-programs-user-conf  $PCH_CONF/list-of-programs-user.conf
    fi

    #Schriftgrößen rücksetzen
    if [  -e $PCH_ETC_CONF_DIR/program-changer.css ]; then
     rm $PCH_ETC_CONF_DIR/program-changer.css
    fi
    if [  -e $PCH_ETC_CONF_DIR/program-changer-setup.css ]; then
     rm $PCH_ETC_CONF_DIR/program-changer-setup.css
    fi

    #Datenbank erstellen
    cat $PCH_TEMPLATES/list-of-programs-conf > $PCH_CONF/list-of-programs.conf
    cat $PCH_CONF/list-of-programs-user.conf >> $PCH_CONF/list-of-programs.conf

    if ( check_x_running  >/dev/null 2>&1 ) && ( ! test -z "$DISPLAY" ); then
      /usr/bin/program_changer_setup -pchanger-conf $PCH_ETC_CONF  -remote-mapping-script $PCH_REMOTE_MAPPING -enable-skin-script $PCH_ENABLE_SCRIPT
      CNF_SUCCESS=$?
    else
     CONSOLE=$(fgconsole)
     if [ $? -eq "0" ] ;then
      $PCH_DIR/lib/setup.sh
      CNF_SUCCESS=$?    
     fi
    fi
    $PCH_DIR/lib/make-program-changer-script.sh
fi 

if [[ $CNF_SUCCESS != 0 ]]
  then 
     Show_Message
fi
}

#####
Write_Status()
{
RC_PCH_STAT=$1
sed -i '/"*program-changer-setup.sh"*/d' $SETUP_STATUS
echo "# `date` program-changer-setup.sh zuletzt ausgefuehrt" >> $SETUP_STATUS
sed -i '/PCH_Setup"*/d' $SETUP_STATUS
echo "PCH_Setup=\"$RC_PCH_STAT\""                            >> $SETUP_STATUS
}

#####
Show_Message()
{
TITLE=" PChanger Fehlermeldung "

case $CNF_SUCCESS in
   1) TEXT1="Fehler1"                                    ;;
   2) TEXT1="Fehler2"                                    ;;
   3) TEXT1="Fehler3"                                    ;;
   *) TEXT1="unbekannter Fehler"                         ;;
esac

TEXT2=" evtl. auch in fett ! "
TEXT3=" oder in rot  "
TEXT4=" oder auch so "

[ -f /usr/bin/zenity ] && Displ_with_Zenity || Displ_with_Dialog
}

#####
Displ_with_Zenity()
{
  {
  i=0
  TIMEOUT_Z=$(( $TIMEOUT * 10 ))
  while (( $i <= $TIMEOUT_Z ))
    do 
      echo $i; sleep 0.1; i=$(( $i + 1 ))
    done ;
  } |zenity --progress \
            --title "$TITLE" \
            --percentage=0 \
            --auto-close \
            --auto-kill \
            --no-cancel \
            --window-icon="$PCH_DIR/pictures/easyvdr-logo.png"\
            --width=530 \
            --height=200 \
            --text \
"$TEXT1\n\
<b>$TEXT2</b> \n\
<span color=\"red\">$TEXT3</span>\n\
<span size=\"xx-large\">$TEXT4</span>\n\n"
}

#####
Displ_with_Dialog()
{
xterm -e bash $PCH_DIR/lib/Dialog_Message "$TITLE" $TIMEOUT "$TEXT1" "$TEXT2" "$TEXT3" "$TEXT4"
}

####################### run ##################################

RunF=1
if (ps -e | grep -q easyvdr-runfron); then
  RunF=0
  svdrpsend remo off
  sleep 1
  stop easyvdr-frontend
fi


Run_Pch_Setup

if [ $RunF -eq 0 ]; then
   svdrpsend remo on
   sleep 1
   start easyvdr-frontend
fi

