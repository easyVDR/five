<?php
include('includes/kopf.php'); 

$aktion = safeget('aktion');
switch ($aktion) {
 case "touch_video":
   exec("sudo $SH_BEFEHL touch /video0");
   break;
 case "video_rechte":
   exec("sudo $SH_BEFEHL chown -vR vdr:vdr /media/easyvdr01/video0/");
   break;
 case "video_clean":
   // Verzeichnisse mit *.del löschen
   exec("sudo $SH_BEFEHL find /video0/ -type d -name *.del -exec rm -rf {} \;");
   // leere Veerzeichnisse löschen 
   exec("sudo $SH_BEFEHL find /video0/ -type d -empty | xargs rmdir");
   break;
 default:
}

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_tools);

echo "<a href='?aktion=touch_video'><button>$txt_b_touch_video</button></a><br/>";
echo "<a href='?aktion=video_rechte'><button>$txt_b_video_rechte</button></a><br/>";
echo "<a href='?aktion=video_clean'><button>$txt_b_video_clean</button></a><br/>";
echo "<a href='eplist.php?action=dl'><button>$txt_b_eplist_dl</button></a><br/>";
echo "<a href='tatort.php?action=einlesen'><button>$txt_b_tatort_einlesen</button></a><br/>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_info);

echo "<h3>$txt_touch_video_ue</h3>";
echo $txt_touch_video_1;
echo $txt_touch_video_2;

echo "<h3>$txt_video_rechte_ue</h3>";
echo $txt_video_rechte_1;
echo $txt_video_rechte_2;

echo "<h3>$txt_video_clean_ue</h3>";
echo $txt_video_clean_1;
echo $txt_video_clean_2;

echo "<h3>$txt_eplist_dl_ue</h3>";
echo $txt_eplist_dl_1;
echo $txt_eplist_dl_2;

echo "<h3>$txt_tatort_einlesen_ue</h3>";
echo $txt_tatort_einlesen_1;
echo $txt_tatort_einlesen_2;

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
