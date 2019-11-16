#!/bin/bash
#
#   startet das frontend für graphtft
#
#   05.11.2006 Frank Jung
#   29.05.2010 Captain_Beefheart ( geändert )
#   15.07.2011 SirNova (Anpassungen für 0.9)
#   11.07.2012 Captain_Beefheart nun wieder mit Warteschleife
#   => /usr/bin

SCR_NR=$1
GRATFT_FE="/usr/bin/graphtft-fe"
SETUPCONF="/var/lib/vdr/setup.conf"

if ( ! pidof graphtft-fe ); then
  while ( ! pidof Xorg )
    do
      sleep 0.1
    done
  sleep 2
  hoehe=$( grep graphtft.Height $SETUPCONF | cut -d = -f2)
  breite=$( grep graphtft.Width $SETUPCONF | cut -d = -f2)
  DISPLAY=:$SCR_NR $GRATFT_FE -h localhost -e 2 -n -W$breite -H$hoehe -f -r &
  exit 0
fi
