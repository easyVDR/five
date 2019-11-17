#!/bin/sh
#
# music_copytrack.sh - v.0.1
#
# 2005/28/12 by Morone
#
#
#
##
### Usage:
### =====
### Edit ../data/command.conf to your need.
### Then place an entry in your musiccmds.conf like this:
### **********************************************************
### Copy track to CopyDir? : /usr/local/bin/music_copytrack.sh
### **********************************************************
### If you dont understand german , translate the output to you language.
##
#
#
#
#

#

SOURCE=$1
COPYDIR=$2
CONFIG=$6
. $CONFIG/data/musiccmds.conf
FILENAME=`basename "$1"`
TARGET=$COPYDIR/$FILENAME

#
if [ ! -d "$COPYDIR" ]; then
echo ""
echo "Verzeichnis nicht vorhanden."
echo "'$COPYDIR' wird erstellt"
mkdir -pv "$COPYDIR";
echo ""
echo ""
fi
#
#
#
FILESIZE=`du -s "$SOURCE" | awk '{ print $1 }'`
DIRSIZE=`df "$COPYDIR" | tail -n1 | awk '{ print $4 }'`
#
FILESIZE_H=`du -sh "$SOURCE" | awk '{ print $1 }'`
DIRSIZE_H=`df -h "$COPYDIR" | tail -n1 | awk '{ print $4 }'`
#
# echo $FILESIZE | sed -e 's/^\([a-zA-Z0-9]*\).*/\1/'`
#
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
echo "Track wurde nach  '$TARGET'  kopiert."
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
