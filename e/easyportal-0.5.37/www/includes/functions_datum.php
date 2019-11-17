<?PHP

// Datum von Englisch ins deutsche konvertieren
function datum_ed($datum) {
  $jahr = substr($datum,0,4);
  $mon  = substr($datum,5,2);
  $tag  = substr($datum,8,2);
  $datneu = $tag.'.'.$mon.'.'.$jahr;
  return $datneu;
}

// Datum von Deutsch ins englische konvertieren (für Datenbank):
function datum_de($datum)
{
  $jahr = substr($datum,6,4);
  $mon  = substr($datum,3,2);
  $tag  = substr($datum,0,2);
  $datneu = $jahr.'-'.$mon.'-'.$tag;
  return $datneu;
}

?>
