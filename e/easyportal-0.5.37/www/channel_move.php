<?php
include('includes/kopf.php'); 

// Name der Datei
//$datei = "/var/www/tmp/1_Astra.conf";
$channel = $_GET[channel];
$datei   = "/var/lib/vdr/channels/".$channel;

// Daten aus der Datei lesen
$daten = file($datei);


echo "<div id='kopf2'>Kanäle verschieben</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>Kanäle verschieben '$channel'</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Kanäle verschieben '$channel'");

$to = -1;
if (isset($_GET["move"])) {
  if (is_numeric($_GET["move"]) && is_numeric($_GET["to"])) {
    if ($_GET["move"] <= count($daten) && $_GET["to"] <= count($daten) && $_GET["move"] > -1 && $_GET["to"] > -1 ) {
      $move = $_GET["move"];
      $to = $_GET["to"];
      // Daten kopieren
      $daten_kopie = $daten;
      // Ausgewählter Datensatz entfernen
      unset($daten_kopie[$move]);
      // Datensatz an neuer Position einfügen
      array_splice($daten_kopie, $to, 0, $daten[$move]);
      // Daten kopieren
      $daten = $daten_kopie;
      // Daten in die Datei schreiben
      $handler = fOpen($datei , "w");
      fWrite($handler, implode("", $daten));
      fClose($handler);
      // file_put_contents($datei, implode("", $daten)); // PHP5
    }
  }
}

// Daten ausgeben
echo "<table class='tab1'>";
foreach ($daten as $key => $element) {
  echo "<tr";
 
  // Hintergrundfarbe
  if ($key == $to) { echo " style=\"background-color: #EAF9FF;\">"; }
  else { echo " style=\"background-color: #' . ($key % 2 ? 'FFFFFF' : 'E9E9E9') . ';\">"; }
  
  $stringposition = strpos($element, ";");
  
  if ($stringposition) { $element_t = substr($element, 0, $stringposition); }
  else { 
    $stringposition = strpos($element, ":");
    if ($stringposition) { $element_t = substr($element, 0, $stringposition); }
    else { $element_t = $element; }
  }

  echo "<th style=\"width: 1%; white-space: nowrap;\">";

  // Links zum verschieben
  if ($key > 0) { echo " <a class='b' href='?channel=$channel&move=".$key."&amp;to=".($key -1)."' title='Aufwärts'>&nbsp;<img src='/images/icon/pfeil_hoch.png' height='14px'>&nbsp;</a> "; }
  if ($key < count($daten) -1) { echo " <a class='b' href='?channel=$channel&move=".$key."&amp;to=".($key +1)."' title='Abwärts'>&nbsp;<img src='/images/icon/pfeil_runter.png' height='14px'>&nbsp;</a> "; }

  if (substr ($element_t,0,1) == ":") {
    $element_t = substr ($element_t, 1, strlen($element_t)-1);
    echo "<td height='25px'>&nbsp;<b>".$element_t."</b>&nbsp;</td>"; }
  else {  
    echo "<td>&nbsp;".$element_t."&nbsp;</td>"; }
  
  echo "</th></tr>";
}
echo "</table>";


container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
