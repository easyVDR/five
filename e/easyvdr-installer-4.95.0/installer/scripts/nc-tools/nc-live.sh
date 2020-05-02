#!/bin/bash
# by Martin and TVIA 12-14-12 for easyVDR
#
##diseqc for netceiver
rm /var/lib/vdr/diseqc.conf
F=/etc/vdr/diseqc.conf
echo "" >>$F
echo "A1 S19.2E 11725 V 0 [ e0 10 6f c0 00 00 ] " >>$F
echo "A1 S19.2E 12750 V 0 [ e0 10 6f c0 00 01 ] " >>$F
echo "A1 S19.2E 11725 H 0 [ e0 10 6f c0 00 02 ] " >>$F
echo "A1 S19.2E 12750 H 0 [ e0 10 6f c0 00 03 ] " >>$F
echo "A1 S13.0E 11725 V 0 [ e0 10 6f 82 00 00 ] " >>$F
echo "A1 S13.0E 12750 V 0 [ e0 10 6f 82 00 01 ] " >>$F
echo "A1 S13.0E 11725 H 0 [ e0 10 6f 82 00 02 ] " >>$F
echo "A1 S13.0E 12750 H 0 [ e0 10 6f 82 00 03 ] " >>$F
cd /var/lib/vdr
chmod 755 diseqc.conf

sleep 5

##vlan0.2 for netceiver
rm /etc/network/interfaces
F=/etc/network/interfaces
echo "" >>$F
echo "#Netceiver" >>$F
echo "auto eth0.2" >>$F
echo "iface eth0.2 inet manual" >>$F
echo "pre-up vconfig add eth0 2" >>$F
echo "post-up ifconfig eth0.2 up" >>$F
modprobe 8021q
sleep 4
ifup eth0.2

sleep 2

##netwoking restart
cd /
/etc/init.d/networking restart

sleep 2
stopvdr
sleep 3

##mcli plugin for netceiver
cd /var/lib/vdr
rm sysconfig
cp sysconfig.mcli sysconfig

sleep 2
startvdr