<?php
include('includes/kopf.php'); 

$channel = safeget('channel');
$quelle  = safeget('quelle');
if ($quelle=="") {$quelle="alle";}

if (($channel) and ($_GET['aktion']=="aktiv")){
  exec("sudo $SH_EASYPORTAL chanact $channel");
  $txt_debug_meldungen[] = "Kanalliste aktiviert: $channel";
} 
if (($channel) and ($_GET['aktion']=="del")){
  exec("rm /var/lib/vdr/channels/$channel");
  $txt_debug_meldungen[] = "Kanalliste gelöscht: $channel"; 
} 

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Channels.png", $txt_ue_channellist);

echo "<form action=".$_SERVER['PHP_SELF']." method='get'>\n";

if ($quelle=="alle")      {$checked_a="checked";} else {$checked_a="";}
if ($quelle=="favoriten") {$checked_f="checked";} else {$checked_f="";}
if ($quelle=="DVB-S")     {$checked_s="checked";} else {$checked_s="";}
if ($quelle=="DVB-C")     {$checked_c="checked";} else {$checked_c="";}
if ($quelle=="DVB-T")     {$checked_t="checked";} else {$checked_t="";}

echo "<table border=0><tr>";
echo   "<td>$txt_quelle_waehlen:</td><td>";
echo   " <nobr><input type='radio' name='quelle' value='alle' $checked_a onChange='this.form.submit()'> $txt_alle</nobr> ";
echo   " <nobr><input type='radio' name='quelle' value='favoriten' $checked_f onChange='this.form.submit()'> $txt_favoriten</nobr>&nbsp;";
echo   " <nobr><input type='radio' name='quelle' value='DVB-S' $checked_s onChange='this.form.submit()'>&nbsp;DVB-S</nobr>&nbsp;";
echo   " <nobr><input type='radio' name='quelle' value='DVB-C' $checked_c onChange='this.form.submit()'>&nbsp;DVB-C</nobr>&nbsp;";
echo   " <nobr><input type='radio' name='quelle' value='DVB-T' $checked_t onChange='this.form.submit()'>&nbsp;DVB-T</nobr>&nbsp; </td>";
echo "</tr>";
echo "</table>";

echo "<br/>";
echo "<img src='images/dvb/$quelle.png'><br/><br/>";

echo "<table class='tab1'>";
echo "<tr height=30>";
echo "<th>$txt_channellists</th>";
echo "<th align='center'>Aktionen</th>";
echo "</tr>";

$default=shell_exec("ls -l /var/lib/vdr/channels.conf | awk '{ print $11 }' | awk -F'/' '{ print $6 }'");
$default=substr(ltrim($default),0,-1);

$alledateien = scandir('/var/lib/vdr/channels');

foreach ($alledateien as $file) { // Ausgabeschleife
  $file=ltrim($file);
  if ($file != "." && $file != "..") {

   $file_anzeige = Str_replace("_", " ", substr($file,0,Strlen($file)-5));
   $icon="";

   switch ($quelle) {
    case "DVB-S":
    case "DVB-C":
    case "DVB-T":
      if (strtoupper(substr($file,0,5))==$quelle) {
        $ausgabe = "1"; 
        $file_anzeige = SubStr($file_anzeige, 6, StrLen($file_anzeige));
        if (substr($file_anzeige,0,2)=="at") { $icon="<img src='images/flaggen/at.png'> ";}
        if (substr($file_anzeige,0,2)=="de") { $icon="<img src='images/flaggen/de.png'> ";}
        if (substr($file_anzeige,0,2)=="dk") { $icon="<img src='images/flaggen/dk.png'> ";}
        if (substr($file_anzeige,0,2)=="fi") { $icon="<img src='images/flaggen/fi.png'> ";}
        if (substr($file_anzeige,0,2)=="ie") { $icon="<img src='images/flaggen/ie.png'> ";}
        if (substr($file_anzeige,0,2)=="nl") { $icon="<img src='images/flaggen/nl.png'> ";}
      } 
      else {
        $ausgabe = "0"; } 
      break;
    case "favoriten":
      if ( (strtoupper(substr($file,0,5))!="DVB-S") and (strtoupper(substr($file,0,5))!="DVB-C") and (strtoupper(substr($file,0,5))!="DVB-T")){
        $ausgabe = "1"; 
      } 
      else {
        $ausgabe = "0"; } 
      break;
    case "alle":
    default:
      $ausgabe = "1";
      break;
   }

   if ($ausgabe == "1") {
      // $t = substr($file_anzeige, 7, strlen(file_anzeige));
      if ($file == $default) {
        $checkinfo=" - aktuell aktiviert";
        echo "<tr><td align='left'>$icon <b>$file_anzeige $checkinfo</b></td>\n";
      } else {
        $checkinfo="";
        echo "<tr><td align='left'>$icon $file_anzeige $checkinfo</td>\n";
      }
      echo "<td align='left'><a onmouseover=\"Tip('$TIP_view')\" href='channel_edit.php?channel=$file'>&nbsp;<img src=/images/icon/view.png>&nbsp;</a>";
      
      if ($file != $default) {
        echo "<a onmouseover=\"Tip('$TIP_aktiv')\" href='".$_SERVER['PHP_SELF']."?channel=$file&quelle=$quelle&aktion=aktiv' onclick=\"wait('WaitSave')\">&nbsp;<img src=/images/icon/ok.png>&nbsp;</a>";
        echo "<a onmouseover=\"Tip('$TIP_sort')\" href='channel_move.php?channel=$file'>&nbsp;<img src=/images/icon/rename.png>&nbsp;</a>";
        echo "<a onmouseover=\"Tip('$TIP_rename')\" href='channel_rename.php?channel=$file'>&nbsp;<img src=/images/icon/rename.png>&nbsp;</a>";
        echo "<a onmouseover=\"Tip('$TIP_del')\" href='".$_SERVER['PHP_SELF']."?channel=$file&quelle=$quelle&aktion=del' onclick=\"wait('WaitSave')\">&nbsp;<img src=/images/icon/del.png>&nbsp;</a>";
        echo "</td>";
      } else {
        echo "&nbsp;</td>";
      } 
      echo "</tr>";
   }
  }
}

echo "</table>";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>