<?php
include('includes/kopf.php'); 

$channels_dir = "/var/lib/vdr/channels/";

$aktion = safeget('aktion');
switch ($aktion) {
  case "del_dvb-s":
    exec("find $channels_dir -type f -iname '*dvb-s*' -delete");
    break;
  case "del_dvb-c":
    exec("find $channels_dir -type f -iname '*dvb-c*' -delete");
    break;
  case "del_dvb-t":
    exec("find $channels_dir -type f -iname '*dvb-t*' -delete");
    break;
  default:
}

$anz_dvb_s = exec("find $channels_dir -type f -iname '*dvb-s*' |wc -l");
$anz_dvb_c = exec("find $channels_dir -type f -iname '*dvb-c*' |wc -l");
$anz_dvb_t = exec("find $channels_dir -type f -iname '*dvb-t*' |wc -l");

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Channels.png", $txt_ue_channellist);

echo "$txt_channellist1<br/>";
echo "$txt_channellist2<br/><br/><br/>";

echo "<a href=?aktion=del_dvb-s><button>$txt_b_del_dvb_s</button></a>&nbsp;$anz_dvb_s $txt_delete<br/><br/>";
echo "<a href=?aktion=del_dvb-c><button>$txt_b_del_dvb_c</button></a>&nbsp;$anz_dvb_c $txt_delete<br/><br/>";
echo "<a href=?aktion=del_dvb-t><button>$txt_b_del_dvb_t</button></a>&nbsp;$anz_dvb_t $txt_delete<br/><br/>";

echo "<br/>$txt_del_einzeln<br/>";


container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
