<?php

$series_id = $_GET[series_id];

$abfrage = "SELECT * FROM series WHERE Series_id=$series_id";
$ergebnis = mysql_query($abfrage);
$row = mysql_fetch_object($ergebnis);


echo "<h1>Serie: $row->series_name</h1>";

echo "<a href='series_info.php?series_id=".$series_id."' class='button1'>Info</a>";
echo "&nbsp;&nbsp;";
echo "<a href='series_episode.php?series_id=".$series_id."' class='button1'>Episoden</a>";
echo "&nbsp;&nbsp;";

echo "<br>";
?>