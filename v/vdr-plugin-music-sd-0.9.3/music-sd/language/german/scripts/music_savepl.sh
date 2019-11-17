#!/bin/sh
#
# 2006/01/27 by Morone
#
# music_savepl.sh - v.0.1
#
# A script to copy the current playlist to a place,
# where you can edit the list in MP3-Plugins menu.
#
#
#
##
### Usage:
### ======
### Edit $SOURCE $TARGET $COMMAND to your need.
### Then place an entry in musiccmds.conf like this:
### ******************************************************
### Save current playlist for editing? : /usr/local/bin/music_savepl.sh
### ******************************************************
### If you dont understand german , translate the output in your language
##
#
#
#
CONFIG=$6
. $CONFIG/data/musiccmds.conf

DATE=`date +%Y%m%d_%T`
SOURCE="$MUSIC_LIB/@current.m3u"
TARGET="$MUSIC_LIB/current-$DATE.m3u"
#
#
FILESIZE=`du -s "$SOURCE" | awk '{ print $1 }'`
DIRSIZE=`df "$MUSIC_LIB" | tail -n1 | awk '{ print $3 }'`
#
FILESIZE_H=`du -sh "$SOURCE" | awk '{ print $1 }'`
DIRSIZE_H=`df -h "$MUSIC_LIB" | tail -n1 | awk '{ print $3 }'`
#
#
#
if [ $DIRSIZE -lt $FILESIZE ]; then
echo "#################"
echo "# -=  ERROR  =- #"
echo "#################"
echo ""
echo "Nicht genügend Speicherplatz vorhanden"
echo ""
echo ""
echo "Dateigröße: $FILESIZE Kbyte"
echo "Verfügbar:   $DIRSIZE Kbyte"
exit 1
else
$CP_FILES_CMD "$SOURCE" "$TARGET";
fi
#
#
if [ -e "$TARGET" ]; then
echo "Done"
echo ""
echo "Playliste wurde nach  '$TARGET'  kopiert."
echo ""
echo "Dateigröße= $FILESIZE_H" 
echo "Verfügbar  = $DIRSIZE_H"
exit 2
elif [ ! -e "$SOURCE" ]; then
echo "#################"
echo "# -=  ERROR  =- #"
echo "#################"
echo ""
echo "Quelldatei wurde nicht gefunden"
exit 3
else
echo "#################"
echo "# -=  ERROR  =- #"
echo "#################"
echo ""
echo "Fehler bei Kopiervorgang"
echo "Befehl:" $CP_FILES_CMD
echo "Quelle: '$SOURCE'"
echo "Ziel: '$TARGET'"
echo ""
echo "Dateigröße: $FILESIZE"
echo "Verfügbar:   $DIRSIZE"
fi

#######################################EOF
