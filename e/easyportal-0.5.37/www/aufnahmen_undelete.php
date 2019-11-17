<?php
include('includes/kopf.php'); 

$undelete = safeget('undelete');
if ($undelete != "") {
  // Aufnahme widerherstellen
  $undelete = "/video0/$undelete";
  $undelete = str_replace('"', '\"', $undelete);
  $del3 = str_replace("del", "rec", $undelete);
  exec("sudo $SH_BEFEHL mv $undelete $del3");
  exec("sudo $SH_BEFEHL touch /video0/.update"); 
}

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", $txt_ue_del_rec);

$aufnahmen_del_anz = shell_exec("find /video0/ -type f -name info |cut -d'/' -f3- |grep .del/info |wc -l");


if ($aufnahmen_del_anz > 0) {
  echo "$txt_anz_aufnahmen1 $aufnahmen_del_anz $txt_anz_aufnahmen2.<br/><br/>";
  exec("find /video0/ -type f -name info |cut -d'/' -f3- |grep .del/info", $aufnahmen_del);

  echo "<table class=tab1>";
  echo "<tr><th>$txt_tab_del_rec</th><th>$txt_tab_datum</th><th>$txt_tab_aktionen</th></tr>";
  foreach ($aufnahmen_del as $aufnahme) 
  {
    $aufnahme = substr ($aufnahme, 0, strlen($aufnahme)-5);
    $aufnahme_ansicht = substr($aufnahme,0,strrpos($aufnahme,"/") ); 
    $aufnahme_ansicht = str_replace("_", " ", $aufnahme_ansicht);
    $datum_all = substr($aufnahme, strrpos($aufnahme,"/") + 1, strlen($aufnahme) ); 
    $datum = substr($datum_all, 0, 10);
    $zeit  = str_replace('.', ':', substr($datum_all, 11, 5));
    $datum_zeit = $datum." ".$zeit;
    $dir_encode = urlencode($aufnahme);

    echo "<tr>";
    echo "<td>$aufnahme_ansicht</td>";
    echo "<td>$datum_zeit</td><td>";
    echo "<a onmouseover=\"Tip('$TIP_wiederherstellen')\" href='?undelete=$dir_encode'>&nbsp;<img src=/images/icon/wiederherstellen.png>&nbsp;</a>";
    echo "</td>";
    echo "</tr>";
  }
  echo "</table>";
}
else {
  echo "$txt_no_del<br/>";
}

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
