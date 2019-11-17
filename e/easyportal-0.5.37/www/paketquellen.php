<?php
include('includes/kopf.php'); 

if (file_exists($EASYPORTAL_DOC_ROOT."tmp")) {
//    echo "Das Verzeichnis existiert";
} else {
//    echo "Das Verzeichnis existiert nicht";
    exec("sudo $SH_BEFEHL mkdir /var/www/tmp"); 
    exec("sudo $SH_BEFEHL chown -vR vdr:vdr /var/www/tmp"); 
}

/*
function quelle_hinzu($line, $quelle) {
  if (($line == "# deb $quelle") or ($line == "#deb $quelle")) { $line = "deb $quelle"; }
  if (($line == "# deb-src $quelle") or ($line == "#deb-src $quelle")) { $line = "deb-src $quelle"; }
  if  ($line == "deb $quelle") {$vdr_stable = 1;}
}
*/

echo "<div id='kopf2'><a href='update.php' class='w'>Update</a> - Paketquellen</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_tools.php'); 

echo "<br/>";
echo "<nav class='navi3'><ul>";
echo "<li><a href='update.php'>Zurück zur Übersicht</a></li>";
echo "</ul></nav>";

echo "<h2>Paketquellen</h2>";

// ############################################ Ändern #################################################
if (safepost('aendern')) {
  $stable   = safepost('stable');
  if ($stable == "") {$stable = 0;}
  $testing  = safepost('testing');
  if ($testing == "") {$testing = 0;}
  $unstable = safepost('unstable');
  if ($unstable == "") {$unstable = 0;}

  // echo "Status Ändern: $stable;$testing;$unstable<br/>";
  
  $vdr_stable      = 0;
  $base_stable     = 0;
  $others_stable   = 0;
  
  $vdr_testing     = 0;
  $base_testing    = 0;
  $others_testing  = 0;

  $vdr_unstable    = 0;
  $base_unstable   = 0;
  $others_unstable = 0;
  
  $array_sources_list = file("/etc/apt/sources.list");      // Datei in ein Array einlesen
  
  $datei_neu = fopen($EASYPORTAL_DOC_ROOT."tmp/sources.list","a+");
  foreach ($array_sources_list as $line) {
     $line = trim($line);
     if ($stable == 1) {
       ## vdr stable
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main") or 
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main") or 
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main") {$vdr_stable = 1;}
       ## base-stable
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main") or
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main") or 
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main") {$base_stable = 1;}
       ## others-stable
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main") or
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main") or
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main") {$others_stable = 1;}
    } else {
       $vdr_stable     = 1;
       $base_stable    = 1;
       $others_stable  = 1;
       ## vdr stable
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main"; }
       ## base-stable
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/3-base-stable/ubuntu trusty main"; }
       ## others-stable
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/3-others-stable/ubuntu trusty main"; }
    }
    if ($testing == 1) {
       ## vdr testing
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main") or
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main") or
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main") {$vdr_testing = 1;}
       ## base-testing
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main") or 
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main") or 
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main") {$base_testing = 1;}
       ## others-testing
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main") or
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main") or 
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main") {$others_testing = 1;}
    } else {
       $vdr_testing     = 1;
       $base_testing    = 1;
       $others_testing  = 1;
       ## vdr testing
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main"; }
       ## base-testing
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main"; }
       ## others-testing
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main"; }
    }
    if ($unstable == 1) {
       ## vdr unstable
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main") or 
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main") or
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main") {$vdr_unstable = 1;}
       ## base-unstable
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main") or
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main") or
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main") {$base_unstable = 1;}   
       ## others-unstable
       if (($line == "# deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main") or
           ($line == "#deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main")) {
           $line =  "deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main"; }
       if (($line == "# deb-src http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main") or
           ($line == "#deb-src http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main")) {
           $line =  "deb-src http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main"; }
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main") {$others_unstable = 1;}   
    } else {
       $vdr_unstable    = 1;
       $base_unstable   = 1;
       $others_unstable = 1;
       ## vdr unstable
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main"; }
       ## base-unstable
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main"; }
       ## others-unstable
       if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main") {
           $line =  "# deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main"; }
       if ($line == "deb-src http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main") {
           $line =  "# deb-src http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main"; }
    }
    
    $line = $line."\n";
    fwrite($datei_neu, $line);
  }
  if ($vdr_testing == 0) { 
     fwrite($datei_neu, "\n## vdr-testing\n");
     fwrite($datei_neu, "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main\n"); 
     fwrite($datei_neu, "deb-src http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main\n"); 
  }
  if ($base_testing == 0) { 
     fwrite($datei_neu, "\n## base-testing\n");
     fwrite($datei_neu, "deb http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main\n"); 
     fwrite($datei_neu, "deb-src http://ppa.launchpad.net/easyvdr-team/3-base-testing/ubuntu trusty main\n"); 
  }
  if ($others_testing == 0) { 
     fwrite($datei_neu, "\n## others-testing\n");
     fwrite($datei_neu, "deb http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main\n"); 
     fwrite($datei_neu, "deb-src http://ppa.launchpad.net/easyvdr-team/3-others-testing/ubuntu trusty main\n"); 
  }
  if ($vdr_unstable == 0) { 
     fwrite($datei_neu, "\n## vdr-unstable\n");
     fwrite($datei_neu, "deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main\n"); 
     fwrite($datei_neu, "deb-src http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main\n"); 
  }
  if ($base_unstable == 0) { 
     fwrite($datei_neu, "\n## base-unstable\n");  
     fwrite($datei_neu, "deb http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main\n"); 
     fwrite($datei_neu, "deb-src http://ppa.launchpad.net/easyvdr-team/5-base-unstable/ubuntu focal main\n"); 
  } 
  if ($others_unstable == 0) { 
     fwrite($datei_neu, "\n## others-unstable\n");
     fwrite($datei_neu, "deb http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main\n"); 
     fwrite($datei_neu, "deb-src http://ppa.launchpad.net/easyvdr-team/5-others-unstable/ubuntu focal main\n"); 
  }

  fclose($datei_neu); // Datei schließen
  
  exec("sudo $SH_BEFEHL mv /etc/apt/sources.list /etc/apt/sources.list.old");
  exec("sudo $SH_BEFEHL mv /var/www/tmp/sources.list /etc/apt/sources.list");

  exec("sudo $SH_UPDATE update");
  
//  echo "<b>Paketquellen wurden geändert</b>";
//  echo "<br/><br/>";
}

// *************************************** Container Begin 'Paketquellen' ****************************************************
container_begin(1, "paket.gif", "EasyVDR Paketquellen");

echo "EasyVDR Paketquellen aktivieren/deaktivieren:";
echo "<br/><br/>";

$datei=fopen("/etc/apt/sources.list","r");

$stable   = 0;
$testing  = 0;
$unstable = 0;

while (!feof($datei)) {
  $line = trim(fgets($datei));
  if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-stable/ubuntu trusty main")   {$stable = 1;}
  if ($line == "deb http://ppa.launchpad.net/easyvdr-team/3-vdr-testing/ubuntu trusty main")  {$testing = 1;}
  if ($line == "deb http://ppa.launchpad.net/easyvdr-team/5-vdr-unstable/ubuntu focal main") {$unstable = 1;}
}

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='stable' value='1'>";

if ($stable == 1)   {$checked_s = "checked";} else {$checked_s = "";}
if ($testing == 1)  {$checked_t = "checked";} else {$checked_t = "";}
if ($unstable == 1) {$checked_u = "checked";} else {$checked_u = "";}

echo "<table border=0>";
echo "<tr><td>Stable - Die Paketquelle für alle Enduser:</td><td><input type='checkbox' name='stable'   $checked_s value='1' Disabled></td></tr>";
echo "<tr><td>Testing - Die Paketquelle für Betatester:</td> <td><input type='checkbox' name='testing'  $checked_t value='1'>         </td></tr>";
if ($_SESSION["r_expert"] == "y" ) {
  echo "<tr><td>Unstable - Die Paketquelle für Entwickler:</td><td><input type='checkbox' name='unstable' $checked_u value='1' Disabled >&nbsp;&nbsp;<img height='15px' src=/images/menuicons/e.png onmouseover=\"Tip('Nur für Experten !!!')\" >  </td></tr>";
} else {
  echo "<tr><td>Unstable - Die Paketquelle für Entwickler:</td><td><input type='checkbox' name='unstable' $checked_u value='1'>&nbsp;&nbsp;<img height='15px' src=/images/menuicons/e.png onmouseover=\"Tip('Nur für Experten !!!')\" >  </td></tr>";

}
echo "</table>";
echo "<br/>";

echo "<input type='submit' name='aendern' value='ändern' onclick=\"wait('WaitPaketquellen')\">";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Sources.list' ****************************************************
container_begin(1, "paket.gif", "Die komplette /etc/apt/sources.list");

echo "<pre>";
$datei=fopen("/etc/apt/sources.list","r");
while (!feof($datei)) {
  $line = fgets($datei); 
  if($line{0}=="#") { 
    $line = "<font color='green'>".$line."</font>";
  } 
  echo $line;
}
fclose($datei);
echo "</pre>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Hilfe' ****************************************************
container_begin(1, "Info.png", "Die komplette /etc/apt/sources.list");

echo "In der Datei /etc/apt/sources.list stehen die sogenannten Repositories (engl. Lager, Depot), also Quellen für Pakete. <br/>";
echo "Dies können entweder CDs oder DVDs, Verzeichnisse auf der Festplatte oder in der Regel Verzeichnisse auf HTTP- oder FTP-Servern sein. Befindet sich das gesuchte Paket auf einem Server (oder einem lokalen Datentr&auml;ger), so kann man dieses installieren.";
echo "<br/><br/>";
echo "Die EasyVDR Paketquellen können Easy über die 2 Checkboxen an- und abgewählt werden.<br/>";
echo "Es stehen folgende Quellen zur Auswahl:<br/><br/>";
echo "Stable - Die Paketquelle für alle Enduser (kann nicht abgewählt werden)<br/>";
echo "Testing - Die Paketquelle für Betatester<br/>";
echo "Unstable - Die Paketquelle für Entwickler<br/>";
echo "<br/>Nach Änderung können Pakete aus dieser Quelle installiert werden<br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
