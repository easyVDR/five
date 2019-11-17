<?php

if ($timer_nr == "neu") { $txt_ue_container = "Timer hinzufügen"; } else { $txt_ue_container = "Timer editieren"; }

// *************************************** Container Begin '' ****************************************************
container_begin(2, "activetimer.png", $txt_ue_container);

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='timer_nr' value='".$Timer_Array_show['timer_nr']."'>";
echo "<input type='hidden' name='suchtimer' value='".$Timer_Array_show['suchtimer']."'>";

echo "<table class='tab1'>"; 
echo "<tr><th>Timer-Nr:    </th><td>".$Timer_Array_show['timer_nr']."</td></tr>";
echo "<tr><th>Datei:       </th><td><input type='text' size='50' maxlength='50' name='datei'       value='".$Timer_Array_show['datei']."'>       </td></tr>";
echo "<tr><th>Verzeichnis: </th><td><input type='text' size='50' maxlength='50' name='verzeichnis' value='".$Timer_Array_show['verzeichnis']."'> </td></tr>";

echo "<tr><th>Status:       </th><td>";
if ($Timer_Array_show['aktiv']==1) {$checked_1="checked"; $checked_0="";} else {$checked_1=""; $checked_0="checked";}
echo   " <input type='radio' name='aktiv' value='1' $checked_1> aktiv ";
echo   " <input type='radio' name='aktiv' value='0' $checked_0> nicht aktiv ";
echo "</td></tr>";


echo "<tr><th>Channel:     </th><td>"; 
echo "<input type='text' size='10' maxlength='10' name='channel' value='".$Timer_Array_show['channel']."'>";
 /*
$channels_conf = file("/var/lib/vdr/channels.conf");

$eintrage=explode(':',$inhalt);
echo "<select name='channel' size='1'>";
foreach($channels_conf as $zeile)
{
  echo "<option ";
  $zeile_array=explode(':',$zeile);
  if ($zeile_array[0]!="") {
    $zeile_array2 = explode(';',$zeile_array[0]);
    if ($inhalt[0] == $channel) {echo "selected ";}
    echo "value=$inhalt[0]>$zeile_array2[0]</option>";
  }
}
echo "</select>"; */
echo "</td></tr>";

echo "<tr><th>Tag:         </th><td><input type='text' size='10' maxlength='10' name='tag' value='".$Timer_Array_show['tag']."'></td></tr>";
echo "<tr><th>Anfang:      </th><td><input type='text' size='10' maxlength='10' name='anfang' value='".$Timer_Array_show['anfang']."'></td></tr>";
echo "<tr><th>Ende:        </th><td><input type='text' size='10' maxlength='10' name='ende' value='".$Timer_Array_show['ende']."'></td></tr>";
echo "<tr><th>VPS:         </th><td><input type='checkbox' name='vps' value='1'></td></tr>";
echo "<tr><th>Priorität:   </th><td><input type='text' size='10' maxlength='10' name='prio' value='".$Timer_Array_show['prio']."'></td></tr>";
echo "<tr><th>Lebensdauer:          </th><td><input type='text' size='10' maxlength='10' name='lebensdauer' value='".$Timer_Array_show['lebensdauer']."'></td></tr>";
echo "<tr><th>Kindersicherung:      </th><td><input type='checkbox' name='kindersicherung' value='1'></td></tr>";
echo "<tr><th valign=top>Suchtimer: </th><td>".htmlentities($Timer_Array_show['suchtimer']). "</td></tr>"; 
echo "</table>";
echo "<br/>";

if ($timer_nr == "neu") {
  echo "<input type='submit' name='neu' value='Neuen Timer anlegen'>";
}
else {
  echo "<input type='submit' name='save' value='Änderungen speichern'>";
}

echo "</form>";
echo "<a href='timers.php?timer_nr=$timer_nr'><button>Abbrechen</button></a>";

container_end();
// *************************************** Container Ende ****************************************************

?>