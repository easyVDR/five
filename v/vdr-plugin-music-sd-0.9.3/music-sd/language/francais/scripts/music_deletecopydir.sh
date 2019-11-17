#!/bin/sh
#
# music_deleteusbstick.sh - v.0.1
#
# 2005/28/12 by Morone
#
#
#
##
### Usage:
### =====
### Edit %COMMAND to your need.
### Then place an enry in your musiccmds.conf like this:
### **********************************************************
### Copy track to CopyDir? : /usr/local/bin/music_copytrack.sh
### **********************************************************
### If you dont understand german , translate the output to you language.
##
#
#
#
#active Song
SONG=$1
#CopyDir
COPYDIR=$2
#Artist
ARTIST=$3
#Album
ALBUM=$4
#ArtistCover
COVERDIR=$5
#Config dir
CONFIG=$6
#Delete commmand
COMMAND='/usr/bin/rm -rf' 
#SVDRP command
SVDRCMD="/VDR/bin/svdrpsend.pl"
#
#
if [ ! -d "$COPYDIR" ]; then
echo ""
echo "Verzeichnis nicht vorhanden."
echo ""
echo ""
exit 1
else
echo "Execute: $COMMAND '$COPYDIR/*'"
$COMMAND $COPYDIR/*;
fi

$SVDRCMD MESG "Löschvorgang abgeschlossen"
#
#
#######################################EOF
