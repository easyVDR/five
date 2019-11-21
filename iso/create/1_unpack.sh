#!/bin/sh

DIR=/home/easyvdr-iso/ISO/

sudo mount -o loop $DIR/org/*.iso $DIR/loop
sudo rsync -av $DIR/loop/ $DIR/cd-image

sudo umount $DIR/loop