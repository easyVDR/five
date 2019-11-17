#!/bin/bash -e
#set -x

#CONFIG

SOURCEDIR=/etc/vdr/plugins/skinnopacity/logos
OUTPUTDIR=/etc/vdr/plugins/skinnopacity/logos_darkred
FOREGROUND=/usr/local/src/VDR/PLUGINS/src/skinnopacity/logoconverter/backgrounds/fg4.png
BACKGROUND=/usr/local/src/VDR/PLUGINS/src/skinnopacity/logoconverter/backgrounds/bg2.png

#MAIN
IFS=$'\n'
FILES=$(find $SOURCEDIR -name *.png | sort)
sedstring="s!$SOURCEDIR!$OUTPUTDIR!g"

[ ! -d $SOURCEDIR ] && (echo "ERR: $SOURCEDIR1 not found"; exit 1);
[ ! -f $FOREGROUND ] && (echo "ERR: $FOREGROUND not found"; exit 1);
[ ! -f $BACKGROUND ] && (echo "ERR: $BACKGROUND not found"; exit 1);

echo "Convert new logos to format $(basename $OUTPUTDIR)"
for file in $FILES; do
  targetdir=`dirname $file | sed -e $sedstring`
  targetfile="$targetdir/$(basename $file)"
  if [ ! -f $targetfile ] ; then
    echo $targetfile
    [ ! -d $targetdir ]  && mkdir -p $targetdir
    convert +dither -background 'transparent' -resize '220x164' -extent '268x200' -gravity 'center' "$file" png:- 2> /dev/null | \
      composite - $BACKGROUND png:- 2> /dev/null | \
      composite -compose screen -blend 50x100 $FOREGROUND - "$targetfile" 2> /dev/null
  fi
done

exit 0
