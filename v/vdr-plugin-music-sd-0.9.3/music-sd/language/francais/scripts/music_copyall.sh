#!/bin/bash

# parameter
# 1 : Full path to track ( e.g. "/media/mp3/album/Madonna/Best_of/Holiday.mp3" )
# 2 : Path where to copy tracks ( e.g. "/media/mobilephone/audio" )
# 3 : Name of artist ( e.g. "Madonna" )
# 4 : Name of album ( e.g. "Best of" )
# 5 : Path to artists cover directory ( e.g. "/media/pictures/cover" )
# 6 : Path to music configuration  ( e.g. "/etc/vdr/plugins/music" )

TARGET=$2
CONFIG=$6
SVDRCMD="/VDR/bin/svdrpsend.pl"

OLD_IFS=$IFS
IFS='
'

FILE=`cat $CONFIG/playlists/burnlist.m3u`

echo "Begin to copy tracks"

$SVDRCMD MESG "Kopiervorgang gestartet"

for i in $FILE; do
cp -f "$i" "$TARGET"
done

echo "Tracks copied..."

# comment this if you dont copy to a flashdevice (otherwise it didnt hurt.. ;))
echo "Wait 5 sec. before Sync"
sleep 5
sync


IFS=$OLD_IFS

$SVDRCMD MESG "Kopiervorgang abgeschlossen !"

echo "DONE !"

# mit Iso vorher
# ---------------
#echo "tracks copied..do iso"
#mkisofs -r -J -o /tmp/mp3burn.iso /tmp/burn

#echo "iso done..now start to burn"
#cdrecord -v dev=ATA:1,0,0 speed=4 /tmp/mp3burn.iso

## BURN MP3-CD

# on the fly with 4MB buffer..
# ---------------
#echo "tracks copied..burn files"
#mkisofs -r /tmp/burn | cdrecord -v fs=4m speed=4 dev=ATA:1,0,0 -


## BURN AUDIO-CD
#cdrecord -v -audio -pad dev=ATA:1,0,0 speed=4 /tmp/track*.wav

#echo "remove files..."
#rm -f /tmp/burn/*.*
#echo "DONE !"

#echo "CD ready"
