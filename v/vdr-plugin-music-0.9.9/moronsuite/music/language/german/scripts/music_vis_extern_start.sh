#!/bin/sh

CONFIG=$1
. $CONFIG/data/musiccmds.conf

echo "starting external program : '$EXTERN_VIS_CMD $EXTERN_VIS_OPT'"
$EXTERN_VIS_CMD $EXTERN_VIS_OPT
