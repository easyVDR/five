<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');

// *************************** check eplist *****************************************
function check_eplist ($T) {
  global $eplists_dir;
  $eplist_dir_array = scandir($eplists_dir);
  foreach ($eplist_dir_array as $file) { // Ausgabeschleife
    $file=ltrim($file);
    if (strpos($file, ".episodes")!==false) {
      $file_anzeige = Str_replace("_", " ", substr($file,0,Strlen($file)-9));
      if ($file_anzeige == $T) { 
        $serie_gefunden = "y";
        return($file);
      }
    }
  }
}

if (!empty($_POST)) {
  //  echo 'Post request<br />';
  $info       = safepost('info');
  $dir_encode = safepost('dir_encode');
  $T          = safepost('T');
  $S          = safepost('S');
  $D          = safepost('D');

  $sub=0;
  
  $daten = file($info);                   // Datei einlesen

//echo $info;
  
  $fp = fopen($info, 'w');                // Datei neu erstellen
  foreach ($daten as $zeile){
    if ($zeile{0} == "T") {               // Wenn gesuchte Zeile
      $zeile = "T $T\n";                  // Zeile wieder zusammensetzen
    }
    if ($zeile{0} == "S") {               // Wenn gesuchte Zeile
      $zeile = "S $S\n";                  // Zeile wieder zusammensetzen
      $sub=1;
    }
    if ($zeile{0} == "D") {               // Wenn gesuchte Zeile
      if (($sub==0) and ($S!="")) {
        $zeile = "S $S\n";
        fwrite($fp, $zeile);  
        $sub=1;
      }
      $D = str_replace("\n", "|", $D);
      $D = str_replace("\r", "", $D);
      $zeile = "D $D\n";                  // Zeile wieder zusammensetzen
    }
    fwrite($fp, $zeile);                  // Zeile schreiben
  }
  fclose($fp);                            // neue Datei schliessen     
  //header("Location: aufnahme.php?info=$dir_encode");
  redirect('/aufnahme.php?info='.$dir_encode);
  exit();  
}
else
{
  $info = "/video0/".safeget('info')."/info";
  $dir_encode = urlencode(safeget('info'));
}

echo "<div id='kopf2'>";
echo "<a href='aufnahmen.php' class='w'>$txt_aufnahmen</a> - $txt_kopf2";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "film.png", $txt_ue_rec);

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='info' value='$info'>";
echo "<input type='hidden' name='dir_encode' value='$dir_encode'>";

echo "<a href='aufnahmen.php'><button type='button'>$txt_b_aufnahmen</button></a>";
echo "<a href='aufnahme.php?info=$dir_encode'><button type='button'>$txt_b_aufnahme</button></a>";
echo "<input type='submit' name='submit' value='Speichern'>";

echo "<br/><br/>";

echo "<table class='tab1'>";
$datei=fopen($info,"r");
while (!feof($datei)) {
  $line = fgets($datei); 
  echo "  <tr>";
  switch ($line{0}) {
    case "C":
        // C = Aufnahmesender
        $C = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_kanal</td><td>$C</td><td><b>Serien-Check</b></td>";
        echo "<input type='hidden' name='C' value='$C'>";
        break;
    case "E":
        // E = EventID StartZeit Dauer TableID, wie in epg.data
        $E = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_event</td><td>$E</td><td>&nbsp;</td>";
        break;
    case "T":
        // T = Titel
        $T = substr($line,2,strlen($line)-3);
        $episodes = check_eplist($T);
        if ($episodes == "") {
          echo "<td>$txt_t_titel</td><td><input type='text' size='40' name='T' value='$T'></td><td><nobr>Keine Serie erkannt</nobr></td>";
        } else {  
          echo "<td>$txt_t_titel</td><td><input type='text' size='40' name='T' value='$T'></td><td><a class='b' href='serie.php?serie=$T'>$T</a></td>";
        }
        break;
    case "S":
        // S = Subtitel
        $S = substr($line,2,strlen($line)-3);
        $gefundene_Folge = exec("cat '$eplists_dir/$T.episodes' |grep '$S'");
        $array_gefundene_Folge = preg_split("/[\t]/", $gefundene_Folge);
        if (!isset($array_gefundene_Folge[1])) { $array_gefundene_Folge[1] = "";}
        if (!isset($array_gefundene_Folge[2])) { $array_gefundene_Folge[2] = "";}
        if (!isset($array_gefundene_Folge[3])) { $array_gefundene_Folge[3] = "";}
        if (!isset($array_gefundene_Folge[6])) { $array_gefundene_Folge[6] = "";}
        $txt_debug_meldungen[] = "gefundene_Folge: $gefundene_Folge";
        $staffel = $array_gefundene_Folge[0] ;
        $staffel *= 1;  // führende Nullen entfernen
        if ($staffel == 0) { $staffel = "";}
        $staffelfolgen_zeile = exec("cat '$eplists_dir/$T.episodes' |grep '# $staffel\t'");
        $staffelfolgen_array = preg_split("/[\t]/", $staffelfolgen_zeile);
        if (!isset($staffelfolgen_array[1])) { $staffelfolgen_array[1] = "";}
        if (!isset($staffelfolgen_array[2])) { $staffelfolgen_array[2] = "";}
        $staffelfolgen = $staffelfolgen_array[2] - $staffelfolgen_array[1] + 1;
        if ($staffel == "") { $staffelfolgen = "";}
        echo "<td>$txt_t_subtitel</td><td><input type='text' size='40' name='S' value='$S'></td><td>".$array_gefundene_Folge[3]."</td>";
        $S_yn="y";
        break;
    case "D":
        // D = Description
        if ($S_yn!="y") {echo "</tr><tr><td>$txt_t_subtitel (neu)</td><td><input type='text' size='40' name='S' value=''></td><td>&nbsp;</td></tr><tr>";}
        $D = substr($line,2,strlen($line)-3);
        $D = str_replace("|", "\n", $D);
        echo "<td>$txt_t_beschreibung</td><td><textarea name='D' rows='40' cols='100'>$D</textarea></td><td valign=top>";
        
        
        if ($episodes != "") {
          if ($T=="Tatort") {
            echo "Genre: Krimi<br/>";
            echo "Kategorie: Serie<br/>";
            echo "Land: D<br/>";
            $jahr = $staffel + 1969;
            echo "Jahr: $jahr<br/>";
            echo "Folge: $array_gefundene_Folge[2]<br/>";
            echo "<br/><br/>";
          
            echo "Serie: $T<br/>";
            echo "<nobr>Episode: $S</nobr><br/>";
            echo "<nobr>Ermittler: $array_gefundene_Folge[4]</nobr><br/>";
            echo "Ort: $array_gefundene_Folge[5]<br/>";
            echo "Sender: $array_gefundene_Folge[6]<br/>";
            echo "Staffel: $staffel<br/>";
            echo "Staffelfolge: $array_gefundene_Folge[1]<br/>";
            echo "Staffelfolgen: $staffelfolgen<br/>";
            echo "Folge: $array_gefundene_Folge[2]<br/>";
          }
          else {
            echo "Genre: <br/>";
            echo "Kategorie: Serie<br/>";
            echo "Land: <br/>";
            echo "Jahr: <br/>";
            echo "Folge: $array_gefundene_Folge[2]<br/>";
            echo "<br/><br/><br/><br/>";
          
          
            echo "<nobr>Serie: $T</nobr><br/>";
            echo "<nobr>Episode: $S</nobr><br/>";
            echo "Staffel: $staffel<br/>";
            echo "Staffelfolge: $array_gefundene_Folge[1]<br/>";
            echo "Staffelfolgen: $staffelfolgen<br/>";
            echo "Folge: $array_gefundene_Folge[2]<br/>";
          }
        }
        echo "</td>";
        
        break;
    case "X":
        // X = Technische Details 
        $X = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_details</td><td>$X</td><td>&nbsp;</td>";
        break;
    case "V":
        // V = VPS Zeit (time_t) 
        $V = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_vps</td><td>$V</td><td>&nbsp;</td>";
        break;
    case "F":
        // F = Framerate 
        $F = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_framerate</td><td>$F</td><td>&nbsp;</td>";
        break;
    case "P":
        // P = Priorität
        $P = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_prio</td><td>$P</td><td>&nbsp;</td>";
        break;
    case "L":
        // L = Lebensdauer  
        $L = substr($line,2,strlen($line)-3);
        echo "<td>$txt_t_lebensdauer</td><td>$L</td><td>&nbsp;</td>";
        break;
  } 
  echo "</tr>";
}
fclose($datei);
echo "</table>";

echo "<br><br>";

echo "<a href='aufnahmen.php'><button type='button'>$txt_b_aufnahmen</button></a>";
echo "<a href='aufnahme.php?info=$dir_encode'><button type='button'>$txt_b_aufnahme</button></a>";
echo "<input type='submit' name='submit' value='Speichern'>";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
