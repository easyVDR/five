<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>";
echo "Cron";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>Cron</h2>";

$datei = fopen($EASYPORTAL_DOC_ROOT."/log/easyportal.log","r");
$inhalt = fread($datei,filesize($EASYPORTAL_DOC_ROOT."/log/easyportal.log"));
fclose($datei);

// *************************************** Container Begin ****************************************************
container_begin(1, "changelog.png", "Cron");


$resultSet = $db->executeQuery("SELECT * FROM BB_cronjob");

while($resultSet->next()) {
  $datum = date("d.m");
  $D = date("d");
  $M = date("m");
 
  if($resultSet->getCurrentValueByName("Tag") == "*"){
    if($resultSet->getCurrentValueByName("Monat") == "*"){
      if($resultSet->getCurrentValueByName("Lausf") < "$datum"){

        //include($resultSet->getCurrentValueByName("Url"));
        echo "ausführen";
        
        $aendern = "UPDATE BB_cronjob Set Lausf='$datum' WHERE ID = '".$resultSet->getCurrentValueByName("ID")."'";
        $update = $db->executeQuery($aendern);
      }
    } else {
    if($resultSet->getCurrentValueByName("Monat") == $M){
      //include($resultSet->getCurrentValueByName("Url"));
      echo "ausführen";

      $aendern = "UPDATE BB_cronjob Set Lausf='$datum' WHERE ID = '".$resultSet->getCurrentValueByName("ID")."'";
      $update = $db->executeQuery($aendern);
    }
    }
  }
  else {
    if($resultSet->getCurrentValueByName("Tag") == $D){
      if($resultSet->getCurrentValueByName("Monat") == "*"){
        if($resultSet->getCurrentValueByName("Lausf") < $datum){
          //include($resultSet->getCurrentValueByName("Url"));
          echo "ausführen";

          $aendern = "UPDATE BB_cronjob Set Lausf='$datum' WHERE ID = '".$resultSet->getCurrentValueByName("ID")."'";
          $update = $db->executeQuery($aendern);
        }
      } else {
        if($resultSet->getCurrentValueByName("Monat") == $M){
          //include($resultSet->getCurrentValueByName("Url"));
          echo "ausführen";

          $aendern = "UPDATE BB_cronjob Set Lausf='$datum' WHERE ID = '".$resultSet->getCurrentValueByName("ID")."'";
          $update = $db->executeQuery($aendern);
        }
      }
    }
  }
  if(($resultSet->getCurrentValueByName("Tag") == "+") and ($resultSet->getCurrentValueByName("Monat") == "+")){
    echo "ausführen";

    //include($resultSet->getCurrentValueByName("Url"));
  } 
}



container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
