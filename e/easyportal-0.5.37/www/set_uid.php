<?php
// setzt ein Cookie mit der User-Id
//echo $_GET["id"];
setcookie("user_id", $_GET["id"], time()+(3600*24*100)); // 100 Tage
header("Location: login.php");
?>