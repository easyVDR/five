#!/bin/sh

CONFIG=$6
. $CONFIG/data/musiccmds.conf

# variables
OLD_IFS=$IFS
IFS='
'

INPUT=$1
REPLACE_PATH=$2


if [ !$INPUT ] || [ !$REPLACE_PATH ] ; then
echo "Command:"
echo "music_convertwinamp.sh [INPUTFILE] [REPLACE_PATH]"
echo ""
echo "where:"
echo "INPUTFILE      = filename(playlist) to convert"
echo "REPLACE_PATH   = string to delete in INPUTFILE"
echo ""
exit 1
fi


DIR=$(dirname $INPUT)
FILENAME=$(basename $INPUT)
NEWFILENAME=$(basename $FILENAME .m3u)
TMPFILE=$INPUT-music.bak

# replace path
REPLACE_PATH=`echo $REPLACE_PATH | tr [:lower:] [:upper:]`

# length to cut
CUTLENGTH=${#REPLACE_PATH}


echo "Converting started"
echo "=================="
echo "INPUT        = $INPUT"
echo "DIR          = $DIR"
echo "FILENAME     = $FILENAME"
echo "NEWFILENAME  = $NEWFILENAME"
echo "REPLACE_PATH = $REPLACE_PATH"
echo "TMPFILE      = $TMPFILE"
echo "CUTLENGTH    = $CUTLENGTH"
echo ""


cd $DIR

## CONVERT FILE TO UNIX FORMAT
if [ -f $DOS2UNIX_CMD ]; then
 $DOS2UNIX_CMD $FILENAME
fi


## CHECK FOR WINAMP
ISWINAMP=`cat $FILENAME | grep EXTM3U`
if [ ! $ISWINAMP ]; then
 echo "$FILENAME is not a winamp playlist, aborting !"
 exit 1
fi


## REMOVE TEMPFILE
if [ -f $TMPFILE ]; then
rm $TMPFILE
fi 

## CONVERTION
for i in `cat $INPUT`;
do
EXPR1=`echo $i |cut -b 2-4`;


if [ ! $EXPR1 == "EXT" ]; then
  EXPR2=`echo $i |cut -b 1-${#REPLACE_PATH}`;
  EXPR2=`echo $EXPR2 | tr [:lower:] [:upper:]`
#  echo "EXPR1= $EXPR1"
#  echo "EXPR2= $EXPR2"
#  echo "REPL = $REPLACE_PATH"

  if [ $EXPR2 == $REPLACE_PATH ]; then
#    echo "Line found and converted"
    echo $i |cut -b $((${#REPLACE_PATH}+2))- >> $TMPFILE;
  fi
fi

done;

IFS=$OLD_IFS

## REPLACE WIN SLASH AND CREATE NEW PLAYLIST
sed "s/\\\/\//g" $TMPFILE > $NEWFILENAME-converted.m3u


## REMOVE TEMPFILE
if [ -f $TMPFILE ]; then
echo "remove tempfile '$TMPFILE'"
rm $TMPFILE
fi 

echo ""
echo "..finito"
