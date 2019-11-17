#!/bin/bash
echo "$(date '+%d.%m.%Y %H:%M:%S') - Befehl ausgeführt: sudo $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17}" >> /var/www/log/easyportal.log 
sudo $1 $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} 2>&1
#esac
