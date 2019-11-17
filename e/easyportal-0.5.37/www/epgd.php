<?php
include('includes/kopf.php'); 
session_start();

echo "<div id='kopf2'>";
echo "epgd";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

echo "<h2>epgd</h2>";

echo "<div class='Container1sp'>";
echo "<div class='title'>";
echo "	<img src='images/menuicons/Info.png' align='left' class='icon' width='30'>&nbsp;&nbsp;epgd";
echo "</div>";  // title
echo "<br/>";

//$plugin = "epg2vdr";
$datei = fopen("/var/lib/vdr/setup.conf","r");
while (!feof($datei)) {
  $line = fgets($datei); 
  if (strpos($line, "epg2vdr.DbHost") !== false) {
    $DbHost = substr ($line,17,strlen($line));
  }
  if (strpos($line, "epg2vdr.DbName") !== false) {
    $DbName = substr ($line,17,strlen($line));
  }
  if (strpos($line, "epg2vdr.DbPass") !== false) {
    $DbPass = substr ($line,17,strlen($line));
  }
  if (strpos($line, "epg2vdr.DbPort") !== false) {
    $DbPort = substr ($line,17,strlen($line));
  }
  if (strpos($line, "epg2vdr.DbUser") !== false) {
    $DbUser = substr ($line,17,strlen($line));
  } 
}
fclose($datei);

if ($DbHost == ""){
  echo "epgd-Datenbank ist nicht konfiguriert!";
}
else
{
  echo "epgd-Datenbank ist konfiguriert:<br/><br/>";
  echo "<table border=1>";
  echo "<tr><td>DbHost </td><td>$DbHost</td></tr>";
  echo "<tr><td>DbName </td><td>$DbName</td></tr>";
  echo "<tr><td>DbPass </td><td>$DbPass</td></tr>";
  echo "<tr><td>DbPort </td><td>$DbPort</td></tr>";
  echo "<tr><td>DbUser </td><td>$DbUser</td></tr>";
  echo "</table>";
  $_SESSION['dbhost'] = trim($DbHost);
  $_SESSION['dbname'] = trim($DbName);
  $_SESSION['dbpass'] = trim($DbPass);
  $_SESSION['dbport'] = trim($DbPort);
  $_SESSION['dbuser'] = trim($DbUser);
  echo "<br/>";
  echo "<a href=/epgd/index.php target='_blank'><button>epgd-Datenbank anschauen/durchsuchen</button></a>";
}
  
echo "<br/><br/>";
echo "</div>";  // Container

echo "<div class='Container1sp'>";
echo "<div class='title'>";
echo "	<img src='images/menuicons/Info.png' align='left' class='icon' width='30'>&nbsp;&nbsp;Hilfe";
echo "</div>";  // title
echo "<br/>";
echo "<b>epgd</b><br/><br/>";

echo "Mit epgd hat man eine mysql-Datenbank für epg-Daten. Die Infos werden von epgdata, thetvdb, themoviedb, eplist, etc.";
echo "vom Internet geladen und zusammenfügt, um ein möglichst gutes EPG zu bekommen. Mit dem plugin epg2vdr werden die Daten in den vdr geladen.<br/><br/>";

echo "Wenn eine Datenbank konfiguriert ist, kann auch direkt (ohne vdr) in die Datenbank geschaut werden.<br/><br/>";

echo "</div>";  // Container

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
