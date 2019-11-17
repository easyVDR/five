#!/bin/sh

CONFIG=$6
. $CONFIG/data/musiccmds.conf

echo "starting fische : '$FISCHEPATH $FISCHEOPT'"
$FISCHEPATH $FISCHEOPT
