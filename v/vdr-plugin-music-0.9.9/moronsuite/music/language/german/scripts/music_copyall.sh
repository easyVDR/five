#!/bin/sh

TARGET=$2
CONFIG=$6
. $CONFIG/data/musiccmds.conf

OLD_IFS=$IFS
IFS='
'



FILE=`cat $CONFIG/playlists/burnlist.m3u`

echo "Beginne mit Kopiervorgang der Tracks"

$SVDR_CMD MESG "Kopiervorgang gestartet"

for i in $FILE; do
cp -f "$i" "$TARGET";
done

echo "Tracks kopiert..."

# comment this if you dont copy to a flashdevice (otherwise it didnt hurt.. ;))
#echo "Warte 5 Sekunden vor Syncronisierung"
#sleep 5
#sync


IFS=$OLD_IFS

$SVDR_CMD MESG "Kopiervorgang abgeschlossen !"

echo "FERTIG !"
