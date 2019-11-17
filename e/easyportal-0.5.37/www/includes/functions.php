<?PHP
# Importiere URL Parameter aus $_POST
function safepost ($param) {
  $retval="";
  if (isset($_POST["$param"])) $retval=$_POST["$param"];
  return $retval;
}

# Importiere URL Parameter aus $_GET
function safeget ($param) {
  $retval="";
  if (isset($_GET["$param"])) $retval=$_GET["$param"];
  return $retval;
}

function unhtmlentities($string)
{
  // replace numeric entities
  $string = preg_replace('~&#x([0-9a-f]+);~ei', 'chr(hexdec("\\1"))', $string);
  $string = preg_replace('~&#([0-9]+);~e', 'chr("\\1")', $string);
  // replace literal entities
  $trans_tbl = get_html_translation_table(HTML_ENTITIES);
  $trans_tbl = array_flip($trans_tbl);
  return strtr($string, $trans_tbl);
}
?>
