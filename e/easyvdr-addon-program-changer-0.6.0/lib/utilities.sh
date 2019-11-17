#!/bin/bash
#18.03.13  V 0.2 add disable_pulseaudio_auto_start Bleifuss 
#          V 0.3 add Steam & Gnome Terminal Profile
#18.09.13        add PCHANGER_DISPLAY & USER
#18.09.13        add switch PULSEAUDIO_DEFAULT
#31.07.14        add Steam user-conf  
#03.11.16        change steam setup

#Testen ob Parameter schon eingelsesn sind
if [  X$MAIN_SCRIPT_HEADER != X"run" ];then
. /usr/share/easyvdr/program-changer/templates/program-changer-main-script-header
fi


#Nummer der Soundkarte "man amixer"
CARD=0
#Deaktiviert um festzustellen ob das noch jemand nutzt
CARD=100

function master_volume_up {
 MASTER_VOLUME=$(amixer -c $CARD get Master | grep   "\[" | cut -d "["  -f 2 | cut -d "%" -f 1)
 #echo $MASTER_VOLUME
 MASTER_VOLUME=$(($MASTER_VOLUME + 4))
 #echo $(($MASTER_VOLUME + 10))
 amixer set -c $CARD Master $MASTER_VOLUME%
}

function master_volume_down {
MASTER_VOLUME=$(amixer -c $CARD get Master | grep   "\[" | cut -d "["  -f 2 | cut -d "%" -f 1)
#echo $MASTER_VOLUME
MASTER_VOLUME=$(($MASTER_VOLUME - 4))
#echo $(($MASTER_VOLUME + 10))
amixer set -c $CARD Master $MASTER_VOLUME%
}

function master_volume_mute {
MUTE=$(amixer -c $CARD get Master | grep   "\[" | cut -d "["  -f 4 | cut -d "]" -f 1)
if [ $MUTE == "off" ] ; then
 amixer set -c $CARD Master on
 #sleep 1
else
 amixer set -c $CARD Master off
# sleep 1
fi
}

function vdr_shutdown {
if (! ps -e | grep -q easyvdr-runfron); then
 easyvdr-frontend start
 sleep 5
fi
/usr/bin/poweroffvdr &
}

function no_vdr_shutdown {
sleep 1
}

function vdr_restart {
 easyvdr-frontend stop
sleep 1
 easyvdr-vdr stop
sleep 5
 easyvdr-vdr start
sleep 1
 easyvdr-frontend start START_STATE="sysstart"
}

function start_firefox {
 find $HOME_PATH/.mozilla/firefox/ -name sessionstore.* -type f | xargs rm -f
#echo  firefox $1
  firefox $1
}


function nvidia_tv_settings {
## xorg.conf sichern 
 if [ ! -e /etc/X11/xorg.conf_backup_easyvdr ]; then
  cp /etc/X11/xorg.conf /etc/X11/xorg.conf_backup_easyvdr  
 fi
 nvidia-settings
## DPI Parameter hinzufügen
 cp /etc/X11/xorg.conf /etc/X11/xorg.tmp
 if ( ! grep -q DPI /etc/X11/xorg.conf );then
  ## Ist die Datei vorhanden?
  if [ -e /etc/X11/xorg.tmp ]; then 
   cat /etc/X11/xorg.tmp | sed -e '/"Monitor"/a\' -e ' Option  "UseEdidDPI" "FALSE"\n Option  "ConstantDPI" "FALSE"\n Option  "DPI"   "100x100"' > /etc/X11/xorg.conf
  fi
 fi
}


function easyvdr_setup {
 #VT_KONSOLE_OLD=$(fgconsole)        ## aktuell aktives Terminal merken
 chvt 2                             ## auf console 2 umschalten
 #chvt $VT_KONSOLE_OLD              FILE_PATH="/etc/xdg/autostart" # zurueck zur aufrufenden Konsole
}

function pch_setup {
##### includes #####
. /usr/lib/vdr/functions/easyvdr-functions-lib
. /usr/lib/vdr/easyvdr-config-loader
 /usr/share/easyvdr/program-changer/program-changer-setup.sh
 easyvdr-program-changer
}


function easyvdr_update {
 
 DISPLAY=$PCHANGER_DISPLAY xterm -fullscreen -e 'echo ;echo ;echo ;echo ;echo "Easyvdr Update wird gestartet";echo ;echo ;echo "BITTE KEINE TASTE DRÜCKEN BIS DAS UPDATE BEENDET IST"; echo ;echo ; /usr/bin/easyvdr-update -i ; sleep 10;'
 rm /tmp/.start_update
 start easyvdr-frontend START_STATE="sysstart"
}


############## Ab hier Steam
function install_steam {

#cd /var/tmp
#wget https://steamcdn-a.akamaihd.net/client/installer/steam.deb
#dpkg -i steam.deb
DISPLAY=$PCHANGER_DISPLAY su root -c "$PRG_INSTALLER -1 Steam $GAMES_INSTALL etherwake" 
DISPLAY=$PCHANGER_DISPLAY su root -c "xterm -fullscreen -e apt-get install steam"
DISPLAY=$PCHANGER_DISPLAY su -l vdr -c "steam" | at now

#apt-get -f install

#Etherwake für wake on lan
DISPLAY=$PCHANGER_DISPLAY /usr/bin/yad --title="Steam Installation" --text="Steam wurde installiert" --button="Weiter" 
}


function init_gaming_mod {

 #Streaming Server einschalten
 etherwake $STEAM_SERVER_MAC

# # Xbox 360 Treiber laden
# if [ $(lsmod | grep xpad -c) -gt 0 ] ; then
#  rmmod xpad #Kernel Treiber entladen
#  xboxdrv --mimic-xpad --dbus session -D &
# fi

#Steam User testen
 . /etc/vdr/program-changer/program-changer-script.conf
 SET_STEAM_USER="/usr/share/easyvdr/program-changer/program-changer-set-steam-user.sh"

 case $1 in
  chk_steam_passwd_1)
     if [ ${STEAM_USER_NAME[0]} == "HUGO1" ];then
      DISPLAY=$PCHANGER_DISPLAY $SET_STEAM_USER 
     fi
  ;;
  chk_steam_passwd_2)
     if [ ${STEAM_USER_NAME[1]} == "HUGO2" ];then
      DISPLAY=$PCHANGER_DISPLAY $SET_STEAM_USER 
     fi
  ;;
  chk_steam_passwd_3)
     if [ ${STEAM_USER_NAME[2]} == "HUGO3" ];then
      DISPLAY=$PCHANGER_DISPLAY $SET_STEAM_USER 
     fi
  ;;

 esac

 # Bildschirmauflösung einstellen
 # Parameter einlesen
. /etc/vdr/easyvdr/easyvdr-video-settings.conf

  DISPLAY=$PCHANGER_DISPLAY xrandr --output $VDR_SCREEN_CONNECTOR --off
  DISPLAY=$PCHANGER_DISPLAY xrandr --output $VDR_SCREEN_CONNECTOR --auto --rate 60.0

  XDG_RUNTIME_DIR=/run/user/2000 su vdr -l -c "/usr/lib/vdr/easyvdr-x-lib-pulseaudio GAMING"

}

function switch_xboxdriver {
 # Xbox 360 Treiber laden
 if [ $(lsmod | grep xpad -c) -gt 0 ] ; then
  rmmod xpad #Kernel Treiber entladen
  xboxdrv --mimic-xpad --dbus session -D &
 else
  pkill xboxdrv
  #Kerneltreiber laden
  modprobe xpad
 fi
}

#######################################

function wol_server {
 etherwake $WOL_SERVER_MAC
 sleep $WOL_SERVER_BOOT_TIME
 mount -a
 sleep 2
 svdrpsend UPDR
}

#######################################

function raspberry_streaming_on {
 apt-get install mplayer sshpass

 sshpass  -p raspberry ssh $RASPBERRY_USER@$RASPBERRY_NAME "echo /opt/vc/bin/raspivid --framerate $RASPBERRY_FREQ -t 999999 -o \- \| nc  $RASPBERRY_STREAM_CLIENT 5001 > cam_script" &
 sshpass  -p raspberry ssh $RASPBERRY_USER@$RASPBERRY_NAME "sh ./cam_script"&
 nc -l -p 5001 | mplayer -fps $FREQ -cache 1024 - &
}

#######################################

function install_google_chrome {
 if [ $(dpkg -l | grep " google-chrome-stable " | cut -d" " -f1) !="ii" ];then
  cd /tmp/
  DISPLAY=$PCHANGER_DISPLAY xterm -fullscreen -e "wget -q -O - https://dl-ssl.google.com/linux/linux_signing_key.pub | sudo apt-key add -"
  sh -c 'echo "deb http://dl.google.com/linux/chrome/deb/ stable main" >> /etc/apt/sources.list.d/google-chrome.list'
  DISPLAY=$PCHANGER_DISPLAY xterm -fullscreen -e "apt-get update"
  DISPLAY=$PCHANGER_DISPLAY su root -c "$PRG_INSTALLER $1 Google-Chrome google-chrome-stable" 
 fi
}

########## Main
$1 $2
