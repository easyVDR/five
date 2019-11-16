#!/bin/bash
#
# $Id$
#
# image_pregen.sh
#
# call with file or dir to pregenerate the file or dir
# if you set first param. to -at, the script will use at to start a job for
# atd, so it runs asychonous in the background
# 
# by Onno Kreuzinger, onno_AT_kreuzinger_DOT_biz
# 2004-06-27    a.brachold - remove nocopy method
# 2004-08-14    a.brachold - update for imageplugin.sh version 0.2.0
#
[ "z$DEBUG" = "zyes" ] && set -xv

[ "z$1" = "z" ] && echo "start atleast with file or dir as argument, to pregenerate that file or dir." && exit 1
[ "z$1" = "z-at" ] && echo "starting at job mode.." && echo "$0 $2 $3"|at now && exit 0
[ "z$2" = "z-at" ] && echo "starting at job mode.." && echo "$0 $1 $3"|at now && exit 0
[ "z$1" = "z-follow" ] && FOLLOWMODE="true" && shift
[ "z$2" != "z" ] && shift

CONVERT_SCRIPT="imageplugin.sh"
CACHEFOLDER=${CACHEFOLDER:-"/tmp/image"}

[ ! -e $CONVERT_SCRIPT ] && echo "Can't find our worker $CONVERT_SCRIPT, please adjust path or variable CONVERT_SCRIPT" && exit 1

if [ -d "$1" ]
  then
	if [ "z$FOLLOWMODE" = "ztrue" ]
	  then
		echo "$0 : recursive dir mode "$1""
		find "$1" -follow -type f \
		 -name "*\.[jJbBtTpPmM][pPmMiInN][gGpPeEfFmM]" \
		 -exec $CONVERT_SCRIPT "{}" "$CACHEFOLDER{}.pnm" 688 544 0 0 0 \; 2> /dev/null
	  else
		echo "$0 : dir mode "$1""
		find "$1" -maxdepth 1 -follow -type f \
		 -name "*\.[jJbBtTpPmM][pPmMiInN][gGpPeEfFmM]" \
		 -exec $CONVERT_SCRIPT "{}" "$CACHEFOLDER{}.pnm" 688 544 0 0 0 \; 2> /dev/null
	fi
elif [ -e "$1" ]
  then
	echo "$0 : single file mode "$1""
	$CONVERT_SCRIPT "$1" "$CACHEFOLDER$1.pnm" 688 544 0 0 0 2> /dev/null
else
    echo -e "$1 is no file or directory\n syntax: $0 [-at] [-follow] <file|folder>"
fi
echo "done"
