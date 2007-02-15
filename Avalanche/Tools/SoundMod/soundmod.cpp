//============================================================================
//==
//== Project: soundmod
//== File:    soundmod.cpp
//== Desc:    Builds a composite sound bank file from wav/aiff files and
//==          control modules files.
//== Author:  Rob Nelson
//== Date:    2/6/2003
//== (c)      2003 Avalanche Software, Inc.
//==
//============================================================================

#include <windows.h>
#include "soundmod.h"
#include "parser.h"


//===== DEFINES ==============================================================

//===== TYPES ================================================================

//===== VARIABLES ============================================================

int	verboseOutput = 0;
bool	doNothing     = false;
bool	forceBuild    = false;
int	newestModuleFileTime = 0;
char* bankFilename  = NULL;
char* sourcePath    = "";
bool	exportDBL	  = false;

ExpandableArray<Source> sourceArr;
ExpandableArray<Sound>  soundArr;
ExpandableArray<Group>  groupArr;
ExpandableArray<Target> targetArr;


//===== PROTOTYPES ===========================================================

void parseScript( char* filename );


//============================================================================

int expandVar( char* dst, char* varStart, int len, char* source, char* target, char* loopArg )
{
	char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ];
   char workBuf[ _MAX_PATH  ];

	int i;
	int nArgs = 0;
	char* args[ 16 ];

	char tempBuf[ 256 ];
	char* p = tempBuf;
	memcpy( tempBuf, varStart, len );
	tempBuf[ len ] = 0;

	while ( *p != 0 && nArgs < 16 )
	{
		args[ nArgs++ ] = p;
		while ( *p != 0 && *p != ':' ) p++;
		while ( *p != 0 && *p == ':' ) *p++ = 0;
	}

	if ( nArgs > 0 )
	{
		if      ( stricmp( args[ 0 ], "source"  ) == 0 ) { strcpy( workBuf, source );  }
		else if ( stricmp( args[ 0 ], "target"  ) == 0 ) { strcpy( workBuf, target );  }
		else if ( stricmp( args[ 0 ], "looparg" ) == 0 ) { strcpy( workBuf, loopArg ); }
		else
		{
			fprintf( stderr, "ERROR: unknown variable '%s'\n", args[ 0 ] );
			exit( 1 );
		}

		for ( i = 1 ; i < nArgs ; i++ )
		{
			if ( stricmp( args[ i ], "path" ) == 0 )
			{
				_splitpath( workBuf, drive, dir, fname, ext );
				sprintf( workBuf, "%s%s", drive, dir );
			}
			else if ( stricmp( args[ i ], "filename" ) == 0 )
			{
				_splitpath( workBuf, drive, dir, fname, ext );
				sprintf( workBuf, "%s%s", fname, ext );
			}
			else if ( stricmp( args[ i ], "name" ) == 0 )
			{
				_splitpath( workBuf, drive, dir, fname, ext );
				sprintf( workBuf, "%s", fname );
			}
			else if ( stricmp( args[ i ], "ext" ) == 0 )
			{
				_splitpath( workBuf, drive, dir, fname, ext );
				sprintf( workBuf, "%s", ext );
			}
			else
			{
				fprintf( stderr, "ERROR: unknown modifier '%s' on variable '%s'\n", args[ i ], args[ 0 ] );
				exit( 1 );
			}
		}
	}

	return sprintf( dst, "%s", workBuf );
}

//============================================================================

void expandLine( char* line, char* result, char* source, char* target, char* loopArg )
{
	char* s = line;
	char* d = result;

	while ( *s != 0 )
	{
		if ( *s != '[' )
			*d++ = *s++;
		else
		{
			char* v = s + 1;
			while ( *v != 0 && *v != ']' ) v++;
			d += expandVar( d, s + 1, v - s - 1, source, target, loopArg );
			s = v;
			if ( *s != 0 ) s++;
		}
	}
	*d = 0;
}

//============================================================================

void parseModulesSection( Parser& p )
{
	while ( p.NextLine() && p.arg[0][0] != '[' )
	{
		if ( verboseOutput )
			p.Print( "include module '%s'\n", p.arg[ 0 ] );
		parseScript( p.arg[ 0 ] );
	}
}

//============================================================================

bool filenamesMatch( const char* file1, const char* file2 )
{
	// test if two filenames are the same, ignoring the extension
	for ( ; tolower(*file1) == tolower(*file2) ; file1++, file2++ )
		if ( *file1 == 0 || *file1 == '.' )
			return true;
	return false;
}

//============================================================================

#define MIN_VOLUME	0.0f
#define MAX_VOLUME	100.0f
#define MIN_PITCH		-24.0f	// 2 octives down
#define MAX_PITCH		24.0f		// 2 octives high

void parseSoundsSection( Parser& p )
{
	int i;

	while ( p.NextLine() && p.arg[0][0] != '[' )
	{
		if ( p.nArgs < 2 )
			p.Error( "sound '%s' has too few parameters\n", p.arg[ 0 ] );

		//-- make sure this sound name isn't already entered
		for ( i = 0 ; i < soundArr.Count() ; i++ )
			if ( stricmp( soundArr[ i ].name, p.arg[ 0 ] ) == 0 )
				break;
		if ( i < soundArr.Count() )
		{
			p.Print( "sound '%s' redefined, ignoring.\n", p.arg[ 0 ] );
			continue;
		}

		Sound s;
		s.name = strdup( p.arg[ 0 ] );
		s.volume1     = 100.0f;
		s.volume2     = 100.0f;
		s.pitch1      = 0.0f;
		s.pitch2      = 0.0f;
		s.presence    = 0.0f;
		s.ambience    = 0.0f;
		s.audibleDist = 0.0f;
		s.wet         = -1.0f;		// negative means not specified by mod file
		s.dry         = -1.0f;		// negative means not specified by mod file

		Source src;
		src.name		 = strdup( p.arg[ 1 ] );
		src.is2D     = false;
		src.looping  = false;

		//-- now parse any arguments
		for ( i = 2 ; i < p.nArgs ; i++ )
		{
			if ( stricmp( p.arg[ i ], "v" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'v'\n", s.name );
				s.volume1 = s.volume2 = (float) atof( p.arg[ ++i ] );
				if ( s.volume1 < MIN_VOLUME || s.volume1 > MAX_VOLUME ) p.Error( "sound '%s' volume out of range (%f) [must be %.0f..%.0f]\n", s.name, s.volume1, MIN_VOLUME, MAX_VOLUME );
			}
			else if ( stricmp( p.arg[ i ], "p" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'p'\n", s.name );
				s.pitch1 = s.pitch2 = (float) atof( p.arg[ ++i ] );
				if ( s.pitch1 < MIN_PITCH || s.pitch1 > MAX_PITCH ) p.Error( "sound '%s' pitch out of range (%f) [must be %.0f..%.0f]\n", s.name, s.pitch1, MIN_PITCH, MAX_PITCH );
			}
			else if ( stricmp( p.arg[ i ], "vrange" ) == 0 )
			{
				if ( i + 2 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'vr'\n", s.name );
				s.volume1 = (float) atof( p.arg[ ++i ] );
				s.volume2 = (float) atof( p.arg[ ++i ] );
				if ( s.volume1 < MIN_VOLUME || s.volume1 > MAX_VOLUME ) p.Error( "sound '%s' volume1 out of range (%f) [must be %.0f..%.0f]\n", s.name, s.volume1, MIN_VOLUME, MAX_VOLUME );
				if ( s.volume2 < MIN_VOLUME || s.volume2 > MAX_VOLUME ) p.Error( "sound '%s' volume2 out of range (%f) [must be %.0f..%.0f]\n", s.name, s.volume2, MIN_VOLUME, MAX_VOLUME );
				if ( s.volume1 > s.volume2 )            p.Error( "sound '%s' volume1 must be less than volume2 (%f, %f)\n", s.name, s.volume1, s.volume2 );
			}
			else if ( stricmp( p.arg[ i ], "prange" ) == 0 )
			{
				if ( i + 2 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'pr'\n", s.name );
				s.pitch1 = (float) atof( p.arg[ ++i ] );
				s.pitch2 = (float) atof( p.arg[ ++i ] );
				if ( s.pitch1 < MIN_PITCH || s.pitch1 > MAX_PITCH ) p.Error( "sound '%s' pitch1 out of range (%f) [must be %.0f..%.0f]\n", s.name, s.pitch1, MIN_PITCH, MAX_PITCH );
				if ( s.pitch2 < MIN_PITCH || s.pitch2 > MAX_PITCH ) p.Error( "sound '%s' pitch2 out of range (%f) [must be %.0f..%.0f]\n", s.name, s.pitch2, MIN_PITCH, MAX_PITCH );
				if ( s.pitch1 > s.pitch2 )            p.Error( "sound '%s' pitch1 must be less than pitch2 (%f, %f)\n", s.name, s.pitch1, s.pitch2 );
			}
			else if ( stricmp( p.arg[ i ], "amb" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'amb'\n", s.name );
				s.ambience = (float) atof( p.arg[ ++i ] );
				if ( s.ambience < 0 || s.ambience > 100 ) p.Error( "sound '%s' ambience out of range (%f)\n", s.name, s.ambience );
			}
			else if ( stricmp( p.arg[ i ], "pres" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'pres'\n", s.name );
				s.presence = (float) atof( p.arg[ ++i ] );
				if ( s.presence < 0 || s.presence > 100 ) p.Error( "sound '%s' presence out of range (%f)\n", s.name, s.presence );
			}
			else if ( stricmp( p.arg[ i ], "wet" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'wet'\n", s.name );
				s.wet = (float) atof( p.arg[ ++i ] );
				if ( s.wet < 0 || s.wet > 100 ) p.Error( "sound '%s' wet out of range (%f)\n", s.name, s.wet );
			}
			else if ( stricmp( p.arg[ i ], "dry" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'dry'\n", s.name );
				s.dry = (float) atof( p.arg[ ++i ] );
				if ( s.dry < 0 || s.dry > 100 ) p.Error( "sound '%s' dry out of range (%f)\n", s.name, s.dry );
			}
			else if ( stricmp( p.arg[ i ], "audibleDist" ) == 0 )
			{
				if ( i + 1 >= p.nArgs ) p.Error( "sound '%s' missing argument after 'd'\n", s.name );
				s.audibleDist = (float) atof( p.arg[ ++i ] );
				if ( s.audibleDist < 0 || s.audibleDist > 255 ) p.Error( "sound '%s' audibleDist out of range (%f) [0..255 valid]\n", s.name, s.audibleDist );
			}
			else if ( stricmp( p.arg[ i ], "2D" ) == 0 )
			{
				src.is2D = true;
			}
			else if ( stricmp( p.arg[ i ], "3D" ) == 0 )
			{
				src.is2D = false;
			}
			else if ( stricmp( p.arg[ i ], "loop" ) == 0 )
			{
				src.looping = true;
			}
			else if ( stricmp( p.arg[ i ], "noloop" ) == 0 )
			{
				src.looping = false;
			}
			else
				p.Error( "sound '%s' has unknown argument '%s'\n", s.name, p.arg[ i ] );
		}

		//-- force looped files to use the AIF file
		if ( src.looping )
		{
			char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ];
			char newFile[ _MAX_PATH ];
			_splitpath( src.name, drive, dir, fname, ext );
			sprintf( newFile, "%s%s%s.aif", drive, dir, fname );
			free( src.name );
			src.name = strdup( newFile );
		}

		//-- and assign the sound to use this source name
		s.sourceName = src.name;

		//-- add source if it's unique
		for ( i = 0 ; i < sourceArr.Count() ; i++ )
			if ( filenamesMatch( sourceArr[ i ].name, src.name ) )
				break;
		if ( i >= sourceArr.Count() )
		{
			struct stat statBuf;
			char fullSourceName[ 256 ];
			sprintf( fullSourceName, "%s%s", sourcePath, src.name );
			if ( stat( fullSourceName, &statBuf ) != 0 )
				p.Error( "source file '%s' not found\n", fullSourceName );
			src.fileChangedTime = statBuf.st_mtime;
			sourceArr.Add( src );
			if ( verboseOutput )
			{
				printf( "new file '%s', time=%d\n", fullSourceName, src.fileChangedTime );
			}
		}
		else
		{
			if ( sourceArr[ i ].looping != src.looping ) p.Error( "sound '%s' src '%s' must always be looping or not-looping, not mixed\n", s.name, src.name );
		}

		//-- add sound
		soundArr.Add( s );
	}
}

//============================================================================

void parseGroupsSection( Parser& p )
{
	int i;

	while ( p.NextLine() && p.arg[0][0] != '[' )
	{
		if ( p.nArgs < 3 )
			p.Error( "group '%s' has too few arguments\n", p.arg[ 0 ] );

		//-- check to see if group already exists
		for ( i = 0 ; i < groupArr.Count() ; i++ )
			if ( stricmp( groupArr[ i ].name, p.arg[ 0 ] ) == 0 )
				break;
		if ( i < groupArr.Count() )
		{
			p.Print( "group '%s' redefined, ignoring.\n", p.arg[ 0 ] );
			continue;
		}

		Group g;
		g.name = strdup( p.arg[ 0 ] );

		if      ( stricmp( p.arg[ 1 ], "random"        ) == 0 ) { g.type = SOUND_GROUP_TYPE_RANDOM; }
		else if ( stricmp( p.arg[ 1 ], "random_unique" ) == 0 ) { g.type = SOUND_GROUP_TYPE_RANDOM_UNIQUE; }
		else if ( stricmp( p.arg[ 1 ], "random_skip"   ) == 0 ) { g.type = SOUND_GROUP_TYPE_RANDOM_SKIP; }
		else
			p.Error( "group '%s' has unknown type '%s'\n", g.name, p.arg[ 1 ] );

		for ( i = 2 ; i < p.nArgs ; i++ )
		{
			//-- find sound in sound list
			for ( int j = 0 ; j < soundArr.Count() ; j++ )
				if ( stricmp( soundArr[ j ].name, p.arg[ i ] ) == 0 )
					break;
			if ( j >= soundArr.Count() )
				p.Error( "group '%s' has undefined sound '%s'\n", g.name, p.arg[ i ] );

			g.soundList.Add( strdup( p.arg[ i ] ) );
		}

		groupArr.Add( g );
	}
}

//============================================================================

void parseTargetSection( Parser& p )
{
	Target t;

	while ( p.NextLine() && p.arg[0][0] != '[' )
	{
		if ( stricmp( p.arg[0], "extension" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for extension\n" );
			char  buf[ 256 ];
			char* bufSrc;
			for ( bufSrc = p.arg[1] ; *bufSrc == '.' ; bufSrc++ ) {}
			sprintf( buf, ".%s", bufSrc );
			t.destExtension = strdup( buf );
		}
		else if ( stricmp( p.arg[0], "dest_path" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for dest_path\n" );
			char  buf[ 256 ];
			char* bufEnd;
			strcpy( buf, p.arg[ 1 ] );
			bufEnd = buf + strlen( buf ) - 1;
			if ( bufEnd >= buf && ( *bufEnd != '/' && *bufEnd != '\\' ) )
				strcat( bufEnd, "\\" );

			t.destPath = strdup( buf );
		}
		else if ( stricmp( p.arg[0], "endian" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for endian\n" );
			t.bigEndian = ( stricmp( p.arg[1], "big" ) == 0 );
		}
		else if ( stricmp( p.arg[0], "max_bank_size" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for max_bank_size\n" );
			t.maxBankSize = atoi( p.arg[1] );
		}
		else if ( stricmp( p.arg[0], "pad" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for pad\n" );
			t.pad = atoi( p.arg[1] );
		}
		else if ( stricmp( p.arg[0], "header_size" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for header_size\n" );
			t.headerSize = atoi( p.arg[1] );
		}
		else if ( stricmp( p.arg[0], "loop_arg" ) == 0 )
		{
			char  buf[ 256 ];
			t.loopArgument = strdup( p.MergeArgs( buf, 1, p.nArgs - 1 ) );
		}
		else if ( stricmp( p.arg[0], "noloop_arg" ) == 0 )
		{
			char  buf[ 256 ];
			t.noloopArgument = strdup( p.MergeArgs( buf, 1, p.nArgs - 1 ) );
		}
		else if ( stricmp( p.arg[0], "build" ) == 0 )
		{
			char  buf[ 256 ];
			p.MergeArgs( buf, 1, p.nArgs - 1 );
			if ( strlen( buf ) > 0 )
				t.buildCommands.Add( strdup( buf ) );
		}
		else if ( stricmp( p.arg[0], "external" ) == 0 )
		{
			if ( p.nArgs != 2 ) p.Error( "incorrect number of arguments for external\n" );
			t.external = strdup( p.arg[ 1 ] );
		}
		else
		{
			p.Error( "unknown target command '%s'\n", p.arg[ 0 ] );
		}
	}

	if ( t.destPath == NULL )           p.Error( "target missing dest_path\n"  );
	if ( t.destExtension == NULL )      p.Error( "target missing extension\n"  );
	if ( t.loopArgument == NULL )       p.Error( "target missing loop_arg\n"   );
	if ( t.noloopArgument == NULL )     p.Error( "target missing noloop_arg\n" );
	if ( t.buildCommands.Count() == 0 ) p.Error( "target missing build\n"      );

	targetArr.Add( t );
}

//============================================================================

void parseScript( char* filename )
{
	Parser p( filename );

	//-- this is a source file to the package, so we need to remember the newest time
	//-- for dependency checking later on.
	struct stat statBuf;
	if ( stat( filename, &statBuf ) != 0 )
		p.Error( "couldn't stat '%s' for filetime\n", filename );
	else if ( statBuf.st_mtime > newestModuleFileTime )
		newestModuleFileTime = statBuf.st_mtime;

	while ( !p.End() )
	{
		if ( p.nArgs > 1 || p.arg[ 0 ][ 0 ] != '[' )
			p.Error( "no section defined yet\n" );

		if      ( stricmp( p.arg[ 0 ], "[modules]" ) == 0 )  { parseModulesSection( p ); }
		else if ( stricmp( p.arg[ 0 ], "[sounds]"  ) == 0 )  { parseSoundsSection( p );  }
		else if ( stricmp( p.arg[ 0 ], "[groups]"  ) == 0 )  { parseGroupsSection( p );  }
		else if ( stricmp( p.arg[ 0 ], "[target]"  ) == 0 )  { parseTargetSection( p );  }
		else
			p.Error( "unknown section '%s'\n", p.arg[ 0 ] );
	}
}

//============================================================================

void makePath( char* path )
{
	char*	p;
	char	save;

	for ( p = path ; *p != 0 ; p++ )
	{
		if ( *p == '\\' || *p == '/' )
		{
			save = *p;
			*p = 0;
			mkdir( path );
			*p = save;
		}
	}
}

//============================================================================

void loadSourceFiles()
{
	int i;

	for ( i = 0 ; i < sourceArr.Count() ; i++ )
	{
		FILE* fp = fopen( sourceArr[ i ].targetName, "rb" );
		if ( fp == NULL )
		{
			fprintf( stderr, "ERROR: can't open input '%s'\n", sourceArr[ i ].targetName );
			exit( 1 );
		}

		//-- get file size
		fseek( fp, 0, SEEK_END );
		sourceArr[ i ].targetSize = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		//-- alloc buffer
		sourceArr[ i ].targetData = (char*) malloc( sourceArr[ i ].targetSize );
		if ( sourceArr[ i ].targetData == NULL )
		{
			fprintf( stderr, "ERROR: couldn't alloc %d bytes for file '%s'\n", sourceArr[ i ].targetSize, sourceArr[ i ].targetName );
			exit( 1 );
		}

		//-- load the data
		int bytesRead = fread( sourceArr[ i ].targetData, 1, sourceArr[ i ].targetSize, fp );
		if ( bytesRead != sourceArr[ i ].targetSize )
		{
			fprintf( stderr, "ERROR: couldn't read %d bytes from file '%s' (got %d)\n", sourceArr[ i ].targetSize, sourceArr[ i ].targetName, bytesRead );
			exit( 1 );
		}

		fclose( fp );

		if ( verboseOutput )
			printf( "..loaded %s (%d bytes)\n", sourceArr[ i ].targetName, sourceArr[ i ].targetSize );
	}
}

//============================================================================

bool buildAttribIsDifferent( char* attribFilename, char* attrString )
{
	FILE* fp;

	if ( ( fp = fopen( attribFilename, "rb" ) ) != NULL )
	{
		char buf[ 256 ];
		int len;
		fseek( fp, 0, SEEK_END );
		len = ftell( fp );
		fseek( fp, 0, SEEK_SET );
		fread( buf, 1, len, fp );
		buf[ len ] = 0;
		fclose( fp );

		if ( strcmp( buf, attrString ) == 0 )
			return false;
	}

	return true;
}

//============================================================================

void buildAttribWrite( char* attribFilename, char* attrString )
{
	FILE* fp;
	if ( ( fp = fopen( attribFilename, "wb" ) ) != NULL )
	{
		fwrite( attrString, 1, strlen( attrString ), fp );
		fclose( fp );
	}
}

//============================================================================

// the standard "system" call will sometimes fail because it (internally)
// unquotes all the arguments before executing the command line.  this is true
// also of spawn* fuctions.  The best solution is to use CreateProcess directly.

int mySystem( char* command )
{
   STARTUPINFO         StartupInfo;
   PROCESS_INFORMATION ProcessInfo;
   memset( &StartupInfo, 0, sizeof(StartupInfo) );
   memset( &ProcessInfo, 0, sizeof(ProcessInfo) );
   StartupInfo.cb = sizeof( StartupInfo );

   int status = CreateProcess( NULL, command, NULL, NULL, true, 0, NULL, NULL, &StartupInfo, &ProcessInfo );
   if ( status != 0 )
   {
		DWORD retVal = 0;
      WaitForSingleObject( ProcessInfo.hProcess, (DWORD)( -1L ) );
      GetExitCodeProcess( ProcessInfo.hProcess, &retVal );
      CloseHandle( ProcessInfo.hProcess );
      CloseHandle( ProcessInfo.hThread );
		return retVal;
   }
	else
	{
		fprintf( stderr, "ERROR executing \"%s\"", command );

		LPVOID lpMsgBuf;
		if ( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL,
									GetLastError(),
									MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
									(LPTSTR) &lpMsgBuf,
									0,
									NULL ) )
		{
			fprintf( stderr, " - %s", lpMsgBuf );
			LocalFree( lpMsgBuf );
		}

		fprintf( stderr, "\n" );
		return -1;
	}
}

//============================================================================

void buildTarget( Target& t )
{
	char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ];
	char target[ _MAX_PATH ];
	char targetAttrib[ _MAX_PATH ];
	int i;
	int newestDestFileTime = 0;
	int bankFileTime = 0;
	bool  buildBankFile = false;

	//-- build all the sources
	for ( i = 0 ; i < sourceArr.Count() ; i++ )
	{
		_splitpath( sourceArr[ i ].name, drive, dir, fname, ext );
		sprintf( target, "%s%s%s%s", t.destPath, dir, fname, t.destExtension );
		sprintf( targetAttrib, "%s%s%s.attr", t.destPath, dir, fname );

		int destFileTime = 0;
		struct stat statBuf;
		if ( stat( target, &statBuf ) != 0 )
		{
			//-- file doesn't exist or path doesn't exist.  make the path just to be sure
			if ( !doNothing )
				makePath( target );

			destFileTime = 0;
		}
		else
		{
			destFileTime = statBuf.st_mtime;
		}

		if ( destFileTime > newestDestFileTime )
			newestDestFileTime = destFileTime;

		//-- check to see if the attr changed
		char attrString[ 256 ];
		sprintf( attrString, "%s\n", sourceArr[ i ].looping ? "loop" : "noloop" );
		bool attrChanged = buildAttribIsDifferent( targetAttrib, attrString );

		if ( verboseOutput )
		{
			printf( "target '%s', time=%d attrChanged=%d\n", target, destFileTime, attrChanged );
		}

		if ( forceBuild || attrChanged || sourceArr[i].fileChangedTime > destFileTime )
		{
			//-- build
			for ( int j = 0 ; j < t.buildCommands.Count() ; j++ )
			{
				char buf[ 512 ];
				char fullSourceName[ 256 ];
				sprintf( fullSourceName, "%s%s", sourcePath, sourceArr[ i ].name );
				expandLine( t.buildCommands[ j ], buf, fullSourceName, target, sourceArr[ i ].looping ? t.loopArgument : t.noloopArgument );
				printf( "%s\n", buf );
				if ( !doNothing )
				{
					if ( mySystem( buf ) != 0 )
					{
						fprintf( stderr, "ERROR returned by '%s', aborting\n", buf );
						exit( 1 );
					}
				}
			}
			if ( !doNothing )
				buildAttribWrite( targetAttrib, attrString );
			buildBankFile = true;
		}

		sourceArr[ i ].targetName = strdup( target );
	}

	//-- test the bank file time/date
	sprintf( target, "%s%s", t.destPath, bankFilename );
	struct stat statBuf;
	if ( stat( target, &statBuf ) != 0 )
	{
		//-- file doesn't exist or path doesn't exist.  make the path just to be sure
		if ( !doNothing )
			makePath( target );
		bankFileTime = 0;
	}
	else
	{
		bankFileTime = statBuf.st_mtime;
	}

	//-- build the bank file if we should
	if ( ( forceBuild ) || ( buildBankFile ) || ( newestDestFileTime > bankFileTime ) || ( newestModuleFileTime > bankFileTime ) )
	{
		if ( !doNothing )
		{
			printf( "Creating bank file '%s'\n", target );
			loadSourceFiles();
			exportBankFile( target, t, sourceArr, soundArr, groupArr );
		}
		else
		{
			printf( "building '%s' (DEACTIVATED WITH -n FLAG)\n", target );
		}
	}

	//-- free up the target file names for another target
	for ( i = 0 ; i < sourceArr.Count() ; i++ )
	{
		if ( sourceArr[ i ].targetName != NULL ) { free( sourceArr[ i ].targetName ); sourceArr[ i ].targetName = NULL; }
		if ( sourceArr[ i ].targetData != NULL ) { free( sourceArr[ i ].targetData ); sourceArr[ i ].targetData = NULL; }
		sourceArr[ i ].targetSize = 0;
	}
}

//============================================================================

void usage( void )
{
	fprintf( stderr, "USAGE: soundmod [options] [soundmod.cfg] [script.mod]\n" );
	fprintf( stderr, "  [OPTIONS]\n" );
	fprintf( stderr, "    -b bankFile    set the output filename of the bankfile\n" );
	fprintf( stderr, "    -d             output DBL format\n" );
	fprintf( stderr, "    -s sourcePath  specify the source path for all WAV files\n" );
	fprintf( stderr, "    -f             force all files build regardless of time/date\n" );
	fprintf( stderr, "    -n             do nothing, but display what would be done\n" );
	fprintf( stderr, "    -v             enable verbose output\n" );
	exit( 1 );
}

//============================================================================

int main( int argc, char** argv )
{
	int	i;
	int	realArgs = 0;
	char*	scriptFilename = NULL;
	char* configFilename = NULL;

	//-- parse arguments
	for ( i = 1 ; i < argc ; i++ )
	{
		if ( argv[ i ][ 0 ] == '-' )
		{
			switch ( tolower( argv[ i ][ 1 ] ) )
			{
				case 'f':	forceBuild = true;	break;
				case 'd':	exportDBL = true;		break;
				case 'n':	doNothing = true;		break;
				case 'v':	verboseOutput++;		break;
				case 's':
					if ( i + 1 >= argc ) { fprintf( stderr, "ERROR: not enough parameters for -s\n" ); usage(); break; }
					i++;
					{
						int len = strlen( argv[ i ] );
						sourcePath = (char*) malloc( len + 2 );
						strcpy( sourcePath, argv[ i ] );
						if ( sourcePath[ len - 1 ] != '/' && sourcePath[ len - 1 ] != '\\' )
							strcat( sourcePath, "\\" );
					}
					break;
				case 'b':
					if ( i + 1 >= argc ) { fprintf( stderr, "ERROR: not enough parameters for -b\n" ); usage(); break; }
					bankFilename = argv[ ++i ];
					break;
				default:
					fprintf( stderr, "ERROR: unknown parameter: '%s'\n", argv[ i ] );	usage();	break;
			}
		}
		else
		{
			switch ( realArgs++ )
			{
				case 0:
					configFilename = argv[ i ];
					break;
				case 1:
					scriptFilename = argv[ i ];
					break;
				default:
					fprintf( stderr, "ERROR: too many arguments specified\n" );
					usage();
			}
		}
	}

	if ( realArgs != 2 )
	{
		fprintf( stderr, "ERROR: too few parameters\n" );
		usage();
	}

	//-- if we don't have a bank filename, make one up from the source script name
	if ( bankFilename == NULL )
	{
		char drive[ _MAX_DRIVE ], dir[ _MAX_DIR ], fname[ _MAX_FNAME ], ext[ _MAX_EXT ];
		char temp[ _MAX_PATH ];
		_splitpath( scriptFilename, drive, dir, fname, ext );
		sprintf( temp, "%s.bnk", fname );
		bankFilename = strdup( temp );
	}

	//-- read config
	parseScript( configFilename );
	if ( targetArr.Count() == 0 )
	{
		fprintf( stderr, "ERROR: config '%s' doesn't have any target sections\n" );
		exit( 1 );
	}

	//-- action
	parseScript( scriptFilename );

	//-- sort the sounds and groups, so we can binary search them in the engine
	soundArr.Sort();
	groupArr.Sort();

	if ( verboseOutput )
	{
		for ( i = 0 ; i < soundArr.Count() ; i++ )
			printf( "SOUND: %s src=%s v=[%.1f,%.1f] p=[%.1f,%.1f]\n", soundArr[i].name, soundArr[i].sourceName, soundArr[i].volume1, soundArr[i].volume2, soundArr[i].pitch1, soundArr[i].pitch2 );
		for ( i = 0 ; i < groupArr.Count() ; i++ )
			printf( "GROUP: %s type=%d count=%d\n", groupArr[i].name, groupArr[i].type, groupArr[i].soundList.Count() );
		for ( i = 0 ; i < targetArr.Count() ; i++ )
		{
			printf( "TARGET:\n" );
			printf( "  destPath:       %s\n", targetArr[ i ].destPath );
			printf( "  destExtension:  %s\n", targetArr[ i ].destExtension );
			printf( "  loopArgument:   %s\n", targetArr[ i ].loopArgument );
			printf( "  noloopArgument: %s\n", targetArr[ i ].noloopArgument );
			printf( "  bigEndian:      %d\n", targetArr[ i ].bigEndian );
			printf( "  maxBankSize:    %d\n", targetArr[ i ].maxBankSize );
			printf( "  pad:            %d\n", targetArr[ i ].pad );
			if ( targetArr[ i ].external )
				printf( "  external:			%s\n", targetArr[ i ].external );
			for ( int j = 0 ; j < targetArr[ i ].buildCommands.Count() ; j++ )
				printf( "  build:          %s\n", targetArr[ i ].buildCommands[ j ] );
		}
	}

	//-- include the exe itself as a "source" for dependency checking
	struct stat statBuf;
	if ( stat( argv[ 0 ], &statBuf ) == 0 && statBuf.st_mtime > newestModuleFileTime )
		newestModuleFileTime = statBuf.st_mtime;

	//-- build the targets
	for ( i = 0 ; i < targetArr.Count() ; i++ )
		buildTarget( targetArr[ i ] );

	return( 0 );
}

//============================================================================
