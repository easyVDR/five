<?php
include('includes/kopf.php'); 

$plugin = safeget('plugin');
$action = safeget('action');

if ($action=="aktivieren") {
  shell_exec("sudo $SH_EASYPORTAL on ".$plugin);
}
if ($action=="deaktivieren") {
  shell_exec("sudo $SH_EASYPORTAL off ".$plugin);
}

$datei = fopen("/var/lib/vdr/plugins/setup/vdr-setup.xml","r");
while (!feof($datei)) {
  $line = fgets($datei); 
  if (strpos($line, "<plugin name=\"$plugin\"") !== false) {
  $n = strpos($line, 'info=');
  $t = substr(substr($line, $n, strlen($line)-$n),6);  
  $info = split('\"',$t);
  $info = $info[0];

  $n = strpos($line, 'active=');
  $t = substr(substr($line, $n, strlen($line)-$n),8);
  $active = split('\"',$t);
  $active = $active[0];

  if ($active=="yes") { $action="deaktivieren"; } else { $action="aktivieren"; } 

  if (strpos($line, 'param=') > 0) {
    $n = strpos($line, 'param=');
    $t = substr(substr($line, $n, strlen($line)-$n),7);  
    $param = split('\"',$t);
    $param = $param[0];
  }
  else
    $param = "";
    // echo "'$info'<br>";
    // echo "'$active'<br>";
    // echo "'$param'<br><br>";
  }
}
fclose($datei);

// Installierte Version
$befehl = "vdr --version | grep '$plugin '";
exec("sudo $SH_BEFEHL $befehl", $ausgabe); 
if ($ausgabe[0]!="") {
  $install = "installiert";
  $install_version_t = explode(' - ', $ausgabe[0]);
  $install_version_t = explode('(', $install_version_t[0]);
  $install_version = substr($install_version_t[1],0,strlen($install_version_t[1])-1);
} else {
  $install = "nicht installiert";
  $install_version = "-";
}

// Aktuelle Version
$befehl = "apt-cache show vdr-plugin-$plugin | grep Version";
exec("sudo $SH_BEFEHL $befehl", $ausgabe2); 
$aktuelle_version = substr($ausgabe2[0],9);

echo "<div id='kopf2'>";
echo "<a href='einstellungen.php' class='w'>Einstellungen</a> - <a href='plugins.php' class='w'>Plugins</a> - Plugin: $plugin";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_addons.php'); 

echo "<p>";
echo "<h2>VDR-Plugin: '$plugin'</h2>";
echo "<br>";

echo "<table border=0 width=100%>";  // Inhalt Mehrspaltig
echo "<tr><td align=center valign=top width=50%>";

// *************************************** Container Begin 'Allgemeine Infos' ****************************************************
container_begin(2, "devstatus.png", "Allgemeine Infos");

echo "<table border='0' cellspacing='2' width=100%>";
echo "  <tr><td width='200px'>Pluginname:   </td><td>$plugin          </td></tr>";
echo "  <tr><td>Pluginbeschreibung:         </td><td>$info            </td></tr>";
echo "  <tr><td>Installiert:                </td><td>$install         </td></tr>";
echo "  <tr><td>Installierte Version:       </td><td>$install_version </td></tr>";
echo "  <tr><td>Verf&uuml;gbare Version:    </td><td>$aktuelle_version</td></tr>";
echo "  <tr><td>Aktiv:                      </td><td>$active          </td></tr>";
echo "  <tr><td>Aktivieren/Deaktivieren:    </td><td><a href='plugins.php?plugin=".$plugin."&action=".$action."'><button>$action</button></a></td></tr>";
echo "  <tr><td valign=top>Aufruf-Parameter:</td><td>'$param'         </td></tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Links' ****************************************************
container_begin(2, "devstatus.png", "Links");

echo "<table border='0' cellspacing='2' width=100%>";
echo "  <tr>";
echo "    <td valign=top width='200px'>EasyVDR-Wiki:</td>";
echo "    <td ><a class='b' href='http://wiki.easy-vdr.de/index.php?title=Plugin_$plugin' target='_blank'>http://wiki.easy-vdr.de/index.php?title=Plugin_$plugin</a></td>";
echo "  </tr>";
echo "  <tr>";
echo "    <td valign=top>VDR-Wiki:</td>";
echo "    <td><a class='b' href='http://www.vdr-wiki.de/wiki/index.php/$plugin-plugin' target='_blank'>http://www.vdr-wiki.de/wiki/index.php/$plugin-plugin</a></td>";
echo "  </tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</td><td align=center valign=top>";

// *************************************** Container Begin 'Plugin-Einstellungen' ****************************************************
container_begin(2, "devstatus.png", "Plugin-Einstellungen");

$datei2 = fopen("/var/lib/vdr/setup.conf","r");
$i=0;
while (!feof($datei2)) {
  $line = fgets($datei2); 
  if (strpos($line, $plugin.".") !== false) {
    echo $line."<br>";
    $i=1;
  }
}
if ($active=='no') {echo "<p>Keine Einstellungen, da Plugin nicht aktiviert ist.<br>";}
if (($active=='yes') and ($i==0)) {echo "<p>Das Plugin hat keine Einstellungen!<br>";}

container_end();
// *************************************** Container Ende ****************************************************

echo "</td></tr></table>"; // Inhalt 2 Spaltig

//echo "<br/>";
//echo "<center><a href='plugins.php'><button>Zurück zur Pluginübersicht</button></a></center>";

echo "</div>";  // inhalt

include('includes/fuss.php'); 
?>