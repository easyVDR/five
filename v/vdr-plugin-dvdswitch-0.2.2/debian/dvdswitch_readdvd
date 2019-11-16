#!/bin/sh
#
# This script will be called by the vdr-plugin-dvdswitch to copy a DVD to
# the local drive.
#
# It gets the following parameters:
#
# $1 = directory for dvd's (see plugin configuration menu)
# $2 = name of dvd
# $3 = original dvd-device
#

rm -f '$1/$2.iso'
mkdir -p '$1/'

echo "echo \"Starting dvd copying... \" &&
  dd if=$3 of='$1/$2.iso'" | at now
