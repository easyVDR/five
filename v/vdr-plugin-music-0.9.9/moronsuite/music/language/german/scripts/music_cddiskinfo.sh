#!/bin/sh

CONFIG=$6
. $CONFIG/data/musiccmds.conf

cdrdao disk-info $CDRDAO_BURN_DRIVER --device $CDRDAO_DEV
