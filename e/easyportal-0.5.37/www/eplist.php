<?php
include('includes/kopf.php'); 

$action      = safeget('action');

// $eplists_dir  wird im kopf.php konfiguriert

$dest_dir = substr($eplists_dir, 0, strrpos($eplists_dir,"/")+1 ); 
$dest_dir2 = substr($eplists_dir, 0, strrpos($eplists_dir,"/") ); 
if (!file_exists($eplists_dir."/")) {
  exec("sudo $SH_BEFEHL mkdir $dest_dir");
  exec("sudo $SH_EASYPORTAL ownervdr $dest_dir");
  exec("sudo $SH_BEFEHL mkdir $eplists_dir");
  exec("sudo $SH_EASYPORTAL ownervdr $eplists_dir");
  $action="dl";
}

if ($action=="dl") {
  exec("sudo $SH_EASYPORTAL dl_eplists $dest_dir");
}

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_download);

echo "$txt_text1<br/><br/>";
echo "<a href=".$_SERVER['PHP_SELF']."?action=dl><button>$txt_b_download</button></a><br/>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", "eplists - Info");

exec("ls $eplists_dir -l -h -t |head -20 |grep .episodes |cut -d' ' -f5-", $last_updates);
//echo $last_updates;

echo "<b>$txt_text2</b><br/><br/>";

echo "<table class=tab1>";
echo "<tr><th>$txt_tab_datum</th><th>$txt_tab_zeit</th><th>$txt_tab_serien</th><th>$txt_tab_size</th></tr>";
$nr=0;
foreach ($last_updates as $file) { // Ausgabeschleife
  $new_string = trim($file);
  $stringposition = strpos($new_string, " ");
  $groesse = substr($new_string, 0, $stringposition);
  $new_string = trim(substr($new_string, $stringposition+1));
  $stringposition = strpos($new_string, " ");
  $month = substr($new_string, 0, $stringposition);
  $new_string = trim(substr($new_string, $stringposition+1));
  $stringposition = strpos($new_string, " ");
  $day = substr($new_string, 0, $stringposition);
  $datum = $day.".".$month;
  $new_string = trim(substr($new_string, $stringposition+1));
  $stringposition = strpos($new_string, " ");
  $zeit = substr($new_string, 0, $stringposition);
  $new_string = trim(substr($new_string, $stringposition+1));
  $file_anzeige = Str_replace("_", " ", substr($new_string,0,Strlen($new_string)-9));

  echo "<tr>";
  echo "<td align=right>$datum</td>";
  echo "<td>$zeit</td>";
  echo "<td><a class='b' href='serie.php?serie=$file_anzeige'>$file_anzeige</td>";
  echo "<td align=right>$groesse</td>";
  echo "</tr>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
