<?php
include('includes/kopf.php'); 

$master_lang = "Deutsch";

$edit_lang   = safepost('edit_lang');
if ($edit_lang == "") { $edit_lang = safeget('edit_lang'); }

$sprachdatei = safepost('sprachdatei');
if ($sprachdatei == "") { $sprachdatei = safeget('sprachdatei'); }

if ($edit_lang == "") {$edit_lang = $master_lang;}

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

echo "Zu editierende Sprache auswählen:<br/>";
$language_dir = $EASYPORTAL_DOC_ROOT."lang/";
$dir_array = scandir($language_dir);  // Alle Dateien und Ordner in ein Array packen 
sort($dir_array);                     // Array sortieren
foreach($dir_array as $einzel_datei){                   // jetzt alle Array Inhalte durchgehen
  if($einzel_datei != "." && $einzel_datei != "..") {   // schauen ob die Zeichen "." oder ".." nicht enthalten sind
    if(is_dir($language_dir.$einzel_datei)) {           // jetzt schauen ob es ein Verzeichniss ist
      if ($edit_lang==$einzel_datei) {$checked_s="checked";} else {$checked_s="";}
      echo "<input type='radio' name='edit_lang' value='$einzel_datei' $checked_s onChange='this.form.submit()'> $einzel_datei &nbsp;";
    }
  }
}
echo "</form>";

echo "<br/><br/>";

$datei=file($EASYPORTAL_DOC_ROOT."lang/$master_lang/$sprachdatei");
unset($inhalt,$eintrage);
for ($i=0;$i<count($datei);$i++){ 
  //$inhalt .="$datei[$i]";
  $eintrage[] = $datei[$i];
}
//$eintrage=explode(';',$inhalt); // Wir erzeugen einen Array

echo "<table class='tab1'>";
    echo "<tr>";
    echo "<th width='50px' height='25px'>Variable</th>"; 
    echo "<th>Master-Sprache $master_lang</th>"; 
    echo "<th>Sprache $edit_lang</th>"; 
    echo "<th width=20px>Aktionen</th>"; 
    echo "</tr>";

for ($i=0;$i<count($eintrage);$i++){ 
  
  if (substr($eintrage[$i],0,1) == "$") { 
    // Variable
    $variable = substr($eintrage[$i],1,strlen($eintrage[$i])-1);
    $stringposition = strpos($variable, "=");
    $variable = substr($variable, 0, $stringposition);
    $stringposition = strpos($eintrage[$i], "\"");
    $wert  = substr($eintrage[$i], $stringposition+1);
    $stringposition = strpos($wert, "\"");
    $wert = substr($wert, 0, $stringposition);

    // edit Lang
    $edit_zeile = exec("sudo $SH_BEFEHL cat ".$EASYPORTAL_DOC_ROOT."lang/$edit_lang/$sprachdatei | grep '$variable'");
    $stringposition = strpos($edit_zeile, "\"");
    $wert2  = substr($edit_zeile, $stringposition+1);
    $stringposition = strpos($wert2, "\"");
    $wert2 = substr($wert2, 0, $stringposition);

    echo "<tr>";
    echo "<td>$variable</td>"; 
    echo "<td>$wert</td>"; 
    echo "<td>$wert2</td>"; 
    echo "<td align=center>"; 
    echo "<a onmouseover=\"Tip('Sprachfile editieren')\" href='lang_edit.php?sprachdatei=$sprachdatei&sprache=$edit_lang&var=$variable'>&nbsp;<img src=/images/icon/edit.png></a>";
    echo "</td>"; 
    echo "</tr>";
  }
  if (substr($eintrage[$i],0,2) == "//") { 
    $kommentar = $eintrage[$i];
    $kommentar = str_replace("//", "", $kommentar);
    echo "<tr>";
    echo "<th colspan=4 height='25px'>&nbsp;&nbsp;&nbsp;$kommentar</th>"; 
    echo "</tr>";
  
  }
}

echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
