<?php
include('includes/kopf.php'); 

$TIP_edit = "Eintrag editieren";

?>
<script type="text/javascript" src="/includes/js/jquery.tablesorter.js"></script> 
<script type="text/javascript">
  $(document).ready(function() { $("#sortTable_alle").tablesorter(
    {headers: { 9:{ sorter: false } }, sortList: [[4,0], [5,0]] }
  ); } );
  
  $(document).ready(function() { $("#sortTable_title").tablesorter(
    {headers: { 2:{ sorter: false } }, sortList: [[0,0]] }
  ); } );
  
  $(document).ready(function() { $("#sortTable_suchtimer").tablesorter(
    {headers: { 3:{ sorter: false } }, sortList: [[0,0]] }
  ); } );
  
</script>
<?php

$txt_suchtimer_group = "Suchtimer Gruppieren";

$done_pfad = "/var/www/databases/portal/epgsearchdone.txt";

if (file_exists($done_pfad)) {
  $t = exec("sudo $SH_BEFEHL cat $done_pfad | grep '#ep_serie#ep_title#ep_se#ep_ep#ep_no' ");
  if ($t == "") { header("Location: epgsearchdone_admin.php"); }
} else { header("Location: epgsearchdone_admin.php"); }


$aktion   = safeget('aktion');
$group_by = safeget('group_by');
$title    = safeget('title');
$subtitle = safeget('subtitle');
$timer_id = safeget('timer_id');
$view     = safeget('view');

if (($group_by == "") AND ($view == "") AND ($title == "") AND ($timer_id == "")) { $group_by = "ep_serie";}

if ($title == "") { 
  $WHERE = ""; 
  $where_str = "";
}
else { 
  if ($subtitle == "") {
    $WHERE = "WHERE ep_serie='$title'"; 
    $where_str = " mit Titel '$title'";
  } else {
    $WHERE = "WHERE ep_serie='$title' AND ep_title='$subtitle'"; 
    $where_str = " mit Titel/Episode: '$title: $subtitle'";
  }
}
if ($timer_id != "") {
    $WHERE = "WHERE Timer_id='$timer_id'"; 
    $where_str = " mit Timer-ID: '$timer_id'";
}


if ($group_by == "ep_serie")  { $group_str = "Gruppiert nach Title"; }
if ($group_by == "suchtimer") { $group_str = "Gruppiert nach Suchtimer"; }
if ($view == "all")           { $group_str = "Alle Datensätze"; }
//if ($timer_id != "")          { $group_str = "xx"; }

echo "<div id='kopf2'>Timer - epgsearch - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "activetimer.png", "$txt_ue_done_data: $where_str $group_str");

if (file_exists($done_pfad)) {
  echo "<a href='?view=all' onclick=\"wait('WaitSeite')\"><button>$txt_b_alle</button></a>";
  echo "<a href='?group_by=ep_serie'><button>$txt_title_group</button></a>";
  echo "<a href='?group_by=suchtimer'><button>$txt_suchtimer_group</button></a>";
  echo "<br/>";
 
  $episodes = trim(shell_exec("ls $eplists_dir/*.episodes |cut -d'/' -f5-"));
  if (!(strpos($episodes, $title)) === false) {
     echo "<br/><a href='serie.php?serie=$title'><button>zur Serie '$title'</button></a>";
     echo "<br/>";
  } else {
     echo "";
  }

  if ($group_by == "ep_serie") {
    // **************************  Tabelle Group by Title ********************************************
    $Anz="";
    $resultSet = $db->executeQuery("SELECT ep_serie, COUNT(*) AS Anz FROM epgsearchdone GROUP BY ep_serie ORDER BY ep_serie");
    $Anz = $resultSet->getRowCount();
    echo "<br/>$Anz Datensätze gefunden<br/><br/>";

    if ($Anz>0) {
      echo "<table id='sortTable_title' class='tab1 tablesorter'>";
      echo "<thead>";
      echo "<tr><th>Title</th>";
      echo "<th align=right>Anzahl</th>";
      echo "<th align=right>gefundene Serie</th></tr>";
      echo "</thead>";
      echo "<tbody>";
      
      while($resultSet->next()) {
        $ep_serie = $resultSet->getCurrentValueByName("ep_serie");
        if (!(strpos($episodes, $ep_serie)) === false) {
           $gefundene_serie = "<a href='serie.php?serie=$ep_serie'>zur Serie</a>";
        } else {
           $gefundene_serie = "";
        }
        echo "<tr>";
        echo "<td align='left'><a href='?title=$ep_serie'>$ep_serie</td>" ;
        echo "<td align='right'>". $resultSet->getCurrentValueByName("Anz") ."</td>" ;
        echo "<td align='right'>$gefundene_serie</td>" ;
        echo "</tr>";
      }
      echo "</tbody>";
      echo "</table>";
    }
  }
 else if ($group_by == "suchtimer") { 
    // **************************  Tabelle Group by Suchtimer ********************************************
    $Anz="";
    $resultSet = $db->executeQuery("SELECT Timer_id, COUNT(*) AS Anz FROM epgsearchdone GROUP BY Timer_id ORDER BY Timer_id");
    $Anz = $resultSet->getRowCount();
    echo "<br/>$Anz Datensätze gefunden<br/><br/>";
    
    if ($Anz>0) {
      $epgsearch_conf = "/var/lib/vdr/plugins/epgsearch/epgsearch.conf";

      echo "<table id='sortTable_suchtimer' class='tab1 tablesorter'>";
      echo "<thead>";
      echo "<tr height=30><th>ID</th>";
      echo "<th>Suchtimer</th>";
      echo "<th>Anzahl</th>";
      echo "<th>Aktionen</th></tr>";
      echo "</thead>";
      echo "<tbody>";
      
      while($resultSet->next()) {
        $timer_id = $resultSet->getCurrentValueByName("Timer_id");
        $suchtimer = exec("sudo $SH_BEFEHL cat $epgsearch_conf |grep '^$timer_id:' |cut -d: -f2");
        echo "<tr>";
        echo "<td align='right'>$timer_id</td>" ;
        echo "<td>$suchtimer</td>" ;
        echo "<td align='right'>". $resultSet->getCurrentValueByName("Anz") ."</td>";
        echo "<td><a href='view_suchtimer.php?id=$timer_id'>'Suchtimer'</a> &nbsp;&nbsp;&nbsp;<a href='epgsearchdone.php?timer_id=$timer_id'>'epgsearchdone-Einträge dieses Suchtimers'</a></td>" ;
        echo "</tr>";
      }
      echo "</tbody>";
      echo "</table>";
    }
 }
 else
  {
    
    // **************************  Tabelle alle Datensätze ********************************************
    $Anz="";
    $ORDER = "ORDER BY Title, SubTitel";
    if ($title!="") {$ORDER = "ORDER BY ep_no, SubTitel";}
    $resultSet = $db->executeQuery("SELECT * FROM epgsearchdone $WHERE $ORDER");
    $Anz = $resultSet->getRowCount();
    echo "<br/>$Anz Datensätze gefunden<br/><br/>";
    if ($Anz>0) {
      echo "<table id='sortTable_alle' class='tab1 tablesorter'>";
      echo "<thead>";
      echo "<tr>";
      //echo "<th align='right'>ID</th>";
      echo "<th align='center'>Datum</th><th>Länge<br/>/min.</th><th>Such-<br/>Timer</th><th>Channel</th>";
      echo "<th>Title</th><th>Subtitle</th><th onmouseover=\"Tip('$TIP_SE')\">SE</th><th onmouseover=\"Tip('$TIP_EP')\">EP</th>";
      echo "<th onmouseover=\"Tip('$TIP_No')\">No</th><th align=center>Aktionen</th></tr>";
      echo "</thead>";
      echo "<tbody>";
      while($resultSet->next()) {
        echo "<tr>";
        //echo "<td nowrap align='right'>". $resultSet->getCurrentValueByName("id") ."</td>" ;
        $record_str = $resultSet->getCurrentValueByName("Record");
        $record_str_array = explode(' ', $record_str);
        //$datum = date("d.m.Y H:i",$record_str_array[0]);
        $datum = date("Y-m-d H:i",$record_str_array[0]);
        echo "<td nowrap align='right'>". $datum ."</td>" ;  // Datum
        $laenge = round($record_str_array[1]/60) ;
        echo "<td nowrap align='right'>". $laenge ."</td>" ;  // Länge
        $timer_id = $record_str_array[2];
        echo "<td nowrap align='right'><a href='view_suchtimer.php?id=$timer_id' class='b'>$timer_id</a></td>" ; // Suchtimer
        
        $channel_str = $resultSet->getCurrentValueByName("Channel");
        $channel_str_array = explode('-', $channel_str);
       
        $befehl = "cat /var/lib/vdr/channels.conf | grep $channel_str_array[3]";
        //$txt_debug_meldungen[] = "Befehl: $befehl"; 
        $channel_str = exec($befehl);
        $stringposition = strpos($channel_str, ";");
        $channel_str = substr($channel_str, 0, $stringposition);
        $stringposition = strpos($channel_str, ",");
        if ($stringposition) {$channel_str = substr($channel_str, 0, $stringposition);}
        
        echo "<td nowrap align='left' >". $channel_str ."</td>" ;
        //echo "<td        align='left' >". $resultSet->getCurrentValueByName("Title") ."</td>" ;
        //echo "<td        align='left' >". $resultSet->getCurrentValueByName("SubTitel") ."</td>" ;
        
        echo "<td align='left'  >". $resultSet->getCurrentValueByName("ep_serie") ."</td>" ;
        echo "<td align='left'  >". $resultSet->getCurrentValueByName("ep_title") ."</td>" ;
        echo "<td align='right' >". $resultSet->getCurrentValueByName("ep_se ") ."</td>" ;
        echo "<td align='right' >". $resultSet->getCurrentValueByName("ep_ep") ."</td>" ;
        echo "<td align='right' >". $resultSet->getCurrentValueByName("ep_no") ."</td>" ;
        
        $Description = $resultSet->getCurrentValueByName("Description");
        $Description = trim(str_replace("|", "<br/>", $Description));
        $Description = str_replace("'", " ", $Description);
        $Description = str_replace("\"", " ", $Description);
        
        
        echo "<td align='left'><a onmouseover=\"Tip('$TIP_view')\" href='epgsearchdone_view.php?id=".$resultSet->getCurrentValueByName("id")."'>&nbsp;<img src=/images/icon/view.png>&nbsp;</a>" ;
        echo                  "<a onmouseover=\"Tip('$TIP_edit')\" href='epgsearchdone_edit.php?id=".$resultSet->getCurrentValueByName("id")."'>&nbsp;<img src=/images/icon/edit.png>&nbsp;</a>" ;
        echo                  "<a onmouseover=\"Tip('$TIP_del')\" href='epgsearchdone_del.php?id=".$resultSet->getCurrentValueByName("id")."&aktion=del'>&nbsp;<img src=/images/icon/del.png>&nbsp;</a></td>" ;
        
        //echo "<td align='left' >$Description</td>" ;
      // echo "<td align='left' onmouseover=\"Tip('".$Description."', WIDTH, 450)\"><div style='color:#0000ff;'>Bescheibung</div></td>" ;
        //echo "<td align='left'><a href='#' class='tooltip' data-tooltip='".$Description."'>Bescheibung</a></td>" ;
        echo "</tr>";
      }
      echo "</tbody>";
      echo "</table>";
    }
  }
}
else {
  echo "$txt_error1<br/><br/>";
  echo "<a href='?aktion=scan'><button>$txt_b_scan</button></a><br/><br/>";
}

  
container_end();
// *************************************** Container Ende ****************************************************


// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Hilfe");

echo $help1;
echo $help2;
echo $help3;
echo $help4;
echo $help5;
echo "<a href='epgsearchdone_admin.php'><button>$txt_b_admin</button></a><br/><br/>";

container_end();
// *************************************** Container Ende ****************************************************


echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
