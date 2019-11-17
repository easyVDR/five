#!/bin/sh

CONFIG=$6

. $CONFIG/data/musiccmds.conf

$SVDR_CMD MESG 'Brennvorgang gestartet'
$RM_FILE_CMD $AUDIO_TMP_DIR/*.*
$NICE_CMD $AUDIO_CNV_CMD -s clean -d $BURN_DEVICE -c $CDRDAO_BURN_DRIVER -t $AUDIO_TMP_DIR $CONFIG/playlists/burnlist.m3u
$SVDR_CMD MESG 'Brennvorgang abgeschlossen'

