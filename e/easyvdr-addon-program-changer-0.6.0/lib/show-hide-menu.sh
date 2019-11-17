#!/bin/bash
##### Beim Drücken der Taste SHOW_HIDE_MENU wird dieses Skript gestartet
############ Skript zum abschalten der Fernbedienung des VDR ###########
####################### Version 0.4  21.06.2012 ########################
# Version 0.5  10.07.12 Bleifuss2 Mute Parameter hinzugefügt
#              14.05.14 Mute entfernt
#              17.10.15 Anpassungen für Kodi
#Anderungen an der Variable müssen im Skript program-changer.sh & start_program.sh ebenfalls durchgeführt werden


TMP_FILE_SHOW_PCHANGER=/tmp/.show_program-changer

DEBUG=0

if [ $DEBUG -eq 1 ]; then
  echo "Parameter: $1 $2 $3 " >> /tmp/program-changer-show-hide-menu.sh
fi


function error {
 svdrpsend   MESG "show-hide-menu.sh Parameter falsch fuer Hilfe show-hide-menu.sh im terminal starten"
 echo 
 echo 
 echo Falscher Parameter
 echo Das Skript schaltet die Fernbedienung des VDR beim Aufruf des Menü aus und beim beenden wieder ein.
 echo Es muss als Parameter -menu-start oder -menu-end übergeben werden.
# echo Syntax: 
 exit 1
}


#Parameter testen
if [ $# -ne 0 ]; then
 if [ $1 = "-menu-start" ]; then
  svdrpsend remo off
  chvt 7
#  sleep 0.1
#  svdrpsend  MESG "Fernbedienung im VDR ist jetzt deaktiviert"
 else
  if [ $1 = "-menu-end" ]; then
    #Datei löschen (Befehl für Pchanger anzeigen)
    if [ -e $TMP_FILE_SHOW_PCHANGER ]; then
     rm $TMP_FILE_SHOW_PCHANGER
    fi
    #Nur bei den VDR Frontend die Fernbedienung wieder einschalten
    pgrep easyvdr-runfront*
    if [ $? -eq 0 ]; then
     #Testen ob kodi als Frontend läuft
     pgrep kodi*
     if [ $? -eq 0 ]; then
     exit 0
     fi
#    sleep 0.1
     svdrpsend remo on
     sleep 0.3
#    svdrpsend  MESG "Fernbedienung im VDR ist jetzt Aktiviert"
     exit 0
    fi
  else
   error
  fi
 fi
else
 error
fi
