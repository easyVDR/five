<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');

echo "<div id='kopf2'>".$txt_kopf2."</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_user.php'); 
echo "<h2>$txt_ueberschrift</h2>";

echo "<div class='links'>";

// *************************************** Container Begin 'Usereinstellungen' ****************************************************
container_begin(1, "Info.png", $txt_ue_einstellungen." '".$_SESSION["username"]."'");

if ($_SESSION['user_id']=="") {
  echo "$txt_kein_user <br/>";
}
else {
  if (safepost('submit')) {
    $expertenmodus = $_SESSION['r_expert']  = safepost('expertenmodus');
    $sprache       = $_SESSION['sprache']   = safepost('sprache');
    //$angemeldet_bleiben     = $_SESSION['angemeldet_bleiben'] = safepost('angemeldet_bleiben');
    $design         = $_SESSION['design']     = safepost('design');
    
    echo "$txt_save1 '$_SESSION[username]' (ID $_SESSION[user_id]) $txt_save2<br/><br/>";
    $db->executeQuery("UPDATE user SET Sprache='$sprache', design='$design', r_expert='$expertenmodus' WHERE ID='$_SESSION[user_id]'");
    redirect('/user_einstellungen.php'); 
  } else {
    $expertenmodus = $_SESSION['r_expert'];
    $sprache       = $_SESSION['sprache'];
    //$angemeldet_bleiben     = $_SESSION['angemeldet_bleiben'];
    $design         = $_SESSION['design'];
  }

  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
  echo "<input type='hidden' name='expertenmodus' value='n'>";

  echo "<table border='0'>";
  echo "<tr><td valign=top>$txt_spracheinstellungen:</td>";
  echo "<td align=left>";
  $language_dir = $EASYPORTAL_DOC_ROOT."lang/";
  $dir_array = scandir($language_dir);  // Alle Dateien und Ordner in ein Array packen 
  sort($dir_array);                     // Array sortieren
  foreach($dir_array as $einzel_datei){                   // jetzt alle Array Inhalte durchgehen
    if($einzel_datei != "." && $einzel_datei != "..") {   // schauen ob die Zeichen "." oder ".." nicht enthalten sind
      if(is_dir($language_dir.$einzel_datei)) {           // jetzt schauen ob es ein Verzeichniss ist
        if ($sprache==$einzel_datei) {$checked_s="checked";} else {$checked_s="";}
        echo "<input type='radio' name='sprache' value='$einzel_datei' $checked_s >$einzel_datei &nbsp;";
      }
    }
  }
  echo "</td></tr>";

  echo "<tr><td>$txt_expertenmodus:</td><td>";
  if ($expertenmodus == "y") { $checked_expert = "checked";} else { $checked_expert = ""; }
  echo "<input type='checkbox' name='expertenmodus' value='y' $checked_expert>";
  echo "</td></tr>";

  echo "<tr><td>$txt_designauswahl:</td><td>";
  echo "<select name='design'>";
  if ($_SESSION['design'] == "easyportal_20")                  { $sel1 = " selected";} else { $sel1 = "";}
  if ($_SESSION['design'] == "easyportal_forumcolors")         { $sel2 = " selected";} else { $sel2 = "";}
  if ($_SESSION['design'] == "easyportal_frogs")               { $sel3 = " selected";} else { $sel3 = "";}  
  if ($_SESSION['design'] == "easyportal_she_sells_seashells") { $sel4 = " selected";} else { $sel4 = "";}  
  if ($_SESSION['design'] == "easyportal_vintage_modern")      { $sel5 = " selected";} else { $sel5 = "";}  
  echo "<option value='easyportal_20'                  $sel1>Easyportal 2.0</option>";
  echo "<option value='easyportal_forumcolors'         $sel2>ForumColors</option>";
  echo "<option value='easyportal_frogs'               $sel3>Frogs</option>";
  echo "<option value='easyportal_she_sells_seashells' $sel4>She Sells Seashells</option>";
  echo "<option value='easyportal_vintage_modern'      $sel5>Vintage Modern</option>";
  echo "</select>";
  echo "</td></tr>";
  
  echo "</table>";
  echo "<br/>";
  echo "<input type='submit' name='submit' value='Speichern'>";
  echo "</form>";
}
  
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin 'Info' ****************************************************
container_begin(1, "Info.png", $txt_ue_info);
echo "$txt_info<br/>";
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>";

echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>