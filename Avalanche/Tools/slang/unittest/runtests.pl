#
# runtests.pl - run the unit tests, verify results
#

use strict;

mkdir( "out" );

my @tests = <*.sla>;
for ( @tests )
{
	run( $_ );
}

#=============================================================================

sub run
{
	my( $file ) = @_;
	my( $correctResults ) = "";
	
	# read the file and get results
	open( F, $file ) or die "ERROR: couldn't open $file for read\n";
	while( <F> )
	{
		$correctResults .= "$1\n" if m;result:\s*(.*)$;
	}
	close( F );

	# run, comple, get results from first run and reloaded exe run	
	my $exe = "out\\$file";  $exe =~ s;\.sla$;.sx;i;
        my $results    = `..\\debug\\slang.exe -s $file -r -o $exe`;
        my $exeResults = `..\\debug\\slang.exe -x $exe -r`;

	verify( "$file", $correctResults, $results );
	verify( "$exe",  $correctResults, $exeResults );
}

#=============================================================================

sub verify
{
	my( $file, $correctResults, $results ) = @_;
	my( @correctResults ) = split( /\n/, $correctResults );
	my( @results )        = split( /\n/, $results );
	
	# verify results
	if ( scalar( @correctResults ) != scalar( @results ) )
	{
		printf "%-16s : FAIL: incorrect # of results ( want %3d, have %3d )\n", $file, scalar( @correctResults ), scalar( @results );
	}
	else
	{
		my $i;
		my @pass = ();
		my @fail    = ();
		for ( $i = 0 ; $i <= $#results ; $i++ )
		{
			if ( $results[ $i ] ne $correctResults[ $i ] )
			{
				push( @fail, $i );
			}
			else
			{
				push( @pass, $i );
			}
		}
		
		my $passfail = scalar( @fail ) == 0 ? "ok  " : "FAIL";
		
		printf "%-16s : $passfail ( %3d pass, %3d fail )\n", $file, scalar( @pass ), scalar( @fail );
		
		for ( @fail )
		{
			printf( "\ttest %d - expected '%s', got '%s'\n", $_ + 1, $correctResults[$_], $results[$_] );
		}
	}
}

#=============================================================================
