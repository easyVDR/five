<?php
include('includes/kopf.php'); 

$aktion   = safeget('aktion');

$done_pfad = "/var/www/databases/portal/epgsearchdone.txt";
$message   = "";


if (file_exists($done_pfad)) {
  // falls es noch eine alte Version der Datenbank gibt, diese löschen
  $t = exec("sudo $SH_BEFEHL cat $done_pfad | grep 'id#Record#Timer_id#Channel#Title#SubTitel#Description#Sonst#ep_serie#ep_title#ep_se#ep_ep#ep_no' ");
  if ($t == "") { 
    unlink($done_pfad); 
    $message = "Es wurde eine alte Version der Datenbank gefunden und diese gelöscht.<br/>";
  }
}

if (!file_exists($done_pfad)) {
  $db->executeQuery("CREATE TABLE epgsearchdone (id inc, Record str, Timer_id str, Channel str, Title str, SubTitel str, Description str, Sonst str, ep_serie str, ep_title str, ep_se str, ep_ep str, ep_no str)");
  $message = $message."Die Datenbank ist nicht vorhanden und wird angelegt.<br/>";
}


function entferne_umbruch ($text){
  $text = trim($text);
  $text = nl2br($text);
  $text = str_replace(array("\n","\r"),"",$text);
  return $text;
}

/*********************** epgsearchdone.data neu einlesen ************************/
if ($aktion=="scan") {
  $db->executeQuery("DELETE FROM epgsearchdone");
  $epgsearchdone_array = file("/var/lib/vdr/plugins/epgsearch/epgsearchdone.data");  
  $episodes = trim(shell_exec("ls $eplists_dir/*.episodes |cut -d'/' -f5-"));
  $R = $Timer_id = $C = $T = $S = $D = $At = $ep_serie = $ep_title = $ep_se = $ep_ep = $ep_no = "";
  foreach($epgsearchdone_array as $zeile)
  {
    switch (substr($zeile, 0, 1)) {
     case "R":
       $R = substr($zeile, 2, strlen($zeile)-2);
       $R = entferne_umbruch($R);
       
       $R_array = explode(' ', $R);
       $datum = date("d.m.Y H:i",$R_array[0]);
       $laenge = round($R_array[1]/60) ;
       $Timer_id = $R_array[2];
       
       break;
     case "C":
       $C = substr($zeile, 2, strlen($zeile)-2);
       $C = entferne_umbruch($C);
       break;
     case "T":
       $T = substr($zeile, 2, strlen($zeile)-2);
       $T = str_replace("'", " ", $T);
       $T = entferne_umbruch($T);
       //  $T = str_replace("\", " ", $T);
       //  $T = str_replace("\\"", " ", $T);
       break;
     case "S":
       $S = substr($zeile, 2, strlen($zeile)-2);
       $S = str_replace("'", " ", $S);
       $S = entferne_umbruch($S);
       //  $S = str_replace("\", " ", $S);
       //  $S = str_replace("\\"", " ", $S);
       break;
     case "D":
       $D = substr($zeile, 2, strlen($zeile)-2);
       $D = str_replace("'", " ", $D);
       $D = entferne_umbruch($D);
       //  $D = str_replace("\", " ", $D);
       //  $D = str_replace("\\"", " ", $D);
       break;
     case "@":
       $At = substr($zeile, 2, strlen($zeile)-2);
       $At = entferne_umbruch($At);
       break;
     case "r":
       // Tatort
       if (substr($T,0,8) == "Tatort: ") {
         $ep_serie = "Tatort";
         $ep_title = substr($T, 8, strlen($T)-8) ;
       } else {
         $ep_serie = $T; 
         $ep_title = $S;
       }

       if (((!empty($ep_serie)) && (strpos($episodes, $ep_serie))) === false) {
         $pos = strpos(strtolower($episodes), strtolower($ep_serie));
         if ($pos !== false) {
           //echo "$ep_serie - $pos - ".substr($episodes, $pos, strlen($ep_serie))."<br/>";
           $ep_serie = substr($episodes, $pos, strlen($ep_serie));
         }       
        }
     
       if (((!empty($ep_serie)) && (!empty($ep_title)) && (strpos($episodes, $ep_serie))) === false) {
         $ep_se = $ep_ep = $ep_no = "";
       } else {

         $befehl = "grep -i '".$ep_title."' '$eplists_dir/$ep_serie.episodes' ";
         $epi = trim(shell_exec("$befehl"));
         $epi_array = preg_split("/[\t]/", $epi);

         $ep_se = $epi_array[0];
         $ep_se *= 1;
         if ($ep_se == 0) {$ep_se = "";}
         if (isset($epi_array[1])) { $ep_ep = $epi_array[1]; } else { $ep_ep = "";}
         if (isset($epi_array[2])) { $ep_no = $epi_array[2]; } else { $ep_no = "";}
         //$No = $epi_array[2];
       }
       $db->executeQuery("INSERT INTO epgsearchdone (Record, Timer_id, Channel, Title, SubTitel, Description, Sonst, ep_serie, ep_title, ep_se, ep_ep, ep_no ) VALUES ('$R', '$Timer_id', '$C', '$T', '$S', '$D', '$At', '$ep_serie', '$ep_title', '$ep_se', '$ep_ep', '$ep_no' )");
       $R = $Timer_id = $C = $T = $S = $D = $At = $ep_serie = $ep_title = $ep_se = $ep_ep = $ep_no = "";
       break;
     default:
       // echo "ungültige Zeile";
    }
    //echo "$zeile<br/>";
  }
  $message = $message.$txt_scanned."<br/>";
}

/*********************** epgsearchdone.data zurückschreiben ************************/
if ($aktion=="save") {
  exec("sudo $SH_EASYPORTAL stopvdr");
  sleep(5);

  $dateiname = "/var/lib/vdr/plugins/epgsearch/epgsearchdone.data";
  //$dateiname = "/var/www/tmp/epgsearchdone.data";
  
  $datum = date("Y-m-d_Hi",time());
  $newname   = $dateiname."_".$datum;
  
  
  //rename($dateiname, $newname);
  copy($dateiname, $newname);
  
  $handler = fopen($dateiname , "w+");
  $zeile="";
  $resultSet = $db->executeQuery("SELECT * FROM epgsearchdone $WHERE $ORDER");
  while($resultSet->next()) {
    $record      = $resultSet->getCurrentValueByName("Record");
    $Channel     = $resultSet->getCurrentValueByName("Channel");
    $Title       = $resultSet->getCurrentValueByName("Title");
    $SubTitel    = $resultSet->getCurrentValueByName("SubTitel");
    $Description = $resultSet->getCurrentValueByName("Description");
    $Sonst       = $resultSet->getCurrentValueByName("Sonst");

    $zeile = $zeile."R $record\n";
    $zeile = $zeile."C $Channel\n";
    $zeile = $zeile."T $Title\n";
    $zeile = $zeile."S $SubTitel\n";
    $zeile = $zeile."D $Description\n";
    $zeile = $zeile."@ $Sonst\n";
    $zeile = $zeile."r\n";

    
  }
  // Dateiinhalt in die Datei schreiben
  fwrite($handler , $zeile);
  fclose($handler); // Datei schließen
  $message = $message."Epgsearchdone.data wurde zurückgeschrieben.<br/>";
  exec("sudo $SH_EASYPORTAL startvdr");
}


$resultSet = $db->executeQuery("SELECT * FROM epgsearchdone");
$anz = $resultSet->getRowCount();
//$message = $message."$anz Datensätze in epgseachdone-Datenbank.<br/>";
if ($anz==0) {
  $message = $message."Es sind keine Datensätze in der Datenbank. Bitte 'epgsearchdone.data neu einlesen'<br/>";
}



echo "<div id='kopf2'>Timer - epgsearch - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "activetimer.png", "$txt_ue_epgsearch_admin");


echo $message;
echo "<br/>";

//echo "<a href='epgsearchdone.php'><button>$txt_b_zuruck</button></a><br/><br/>";

if (file_exists($done_pfad)) {
  echo "<a href='?aktion=scan'><button>$txt_b_scan</button></a><br/><br/>";
}
else {
  echo "$txt_error1<br/><br/>";
  echo "<a href='?aktion=scan'><button>$txt_b_scan</button></a><br/><br/>";
}

echo "<a href='?aktion=save'><button>$txt_b_schreiben</button></a><br/><br/>";


  
container_end();
// *************************************** Container Ende ****************************************************


// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Hilfe");

echo $help1."<br/>";
echo $help2."<br/>";
echo $help3."<br/>";
echo $help4."<br/>";
echo $help5."<br/>";
echo $help6."<br/>";

container_end();
// *************************************** Container Ende ****************************************************


echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
