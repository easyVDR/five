<?php
session_start();
$key = $_GET[key];
$pc  = $_GET[pc];

if ($key!="") {
  exec("svdrpsend HITK ".$key);
}
else {
  $SH_BEFEHL = "/var/www/scripts/befehl.sh";
  $KODI_pid = exec("sudo $SH_BEFEHL pidof kodi.bin");
  if (($KODI_pid != "") and ($pc!=0)) { header('Location: fb_kodi.php'); }
}

if ($pc!="") {
  $SH_BEFEHL = "/var/www/scripts/befehl.sh";
  $pcscript = "/usr/share/easyvdr/program-changer/program-changer-easy-portal.sh";
  exec("sudo $SH_BEFEHL $pcscript $pc");
}
?>

<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML Basic 1.1//EN"
    "http://www.w3.org/TR/xhtml-basic/xhtml-basic11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="de">
<head>
  <title>easyVDR-Fernbedienung</title>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
  <link rel="stylesheet" href="/css/fb.css"> 
  <!-- <meta name="viewport" content="width=device-width, initial-scale=1"> -->
  <!-- <meta name="viewport" content="width=320, initial-scale=1" /> -->
  <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
</head>
<body>

<?php

if ($_SESSION["sprache"] == "") { $sprache = "Deutsch"; } else { $sprache = $_SESSION["sprache"]; }
$sprachdatei = "lang/".$sprache."/txt_fb.php";
include($sprachdatei);

echo "<center>";
echo "<div class='zeile'>";
echo "<a href='?key=Mute'  class='button'>$txt_mute</a>";
echo "<a href='?key=User0' class='button'>$txt_user0</a>";
echo "<a href='?key=Power' class='button'>$txt_power</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=".urlencode("Volume+")."' class='button'>$txt_vol_up</a>";
echo "<a href='?key=Up' class='button'>$txt_up</a></td>";
echo "<a href='?key=".urlencode("Channel+")."' class='button'>$txt_chan_up</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=Left'  class='button'>$txt_left</a>";
echo "<a href='?key=Ok'    class='button'>$txt_ok</a>";
echo "<a href='?key=Right' class='button'>$txt_right</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=".urlencode("Volume-")."' class='button'>$txt_vol_down</a>";
echo "<a href='?key=Down' class='button'>$txt_down</a>";
echo "<a href='?key=".urlencode("Channel-")."' class='button'>$txt_chan_down</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=Menu' class='button'>$txt_menu</a>";
echo "<a href='?key=Info' class='button'>$txt_info</a>";
echo "<a href='?key=Back' class='button'>$txt_back</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=Red'    class='button4' style='background-color:red;'>$txt_red</a>";
echo "<a href='?key=Green'  class='button4' style='background-color:lightgreen;'>$txt_green</a>";
echo "<a href='?key=Yellow' class='button4' style='background-color:yellow;'>$txt_yellow</a>";
echo "<a href='?key=Blue'   class='button4' style='background-color:lightblue;'>$txt_blue</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?pc=0'  class='button b_vdr'>&nbsp;</a>";
echo "<a href='?key=0' class='button'>0</a>";
echo "<a href='fb_kodi.php?pc=1'  class='button'>$txt_kodi</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=1' class='button'>1</a>";
echo "<a href='?key=2' class='button'>2</a>";
echo "<a href='?key=3' class='button'>3</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=4' class='button'>4</a>";
echo "<a href='?key=5' class='button'>5</a>";
echo "<a href='?key=6' class='button'>6</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=7' class='button'>7</a>";
echo "<a href='?key=8' class='button'>8</a>";
echo "<a href='?key=9' class='button'>9</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=Stop'  class='button b_stop'>&nbsp</a>";
echo "<a href='?key=Play'  class='button b_play'>&nbsp</a>";
echo "<a href='?key=Pause' class='button b_pause'>&nbsp;</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=Record'  class='button b_rec'>&nbsp;</a>";
echo "<a href='?key=FastRew' class='button b_fast_rewind'>&nbsp;</a>";
echo "<a href='?key=FastFwd' class='button b_fast_forward'>&nbsp;</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=Schedule'   class='button4'>$txt_schedule</a>";
echo "<a href='?key=Channels'   class='button4'>$txt_channels</a>";
echo "<a href='?key=Timers'     class='button4'>$txt_timers</a>";
echo "<a href='?key=Recordings' class='button4'>$txt_recordings</a>";
echo "</div>";

echo "<div class='zeile'>";
echo "<a href='?key=User1' class='button4'>$txt_user1</a>";
echo "<a href='?key=User2' class='button4'>$txt_user2</a>";
echo "<a href='?key=User3' class='button4'>$txt_user3</a>";
echo "<a href='?key=User4' class='button4'>$txt_user4</a>";
echo "</div>";

echo "</center>";
echo "</body>";
echo "</html>";
?>
