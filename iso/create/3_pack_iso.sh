#!/bin/bash

DIR=/home/easyvdr-iso/ISO/
IMAGE=$DIR/ea5-prealpha-005.iso
BUILD=$DIR/cd-image/


#sudo mount -o loop $DIR/org/*.iso $DIR/loop
#sudo rsync -av $DIR/loop/ $DIR/cd-image




cd $DIR/cd-image
chmod 666 $DIR/cd-image/install/filesystem.size
sudo du -sx --block-size=1 ./ | cut -f1 > $DIR/cd-image/install/filesystem.size
#
#mkdir -p /opt/indices /opt/apt-ftparchive
#cd /opt/indices/
#DIST=xenial
#for SUFFIX in extra.main main main.debian-installer restricted restricted.debian-installer; do
#  wget http://archive.ubuntu.com/ubuntu/indices/override.$DIST.$SUFFIX
#done



sudo mkisofs -r -V "easyVDR Server-ISO 001" \
            -cache-inodes \
            -J -l -b isolinux/isolinux.bin \
            -c isolinux/boot.cat -no-emul-boot \
            -boot-load-size 4 -boot-info-table \
            -o $IMAGE $BUILD


isohybrid $IMAGE
#isohybrid -u fertiges-isoimage.iso





#cd ..
#sudo xorriso -as mkisofs -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 4 -b isolinux/isolinux.bin -c isolinux/boot.cat -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -o ../fertiges-isoimage.iso .
#cd ..
#
#ISO="easyvdr-4.0.0-64-lubuntu-prealpha010.iso"
#mv fertiges-isoimage.iso $ISO
#zsyncmake $ISO
#md5sum $ISO >md5.txt

