#!/usr/bin/perl
use OSDServer;

my $server = OSDServer->Open() or die "open";
my $menu = $server->NewMenu("Hello World selecting");
$menu->EnableEvent(["keyOk", "close"]);
my $one = $menu->AddNewOsdItem("Select one thing");
my $another = $menu->AddNewOsdItem("Select another thing");
my $different = $menu->AddNewOsdItem("Select something different");
$menu->Show();
my (undef,undef,$event) = $menu->SleepEvent();
my (undef, $item, undef) = $menu->GetCurrent();

if ($item eq $one and $event eq "keyOk") {
	print "One thing selected.\n";
} elsif ($item eq $another and $event eq "keyOk") {
	print "Another thing selected.\n";
} elsif ($item eq $different and $event eq "keyOk") {
	print "Something different selected.\n";
} elsif ($event eq "close") {
	print "Nothing selected.\n";
}
$server->Close();
