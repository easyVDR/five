<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');

?>
<script type="text/javascript" src="/includes/js/jquery.tablesorter.js"></script> 
<script type="text/javascript">
  $(document).ready(function() { 
    $("#sortTable").tablesorter( {sortList: [[1,0]] }  ); 
    
  } );
  

</script>
<?php

</script>
<?php

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_tools.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Lan.png", $txt_ue_netzwerk_geraete);

if (!file_exists("/var/www/databases/portal/netzwerkgeraete.txt")) { 
  echo "$txt_error1<br/><br/>";
  //wait('WaitLANSuche'); redirect('/netzwerk_geraete_update.php'); 
  echo "<a href='netzwerk_geraete_update.php' onclick=\"wait('WaitLANSuche')\"><button>".$txt_b_suchen."</button></a>";
}
else {

  // Rechner aufwachen
  $wol = safeget('WOL');
  if ($wol != "") { exec("sudo $SH_BEFEHL etherwake $wol"); }

  // Online Rechner ermitteln
  exec("sudo $SH_BEFEHL nmap -sP 192.168.1.0/24 |grep 'Nmap scan report' | cut -d'(' -f2 |cut -d')' -f1", $on);

  echo "<a href='netzwerk_geraete_update.php' onclick=\"wait('WaitLANSuche')\"><button>".$txt_b_suchen."</button></a>";
  echo "<br><br>";

  $ansicht = safepost('ansicht');
  if ($ansicht == "") {$ansicht = "alle";}
  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
  if ($ansicht == "vdr") { $checked_ansicht_vdr = "checked"; $checked_ansicht_alle = "";} else { $checked_ansicht_vdr = ""; $checked_ansicht_alle = "checked";}
  echo "$txt_anzeige $txt_auswahl_vdrs   <input type='radio' name='ansicht' value='vdr' $checked_ansicht_vdr onChange='this.form.submit()' />&nbsp;&nbsp;";
  echo "$txt_alle_Geräte <input type='radio' name='ansicht' value='alle' $checked_ansicht_alle onChange='this.form.submit()' /><br/><br/>";
  echo "</form>";

  echo "<table id='sortTable' class='tab1 tablesorter'>";
  echo "<thead>";
  echo "  <tr>";
  echo "    <th align='center'>$txt_online</th>";
  echo "    <th>$txt_ip</th>";
  echo "    <th>$txt_name</th>";
  echo "    <th class='kl700aus'>$txt_mac</th>";
  echo "    <th class='kl1200aus'>$txt_hersteller</th>";
  echo "    <th>$txt_is_vdr</th>";
  echo "    <th class='kl800aus'>VDR<br>Version</th>";
  echo "    <th class='kl900aus'>streamdev<br>server</th>";
  echo "    <th class='kl900aus'>streamdev<br>client</th>";
  echo "    <th class='kl1000aus'>peer</th>";
  echo "    <th class='kl1100aus'>remote<br>timer</th>";
  echo "    <th class='kl1100aus'>remote<br>osd</th>";
  echo "    <th>WOL</th>";
  echo "  </tr>";
  echo "</thead>";

  echo "<tbody>";
  if ($ansicht == "vdr") { $WHERE = " where istVDR=1"; } else { $WHERE = ""; } 
  $resultSet = $db->executeQuery("SELECT * FROM netzwerkgeraete $WHERE");
  while($resultSet->next()) {
    $ip = $resultSet->getCurrentValueByName("IP");

    echo "<tr height=22><td align='center' id='i".str_replace(".", "", $ip)."'>";
    if (in_array($resultSet->getCurrentValueByName("IP"), $on)) { echo "<a style='visibility:hidden;'>x</a><img src='images/led_green.gif' class='icon'><a style='visibility:hidden;'>x</a>"; } // Wenn IP Online
    echo "</td>";
    echo "<td>".$resultSet->getCurrentValueByName("IP")."</td>";
    echo "<td>".$resultSet->getCurrentValueByName("Name")."</td>";
    echo "<td class='kl700aus'>".$resultSet->getCurrentValueByName("MAC")."</td>";
    echo "<td class='kl1200aus'>".$resultSet->getCurrentValueByName("Hersteller")."</td>";
    echo "<td>".$resultSet->getCurrentValueByName("istVDR")."</td>";
    echo "<td class='kl800aus'>".$resultSet->getCurrentValueByName("VDRVersion")."</td>";
    echo "<td class='kl900aus'>".$resultSet->getCurrentValueByName("streamdevserver")."</td>";
    echo "<td class='kl900aus'>".$resultSet->getCurrentValueByName("streamdevclient")."</td>";
    echo "<td class='kl1000aus'>".$resultSet->getCurrentValueByName("peer")."</td>";
    echo "<td class='kl1100aus'>".$resultSet->getCurrentValueByName("remotetimer")."</td>";
    echo "<td class='kl1100aus'>".$resultSet->getCurrentValueByName("remoteosd")."</td>";
    if ((!in_array($resultSet->getCurrentValueByName("IP"), $on)) and ($resultSet->getCurrentValueByName("MAC")!="")) 
      { echo "<td align='center'><a onmouseover=\"Tip('$TIP_WOL')\" href='".$_SERVER['PHP_SELF']."?WOL=".$resultSet->getCurrentValueByName("MAC")."' class='b' onclick=\"wait('WaitSeite')\"><a style='visibility:hidden;'>x</a><img src=/images/icon/power.png width=14px><a style='visibility:hidden;'>x</a></a></td>"; } 
    else { echo "<td>&nbsp;</td>"; }
    echo "</tr>";
  }
  echo "</tbody>";
  echo "</table>";
}

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
