<?php
include('includes/kopf.php'); 

$sprachdatei = safeget('sprachdatei');
$master_lang = "Deutsch";
$edit_lang   = safeget('sprache');
$variable    = safeget('var');


function save_var ($datei_path, $variable, $neuer_wert) {
  // Variable in PHP-Datei ändern
  $daten = file($datei_path);                 // Datei einlesen
  $fp = fopen($datei_path, 'w');              // Datei neu erstellen
  foreach ($daten as $zeile){
    $felder = explode("=", $zeile);           // Zeile aufteilen
    if (($zeile == "?>") AND ($variable_vorhanden != "ja")) {
      $zeile = "$".$variable." = \"". $neuer_wert ."\";\n";
      fwrite($fp, $zeile);                      // Zeile schreiben
      $zeile = "?>";
    }
    elseif (trim($felder[0]) == "$".$variable) {  // Wenn gesuchte Zeile
      $felder[1] = " \"$neuer_wert\";\n";     // 2. Feld ändern
      $zeile = implode("=", $felder);         // Zeile wieder zusammensetzen
      $variable_vorhanden = "ja";
    }
    fwrite($fp, $zeile);                      // Zeile schreiben
  }
  fclose($fp);                                // neue Datei schliessen  
}


if (safepost('save')!="") {
  echo "Gespeichert<br/>";
  $sprachdatei = safepost('sprachdatei');
  $edit_lang   = safepost('edit_lang');
  $variable    = safepost('variable');
  $neuer_wert  = safepost('neuer_wert');

  $sprachdatei_path = "/var/www/lang/$edit_lang/$sprachdatei";
  // echo "$sprachdatei_path<br/>";
  save_var($sprachdatei_path, $variable, $neuer_wert);
}

$master_zeile = exec("sudo $SH_BEFEHL cat ".$EASYPORTAL_DOC_ROOT."lang/$master_lang/$sprachdatei | grep '$variable'");
$stringposition = strpos($master_zeile, "\"");
$wert  = substr($master_zeile, $stringposition+1);
$stringposition = strpos($wert, "\"");
$master_wert = substr($wert, 0, $stringposition);

$edit_zeile = exec("sudo $SH_BEFEHL cat ".$EASYPORTAL_DOC_ROOT."lang/$edit_lang/$sprachdatei | grep '$variable'");
$stringposition = strpos($edit_zeile, "\"");
$wert  = substr($edit_zeile, $stringposition+1);
$stringposition = strpos($wert, "\"");
$edit_wert = substr($wert, 0, $stringposition);


echo "<div id='kopf2'>";
echo "Sprache Editieren";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";
//include('includes/subnavi_tools.php'); 
//echo "<br/>";

echo "<h2>Sprache Editieren</h2>";

$seite = substr($sprachdatei, 4, strlen($sprachdatei)-4);

// *************************************** Container Begin ****************************************************
container_begin(1, "Log.png", "Sprachfiles zur Seite '$seite'");


echo "Link zur Seite: <a href='$seite' class='b'>$seite</a><br/><br/>";

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='sprachdatei' value='$sprachdatei'>";
echo "<input type='hidden' name='variable' value='$variable'>";
echo "<input type='hidden' name='edit_lang' value='$edit_lang'>";

echo "<table class=tab1>";
echo "<tr><td>Seite</td><td>$sprachdatei</td></tr>";
echo "<tr><td>zu editierende Sprache</td><td>$edit_lang</td></tr>";
echo "<tr><td>Variable</td><td>$variable</td></tr>";
echo "<tr><td>Master-Sprache ($master_lang)</td><td>$master_wert</td></tr>";
echo "<tr><td>Edit-Sprache</td><td><input type='text' name='neuer_wert' size='150' value='$edit_wert'></td></tr>";
echo "</table>";

echo "<br/>";
echo "<input type='submit' name='save' value='speichern'>";
echo "</form>";

echo "<br/>";
echo "<a href='lang_view.php?sprachdatei=$sprachdatei&edit_lang=$edit_lang'><button>Alle Texte dieser Seite</button></a>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
