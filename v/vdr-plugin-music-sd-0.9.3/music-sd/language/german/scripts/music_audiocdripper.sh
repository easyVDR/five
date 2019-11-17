#!/bin/sh
#
# 2006/11/14 by Morone
#
# music_audiocdripper.sh - v.0.1
#
# A script to start vdrplugin-ripit over SVDRP
#$1 songname
#$2 copydir
#$3 artist
#$4 album
#$5 artistcoverdir
#
#
#
##
### Usage:
### ======
### CHANGE below the PATH to svdrpsend.pl.
### Place an entry in musiccmds.dat like this:
### **********************************************************************************
### Start AudioCD-Ripper?   :  echo "/PATH_TO_SCRIPT/music_audiocdripper.sh" | at now;
### **********************************************************************************
### thats all and lets hope you get good results ;)
##
#
#
#
#
$SVDRP_CMD PLUG ripit MAIN
#
#
#######################################EOF
