<?php
include('includes/kopf.php'); 


echo "<div id='kopf2'>Test-Seite</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>XML-Test-Seite</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Test");

/*

$guisettings = "/home/easyvdr/.kodi/userdata/guisettings.xml";
$xml = simplexml_load_file($guisettings);
//$settings = new SimpleXMLElement($xml);

//print_r($xml);

//echo $xml[1];
echo $xml->services->webserver; 
echo "<br/>";
echo $xml->services->webserverport; 
*/

$SH_BEFEHL   = "/var/www/scripts/befehl.sh";
$advancedsettings = "/home/easyvdr/.kodi/userdata/advancedsettings.xml";

$xml = simplexml_load_file($advancedsettings);

$x1 = exec("sudo $SH_BEFEHL cat $advancedsettings| grep advancedsettings;");
if ($x1 == "") {
  exec("sudo $SH_BEFEHL echo '<advancedsettings>' >> $advancedsettings");
  exec("sudo $SH_BEFEHL echo '</advancedsettings>' >> $advancedsettings");
}

$x2 = exec("sudo $SH_BEFEHL cat $advancedsettings| grep webserver;");
if ($x2 == "") {
  $bericht = $xml->addChild("services");
  $bericht->addChild("webserver", "true");
}

$handle = fopen($advancedsettings, "wb"); 
fwrite($handle, $xml->asXML());
fclose($handle);




//echo "...<br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
