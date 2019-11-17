<script>
function Wuerfel(){
  document.getElementById("A1").style.backgroundImage="url(/images/wuerfel/w"+RND()+".gif)";
  document.getElementById("A2").style.backgroundImage="url(/images/wuerfel/w"+RND()+".gif)";
  document.getElementById("A3").style.backgroundImage="url(/images/wuerfel/w"+RND()+".gif)";
  document.getElementById("A4").style.backgroundImage="url(/images/wuerfel/w"+RND()+".gif)";
  document.getElementById("A5").style.backgroundImage="url(/images/wuerfel/w"+RND()+".gif)";
  document.getElementById("A6").style.backgroundImage="url(/images/wuerfel/w"+RND()+".gif)";
}

function RND(){
 return Math.ceil(Math.random()*6);
}

</script>

<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>Würfelspiel</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>Würfelspiel</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Würfelspiel");

echo "<br/>";
echo "<input type='button' value='Würfeln' onClick='Wuerfel();'><br/><br/>";
echo "<br/>";
echo "<img src='/images/wuerfel/spacer.gif' width='15' height='15' id='A1'> ";
echo "<img src='/images/wuerfel/spacer.gif' width='15' height='15' id='A2'> ";
echo "<img src='/images/wuerfel/spacer.gif' width='15' height='15' id='A3'> ";
echo "<img src='/images/wuerfel/spacer.gif' width='15' height='15' id='A4'> ";
echo "<img src='/images/wuerfel/spacer.gif' width='15' height='15' id='A5'> ";
echo "<img src='/images/wuerfel/spacer.gif' width='15' height='15' id='A6'> ";
echo "<br/><br/><br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

?>
<script>
for (var i = 1; i < 7; i++) {
 document.getElementById("A"+i).style.backgroundImage="url(/images/wuerfel/w"+i+".gif)";
}
</script>
<?php

include('includes/fuss.php'); 
?>

