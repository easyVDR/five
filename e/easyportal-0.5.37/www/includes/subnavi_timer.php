<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_timer.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if ($_SERVER['PHP_SELF']=="/timer_info.php") 
  {echo "<li><a href='timer_info.php' class='pressed'>$txt_timer_info</a></li>"; }
else 
  {echo "<li><a href='timer_info.php'>$txt_timer_info</a></li>"; }
  
if ($_SERVER['PHP_SELF']=="/timers.php") 
  {echo "<li><a href='timers.php' class='pressed'>$txt_timers</a></li>"; }
else 
  {echo "<li><a href='timers.php'>$txt_timers</a></li>"; }

if (($_SERVER['PHP_SELF']=="/suchtimer.php") or ($_SERVER['PHP_SELF']=="/suchtimer_view.php") or ($_SERVER['PHP_SELF']=="/suchtimer_edit.php") 
 or ($_SERVER['PHP_SELF']=="/epgsearchdone.php") or ($_SERVER['PHP_SELF']=="/epgsearchdone_admin.php") or ($_SERVER['PHP_SELF']=="/epgsearchdone_view.php")
 or ($_SERVER['PHP_SELF']=="/epgsearch_timersdone.php") or ($_SERVER['PHP_SELF']=="/epgsearch_changrps.php")
 or ($_SERVER['PHP_SELF']=="/epgsearch_cats.php"))
  {echo "<li><a href='epgsearchdone_admin.php' class='pressed'>$txt_epgsearch</a></li>"; }
else 
  {echo "<li><a href='epgsearchdone_admin.php'>$txt_epgsearch</a></li>"; }
  
echo "</ul></nav>";
?>
