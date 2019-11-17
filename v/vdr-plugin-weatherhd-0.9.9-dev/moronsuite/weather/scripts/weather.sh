#!/bin/sh
#
# weather.sh
#
# source: vdr-plugin weatherhd
#

# To use this examples of urls , you must have the permission of owner to download and
# use them. Otherwise you must change the URLs. See README or README.DE.

SETUPDIR=/var/lib/vdr
WNGPATH=$1
ID1=$2
ID2=$3
ID3=$4

KEY=""

# the xslt processor
XSLTCMD="/usr/bin/xsltproc"
# the wget
WGETCMD="/usr/bin/wget"
# fromdos (dos2unix)
FROMDOS="/usr/bin/fromdos"


# The XSLT to use when translating the response from accuweather.com
XSLT="$WNGPATH/scripts/accu.xslt"


ACCURL1="http://forecastfox.accuweather.com/adcbin/forecastfox/weather_data.asp?location=$ID1&metric=1&partner=forecastfox"
ACCURL2="http://forecastfox.accuweather.com/adcbin/forecastfox/weather_data.asp?location=$ID2&metric=1&partner=forecastfox"
ACCURL3="http://forecastfox.accuweather.com/adcbin/forecastfox/weather_data.asp?location=$ID3&metric=1&partner=forecastfox"

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

for i in $(seq 0 13) ; do
    eval ${URLS[$i]:+ wget $WGETOPT "${URLS[$i]}" -O "$SETUPDIR/plugins/moronsuite/weather/downloads/pic${i}"}
done


    $WGETCMD -q "$ACCURL1" -O "/tmp/rawdata1.xml"
    $XSLTCMD -o "$WNGPATH/downloads/data1.xml" "$XSLT" "/tmp/rawdata1.xml"
    $FROMDOS "/tmp/rawdata1.xml"

echo "$WGETCMD -q '$ACCURL1' -O '/tmp/rawdata1.xml'"

    $WGETCMD -q "$ACCURL2" -O "/tmp/rawdata2.xml"
    $XSLTCMD -o "$WNGPATH/downloads/data2.xml" "$XSLT" "/tmp/rawdata2.xml"
    $FROMDOS "/tmp/rawdata2.xml"

    $WGETCMD -q "$ACCURL3" -O "/tmp/rawdata3.xml"
    $XSLTCMD -o "$WNGPATH/downloads/data3.xml" "$XSLT" "/tmp/rawdata3.xml"
    $FROMDOS "/tmp/rawdata3.xml"

