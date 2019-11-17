<?php

function redirect($url) {
  session_write_close();
  
  // Für URLs wie /bla/
  $arr = parse_url($url);
  if (!isset($arr['scheme'])) {
    //$url = 'http://' . $_SERVER['SERVER_NAME'] . $url;
    $url = 'http://' . $_SERVER['SERVER_NAME'] . ( $_SERVER['SERVER_PORT'] == 80 ? "" : ":".$_SERVER['SERVER_PORT'] ) . $url;
    if (defined('SID') AND SID != '') {
        $url .= (strpos($url, '?') ? '&' : '?') . SID;
    }
  }
  
  if (!headers_sent()) {
    if (ob_get_level() != 0) { ob_end_clean(); }
    header("Location: " . $url);
  }
  ?>
  <html>
    <meta http-equiv="Refresh" content="0;url=<?=$url?>">
    <body onload="try { self.location.href='<?=$url?>' } catch(e) {}">
      <a href="<?=$url?>">Redirect</a>
    </body>
  </html>
  <?php
  die();
}
?>

