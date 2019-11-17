#!/bin/bash
############# Pchanger Easyportal Start Skript
#v 0.1 Bleifuss

#Anderungen an der Variable müssen im Skript program-changer.sh & show_hide-menu.sh ebenfalls durchgeführt werden
TMP_FILE_SHOW_PCHANGER=/tmp/.show_program-changer

if [ -e $TMP_FILE_SHOW_PCHANGER ]; then
 rm $TMP_FILE_SHOW_PCHANGER
fi
#Start Program
if [ $# -ne 0 ]; then
echo $*
 $*
fi
echo FRONTEND > /tmp/.pch-exec