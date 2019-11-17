<?php
$episode_id = $_GET[episode_id];
$series_id = $_GET[series_id];

include("inc/inc_head.php");
include("inc/inc_series.php");

$abfrage = "SELECT * FROM series_episode WHERE episode_id='$episode_id'";
$ergebnis = mysql_query($abfrage);
$row = mysql_fetch_object($ergebnis);


echo "<h1>Episode<h1>";

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>Episode-ID</th>";
echo "    <td>".$row->episode_id."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Episode-Nr</th>";
echo "    <td>".$row->episode_number."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Staffel-Nr</th>";
echo "    <td>".$row->season_number."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Episoden-Name</th>";
echo "    <td>".$row->episode_name."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Overview</th>";
echo "    <td>".$row->episode_overview."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Erstaustrahlung</th>";
echo "    <td>".datum2de($row->episode_firstaired)."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Gast-Stars</th>";
echo "    <td>".$row->episode_gueststars."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Rating</th>";
echo "    <td>".$row->episode_rating."&nbsp;</td>";
echo "  </tr>";
echo "</table>";
echo "<br>";

echo "<img src='series_media_episodes.php?id=".$row->episode_id."'>";

include("inc/inc_foot.php");
?>