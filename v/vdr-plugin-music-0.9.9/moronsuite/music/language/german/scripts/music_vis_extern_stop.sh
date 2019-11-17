#!/bin/sh

CONFIG=$1
. $CONFIG/data/musiccmds.conf

#PROG_ID=`ps -A | grep $EXTERN_VIS_NAME | grep -v grep | awk '{print $1}'`


echo "stopping external program : '$KILL_VIS_CMD  $EXTERN_VIS_NAME'"
$KILL_VIS_CMD $EXTERN_VIS_NAME
