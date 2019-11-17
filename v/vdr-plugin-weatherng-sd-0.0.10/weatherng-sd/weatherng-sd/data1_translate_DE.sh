#!/bin/sh
#
# weatherng-sd.sh 
# 
############################################################### 
# translate *.xml  de_DE                                      #
# weatherng-sd                                                #
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
# Change SETUPDIR if you need (Path to $VDRCONFIG [setup.conf]
SETUPDIR=/var/lib/vdr


## Locale 
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#en_US#de_DE#g' {} \;


### Country
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Germany#DE#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#GERMANY#DE#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#UNITED.*KINGDOM#UK#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#United.*Kingdom#UK#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#South.*Africa#SA#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#SOUTH.*AFRICA#SA#g' {} \;


## Day
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sunday#Sonntag#g' {} \;
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Monday#Montag#g' {} \;
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Tuesday#Dienstag#g' {} \;
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Wednesday#Mittwoch#g' {} \;
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Thursday#Donnerstag#g' {} \;
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Friday#Freitag#g' {} \;
find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Saturday#Samstag#g' {} \;


### Fair
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Fair#Heiter#g' {} \;
#
#
### Clear
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Clear#überwiegend Klar#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Sunny/Windy#überwiegend Sonnig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Sunny#überwiegend Sonnig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly.*Sunny/Windy#teils Sonnig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Clear/Windy#überwiegend Klar, Windig#g' {} \;
#
#
### Cloudy
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Cloudy#Bewölkt#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Overcast#Trüb#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Cloudy/Windy#Bewölkt, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Cloudy.*/.*Wind#Bewölkt, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Overcast/Windy#Bewölkt, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly.*Cloudy#teils Bewölkt#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Cloudy#überwiegend Bewölkt#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Cloudy/Windy#überwiegend Bewölkt, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly.*Cloudy/Windy#teils Bewölkt, Windig#g' {} \;
#
#
### Rain
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Chance.*Of.*Rain#Regen möglich#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Drizzle#leichter Niesel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Drizzle#Niesel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain#leichter Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*Early#anfangs leichter Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*Late#später leichter Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Light.*Rain#vormittags leichter Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Pm.*Rain#nachmittags Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Pm.*Light.*Rain#nachmittags leichter Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Pm.*Rain.*/.*Wind#nachmittags Regen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Wind.*Early#anfangs Regen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain#Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Heavy.*Rain#Regengüsse#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Drizzle/Windy#leicht Nieselig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Drizzle/Windy#Nieselig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain/Windy#leichter Regen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*And.*Windy#leicher Regen und Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain/Windy#Regnerisch, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Wind#Regnerisch, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Heavy.*Rain/Windy#Regengüsse, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*/.*Wind#leichter Regen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*/.*Fog#leichter Regen, Nebel#g' {} \;
#
#
### Showers
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*Shower#Regenschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*Shower.*and.*Windy#Regenschauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Shower#Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Showers#Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Showers.*/.*Wind#Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Showers.*Late#später Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Showers.*Early#anfangs Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*Shower#leichter Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#AM.*Shower#vormittags Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Showers#vormittags Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#PM.*Shower#nachmittags Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Pm.*Showers#nachmittags Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered.*Showers#vereinzelt Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Few.*Showers#geringe Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*Shower/Windy#starke Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Shower/Windy#Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*Shower/Windy#leichter Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#AM.*Shower/Windy#vormittags Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#PM.*Shower/Windy#nachmittags Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered.*Shower/Windy#vereinzelt Schauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*With.*Thunder#Leichter Regen mit Donner#g' {} \;
#
#
### Snow
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Flurries#Schneeböen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Flurries#leichte Schneeböen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow#leichter Schneefall#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Freezing.*Drizzle#leichter, überfrierender Niesel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow#Schnee#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Heavy.*Snow#starker Schneefall#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow.*Pellets#leichte Reifgraupel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow.*Grains#leichte Schneekörner#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Pellets#Reifgraupel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Ice.*Pellets#leichte Eisgraupel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Ice.*Crystals#Eiskristalle#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Ice.*Pellets#Eisgraupel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Wintery.*Weather#Winterlich#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Freezing.*Rain#leichter Frostregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Freezing.*Rain#überfrierender Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow/Freezing.*Rain#Schnee, überfrierender Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Flurries/Windy#Böen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Flurries/Windy#leichte Böen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow/Windy#leichter Schneefall, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow/Windy#Schnee, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Heavy.*Snow/Windy#starker Schneefall, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Drifting.*Snow#Schneetreiben#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow.*Pellets/Windy#leichte Reifgraupel, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Pellets/Windy#Reifgraupel, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Ice.*Pellets/Windy#leichte Eisgraupel, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Ice.*Pellets/Windy#Eisgraupel, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Freezing.*Rain/Windy#leichter überfrierender Regen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Freezing.*Rain/Windy#überfrierender Regen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*and.*Snow#Schneeregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*And.*Snow#Schneeregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Snow#Schneeregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow.*/.*Fog#leichter Schneefall,Nebelig#g' {} \;
#
#
### Snow Showers
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Showers#Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Shower#Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Snow.*Shower#leichte Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Few.*Snow.*Showers#wenige Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered.*Snow.*Showers#vereinzelt Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#PM.*Snow.*Showers#nachmittags Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Pm.*Snow.*Showers#nachmittags Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#AM.*Snow.*Showers#vormittags Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Snow.*Showers#vormittags Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Showers.*Late#später Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Showers.*Early#anfangs Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain/Snow.*Showers#Regen, Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Snow.*Showers#Regen, Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Snow.*Showers.*Early#anfangs Regen, Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Snow.*Showers.*Late#später Regen, Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*To.*Snow#in Schnee übergehender Regen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Showers/Windy#Schneeschauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Snow.*Shower.*/.*Wind#Schneeschauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#PM.*Snow.*Showers/Windy#nachmittags Schneeschauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#AM.*Snow.*Showers/Windy#vormittags Schneeschauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Rain.*/Snow.*Showers#vormittags Regen, Schneeschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain/Snow.*Showers/Windy#Regen, Schneeschauer, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Freezing.*Rain.*and.*Windy#überfrierender Regen, Windig#g' {} \;
#
#
### Thunder
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#T-Storms#Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#T-Storm#Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#T-Showers#Gewitterregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#T-Storms.*Early#anfangs Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered.*T-Storms#vereinzelt Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Isolated.*T-Storms#örtlich Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#T-Storms/Windy#Gewitter, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered.*T-Storms/Windy#vereinzelt Gewitter, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*T-Storms#vormittags Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Pm.*T-Storms#nachmittags Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Heavy.*T-Storm#schweres Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain/Thunder#Regen, Donner#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Rain.*/.*Thunder##g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Thunder#Donner#g' {} \;
#
#
### Dust
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Dust#Staub#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Blowing.*Dust#Staubverwehungen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Blowing.*Dust.*and.*Windy#Staubverwehungen, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Widespread.*Dust#verbreitet Staub#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Widespread.*Dust.*and.*Windy#verbreitet Staub und Wind#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Blowing.*Sand.*and.*Windy#Sandwehen und Wind#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Blowing.*Sand#Sandwehen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Low.*Drifting.*Sand#leichter Flugsand#g' {} \;
#
#
### Fog
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Fog#Nebel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Foggy#Nebelig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Haze#Dunst#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mist#Nebelschleier#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Fog/Windy#Nebel, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Haze/Windy#Dunst, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mist/Windy#Nebelschleier, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Fog#Leichter Nebel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Shallow.*Fog#Seichter Nebel#g' {} \;
#
#
### N/A
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Data.*Not.*Available#keine Daten#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#N/A#keine Daten#g' {} \;
#
#
### Missing Weather Conditions
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sleet#Graupel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sprinkles#Sprühregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Clear#Klar#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#clear#Klar#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sunny#Sonnig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sunny.*and.*Windy#Sonnig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Cloudy.*And.*Windy#Wolkig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Cloudy.*and.*Windy#Wolkig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Cloudy.*And.*Windy#überwiegend Wolkig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly.*Cloudy.*And.*Windy#teils Wolkig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Clouds.*Early.*/.*Clearing.*Late#anfangs Bewölkt, später Aufklarend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Showers.*in.*the.*Vicinity#Schauer in der Umgebung#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Showers.*In.*The.*Vicinity#Schauer in der Umgebung#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Thunder.*in.*the.*Vicinity#Gewitter in der Umgebung#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Thunder.*In.*The.*Vicinity#Gewitter in der Umgebung#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*with.*Thunder#Leichter Regen mit Donner#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sand.*And.*Windy#Sand, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Fair.*And.*Windy#Heiter, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Fair.*and.*Windy#Heiter, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly.*Cloudy.*and.*Windy#teils Wolkig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly.*Cloudy.*/.*Wind#teils Bewölkt, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Mostly.*Cloudy.*and.*Windy#überwiegend Bewölkt, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sunny.*And.*Windy#Sonnig, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Clear.*And.*Windy#Klar, Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#CALM#Windstill#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Fog.*/.*Pm.*Clouds#vormittags Nebel, nachmittags Wolken#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Fog.*/.*Pm.*Sun#vormittags Nebel, nachmittags Sonnig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Am.*Clouds.*/.*Pm.*Sun#vormittags Wolkig, nachmittags Sonne#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Freezing.*Fog#Gefrierender Nebel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Heavy.*Fog#dichter Nebel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered.*Clouds#vereinzelte Wolken#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Showers.*Rain#leichte Regenschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Light.*Rain.*Showers#leichte Regenschauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Windy#Windig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Partly#teils#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Schauers#Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#P.*Bewölkt#teils Bewölkt#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#M.*Bewölkt#überwiegend Bewölkt#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#T-Schauer#Gewitterregen#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Scattered#vereinzelte#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Sct#vereinzelte#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Gewitter.*Early#später Gewitter#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Few#geringe#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Clouds.*Early.*/.*Klaring.*Late#Wolkig, später Aufklarend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Schauer.*Early#anfangs Schauer#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Schauer.*Late#später Schauer#g' {} \;
#
#
### UV Intensity
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Moderate#Mäßig#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Very.*High#sehr Hoch#g' {} \;
#
#
### Pressure
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#steady#Gleichbleibend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#rising#Steigend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#rising.*rapidly#schnell Steigend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#falling#Fallend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#falling.*rapidly#schnell Fallend#g' {} \;
#
#
### Moon Phases
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Waxing.*Crescent#Zunehmend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#First.*Quarter#erstes Viertel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Waxing.*Gibbous#zunehmender Mond#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Full#Vollmond#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Waning.*Gibbous#abnehmender Mond#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Last.*Quarter#letztes Viertel#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#Waning.*Crescent#Abnehmend#g' {} \;
#find $SETUPDIR/plugins/weatherng-sd/data1.xml -type f -exec sed -i 's#New.*Moon#Neumond#g' {} \;


