<?php
include('includes/kopf.php'); 

$plugin         = safeget('plugin');
$action         = safeget('action');

$inst_submit    = safepost('inst_submit');
$aktiv_view     = safepost('aktiv_view');
$not_aktiv_view = safepost('not_aktiv_view');
$aktiv_submit   = safepost('aktiv_submit');
$aktivieren     = safepost('aktivieren');
$deaktivieren   = safepost('deaktivieren');

$PLUGIN = "";

if ($plugin!="") {
  shell_exec("sudo $SH_EASYPORTAL ".$action." ".$plugin); 
}

if ($inst_submit!="") {
  exec("sudo $SH_EASYPORTAL stopvdr");

  //  echo "<font color='white'>";
  //  echo "Mehrere Plugins aktivieren/deaktivieren <br/>";
  //  echo "Aktivieren: $aktivieren";
  foreach ($aktivieren as $plugin_a) 
  {
  //  echo $plugin_a." ";
    shell_exec("sudo $SH_EASYPORTAL on_2 ".$plugin_a); 
  }

  //  echo "<br/>DeAktivieren: ";
  foreach ($deaktivieren as $plugin_d) 
  {
  //  echo $plugin_d." ";
    shell_exec("sudo $SH_EASYPORTAL off_2 ".$plugin_d); 
  }
  //  echo "<font color='black'>";
  exec("sudo $SH_EASYPORTAL startvdr");
}

echo "<div id='kopf2'>";
echo "<a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_addons.php'); 

echo "<p>";
echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "plugin.png", $txt_ue_plugins);

exec("sudo $SH_EASYPORTAL lst",$PLUGINS);
echo $SH_EASYPORTAL;

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

echo "<table border=0><tr>";
echo "<td>$txt_aktive_plugins_anzeigen:</td><td>";
if ($aktiv_view == "") {$aktiv_view = "1";}
if ($aktiv_view == 1) {$checked_1="checked"; $checked_0=""; }
else {$checked_0="checked"; $checked_1=""; }
echo "<input type='radio' name='aktiv_view' value='1' $checked_1 onChange='this.form.submit()'> $txt_yes &nbsp;";
echo "<input type='radio' name='aktiv_view' value='0' $checked_0 onChange='this.form.submit()'> $txt_no </td>";
echo "</tr><tr>";
echo "<td>$txt_nicht_aktive_plugins_anzeigen:</td><td>";
if ($not_aktiv_view == "") {$not_aktiv_view = "1";}
if ($not_aktiv_view == 1) {$checked_1="checked"; $checked_0=""; }
else {$checked_0="checked"; $checked_1=""; }
echo "<input type='radio' name='not_aktiv_view' value='1' $checked_1 onChange='this.form.submit()'> $txt_yes &nbsp;";
echo "<input type='radio' name='not_aktiv_view' value='0' $checked_0 onChange='this.form.submit()'> $txt_no </td>";
echo "</tr></table>";

echo "<br/>";
echo "<input type='submit' name='inst_submit' value='$txt_b_aktiv_deaktiv' onclick=\"wait('WaitAktivieren')\">";
echo "<br/><br/>";

if (($aktiv_view == 1) or ($not_aktiv_view == 1)) {
  // ******************* Plugin-Tabelle ********************
  echo "<table class='tab1'>";
  echo "<tr height=35px'>";
  echo "<th>$txt_plugin</th>";
  echo "<th class='kl600aus' align=center>$txt_beschreibung</th>";
  echo "<th class='kl800aus' align=center>$txt_status</th>";
  echo "<th align='center'>$txt_aktivieren</th>";
  echo "<th align='center'>DeAktiv.</th>";
  echo "<th align='center'>Einzelakt.</th>";
  echo "</tr>";

  foreach ($PLUGINS as $ROW) {
    if(substr($ROW,0,1)<>"-") {
      $DATA = explode("\"", $ROW);
      $ANZAHL = count($DATA);
      for ($i = 0; $i < $ANZAHL; $i++) {
        if(strlen($DATA[$i])>1) {
          if(trim($DATA[$i]) == "name=") {
            $PLUGIN = htmlentities(str_replace("=", "", $DATA[$i+1]));
          }
          if(trim($DATA[$i]) == "info=") {
            $INFO = (str_replace("=", "", $DATA[$i+1]));
          }
          if(trim($DATA[$i]) == "active=") {
            if(str_replace("=", "", $DATA[$i+1])=="yes") {
              $BUTTON  = "ok.png";
              $ACT     = "off";
              $status  = $txt_aktiviert;
              $MESSAGE = $TIP_PLUGINS_OFF;
            } else {
              $BUTTON  = "ok_no.png";
              $ACT     = "on";
              $status  = $txt_deaktiviert;
              $MESSAGE = $TIP_PLUGINS_ON;
            }
          }
        }
      }
      echo "<tr>";
      if ($PLUGIN!="") {
        if ( (($aktiv_view == 1) or (($aktiv_view == 0) and ($status == $txt_deaktiviert))) and (($not_aktiv_view == 1) or (($not_aktiv_view == 0) and ($status == $txt_aktiviert)) ))
        {
          echo "<td><a class='b' href='view_plugin.php?plugin=$PLUGIN'>$PLUGIN</a></td>";
          echo "<td class='kl600aus' align=left>$INFO</td>";
          echo "<td class='kl800aus' align=right>$status</td>";
          echo "<td align='center'>";  
          if ($ACT== "on") { $daten[8] = "<input value='$PLUGIN' name='aktivieren[]' type='Checkbox'>";} else { $daten[8] = "";}
          echo $daten[8];
          echo "</td><td align='center'>";
          if ($ACT != "on") { $daten[9] = "<input value='$PLUGIN' name='deaktivieren[]' type='Checkbox'>";} else { $daten[9] = "";}
          echo $daten[9];
          echo "</td><td align='center'>";
          echo "<a href='plugins.php?plugin=$PLUGIN&action=$ACT' onClick=\"if(confirm('$txt_stop_vdr')); else return false;\" onmouseover=\"Tip('$MESSAGE')\">";
          echo "<img src='images/icon/$BUTTON'></a>";
          echo "</td></tr>";
        }
      }
    }
  }
  echo "</table>";
  // ******************* Ende Plugin-Tabelle ********************
  echo "<br/>";
  echo "<input type='submit' name='inst_submit' value='$txt_b_aktiv_deaktiv' onclick=\"wait('WaitAktivieren')\">";
  echo "</form>";

}
else
{
  echo "$txt_error1</td></tr><br/>";
}

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_hilfe);

echo "$txt_hilfe1<br/>";
echo "$txt_hilfe2<br/>";
echo "$txt_hilfe3<br/><br/>";
echo "$txt_hilfe4<br/><br/>";

echo "<a href='addons.php'><button>$txt_b_addons_install</button></a>";

container_end();
// *************************************** Container Ende ****************************************************


echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>