<?php
include('includes/kopf.php');
include('includes/function_redirect.php');

$EASYVDRHOST       = shell_exec('less /var/lib/vdr/sysconfig | grep HOSTNAME | awk -F\'HOSTNAME\' \'{ print substr($2,2)}\'');
$easyvdr_video_out = exec("cat /var/log/easyvdr/setup.state | grep easyvdr_video_out | cut -d'\"' -f2");

$txt_debug_meldungen[] = "Video-Out: $easyvdr_video_out";

echo "<div id='kopf2'>$txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";
echo "<h2>$txt_ueberschrift1 $EASYVDRHOST $txt_ueberschrift2: ". $_SERVER['SERVER_ADDR'] ."</h2>";
echo "<div class='links'>";
  include('container/c_hardware.php');
  if ($easyvdr_video_out == "nvidia") { include('container/c_grafik_nvidia.php'); }
  if ($easyvdr_video_out == "intel")  { include('container/c_grafik_intel.php');  }
echo "</div>";  // class='links'
echo "<div class='rechts'>";
  include('container/c_festplattenauslastung.php');
  include('container/c_sensors.php');
echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig
echo "</center>";
echo "</div>";    // Inhalt

include('includes/fuss.php');
?>