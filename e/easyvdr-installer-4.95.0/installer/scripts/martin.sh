#!/bin/bash
cd /tmp
rm -f ./live-test.sh
wget http://easy-vdr.de/~martin/live-test.sh
chmod +x ./live-test.sh
./live-test.sh

