<?php
include('includes/kopf.php'); 

$aktion = safeget('aktion');

if ($aktion == "vdrstoppen") {
  exec("sudo $SH_EASYPORTAL stopvdr");
}

if ($aktion == "vdrstarten") {
  exec("sudo $SH_EASYPORTAL startvdr");
}

if ($aktion == 'vdr') {
  exec("sudo $SH_BEFEHL sh /usr/share/easyvdr/program-changer/program-changer.sh 0");
}

if ($aktion == 'xbmc') {
  exec("sudo $SH_BEFEHL sh /usr/share/easyvdr/program-changer/program-changer.sh 1");
}

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_tools.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Befehle.png", $txt_ue_befehle);

//echo "<button>System runterfahren</button><br/>";
//echo "<button>System neustarten</button><br/>";
echo "<a href='befehle.php?aktion=vdrstoppen' onclick=\"wait('WaitStopvdr')\"><button>$txt_b_vdr_stop</button></a><br/><br/>";
echo "<a href='befehle.php?aktion=vdrstarten' onclick=\"wait('WaitStartvdr')\"><button>$txt_b_vdr_start</button></a><br/>";
//echo "<button>VDR neustarten</button><br/>";
//echo "<button>VDR und Desktop neustarten</button><br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
