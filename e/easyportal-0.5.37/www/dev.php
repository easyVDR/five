<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php'); 

$php_error_report     = safepost('php_error_report');
$debug_meldungen      = safepost('debug_meldungen');

if (safepost('php_error_report')!="") {
  $_SESSION['php_error_report'] = $php_error_report;
  header('Location: dev.php');
  //redirect('/dev.php');
}

if (safepost('debug_meldungen')!="") {
  $_SESSION['debug_meldungen'] = $debug_meldungen;
  //header('Location: dev.php');
  redirect('/dev.php');
}


echo "<div id='kopf2'>".$txt_kopf2."</div>";

echo "<div id='inhalt'>";
echo "<center>";
//include('includes/subnavi_einstellungen.php'); 
echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin 'Developer' ****************************************************
container_begin(1, "Info.png", $txt_ue_dev);

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='php_error_report' value='0'>";
echo "<input type='hidden' name='debug_meldungen' value='0'>";

if ($_SESSION['php_error_report'] == 1) { $checked_php_error_report = "checked";} else { $checked_php_error_report = ""; }
if ($_SESSION['debug_meldungen']  == 1) { $checked_debug_meldungen  = "checked";} else { $checked_debug_meldungen  = ""; }

echo "<table border=0>";
echo "<tr>";
echo "<td>$txt_php_error_report: </td><td><input type='checkbox' name='php_error_report' value='1' $checked_php_error_report onChange='this.form.submit()' /></td>";
echo "</tr><tr>";
echo "<td>$txt_debug_meldungen2: </td><td><input type='checkbox' name='debug_meldungen' value='1' $checked_debug_meldungen onChange='this.form.submit()' /></td>";
echo "</tr>";
echo "</table>";
echo "</form>";
container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Developer' ****************************************************
container_begin(1, "Info.png", $txt_ue_links);
echo "&bull; <a class='b' href='/txt-db-api/manual/de/html/index.html'>txt-db-api Manual</a><br/>";
echo "&bull; <a class='b' href='/epgd/index.php'>epgd</a><br/>";
echo "&bull; <a class='b' href='/datenbank.php'>Datenbank</a><br/>";
container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Developer' ****************************************************
container_begin(1, "Info.png", $txt_ue_info);
echo "$txt_info<br/>";
container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>