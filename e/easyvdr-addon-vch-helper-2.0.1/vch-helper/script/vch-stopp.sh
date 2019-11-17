#!/bin/sh
# mango 10.05.2011
# easyvdr_0.9.XX, 1.0.X, 2.0.X
# Vodcatcher-Helper Stopp

telnet localhost 6666 <<EOF
shutdown
EOF

#### VDR-OSD-MESSAGE ####
OSD_MESSAGE="VCH-Helper beendet!"
/usr/bin/svdrpsend mesg  "$OSD_MESSAGE"
