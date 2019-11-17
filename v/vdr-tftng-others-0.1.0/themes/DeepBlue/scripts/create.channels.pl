#!/usr/bin/perl

if (not defined $ARGV[0]) {die "No parameter given!\n"};

my $path = "/etc/vdr/plugins/graphTFT/themes/DeepBlue/scripts";


$cnt = $cnt1 = 0;

%scan = ();
open (SCAN, "< $path/astra.conf") or die "Can't open file\n";
while (<SCAN>) {
	
    $line = $_;
    $line =~ s/\r//;
    $line =~ s/\n//;
    if ($line eq '' or $line =~ /^:/ or $line =~ /^@/ ) { next; }

    next if (/^:/);
    chomp;
    my($name, $frequency, $polarization, $source, $symbolrate, $vpid, $apid,
     $tpid, $ca, $service_id, $nid, $tid, $rid) = split(/\:/, $_);

    if ( $source eq 'T' || $source eq 'C' ) {
        if ( length($frequency) > 3) {
	    $frequency=substr($frequency, 0, length($frequency)-3);
	}
	if ( length($frequency) > 3) {
	    $frequency=substr($frequency, 0, length($frequency)-3);
	}
    }

    $data = $nid>0 ? $tid : $frequency;
	
    unless ($chan{"$source-$nid-$data-$service_id"}) {
        $scan{"$source-$nid-$data-$service_id"} = $line ;
        $cnt++;
    }
    else { print "$source-$nid-$data-$service_id", "\t", $line, "\n"; $cnt1++; }
}
close(SCAN) or die "Can't close file\n";

@other = ();
open (CONF, ">>$path/channels.conf") or die "Can't open file!\n";
open (REF, "< $ARGV[0]") or die "Can't open file\n";
while (<REF>) {

    $line = $_;
    $line =~ s/\r//;
    $line =~ s/\n//;
    if ($line eq '') { next; }
    if ($line =~ /^:/ or $line =~ /^@/ ) { print CONF $line, "\n"; next; }

    next if (/^:/);
    chomp;
    my($name, $frequency, $polarization, $source, $symbolrate, $vpid, $apid,
     $tpid, $ca, $service_id, $nid, $tid, $rid) = split(/\:/, $_);

    if ( $source eq 'T' || $source eq 'C' ) {
        if ( length($frequency) > 3) {
	    $frequency=substr($frequency, 0, length($frequency)-3);
	}
	if ( length($frequency) > 3) {
	    $frequency=substr($frequency, 0, length($frequency)-3);
	}
    }

    $data = $nid>0 ? $tid : $frequency;
	
    if ($scan{"$source-$nid-$data-$service_id"}) {
	print CONF $scan{"$source-$nid-$data-$service_id"}, "\n";
	delete $scan{"$source-$nid-$data-$service_id"};
    }
}

for my $key ( keys %scan ) { 
    my $line = $scan{$key};
    print CONF $line, "\n";
}

close (CONF) or die "Can't close file\n";
close(REF) or die "Can't close file\n";

