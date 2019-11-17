#!/bin/sh
#
# 2006/10/10 by Morone
#
# music_savesl.sh - v.0.1
#
# A script to copy the current searchresult to a place,
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
DATE=`date +%Y%m%d_%T`
TARGETDIR="/vdr_daten/mp3"
SOURCE="$TARGETDIR/@suchergebnis.m3u"
TARGET="$TARGETDIR/current-search-$DATE.m3u"
#
#
FILESIZE=`du -s "$SOURCE" | awk '{ print $1 }'`
DIRSIZE=`df "$TARGETDIR" | tail -n1 | awk '{ print $4 }'`
#
FILESIZE_H=`du -sh "$SOURCE" | awk '{ print $1 }'`
DIRSIZE_H=`df -h "$TARGETDIR" | tail -n1 | awk '{ print $4 }'`
#
COMMAND='/usr/bin/cp -f '
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
$COMMAND "$SOURCE" "$TARGET";
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
echo "Befehl:" $COMMAND
echo "Quelle: '$SOURCE'"
echo "Ziel: '$TARGET'"
echo ""
echo "Dateigröße: $FILESIZE"
echo "Verfügbar:   $DIRSIZE"
fi

#######################################EOF