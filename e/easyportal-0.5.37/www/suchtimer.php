<?php
include('includes/kopf.php'); 

?>
<script type="text/javascript" src="/includes/js/jquery.tablesorter.js"></script> 
<script type="text/javascript">
  $(document).ready(function() { $("#sortTable").tablesorter(
    {headers: { 6:{ sorter: false } }, sortList: [[0,0]] }
  ); } );
</script>
<?php

echo "<div id='kopf2'>Timer - epgsearch - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "activetimer.png", $txt_ue_suchtimer);

$epgsearch_conf = "/var/lib/vdr/plugins/epgsearch/epgsearch.conf";
if (file_exists($epgsearch_conf)) {
  //echo "Die Datei $epgsearch_conf existiert";
  $suchtimer_array = file($epgsearch_conf); 

  echo "<table id='sortTable' class='tab1 tablesorter'>";
  echo "<thead>";
  echo "<tr><th>$txt_tab_id</th><th>$txt_tab_suche</th>";
  //echo "<th>$txt_tab_zeit</th>";
  echo "<th>$txt_tab_start</th><th>$txt_tab_stop</th><!--<th>$txt_tab_kanal</th>--><th>$txt_tab_kanalgruppe</th><th>$txt_tab_verzeichnis</th><th>$txt_tab_aktionen</th></tr>";
  echo "</thead>";
  echo "<tbody>";
  foreach ($suchtimer_array as $datensatz) 
  {
    $datensatz_Array = explode(':', $datensatz);
    if ($datensatz_Array[1] == "") { $txt_suche = "*"; }
    else { $txt_suche = $datensatz_Array[1]; }
    if ($datensatz_Array[3] == "") { $txt_start_von = ""; }
    else { $txt_start_von = substr ($datensatz_Array[3],0,2) .":". substr ($datensatz_Array[3],2,2); }
    if ($datensatz_Array[4] == "") { $txt_start_bis = ""; }
    else { $txt_start_bis = substr ($datensatz_Array[4],0,2) .":". substr ($datensatz_Array[4],2,2); }
    echo "<tr>";
    echo "<td align='right'><a class='b' href='suchtimer_view.php?id=$datensatz_Array[0]'>&nbsp;$datensatz_Array[0]&nbsp;</a></td>";  // ID
    echo "<td>$txt_suche</td>"; // Suche
    // echo "<td align='right'>$datensatz_Array[2]</td>"; // Zeit
    echo "<td>$txt_start_von</td>"; // Start von
    echo "<td>$txt_start_bis</td>"; // Start bis
    // echo "<td>$datensatz_Array[5]</td>"; // Kanal 
    echo "<td>$datensatz_Array[6]</td>"; // Kanal / Kanalgruppe
    echo "<td>$datensatz_Array[19]</td>"; // Verzeichnis
    echo "<td align=center><a onmouseover=\"Tip('Suchtimer ansehen')\" href='suchtimer_view.php?id=$datensatz_Array[0]'>&nbsp;<img src=/images/icon/view.png>&nbsp;</a>";
    echo                  "<a onmouseover=\"Tip('Suchtimer editieren')\" href='suchtimer_edit.php?id=$datensatz_Array[0]'>&nbsp;<img src=/images/icon/edit.png>&nbsp;</a>
    
    </td>"; // Aktionen
    echo "</tr>";
  }
  echo "</tbody>";
  echo "</table>";

} else {
    echo "Es ist keine epgsearch-Konfig-Datei vorhanden.";
}


container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
