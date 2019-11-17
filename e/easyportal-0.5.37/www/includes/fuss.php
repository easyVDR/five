<?php

echo "<div id='fuss'>";
echo "&copy; ".date("Y")." easyPortal $EASYPORTAL_VER for easyVDR by Sinai / Steevee ";
$sprachdatei = "txt_".substr($_SERVER['PHP_SELF'], 1, strlen($_SERVER['PHP_SELF'])-1);
//echo $sprachdatei;
if (($_SESSION["r_expert"] == "y" ) and (file_exists("/var/www/lang/".$_SESSION["sprache"]."/$sprachdatei"))) {
  echo "&nbsp;&nbsp;&nbsp;<a onmouseover=\"Tip('Sprachfile editieren')\"  href='lang_view.php?sprachdatei=$sprachdatei&edit_lang=".$_SESSION["sprache"]."'><img src=/images/icon/edit.png></a>";
}
echo "</div>";


if ($_SESSION['debug_meldungen']=="1") {
  ?>
  <script type="text/javascript">

  function getWidth() {
  var de = document.documentElement;
  var myWidth = window.innerWidth || self.innerWidth || (de&&de.clientWidth) || document.body.clientWidth;
  return myWidth;
  }

  function getHeight() {
  var de = document.documentElement;
  var myHeight = window.innerHeight || self.innerHeight || (de&&de.clientHeight) || document.body.clientHeight;
  return myHeight;
  }

  </script>
  <?php

 // echo "<script type='text/javascript'>alert('$txt_debug_meldungen');</script>";
  echo "<br/><font color='black'>";
  echo "<table border=1 style='background-color:#efefef;'>";

  echo "<th colspan=2><font color='red'>Debug-Medungen</font></th>";
  
  foreach ($txt_debug_meldungen as $datensatz) {
    echo "<tr>";
    $stringposition = strpos($datensatz, ":");
    $string_1       = trim(substr($datensatz, 0, $stringposition));
    $string_2       = trim(substr($datensatz, $stringposition+1));
    echo "<td>$string_1</td><td>'$string_2'";

    if (($string_1 == "Sprachdatei Seite") and ($string_2 != "--- existiert nicht ---")) {
      $sprachdatei = substr($string_2, strrpos($string_2,"/") + 1, strlen($string_2) ); 
      $stringposition = strpos($string_2, "/");
      $edit_lang = substr($string_2, $stringposition+1);
      $stringposition = strpos($edit_lang, "/");
      $edit_lang = substr($edit_lang, 0, $stringposition);
      echo "<a href='lang_view.php?sprachdatei=$sprachdatei&edit_lang=$edit_lang'>&nbsp;&nbsp;&nbsp;<img src=/images/icon/edit.png>&nbsp;</a>";
    }
    
    echo "</td>";
    echo "</tr>";
  }

  echo "<tr><td>POST-Variablen:</td><td>";
  Debug::dump($_POST, true, '', true);
  echo "</td></tr>";
  
  echo "<tr><td>GET-Variablen:</td><td>";
  Debug::dump($_GET, true, '', true);
  echo "</pre></td></tr>";

  echo "<tr><td>Session-Variablen:</td><td>";
  Debug::dump($_SESSION, true, '', true);
  echo "</td></tr>";
  
  echo "<tr><td>Cookies:</td><td>";
  Debug::dump($_COOKIE, true, '', true);
  echo "</td></tr>";
 
  $IncludeDateien = get_included_files();
  echo "<tr><td>IncludeDateien:</td><td>";
  foreach ($IncludeDateien as $datensatz) {  echo $datensatz."<br/>"; }
  echo "</tr>";
  
  echo "<tr><td>Fenstergröße (BxH):</td><td>";
  ?><script type="text/javascript">document.write(getWidth() + ' x ' + getHeight())</script><?php
  echo "</td></tr>";
  
/*
  $GeladeneErweiterungen = get_loaded_extensions();
  echo "<tr><td>Geladene Erweiterungen anzeigen:</td><td>";
  Debug::dump($GeladeneErweiterungen, true, '', true);
  echo "</td></tr>";
*/

  $endzeit=explode(" ", microtime());
  $endzeit=$endzeit[0]+$endzeit[1];
  echo "<tr><td>Ladezeit der Seite:</td><td>";
  echo round($endzeit - $startzeit,6)." Sekunden";
  echo "</td></tr>";
  
  echo "</table>";
  echo "<br/>&nbsp;";
}
/*
  setcookie ("sprache", "", time() - 3600);
  setcookie ("expertenmodus", "", time() - 3600);
  setcookie ("x_spaltig", "", time() - 3600);
  unset($_COOKIE["sprache"]);
  unset($_COOKIE["expertenmodus"]);
  unset($_COOKIE["x_spaltig"]);
*/

?>
<a href="#" class="scrollicon" title="zum Seitenanfang"><div>^</div></a>
<script type="text/javascript">
  $(window).scroll(function(){
    if ($(this).scrollTop() > 80) {
        $('.scrollicon').fadeIn();
    } else {
        $('.scrollicon').fadeOut();
    }
  });

  $('.scrollicon').click(function(){
    $('html, body').animate({ scrollTop: 0 }, 1200);
    return false;
  });
</script>

<?php
echo "</body>";
echo "</html>";
?>