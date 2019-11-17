#!/usr/bin/perl -w

$top = 28;	# Number of LEDs at the top
$bot = 28;	# Number of LEDs at the bottom
$left = 16;	# Number of LEDs on the left => might be higher than physically when $cor=1
$right = 16;	# Number of LEDs on the right => might be higher than physically when $cor=1

$horz = 3;	# horizontal deep on left/right (grabbed Fields)
$vert = 2;	# vertical deep at top/bottom (grabbed Fields)

@fit = ('bot', 'right', 'top', 'left');		# pyhsical/technical LED setup

$dir = 'r';	# forward or reverse (clock) direction (f/r)
$cor = 0;	# have a single LED in each corner (0/1)
$shift = 0;	# shift first LED to last position (0/1)

############## Do not touch below here ##############

$top = $top-1;
$bot = $bot-1;
$left = $left-1;
$right = $right-1;
$horz = $horz-1;
$vert = $vert-1;

for ($t=0;$t<=$top;$t++) { 
  $value = "led top $t 0";
  push(@top, $value);
}

for ($b=$bot;$b>=0;$b--) {
  $vert1 = $right-$vert;
  $value = "led bot $b $right";
  push(@bot, $value);
}

for ($l=$left;$l>=0;$l--) {
  if ( $cor==1 and ($l==0 or $l==$left-1)) { next; }
  $value = "led left 0 $l";
  push(@left, $value);
}

for ($r=0;$r<=$right;$r++) {
  if ( $cor==1 and ($r==0 or $r==$right-1)) { next; }
  $horz1 = $top-$horz;
  $value = "led right $top $r";
  push(@right, $value);
}

if ($dir eq 'r') { 
  @left = reverse(@left);
  @bot = reverse(@bot);
  @right = reverse(@right);
  @top = reverse(@top);
}


open (CONF, ">seduatmo.conf") or die "Can't open CONF file\n";

$cnt = 1;
foreach $side(@fit) {
  print CONF "# $side\n";
  if ($cnt==1 and $shift==1) {
    $first = $$side[0];
    shift(@$side);
  }
  print CONF join("\n",@$side);
  print CONF "\n\n";
  $cnt++
}

if ($shift==1) {
  print CONF "# shift first LED to last position\n";
  print CONF $first, "\n";
}

close CONF;
