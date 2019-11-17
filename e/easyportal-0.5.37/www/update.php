<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');
include('includes/function_rec_countdown.php');

$aktion   = safeget('aktion');


function check_make_dir($verzeichnis) {
  if (!file_exists($verzeichnis)) { exec("mkdir $verzeichnis"); }
}

check_make_dir("/var/www/log");
check_make_dir("/var/www/log/update");

if ($aktion == "update") {
  exec("sudo $SH_UPDATE update");
  redirect("/view_update.php?file=update.log");
  exit();  
}

if ($aktion == "upgrade_s") {
  exec("sudo $SH_UPDATE upgrade_s");
  redirect("/view_update.php?file=upgrade_s.log");
  exit();  
}

if ($aktion == "upgrade") {
  exec("sudo $SH_UPDATE upgrade");
  redirect("/view_update.php?file=upgrade.log");
  exit();  
}

if ($aktion == "dist_upgrade_s") {
  exec("sudo $SH_UPDATE dist_upgrade_s");
  redirect("/view_update.php?file=dist_upgrade_s.log");
  exit();  
}

if ($aktion == "dist_upgrade") {
  exec("sudo $SH_UPDATE dist_upgrade");
  redirect("/view_update.php?file=dist_upgrade.log");
  exit();  
}


echo "<div id='kopf2'><a href='update.php' class='w'>Tools - $txt_kopf2</a></div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_tools.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "software-update.png", $txt_ue_update);

echo "<table border=0>";
echo "<tr>";
echo "<td><a href='update.php?aktion=update' onclick=\"wait('WaitUpdate')\"><button>$txt_b_update</button></a></td>";
echo "<td><a href='view_update.php?file=update.log'><button>View Log</button></a></td>";
echo "<td>&nbsp;</td>";
echo "</tr><tr>";
echo "<td><a href='update.php?aktion=upgrade' onclick=\"wait('WaitUpgrade')\"><button>upgrade</button></a></td>";
echo "<td><a href='view_update.php?file=upgrade.log'><button>$txt_b_view_log</button></a></td>";
echo "<td>$txt_warning</td>";
echo "</tr><tr>";
echo "<td><a href='update.php?aktion=upgrade_s' onclick=\"wait('WaitUpgrade_s')\"><button>Simuliert &nbsp; upgrade</button></a></td>";
echo "<td><a href='view_update.php?file=upgrade_s.log'><button>$txt_b_view_log</button></a></td>";
echo "<td>&nbsp;</td>";
echo "</tr><tr>";
echo "<td><a href='update.php?aktion=dist_upgrade' onclick=\"wait('WaitDistUpgrade')\"><button>dist-upgrade</button></a></td>";
echo "<td><a href='view_update.php?file=dist_upgrade.log'><button>$txt_b_view_log</button></a></td>";
echo "<td>$txt_warning</td>";
echo "</tr><tr>";
echo "<td><a href='update.php?aktion=dist_upgrade_s' onclick=\"wait('WaitDistUpgrade_s')\"><button>Simuliert &nbsp; dist-upgrade</button></a></td>";
echo "<td><a href='view_update.php?file=dist_upgrade_s.log'><button>$txt_b_view_log</button></a></td>";
echo "<td>&nbsp;</td>";
echo "</tr><tr>";
echo "<td>&nbsp;</td>";
echo "<td>&nbsp;</td>";
echo "<td></td>";
echo "</tr><tr>";
echo "<td><a href='paketquellen.php'><button>$txt_b_repositories</button></a></td>";
echo "<td>&nbsp;</td>";
echo "<td></td>";
echo "</tr>";
echo "</table>";
echo "<br/><div id='rec_countdown'>Zeit zur nächsten Aufnahme wird berechnet!</div>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", "Hilfe");

echo "<h3>$txt_b_update         </h3> $txt_help_update       <br>                   $txt_help_view_log<br><br>";
echo "<h3>$txt_b_upgrade        </h3> $txt_help_upgrade      <br> $txt_warning <br> $txt_help_view_log<br><br>";
echo "<h3>$txt_b_upgrade_s      </h3> $txt_help_upgrade_s    <br>                   $txt_help_view_log<br><br>";
echo "<h3>$txt_b_distupgrade    </h3> $txt_help_distupgrade  <br> $txt_warning <br> $txt_help_view_log<br><br>";
echo "<h3>$txt_b_distupgrade_s  </h3> $txt_help_distupgrade_s<br>                   $txt_help_view_log<br><br>";
echo "<h3>$txt_b_repositories   </h3> $txt_help_repositories <br><br>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>