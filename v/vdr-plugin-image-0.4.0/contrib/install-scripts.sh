#!/bin/sh
#
# $Id$
#
#set -xv
pushd $(dirname $0) &>/dev/null

BIN_FILES="../scripts/imageplugin.sh ../contrib/image_pregen.sh"
BIN_DEST="/usr/bin"

echo -e "Warning: this script will install these files:\n$BIN_FILES\nto $BIN_DEST,\npress CTRL-C to abort, or anykey to begin."
read ANYKEY

#installing bin files
echo "Installing $BIN_FILES to $BIN_DEST....."
for FILE in $BIN_FILES
do
	echo "copying $FILE to $BIN_DEST ..."
	rm -f $BIN_DEST/$FILE
	cp -f $FILE $BIN_DEST/$FILE;RV=$?
	[ $RV -ne 0 ] && echo -e "Errorcode $RV on file: $BIN_DEST/$FILE.\n"
done

#check and install mount.sh
if $(which mount.sh &>/dev/null)
    then 
	OLD_MOUNT=$(which mount.sh)
	echo -e "Should the existing mount.sh ("$OLD_MOUNT")\nbe overwriten? (y/N)"
	read -n 1 -t 90 $KEYPRESSED
	[ "z$KEYPRESSED" = "zy" -o "z$KEYPRESSED" = "zY" ] &&  rm -f $OLD_MOUNT && cp -f ../scripts/mount.sh $OLD_MOUNT
    else
	cp ../scripts/mount.sh $BIN_DEST/mount.sh 
fi

which mount.sh &>/dev/null || echo -e "Error could not execute mount.sh, please check.\n"

popd &>/dev/null &>/dev/null
echo -e "Remember to copy/create imagesources.conf and imagecmds.conf\nfiles in your plugins config directory."
