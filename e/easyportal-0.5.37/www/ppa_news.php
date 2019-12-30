<?php
include('includes/kopf.php'); 

?>
<style type="text/css">
  @media all and (max-width:700px) {
    .postTable, .postTable thead, .postTable tbody, .postTable tr, .postTable th, .postTable td {
      display: block;
    }

    .postTable {
      text-align: right;
      width: 100%;
    }

    .postTable thead {
      position: absolute;
      top: -1111px;
      left: -1111px;
    }

    .postTable td {
      text-align: left;
    }
    
    .postTable td:before {
      float: left;
      padding: 0 10px 0 0;
      content: attr(data-label) ":";
    }

  }
</style>
<?php


function last_ppa_updates($array_datei)
{
  $aus = 0;
  $rest_del=0;
  foreach($array_datei as $key=>$zeile)
  {
    $pos = strpos($zeile, "Latest updates");
    // echo $pos;
    if (($pos === false) and ($aus==0)){
      // Der String wurde nicht gefunden";
      unset($array_datei[$key]);
    } else {
      // Der String wurde gefunden";
      $aus=1;
    }    
    if ($aus==1) {
       $pos2 = strpos($zeile, "</div>");
       if ($pos2 !== false) {$rest_del=1;}
       if ($rest_del==1){ unset($array_datei[$key]); }       
    }
    $pos3 = strpos($zeile, "Successfully built");
    if ($pos3 !== false){ unset($array_datei[$key]); }       
    $pos4 = strpos($zeile, "Latest updates");
    if ($pos4 !== false){ unset($array_datei[$key]); }       
  }
  foreach($array_datei as $zeile)
  {
    echo($zeile);
    //echo(htmlentities($zeile));
  }
}

echo "<div id='kopf2'>Tools - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_tools.php'); 

echo "<h2>$txt_ueberschrift</h2>";

$a_base_stable      = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-base-stable");
$a_vdr_stable       = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-vdr-stable");
$a_others_stable    = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-others-stable");
$a_base_testing     = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-base-testing");
$a_vdr_testing      = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-vdr-testing");
$a_others_testing   = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-others-testing");
if ($_SESSION["r_expert"] == "y" ) {
  $a_base_unstable    = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-base-unstable");
  $a_vdr_unstable     = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-vdr-unstable");
  $a_others_unstable  = file("https://launchpad.net/~easyvdr-team/+archive/ubuntu/5-others-unstable");
 
}

// *************************************** Container Begin ****************************************************
container_begin(1, "new.png", "$txt_ue_last_updates: stable");

echo "<table class='postTable' width=100%>";
echo "<thead>";
echo "<tr align=center><th width=33%>base</th><th width=33%>vdr</th><th width=33%>others</th></tr>";
echo "</thead>";
echo "<tbody>";
echo "<tr><td data-label='base'><br/>"; 
last_ppa_updates($a_base_stable);
echo "<br/></td><td data-label='vdr'><br/>";
last_ppa_updates($a_vdr_stable);
echo "<br/></td><td data-label='others'><br/>";
last_ppa_updates($a_others_stable);
echo "<br/></td></tr>";
echo "<tbody>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "new.png", "$txt_ue_last_updates: testing");

echo "<table class='postTable' width=100%>";
echo "<thead>";
echo "<tr align=center><th width=33%>base</th><th width=33%>vdr</th><th width=33%>others</th></tr>";
echo "</thead>";
echo "<tbody>";
echo "<tr><td data-label='base'><br/>";
last_ppa_updates($a_base_testing);
echo "<br/></td><td data-label='vdr'><br/>";
last_ppa_updates($a_vdr_testing);
echo "<br/></td><td data-label='others'><br/>";
last_ppa_updates($a_others_testing);
echo "<br/></td></tr>";
echo "<tbody>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

if ($_SESSION["r_expert"] == "y" ) {
// *************************************** Container Begin ****************************************************
container_begin(1, "new.png", "$txt_ue_last_updates: 5-unstable(focal)&nbsp;&nbsp;<img height='15px' src=/images/menuicons/e.png onmouseover=\"Tip('Nur für Experten !!!')\" >");

echo "<table class='postTable' width=100%>";
echo "<thead>";
echo "<tr align=center><th width=33%>base</th><th width=33%>vdr</th><th width=33%>others</th></tr>";
echo "</thead>";
echo "<tbody>";
echo "<tr><td data-label='base'><br/>";
last_ppa_updates($a_base_unstable);
echo "<br/></td><td data-label='vdr'><br/>";
last_ppa_updates($a_vdr_unstable);
echo "<br/></td><td data-label='others'><br/>";
last_ppa_updates($a_others_unstable);
echo "<br/></td></tr>";
echo "<tbody>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

}

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
