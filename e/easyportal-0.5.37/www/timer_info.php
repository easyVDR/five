<?php
include('includes/kopf.php'); 
include('includes/function_rec_countdown.php');

$txt_kopf2        = "Timer - TimerMenü Info";
$txt_ueberschrift = "TimerMenü Info";
$txt_ue_timer     = "TimerMenü Info";

$txt_timers                   = "Timer";
$txt_info_timers              = "Hier können Timer angeschaut und bearbeitet";
$txt_epgsearch                = "epgsearch";
$txt_info_epgsearch           = "Suchtimer usw. von epgsearch";

echo "<div id='kopf2'>$txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_timer.php'); 
echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_timer);

echo "$txt_info1<br/><br/>";

echo "<table class='tab2'>";
echo "<tr><th><a href='timers.php'                 class='b'>$txt_timers</a>:</th>     <td>$txt_info_timers</td>";
echo "<tr><th><a href='epgsearchdone_admin.php'    class='b'>$txt_epgsearch</a>:</th>  <td>Alles rund um das Plugin epgsearch</td>";
echo "<tr><th>&nbsp;&nbsp;&nbsp;<a href='suchtimer.php'              class='b'>Suchtimer</a>:</th>  <td>$txt_info_epgsearch</td>";
echo "<tr><th>&nbsp;&nbsp;&nbsp;<a href='epgsearch_timerdone.php'    class='b'>timersdone.conf</a>:</th>  <td>Erledigte Timer</td>";
echo "<tr><th>&nbsp;&nbsp;&nbsp;<a href='epgsearchdone.php'          class='b'>epgsearchdone.data</a>:</th>  <td>Hier kann die epgsearchdone.data angezeigt und bearbeitet werden.</td>";
echo "<tr><th>&nbsp;&nbsp;&nbsp;<a href='epgsearch_changrps.php'     class='b'>Kanalgruppen</a>:</th>  <td>Hier kann die epgsearchchangrps.conf angezeigt werden</td>";
echo "<tr><th>&nbsp;&nbsp;&nbsp;<a href='epgsearch_changrps.php'     class='b'>epgsearchcats.conf</a>:</th>  <td>Hier kann die epgsearchcats.conf angezeigt werden</td>";
echo "</table>";

echo "<br/><font color='red'>$txt_info_warn_rot</font>&nbsp;$txt_info_warn2<br/>";
echo "<div id='rec_countdown'>$txt_zeit_berechnung</div><br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>