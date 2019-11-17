#!/bin/bash
# V1.0 17.04.2015 Steevee
# V1.0 01.03.2016 Bleifuss 

# Prüfen ob Verzeichniss existiert
if [[ ! -d /var/www/includes ]] ; then
 exit 0
fi

cat /usr/share/easyvdr/program-changer/config/program-changer-main-script-parameter-easyvdr-portal |grep ^name_button > temp.txt
cat /usr/share/easyvdr/program-changer/config/program-changer-main-script-parameter-easyvdr-portal |grep ^picture_button >> temp.txt
# am Zeilenanfang ein '$'
sed -i 's/^/$/' temp.txt
# nach dem Gleicheitszeichen ein '"'
sed -i 's/=/="/' temp.txt
# am Zeilenende ein '";'
sed -i 's/$/";/g' temp.txt

# php-Tag am Anfang
echo "<?php" > /var/www/includes/variablen_pchanger.php

# Kommentar
echo >> /var/www/includes/variablen_pchanger.php
echo "//Diese Datei wird mit make-program-changer.sh & make-program-changer-easyportal-php.sh generiert" >> /var/www/includes/variablen_pchanger.php
echo "//Manuelle Änderungen werden überschrieben!!" >> /var/www/includes/variablen_pchanger.php
echo >> /var/www/includes/variablen_pchanger.php

cat temp.txt >> /var/www/includes/variablen_pchanger.php
echo "?>" >> /var/www/includes/variablen_pchanger.php
rm temp.txt
