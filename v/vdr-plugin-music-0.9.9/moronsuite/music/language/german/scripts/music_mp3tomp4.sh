#!/bin/sh

## script to convert audiofiles to mpeg4
##
## req.: ffmpeg

# sh mp3tomp4.sh /media/MyMP3s/rockers.mp3  /media/MyMP4s


FILE=$1
MP4DIR=$2
CONFIG=$6
. $CONFIG/data/musiccmds.conf

OLD_IFS=$IFS
IFS='
'

FILENAME=`basename "$FILE"`
FILENAME2=`basename "$FILE" .mp3`


if [ -f "$FILE" ]; then
  echo "Kopiere '$FILE' nach /tmp"
  $CP_FILES_CMD "$FILE" /tmp
fi

INPUT="/tmp/$FILENAME"

if [ -f "$INPUT" ]; then
        chmod +w $INPUT
        OUTPUT="/tmp/$FILENAME2.m4a"
        echo "Encodiere..."
        ffmpeg -i "/tmp/$FILENAME" $OUTPUT
        echo "Verschiebe $OUTPUT ins Zielverzeichnis.."
        mv -f $OUTPUT $MP4DIR/
        exit 0
fi

echo "FILE      = $FILE"
echo "MP4DIR    = $MP4DIR"
echo "FILENAME  = $FILENAME"
echo "FILENAM2  = $FILENAME2"
echo "INPUT     = $INPUT"
echo "OUTPUT    = $OUTPUT"

IFS=$OLD_IFS