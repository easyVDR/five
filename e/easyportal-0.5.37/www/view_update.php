<?php
include('includes/kopf.php'); 
$file   = $_GET["file"];

if (!file_exists($EASYPORTAL_DOC_ROOT."log/update")) { exec("sudo -u root $SH_UPDATE mklogdir"); }

echo "<div id='kopf2'>";
echo "<a href='update.php' class='w'>Update</a> - View Log";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_tools.php'); 
echo "<br/>";

echo "<nav class='navi3'><ul>";
echo "<li><a href='update.php'>Zurück zur Übersicht</a></li>"; 
echo "</ul></nav>";

echo "<h2>Update-Log</h2>";

if (file_exists($EASYPORTAL_DOC_ROOT."log/update/$file")) {
  $datei = fopen($EASYPORTAL_DOC_ROOT."log/update/$file","r");
  $inhalt = fread($datei,filesize($EASYPORTAL_DOC_ROOT."log/update/$file"));
  fclose($datei);
}
else {
  $inhalt = "</pre>Es liegt kein Log vor!<pre>";
}
// *************************************** Container Begin ****************************************************
container_begin(1, "Log.png", $file);

echo "<pre>$inhalt</pre>";
//echo "<textarea style='background-image:url(images/bgcontent.png)' cols=\"145\" rows=\"38\">$inhalt</textarea>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
