<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_einstellungen.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if (($_SERVER['PHP_SELF']=="/einstellungen.php"))
  {echo "<li><a href='einstellungen.php' class='pressed'>Einstellungen</a></li>"; }
else 
  {echo "<li><a href='einstellungen.php'>Einstellungen</a></li>"; }

if (($_SERVER['PHP_SELF']=="/plugins.php") or ($_SERVER['PHP_SELF']=="/view_plugin.php") or ($_SERVER['PHP_SELF']=="/addons.php"))
  {echo "<li><a href='plugins.php' class='pressed'>$txt_addons</a></li>"; }
else 
  {echo "<li><a href='plugins.php'>$txt_addons</a></li>"; }

if (($_SERVER['PHP_SELF']=="/channel.php") or ($_SERVER['PHP_SELF']=="/channel_upload.php") or ($_SERVER['PHP_SELF']=="/channel_edit.php") 
  or ($_SERVER['PHP_SELF']=="/channelpedia.php") or ($_SERVER['PHP_SELF']=="/channel_delete.php"))
  {echo "<li><a href='channel.php' class='pressed'>$txt_kanalliste</a></li>"; }
else 
  {echo "<li><a href='channel.php'>$txt_kanalliste</a></li>"; }

//if ($_SERVER['PHP_SELF']=="/netzwerk.php")
//  {echo "<li><a href='netzwerk.php' class='pressed'>$txt_netzwerk</a></li>"; }
//else 
//  {echo "<li><a href='netzwerk.php'>$txt_netzwerk</a></li>"; }

if ($_SERVER['PHP_SELF']=="/fernzugriff.php")
  {echo "<li><a href='fernzugriff.php' class='pressed'>$txt_fernzugriff</a></li>"; }
else 
  {echo "<li><a href='fernzugriff.php'>$txt_fernzugriff</a></li>"; }
  
if ($_SERVER['PHP_SELF']=="/systemeinstellungen.php")
  {echo "<li><a href='systemeinstellungen.php' class='pressed'>$txt_system</a></li>"; }
else 
  {echo "<li><a href='systemeinstellungen.php'>$txt_system</a></li>"; }

if ($_SERVER['PHP_SELF']=="/sonstige_einstellungen.php")
  {echo "<li><a href='sonstige_einstellungen.php' class='pressed'>$txt_sonstiges</a></li>"; }
else 
  {echo "<li><a href='sonstige_einstellungen.php'>$txt_sonstiges</a></li>"; }

if ($_SERVER['PHP_SELF']=="/bootlogo.php")
  {echo "<li><a href='bootlogo.php' class='pressed'>$txt_bootlogo</a></li>"; }
else 
  {echo "<li><a href='bootlogo.php'>$txt_bootlogo</a></li>"; }

if ($_SERVER['PHP_SELF']=="/root_login.php")
  {echo "<li><a href='root_login.php' class='pressed'>$txt_rootlogin</a></li>"; }
else 
  {echo "<li><a href='root_login.php'>$txt_rootlogin</a></li>"; }

echo "</ul></nav>";
?>
