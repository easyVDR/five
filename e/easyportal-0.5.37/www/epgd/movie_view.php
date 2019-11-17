<?php
$movie_id = $_GET[movie_id];

include("inc/inc_head.php");
//include("inc/inc_series.php");

$abfrage = "SELECT * FROM movie WHERE movie_id='$movie_id'";
$ergebnis = mysql_query($abfrage);
$row = mysql_fetch_object($ergebnis);


echo "<h1>Film<h1>";

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>Movie-ID</th>";
echo "    <td>".$row->movie_id."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Film-Name</th>";
echo "    <td>".$row->movie_title."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Original-Titel</th>";
echo "    <td>".$row->movie_original_title."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Tagline</th>";
echo "    <td>".$row->movie_tagline."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Overview</th>";
echo "    <td>".$row->movie_overview."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Altersbeschränkung</th>";
echo "    <td>".$row->movie_adult."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Budget</th>";
echo "    <td>".$row->movie_budget."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Genre</th>";
echo "    <td>".$row->movie_genres."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Homepage</th>";
echo "    <td><a href='".$row->movie_homepage."'>".$row->movie_homepage."</a>&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Release Date</th>";
echo "    <td>".datum2de($row->movie_release_date)."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Länge</th>";
echo "    <td>".$row->movie_runtime."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Popularität</th>";
echo "    <td>".$row->movie_popularity."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Vote Average</th>";
echo "    <td>".$row->movie_vote_average."&nbsp;</td>";
echo "  </tr>";

echo "</table>";
echo "<br>";

echo "<h2>Schauspieler</h2>";


echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>Schauspieler</th>";
echo "    <th>Rolle</th>";
echo "  </tr>";

$abfrage = "SELECT a.actor_id, a.actor_role, b.actor_name
            FROM movie_actors a 
            LEFT JOIN movie_actor b ON (a.actor_id = b.actor_id)
            WHERE a.movie_id='$movie_id'";

$ergebnis = mysql_query($abfrage);
while($row = mysql_fetch_object($ergebnis))
{
  echo "  <tr>"; 
  echo "    <td><a href='actor.php?actor_id=".$row->actor_id."'>".$row->actor_name."</a></td>";
  echo "    <td>".$row->actor_role."</td>";
  echo "  </tr>";
}
echo "</table>";


include("inc/inc_foot.php");
?>