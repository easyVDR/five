#!/bin/sh

# EDIT only! path2java


#$ENV{PATH}="/bin:/sbin:/usr/bin:/usr/local/bin:/opt/blackdown-jre-1.4.2.03/bin";


artist="$1"
album="$2"
coverdir="$3"
tmpdir="$4"
maxdl="$5"
basedir="$6"
filename="$7"
tempdir="$tmpdir/music_cover"
nodata="$tmpdir/music_nodata"
logger="$tempdir/cover.log"
CMD=""
chooserdir="$HOME/.coverchooser/Temp"


echo ""
echo "-----------------------------------------------------------------------------------"
echo "NAME:"
echo "      getcover.sh - bash Script to download coverpictures"
echo ""
echo "SYNOPSIS:"
echo "      getcover.sh 'artist' 'album' 'coverdir' 'tmpdir' 'maxdl' 'basedir' 'filename'"
echo ""
echo "DESCRIPTION:"
echo "      This bash script makes it a lot easier to fill your"
echo "      musiccollection with coverpictures."
echo ""
echo "OPTIONS:"
echo "     artist      = Artist of track"
echo "     album       = Name of album"
echo "     coverdir    = Path to artists cover directory"
echo "     tmpdir      = Path where to save downloaded images"
echo "     maxdl       = How much coverpicture do you want ?"
echo "     basedir     = Path to track"
echo "     filename    = Full path to track without suffix"
echo ""
echo "-----------------------------------------------------------------------------------"
echo ""

# START.......
for file in $tempdir/*.log
do
    if [ -f "${file}" ]; then
    echo "Loesche Datei: '$file'"
    rm -f "${file}";
    fi
done


echo "========================" > $logger
echo "LOGDATEI 'cover.log'"     >> $logger
echo "Cover download started !" >> $logger
echo "========================" >> $logger

# DON'T CHANGE ANYTHING HERE
echo "Artist     : $artist"              >> $logger
echo "Album      : $album"               >> $logger
echo "Coverdir   : $coverdir"            >> $logger
echo "Tmpdir     : $tmpdir"              >> $logger
echo "Tempdir    : $tempdir"             >> $logger
echo "Nodata     : $nodata"              >> $logger
echo "MaxDL      : $maxdl"               >> $logger
echo "Logfile    : $logger"              >> $logger
echo "Chooserdir : $chooserdir"          >> $logger
echo ""                                  >> $logger
echo "<value> ARTIST    =$artist"        >> $logger
echo "<value> ALBUM     =$album"         >> $logger
echo "<value> COVERDIR  =$coverdir"      >> $logger
echo "<value> BASEDIR   =$basedir"       >> $logger
echo "<value> FILENAME  =$filename"      >> $logger
echo ""                                  >> $logger




if [ "$artist" = "" ]; then
    echo "" >> $logger
    echo "Artist empty. Give up !" >> $logger
    cat $logger
    exit 1
fi


if [ ! -r "$tempdir/cover.log" ]; then
     echo "$tempdir/cover.log konnte nicht gelesen werden (ABBRUCH)" >> $logger
     echo "$tempdir/cover.log konnte nicht gelesen werden (ABBRUCH)"
     exit 1
fi

for file in $tempdir/*.jpg
do
    if [ -f "${file}" ]; then
    echo "Loesche Datei: '$file'" >> $logger
    rm -f "${file}";
    fi
done

for file in $tempdir/*.png
do
    if [ -f "${file}" ]; then
    echo "Loesche Datei: '$file'" >> $logger
    rm -f "${file}";
    fi
done

for file in $tempdir/*.gif
do
    if [ -f "${file}" ]; then
    echo "Loesche Datei: '$file'" >> $logger
    rm -f "${file}";
    fi
done


#if [ -f "$tempdir/cover.log" ]; then
#    echo "" >> $logger
#    echo "Entferne vorhandene Bilddateien aus '$tempdir'" >> $logger
#    echo "" >> $logger
#    rm $tempdir/*.jpg
#    rm $tempdir/*.png
#else
#    echo "" >> $logger
#    echo "$tempdir/cover.log nicht vorhanden (ABBRUCH)" >> $logger
#    echo "" >> $logger
#    cat $logger
#    exit
#fi

echo "" >> $logger
coverchooser --artist=${artist} --album=${album} --path=${tempdir} --all --ordered --module=discogs --module=lastfm --module=google --timeout=15 --verbose
echo "Command: '$CMD'" >> $logger
echo "" >> $logger

echo ""
echo ""
echo "====================== Coverdownload start here ====================="
echo ""
$CMD
echo ""
echo "====================== Coverdownload end here   ====================="
echo ""
echo ""



for file in $chooserdir/*.jpg
do
    if [ -f "${file}" ]; then
    echo "Kopiere Datei: '$file' nach '$tempdir'" >> $logger
    cp -f "${file}" $tempdir;
    fi
done

for file in $chooserdir/*.png
do
    if [ -f "${file}" ]; then
    echo "Kopiere Datei: '$file' nach '$tempdir'" >> $logger
    cp -f "${file}" $tempdir;
    fi
done

for file in $chooserdir/*.gif
do
    if [ -f "${file}" ]; then
    echo "Kopiere Datei: '$file' nach '$tempdir'" >> $logger
    cp -f "${file}" $tempdir;
    fi
done



#cp -f $chooserdir/*.jpg $tempdir
#cp -f $chooserdir/*.png $tempdir


result="0"

for file in $chooserdir/*.jpg
do
    if [ -f "${file}" ]; then
    result="1";
    break
    fi
done

for file in $chooserdir/*.png
do
    if [ -f "${file}" ]; then
    result="1";
    break
    fi
done

for file in $chooserdir/*.gif
do
    if [ -f "${file}" ]; then
    result="1";
    break
    fi
done



if [ result="1" ]; then
    echo "" >> $logger
    echo "Download von Cover erfolgreich !" >> $logger
    echo "" >> $logger
    /usr/bin/svdrpsend PLUG coverviewer PLAYDIR $tempdir
else
    echo "" >> $logger
    echo "ERROR: Download von Cover NICHT erfolgreich !" >> $logger
    echo "" >> $logger
    /usr/bin/svdrpsend PLUG coverviewer PLAYDIR $nodata
fi

cat $logger
