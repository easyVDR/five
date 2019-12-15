#!/bin/bash

#############################################################################
# easyportal.sh v0.2
# by nogood, Steevee
#
# kann mit folgenden Parameter aufgerufen werden:
# ---------------------------------------------------------------------------
# easyportal.sh easyinfo
#   ruft das Script easyInfo auf und gibt das Ergebnis auf StdOut aus
# ---------------------------------------------------------------------------
# easyportal.sh del /pfad/zu/datei
#   löscht die angegebene Datei und erstellst sie neu
# ---------------------------------------------------------------------------
# easyportal.sh chk pluginname
#   prüft ob ein Pluginfile existiert, gibt true oder false zurück
# ---------------------------------------------------------------------------
# easyportal.sh on pluginname
#   Aktiviert das Plugin in der vdr-setup.xml und sysconfig
# ---------------------------------------------------------------------------
# easyportal.sh off pluginname
#   Deaktiviert das Plugin in der vdr-setup.xml und sysconfig
# ---------------------------------------------------------------------------
# easyportal.sh lst
#   gibt eine Liste aller verfügbaren Plugins zurück
# ---------------------------------------------------------------------------
# easyportal.sh ver
#   gibt die Aktuell installierte easyVDR-Version zurück
# ---------------------------------------------------------------------------
# easyportal.sh startvdr
#   Startet den VDR
# ---------------------------------------------------------------------------
# easyportal.sh stopvdr
#   Stoppt den VDR
# ---------------------------------------------------------------------------
# easyportal.sh help
#   zeigt eine kurze Beschreibung an
# ---------------------------------------------------------------------------
# easyportal.sh hddtemp <device>
#   zeigt die Temperatur der HDD an
# ---------------------------------------------------------------------------
# easyportal.sh hddname <device>
#   ermittelt mit hwinfo das Plattenmodell
# ---------------------------------------------------------------------------
# easyportal.sh uptime
#   zeigt die Uptime des Systems an
# ---------------------------------------------------------------------------
# easyportal.sh date
#   zeigt das Datum und die Systemzeit an
# ---------------------------------------------------------------------------
# easyportal.sh timer
#   zeigt die Timer an
# ---------------------------------------------------------------------------
# easyportal.sh vdruptime
#   uptime des VDR ermitteln
# ---------------------------------------------------------------------------
# easyportal.sh ssh
#   startet SSHd neu
#----------------------------------------------------------------------------
#       easyportal.sh port
#               ermittelt Port von Live
#----------------------------------------------------------------------------
#       easyportal.sh setsysconfig
#               setzt wert in sysconfig
# ---------------------------------------------------------------------------
# easyportal.sh chanact
#   aktiviert neue Kanalliste
#############################################################################
# Includes
#

. /usr/lib/vdr/easyvdr-config-loader
. /usr/lib/vdr/functions/easyvdr-functions-lib
. /usr/share/easyvdr/easyvdr-version

#############################################################################
# Variablen
#

EasyPortalShVersion="0.2"
EasyPortalLog="/var/www/log/easyportal.log"


#############################################################################
# Funktionen
#

# VDR API-Ermitteln #########################################################
function getApi {
  API=`/usr/bin/vdr -V | grep "The Video Disk Recorder" | cut -d" " -f2 | tr -d "()" | cut -d "/" -f2`
  #API="2.0.6"
  echo $API
}

# Installations-ISO ermitteln #########################################################
function getIso {
  ISO=`cat /etc/isoinfo`
  echo $ISO
}

# Kernelversion ermitteln #########################################################
function getKernel {
  KERNEL=`uname -r`
  echo $KERNEL
}

# Aktuell Installierte Version ermitteln ####################################
function getVersion {
  EASYVDRSUBVERS="${EASYVDRSUBVERS##*.}"
  echo $EASYVDRBASEVERS$EASYVDRSUBVERS
}

# Plugins auflisten #########################################################
function getPlugins {
  getApi
  grep "<plugin name" $PLUGIN_CFG_DIR/setup/vdr-setup.xml | grep -v 'protected="yes"' | sort | while read a; do
    a=${a#<plugin}
    a=${a%/>}
    PL=$(echo $a | awk -F\" {'print $2'})
    if [ -f $PLUGIN_DIR"/libvdr-"$PL".so."$API ] ; then
      echo $a
    fi
  done
}

# Kanalliste aktivieren #######################################################
function chanact {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> /var/www/log/easyportal.log 
  /usr/bin/stopvdr >/dev/null && sleep 5
  echo "$(date '+%d.%m.%Y %H:%M:%S') - Kanalliste aktivieren: $1" >> $EasyPortalLog 
  rm /var/lib/vdr/channels.conf
  ln -s /var/lib/vdr/channels/$1 /var/lib/vdr/channels.conf
  chown vdr:vdr /var/lib/vdr/channels.conf
  restart easyvdr-desktop >/dev/null && sleep 5
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> $EasyPortalLog 
  /usr/bin/startvdr >/dev/null && sleep 5
echo $1
}

# Plugin Deaktivieren #######################################################
function PluginOff {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> $EasyPortalLog 
  /usr/bin/stopvdr >/dev/null && sleep 5
  echo "$(date '+%d.%m.%Y %H:%M:%S') - Plugin deaktiviert: $1" >> $EasyPortalLog 
  DeactivatePlugin "$1"
  cp $CFG_DIR/sysconfig $CFG_DIR/sysconfig.$(date '+%d%m%Y%H%M')
  echo $?
  echo $(date '+%d.%m.%Y %H:%M')\;$1\;"deaktiviert" >> $2
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> $EasyPortalLog 
  /usr/bin/startvdr >/dev/null && sleep 5
}

# Plugin Deaktivieren (ohne start/stop vdr) #######################################################
function PluginOff_2 {
  if ( /usr/bin/svdrpsend VOLU | grep -q "closing connection" ); then 
    echo "$(date '+%d.%m.%Y %H:%M:%S') - Warnung! Plugin $1 deaktiviert, obwohl VDR noch läuft" >> $EasyPortalLog 
  else
    echo "$(date '+%d.%m.%Y %H:%M:%S') - Plugin deaktiviert: $1" >> $EasyPortalLog 
  fi
  DeactivatePlugin "$1"
  cp $CFG_DIR/sysconfig $CFG_DIR/sysconfig.$(date '+%d%m%Y%H%M')
  echo $?
  echo $(date '+%d.%m.%Y %H:%M')\;$1\;"deaktiviert" >> $2
}

# Plugin Aktivieren #########################################################
function PluginOn {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> $EasyPortalLog 
    /usr/bin/stopvdr >/dev/null && sleep 5
    cp $CFG_DIR/sysconfig $CFG_DIR/sysconfig.$(date '+%d%m%Y%H%M')
    ActivatePlugin "$1"
  echo $?
  echo $(date '+%d.%m.%Y %H:%M')\;$1\;"aktiviert" >> $2

  ### Pluginliste in der sysconfig wieder sortieren
  #loescht in der vdr-setup.xml alle kommentare
  sed -i '{/^-/d;/^<!--/d}' $PLUGIN_CFG_DIR/setup/vdr-setup.xml
  # Aktive Plugins in der vdr-setup.xml in Liste:
  ActivatedPluginsInVDRSetupXML=$(grep -w -e "<plugin name=.*active=\"yes\"" $PLUGIN_CFG_DIR/setup/vdr-setup.xml | sed -e 's/.*<plugin name=\"//' | sed -e 's/\".*$//')
  # Liste invertieren
  for i in $ActivatedPluginsInVDRSetupXML; do
    Plugins_to_activate=$i" "$Plugins_to_activate
  done
    # PluginListe in der sysconfig löschen
  sed -i 's/^PLUGINLIST=.*$/PLUGINLIST=\"\"/g' $CFG_DIR/sysconfig
  # Plugins wieder aktivieren
  echo "$(date '+%d.%m.%Y %H:%M:%S') - Plugins aktiviert: $1" >> $EasyPortalLog 
  ActivatePlugin $Plugins_to_activate
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> $EasyPortalLog 
  /usr/bin/startvdr >/dev/null && sleep 5
}

# Plugin Aktivieren (ohne start/stop vdr) #########################################################
function PluginOn_2 {
  if ( /usr/bin/svdrpsend VOLU | grep -q "closing connection" ); then 
    echo "$(date '+%d.%m.%Y %H:%M:%S') - Warnung! Plugin $1 aktiviert, obwohl VDR noch läuft" >> $EasyPortalLog 
  else
    echo "$(date '+%d.%m.%Y %H:%M:%S') - Plugin aktiviert: $1" >> $EasyPortalLog 
  fi
    cp $CFG_DIR/sysconfig $CFG_DIR/sysconfig.$(date '+%d%m%Y%H%M')
    ActivatePlugin "$1"
  echo $?
  echo $(date '+%d.%m.%Y %H:%M')\;$1\;"aktiviert" >> $2

  ### Pluginliste in der sysconfig wieder sortieren
  #loescht in der vdr-setup.xml alle kommentare
  sed -i '{/^-/d;/^<!--/d}' $PLUGIN_CFG_DIR/setup/vdr-setup.xml
  # Aktive Plugins in der vdr-setup.xml in Liste:
  ActivatedPluginsInVDRSetupXML=$(grep -w -e "<plugin name=.*active=\"yes\"" $PLUGIN_CFG_DIR/setup/vdr-setup.xml | sed -e 's/.*<plugin name=\"//' | sed -e 's/\".*$//')
  # Liste invertieren
  for i in $ActivatedPluginsInVDRSetupXML; do
    Plugins_to_activate=$i" "$Plugins_to_activate
  done
    # PluginListe in der sysconfig löschen
  sed -i 's/^PLUGINLIST=.*$/PLUGINLIST=\"\"/g' $CFG_DIR/sysconfig
  # Plugins wieder aktivieren
  ActivatePlugin $Plugins_to_activate
}


# Prüfen ob Plugin vorhanden ist ############################################
function PluginExist {
  getApi
  echo $PLUGIN_DIR"/libvdr-"$1".so."$API
  if [ -f $PLUGIN_DIR"/libvdr-"$1".so."$API ] ; then
    echo true
  else
    echo false
  fi
}

# Datei löschen #############################################################
function DeleteFile {
  if [ -f "$1" ] ; then
    $(rm $1)
    $(touch $1)
  fi
}

# SSH neu starten ausführen ########################################################
function ssh {
  /etc/init.d/ssh restart
}

# Live-Port ermitteln ########################################
function port {
  dynport=`grep PLUGINLIST $CFG_DIR/sysconfig | awk -F'-Plive' '{ print substr($2,5,4)}'`
  sed -i 's/live;Live.*/live;Live;'$dynport'/' /var/www/includes/home_dienste_var.inc
}

# VDR Starten ###############################################################
function startVdr {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr starten" >> $EasyPortalLog 
  /usr/bin/startvdr >/dev/null && sleep 5
}

# VDR Stoppen ###############################################################
function stopVdr {
  echo "$(date '+%d.%m.%Y %H:%M:%S') - vdr stoppen" >> $EasyPortalLog 
  /usr/bin/stopvdr >/dev/null && sleep 5
}

# hddtemp ###################################################################
function HddTemp {
  hddtemp $1 -q | awk -F: {'print $3'}
}

# hddname ###################################################################
function hddname {
  hwinfo --disk --short | grep $1 | awk -F$1 {'print $2'}
}

# Uptime ####################################################################
function SysUptime {
  ruptime="$(uptime)"
  if $(echo $ruptime | grep -E "days|day" >/dev/null); then
    TAG=$(echo $ruptime | awk '{print $3}')
    STD=$(echo $ruptime | awk '{print $5}' | awk -F: '{print $1}')
    MIN=$(echo $ruptime | awk '{print $5}' | awk -F: '{print $2}' | sed s/,//g) 
    x="$TAG,$STD,$MIN";
  else
    if $(echo $ruptime | grep -E "min" >/dev/null) ; then
      x=$(echo $ruptime | sed s/,//g | awk '{print $3}' | awk '{print "0,0,"$1}')
    else
      x=$(echo $ruptime | sed s/,//g | awk '{print $3}' | awk -F: '{print "0,"$1","$2}')
    fi    
  fi
  echo $x
}

# date ######################################################################
function eDate {
  date
}

# Timer #####################################################################
function timer {
  cat $CFG_DIR/timers.conf | awk -F: {'print $3";;"$4";;"$5";;"$8'} | sort
  #cat $CFG_DIR/timers.conf | sort
}

# VDR-Uptime ################################################################
function vdruptime {
  if [ -f "/var/run/easyvdr-loaded-modules" ] ; then
    stat -c%Z /var/run/easyvdr-loaded-modules
  fi
}

# setsysconfig ##############################################################
function setsysconfig () {
  cp $CFG_DIR/sysconfig $CFG_DIR/sysconfig.$(date '+%d%m%Y%H%M')
  local ConfigVarName="$1"
  shift
  local ConfigValue="\"$*\""

  echo "$(date '+%d.%m.%Y %H:%M:%S') - Setsysconfig: $ConfigVarName=$ConfigValue" >> $EasyPortalLog 

  if [ $(grep -w -e "$ConfigVarName=" $CFG_DIR/sysconfig &>/dev/null; echo $?) = "1"  ]
  then
    echo "$ConfigVarName=$ConfigValue" >> $CFG_DIR/sysconfig
  else
    ConfigValue=${ConfigValue//\//\\/}
    sed -i 's/^'$ConfigVarName'=.*$/'$ConfigVarName'='"$ConfigValue"'/' $CFG_DIR/sysconfig
  fi
}

function move {
    chown vdr:vdr /var/www/upload/*
    mv "$2" "$3"
}

function mvchannel {
    mv /var/www/upload/* /var/lib/vdr/channels/
    chown vdr:vdr /var/lib/vdr/channels/*
}

function vdrdate {
    date +'%Y-%m-%d %H:%M'
}

function schreibZeile {
    echo $1 >> $2
}

function ownervdr {
    chown vdr:vdr $1
}

function program_changer_variablen {
    cat /usr/share/easyvdr/program-changer/program-changer-easy-portal.sh |grep ^name_button > /var/www/tmp/temp.txt
    cat /usr/share/easyvdr/program-changer/program-changer-easy-portal.sh |grep ^picture_button >> /var/www/tmp/temp.txt
    # am Zeilenafang ein '$'
    sed -i 's/^/$/' /var/www/tmp/temp.txt
    # nach dem Gleicheitszeichen ein '"'
    sed -i 's/=/="/' /var/www/tmp/temp.txt
    # am Zeilenende ein '";'
    sed -i 's/$/";/g' /var/www/tmp/temp.txt
    # php -Tags am Anfang und Ende der Datei
    echo "<?php" > /var/www/includes/variablen_pchanger.php
    cat /var/www/tmp/temp.txt >> /var/www/includes/variablen_pchanger.php
    echo "?>" >> /var/www/includes/variablen_pchanger.php
    rm /var/www/tmp/temp.txt
    echo "$(date '+%d.%m.%Y %H:%M:%S') - Program-Changer Konfiguration neu eingelesen" >> $EasyPortalLog 
}

# Download eplists ##########################################################
function dl_eplists {
    cd tmp
    wget --no-check-certificate https://www.eplists.de/eplists_full_utf8.cgi?action=dutf
    mv eplists_full_utf8.cgi?action=dutf eplists_full_utf8.tgz
    tar -xvzf eplists_full_utf8.tgz -C $1
    chown -R vdr:vdr /var/cache/eplists
    rm -r /var/www/tmp/*
    echo "$(date '+%d.%m.%Y %H:%M:%S') - eplists wurden runtergeladen" >> $EasyPortalLog 
}

function rootpw {
    echo "root:${1}" | chpasswd
}

# help ######################################################################
function hilfe {
  echo ""
  echo "easyportal.sh $EasyPortalShVersion"
  echo ""
  echo "Options:"
  echo ""
  echo "    easyinfo                 ruft das Script easyInfo auf und gibt das Ergebnis auf StdOut aus"
  echo "    del <datei>              löscht die angegebene Datei und erstellst sie neu"
  echo "    rm <dateialt> <dateineu> benennt die angegebene Datei um"
  echo "    chk <pluginname>         prüft ob ein Pluginfile existiert, gibt true oder false zurück"
  echo "    on <pluginname>          Aktiviert das Plugin in der vdr-setup.xml und sysconfig"
  echo "    off <pluginname>         Deaktiviert das Plugin in der vdr-setup.xml und sysconfig"
  echo "    lst                      gibt eine Liste aller verfügbaren Plugins zurück"
  echo "    kernel                   gibt die Aktuell installierte Kernel-Version zurück"
  echo "    ver                      gibt die Aktuell installierte easyVDR-Version zurück"
  echo "    vdrver                   gibt die Aktuell installierte easyVDR-Version zurück"
  echo "    upd                      Prüft auf Updates und gibt sie aus"
  echo "    startvdr                 Startet den VDR"
  echo "    stopvdr                  Stoppt den VDR"
  echo "    hddtemp <device>         Zeigt die Temperatur der angegebenen Festplatte an"
  echo "    hddname <device>         Zeigt das Festplattenmodell an"
  echo "    uptime                   Zeigt die Uptime des System an"
  echo "    vdruptime                Zeigt das Datum des VDR-Starts an"
  echo "    ssh                      Startet SSHd neu"
  echo "    chanact                  Akiviert neue Kanalliste"
  echo "    setsysconfig <VarName> <value>  Setzt Wert in sysconfig";
  echo "    help                     zeigt diese Beschreibung an"
  echo ""
}

#############################################################################
# Sciptablauf
#

case $1 in

  del) # Log Datei löschen
    DeleteFile $2
  ;;

  chk) # Prüfen ob ein Plugin vorhanden ist 
    PluginExist $2
  ;;

  on) # Plugin Aktivieren
    PluginOn $2 $3
  ;;
  on_2) # Plugin Aktivieren (ohne stop/start)
    PluginOn_2 $2 $3
  ;;

  off) # Plugin Deaktivieren
    PluginOff $2 $3
  ;;

  off_2) # Plugin Deaktivieren (ohne stop/start)
    PluginOff_2 $2 $3
  ;;

  lst) # Plugins Auflisten
    getPlugins
  ;;

  ver) # Installierte easyVDR-Version
    getVersion
  ;;

  chanact) # aktiviert Kanalliste
    chanact $2
  ;;

  ssh) # Startet SSHd neu
    ssh
  ;;

  port) # Liveport ermitteln
    port
  ;;

  kernel) # Kernel ermitteln
    getKernel
  ;;

  vdrver) # VDR Version
    getApi
  ;;

  isover) # ISO ermitteln
    getIso
  ;;
  
  startvdr) # VDR Starten
    startVdr
  ;;

  stopvdr) # VDR Stoppen
    stopVdr
  ;;

  hddtemp) # Festplattentemp.
    HddTemp $2
  ;;

  hddname) # Festplattenmodell
    hddname $2
  ;;

  uptime) # Uptime
    SysUptime
  ;;

  date) # Datum
    eDate
  ;;

  timer) # Timer
    timer
  ;;

  vdruptime)
    vdruptime
  ;;

  vdrdate)
    vdrdate
  ;;

  setsysconfig) 
    setsysconfig $2 $3 $4 $5 $6 $7 $8 $9
    ;;
    
  mvchannel) 
    mvchannel $2 
  ;;

  move) 
    move $2 $3
  ;;
  
  schreibZeile) 
    schreibZeile $2 
  ;;

  ownervdr) 
    ownervdr $2 
  ;;

  program_changer_variablen)
    program_changer_variablen
  ;;
  
  dl_eplists)
    dl_eplists $2
  ;;

  rootpw)
    rootpw $2
  ;;
  
  help) # Hilfe anzeigen
    hilfe
  ;;

  *) # Ungültige eingabe
    hilfe
    exit 1
  ;;

esac
