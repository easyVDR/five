<?php
include('includes/kopf.php'); 
include('includes/function_rec_countdown.php'); 

$installed_view     = safepost('installed_view');
$not_installed_view = safepost('not_installed_view');
$inst_submit        = safepost('inst_submit');
$installieren       = safepost('installieren');
$deinstallieren     = safepost('deinstallieren');
$auch_aktivieren    = safepost('auch_aktivieren');

//echo "installed_view=$installed_view<br/>";

echo "<div id='kopf2'>$txt_einstellungen - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_addons.php'); 

echo "<h2>$txt_ueberschrift</h2>";

if ($inst_submit == $txt_b_install_deinstall) {
  // Es wurde 'installieren / Deinstallieren' geklickt
  // *************************************** Container Begin ****************************************************
  container_begin(1, "install.png", $txt_ue_addons);

  exec("sudo $SH_EASYPORTAL stopvdr");
  exec("sudo $SH_UPDATE update");
  
  if ($installieren != "") {
    if ($auch_aktivieren == 1) { echo "<b>$txt_i_install_aktiv:</b><br/><br/>"; } else { echo "<b>$txt_i_install:</b><br/><br/>"; }
    foreach ($installieren as $addon)  { // Installieren
      shell_exec("sudo $SH_UPDATE install $addon");
      echo $addon;
      $plugin = str_replace("vdr-plugin-", "", $addon);
      if (($auch_aktivieren == 1) and ($addon != $plugin)) { 
        shell_exec("sudo $SH_EASYPORTAL on_2 $plugin"); 
        echo " ($plugin aktiviert)";
      }
      echo "<br/>";
    }
  }
  echo "<br/>";
  if ($deinstallieren != "") {
    echo "<b>$txt_i_de_install:</b><br/><br/>";
    foreach ($deinstallieren as $addon) { // Deinstallieren
      echo "$addon";
      $plugin = str_replace("vdr-plugin-", "", $addon);
      if ($addon != $plugin) {
        shell_exec("sudo $SH_EASYPORTAL off_2 $plugin"); 
        echo " ($plugin de-aktiviert)";
      }
      shell_exec("sudo $SH_UPDATE remove $addon");
      echo "<br/>";
    }
  }
  exec("sudo $SH_BEFEHL apt-get clean");
  exec("sudo $SH_EASYPORTAL startvdr");
  echo "<br/><br/>";
  echo "<a href='addons.php'><button>$txt_b_zu_addons</button></a>&nbsp;&nbsp;";
  echo "<a href='plugins.php'><button>$txt_b_zu_plugins</button></a>";
  echo "<br/><br/>";
}
else {
  // Es wurde noch nicht 'installieren geklickt'
  // Video-Out bestimmen
  $datei = fopen('/var/log/easyvdr/setup.state', 'r');
  while (!feof($datei))  {
    $zeile = fgets($datei);
    if (substr ($zeile,0,17) == "easyvdr_video_out") {  
      $video_out = substr($zeile,18,strlen($zeile));
      $video_out = trim(str_replace("\"", "", $video_out));
    }
  }
  fclose($datei);
  switch ($video_out) {
    case "ff":     $video_out_matrix_nr = "0"; break;
    case "ff-hd":  $video_out_matrix_nr = "1"; break;
    case "ehd":    $video_out_matrix_nr = "2"; break;
    case "nvidia": $video_out_matrix_nr = "3"; break;
    case "pvr350": $video_out_matrix_nr = "4"; break;
    case "x":      $video_out_matrix_nr = "5"; break;
    case "ati":    $video_out_matrix_nr = "6"; break;
    case "frei":   $video_out_matrix_nr = "7"; break;
  }

  // *************************************** Container Begin ****************************************************
  container_begin(1, "install.png", $txt_ue_addons);

  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

  echo "$txt_videoout: <b>$video_out</b> <br/><br/>";

  echo "<table border=0><tr>";
  echo "<td>$txt_installed_addons:</td><td><nobr>";
  if ($installed_view == "") {$installed_view = "1";}
  if ($installed_view == 1) {$checked_1="checked"; $checked_0=""; }
  else {$checked_0="checked"; $checked_1=""; }
  echo "<input type='radio' name='installed_view' value='1' $checked_1 onChange='this.form.submit()'> $txt_yes &nbsp;";
  echo "<input type='radio' name='installed_view' value='0' $checked_0 onChange='this.form.submit()'> $txt_no </td>";
  echo "</nobr></tr><tr>";
  echo "<td>$txt_not_installed_addons:</td><td><nobr>";
  if ($not_installed_view == "") {$not_installed_view = "1";}
  if ($not_installed_view == 1) {$checked_1="checked"; $checked_0=""; }
  else {$checked_0="checked"; $checked_1=""; }
  echo "<input type='radio' name='not_installed_view' value='1' $checked_1 onChange='this.form.submit()'> $txt_yes &nbsp;";
  echo "<input type='radio' name='not_installed_view' value='0' $checked_0 onChange='this.form.submit()'> $txt_no </td>";
  echo "</nobr></tr><tr>";
  echo "<td>$txt_auch_aktivieren:</td>";
  echo "<td><input value='1' name='auch_aktivieren' type='Checkbox'></td>";
  echo "</tr></table>";
  echo "<br/>";

  if (($installed_view==1) or ($not_installed_view==1)) {
    echo "<font color=red>$txt_attention</font><br/>";
    echo "<div id='rec_countdown'>$txt_zeit_berechnung</div><br/>";
    echo "<input type='submit' name='inst_submit' value='$txt_b_install_deinstall' onclick=\"wait('WaitInstall')\">";
    echo "<br/><br/>";

    exec("dpkg --get-selections| grep -v deinstall", $installed_array);
    $installed = implode(" ", $installed_array);
   
    $datei = fopen('/usr/share/easyvdr/setup/setup.addonlist.v4', 'r');
    echo "<table class='tab1'><tr>";
    echo "<th class='kl600aus'>$txt_kurzbeschreibung</th>"; // Menüpunkt daten[0]
    echo "<th class='kl700aus'>$txt_beschreibung</th>";     // Kommentar daten[1]
    //echo "<th>+/-</th>";                                  // +//-      daten[2]
    echo "<th>$txt_paket</th>";                             // debname   daten[3]
    //echo "<th>addi1</th>";                                // daten[4]
    //echo "<th>addi2</th>";                                // daten[5]
    //echo "<th>Confl_Matrix</th>";                         // daten[6]
    echo "<th class='kl800aus' align=center>$txt_status</th>"; // daten[7]
    echo "<th>$txt_installeren</th>";                       // daten[8]
    echo "<th align=center>$txt_deinstallieren</th></tr>";  // daten[9]
    while (!feof($datei)) {
      $zeile = fgets($datei);
      $daten = preg_split("/\t+/", $zeile);
      if (($daten[0]<>"vers.5") and ($daten[0]<>"XXX") and ($daten[0]<>"Menupunkt:") and ($daten[0]<>"") and ($daten[2]=="+")) {
        if (substr ($daten[6],$video_out_matrix_nr,1) == "X") {
          $daten[1] = str_replace("ae", "ä", $daten[1]);
          $daten[1] = str_replace("oe", "ö", $daten[1]);
          //$daten[1] = str_replace("ue", "ü", $daten[1]);
          $daten[1] = str_replace(" fuer ", " für ", $daten[1]);
          $daten[1] = str_replace("Ue", "Ü", $daten[1]);
          $pos = strpos($installed, $daten[3]);
          if ($pos == "0") { $daten[7] = $txt_deinstalliert;} else { $daten[7] = $txt_installiert;}
          if ($pos == "0") { $daten[8] = "<input value='$daten[3]' name='installieren[]' type='Checkbox'>";} else { $daten[8] = "";}
          if ($pos != "0") { $daten[9] = "<input value='$daten[3]' name='deinstallieren[]' type='Checkbox'>";} else { $daten[9] = "";}
          if ( (($installed_view == 1) or (($installed_view == 0) and ($daten[7] == $txt_deinstalliert))) and 
               (($not_installed_view == 1) or (($not_installed_view == 0) and ($daten[7] == $txt_installiert))) ){
            echo "<tr>";  
            echo "<td class='kl600aus' align=left>$daten[0]</td>";  // Menüpunkt/Kurzbeschreibung
            echo "<td class='kl700aus' align=left>$daten[1]</td>";  // Kommentar
        //  echo "<td>$daten[2]</td>";  // +//-
            if (substr($daten[3], 0, 11) == "vdr-plugin-") {
               $plg = substr($daten[3], 11);
               echo "<td><a class='b' href='view_plugin.php?plugin=$plg'>$daten[3]</a></td>";  // debname
            }
            else {        
               echo "<td>$daten[3]</td>";  // debname
            }
        //  echo "<td>$daten[4]</td>";  // addi1
        //  echo "<td>$daten[5]</td>";  // addi2
        //  echo "<td>$daten[6]</td>";  // Confl_Matrix
            echo "<td class='kl800aus' align=right>$daten[7]</td>"; // Installiert
            echo "<td align=center>$daten[8]</td>";  // Installieren
            echo "<td align=center>$daten[9]</td>";  // De-Installieren
            echo "</tr>";
          }
        }
      }
    }
    echo "</table>";
    fclose($datei);

    echo "<br/><br/>";
    echo "<input type='submit' name='inst_submit' value='$txt_b_install_deinstall' onclick=\"wait('WaitInstall')\">";
  }
  else
  {
    echo "$txt_keine_plugins<br/>";
  }  

  echo "<br/><br/>";
    
  echo "</form>";

} // nicht installieren

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_hilfe);

echo "$txt_hilfe1<br/>";
echo "$txt_hilfe2<br/>";
echo "$txt_hilfe3<br/><br/>";

echo "$txt_aktivieren<br/><br/>";
echo "<a href='plugins.php'><button>$txt_b_aktivieren</button></a><br/><br/>";

container_end();
// *************************************** Container Ende ****************************************************


echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
