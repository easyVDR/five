<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_easyinfo.php'); 

exec("sudo $SH_EASYPORTAL delAltlasten");

$log_root = $EASYPORTAL_DOC_ROOT."log";
$log_dir  = $log_root."/easyinfo/";
$log_dir2 = $log_root."/easyinfo";
if (!file_exists($log_root)) { exec("mkdir $log_root"); }
if (!file_exists($log_dir2)) { exec("mkdir $log_dir2"); }


if (!file_exists($log_dir."*.log")) {
  $exist = glob($log_dir."*.log");
  if ($exist[0]=="") {
    exec("sudo $SH_EASYINFO2");	 
  }
}

$akt_easyinfo = substr(readlink ( '/var/www/log/easyinfo/easyinfo.log' ), 13);
echo "<h2>easyInfo: '$akt_easyinfo'</h2>";
echo "<div class='links'>";

function make_link($Kategorie) {
  $db = new Database("portal");
  $resultSet = $db->executeQuery("SELECT * FROM easyinfo WHERE Kategorie='$Kategorie'");
  while($resultSet->next()) {
    $id   = $resultSet->getCurrentValueByName("id");
    $Name = $resultSet->getCurrentValueByName("Name");
    echo "<a class='b' href='view_easyinfo.php?id=$id'>$Name</a><br/>";
  }
}

// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_allgemein);
make_link("all");
container_end();
// *************************************** Container Ende ****************************************************
// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_lan);
make_link("net");
container_end();
// *************************************** Container Ende ****************************************************
// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_audio);
make_link("audio");
container_end();
// *************************************** Container Ende ****************************************************
// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_setup);
make_link("setup");
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_vdr);
make_link("vdr");
container_end();
// *************************************** Container Ende ****************************************************
// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_hardware);
make_link("hw");
container_end();
// *************************************** Container Ende ****************************************************
// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_harddisk);
make_link("hd");
container_end();
// *************************************** Container Ende ****************************************************
// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_fb);
make_link("remote");
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig
echo "</center>";
echo "</div>";

include('includes/fuss.php'); 
?>