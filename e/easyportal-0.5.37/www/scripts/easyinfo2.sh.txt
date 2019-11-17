#!/bin/bash

### Variablen
. /usr/lib/vdr/easyvdr-config-loader

echo "$(date '+%d.%m.%Y %H:%M:%S') - Easyinfo erstellt" >> /var/www/log/easyportal.log 

date1=$(date +%y-%m-%d_%H%M)
date2=$(date +%d.%m.%Y_%H:%M)
logger "erstelle easyInfo"

logdatei="/var/www/log/easyinfo/easyinfo_"$date1".log"
templogdatei="/var/www/log/easyinfo/easyinfo.log.temp"
errorlogdatei="/var/www/log/easyinfo/easyinfo.log.error"
#dbdatei="/var/www/includes/easyinfo/easyinfo.csv"
dbdatei="/var/www/databases/portal/easyinfo.txt"

#echo $logdatei
//echo $date2 > "$templogdatei"
//echo "" >> "$templogdatei"

$i=0
while read line
do
  if [ "$i" -gt "3" ]   # erst ab 4. Zeile der Datenbankdatei
  then  
    #echo $line 
    IFS="#"
    array=($line)
  #  echo ${array[0]}  #ID  
  #  echo ${array[1]}  #Befehl
  #  echo ${array[2]}  #Name
  #  echo ${array[3]}  #Kategorie
  #  echo ${array[4]}  #Farbe
  #  echo ${array[5]}  #Hilfe
    echo "#####################################################" >> "$templogdatei"
    echo "##### "${array[2]}"   " >> "$templogdatei"
    echo "#####################################################" >> "$templogdatei"
  #  echo "${array[1]}" >> $templogdatei
    echo "" >> "$templogdatei" 

    eval ${array[1]} >> $templogdatei 2>&1
    
    echo "" >> "$templogdatei"
  fi
  
  i=$(( $i + 1 ))
  
done < $dbdatei


eval tail -n 5000 /var/log/syslog > /var/www/log/easyinfo/easyinfo.log.syslog 2>&1

echo $date2 "- EasyPortal" $1 > $errorlogdatei
echo "" >> $errorlogdatei
echo "#####################################################" >> $errorlogdatei
echo "##### ERRORS IM LOGFILE   " >> $errorlogdatei
echo "#####################################################" >> $errorlogdatei
echo "" >> $errorlogdatei
# cat /var/www/log/easyinfo/easyinfo.log.temp | grep error >> $errorlogdatei

echo "-----------------------------------------------------" >> $errorlogdatei
echo "-----              ERRORS in syslog             -----" >> $errorlogdatei
echo "-----------------------------------------------------" >> $errorlogdatei
echo "" >> $errorlogdatei
cat /var/www/log/easyinfo/easyinfo.log.syslog | grep --context=1 --ignore-case error >> $errorlogdatei
echo "" >> $errorlogdatei

echo "-----------------------------------------------------" >> $errorlogdatei
echo "-----              ERRORS in xorg.log           -----" >> $errorlogdatei
echo "-----------------------------------------------------" >> $errorlogdatei
echo "" >> $errorlogdatei
cat /var/log/Xorg.0.log | grep --context=1 --ignore-case "(EE)" --invert-match "(EE) error" >> $errorlogdatei
echo "" >> $errorlogdatei


cat $errorlogdatei $templogdatei > $logdatei


# Fritzbox-Passwort aus EasyInfo löschen
ConfigVarName="fritzbox.EncodedPassword"
ConfigValue="!!! nicht ausgelesen !!!"
if [$(grep -w -e "$ConfigVarName = " $logdatei &>/dev/null; echo $?) = "1"  ]
then
  echo ""
  #echo "$ConfigVarName = $ConfigValue" >> $logdatei
else
  ConfigValue=${ConfigValue//\//\\/}
  sed -i 's/^'$ConfigVarName' = .*$/'$ConfigVarName' = '$ConfigValue'/' $logdatei
fi

# Weatherforecast Key aus EasyInfo löschen
ConfigVarName="weatherforecast.userapikey"
ConfigValue="!!! nicht ausgelesen !!!"
if [$(grep -w -e "$ConfigVarName = " $logdatei &>/dev/null; echo $?) = "1"  ]
then
  echo ""
  #echo "$ConfigVarName = $ConfigValue" >> $logdatei
else
  ConfigValue=${ConfigValue//\//\\/}
  sed -i 's/^'$ConfigVarName' = .*$/'$ConfigVarName' = '$ConfigValue'/' $logdatei
fi



rm /var/www/log/easyinfo/easyinfo.zip
zip -j /var/www/log/easyinfo/easyinfo.zip $logdatei

rm $errorlogdatei
rm $templogdatei
rm /var/www/log/easyinfo/easyinfo.log.syslog
rm /var/www/log/easyinfo/easyinfo.log.temp

# Link easyinfo.log auf erstelltes Easyinfo
rm /var/www/log/easyinfo/easyinfo.log
ln -s "$logdatei" /var/www/log/easyinfo/easyinfo.log 
chmod 755 /var/www/log/easyinfo/easyinfo.log 
chown vdr /var/www/log/easyinfo/easyinfo.log

