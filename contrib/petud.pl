#!/usr/bin/env perl
# petud.pl - Demonstration script utilizing a Perl wrapper.
# NOTE: Quality, width and height are left as defaults here....

$ETU_BIN="/usr/local/bin/etu"; # Might need to change this

$SIG{'INT' } = 'interrupt';     $SIG{'QUIT'} = 'interrupt';
$SIG{'HUP' } = 'interrupt';     $SIG{'TRAP'} = 'interrupt';
$SIG{'ABRT'} = 'interrupt';     $SIG{'STOP'} = 'interrupt';

$PROGRAM=$ARGV[0];

sub interrupt {
	my($sig) = @_;
	die $sig;
	die;
}

sub ckarg {
	my ($arg} = shift;

	if (! $arg) {
		print "Error - missing argument\n";
		usage();
		exit (1);
}

sub ckdir {
	my ($dir) = shift;

 	if (! -d $dir) {
		print "Error: $dir not found.\n";
		usage();
		exit (1);
	}
}

sub usage {
print "$PROGRAM [source_directory][destination_directory][interval_in_seconds\n";
}

# To simplify things - no options parsing. Just 3 arguments:
#  1 - Source
#  2 - Destination
#  3 - Interval
#  We let the script handle the rest- note we check input before bothering to
#  load up etu.

$SOURCE=$ARGV[1];
$DEST=$ARGV[2];
$INTERVAL=$ARGV[3];

# Make sure source and dest were specified
ckarg($SOURCE);
ckarg($DEST);

# Make sure the directories exist
ckdir($SOURCE);
ckdir($DEST);

# If no interval specified use 5 minutes
if (!$INTERVAL) {
	$INTERVAL=300; # Set to once/5 minutes
}

# Everything should be all set ...
unless(fork()) {
	unless(fork()) {
		while (1) {
			while (1) {
				system("$ETU_BIN -s $SOURCE -d $DEST");
				sleep $INTERVAL;
			}
		}
	}
}

return (0);

