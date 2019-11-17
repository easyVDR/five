<?php
include('includes/kopf.php'); 
include('includes/function_linecolor.php'); 

$id = safeget('id');
if ($id=="") { $id = safepost('id');}

$info_version = safepost('info_version');

$resultSet = $db->executeQuery("SELECT * FROM easyinfo WHERE id='$id'");
$resultSet->next();
$Befehl       = $resultSet->getCurrentValueByName("Befehl");
$Name         = $resultSet->getCurrentValueByName("Name");
$Kategorie    = $resultSet->getCurrentValueByName("Kategorie");
$Highlight    = $resultSet->getCurrentValueByName("Highlight");
$Beschreibung = $resultSet->getCurrentValueByName("Beschreibung");

$info_version = safepost('info_version');

echo "<div id='kopf2'>";
echo "<a href='easyinfo.php' class='w'>EasyInfo</a> - $Kategorie - '$Name'";
echo "</div>";

echo "<div id='inhalt'>";

echo "<center>";
include('includes/subnavi_easyinfo.php'); 

echo "<br/>";
echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='id' value='$id'>";

if ($info_version == "") {$info_version = "aktuell";}
if ($info_version == "aktuell") {$checked_1="checked"; $checked_0=""; }
else {$checked_0="checked"; $checked_1=""; }
echo "<input type='radio' name='info_version' value='aktuell' $checked_1 onChange='this.form.submit()'> Aktuell &nbsp;";
echo "<input type='radio' name='info_version' value='save' $checked_0 onChange='this.form.submit()'> gespeicherte Auswahl <br/>";

echo "</form>";


if ($info_version == "aktuell"){
  // ********************************* Aktuelle EasyInfo *************************************
  echo "<h2>easyInfo: Aktuell</h2>";
  echo "<center>";

  // *************************************** Container Begin '' ****************************************************
  container_begin(1, "Info.png", $Name);

  // echo $Befehl."<br>";
  if ($id == 0) // ERRORS IM LOGFILE
  {
    exec("sudo $SH_EASYINFO_ERROR"); 

    $ausgabe = file($EASYPORTAL_DOC_ROOT."log/easyinfo/easyinfo.log.error");
    
    echo "<pre>";
    $erstezeile = 1;
    foreach ($ausgabe as $line) 
    {
      $line_color = linecolor($line, $Highlight, $erstezeile);
      echo $line_color;
      $erstezeile=$erstezeile+1;
    } 
    echo "</pre>";
  }
  else
  {
    exec("sudo $SH_BEFEHL $Befehl", $ausgabe); 
    echo "<pre>";
    $erstezeile = 1;
    foreach ($ausgabe as $line) 
    {
      $line_color = linecolor($line, $Highlight, $erstezeile);
      echo $line_color."<br/>";
      $erstezeile=$erstezeile+1;
    } 
    echo "</pre>";
  }

  container_end();
  // *************************************** Container Ende ****************************************************
}

if ($info_version == "save"){
  // ********************************* gespeicherte EasyInfo *************************************
  $akt_easyinfo = substr(readlink ( '/var/www/log/easyinfo/easyinfo.log' ), 22);
  echo "<h2>easyInfo: '$akt_easyinfo'</h2>";

  // *************************************** Container Begin '' ****************************************************
  container_begin(1, "Info.png", $Name);

  $datei = fopen($EASYPORTAL_DOC_ROOT."log/easyinfo/easyinfo.log","r");

  $anzeigen="n";
  echo "<pre>";
  $i=1;
  $erstezeile = 1;
  while (!feof($datei)) {
    $line = fgets($datei); 
    if ($line!=str_replace("##### $Name  ","",$line)) {
      $anzeigen="y";
      $line = fgets($datei);    
      $line = fgets($datei);    
      $line = fgets($datei);    
    }
    if ($line!=str_replace("#####################################################","",$line)) { 
       $anzeigen="n";
    }    
    if ($anzeigen=="y") {
      $line_color = linecolor($line, $Highlight, $erstezeile);
      echo $line_color;
      $erstezeile=$erstezeile+1;
    }
  }
  fclose($datei);
  echo "</pre>";
  container_end();
  // *************************************** Container Ende ****************************************************
}  // gespeicherte EasyInfo 


if ($Beschreibung!=""){

  // *************************************** Container Begin '' ****************************************************
  container_begin(1, "Info.png", "Hilfe");
  echo $Beschreibung."<br/>";
  container_end();
  // *************************************** Container Ende ****************************************************
  
}

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>