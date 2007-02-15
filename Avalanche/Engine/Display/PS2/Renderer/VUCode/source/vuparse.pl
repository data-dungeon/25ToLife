	eval "\$$1=\$2" while $ARGV[0] =~ /^(\w+)=(.*)/ && shift;

	if( $process eq "start_hdr" ) {
		die "Pass a defname=filename" if $defname eq "";

		$defname =~ tr/a-z/A-Z/;
		$defname =~ tr/\./_/;
		print "#ifndef __" . $defname . "\n";
		print "#define __" . $defname . "\n\n";
		print "#define mVUCodeAddr(adr) extern unsigned int adr __attribute__\( \( section\( \".vudata\" \)\)\)\;\n"

	}
	elsif( $process eq "add_hdr" )
	{
		print "\n";

		if( defined($filename) ) {
			 print "\n\/\/ globals from " . $filename . "\n";
		}
		while(<>){
		  if(!($filename eq $ARGV))
		  {
			 $filename = $ARGV;
			 print "\n\/\/ globals from " . $filename . "\n";
		  }
		  print "mVUCodeAddr( $1\)\n" if /\export[ \t]*\([ \t]*(\w*)[ \t]*\)/;
		  print "mVUCodeAddr( $1\)\n" if /\.global[ \t]*(\w*)[ \t]*/;
		}

		print "\n";

	}
	elsif( $process eq "finish_hdr" ) {
		print "#endif\n";
	}
	elsif( $process eq "write_tbl" ) {

           $tname = 'vuAnimTable' . $1 if $tname =~ /Anim(.*)MT.cpp/;

		if( $nspace ) {
			$tname = $nspace . "::" . $tname;
		}

		print "unsigned int " . $tname . "[] =\n{\n";

		while( <> ) {
	 		if( /tableentry[ \t]*\([ \t]*(\w*)[ \t]*\)/ ) {
				print "\t(unsigned int) \&$1,\n";
			}
		}

		print "} ;\n\n";

		print "const unsigned int " . $tname . "Entries = sizeof\(" . $tname . "\) \/ sizeof\(unsigned int\);\n";
	}
