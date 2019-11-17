#!/bin/sh

CONFIG=$6
. $CONFIG/data/musiccmds.conf

echo "HITK Power" | $NETCAT -q 1 $HOST $SVDR_PORT
