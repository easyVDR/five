<?php
$series_id = $_GET[series_id];

include("inc/inc_head.php");
include("inc/inc_series.php");

echo "<h1>Episoden<h1>";

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>Episode-Nr</th>";
echo "    <th>Season Nr</th>";
echo "    <th>Name</th>";
//echo "    <th>Genre</th>";
echo "  </tr>";

$abfrage = "SELECT * FROM series_episode WHERE series_id='$series_id' ORDER BY season_number, episode_number";
$ergebnis = mysql_query($abfrage);
while($row = mysql_fetch_object($ergebnis))
{
    echo "  <tr>";
    echo "    <td>".$row->episode_number."&nbsp;</td>";
    echo "    <td>".$row->season_number."&nbsp;</td>";
    echo "    <td><a href='series_episode_view.php?episode_id=".$row->episode_id."&series_id=".$row->series_id."'>".$row->episode_name."</a>&nbsp;</td>";
//    echo "    <td><img src='../images/kategorie/".$row->kategorie.".jpg' height='50'>&nbsp;</td>";
//    echo "    <td><a href='v_kategorie_edit.php?ID=".$row->ID."'><button class='button1'>Edit</button></a></td>";
    echo "  </tr>";
}
echo "</table>";
echo "<br>";

include("inc/inc_foot.php");
?>