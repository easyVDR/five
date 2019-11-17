#!/bin/bash
#V 0.1 18.10.2012 Bleifuss
#V 0.2 18.03.2013 Bleifuss
#V 0.2 10.11.2013 add logging Bleifuss
#V 0.2 04.09.2014 add default lirc.conf & steam scripts 
#V 0.3 01.03.2016 
###########

PCH_DIR="/usr/share/easyvdr/program-changer"                              #PChanger 
PCH_CFG="/etc/vdr/easyvdr/program-changer"
PCH_TEMPLATES="/usr/share/easyvdr/program-changer/templates"
PCH_CONF="/usr/share/easyvdr/program-changer/config"

WINDOWS_SCRIPTS="/media/easyvdr01/windows"

LOGGER=" logger -t make-program-changer-script.sh"

$LOGGER "start"

## Lirc Konfiguration vorhanden?
if [ ! -e $PCH_CFG/program-changer-lirc.conf ]; then
 $LOGGER "create program-changer-lirc.conf file"
 cp $PCH_TEMPLATES/program-changer-lirc-template.conf  $PCH_CFG/program-changer-lirc.conf
fi

## Tastatur Konfiguration vorhanden?
if [ ! -e $PCH_CFG/program-changer-keyboard.conf ]; then
 $LOGGER "create program-changer-keyboard.conf file"
 cp $PCH_TEMPLATES/program-changer-keyboard-template.conf  $PCH_CFG/program-changer-keyboard.conf
fi

## conf Verzeichnis vorhanden
if [ ! -d $PCH_CONF ]; then 
 mkdir $PCH_CONF
fi

## Steam Windows Skripts vorhanden?
#if [ ! -e $WINDOWS_SCRIPTS/steam_accounts.cmd ];then
# cp windows-scripts/steam_accounts_template.cmd  $WINDOWS_SCRIPTS/ #> /dev/null 2>&1
#fi
#
if [ ! -e $PCH_CONF/program-changer-main-script-parameter ] ;then
 cp $PCH_TEMPLATES/program-changer-main-script-parameter  $PCH_CONF/program-changer-main-script-parameter
fi

if [ ! -e $PCH_CONF/program-changer-main-script-parameter-easyvdr-portal ] ;then
 cp $PCH_TEMPLATES/program-changer-main-script-parameter-easyvdr-portal  $PCH_CONF/program-changer-main-script-parameter-easyvdr-portal
fi

if [ -e $PCH_DIR/program-changer.sh ]; then
 cp $PCH_CONF/program-changer-main-script-parameter $PCH_CONF/program-changer-main-script-parameter.backup
fi

## Schriftgröße löschen
if [ -e $PCH_CONF/program-changer.css ]; then
 rm $PCH_CONF/program-changer.css
fi

if [ -e $PCH_CONF/program-changer-setup.css ]; then
 rm $PCH_CONF/program-changer-setup.css
fi

if [ -e $PCH_TEMPLATES/program-changer-main-script-header -a -e $PCH_CONF/program-changer-main-script-parameter -a -e $PCH_TEMPLATES/program-changer-main-script ]; then
 echo "#!/bin/bash " > $PCH_DIR/program-changer.sh
 echo "#Diese Datei wird mit make-program-changer.sh generiert" >> $PCH_DIR/program-changer.sh
 echo "#Manuelle Änderungen werden überschrieben!!" >> $PCH_DIR/program-changer.sh
 cat $PCH_TEMPLATES/program-changer-main-script-header >> $PCH_DIR/program-changer.sh
 cat $PCH_CONF/program-changer-main-script-parameter >> $PCH_DIR/program-changer.sh
 cat $PCH_TEMPLATES/program-changer-main-script >>$PCH_DIR/program-changer.sh

 chmod ugo+x $PCH_DIR/program-changer.sh
else
 $LOGGER "missing file can't make program-changer.sh"
fi

## Skript für easyportal erstellen
if [ -e $PCH_TEMPLATES/program-changer-main-script-header -a -e $PCH_TEMPLATES/program-changer-main-script -a -e $PCH_TEMPLATES/program-changer-main-script ]; then
 echo "#!/bin/bash " > $PCH_DIR/program-changer-easy-portal.sh
 echo "#Diese Datei wird mit make-program-changer.sh generiert" >> $PCH_DIR/program-changer-easy-portal.sh
 echo "#Manuelle Änderungen werden überschrieben!!" >> $PCH_DIR/program-changer-easy-portal.sh
 cat $PCH_TEMPLATES/program-changer-main-script-header >> $PCH_DIR/program-changer-easy-portal.sh
 cat $PCH_CONF/program-changer-main-script-parameter-easyvdr-portal >> $PCH_DIR/program-changer-easy-portal.sh
 cat $PCH_TEMPLATES/program-changer-main-script >>$PCH_DIR/program-changer-easy-portal.sh

 chmod ugo+x $PCH_DIR/program-changer-easy-portal.sh

 if [ -e $PCH_DIR/lib/make-program-changer-easyportal-php.sh ]; then
  $LOGGER "write /var/www/includes/variablen_pchanger.php"
  $PCH_DIR/lib/make-program-changer-easyportal-php.sh
 fi
else
 $LOGGER "missing file can't make program-changer-easy-portal.sh"
fi

## Skin setzen
if [ -e $PCH_DIR/pictures/skins/set-skin ];then
 chmod ugo+x $PCH_DIR/pictures/skins/set-skin
 $LOGGER "set-skin: restore program-changer-skin"
 $PCH_DIR/pictures/skins/set-skin
fi

$LOGGER "end"
