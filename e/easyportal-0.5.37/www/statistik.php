<?php
include('includes/kopf.php'); 


$dateien_ges        = exec("sudo $SH_BEFEHL find /var/www -type f |wc -l"); 
//$dateien_php        = exec("sudo $SH_BEFEHL find /var/www -name '*.php' -type f |wc -l"); 
$dateien_php        = exec("sudo $SH_BEFEHL ls -R /var/www/ |grep .php |wc -l "); 
//$dateien_php_zeilen = exec("sudo $SH_BEFEHL find /var/www -name '*.php' -type f -print0 |xargs -0 cat |wc -l"); 
$dateien_css        = exec("sudo $SH_BEFEHL find /var/www -name '*.css' -type f |wc -l"); 
$dateien_css_zeilen = exec("sudo $SH_BEFEHL find /var/www -name '*.css' -type f -print0 |xargs -0 cat |wc -l"); 
$dateien_db         = exec("sudo $SH_BEFEHL find /var/www/databases -name '*.txt' -type f |wc -l"); 
$dateien_images     = exec("sudo $SH_BEFEHL find /var/www/images -type f |wc -l"); 
$dateien_scripts    = exec("sudo $SH_BEFEHL find /var/www/scripts -type f |wc -l"); 


echo "<div id='kopf2'>Statistik</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>Statistik Easyportal</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "statistik.png", "Statistik");

echo "<table class='tab1'>";
echo "<tr><th>Dateien gesamt</th><td align=right>$dateien_ges</td></tr>";
echo "<tr><th>php-Dateien </th><td align=right>$dateien_php</td></tr>";
//echo "<tr><th>Zeilen php-Quellcode</th><td align=right>$dateien_php_zeilen</td></tr>";
echo "<tr><th>css-Dateien</th><td align=right>$dateien_css</td></tr>";
echo "<tr><th>Zeilen css-Quellcode</th><td align=right>$dateien_css_zeilen</td></tr>";
echo "<tr><th>Datenbank Tabellen</th><td align=right>$dateien_db</td></tr>";
echo "<tr><th>Bilder</th><td align=right>$dateien_images</td></tr>";
echo "<tr><th>Dateien Shell-Scripts</th><td align=right>$dateien_scripts</td></tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
