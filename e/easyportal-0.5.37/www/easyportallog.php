<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>";
echo "? - Easyportal-Log";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>Easyportal-Log</h2>";

$datei = fopen($EASYPORTAL_DOC_ROOT."/log/easyportal.log","r");
$inhalt = fread($datei,filesize($EASYPORTAL_DOC_ROOT."/log/easyportal.log"));
fclose($datei);

// *************************************** Container Begin ****************************************************
container_begin(1, "changelog.png", "Easyportal Log");

echo "<pre>$inhalt</pre>";
//echo "<textarea style='background-image:url(images/bgcontent.png)' cols=\"145\" rows=\"38\">$inhalt</textarea>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
