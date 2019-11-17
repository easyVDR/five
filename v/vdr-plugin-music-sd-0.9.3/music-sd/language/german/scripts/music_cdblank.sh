#!/bin/sh

CONFIG=$6
. $CONFIG/data/musiccmds.conf

$SVDR_CMD MESG 'CD Löschvorgang gestartet'
$DEVICE_CLOSE
cdrdao blank $CDRDAO_BURN_DRIVER --device $CDRDAO_DEV --blank-mode minimal -v 9 --eject
$SVDR_CMD MESG 'CD Löschvorgang beendet'
$DEVICE_CLOSE
