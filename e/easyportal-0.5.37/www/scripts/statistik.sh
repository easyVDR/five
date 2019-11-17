#!/bin/bash
#
# 
#
echo "Easyportal"
echo ""
find /var/www/ -type f | wc -l
echo "Dateien gesammt"
echo ""
find /var/www/ -name '*.php' -type f | wc -l
echo "php-Dateien mit"
find /var/www/ -name '*.php' -type f -print0 | xargs -0 cat | wc -l
echo "Zeilen Quellcode"
echo ""
find /var/www/ -name '*.css' -type f | wc -l
echo "css-Dateien mit"
find /var/www/ -name '*.css' -type f -print0 | xargs -0 cat | wc -l
echo "Zeilen Quellcode"
echo ""
find /var/www/ -name '*.sh' -type f | wc -l
echo "sh-Dateien mit"
find /var/www/ -name '*.sh' -type f -print0 | xargs -0 cat | wc -l
echo "Zeilen Quellcode"
echo ""
find /var/www/ -name '*.js' -type f | wc -l
echo "js-Dateien mit"
find /var/www/ -name '*.js' -type f -print0 | xargs -0 cat | wc -l
echo "Zeilen Quellcode"
echo ""
find /var/www/databases -name '*.txt' -type f | wc -l
echo "Datenbank-Dateien"
echo ""
find /var/www/images -name '*.*' -type f | wc -l
echo "Bilder"
echo ""
find /var/www/scripts -name '*.*' -type f | wc -l
echo "Dateien Shell-Scripts"


