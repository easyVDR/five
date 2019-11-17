#!/bin/bash
#
# this script lets you query some information of you nvidia graphics card
#
# 20101109 neovandeen@googlemail.com
#
usage()
{
echo "
usage: $0 options

This script needs nvidia-settings binary installed.

OPTIONS:
   -h      Show this message
   -f      fan speed(s) in percent
   -t      gpu temperature(s) in degree celsius
"
}

# define some variables
NV=`which nvidia-settings`

# is the binary installed?
if [ $? -ne 0 ]; then
echo -e "make sure you have installed nvidia-settings\nexiting..."
exit 1
fi

if [ -z $1 ]; then usage;exit 1;fi
while getopts "afht" options; do
     case $options in

         h )
             usage
             exit 1;;
         f )
                for gpu in `$NV -q all|grep Attribute|grep OperatingSystem|grep gpu|cut -d '[' -f2|cut -c 5`; do
                        SPEED=` $NV -q [fan:$gpu]/GPUCurrentFanSpeed|grep Attribute|cut -d ':' -f4|sed "s/ //g"|sed "s/\.//g" `
                                if [ $SPEED -eq 0 ]; then echo "your fan is off or this setting is restricted by hardware"; else
                        echo "fan speed GPU$gpu: $SPEED" 
                                fi
                done
             exit 0;;
         t )
                for gpu in `$NV -q all|grep Attribute|grep OperatingSystem|grep gpu|cut -d '[' -f2|cut -c 5`; do
                        echo "temp GPU$gpu: "` $NV -q [gpu:$gpu]/GPUCoreTemp|grep Attribute|cut -d ':' -f4|sed "s/ //g"|sed "s/\.//g" `
                done
             exit 0;;
        esac
done

exit 0