#!/bin/sh

chgfile=ChangeLog
topdir=.
if [ ! -f ${topdir}/${chgfile} ]; then
  topdir=..
  if [ ! -f ${topdir}/${chgfile} ]; then
    echo "Can't find Changelog file"
    exit -1
  fi
fi

ver=$(head -1 ${topdir}/${chgfile} | cut -d " " -f 1 | tr -d '\n')
echo -n "$ver"
exit 0
