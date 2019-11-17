#!/bin/sh

CONFIG=$6

. $CONFIG/data/musiccmds.conf



OLD_IFS=$IFS
IFS='
'

FILE=`cat $CONFIG/playlists/burnlist.m3u`

echo "Begin to copy tracks"
$SVDR_CMD MESG "MP3-Daten-CD wird erstellt"

for i in $FILE; do
cp -f "$i" "$MP_TMP_DIR";
done

IFS=$OLD_IFS


# mit Iso vorher
# ---------------
#echo "tracks copied..do iso"
#mkisofs -r -J -o /tmp/mp3burn.iso /tmp/burn

#echo "iso done..now start to burn"
#cdrecord -v dev=ATA:1,0,0 speed=4 /tmp/mp3burn.iso

## BURN MP3-CD

# on the fly with 4MB buffer..
# ---------------
echo "tracks copied..burn files"
mkisofs -r "$MP3_TMP_DIR" | cdrecord -v fs=$CDRECORD_BURN_BUFFER speed=$BURN_SPEED_FILES dev=$CDRECORD_DEV -


## BURN AUDIO-CD
#cdrecord -v -audio -pad dev=ATA:0,1,0 speed=4 /tmp/track*.wav

echo "remove files..."
$RM_FILES_CMD $MP3_TMP_DIR/*.*
echo "DONE !"

$SVDR_CMD MESG "MP3-Daten-CD wurde erstellt"

$EJECT_CMD $BURN_DEVICE

echo "CD ready"
