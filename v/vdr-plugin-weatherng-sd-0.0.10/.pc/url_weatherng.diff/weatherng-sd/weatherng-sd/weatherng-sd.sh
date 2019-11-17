#!/bin/sh
#
# weatherng-sd.sh 
# 
############################################################### 
# weatherng-sd - for sd full-featured card - easyvdr 1.0      #
# show weather data for 9 days                                #
# radarmap - 9 pictures preview                               #
# mango 05.08.2012                                            #
###############################################################
#
# source: vdr-plugin weatherng-sd
#

# To use this examples of urls , you must have the permission of owner to download and
# use them. Otherwise you must change the URLs. See README or README.DE.

# Change SETUPDIR if you need (Path to $VDRCONFIG [setup.conf].
SETUPDIR=/var/lib/vdr

# Wget Options
WGETOPT="-t 4 -T 20"

# Wetter ID SETUPDIR setup.conf
ID1=$1
ID2=$2
ID3=$3

case $LANG in
           de_*)
           MESG='Download abgeschlossen und Radarmaps aktualisiert!'
           URLS=(
                 #####################
				 # Radarmaps 580x348 #
				 #####################
				 # pic0 Aktuelle Temperaturen
                 http://modul.wetter.zdf.de/data/temp-brd-0.jpg
                 # pic1 Temperaturen Nacht
				 http://modul.wetter.zdf.de/data/temp-brd-1.jpg
                 # pic2 Temperaturen Morgen
				 http://modul.wetter.zdf.de/data/vor-brd-2.jpg
                 # pic3 Wetter Aussichten
				 http://modul.wetter.zdf.de/data/vor-brd-3.jpg
                 # pic4 Ozon
				 http://modul.wetter.zdf.de/data/ozon-brd-0.jpg
                 # pic5 UV Index
				 http://modul.wetter.zdf.de/data/uv-brd-0.jpg
				 # pic6 Pollen Nord
                 #http://modul.wetter.zdf.de/data/pollen-nord-0.jpg
                 # pic6 Pollen Westen
                 #http://modul.wetter.zdf.de/data/pollen-west-0.jpg
                 # pic6 Pollen Sued-West
                 http://modul.wetter.zdf.de/data/pollen-suedwest-0.jpg
				 # pic6 Pollen Osten
                 #http://modul.wetter.zdf.de/data/pollen-ost-0.jpg
				 # pic6 Pollen Sued-Osten
                 #http://modul.wetter.zdf.de/data/pollen-suedost-0.jpg
                 # pic7 Seewetter
				 http://modul.wetter.zdf.de/data/see-brd-0.jpg
                 # pic8 Wind
				 http://modul.wetter.zdf.de/data/wind-brd-0.jpg
				 # pic9 Wind Nacht
                 http://modul.wetter.zdf.de/data/wind-brd-1.jpg				 
                 # pic9 Waldbrandgefahr - ##Radarbild wird nur bis Oktober aktuallisiert##
				 #http://modul.wetter.zdf.de/data/sat-euri-01.jpg
                )
          ;;
esac

for i in $(seq 0 9) ; do
    eval ${URLS[$i]:+ wget $WGETOPT "${URLS[$i]}" -O "$SETUPDIR/plugins/weatherng-sd/pic${i}"}
done

wget $WGETOPT "http://xml.weather.com/weather/local/$ID1?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data1.xml"
wget $WGETOPT "http://xml.weather.com/weather/local/$ID2?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data2.xml"
wget $WGETOPT "http://xml.weather.com/weather/local/$ID3?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data3.xml"

##  translate data1.xml & data2.xml & data3.xml en_US to de_DE
sleep 0.2
cd $SETUPDIR/plugins/weatherng-sd
sh data1_translate_DE.sh &
sleep 0.2
sh data2_translate_DE.sh &
sleep 0.2
sh data3_translate_DE.sh &

echo $MESG

