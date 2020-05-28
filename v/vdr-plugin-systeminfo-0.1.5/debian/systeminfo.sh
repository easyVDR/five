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
		KERNEL=$(uname -r)
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
		echo -ne "CPU speed:\t"$VAR
		exit
        	;;

	5)	# hostname and IP (static)
		hostname=$(hostname)
		dnsname=$(dnsdomainname)
		IP=$(ip a | grep inet | grep -vw lo | grep -v inet6 | cut -d \/ -f1 | sed 's/[^0-9\.]*//g')
		echo -ne "s\tHostname:\t"${hostname:-<unknown>}"."${dnsname:-<unknown>}"\tIP: "${IP:-N/A}
		exit
        	;;

	6)	# fan speeds
		CPU=$(sensors | grep -i 'fan1:' | tr -s ' ' | cut -d' ' -f 2)
		CASE=$(sensors | grep -i 'fan2:' | tr -s ' ' | cut -d' ' -f 2)
		echo -ne "Fans:\tCPU: "$CPU" rpm\tCase: "$CASE" rpm"
		exit
        	;;

	7)	# temperature of CPU and mainboard
		CPU=$(sensors | grep -i 'Core 0' | tr -s ' ' | cut -d' ' -f 3)
		MB=$(sensors | grep -i 'temp2' | tr -s ' ' | cut -d' ' -f 2)
		echo -ne "Temperatures:\tCPU: "$CPU"\tMB: "$MB
		exit
        	;;

	8)	# temperature of hard disks
		DISK1=$(/usr/sbin/hddtemp /dev/sda | cut -d: -f1,3)
		DISK2=$(/usr/sbin/hddtemp /dev/sdb | cut -d: -f1,3)
		echo -ne "\t"$DISK1"\t"$DISK2
		exit
        	;;

	9)	# CPU usage
		echo -e "CPU time:\tCPU%"
		exit
        	;;

	10)	# header (static)
		echo -ne "s\t\ttotal / free"
		exit
		;;

	11)	# system disk usage
		VAR=$(df -Pk / | tail -n 1 | tr -s ' ' | cut -d' ' -f 2,4)
		echo -ne "System Disk:\t"$VAR
		exit
        	;;
	12)	# video disk usage
		VAR=$(df -Pk /media/easyvdr01 | tail -n 1 | tr -s ' ' | cut -d' ' -f 2,4)
		echo -ne "Video Disk:\t"$VAR
		exit
        	;;

	13)	# memory usage
		VAR=$( grep -E 'MemTotal|MemFree' /proc/meminfo | cut -d: -f2 | tr -d ' ')
		echo -ne "Memory:\t"$VAR
		exit
        	;;

	14)	# swap usage
		VAR=$(grep -E 'SwapTotal|SwapFree' /proc/meminfo | cut -d: -f2 | tr -d ' ')
		echo -ne "Swap:\t"$VAR
		exit
        	;;

	15)	# graphics card
        VGA=$(lspci | grep -i vga | cut -d' ' -f8-10 | cut -d] -f1 | head -n1)
		echo -ne "Graphics Card:\t"$VGA]
		exit
        	;;
			
	16)	# graphics card temp
        GPU=$(nvidia-settings -c :0.0 -tq GPUCoreTemp)
        echo -ne "Temperatures:\tnVidia-GPU: "$GPU°C
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
