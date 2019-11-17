#!/bin/bash

### Variablen
. /usr/lib/vdr/easyvdr-config-loader

logger "erstelle easyInfo"

errorlogdatei="/var/www/log/easyinfo/easyinfo.log.error"

eval tail -n 5000 /var/log/syslog > /var/www/log/easyinfo/easyinfo.log.syslog 2>&1

echo "-----------------------------------------------------" > $errorlogdatei
echo "-----              ERRORS in syslog             -----" >> $errorlogdatei
echo "-----------------------------------------------------" >> $errorlogdatei
echo "" >> $errorlogdatei
cat /var/www/log/easyinfo/easyinfo.log.syslog | grep --context=1 --ignore-case error >> $errorlogdatei
echo "" >> $errorlogdatei

echo "-----------------------------------------------------" >> $errorlogdatei
echo "-----              ERRORS in xorg.log           -----" >> $errorlogdatei
echo "-----------------------------------------------------" >> $errorlogdatei
echo "" >> $errorlogdatei
#cat /var/log/Xorg.0.log | grep --context=1 --ignore-case "(EE)" --invert-match "(EE) error" >> $errorlogdatei
echo "" >> $errorlogdatei

cat $errorlogdatei 2>&1

#rm $errorlogdatei
#rm /var/www/log/easyinfo/easyinfo.log.syslog
