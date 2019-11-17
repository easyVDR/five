#!/bin/bash
#switch frontend V. 0.2 von Bleifuss
#08.07.12
#26.03.13 Fix Switch Frontend bei VPS Timer
#08.10.17 systemd update
#Teile sind von easyvdr-update von sinai und Captain Beafheart

#Bei falschen Parametern wird vdr-sxfe gestartet
case "$1" in
          xine)
            START_VDR_FRONTEND=xine
            ;;
          vdr-sxfe)
            START_VDR_FRONTEND=vdr-sxfe
            ;;
          KODI)
            START_VDR_FRONTEND=KODI
            ;;
          Softhddevice)
            START_VDR_FRONTEND=Softhddevice
            ;;
esac
. /usr/lib/vdr/easyvdr-config-loader

#echo Frontend: $VDR_FRONTEND
#Testen ob das Frontend gewechselt werden soll
if [ "$VDR_FRONTEND" == "$START_VDR_FRONTEND" ]; then
     easyvdr-frontend start
else
 ### Frontend wechseln
 ### "Test auf laufende Aufnahme"
 RECORD_STR=$(cut -d: -f1 $CFG_DIR/timers.conf) # alle Zeilen aus Datei in String lesen
 for RECORD in $RECORD_STR; do
 echo "Wert aus timers.conf:$RECORD"
 if [ $RECORD -gt 8 ] ;then # Abfrage ob groesser 1 - dann Aufnahme aktiv
     RECORD_ACTIVE="1"
     break
 fi
 done

 if [ "$RECORD_ACTIVE" == "1" ] ;then 
     DISPLAY=:0 view_picture /usr/share/easyvdr/program-changer/pictures/recording.gif 3
     easyvdr-frontend start 
  else
echo wahl: $START_VDR_FRONTEND
     easyvdr-vdr stop
     sleep 5
     . /usr/lib/vdr/functions/easyvdr-functions-lib          # Laden der Funktionen lib / damit der Befehl SetSysconfig auch bekannt ist
     SetSysconfig VDR_FRONTEND  $START_VDR_FRONTEND          # schreiben des ausgewählten Frontend in die sysconfig
     /usr/lib/vdr/easyvdr-set-settings "frontend"            # setze Parameter je nach frontend, das in der sysconfig eingetragen ist
     easyvdr-vdr start
     sleep 5
     easyvdr-frontend start START_STATE="sysstart"
 fi
fi
