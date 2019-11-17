<?php
      
include('includes/kopf.php'); 
include('includes/function_redirect.php');

if (safepost('login')) { 
  $username = safepost('username');
  // $passwort = md5($_POST["passwort"]);
  $passwort = safepost('passwort');
  $cookie   = safepost('cookie');
  log_portal("login: '$username', cookie: '$cookie'");
  // $txt_debug_meldungen[] = "cookie: $cookie";
  if (($username!="") and ($passwort!="")) {

    $resultSet = $db->executeQuery("SELECT * FROM user WHERE User='$username' AND Passwort='$passwort'");
    $resultSet->next();

    if ($resultSet) {
      $_SESSION["user_id"]   = $resultSet->getCurrentValueByName("ID");
      $_SESSION["username"]  = $resultSet->getCurrentValueByName("User");
      $_SESSION["sprache"]   = $resultSet->getCurrentValueByName("Sprache");
      $_SESSION["r_edit"]    = $resultSet->getCurrentValueByName("r_edit");
      $_SESSION["r_expert"]  = $resultSet->getCurrentValueByName("r_expert");
      $_SESSION["design"]    = $resultSet->getCurrentValueByName("design");
      if ($cookie == 1) { // setcookie("user_id", $resultSet->getCurrentValueByName("ID"), $cookie_dauer); // 100 Tage
        $id = $resultSet->getCurrentValueByName('ID');
        $cookie_dauer = time()+(3600*24*100);
        log_portal("setcookie: user_id, ".$id.", ".$cookie_dauer);
        // echo $id;
        redirect('/set_uid.php?id=2');  // !!! Muss noch geändert werden
      }  
      
 //     if ($cookie == 1) { setcookie("user_id", $_SESSION["user_id"], time()+(3600*24*100)); }  // 100 Tage
 //     if ($cookie == 1) { $_SESSION["set_cookie"] = 1;} 
    }
    if ($_SESSION["user_id"]) { redirect('/login.php'); /*header("Location: login.php");*/ }
    else { $login_fehler="<font color='red'><b>$txt_login_error</b></font><br/><br/>"; }
  }
  else { $login_fehler="<font color='red'><b>$txt_login_error</b></font><br/><br/>"; 
  }
}


if (safepost('logout')) { 
  log_portal("logout");
  unset($_SESSION["user_id"]);
  unset($_SESSION["username"]);
  $_SESSION["sprache"]  = "Deutsch";
  $_SESSION["r_edit"]   = "n";
  $_SESSION["r_expert"] = "n";
  $_SESSION["design"]   = "easyportal_20";
  $username = "";
  $passwort = "";
  setcookie("user_id","",time() - (3600*10));
  redirect('/login.php');
}



echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_user.php'); 

echo "<h2>$txt_ueberschrift</h2>";

$txt_debug_meldungen[] = "SESSION User_ID:".$_SESSION["user_id"];
if(!isset($_SESSION["user_id"])) 
{
/************************************* Login ******************************************/
  // Nicht eingeloggt
  // *************************************** Container Begin ****************************************************
  container_begin(1, "schloss.png", $txt_ue_logout);

  echo $login_fehler;
  
  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
 // echo "<input type='hidden' name='cookie' value='0'>";
  echo "<table>";
  echo "  <tr>";
  echo "    <td>$txt_username  :</td>";
  echo "    <td><input type='text' size='30' maxlength='30' name='username'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_password :</td>";
  echo "    <td><input type='password' size='30' maxlength='30' name='passwort'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_dauerhauft :</td>";
  echo "    <td><input type='Checkbox' name='cookie' value='1'></td>"; 
  echo "  </tr>";
  echo "</table>";
  echo "<br/>";
  echo "<input type='submit' name='login' value='$txt_b_login'>";
  echo "</form>";

  container_end();
  // *************************************** Container Ende ****************************************************
}
else
{
/************************************* Logout ******************************************/
  // Eingeloggt
  // *************************************** Container Begin ****************************************************
  container_begin(1, "schloss.png", $txt_ue_logout);

  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
  echo "$txt_angemeldet_als: <b>".$_SESSION["username"]."</b><br/><br/>";  
  echo "$txt_abmelden<br/><br/>";
  echo "<input type='submit' name='logout' value='$txt_b_logout'>";
  echo "</form>";

  container_end();
  // *************************************** Container Ende ****************************************************
}


// *************************************** Container Begin 'Info' ****************************************************
container_begin(1, "Info.png", $txt_ue_info);

echo "$txt_info1<br/>";
echo "$txt_info2<br/><br/>";
echo "$txt_info3<br/><br/>";
echo "$txt_info4<br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
