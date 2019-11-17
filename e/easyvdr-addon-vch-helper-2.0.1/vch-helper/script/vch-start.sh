#!/bin/sh
# mango 10.05.2011
# easyvdr_0.9.XX, 1.0.X, 2.0.X
# Vodcatcher-Helper Start


cd /usr/share/vdr-vch-helper
./vch.sh &

#### VDR-OSD-MESSAGE ####
OSD_MESSAGE="VCH-Helper gestartet!"
/usr/bin/svdrpsend mesg  "$OSD_MESSAGE"

