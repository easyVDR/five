<?php
include('includes/kopf.php'); 

?>
<script type="text/javascript" src="/includes/js/jquery.tablesorter.js"></script> 
<script type="text/javascript">
  $(document).ready(function() { $("#sortTable").tablesorter(
    {headers: { 0:{ sorter: false } }, sortList: [[2,0]] }
  ); } );
</script>
<?php

$ansicht = safepost('ansicht');
if ($ansicht == "") { $ansicht = "Alle Folgen";}

$image_tatort_dir = "/images/tatort";

$WHERE = "";
$txt_zusatz = "";
$image = "";

$ermittler = safeget('ermittler');
if ($ermittler != ""){  
  $WHERE  = " WHERE Ermittler='".$ermittler."' ORDER BY No"; 
  $txt_zusatz = " mit Ermittler '".$ermittler."'";
  $image = $image_tatort_dir."/ermittler/".$ermittler.".jpg";
}

$ort = safeget('ort');
if ($ort != ""){ 
  $WHERE  = " WHERE Ort='".$ort."' ORDER BY No"; 
  $txt_zusatz = " von Ort '".$ort."'";
}

$sender = safeget('sender');
if ($sender != ""){ 
  $WHERE = " WHERE Sender='".$sender."' ORDER BY No"; 
  $txt_zusatz = " von Sender '".$sender."'";
  $image = $image_tatort_dir."/sender/".$sender.".jpg";
}

$jahr = safeget('jahr');
if ($jahr != ""){ 
  $WHERE = " WHERE Jahr='".$jahr."' ORDER BY No"; 
  $se = $jahr - 1969;
  $txt_zusatz = " von Jahr '".$jahr."' &nbsp; (Staffel $se)";
}

$ansicht = safepost('ansicht');
if (($ansicht == "") and ($WHERE == "")) { $ansicht = "Alle Folgen";}


echo "<div id='kopf2'>Tatort</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>Tatort</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", "Tatort");

if (safeget('action') == "einlesen") {
  $array_folgen = array();

  $db->executeQuery("DELETE FROM tatort");
  echo "episodes wurde neu eingelesen.<br/><br/>";
  $array_datei = file("$eplists_dir/Tatort.episodes");
  foreach($array_datei as $zeile) {
    if (substr($zeile,0,1)!="#") {
      //echo $zeile."<br/>";
      $array_folgen = preg_split("/[\t]/", $zeile);
      //echo $array_folgen[3];
      if (trim($array_folgen[3]) != "n.n.") {
        $jahr = $array_folgen[0] + 1969;
        $array_folgen[3] = str_replace("%n", "", trim($array_folgen[3]));
        $array_folgen[6] = str_replace("%n", "", trim($array_folgen[6]));
        $db->executeQuery("INSERT INTO tatort (SE, EP, No, Title, Ermittler, Ort, Sender, Jahr) 
                VALUES ('$array_folgen[0]', '$array_folgen[1]', '$array_folgen[2]', '$array_folgen[3]', '$array_folgen[4]', '$array_folgen[5]', '$array_folgen[6]', '$jahr'");
      }
    }
  }
}

//echo "<a href='?action=einlesen'><button>episodes neu einlesen</button></a>";
//echo "</br></br>";
echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='submit' name='ansicht' value='Alle Folgen'>";
//echo "<input type='submit' name='ansicht' value='Folgen Alphabetisch'>";
echo "<input type='submit' name='ansicht' value='Ermittler'>";
echo "<input type='submit' name='ansicht' value='Ort'>";
echo "<input type='submit' name='ansicht' value='Sender'>";
echo "<input type='submit' name='ansicht' value='Jahr'>";
echo "</form>";

if (($ansicht=="Alle Folgen") or ($WHERE!="")){
  $resultSet = $db->executeQuery("SELECT * FROM aufnahmen WHERE Serie='Tatort'");
  $anz_rec = $resultSet->getRowCount();
  $array_aufgez_folgen[] = "";
  while($resultSet->next()) {
    $array_aufgez_folgen[] = $resultSet->getCurrentValueByName("No");
  }

  $resultSet = $db->executeQuery("SELECT * FROM tatort ".$WHERE);
  $anz = $resultSet->getRowCount();

  echo "<br/><b>Alle Tatort-Folgen $txt_zusatz</b><br/><br/>";
  
  if (($image != "") and (file_exists("/var/www/".$image))) { echo "<img class='responsive_img' style='max-width:400px;' src='$image'><br/><br/>";}
  if ($ansicht == "Alle Folgen")  { echo "<img class='responsive_img' style='max-width:600px;' src='$image_tatort_dir/tatort_banner.jpg'><br/><br/>";}
  
  echo "Anzahl Folgen: $anz<br/>";
  if ($ansicht=="Alle Folgen") {
    echo "<br/>Davon aufgezeichnete Folgen: $anz_rec<br/>";
  }
  echo "<br/>";
  
  echo "<table id='sortTable' class='tab1 tablesorter'>";
  echo "<thead>";
  echo "<tr height=28><th>SE</th><th>EP</th><th>No</th><th>Title</th><th>Ermittler</th><th>Ort</th><th>Sender</th><th>Jahr</th></tr>";
  echo "</thead>";
  echo "<tbody>";
  while($resultSet->next()) {
    $SE = $resultSet->getCurrentValueByName("SE");
    $SE *= 1;
    $No = $resultSet->getCurrentValueByName("No");
    $Title = $resultSet->getCurrentValueByName("Title");
    
    echo "<tr>";
    echo "<td align=right>".$SE."</td>";
    echo "<td align=right>".$resultSet->getCurrentValueByName("EP")."</td>";
    echo "<td align=right>".$No."</td>";
    
    if (in_array($No, $array_aufgez_folgen)) {
      $resultSet2 = $db->executeQuery("SELECT * FROM aufnahmen WHERE Serie='Tatort' and No='$No'");
      $resultSet2->next();
      $aufnahme = $resultSet2->getCurrentValueByName("Aufnahme");
      echo "<td align=left ><a class='b' href='aufnahme.php?info=$aufnahme' >".$Title."</a></td>";  // Titel 
    }
    else { 
      echo "<td align=left >".$Title."</td>";   // Title
    }
    
   // echo "<td>".$resultSet->getCurrentValueByName("Title")."</td>";
    echo "<td><a class=b href='?ermittler=".$resultSet->getCurrentValueByName("Ermittler")."'>".$resultSet->getCurrentValueByName("Ermittler")."</a></td>";
    echo "<td><a class=b href='?ort=".$resultSet->getCurrentValueByName("Ort")."'>".$resultSet->getCurrentValueByName("Ort")."</a></td>";
    echo "<td><a class=b href='?sender=".$resultSet->getCurrentValueByName("Sender")."'>".$resultSet->getCurrentValueByName("Sender")."</a></td>";
    echo "<td><a class=b href='?jahr=".$resultSet->getCurrentValueByName("Jahr")."'>".$resultSet->getCurrentValueByName("Jahr")."</a></td>";
    echo "</tr>";
  }
  echo "</tbody>";
  echo "</table>";
}

if ($ansicht=="Ermittler") {
  echo "<br/><b>Alle Tatort-Ermittler</b><br/><br/>";
  $resultSet = $db->executeQuery("SELECT * FROM tatort GROUP BY Ermittler ORDER BY Ermittler");

  $anz = $resultSet->getRowCount();
  echo "Anzahl Ermittler: $anz<br/><br/>";
  
  echo "<table class=tab1>";
  echo "<tr><th>Ermittler</th></tr>";
  while($resultSet->next()) {
    if ($resultSet->getCurrentValueByName("Ermittler") != "") {
      echo "<tr>";
      echo "<td><a class=b href='?ermittler=".$resultSet->getCurrentValueByName("Ermittler")."'>".$resultSet->getCurrentValueByName("Ermittler")."</a></td>";
      echo "</tr>";
    }
  }
  echo "</table>";
}

if ($ansicht=="Ort") {
  echo "<br/><b>Alle Orte</b><br/><br/>";
  $resultSet = $db->executeQuery("SELECT * FROM tatort GROUP BY Ort ORDER BY Ort");

  $anz = $resultSet->getRowCount();
  echo "Anzahl Orte: $anz<br/><br/>";
  $i=1;
  echo "<table class=tab1>";
  echo "<tr><th colspan=5>Orte</th></tr>";
  echo "<tr>";
  while($resultSet->next()) {
    if ($resultSet->getCurrentValueByName("Ort") != "") {
      echo "<td width=20%><a class=b href='?ort=".$resultSet->getCurrentValueByName("Ort")."'>".$resultSet->getCurrentValueByName("Ort")."</a></td>";
      if ($i==5) { echo "</tr><tr>"; $i=0;}
      $i++;
    }
  }
  echo "</tr>";
  echo "</table>";
}

if ($ansicht=="Sender") {
  echo "<br/><b>Alle Sender</b><br/><br/>";
  $resultSet = $db->executeQuery("SELECT * FROM tatort GROUP BY Sender ORDER BY Sender");

  $anz = $resultSet->getRowCount();
  echo "Anzahl Sender: $anz<br/><br/>";
  $i=1;

  echo "<table class=tab1>";
  echo "<tr><th colspan=6>Sender</th></tr>";
  echo "<tr>";
  while($resultSet->next()) {
    if ($resultSet->getCurrentValueByName("Sender") != "") {
      echo "<td width=17% align='center'><a class=b href='?sender=".$resultSet->getCurrentValueByName("Sender")."'>".$resultSet->getCurrentValueByName("Sender")."<br/>";
      echo "<img width='50px' alt='' src='".$image_tatort_dir."/sender_logo/".$resultSet->getCurrentValueByName("Sender").".png'></a>";
      echo "</td>";
      if ($i==6) { echo "</tr><tr>"; $i=0;}
      $i++;
    }
  }
  echo "</tr>";
  echo "</table>";
}


if ($ansicht=="Jahr") {
  echo "<br/><b>Nach Jahren</b><br/><br/>";
  $resultSet = $db->executeQuery("SELECT * FROM tatort GROUP BY Jahr ORDER BY Jahr");
  
  $anz = $resultSet->getRowCount();
  echo "Anzahl Jahre: $anz<br/><br/>";
  
  echo "<table class=tab1>";
  echo "<tr><th colspan=10 align=center>Jahre</th></tr>";
  $i=0;
  echo "<tr>";
  while($resultSet->next()) {
    if ($resultSet->getCurrentValueByName("Jahr") != "") {
      echo "<td align=center><a class=b href='?jahr=".$resultSet->getCurrentValueByName("Jahr")."'>&nbsp;".$resultSet->getCurrentValueByName("Jahr")."&nbsp;</a></td>";
      $i++;
      if ($i==10) {
        echo "</tr><tr>";
        $i=0;
      }
      
    }
  }
  echo "</tr>";
  echo "</table>";
}

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
