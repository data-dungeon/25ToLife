//============================================================================
//=
//= ActorParm.cpp - Used to convert heavy object attribute (.OA) files from
//=                 the filterchain into leaner actor parameter (.AP) files that
//=                 the game uses to create actors.
//=
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// hacky stuff so we can use DArray.h
#define WIN32_TOOL_DEBUG
#define DISABLE_ASSERTS

#include "Container/DArray.h"
#include "Tupperware/memorymanager.h"
#include "Tupperware/aggregate.h"
#include "Tupperware/loadsave.h"
#include "Tupperware/pool.h"
#include "Tupperattrib/AccessRoot.h"
#include "Tupperattrib/AccessInstance.h"
#include "Tupperattrib/AccessPath.h"
#include "Tupperattrib/ConfigRoot.h"
#include "Tupperattrib/KeyValue.h"
#include "Tupperattrib/KeyValueContext.h"

//============================================================================

#define BEGIN_BLOCK	"BEGIN OBJECTATTRIBUTES"
#define END_BLOCK    "END OBJECTATTRIBUTES"

//============================================================================

class CExclusions
{
public:
	void AddKey( const char* key )
	{
		if ( strlen( key ) )
		{
			d_exclusionList.Add( strdup( key ) );
		}
	}

	void AddKeysFromFile( const char* filename )
	{
		FILE* fp = fopen( filename, "rt" );
		char buf[ 256 ];
		if ( !fp ) { fprintf( stderr, "ERROR: couldn't open exlusion file '%s'\n", filename ); exit( 1 ); }
		while ( fgets( buf, sizeof( buf ), fp ) )
		{
			// kill comments and strip white space from end of line
			char* p;
			if ( ( p = strstr( buf, "//" ) ) != NULL ) { *p = 0; }
			if ( ( p = strstr( buf, ";"  ) ) != NULL ) { *p = 0; }
			for ( p = buf + strlen( buf ) - 1 ; p > buf && isspace( *p ) ; )
				*p-- = 0;

			// find start of key without leading space
			for ( p = buf ; isspace( *p ) ; )
				p++;

			AddKey( p );
		}
		fclose( fp );
	}

	bool IsExcluded( const char* key )
	{
		for ( int i = 0 ; i < d_exclusionList.Count() ; i++ )
			if ( stricmp( d_exclusionList[ i ], key ) == 0 )
				return true;
		return false;
	}

	void Dump()
	{
		for ( int i = 0 ; i < d_exclusionList.Count() ; i++ )
		{
			printf( "exclusion '%s'\n", d_exclusionList[ i ] );
		}
	}

private:
	DynamicArray<char*> d_exclusionList;
};

CExclusions exclusions;

//============================================================================

class CStringList
{
public:
	CStringList()
	{
		d_refs = 0;
		d_byteSize = 0;
		d_orgByteSize = 0;
	}

	int Add( const char* string )
	{
		d_refs++;
		d_orgByteSize += strlen( string ) + 1;
		for ( int i = 0 ; i < d_strings.Count() ; i++ )
			if ( stricmp( d_strings[ i ], string ) == 0 )
				return i;
		d_strings.Add( strdup( string ) );
		d_byteSize += strlen( string ) + 1;
		return( d_strings.Count() - 1 );
	}

	void Dump()
	{
		printf( "TOTAL STRINGS: %d\n", d_strings.Count() );
		printf( "  byte count : %d\n", d_byteSize );
		printf( "  org bytes  : %d\n", d_orgByteSize );
		printf( "  refs       : %d\n", d_refs );
	}

private:
	int                 d_refs;
	int					  d_byteSize;
	int					  d_orgByteSize;
	DynamicArray<char*> d_strings;
};

CStringList strings;

//============================================================================

// hacky stuff so we can use darray.h
void memSetOneShotNewName( char* x )
{
}

//============================================================================

char* ReadFile( const char* filename, int* len )
{
	FILE* fp;

	// load file
	fp = fopen( filename, "rb" );
	if ( fp != NULL )
	{
		fseek( fp, 0, SEEK_END );
		int fileLen = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		if ( fileLen )
		{
			char* fileBuf = new char[ fileLen ];
			if ( fread( fileBuf, 1, fileLen, fp ) == fileLen )
			{
				fclose( fp );
				*len = fileLen;
				return fileBuf;
			}
			delete fileBuf;
		}

		fclose( fp );
	}

	*len = 0;
	return NULL;
}

//============================================================================

bool IsNumber( const char* string, float& value )
{
	const char* p;
	for ( p = string ; *p ; p++ )
		if ( !isdigit( *p ) && *p != '-' && *p != '.' && *p != 'e' )
			return false;
	if ( p == string )
		return false;

	sscanf( string, "%f", &value );
	return true;
}

//============================================================================

void DoFile( const char* inFilename, const char* outFilename )
{
	int filelen;

	char* buf = ReadFile( inFilename, &filelen );

	FILE* out = fopen( outFilename, "wb" );
	if ( !out )
	{
		fprintf( stderr, "ERROR: couldn't open '%s' for write\n", outFilename );
		exit( 1 );
	}

	if ( buf )
	{
		TupperwareAggregate* d_pTupperwareDatabase = NULL;
		if ( TupperwareLoadSaveManager::Read( buf, filelen, &d_pTupperwareDatabase ) )
		{
			TupAccessRoot tupAccessRoot( d_pTupperwareDatabase );
			TupperwareAggregate* pNodeAttributeTreeAgg = tupAccessRoot.GetNodeAttributeTree();
			if ( pNodeAttributeTreeAgg )
			{
				TupConfigRoot d_tupConfigRoot;
				d_tupConfigRoot.BuildConfigTree( pNodeAttributeTreeAgg );

				TupperwareAggregate* pInstanceAttributesAgg = tupAccessRoot.GetInstanceAttributes();
				if ( pInstanceAttributesAgg )
				{
					TupAccessInstancePool d_tupAccessInstancePool( pInstanceAttributesAgg );

					int numInstances = d_tupAccessInstancePool.GetNumInstances();
					for ( int instanceIndex = 0 ; instanceIndex < numInstances ; instanceIndex++ )
					{
						// get the attributes from the tup
						TupperwareAggregate* tupInstance = d_tupAccessInstancePool.GetInstance( instanceIndex );
						TupAccessInstance tupAccessInstance( tupInstance );
						char *pInstanceAttributesString;
						tupAccessInstance.GetAttributes( &pInstanceAttributesString );
						TupKeyValue tupKeyValue( pInstanceAttributesString, BEGIN_BLOCK, END_BLOCK );
						TupKeyValueContext tupKeyValueContext;
						d_tupConfigRoot.GetKeyValueContexts( tupKeyValue, tupKeyValueContext, true );

						// print name
						char* instanceName;
						tupAccessInstance.GetName( &instanceName );
						fprintf( out, "[%s]\n", instanceName );
//						strings.Add( "instance_name" );
//						strings.Add( instanceName );
						
						// dump parameters
						int numAttribs = tupKeyValueContext.GetNumAttribs();
						for ( int i = 0 ; i < numAttribs ; i++ )
						{
							const char* key   = tupKeyValueContext.GetKeyAsString( i );
							const char* value = tupKeyValueContext.GetValueAsString( i );

							if ( strlen( value ) > 0 )
							{
								if ( ! exclusions.IsExcluded( key ) )
								{
									float fValue;
									if ( IsNumber( value, fValue ) )
									{
										char num[ 32 ], *p;
										sprintf( num, "%.7f", fValue );
										for ( p = num + strlen( num ) - 1 ; p >= num && *p == '0' ; )
											*p-- = 0;
										if ( p >= num && *p == '.' )
											*p-- = 0;

										fprintf( out, "%s=%s\n", key, num );
									}
									else
									{
										fprintf( out, "%s=%s\n", key, value );
									}

//									strings.Add( key );
//									strings.Add( value );
								}
							}
						}
						fprintf( out, "\n" );
					}
				}
			}
		}

		delete buf;
	}

	fclose( out );
}

//============================================================================

void usage()
{
	fprintf( stderr, "USAGE: ActorParm [options] infile outfile\n" );
	fprintf( stderr, "  [options]\n" );
	fprintf( stderr, "    -xf excludeFile - name a file with a list of exluded keys\n" );
	fprintf( stderr, "    -x  excludeKey  - exclude a single key (-x can be used multiple times)\n" );
	exit( 1 );
}

//============================================================================

int main( int argc, char** argv )
{
	int realargs = 0;
	char* inFilename = NULL;
	char* outFilename = NULL;
	int i;

	for ( i = 1 ; i < argc ; i++ )
	{
		if ( !stricmp( argv[ i ], "-xf" ) )
		{
			if ( i + 1 >= argc ) { fprintf( stderr, "ERROR: -xf missing parameter\n" ); usage(); }
			exclusions.AddKeysFromFile( argv[ ++i ] );
		}
		else if ( !stricmp( argv[ i ], "-x" ) )
		{
			if ( i + 1 >= argc ) { fprintf( stderr, "ERROR: -x missing parameter\n" ); usage(); }
			exclusions.AddKey( argv[ ++i ] );
		}
		else
		{
			switch ( realargs++ )
			{
				case 0:	inFilename = argv[ i ];	break;
				case 1:	outFilename = argv[ i ]; break;
				default:	fprintf( stderr, "ERROR: too many arguments\n" ); usage();
			}
		}
	}

	if ( realargs < 2 )
	{
		fprintf( stderr, "ERROR: not enough arguments\n" );
		usage();
	}

	DoFile( inFilename, outFilename );

//	strings.Dump();
}

//============================================================================
