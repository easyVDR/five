#!/bin/bash
#
# Script aus VDR-Portal, keine Ahnung wer es veröffentlicht hat.
# mango 14.08.2015 Script an easyVDR angepasst
# mblaster4711 16.10 2015 Startparameter added batch
#
# convert mit HanbrakeCLI Profile --preset=
# https://trac.handbrake.fr/wiki/BuiltInPresets
#
LOG="/media/easyvdr01/convert/divx/convert.log"
var1=$(atq|wc -l)
var=$[$var1*90]
## echo "/usr/share/easyvdr-addon-convert/handbrake_easyvdr.sh \"${1}\" \"${2}\" >> ${LOG} 2>&1 " | at now "+"$var"min"  >> ${LOG} 2>&1 & ##
echo "/usr/share/easyvdr-addon-convert/handbrake_easyvdr.sh \"${1}\" \"${2}\" >> ${LOG} 2>&1 " | batch  >> ${LOG} 2>&1 &
