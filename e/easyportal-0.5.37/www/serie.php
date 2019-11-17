<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');

if (isset($_POST["serie"])) { $serie = safepost('serie'); } else { $serie = safeget('serie'); }
$txt_debug_meldungen[] = "serie: $serie";

if ($serie == "Tatort") {redirect('/tatort.php');}

echo "<div id='kopf2'>$txt_aufnahmen - $txt_kopf2 - Serienfolgen</div>";
echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", "Serie: ".$serie);

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='serie' value='$serie'>";

$seasonlist = "off";
$short      = "";
$extracol_array = array();
$array_aufgez_folgen = array();
$anz_folgen = 0;
// ******************* episodes einlesen ****************************
$array_datei = file("$eplists_dir/$serie.episodes");
foreach($array_datei as $zeile) {
  // echo $zeile."<br/>";
  if ($seasonlist == "on") {
    if (substr($zeile,0,13)=="# /SEASONLIST") { 
      $seasonlist = "off";
      continue;
    }
    $seasonlist_array[] = preg_split("/[\t]/", trim(substr($zeile,2,strlen($zeile))));
    $t = preg_split("/[\t]/", trim(substr($zeile,2,strlen($zeile)))) ;
    if ($t[2] > $anz_folgen) { $anz_folgen = $t[2]; }
    // echo $zeile."<br/>";
    continue;
  }
  
  if (substr($zeile,0,1)=="#") {
    // Komentar, Schlüsselworte
    if (substr($zeile,0,7)=="# SHORT") { 
      $short = trim(substr($zeile,7,strlen($zeile)));
      continue;
    }
    if (substr($zeile,0,12)=="# SEASONLIST") { 
      $seasonlist = "on";
      continue;
    }
    if (substr($zeile,0,10)=="# COMPLETE") { 
      $complete = "yes";
      continue;
    }
    if (substr($zeile,0,10)=="# EXTRACOL") { 
      $extracol_array[] = explode(" ", trim(substr($zeile,10,strlen($zeile))));
      continue;
    }
  }
  else {
    // Folgen
    $array_folgen[] = preg_split("/[\t]/", $zeile);
  }
}

$resultSet2 = $db->executeQuery("SELECT id FROM epgsearchdone WHERE Title='$serie'");
$Anz2 = $resultSet2->getRowCount();
if ($Anz2 > 0) {
  $epgsearchdone_txt = "<a href='epgsearchdone.php?title=$serie'>$Anz2 Episoden</a> in epgsearchdone<br/><br/>";
  $epgsearchdone_button = "<a href='epgsearchdone.php?title=$serie' onmouseover=\"Tip('alle Einträge in der epgsearchdone.data von Serie: $serie')\"><button type='button'>aufgenomme Folgen</button></a>";
}
else
{
  $epgsearchdone_txt = "Keine Episoden in epgsearchdone<br/><br/>";
  $epgsearchdone_button = "";
}


echo "<a href='serien.php'><button type='button'>$txt_b_serien</button></a>";
echo $epgsearchdone_button;
echo "<br/><br/>";

if ($short!="") { echo "$txt_short: '$short'<br/><br/>"; }

$resultSet = $db->executeQuery("SELECT * FROM aufnahmen WHERE Serie='$serie'");
$anz = $resultSet->getRowCount();
while($resultSet->next()) {
  $array_aufgez_folgen[] = $resultSet->getCurrentValueByName("No");
}
echo "$anz von $anz_folgen $txt_rec<br/>";
echo $epgsearchdone_txt;

echo "$txt_seasons: ";
foreach ($seasonlist_array as $staffel) { echo " <a onmouseover=\"Tip('zu<br/>Staffel $staffel[0]<br/>Folge $staffel[1] - $staffel[2]')\" class='b' href='#SE$staffel[0]'>&nbsp;$staffel[0]&nbsp;</a>"; }

echo "<br/>";
echo "<br/>";

// ************** Tabelle ****************

echo "<table class='tab1'>";
echo "<tr> <th>SE</th> <th>EP</th> <th>No.</th> <th>Title</th>";
foreach($extracol_array as $extracol) { echo "<th>$extracol[1]</th>"; }
echo "<th> </th> </tr>";

$SE_alt=0;
foreach($array_folgen as $zeile) {
  echo "<tr>";
  $SE = $zeile[0];
  $SE *= 1;
  echo "";
  if ($SE != $SE_alt) {
    echo "<th valign='middle' colspan=5 id='SE$SE'>&nbsp;&nbsp;&nbsp;$SE. Staffel </th></tr><tr>";
    //echo "<th valign='middle' colspan=4 id='SE$SE'>&nbsp;&nbsp;&nbsp;$SE. Staffel ";
    //echo "</th><th><a onmouseover=\"Tip('Zum Seitenanfang')\" class='b' href='#kopf1'><img src='images/icon/pfeil_hoch.png'></a></th></tr><tr>";
  }
  echo "<td align=right>$SE</td>";
  $SE_alt = $SE;
  echo "<td align=right>$zeile[1]</td>";  // EP
  echo "<td align=right>$zeile[2]</td>";  // No.

  $titel2 = trim($zeile[3]);
  $resultSet2 = $db->executeQuery("SELECT id FROM epgsearchdone WHERE ep_serie='$serie' and ep_title='".$titel2."'");
  $Anz2 = $resultSet2->getRowCount();
  $resultSet2->next();


  if (in_array($zeile[2], $array_aufgez_folgen)) {
    $resultSet = $db->executeQuery("SELECT * FROM aufnahmen WHERE Serie='$serie' and No='$zeile[2]'");
    $resultSet->next();
    $aufnahme = $resultSet->getCurrentValueByName("Aufnahme");
    echo "<td align=left ><a class='b' href='aufnahme.php?info=$aufnahme' >$zeile[3]</a></td>"; } // Titel 
  else { 
    if ($Anz2 > 0) {
      //echo "<td align=left ><div style='color:green'>$zeile[3]</div></td>";   // Title 
      
      if ($Anz2 == 0) { echo "<td></td>"; }
      if ($Anz2 == 1) { echo "<td><a href='epgsearchdone_view.php?id=".$resultSet2->getCurrentValueByName("id")."' class='b'><div style='color:green'>$zeile[3]</div></a></td>"; }
      if ($Anz2 > 1)  { echo "<td><a href='epgsearchdone.php?title=$serie&subtitle=$titel2'  class='b'><div style='color:green'>$zeile[3]</div></a></td>"; }

      
    } else {
      echo "<td align=left >$zeile[3]</td>";   // Title
    }
  }  
  $i=4;
  foreach($extracol_array as $extracol) { // alle extracols
    echo "<td align=left>$zeile[$i]</td>";
    $i++;
  }

  if ($Anz2 == 0) { echo "<td></td>"; }
  if ($Anz2 == 1) { echo "<td><a href='epgsearchdone_view.php?id=".$resultSet2->getCurrentValueByName("id")."' onmouseover=\"Tip('- Anzahl Einträge in epgsearchdone<br/>- zur epgsearchdone')\" class='b'>$Anz2</a></td>"; }
  if ($Anz2 > 1)  { echo "<td><a href='epgsearchdone.php?title=$serie&subtitle=$titel2' onmouseover=\"Tip('Anzahl Einträge<br/>in epgsearchdone')\" class='b'>$Anz2</a></td>"; }
  echo "</tr>";
}
echo "</table>";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
