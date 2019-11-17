#!/bin/sh
#
# weatherng-sd.sh 
# 
############################################################### 
# weatherng-sd - for sd full-featured card - easyvdr 1.0      #
# show weather data for 9 days                                #
# radarmap - 9 pictures preview                               #
#                                                             #
# mango 05.08.2012                                            #
#                                                             #
# added new url for xml data                                  #
# mango 19.11.2016                                            #
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
		         ##########################
                 # ZDF Radarmaps 1280x720 #
				 ##########################
                 # pic0 Aktuelle Temperaturen
				 http://wwwdyn.zdf.de/ext/weather/temp-brd-0.jpg
                 # pic1 Temperaturen Nacht
				 http://wwwdyn.zdf.de/ext/weather/temp-brd-1.jpg
                 # pic2 Temperaturen Morgen
				 http://wwwdyn.zdf.de/ext/weather/temp-brd-2.jpg
                 # pic3 Wetter Aussichten
				 http://wwwdyn.zdf.de/ext/weather/preview-brd-3.jpg
                 # pic4 Ozon
				 http://wwwdyn.zdf.de/ext/weather/ozone-brd-0.jpg
                 # pic5 UV Index
				 http://wwwdyn.zdf.de/ext/weather/uv-brd-0.jpg
                 # pic6 Pollen
				 http://wwwdyn.zdf.de/ext/weather/poll-brd-0.jpg
                 # pic7 Satellit Deutschland
				 http://wwwdyn.zdf.de/ext/weather/sat-brd-0.jpg
                 # pic8 Seewetter Morgen
				 http://wwwdyn.zdf.de/ext/weather/sea-brd-0.jpg
                 # pic9 Seewetter Heute Temp				 
				 http://wwwdyn.zdf.de/ext/weather/sea-brd-1.jpg
				 # pic10 Wind Heute
				 http://wwwdyn.zdf.de/ext/weather/wind-brd-0.jpg
				 # pic11 Wind Nacht
				 http://wwwdyn.zdf.de/ext/weather/wind-brd-1.jpg
				 # pic12 Wind Morgen
				 http://wwwdyn.zdf.de/ext/weather/wind-brd-2.jpg 				 
				 # pic13 Waldbrandgefahr nur bis Oktober
				 http://wwwdyn.zdf.de/ext/weather/forest-brd-0.jpg
                 )
          ;;
esac

for i in $(seq 0 9) ; do
    eval ${URLS[$i]:+ wget $WGETOPT "${URLS[$i]}" -O "$SETUPDIR/plugins/weatherng-sd/pic${i}"}
done

## old url 05.08.2012 ##
## wget $WGETOPT "http://xml.weather.com/weather/local/$ID1?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data1.xml"
## wget $WGETOPT "http://xml.weather.com/weather/local/$ID2?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data2.xml"
## wget $WGETOPT "http://xml.weather.com/weather/local/$ID3?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data3.xml"

## new url 14.11.2016 ##
wget $WGETOPT "http://wxdata.weather.com/wxdata/weather/local/$ID1?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data1.xml"
wget $WGETOPT "http://wxdata.weather.com/wxdata/weather/local/$ID2?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data2.xml"
wget $WGETOPT "http://wxdata.weather.com/wxdata/weather/local/$ID3?cc=*&unit=m&dayf=9" -O "$SETUPDIR/plugins/weatherng-sd/data3.xml"

##  translate data1.xml & data2.xml & data3.xml en_US to de_DE
sleep 0.5
cd $SETUPDIR/plugins/weatherng-sd
sh data1_translate_DE.sh &
sleep 0.5
sh data2_translate_DE.sh &
sleep 0.5
sh data3_translate_DE.sh &

echo $MESG

