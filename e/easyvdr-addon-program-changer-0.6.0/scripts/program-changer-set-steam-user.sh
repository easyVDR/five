#!/bin/bash
#15.01.2016 V0.1 gb
#sed steam-user
PROGRAM_CHANGER_SCRIPT_CONF="/etc/vdr/program-changer/program-changer-script.conf"
STEAM_USER=$(grep STEAM_USER_NAME $PROGRAM_CHANGER_SCRIPT_CONF|cut -d'"' -f2)
STEAM_USER_PASSWORD=$(grep STEAM_USER_PASSWORD $PROGRAM_CHANGER_SCRIPT_CONF|cut -d'"' -f2)
MYPChangerPictures="/usr/share/easyvdr/program-changer/pictures"
PASSWORD_COUNT=1
for USER in $STEAM_USER
 do
 PASSWORD=$(echo $STEAM_USER_PASSWORD|cut -d" " -f$PASSWORD_COUNT)
 array=($(/usr/bin/yad --center --title="Program Changer Steam User$PASSWORD_COUNT" \
 --form \
 --item-separator=, \
 --separator=" " \
 --field="Username:TEXT" \
 $USER \
 --field="Passwort:TEXT" \
 $PASSWORD \
 --button="Speichern:0" \
 --button="Abbrechen:2" \
 --image=$MYPChangerPictures/button-steam.png))
 ButtonAuswahl=$?
 if [ $ButtonAuswahl -eq "0" ];then
  let INDEX_COUNT=$PASSWORD_COUNT-1
  sed -i 's!STEAM_USER_NAME.*'$INDEX_COUNT']="'$USER'"!STEAM_USER_NAME['$INDEX_COUNT']="'${array[0]}'"!;s!STEAM_USER_PASSWORD.*'$INDEX_COUNT']="'$PASSWORD'"!STEAM_USER_PASSWORD['$INDEX_COUNT']="'${array[1]}'"!' $PROGRAM_CHANGER_SCRIPT_CONF 
 fi
 let PASSWORD_COUNT=$PASSWORD_COUNT+1
 if [ $ButtonAuswahl -eq "2" ] ;then
  exit 0
 fi
 if [ $ButtonAuswahl -eq "252" ] ;then
 exit 0
 fi
done
