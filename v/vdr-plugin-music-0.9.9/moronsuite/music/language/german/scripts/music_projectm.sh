#!/bin/sh

CONFIG=$6
. $CONFIG/data/musiccmds.conf

echo "starting projectm : '$PROJECTMPATH $PROJECTMOPT'"
$PROJECTMPATH $PROJECTMOPT
