#!/bin/bash
#18.04.2015 V0.1 GB & Bleifuss2
#select skins for pchanger
#04.01.2019 modify easyVDRV4

LOGGER=" logger -t program-changer-enable-skin"
MYPChangerSkins="/usr/share/easyvdr/program-changer/pictures/skins"
MYPChangerPictures="/usr/share/easyvdr/program-changer/pictures"
PSkins=`ls -l $MYPChangerSkins | grep '^d' | awk '{print $9}'`
$LOGGER "Skin Auswahl gestartet"
for AuswahlSkin in $PSkins
do
/usr/bin/yad --center --title="Program Changer Skin" \
--text="Program-Changer-"${AuswahlSkin}"-skin mit \"Auswahl\" waehlen,\noder mit \"weiter\" zum naechsten Skin." \
--button="Auswahl:0" \
--button="Weiter:1" \
--image=$MYPChangerSkins/${AuswahlSkin}.png
ButtonAuswahl=$?

if [ $ButtonAuswahl -eq "0" ] ;then
 $LOGGER "Skin Auswahl für $AuswahlSkin getroffen"
 cp $MYPChangerSkins/$AuswahlSkin/* $MYPChangerPictures/
 echo "#!/bin/bash"  >$MYPChangerSkins/set-skin
 echo "#Skript zum setzen des gewählten Skin" >>$MYPChangerSkins/set-skin
 echo "cp $MYPChangerSkins/$AuswahlSkin/* $MYPChangerPictures/" >>$MYPChangerSkins/set-skin
 touch /tmp/skin-config-ready
 exit 0
fi
if [ $ButtonAuswahl -eq "252" ] ;then
 $LOGGER "Skin Auswahl durch Benutzer abgebrochen"
 touch /tmp/skin-config-ready
 exit 0
fi
done
