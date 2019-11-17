#!/bin/sh
svdrpsend.pl MESG 'CD Loeschvorgang gestartet'
eject -t
cdrdao blank --driver generic-mmc --device ATA:1,0,0 --blank-mode minimal -v 9 --eject
svdrpsend.pl MESG 'CD Loeschvorgang beendet'
play -v 5 /VDR/common/ready.wav
eject -t
