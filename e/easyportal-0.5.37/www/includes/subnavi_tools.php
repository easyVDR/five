<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_tools.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if ($_SERVER['PHP_SELF']=="/befehle.php")
  {echo "<li><a href='befehle.php' class='pressed'>$txt_befehle</a></li>"; }
else 
  {echo "<li><a href='befehle.php'>$txt_befehle</a></li>"; }

if ($_SERVER['PHP_SELF']=="/pchanger.php")
  {echo "<li><a href='pchanger.php' class='pressed'>$txt_pchanger</a></li>"; }
else 
  {echo "<li><a href='pchanger.php'>$txt_pchanger</a></li>"; }

if ($_SERVER['PHP_SELF']=="/netzwerk_geraete.php")
  {echo "<li><a href='netzwerk_geraete.php' class='pressed' onclick=\"wait('WaitNetzwerkgeraete')\">$txt_lan</a></li>"; }
else 
  {echo "<li><a href='netzwerk_geraete.php' onclick=\"wait('WaitNetzwerkgeraete')\">$txt_lan</a></li>"; }
  
if (($_SERVER['PHP_SELF']=="/update.php") or ($_SERVER['PHP_SELF']=="/view_update.php") or ($_SERVER['PHP_SELF']=="/paketquellen.php"))
  {echo "<li><a href='update.php' class='pressed'>$txt_update</a></li>"; }
else 
  {echo "<li><a href='update.php'>$txt_update</a></li>"; }
  
if ($_SERVER['PHP_SELF']=="/nachricht.php")
  {echo "<li><a href='nachricht.php' class='pressed'>$txt_nachricht</a></li>"; }
else 
  {echo "<li><a href='nachricht.php'>$txt_nachricht</a></li>";  }
  
if ($_SERVER['PHP_SELF']=="/ppa_news.php")
  {echo "<li><a href='ppa_news.php' class='pressed' onclick=\"wait('WaitPPANews')\">$txt_ppa_news</a></li>"; }
else 
  {echo "<li><a href='ppa_news.php' onclick=\"wait('WaitPPANews')\">$txt_ppa_news</a></li>";  }

if ($_SERVER['PHP_SELF']=="/backup.php")
  {echo "<li><a href='backup.php' class='pressed'>$txt_backup</a></li>"; }
else 
  {echo "<li><a href='backup.php'>$txt_backup</a></li>";  }

if ($_SERVER['PHP_SELF']=="/ssh_warnung.php")
  {echo "<li><a href='ssh_warnung.php' class='pressed'>$txt_ssh</a></li>"; }
else 
  {echo "<li><a href='ssh_warnung.php'>$txt_ssh</a></li>"; }

echo "</ul></nav>";
?>
