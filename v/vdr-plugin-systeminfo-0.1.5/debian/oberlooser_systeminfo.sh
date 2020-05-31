#!/bin/bash
# systeminfo.sh: external data collection script
# This file belongs to the VDR plugin systeminfo
#
# See the main source file 'systeminfo.c' for copyright information and
# how to reach the author.
#
# $Id$
#
# possible output formats:
# (blanks around tabs only for better reading)
# 1)   Name \t Value         	displays Name and Value
# 2)   Name \t Value1 \t Value2 displays Name, Value1 and Value2
# 3)   Name \t total used       displays an additional progress bar (percentage) after the values
# 4)   s \t Name \t ...         defines a static value, this line is only requested during the first cycle
#
# special keywords (they are replaced by the plugin with the actual value):
#      CPU%    CPU usage in percent
#
# test with: for i in $(seq 1 16); do systeminfo.sh $i;echo;done
#
# mango 15.08.2013
# add card name & temp for nvidia cards 

PATH=/usr/bin:/bin:/sbin

case "$1" in
	1)	# kernel version (static)
		KERNEL=$(uname -rm)
		echo -ne "s\tLinux Kernel:\t"$KERNEL
        	;;

	2)	# distribution release (static)
		if test -f /etc/SuSE-release; then
			DISTRI="openSuSE"
			RELEASE=$(head -n 1 /etc/SuSE-release)
		elif test -f /etc/redhat-release; then
			DISTRI="RedHat"
			RELEASE=$(head -n 1 /etc/redhat-release)
		#elif test -f /etc/debian_version; then
			#DISTRI="Debian"
			#RELEASE=$(head -n 1 /etc/debian_version)
		elif test -f /etc/gentoo-release; then
			DISTRI="Gentoo"
			RELEASE=$(head -n 1 /etc/gentoo-release)
		elif test -f /etc/lsb-release; then
			DISTRI=$(grep DISTRIB_ID /etc/lsb-release|cut -d"=" -f 2)
			RELEASE=$(grep DISTRIB_DESCRIPTION /etc/lsb-release|cut -d"=" -f 2)
		elif test -x /usr/bin/crux; then
			DISTRI="Crux"
			RELEASE=$(crux|cut -d" " -f 3)
		elif test -f /etc/arch-release; then
			DISTRI="Arch Linux"
			RELEASE="rolling-release"
		else
			DISTRI="unknown"
			RELEASE="unknow"
		fi
		echo -ne "s\tDistribution:\t"$RELEASE
		exit
        	;;

	3)	# CPU type (static)
		CPUTYPE=$(grep 'model name' /proc/cpuinfo | cut -d':' -f 2  | cut -d' ' -f2- | uniq)
		echo -ne "s\tCPU Type:\t"$CPUTYPE
        	;;

	4)	# current CPU speed
		VAR=$(grep 'cpu MHz' /proc/cpuinfo | sed 's/.*: *\([0-9]*\)\.[0-9]*/\1 MHz/')
		echo -ne "CPU Speed:\t"$VAR
		exit
        	;;

	5)	# hostname and IP (static)
                hostname=$(hostname)
                dnsname=$(dnsdomainname)
                ETH_COUNT=$(ifconfig |grep -c eth)
                COUNT=0
                COUNT_X=1
                while [ $COUNT -le $ETH_COUNT ]
                do
                VARIABLE=$(echo $COUNT_X"p")
                ETH=$(ifconfig | grep eth | sed -ne $VARIABLE | awk '{ print $1 }')
                IP=$(ifconfig $ETH | grep inet | cut -d: -f2 | cut -d' ' -f1)
                IP_NUMBER="$ETH IP: "${IP:-N/A}""
                if ( $COUNT -le $ETH_COUNT );then
                IP_RESULT="$IP_RESULT $IP_NUMBER"
                else
                IP_RESULT="$IP_NUMBER"
                fi
                COUNT_X=$($COUNT_X+1)
                let COUNT=$COUNT+1
                done
                echo -ne "s\tHostname:\t"${hostname:-<unknown>}""${dnsname:-}"\t"$IP_RESULT
                exit
        	;;

	6)	# fan speeds
		CPU=$( sensors | grep -i 'fan3' | tr -s ' ' | cut -d' ' -f 2)
		CASE=$(sensors | grep -i 'fan2' | tr -s ' ' | cut -d' ' -f 2)
		echo -ne "Fans:\tCPU: "$CPU" rpm\tCase: "$CASE" rpm"
		exit
        	;;

	7)	# temperature of CPU and mainboard
		CPU=$(sensors | grep -i 'temp2' | tr -s ' ' | cut -d' ' -f 2)
		MB=$( sensors | grep -i 'temp3' | tr -s ' ' | cut -d' ' -f 2)
		echo -ne "Temperaturen:\tCPU: "$CPU"\tMB: "$MB
		exit
        	;;

	8)	# temperature of hard disks
		DISK1=$(/usr/sbin/hddtemp /dev/sda1 | cut -d: -f1,3)
		DISK2=$(/usr/sbin/hddtemp /dev/sda5 | cut -d: -f1,3)
		echo -ne "HDD Temperatur:\t"$DISK1"\t"$DISK2
		exit
        	;;

	9)	# CPU usage
		echo -e "CPU Last:\tCPU%"
		exit
        	;;

	10)	# header (static)
		echo -ne "Speicher:\tGesamt    /    Frei"
		exit
		;;

	11)	# video disk usage
		VAR=$(df -Pk /video0 | tail -n 1 | tr -s ' ' | cut -d' ' -f 2,4)
		echo -ne "Festplatte:\t"$VAR
		exit
        	;;

	12)	# memory usage
		VAR=$( grep -E 'MemTotal|MemFree' /proc/meminfo | cut -d: -f2 | tr -d ' ')
		echo -ne "RAM Speicher:\t"$VAR
		exit
        	;;

	13)	# swap usage
		VAR=$(grep -E 'SwapTotal|SwapFree' /proc/meminfo | cut -d: -f2 | tr -d ' ')
		echo -ne "Swap:\t"$VAR
		exit
        	;;

	14)	# graphics card
                 VGA=$(lspci | grep -i vga | cut -d] -f1 | cut -d' ' -f8-10 | head -n1)
                 echo -ne "Grafikkarte:\t$VGA]"
                 exit
        	;;
			
	15)	# graphics card temp
                GPU=$(nvidia-settings -c :0.0 -tq GPUCoreTemp)
                echo -ne "Temperatur:\tnVidia-GPU: "$GPU°C
		exit
        	;;			
	test)
		echo ""
		echo "Usage: systeminfo.sh {1|2|3|4|...}"
		echo ""
		exit 1
		;;
esac
exit