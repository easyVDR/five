<?php
$art = $_GET[art];

// Shell-Scripts für diverse OS-Kommandos
$SH_EASYPORTAL     = "/var/www/scripts/easyportal.sh";
$SH_BEFEHL         = "/var/www/scripts/befehl.sh";

exec("sudo $SH_BEFEHL nmap -sP 192.168.1.0/24 |grep 'Nmap scan report' | cut -d'(' -f2 |cut -d')' -f1", $on);

if (in_array($resultSet->getCurrentValueByName("IP"), $on)) { 
echo "<a style='visibility:hidden;'>x</a><img src='images/led_green.gif' class='icon'><a style='visibility:hidden;'>x</a>"; } // Wenn IP Online




?>