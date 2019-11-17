#!/bin/bash
# easyvdr-hw.sh
# Hardewareerkennung 
# 0.0.1 maxx
# 0.0.2 steevee
#

#set -x
version=0.0.2
trenn="\n"
#trenn="|"
file=""
php="no"
rcstart="no"
debug="no"

function usage () {
   echo ""
   echo "easyvdr-hw.sh $version"
   echo ""
   echo "Options: -f, -p und -r bitte immer als erste Optionen benutzen! ( wegen meiner Schreibfaulheit ;) )"
   echo
   echo "        -f <Datei>, --file <Datei>  Datei als Eingabedatei fuer DVB nutzen (gut zum Testen)"
   echo "        -p,  --php                  Als Trennzeichen <br> benutzen"
   echo "        -r,  --rcstart              dmesg snapshot ueber /etc/init.d/RCStartAfterVDR.d/RCStartPersonal"
   echo "             --debug                Debug Ausgabe (fuer DVB Erkennung)"
   echo
   echo "        -a,  --audio                Anzeige der Audio Karte"
   echo "        -c,  --cpu                  Anzeige der CPU"
   echo "        -t,  --takt                 Anzeige der CPU-Taktung"
   echo "        -b,  --tv                   Anzeige der TV-Karten"
   echo "        -d,  --hd                   Anzeige der Festplatten"
   echo "        -o,  --cd                   Anzeige der opt. Laufwerke"
   echo "        -i,  --ip                   Anzeige der IP"
   echo "        -k,  --kernel               Anzeige der Kernel Version"
   echo "        -m,  --mem                  Anzeige der Speichergr��e"
   echo "        -n,  --net                  Anzeige der Netzwerkkarte"
   echo "        -v,  --vga                  Anzeige der Grafikkarte (alt)"
   echo "        -v2, --vga2                 Anzeige der Grafikkarte (neu)"
   echo "        -h,  --help                 Hilfe anzeigen"
   echo
}

function GetParam() {
   while [ $# -gt 0 ]; do
      case "$1" in
         "--audio"	|"-a")	 ShowAudio;
                                 shift;;
         "--cpu"	|"-c")	 ShowCPU;
                                 shift;;
         "--takt"	|"-t")	 ShowCPUTakt;
                                 shift;;
         "--tv"		|"-b")	 ShowTV;
                                 shift;;
         "--tv2"		|"-b")	 ShowTV2;
                                 shift;;
         "--hd"		|"-d")	 ShowHD;
                                 shift;;
         "--cd"		|"-o")	 ShowOptLW;
                                 shift;;
         "--kernel"	|"-k")	 ShowKernel;
                                 shift;;
         "--mem"	|"-m")	 ShowMem;
                                 shift;;
         "--net"	|"-n")	 ShowNet;
                                 shift;;
         "--ip"		|"-i")	 ShowIP;
                                 shift;;
         "--vga"	|"-v")	 ShowVGA;
                                 shift;;
         "--vga2"	|"-v2")	 ShowVGA2;
                                 shift;;
         "--php"	|"-p")	 trenn="<br>";
				 php="yes"
                                 shift;;
         "--rcstart"	|"-r")	 rcstart="yes";
                                 shift;;
         "--file"	|"-f")	 shift;
				 file=$1;
				 echo "inputfile=$file" >&2;
                                 shift;;
         "--debug")              debug="yes";
                                 shift;;

         "--sensors"	|"-s")	 ShowSensors;
                                 shift;;
 
         "--help"       |"-h")   usage;
                                 exit;;
         *)	                 echo "Parameter: $1 unbekannt"
                                 usage;
                                 exit;;
      esac
   done
}

function HwInfo() {
   if [ "x$file" = "x" ]; then
      hwinfo $*
   elif [ -f "$file" ]; then
      cat $file
   else
      echo "$file nicht vorhanden. Ende." >&2
      exit 1
   fi
}
function DMesg2RCStart() {
   if [ $(grep -w -e "^dmesg > /tmp/easyvdr_hw_dmesg0.log" /etc/init.d/RCStartAfterVDR.d/RCStartPersonal &>/dev/null; echo $?) = "1"  ]; then
      echo "dmesg > /tmp/easyvdr_hw_dmesg0.log  # f�r EasyPortal" >> /etc/init.d/RCStartAfterVDR.d/RCStartPersonal
   fi
}
function DMesg() {
   if [ "x$rcstart" = "xyes" ]; then
      DMesg2RCStart
   fi

   if [ ! -f /tmp/easyvdr_hw_dmesg0.log ]; then
      dmesg > /tmp/easyvdr_hw_dmesg0.log   
      cp /tmp/easyvdr_hw_dmesg0.log /tmp/easyvdr_hw_dmesg.log   
   else
      cp /tmp/easyvdr_hw_dmesg0.log /tmp/easyvdr_hw_dmesg.log   
      dmesg >> /tmp/easyvdr_hw_dmesg.log   
   fi

   if [ "x$file" = "x" ]; then
      cat /tmp/easyvdr_hw_dmesg.log
   elif [ -f "$file" ]; then
      cat $file
   else
      echo "$file nicht vorhanden. Ende." >&2
      exit 1
   fi
}

function CutClock() {
   awk '{
    x=$0
    if (substr(x,1,1) == "[" && substr(x,14,1) == "]" )
       printf "%s\n", substr(x,16);
    else   
       printf "%s\n", x;
   }'
}

function ShowHD() {
   hwinfo --disk | grep "Model:" | grep -v "Disk"|
   awk -v TRENN="$trenn" 'FS="[\"]"{
      split($0,s);
      if (++n>1)
         printf TRENN;
      printf s[2];
   }'
   if [ x"$php" = "xno" ]; then
      echo
   fi
}

function ShowOptLW() {
   hwinfo --cdrom | grep "Model:" |
   awk -v TRENN="$trenn" 'FS="[\"]"{
      split($0,s);
      if (++n>1)
         printf TRENN;
      printf s[2];
   }'
   if [ x"$php" = "xno" ]; then
      echo
   fi
}

function ShowNet() {
   hwinfo --netcard | grep "Model:" |
   awk 'FS="[\"]"{
      split($0,s);
      if (++n>=1)
         printf "%s", s[2]
   }'
   if [ x"$php" = "xno" ]; then
      echo
   fi 
}

function ShowAudio() {
   hwinfo --sound | grep "Model:" |
   awk 'FS="[\"]"{
      split($0,s);
      if (++n==1)
         printf "%s", s[2]
   }'
   if [ x"$php" = "xno" ]; then
      echo
   fi
}


function ShowIP() {
   (/sbin/ifconfig eth0; /sbin/ifconfig eth1; /sbin/ifconfig wlan0; /sbin/ifconfig ath0; /sbin/ifconfig wifi0) 2>/dev/null | grep "inet Adresse" |
   awk '{ print $2 }' | awk -F: '
   {
      if (++n>1)
         printf TRENN;
      print $2
   }'
}

function ShowCPU() {
   less /proc/cpuinfo | grep -m1 'model name' | awk -F: '{ print $2 }'
}

function ShowCPUTakt() {
   hwinfo --cpu | grep -m1 Clock: | awk -F: '{ print $2 }' |  cut -b2- | cut -d" " -f1
}

function ShowMem() {
   less /proc/meminfo | grep 'MemTotal:' | awk -F: '{ print $2 }'
}

function ShowKernel() {
   uname -r
}

## old ##
# function ShowTV() {
#    hdet -t MEDIA | grep name | awk -Fname '{ print $2 }'
#    
# }

function ShowTV() {
   hdet -t MEDIA | grep "name" |
   awk -Fname -v TRENN="$trenn" '{
      split($0,s);
      if (++n>1)
         printf TRENN;
      printf s[2];
   }'
   if [ x"$php" = "xno" ]; then
      echo
   fi
}

function ShowVGA() {
   hdet -t VGA | grep name | awk -Fname '{ print $2 }'
}

function ShowVGA2() {
   GRAFIKKARTE=$( lspci | grep VGA | grep -i Intel | cut -d ":" -f3)
   if [ "X$GRAFIKKARTE" == "X" ]; then
     GRAFIKKARTE=$( lspci | grep VGA | grep -i NVIDIA | cut -d ":" -f3)
     if [ "X$GRAFIKKARTE" == "X" ]; then
       GRAFIKKARTE=$( lspci | grep VGA | grep -i AMD | cut -d ":" -f3)
       if [ "X$GRAFIKKARTE" == "X" ]; then
         GRAFIKKARTE=$( lspci | grep VGA | grep -i ATI | cut -d ":" -f3)
       fi
     fi
   fi
   echo $GRAFIKKARTE 
}



function ShowSensors() {
   sensors $2
}

function ShowSensors2() {
   sensors | awk -F: '{ print $2 }'
}


GetParam $*

