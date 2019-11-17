<?php
include('includes/kopf.php'); 

$aktion = safeget('aktion');
$art    = safeget('art');

$backupdirlink = "/var/www/backup/";
$backupdir = "/media/easyvdr01/backup/";
$datetime = date ("Y-m-d_Hi");

if (!file_exists($backupdir))     { exec("sudo -u root ".$SH_BACKUP." mkbackupdir"); }
if (!file_exists($backupdirlink)) { exec("sudo -u root ".$SH_BACKUP." mklink"); }

if ($aktion == "backup" ){  
  exec("sudo -u root ".$SH_BACKUP." ".$art); 
  // echo "$SH_BACKUP = '$SH_BACKUP'<br/>";
  // echo "art = '$art'<br/>";
  // echo "Backup erstellt: $datetime";
  log_portal("Backup $art wurde erstellt");
}

if ($aktion == "restore" ){  
  exec("sudo -u root ".$SH_BACKUP." restore ".$file); 
  log_portal("backup $file wurde wiederhergestellt");
}

if ($aktion == "del") {
  // unlink ($backupdir.$_GET["file"]);
  exec("sudo $SH_BEFEHL rm ".$backupdirlink.$_GET['file']); 
  log_portal("backup $file wurde gelöscht");
}

echo "<div id='kopf2'><a href='#' class='w'>$txt_kopf2</a></div>";  // Kopf

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_tools.php'); 

echo "<h2>$txt_ueberschrift</h2>";

echo "<div class='links'>";

// *************************************** Container Begin ****************************************************
container_begin(2, "database_options.png", $txt_ue_backup_options);

echo "<table border='0' cellspacing='2'>";
echo "<tr>";
echo "  <td width='200px' valign=top>$txt_new_backup:</td>";
echo "  <td width='230px'><a href='backup.php?aktion=backup&art=back_all' onclick=\"wait('WaitBackup')\"><button>$txt_b_all</button></a>";
echo "                    <a href='backup.php?aktion=backup&art=back_cfg' onclick=\"wait('WaitBackup')\"><button>$txt_b_cfg</button></a></td>";
echo "</tr>";
echo "<tr>";
echo "  <td width='200px'>$txt_backup_dir:</td>";
echo "  <td width='230px'>$backupdir</td>";
echo "</tr>";
echo "<tr>";
echo "  <td width='200px'>$txt_date_time:</td>";
echo "  <td width='230px'>$datetime</td>";
echo "</tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(2, "Info.png", $txt_ue_hilfe);

echo "$txt_hilfe1<br/><br/>";
echo "'<b>all</b>' $txt_hilfe_all1<br/>";
echo $txt_hilfe_all2;
echo "<br/><br/>";
echo "'<b>cfg</b>' $txt_hilfe_cfg1<br/>";
echo $txt_hilfe_cfg2;
echo "<br/><br/>";
echo "$txt_hilfe2<br/>";
echo "<font color=red>$txt_hilfe_warnung</font><br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin ****************************************************
container_begin(2, "Regenschirm.png", $txt_ue_backups);

echo "<table class='tab1'>";
echo "<tr>";
echo "  <th height='25px'>$txt_tab_name</th>";
echo "  <th>$txt_tab_size</th>";
echo "  <th class='kl600aus'>$txt_tab_date</th>";
echo "  <th>$txt_tab_action</th>";
echo "</tr>";
$dir = opendir($backupdir);
$dir2 = array();
while($file = readdir($dir)){
  if($file != "." && $file != ".."){
    $dir2[] = $file;
  }
} 
rsort($dir2); 
foreach($dir2 as $file){ 
  if ($file != "." && $file != ".." ) {
    echo "<tr><td>$file&nbsp;</td>";
    echo "<td align='right'>" . bcdiv(filesize($backupdir.$file),1024,1) . " kB</td>";
    echo "<td class='kl600aus'>" . date("Y-m-d H:i",filemtime($backupdir.$file)) . "</td>";
    echo "<td align='center'><a onmouseover=\"Tip('Backup downloaden')\" href='backup/$file'><img src='/images/icon/down.png'></a>&nbsp;";
    echo "<a onmouseover=\"Tip('Backup wiederherstellen')\" href='backup.php?aktion=restore&file=$file'>&nbsp;<img src=/images/icon/wiederherstellen.png></a>&nbsp;";
    echo "<a onmouseover=\"Tip('Backup löschen')\" href='backup.php?aktion=del&file=$file'>&nbsp;<img src=/images/icon/del.png></a></td>";
    echo "</tr>";
  }
}
closedir($dir);
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig
echo "</center>";
echo "</div>";  // inhalt

include('includes/fuss.php'); 
?>
