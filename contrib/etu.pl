#!/usr/bin/env perl
# petu.pl - A set of perl routines that set up an etu call.

##
# Make sure a directory exists
##
sub etu_chkdir {
	my ($dir) = shift;

	if (! -d $dir) {
		print "Error: $dir not found\n";
		return 1;
	}

	return 0;
}

##
# Make sure the quality is a valid nymber btween 1 and 100
##
sub etu_setquality {
	my ($quality) = shift;

	if ($quality < 1) {
		print "Error: Quality must be greater than or equal to 1";
		return 1;
	}

	return 0;
}

##
# Check for negative values
##
sub etu_negcheck {
	my ($value) = shift;

	if ($value <= 0) {
		print "Error: Value must be greater than 0\n";
		return 1;
	}
	
	return 0;
}

##
# Execute etu; the arguments in order are:
# - source directory
# - destination directory (or cache directory)
# - height
# - width
# - quality
##
sub etu_exec {
	my ($src,$dst,$height,$width,$qual) = $@;

	if (!$height) {
		$height=80;
	}

	if (!$width) {
		$width=100;
	}

	if (!$qual) {
		$qual=90;
	}

 	system("/usr/local/bin/etu -s $src -d $dst -h $height -w $width -q $qual") 
}


1; # Main - return a true value

__END__
=head1 DESCRIPTION

This is a small example library for Perl over top of etu.
based on the source directory.

The usage for each function is:
	etu_chkdir (directory_name); 
	etu_setquality (quality);
	etu_begcheck(number);
	etu_exec(source_dir, dest_dir, height, width, quality);

Note that etu_exec only requires source_dir and dest_dir - it will pick
reasonable defaults for the remaining arguments.

=head1 EXAMPLES

	etu_exec ($HOME/images-orig, $HOME/images-rescaled, 80, 100, 90);
	etu_negcheck($height);


=head1 PREREQUISITES

etu

=head1 COREREQUISITES

none

=cut


