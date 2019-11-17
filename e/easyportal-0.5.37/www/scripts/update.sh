#!/bin/bash
#
# Update/Upgrade Scripte (apt-get)
#

#############################################################################
# Includes
#
. /usr/lib/vdr/easyvdr-config-loader
. /usr/lib/vdr/functions/easyvdr-functions-lib
. /usr/share/easyvdr/easyvdr-version


#############################################################################
# Variablen
#

#DATE=$(date +%F_%H%M%S)
DATE=$(date +%F_%H%M)
UPDATELOGDIR="/var/www/log/update"
EasyPortalLog="/var/www/log/easyportal.log"


#############################################################################
# Funktionen
#

function mklogdir {
  mkdir $UPDATELOGDIR
}

function update {
   echo "'apt-get update' ausgeführt am "$DATE > $UPDATELOGDIR/update.log
   echo "" >> $UPDATELOGDIR/update.log
   sudo apt-get update >> $UPDATELOGDIR/update.log
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get update" >> $EasyPortalLog
   echo "---" >> $UPDATELOGDIR/update.log
}

function upgrade_s {
   echo "'apt-get -s upgrade' ausgeführt am "$DATE > $UPDATELOGDIR/upgrade_s.log
   echo "" >> $UPDATELOGDIR/upgrade_s.log
   sudo apt-get -s upgrade >> $UPDATELOGDIR/upgrade_s.log
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get -s upgrade" >> $EasyPortalLog
   echo "---" >> $UPDATELOGDIR/upgrade_s.log
}

function upgrade {
   stopvdr
   echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> $EasyPortalLog 
   echo "'sudo apt-get upgrade' ausgeführt am "$DATE > $UPDATELOGDIR/upgrade.log
   echo "" >> $UPDATELOGDIR/upgrade.log
   sudo apt-get upgrade >> $UPDATELOGDIR.log
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get -n upgrade" >> $EasyPortalLog
   echo "---" >> $UPDATELOGDIR/upgrade.log
   echo "Am "$DATE" wurde ein 'upgrade' durchgeführt<br/><br/>Bitte Simulation erneut starten." > $UPDATELOGDIR/upgrade_s.log
   startvdr
   echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> $EasyPortalLog
}

function dist_upgrade_s {
   echo "'sudo apt-get -s dist-upgrade' ausgeführt am "$DATE > $UPDATELOGDIR/dist_upgrade_s.log
   echo "" >> $UPDATELOGDIR/dist_upgrade_s.log
   sudo apt-get -s dist-upgrade >> $UPDATELOGDIR/dist_upgrade_s.log
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get -s dist-upgrade" >> $EasyPortalLog
   echo "---" >> $UPDATELOGDIR/dist_upgrade_s.log
}

function dist_upgrade {
   stopvdr
   echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> $EasyPortalLog
   echo "'sudo apt-get -y dist-upgrade' ausgeführt am "$DATE > $UPDATELOGDIR/dist_upgrade.log
   echo "" >> $UPDATELOGDIR/dist_upgrade.log
   sudo apt-get -y dist-upgrade >> $UPDATELOGDIR/dist_upgrade.log
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get -n dist-upgrade" >> $EasyPortalLog
   echo "---" >> $UPDATELOGDIR/dist_upgrade.log
   echo "Am "$DATE" wurde ein 'dist-upgrade' durchgeführt<br/><br/>Bitte Simulation erneut starten." > $UPDATELOGDIR/dist-upgrade_s.log
   startvdr
   echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> $EasyPortalLog
}

function install {
   sudo apt-get install $1 --yes
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get install $1 --yes" >> $EasyPortalLog
}

function remove {
   sudo apt-get remove $1 --yes
   echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo apt-get remove $1 --yes" >> $EasyPortalLog
}



##########################################################################################

case $1 in

    mklogdir) 
        mklogdir
    ;;

    update) 
        update
    ;;

    upgrade_s) 
        upgrade_s
    ;;

    upgrade) 
        upgrade
    ;;

    dist_upgrade_s) 
        dist_upgrade_s
    ;;

    dist_upgrade) 
        dist_upgrade
    ;;

    install) 
        install $2
    ;;

    remove) 
        remove $2
    ;;

esac