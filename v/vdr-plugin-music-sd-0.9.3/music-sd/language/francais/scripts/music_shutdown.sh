#!/bin/sh

NETCAT="/usr/bin/nc"

echo "HITK Power" | $NETCAT -q 1 localhost 2001
