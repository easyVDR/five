#!/bin/bash
# Script VDR Frontend Kodi
# Beta 0.3 04.12.2015 gb
# remove xbmc 01.03.16 Bleifuss

PROGRAM_CHANGER=/usr/share/easyvdr/program-changer/program-changer.sh
###Abfrage >>> läuft der Wechsel gerade, dann Ende###
if [ -e /tmp/.switch_aktiv ];then
 exit;
fi
###Setze >>> Script läuft###
touch /tmp/.switch_aktiv

###Abfrage >>> was ist gerade aktiv (Frontend/Kodi)###
Frontend_Aktiv=$(ps ax | grep -wc easyvdr-runfrontend)
Kodi_Aktiv=$(ps ax | grep -wc kodi.bin)

if [ $Frontend_Aktiv -eq 2 ];then
 VDR_Kodi_Aktiv="FRONTEND"
else
 if [ $Kodi_Aktiv -eq 2 ];then
  VDR_Kodi_Aktiv="KODI"
 else
  VDR_Kodi_Aktiv="NONE"
 fi
fi

###Ist nichts aktiv, dann starte Frontend###
if [ $VDR_Kodi_Aktiv = "NONE" ];then
  $PROGRAM_CHANGER FRONTEND
 if [ -e /tmp/.switch_aktiv ];then
  rm /tmp/.switch_aktiv
 fi
 exit;
else
###Ist Frontend aktiv dann beende was läuft und starte Kodi### 
 if [ $VDR_Kodi_Aktiv = "FRONTEND" ];then
  $PROGRAM_CHANGER KODI
 fi
###Ist Kodi aktiv dann beende was läuft und starte Frontend### 
 if [ $VDR_Kodi_Aktiv = "KODI" ];then
  $PROGRAM_CHANGER FRONTEND
 fi
fi
sleep 2
if [ -e /tmp/.switch_aktiv ];then
 rm /tmp/.switch_aktiv
fi