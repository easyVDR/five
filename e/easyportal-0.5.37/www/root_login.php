<?php
include('includes/kopf.php'); 
//include('includes/function_redirect.php');

$submit  = safepost('submit');
$root_pw = safepost('root_pw');

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Root-Login");

if (safepost('root_aktivieren')!="") {
  $wp = "";
  $wp = exec("cat /etc/ssh/sshd_config | grep 'PermitRootLogin without-password'");
  if ($wp != "") {
    echo "root-Zugang mit Passwort aktiviert<br/>";
    exec("sudo $SH_BEFEHL sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config");
    exec("sudo $SH_BEFEHL service ssh restart");
  }
  //sleep(5);
  if ($root_pw == "") {
    echo "root-Passwort ungültig<br/><br/>";
  }
  else {
    $befehl = "sudo $SH_BEFEHL echo 'root:".$root_pw."' | chpasswd";
    //echo "befehl: '$befehl'<br/>";
    exec("sudo $SH_EASYPORTAL rootpw $root_pw");
    echo "root-Passwort '".$root_pw."' wurde gesetzt<br/><br/>"; 
    //sleep(5);
  }
}

if (safepost('root_deaktivieren')!="") {
  exec("sudo $SH_BEFEHL passwd -d root");
  echo "root-Passwort wurde gelöscht<br/><br/>"; 
}

$root_pw_string = shell_exec("sudo $SH_BEFEHL passwd -S");
//echo "$root_pw_string<br/>";
$root_pw_array = explode(' ', $root_pw_string);

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

if (($root_pw_array[0] == "root") and ($root_pw_array[1] == "NP")) {
  echo "$txt_no_root<br/><br/>";
  echo "neues root Passwort: <input type='text' size='20' maxlength='20' name='root_pw' value='$root_pw'><br/><br/>"; 
  echo "<input type='submit' name='root_aktivieren' value='$txt_b_root_aktivieren'>";
}

if (($root_pw_array[0] == "root") and ($root_pw_array[1] == "P")) {
  echo "$txt_root<br/><br/>";
  echo "<input type='submit' name='root_deaktivieren' value='$txt_b_root_deaktivieren'>";
}

echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", $txt_ue_help);

echo "$txt_help1 <br/>";
echo "$txt_help2 <br/>";
echo "$txt_help3 <br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
