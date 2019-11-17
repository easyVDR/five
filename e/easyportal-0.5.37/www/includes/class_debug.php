<?PHP
// Ersatz für "print_r($array)". Gibt ein Array etwas schöner aus
// Aufruf:  "Debug::dump($array, true, '', true);"

final class Debug extends Exception
{
  static public function dump($var = null, $echo = true, $label = null, $hl = true)
  {
    if ($var === null) { throw new Exception('First argument is missing, actually $var is null.'); }
    if (($label !== null) && (is_string(trim($label)))) { $label = rtrim($label) . "\n"; }
    ob_start();
    var_dump($var);
    $output = ob_get_clean();
    if (empty($output)) { throw new Exception('Something goes wrong. The output is empty.'); }
    $output = preg_replace("/\=\>(\s+)/", " => ", $output);
    $output = htmlentities($output, ENT_QUOTES, 'UTF-8');
    if ($hl === true) {
      $nColSpan = '<span style="color:%s;">%s</span>';
      $bColSpan = '<span style="color:%s;font-weight:bold;">%s</span>';
      $pattern = array(
        0 => '/([ ]?)(string|int|float|array)\(([0-9\.]+)\)/',
        1 => '/\[([0-9]+)\]/',
        2 => '/\[(&quot;.*&quot;)\]/',
        3 => '/[ ]{1}NULL/',
        4 => '/(object)\(([\w]+)\)(#[0-9]+) \(([0-9]+)\)/',
        5 => '/(resource)\(([0-9]+)\) of type \(([\w\s]+)\)/',
        6 => '/[ ]{1}\{/',
        7 => '/([\n\s]*)\}/'
      );
      $replace = array(
        0 => '\\1' . sprintf($nColSpan, '#007700', '\\2') . '(' . sprintf($nColSpan, '#DD0000', '\\3') . ')',
        1 => '[' . sprintf($nColSpan, '#DD0000', '\\1') . ']',
        2 => '[' . sprintf($nColSpan, '#DD0000', '\\1') . ']',
        3 => ' ' . sprintf($bColSpan, '#0000BB', 'NULL'),
        4 => sprintf($nColSpan, '#007700', '\\1') . '(' . sprintf($bColSpan, '#0000BB', '\\2') . ')' . sprintf($bColSpan, '#000000', '\\3'),
        5 => sprintf($nColSpan, '#007700', '\\1') . '(' . sprintf($bColSpan, '#0000BB', '\\2') . ') of type (' . sprintf($bColSpan, '#000000', '\\3') . ')',
        6 => sprintf($bColSpan, '#0000BB', ' {'),
        7 => '\\1' . sprintf($bColSpan, '#0000BB', '}'),
      );
      $output = preg_replace($pattern, $replace, $output);
    }
    $output = '<pre>' . $label . $output . '</pre>';
    if ($echo !== true) { return $output; }
    echo($output);
  }
}

?>
