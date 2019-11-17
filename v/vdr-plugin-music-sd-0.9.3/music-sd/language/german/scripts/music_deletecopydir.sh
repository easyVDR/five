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
###
### Edit ../data/command.conf to your need.
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
#Readin config
. $CONFIG/data/musiccmds.conf

#
if [ ! -d "$COPYDIR" ]; then
echo ""
echo "Verzeichnis nicht vorhanden."
echo ""
echo ""
exit 1
else
echo "Execute: $RM_DIR_CMD '$COPYDIR/*'"
$RM_DIR_CMD $COPYDIR/*;
fi

$SVDR_CMD MESG "Löschvorgang abgeschlossen"
#
#
#######################################EOF
