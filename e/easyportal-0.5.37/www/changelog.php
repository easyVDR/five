<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>";
echo "? - Changelog";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>Changelog</h2>";

$datei = fopen($EASYPORTAL_DOC_ROOT."changelog","r");
$inhalt = fread($datei,filesize($EASYPORTAL_DOC_ROOT."changelog"));
fclose($datei);

// *************************************** Container Begin ****************************************************
container_begin(1, "changelog.png", "Changelog");

echo "<pre>$inhalt</pre>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
