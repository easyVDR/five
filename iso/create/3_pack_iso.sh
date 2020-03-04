#!/bin/bash

DIR=/home/easyvdr-iso/ISO/
IMAGE=$DIR/ea5-prealpha-006.iso
BUILD=$DIR/cd-image/


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



#sudo xorriso -as mkisofs -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 4 -b isolinux/isolinux.bin -c isolinux/boot.cat -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -o $IMAGE $BUILD
#sudo xorriso -as mkisofs -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 4 -b isolinux/isolinux.bin -c isolinux/boot.cat -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -isohybrid-mbr /usr/lib/ISOLINUX/isohdpfx.bin -partition_offset 16   -o $IMAGE $BUILD
sudo xorriso -as mkisofs -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 4 -b isolinux/isolinux.bin -c isolinux/boot.cat -eltorito-alt-boot -e boot/grub/efi.img -no-emul-boot -isohybrid-gpt-basdat /usr/lib/ISOLINUX/isohdpfx.bin -partition_offset 16   -o $IMAGE $BUILD
isohybrid -u $IMAGE


#zsyncmake $IMAGE
#md5sum $IMAGE >md5.txt

