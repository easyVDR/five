#!/bin/bash
#
# Backup Script
#

#DATE=$(date +%F_%H%M%S)
DATE=$(date +%F_%H%M)
BACKUP_DIR="/media/easyvdr01/backup"

function back_all {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - Backup all erstellt" >> /var/www/log/easyportal.log 
  SOURCE="etc/lirc etc/X11/xorg.conf* etc/vdr etc/asound* etc/fstab home/easyvdr usr/lib/vdr usr/share/vdr usr/share/easyvdr usr/share/vdr-* var/lib/vdr"
  cd /
  sudo tar -cjpf $BACKUP_DIR/easyvdr-backup-$DATE-all.tar.bz2 $SOURCE
}

function back_cfg {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - Backup cfg erstellt" >> /var/www/log/easyportal.log 
  SOURCE="etc/lirc etc/X11/xorg.conf* etc/vdr etc/asound* etc/fstab var/lib/vdr"
#  EASYVDRHOST=less /var/lib/vdr/sysconfig | grep HOSTNAME | awk -F\'HOSTNAME\'
  cd /
#  sudo tar -cjpf $BACKUP_DIR/easyvdr-$EASYVDRHOST-backup-$DATE-cfg.tar.bz2 $SOURCE
  sudo tar -cjpf $BACKUP_DIR/easyvdr-backup-$DATE-cfg.tar.bz2 $SOURCE
}

function restore {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> /var/www/log/easyportal.log 
  stopvdr
  echo "$(date '+%d.%m.%Y %H:%M:%S') - Restore $2" >> /var/www/log/easyportal.log 
  sudo tar -xfvj $BACKUP_DIR/$2 -C /
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> /var/www/log/easyportal.log 
  startvdr
}

function mkbackupdir {
  mkdir $BACKUP_DIR
}

function mklink {
  // sudo ln -s /var/www/backup $BACKUP_DIR
  ln -s /media/easyvdr01/backup /var/www/backup
}


case $1 in

  back_all)
    back_all
  ;;

  back_cfg)
    back_cfg
  ;;

  restore)
    restore $2
  ;;

  mkbackupdir)
    mkbackupdir
  ;;

  mklink)
    mklink
  ;;


esac