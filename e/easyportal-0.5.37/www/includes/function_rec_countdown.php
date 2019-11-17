<?PHP
$no_timer = 0;

$nexttimer = exec("sudo $SH_BEFEHL svdrpsend next abs | grep 250| cut -d' ' -f3");
//echo "'$nexttimer'";
if ($nexttimer == "") { 
  // keine Timer
  $sec_bis_timer = 0;
  $no_timer = 1;
} 
else {
  $sec_bis_timer = $nexttimer - time();
}
//$sec_bis_timer = 0;
$txt_debug_meldungen[] = "sec_bis_timer: $sec_bis_timer";
?>

<script type="text/javascript">
var zeit = window.setInterval("countdown()", 1000);
var i = <?php echo $sec_bis_timer; ?>;
var no_timer = <?php echo $no_timer; ?>;

function addLeadingZeros(number, length) {
  // führende Nullen hinzufügen
  var num = '' + number;
  while (num.length < length) num = '0' + num;
  return num;
}

function countdown() {
  if (no_timer == 1) {
    window.clearInterval(zeit);
    document.getElementById("rec_countdown").firstChild.data = "<?php echo $txt_keine_timer; ?>";
  }
  else {
    if (i > 0) {
      sec = i;
      var minute = Math.floor(sec/60);
      sec %= 60;
      sec = addLeadingZeros(sec,2);
      var hour = Math.floor(minute/60);
      minute %= 60;
      minute = addLeadingZeros(minute,2);
      document.getElementById("rec_countdown").firstChild.data = "<?php echo $txt_zeit_bis_record; ?> " + hour + ":" + minute + ":" + sec;
    } 
    else {
      window.clearInterval(zeit);
      document.getElementById("rec_countdown").style.color = 'red';
      document.getElementById("rec_countdown").firstChild.data = "<?php echo $txt_achtung_record; ?>";
    }
  }
  i--;
}
</script>

