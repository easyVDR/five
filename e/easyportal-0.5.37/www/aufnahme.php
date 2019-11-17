<?php
include('includes/kopf.php'); 

$info = "/video0/".safeget('info')."/info";
$dir = safeget('info');

$datum_all = substr($dir, strrpos($dir,"/") + 1, strlen($dir) ); 
$datum = substr($datum_all, 0, 10);
$zeit  = str_replace('.', ':', substr($datum_all, 11, 5));

$dir = substr($dir, 0, strlen($dir)-strlen(strrchr($dir, "/")));
$dir_encode = urlencode(safeget('info'));

function x_to_str($x) {
  $X_str = "xxx";
  $X_array = explode(' ', trim($x));
  switch ($X_array[0]) {
    case "1":
      $X_str = "MPEG2-Video";
      $type = "video";
      break;
    case "2":
      $X_str = "MPEG2 Audio";
      $type = "audio";
      break;
    case "3":
      $X_str = "Untertitel";
      break;
    case "4":
      $X_str = "AC3-Audio";
      $type = "audio";
      break;
    case "5":
      $X_str = "H.264-Video";
      $type = "video";
      break;
    case "6":
      $X_str = "HEAAC-Audio";
      $type = "audio";
      break;
    default:
      $X_str = $X_array[0];
  }
  switch ($type) {
    case "video";
      switch ($X_array[1]) {
        case "01": case "05":
          $X_str = $X_str." 4:3";
          break;
        case "02": case "03": case "06": case "07":
          $X_str = $X_str." 16:9";
          break;
        case "04": case "08":
          $X_str = $X_str." >16:9";
          break;
        case "09": case "0D":
          $X_str = $X_str." HD 4:3";
          break;
        case "0A": case "0B": case "0E": case "0F":
          $X_str = $X_str." HD 16:9";
          break;
        case "0C": case "10":
          $X_str = $X_str." HD >16:9";
          break;
      } 
    case "audio":
      switch ($X_array[1]) {
        case "01":
          $X_str = $X_str." Mono";
          break;
        case "03":
          $X_str = $X_str." Stereo";
          break;
        case "05":
          $X_str = $X_str." Dolby Digital";
          break;
        default:
          $X_str = $X_str." ".$X_array[1];
     }
  }
  $X_str = $X_str." ".$X_array[2];
  $X_str = $X_str." ".$X_array[3];
  $X_str = $X_str." ".$X_array[4];
  $X_str = $X_str." ".$X_array[5];
  $X_str = $X_str." ".$X_array[6];
  $X_str = $X_str." ".$X_array[7];
  //$X_str = $X_str." (".$x.")";
  return($X_str);
}

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

echo "<div id='kopf2'><a href='aufnahmen.php' class='w'>$txt_aufnahmen</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "film.png", "$txt_ue_aufnahme");

echo "<a href='aufnahmen.php'><button>$txt_b_aufnahmeliste</button></a>";
echo "<a href='aufnahme_edit.php?info=$dir_encode'><button>$txt_b_editieren</button></a>";
echo "<a href='aufnahme_rename.php?rec=$dir_encode'><button>$txt_b_umbenennen</button></a>";
echo "<br/><br/>";

echo "<table class=tab1>";
echo "<tr><th>$txt_t_verzeichnis</th>  <td>$dir</td>        <th>Serien-Check</th></tr>";
echo "<tr><th>$txt_t_datum</th><td>$datum $zeit</td><td>&nbsp;</td></tr>";
//echo "<tr><th>&nbsp;</th><td>&nbsp;</td></tr>";
$datei=fopen($info,"r");
$S = $T = $staffel = $staffelfolgen = $array_gefundene_Folge[1] = $array_gefundene_Folge[2] = "";

while (!feof($datei)) {
  $line = fgets($datei); 
  echo "<tr>";
  switch ($line{0}) {
    case "C":
        // C = Aufnahmesender
        $C = substr($line,2,strlen($line)-3);
        echo "<th>$txt_t_kanal</th><td>$C</td><td>&nbsp;</td>";
        break;
    case "E":
        // E = EventID StartZeit Dauer TableID, wie in epg.data
        $E = substr($line,2,strlen($line)-3);
        
        $wert_array = explode(' ', trim($E));
        $start = date("d.m.Y H:i",$wert_array[1]);
        $dauer = round($wert_array[2]/60);
        echo "<tr><th>$txt_t_event_id</th>      <td>$wert_array[0]</td></tr>";
        echo "<tr><th>$txt_t_event_start</th>   <td>$start</td></tr>";
        echo "<tr><th>$txt_t_event_dauer</th>   <td>$dauer Min.</td></tr>";
        echo "<tr><th>$txt_t_event_tableid</th> <td>$wert_array[3]</td></tr>";
        echo "<tr><th>$txt_t_event_version</th> <td>$wert_array[4]</td></tr>";

        //echo "<th>Event</th><td>$E</td><td>&nbsp;</td>";
        break;
    case "T":
        // T = Titel
        $T = substr($line,2,strlen($line)-3);

        if (substr($T,0,8) == "Tatort: ") { 
          $T1 = "Tatort"; 
          $S1 = substr($T, 8, strlen($T)-8);
        }
        else { 
          $T1 = $T;
          $S1 = "";
        }
        
        //$t2 = check_eplist($T);
        
        $episodes = check_eplist($T1);
        if ($episodes == "") {
          echo "<th>$txt_t_titel</th><td>$T</td><td><nobr>$txt_keine_serie</nobr></td>";
        } else {  
          echo "<th>$txt_t_titel</th><td><a class='b' href='serie.php?serie=$T1'>$T</a></td><td><a class='b' href='serie.php?serie=$T1'>$T1</a> </td>";
        }
        break;
    case "S":
        // S = Subtitel
        $S = substr($line,2,strlen($line)-3);

        if ($S1 == "") { $S1 = $S;}
        
        $gefundene_Folge = exec("cat '$eplists_dir/$T1.episodes' |grep '$S1'");
        $array_gefundene_Folge = preg_split("/[\t]/", $gefundene_Folge);
        if (!isset($array_gefundene_Folge[1])) { $array_gefundene_Folge[1] = "";}
        if (!isset($array_gefundene_Folge[2])) { $array_gefundene_Folge[2] = "";}
        if (!isset($array_gefundene_Folge[3])) { $array_gefundene_Folge[3] = "";}
        if (!isset($array_gefundene_Folge[6])) { $array_gefundene_Folge[6] = "";}
        
        $txt_debug_meldungen[] = "gefundene_Folge: $gefundene_Folge";
        
        $staffel = $array_gefundene_Folge[0] ;
        $staffel *= 1;  // führende Nullen entfernen
        if ($staffel == 0) { $staffel = "";}

        $staffelfolgen_zeile = exec("cat '$eplists_dir/$T1.episodes' |grep '# $staffel\t'");
        
        if ($staffelfolgen_zeile=="") {
          $staffelfolgen = ""; }
        else {
          $staffelfolgen_array = preg_split("/[\t]/", $staffelfolgen_zeile);
          $staffelfolgen = $staffelfolgen_array[2] - $staffelfolgen_array[1] + 1;
        }
        if ($staffel == "") { $staffelfolgen = "";}

        echo "<th>$txt_t_subtitel</th><td>$S</td><td>";
        //echo $array_gefundene_Folge[3]; 
        echo $S1;
        echo "</td>";
        break;
    case "D":
        // D = Description
        $D = substr($line,2,strlen($line)-3);
        $D = str_replace("|", "<br/>", $D);
        echo "<th>$txt_t_beschreibung</th><td valign=top>$D</td><td valign=top>";
        
        if ($episodes != "") {
          if ($T1=="Tatort") {
            echo "Genre: Krimi<br/>";
            echo "Kategorie: Serie<br/>";
            echo "Land: D<br/>";
            $jahr = $staffel + 1969;
            echo "Jahr: <a class='b' href='tatort.php?jahr=$jahr'>$jahr</a><br/>";
            echo "Folge: $array_gefundene_Folge[2]<br/>";
            echo "<br/><br/>";
          
            echo "Serie: <a class='b' href='tatort.php'>$T1</a><br/>";
            echo "<nobr>Episode: $S1</nobr><br/>";
            echo "<nobr>Ermittler: <a class='b' href='tatort.php?ermittler=$array_gefundene_Folge[4]'>$array_gefundene_Folge[4]</a></nobr><br/>";
            echo "Ort: <a class='b' href='tatort.php?ort=$array_gefundene_Folge[5]'>$array_gefundene_Folge[5]</a><br/>";
            echo "Sender: <a class='b' href='tatort.php?sender=$array_gefundene_Folge[6]'>$array_gefundene_Folge[6]</a><br/>";
            echo "Staffel: <a class='b' href='tatort.php?jahr=$jahr'>$staffel</a><br/>";
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
        
        echo "&nbsp;</td>";
        break;
    case "X":
        // X = Technische Details 
        $X = substr($line,2,strlen($line)-3);
        $X_str = x_to_str($X);
 
        //$X_str = $X;
        echo "<th style='white-space: nowrap;'>$txt_t_details</th><td>$X_str</td><td>&nbsp;</td>";
        break;
    case "V":
        // V = VPS Zeit (time_t) 
        $V = substr($line,2,strlen($line)-3);
        echo "<th>$txt_t_vps</th><td>$V</td><td>&nbsp;</td>";
        break;
    case "F":
        // F = Framerate 
        $F = substr($line,2,strlen($line)-3);
        echo "<th>$txt_t_framerate</th><td>$F</td><td>&nbsp;</td>";
        break;
    case "P":
        // P = Priorität
        $P = substr($line,2,strlen($line)-3);
        echo "<th>$txt_t_prio</th><td>$P</td><td>&nbsp;</td>";
        break;
    case "L":
        // L = Lebensdauer  
        $L = substr($line,2,strlen($line)-3);
        echo "<th>$txt_t_lebensdauer</th><td>$L</td><td>&nbsp;</td>";
        break;
  } 
  echo "</td>";

}
fclose($datei);
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
