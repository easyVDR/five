<?php
include("inc/inc_head.php");

$actor_id = $_GET[actor_id];

echo "<h1>Schauspieler<h1>";

echo "<table>";
echo "  <tr>";
echo "    <td valign=top>";

$abfrage = "SELECT * FROM movie_actor where actor_id=$actor_id";
$ergebnis = mysql_query($abfrage);
$row = mysql_fetch_object($ergebnis);

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>ID</th>";
echo "    <td>".$row->actor_id."</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Name</th>";
echo "    <td>".$row->actor_name."</td>";
echo "  </tr>";
echo "</table>";

echo "<h2>Filme</h2>";

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>Film-ID</th>";
echo "    <th>Film</th>";
echo "  </tr>";

$abfrage = "SELECT a.movie_id, m.movie_title FROM movie_actors a
            LEFT JOIN movie m ON (m.movie_id = a.movie_id)
            WHERE a.actor_id=$actor_id
            ORDER BY m.movie_title";

$ergebnis = mysql_query($abfrage);
while($row = mysql_fetch_object($ergebnis))
{
    echo "  <tr>";
    echo "    <td><a href='movie_view.php?movie_id=".$row->movie_id."'>".$row->movie_id."</a>&nbsp;</td>";
    echo "    <td><a href='movie_view.php?movie_id=".$row->movie_id."'>".$row->movie_title."</a>&nbsp;</td>";
    echo "  </tr>";
}
echo "</table>";

echo "    </td>";
echo "    <td>";


$abfrage = "SELECT * FROM movie_media where actor_id=$actor_id";
$ergebnis = mysql_query($abfrage);
while($row = mysql_fetch_object($ergebnis))
{
   echo "<img src='image.php?id=".$actor_id."'>";
}


echo "    </td>";
echo "</table>";

echo "<br>";

include("inc/inc_foot.php");
?>