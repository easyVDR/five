<?php
include('includes/kopf.php'); 
include('includes/functions_datum.php'); 

// programmierte Timer
$aktion   = safeget('aktion');
$timer_nr = safeget('timer_nr');
if (($timer_nr=='neu') and ($aktion!='edit')) { $timer_nr ='';}

switch ($aktion) {
  case "aktiv":
    exec("svdrpsend MODT $timer_nr on");
    break;
  case "deaktiv":
    exec("svdrpsend MODT $timer_nr off");
    break;
  case "del":
    exec("svdrpsend DELT $timer_nr");
    $timer_nr = "";
    break;
  default:
}

if ((safepost('save')) or (safepost('neu'))){
  $tag    = datum_de(safepost('tag'));
  $anfang = str_replace(":", "", safepost('anfang'));
  $ende   = str_replace(":", "", safepost('ende'));
  if (safepost('verzeichnis') == "") { $titel = safepost('datei'); } else { $titel = safepost('verzeichnis')."~".safepost('datei'); }
  $timer_string = safepost('aktiv').":".safepost('channel').":".$tag.":".$anfang.":".$ende.":".safepost('prio').":".safepost('lebensdauer').":".$titel.":".safepost('suchtimer');
  // TimerNr aktiv:Kanalnummer:Tag_des_Monats:Startzeit:Endzeit:Priorität:Dauerhaftigkeit:Titel:
}
  
if (safepost('save')) {
  $timer_nr = safepost('timer_nr');
  exec("svdrpsend MODT $timer_nr '$timer_string'");
}

if (safepost('neu')) { 
  exec("svdrpsend NEWT '$timer_string'");
}


function timer2array($timerzeile) {
  global $timer_nr, $Timer_Array_show;

//  $timerzeile  = str_replace("|", "*****", $timerzeile);
  $Daten       = explode(":",$timerzeile);
    $timer_nr_t  = substr($Daten[0], 4, strlen($Daten[0])-4);
    $timer_nr_a  = explode(" ",$timer_nr_t);
  $timer_nr2   = $timer_nr_a[0];
  if ($timer_nr == "") { $timer_nr = $timer_nr2;}
  $aktiv       = substr($Daten[0], strrpos($Daten[0]," ") + 1, strlen($Daten[0]) ); 
  $channel     = $Daten[1];
  $tag         = datum_ed($Daten[2]);
  $anfang      = substr($Daten[3],0,2).":".substr($Daten[3],2,4);
  $ende        = substr($Daten[4],0,2).":".substr($Daten[4],2,4);
  $prio        = $Daten[5];
  $lebensdauer = $Daten[6];
  
  $pos = strpos($Daten[7], "~");
  if ($pos === false) {
    $verzeichnis = "";
    $datei = $Daten[7];
  } else {
    $verzeichnis = substr($Daten[7],0,strrpos($Daten[7],"~") ); 
    $datei       = substr($Daten[7], strrpos($Daten[7],"~") + 1, strlen($Daten[7]) ); 
  }
 // $suchtimer   = htmlentities($Daten[8]);
  $suchtimer = $Daten[8];
//  $suchtimer   = str_replace("|", ":", $Daten[8]);
//  $suchtimer   = htmlentities($suchtimer);
  $Timer_Array = array('timer_nr' => $timer_nr2, 'aktiv' => $aktiv, 'channel' => $channel, 'tag' => $tag, 'anfang' => $anfang, 'ende' => $ende, 
                 'prio' => $prio, 'lebensdauer' => $lebensdauer, 'verzeichnis' => $verzeichnis, 'datei' => $datei, 'suchtimer' => $suchtimer);
  if ($timer_nr == $timer_nr2) {
    $Timer_Array_show = $Timer_Array;
  }
  return $Timer_Array;
}


/************************ Alle Timer in Array schreiben **********************************/
exec("svdrpsend LSTT |grep 250 |sort -t: +2 -4 ", $Result);

$Anz_Timer = count($Result);
$txt_debug_meldungen[] = "Anzahl Timer: $Anz_Timer";

$i = 0;
if ($Anz_Timer > 0) {
  foreach ($Result as $timerzeile) {
    $Timer_Array[] = timer2array($timerzeile);
  }
}

/***********************************************************************************/

echo "<div id='kopf2'>$txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<h2>$txt_ueberschrift</h2>";
echo "<div class='links'>";

// *************************************** Container Begin '' ****************************************************
container_begin(2, "activetimer.png", $txt_ue_timer);

//echo "Timer_str: <pre>'$timer_string'</pre>";
echo "<a href='?aktion=edit&timer_nr=neu'><button>$txt_b_neuer_timer</button></a><br/><br/>";

if ($Anz_Timer == 0) { 
  echo "<b>$txt_NoTimer</b><br/>";
}
else {
  echo "<table class='tab1'>";
  $DATUM = "";

  foreach ($Timer_Array as $ds) 
  {
    if ($DATUM <> datum_ed($ds['tag'])) {   // Neuer Tag
      echo "<tr height='25px'><th align='left' colspan='3'>".$ds['tag']."</th></tr>";  
    }
    echo "<tr>";

    echo "<td align='right'>";
    if ($ds['verzeichnis']=="") { $titel = $ds['datei']; } else { $titel = $ds['verzeichnis']."~".$ds['datei']; }
    if ($timer_nr == $ds['timer_nr']) {
      echo "<a href='?timer_nr=".$ds['timer_nr']."' style='color:red;'>".$ds['anfang']."-".$ds['ende']."</a>";
      echo "</td>";
      echo "<td align='left'><a class='b' href='?timer_nr=$ds[timer_nr]'><font color='red'>$titel</font></a></td>";
    }
    else {
      echo "<a class='b' href='?timer_nr=".$ds['timer_nr']."'>".$ds['anfang']."-".$ds['ende']."</a>";
      echo "</td>";
      echo "<td align='left'><a class='b' href='?timer_nr=$ds[timer_nr]'>".$titel."</a></td>";
    }
    echo "</a><td><nobr>";
    if ($ds['aktiv']==1) { echo "<a href='?aktion=deaktiv&timer_nr=".$ds['timer_nr']."' onmouseover=\"Tip('$TIP_deaktivieren')\"><img src='/images/icon/ok.png'>";} else
      { echo "<a href='?aktion=aktiv&timer_nr=".$ds['timer_nr']."' onmouseover=\"Tip('$TIP_aktivieren')\"><img src='/images/icon/ok_no.png'>"; }
    echo "<a onmouseover=\"Tip('$TIP_editieren')\" class='b' href='?aktion=edit&timer_nr=".$ds['timer_nr']."'>&nbsp;<img src=/images/icon/edit.png></a>";
    echo "<a onmouseover=\"Tip('$TIP_loeschen')\"   class='b' href='?aktion=del&timer_nr=".$ds['timer_nr']."'>&nbsp;<img src=/images/icon/del.png></a>"; 
    echo "&nbsp;</a></nobr></td></tr>";
    
    $DATUM=datum_ed($ds['tag']);
    $i++;
  }
  echo "</table>";
  echo "<br/>";
  echo "$txt_anz_timer: $Anz_Timer<br/>";
}
  
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Auswahl ****************************************************

if ($aktion=="edit") {
  include('container/c_timer_edit.php'); }
else {
  include('container/c_timer_show.php');  }

// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>