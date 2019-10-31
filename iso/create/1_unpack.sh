#!/bin/sh

DIR=/home/martin/source/ISO/
#wget xxx

sudo mount -o loop $DIR/org/*.iso $DIR/loop
sudo rsync -av $DIR/loop/ $DIR/cd-image

sudo umount $DIR/loop