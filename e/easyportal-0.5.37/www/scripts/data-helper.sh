#!/bin/bash
#
#    data-helper
# 
##################################################################################################
#cpu-name=`cat /proc/cpuinfo |grep "model name"|tr -s " "|sed -n '1p'`
cpu_name_r=`cat /proc/cpuinfo |grep "model name"`
cpu_name=`cat /proc/cpuinfo |grep "model name"|tr -s " "|sed q|sed 's/.*://'`
cpu_freq_r=`cat /proc/cpuinfo |grep "cpu MHz"`
cpu_freq=`cat /proc/cpuinfo |grep "cpu MHz"|tr -s " "|sed q|sed 's/.*://'|sed -r 's/([^\.]*)\..*/\1/'`
cd_dvd_name_r=`ls /dev/disk/by-id/ -l|grep "/sr[0-9]"`
cd_dvd_name=`ls /dev/disk/by-id/ -l|grep "/sr[0-9]"|cut -b 44-|sed -e 's/->.*$//'`
audio_devices_r=` lspci -mnn|grep "0403"`
audio_devices=` lspci -mnn|grep "0403"|cut -d "\"" -f 3-7|sed 's/\"/ /g'|tr -s " "`
nic_devices_r=` lspci -mnn|grep "0200"`
nic_devices=` lspci -mnn|grep "0200"|cut -d "\"" -f 3-7|sed 's/\"/ /g'|tr -s " "`
gfx_devices_r=`lspci -mnn|grep "0300"`
gfx_devices=`lspci -mnn|grep "0300"|cut -d "\"" -f 3-7|sed 's/\"/ /g'|tr -s " "`
hdd_names_r=`ls /dev/disk/by-id/ -l`
hdd_names=`ls /dev/disk/by-id/ -l|grep -e ata -e usb |grep "/[sh]d[a-z]$"|cut -b 44-|sed 's/_.*//'`
nvidia_temp_r=`nvidia-settings -c :0 -tq GPUCoreTemp`
nvidia_temp="`nvidia-settings -c :0 -tq GPUCoreTemp|tr -s " "|sed q`°C"

echo -e "Erst RohDaten falls lieber in php bearbeiten: \033[1m[Enter]\033[0m"
read -s

clear

echo -e "\033[1mCPU\033[0m"
echo -e "$cpu_name_r"
echo -e "\033[1mTakt\033[0m"
echo -e "$cpu_freq_r"
echo -e "\033[1mOptical\033[0m"
echo -e "$cd_dvd_name_r"
echo -e "\033[1mSound\033[0m"
echo -e "$audio_devices_r"
echo -e "\033[1mNetzwerk\033[0m"
echo -e "$nic_devices_r"
echo -e "\033[1mGrafikkarte\033[0m"
echo -e "$gfx_devices_r"
echo -e "\033[1mHDDs\033[0m"
echo -e "$hdd_names_r"
echo -e "\033[1mNVidiaTemp\033[0m"
echo -e "$nvidia_temp_r"

sleep 1
echo -e "\nSo nun bissl zurechtgestutzt \033[1m[Enter]\033[0m"
read -s

clear

echo -e "\033[1mCPU\033[0m"
echo -e "$cpu_name"
echo -e "\n"
echo -e "\033[1mTakt\033[0m"
echo -e "$cpu_freq MHz"
echo -e "\n"
echo -e "\033[1mOptical\033[0m"
echo -e "$cd_dvd_name"
echo -e "\n"
echo -e "\033[1mSound\033[0m"
echo -e "$audio_devices"
echo -e "\n"
echo -e "\033[1mNetzwerk\033[0m"
echo -e "$nic_devices"
echo -e "\n"
echo -e "\033[1mGrafikkarte\033[0m"
echo -e "$gfx_devices"
echo -e "\n"
echo -e "\033[1mHDDs\033[0m"
echo -e "$hdd_names"
echo -e "\n"
echo -e "\033[1mNVidiaTemp\033[0m"
echo -e "$nvidia_temp"
echo -e ""

sleep 0.5

echo -e "\033[1m"
echo -n "Und "
sleep 0.8
echo -n "nicht "
sleep 0.8
echo -n "sekunden"
sleep 0.5
echo -n "lang "
sleep 0.5
echo -n "warten "
sleep 0.8
echo -n "..."

echo -e "\n"

exit
EOF
#--------
