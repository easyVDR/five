#!/bin/bash
#13.04.2020 enable Terminal 8, set lightdm default


dpkg-reconfigure -u lightdm 

#Nur bis zur Konsole booten
systemctl set-default multi-user.target

TERMINAL=4
sleep 1
chvt $TERMINAL
echo "[100] start.sh v2.0"

function livemodus
{
echo "[110] Direct start..."
chvt $TERMINAL
echo "[113] checking for new packages..."
update_presetup
echo "[115] pre-setup upgraded..."

echo "[116] starting easyvdr-presetup-live..."
/usr/share/easyvdr/setup/easyvdr-presetup-live
echo "[117] starting easyvdr-setup..."
/usr/share/easyvdr/setup/easyvdr-setup
}

function setup
{
echo "[120] Direct setup..."
echo "[121] Starting setup on colsole 8"
#grep -q "m-proxy" /var/log/installer/syslog && cp /usr/share/easyvdr/installer/01proxy /etc/apt/apt.conf.d/
chvt $TERMINAL
echo "[123] checking for new packages..."
update_presetup
echo "[125] pre-setup upgraded..."

echo "[126] starting easyvdr-presetup..."
/usr/share/easyvdr/setup/easyvdr-presetup
echo "[127] starting easyvdr-setup..."
/usr/share/easyvdr/setup/easyvdr-setup
}

update_presetup(){
#cp /etc/apt/sources.list /etc/apt/sources.list.bkp
#sed -i 's/^\(.*\)ubuntu.com\(.*\)$/# \1ubuntu.com\2/g' /etc/apt/sources.list
#apt-get update
echo "[124] upgrading pre-setup inactive..."
#apt-get install --reinstall easyvdr-presetup
####only update for presetup
##apt-get -y -o Dpkg::Options::=--force-confnew dist-upgrade
#cp /etc/apt/sources.list.bkp /etc/apt/sources.list
}

partitioning(){
initctl start easyvdr-dialog-startx
}

update_basics(){
cp /etc/apt/sources.list /etc/apt/sources.list.bkp
sed -i 's/^\(.*\)ubuntu.com\(.*\)$/# \1ubuntu.com\2/g' /etc/apt/sources.list
apt-get update
echo "[124] upgrading pre-setup..."
apt-get install --reinstall easyvdr-presetup easyvdr-installer
cp /etc/apt/sources.list.bkp /etc/apt/sources.list
}





function test
{
echo "[139] Test installation"
rm /etc/init/lightdm.override
cd /usr/share/easyvdr/installer/
wget http://193.34.144.89/~martin/ea20-beta/set.sh.new > /dev/null 2>&1
mv set.sh.new set.sh > /dev/null 2>&1
    echo set.sh wird gestartet
chmod 777 set.sh
./set.sh
bash
}

function installation
{
echo "[130] Regular installation"
cd $PRESEEDDIR
cat 01_header 05_stable > ../easyvdr.seed
cd
#$PRESEED stable
partitioning
}



function alternative-install(){
echo "[170] Alternative-install"

##### rausnehmen !!!
# wird vom configloader gesetzt
#IMGDIR=/home/martin/src/tmp/installer-test/images/
cd $PRESEEDDIR
cat 01_header_alternative 05_stable > ../easyvdr.seed

/usr/bin/xinit /usr/bin/metacity -- :0 vt7 >/dev/null 2>&1  &
#GenImage lang tmp
GenImage640 lang tmp
chvt 7
DISPLAY=:0 /usr/bin/yad --title="Sprache"  --button="Deutsch" --button="English" --image=$IMGDIR/tmp.png  --timeout=30  --timeout-indicator=right
ret=$?
if [ $ret = 0 ] #Wenn de
then LANG="de"
#$PRESEED de
cat 02_de  >> ../easyvdr.seed

fi
if [ $ret = 1 ]
then LANG="en"
cat 02_en  >> ../easyvdr.seed
fi


#GenImage distro-$LANG tmp
GenImage640 distro-$LANG tmp

DISPLAY=:0 /usr/bin/yad --title="Status"  --button="Stable" --button="Testing" --button="Unstable" --image=$IMGDIR/tmp.png  --timeout=30  --timeout-indicator=right
ret=$?
if [ $ret = 0 ]
then echo "Stable"
#nothing todo
fi
if [ $ret = 1 ]
then echo "Testing"
cat 07_testing >> ../easyvdr.seed
echo "deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main" >> $SOURCES
update_basics
fi

if [ $ret = 2 ]
then echo "Unstable"
cat 07_testing 09_unstable >> ../easyvdr.seed
echo "deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main" >> $SOURCES
echo "deb http://ppa.launchpad.net/easyvdr-team/3-base-unstable/ubuntu trusty main" >> $SOURCES
update_basics
fi


killall xinit > /dev/null 2>&1
killall metacity > /dev/null 2>&1
sleep 3
killall xinit > /dev/null 2>&1

#sleep 8
partitioning
#start ubiquity.
}



#autopart)
#cat 11_autopart >> ../easyvdr.seed






#Preparation
echo "[101] Preparing..."
chvt $TERMINAL



function desktop
{
echo "[180] Desktop-Start"
rm /etc/init/lightdm.override
start lightdm
}


function boot-repair
{
echo "[190] Boot-Repair"
rm /etc/init/lightdm.override
start lightdm
sleep 8
boot-repair
}


#Preparation
echo "[101] Preparing..."
chvt $TERMINAL


. /usr/lib/vdr/functions/easyvdr-functions-lib
. /usr/lib/vdr/easyvdr-config-loader
IMGDIR=$INSTALLERDIR/images
PRESEEDDIR=$INSTALLERDIR/preseeds
SOURCES=/etc/apt/sources.list


rm /etc/X11/xorg.conf > /dev/null 2>&1

SHORTLOG="/tmp/HW-det_Tail-Mess.tmp"
touch $SHORTLOG > /dev/null 2>&1
chmod 777 $SHORTLOG

# Bildschirmschoner abschalten
xset -dpms >/dev/null 2>&1 || true
xset s off >/dev/null 2>&1 || true


#Main
FOUND="none"
grep -q "run-easyvdr-install"         /proc/cmdline    && FOUND="run_install"
grep -q "easyvdr-alternative-install" /proc/cmdline    && FOUND="alternative-install"
grep -q "easyvdr-test"                /proc/cmdline    && FOUND="test"
grep -q "easyvdr-live"                /proc/cmdline    && FOUND="livemodus"
grep -q "easyvdr-desktop"             /proc/cmdline    && FOUND="desktop"
grep -q "easyvdr-boot-repair"         /proc/cmdline    && FOUND="boot-repair"
#grep -q "m-proxy"                     /proc/cmdline    && cp /usr/share/easyvdr/installer/01proxy /etc/apt/apt.conf.d/



case $FOUND in
     none) setup ;;
     livemodus) livemodus ;;
     desktop) desktop ;;
     boot-repair)  boot-repair ;;
     run_install) installation  ;;
     test) test  ;;
     run_install-t) installation-t  ;;
     run_install-u) installation-u  ;;
     alternative-install)  alternative-install ;;
esac


