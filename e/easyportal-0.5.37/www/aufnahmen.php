<?php
include('includes/kopf.php'); 

?>
<script type="text/javascript" src="/includes/js/jquery.tablesorter.js"></script> 
<script type="text/javascript">
  $(document).ready(function() { $("#sortTable").tablesorter(
    { headers: { 7:{ sorter: false } }, sortList: [[0,0]] }
  ); } );
</script>



<style>
@media (max-width: 800px) {
  /* Wenn die Tabelle nicht mehr wie die Tabelle sein soll */
  .tab1 table, .tab1 thead, .tab1 tbody, .tab1 th, .tab1 td, .tab1 tr { display: block; }

  /* Hide table headers (but not display: none;, for accessibility) */
  .tab1 thead .tab1 tr {
    position: absolute;
    top: -9999px;
    left: -9999px;
  }

  .tab1 tr { border: 1px solid #ccc; }

  .tab1 td {
    /* Behave  like a "row" */
    border: none;
    border-bottom: 1px solid #eee;
    position: relative;
    padding-left: 50%;
    min-height: 16px;
  }

  .tab1 td:before {
    /* Now like a table header */
    position: absolute;
    /* Top/left values mimic padding */
    top: 2px;
    left: 6px;
    width: 45%;
    padding-right: 10px;
    white-space: nowrap;
  }

  /* Label the data */
  .tab1 td:nth-of-type(1):before { content: "<?php echo $txt_tab_titel; ?>"; }
  .tab1 td:nth-of-type(2):before { content: "<?php echo $txt_tab_datum_zeit; ?>"; }
  .tab1 td:nth-of-type(3):before { content: "<?php echo $txt_tab_serie; ?>"; }
  .tab1 td:nth-of-type(4):before { content: "<?php echo $txt_tab_folge; ?>"; }
  .tab1 td:nth-of-type(5):before { content: "SE"; }
  .tab1 td:nth-of-type(6):before { content: "EP"; }
  .tab1 td:nth-of-type(7):before { content: "No"; }
  .tab1 td:nth-of-type(8):before { content: "<?php echo $txt_tab_aktionen; ?>"; }
}
</style>


<?php

$action=safeget('action');

if (!file_exists("/var/www/databases/portal/aufnahmen.txt")) {
  //echo "Die Datei existiert nicht";
  $db->executeQuery("CREATE TABLE aufnahmen (id inc, Aufnahme str, Serie str, Title str, SE int, EP int, No int)");
  $action = "recsuche";
}

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", "$txt_ue_aufnahmen");

if ($action=="recsuche") {
  echo "Seriensuche: <br/>";
  $db->executeQuery("DELETE FROM aufnahmen");
  $episodes = trim(shell_exec("ls $eplists_dir/*.episodes |cut -d'/' -f5-"));
//  $aufnahmen = shell_exec("find /video0/ -type f -name info |cut -d'/' -f3-");
  $aufnahmen = shell_exec("find /video0/ -type f -name info |cut -d'/' -f3- | grep -v .del/info");

  $aufnahmen_array = preg_split('/\s+/', trim($aufnahmen));
  $anz_aufnahmen = count($aufnahmen_array);
  
  echo "$anz_aufnahmen Aufnahmen werden untersucht.<br/><br/>";
  foreach ($aufnahmen_array as $aufnahme)   {
    echo "<tr>";
    $aufnahme = substr ($aufnahme, 0, strlen($aufnahme)-5);
    $aufnahme = str_replace("'", "\'", $aufnahme);

    $aufnahme_ansicht = substr($aufnahme,0,strrpos($aufnahme,"/") ); 
    $aufnahme_ansicht = str_replace("_", " ", $aufnahme_ansicht);
    $befehl = "grep -i '^T' '/video0/".$aufnahme."/info' |cut -d' ' -f2-";
    $titel = trim(shell_exec("$befehl"));

    
    if (substr($titel,0,8) == "Tatort: ") {
      $sub_titel = substr($titel, 8, strlen($titel)-8) ;
      $titel = "Tatort";
    }
    else {$sub_titel="";}
    
    if (((!empty($titel)) && (strpos($episodes, $titel))) === false) {
      $titel = $sub_titel = $SE = $EP = $No = "";
    } else {
      $befehl = "grep -i '^S' '/video0/".$aufnahme."/info' |cut -d' ' -f2-";
      if ($sub_titel=="") {$sub_titel = trim(shell_exec("$befehl"));}
      $befehl = "grep -i '".$sub_titel."' '$eplists_dir/$titel.episodes' ";
      $epi = trim(shell_exec("$befehl"));
      $epi_array = preg_split("/[\t]/", $epi);
      $SE = $epi_array[0];
      $SE *= 1;
      if ($SE == 0) {$SE="";}
      if (isset($epi_array[1])) { $EP = $epi_array[1]; } else { $EP = "";}
      if (isset($epi_array[2])) { $No = $epi_array[2]; } else { $No = "";}
      //$No = $epi_array[2];
    }
    $db->executeQuery("INSERT INTO aufnahmen (Aufnahme, Serie, Title, SE, EP, No ) VALUES ('$aufnahme', '$titel', '$sub_titel', '$SE', '$EP', '$No' )");
  }
}

echo "<a href='?action=recsuche'><button>$txt_b_durchsuchen</button></a><br/><br/>";

// **************************  Tabelle  ********************************************
echo "<table id='sortTable' class='tab1 tablesorter'>";
echo "<thead>";
echo "<tr><th>$txt_tab_titel</th><th>$txt_tab_datum_zeit</th><th>$txt_tab_serie</th><th>$txt_tab_folge</th>";
echo "<th onmouseover=\"Tip('$TIP_SE')\">SE</th><th onmouseover=\"Tip('$TIP_EP')\">EP</th><th onmouseover=\"Tip('$TIP_No')\">No</th>";
echo "<th>$txt_tab_aktionen</th></tr>";
echo "</thead>";
echo "<tbody>";

$resultSet = $db->executeQuery("SELECT * FROM aufnahmen ORDER BY Aufnahme");
while($resultSet->next()) {
  $aufnahme = $resultSet->getCurrentValueByName("Aufnahme");
  $aufnahme_t = substr ($aufnahme, 0, strlen($aufnahme)-5);
  $title = substr ($aufnahme_t,0,strrpos($aufnahme_t,"/") ); 
  $title = str_replace("_", " ", $title);
  
  $datum_all = substr($aufnahme, strrpos($aufnahme,"/") + 1, strlen($aufnahme) ); 
  $datum = substr($datum_all, 0, 10);
  $zeit  = str_replace('.', ':', substr($datum_all, 11, 5));
  $datum_zeit = $datum." ".$zeit;
  $dir_encode = urlencode($aufnahme);
  $serie = $resultSet->getCurrentValueByName("Serie");
  echo "<tr>";
  echo "<td><a class='b' href='aufnahme.php?info=$dir_encode'>". $title ." </a></td>" ;
  echo "<td nowrap>". $datum_zeit ."</td>" ;
  echo "<td><a class='b' href='serie.php?serie=$serie'>$serie</a></td>" ;
  echo "<td>". $resultSet->getCurrentValueByName("Title") ."</td>" ;
  echo "<td>". $resultSet->getCurrentValueByName("SE") ."</td>" ;
  echo "<td>". $resultSet->getCurrentValueByName("EP") ."</td>" ;
  echo "<td>". $resultSet->getCurrentValueByName("No") ."</td>" ;  
  echo "<td nowrap><a onmouseover=\"Tip('$TIP_edit')\" class='b' href='aufnahme_edit.php?info=$dir_encode'>&nbsp;<img src=/images/icon/edit.png>&nbsp;</a>";
  echo     "<a onmouseover=\"Tip('$TIP_rename')\" class='b' href='aufnahme_rename.php?rec=$dir_encode'>&nbsp;<img src=/images/icon/rename.png>&nbsp;</a>";
  echo     "<a onmouseover=\"Tip('$TIP_del')\" class='b' href='aufnahme_del.php?rec=$dir_encode'>&nbsp;<img src=/images/icon/del.png>&nbsp;</a>"; 
  echo "</td>";
  echo "</tr>";
}
echo "</tbody>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
