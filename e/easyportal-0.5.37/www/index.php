<?php
include('includes/kopf.php');

?>
<script type="text/javascript">

$( document ).ready(function() {
  $.get('requests/r_index.php?art=easyvdr_version', function(data) { $('#easyvdr_version').html(data); })
  $.get('requests/r_index.php?art=isover', function(data)          { $('#isover').html(data); })
  $.get('requests/r_index.php?art=dkms', function(data)            { $('#dkms').html(data); })
});


</script>

<?php

include('includes/function_rec_countdown.php');

$EASYVDRHOST = shell_exec('less /var/lib/vdr/sysconfig | grep HOSTNAME | awk -F\'HOSTNAME\' \'{ print substr($2,2)}\'');

echo "<div id='kopf2'>$txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";
echo "<h2>$txt_ueberschrift1 $EASYVDRHOST $txt_ueberschrift2: ". $_SERVER['SERVER_ADDR'] ."</h2>";
echo "<div class='links'>";

include('container/c_laufende_services.php');
include('container/c_zeit_uptime.php');
include('container/c_versionen.php');
include('container/c_passwoerter.php');

echo "</div>";  // class='links'
echo "<div class='rechts'>";

include('container/c_timers.php');

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig
echo "</center>";
echo "</div>";  // inhalt

include('includes/fuss.php');
?>
