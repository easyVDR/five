<?php
include('includes/head.php'); 
include('includes/kopf.php'); 
include('includes/functions.inc');

$channels_alt = safeget('channel');
 
if ($_POST["Umbenennen"]) { 
  $channels_alt   = safepost('channels_alt');
  $channels_neu   = safepost('channels_neu');
  shell_exec("sudo $SH_BEFEHL mv /var/lib/vdr/channels/".$channels_alt." /var/lib/vdr/channels/".$channels_neu);
  header('Location: channel.php');
//  echo "sudo $SH_EASYPORTAL ren /var/lib/vdr/channels/".$channels_alt." /var/lib/vdr/channels/".$channels_neu;
  exit();  
}

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Channels.png", $txt_ue_channellist);

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='channels_alt' size='60' value='$channels_alt'>";

echo "<p>$txt_text1";

echo "<table border='0' cellspacing='2'>";
echo "  <tr>";
echo "    <td>$txt_old_name: </td>";
echo "    <td>$channels_alt</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <td>$txt_new_name: </td>";
echo "    <td><input type='text' name='channels_neu' size='60' value='$channels_alt'></td>";
echo "  </tr>";
echo "</table>";

echo "<br/>";
echo "<input type='submit' value='$txt_b_rename' name='Umbenennen'>";
echo "</form>";
echo "<a href='channel.php'><button>$txt_b_abort</button></a><br/><br/>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", $txt_ue_info);
echo "$txt_info1<br/>$txt_info2<br/>";
container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";   // inhalt

include('includes/fuss.php'); 

?>