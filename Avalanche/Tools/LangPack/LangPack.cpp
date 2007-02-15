//============================================================================
//=
//= LangPack.cpp - build a binary version of the lang dictionary file.
//= Rob Nelson, Avalanche Software, 3/2005
//=
//============================================================================

#define ASSERT(x) ((void)0)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Container/DArray.h"
#include "EngineHelper/Hash.h"
#include "../../GameLib/GameHelper/TranslatorDict.h"

//============================================================================

#define DEFAULT_GROW	30
#define DEFAULT_ITERATIONS 500

#define ISEOL(p) ( (p) == '\n' || (p) == '\r' )

//============================================================================

class DictHash
{
public:
	DictHash( int size, unsigned int seed );
	~DictHash();
	bool Add( const char* key, const char* text );
	float MissAve();
	int	MissWorst();
	float	MissStandardDeviation();
	float	MissOneShots();
	float	Cost();
	void	Export( const char* filename );
	unsigned int Seed() { return d_seed; }

private:
	struct Entry
	{
		unsigned int	hash;
		const char*		text;
	};
	Entry*		 d_table;
	int			 d_size;
	unsigned int d_seed;
	int			 d_missCount;
	int			 d_missTotal;
	int			 d_missTotalSquared;
	int			 d_missWorst;
	int			 d_missOneShots;
};

//============================================================================

class Dict
{
public:
	static Dict* Load( const char* filename );
	void AddEntry( const char* key, const char* text );
	void Sort();
	void Dump();
	DictHash*	MakeHash( int grow, int iterations, unsigned int firstSeed, bool verbose );

private:
	static int Dict::CmpLength( const void* A, const void* B );
	struct Entry
	{
		const char* key;
		const char* text;
	};
	DynamicArray<Entry> d_dict;
};

//============================================================================

Dict* Dict::Load( const char* filename )
{
	FILE* fp = fopen( filename, "rb" );
	if ( !fp )
	{
		fprintf( stderr, "ERROR: couldn't open %s for read\n", filename );
		exit( 1 );
	}

	fseek( fp, 0, SEEK_END );
	int length = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	char* buf = new char[ length + 1 ];
	buf[ length ] = 0;
	if ( fread( buf, 1, length, fp ) != length )
	{
		fprintf( stderr, "ERROR: couldn't read all of %s\n", filename );
		exit( 1 );
	}
	fclose( fp );

	// parse into entries
	Dict* dict = new Dict();
	const char* key = 0;
	const char* text = 0;
	for ( char* p = buf ; *p ; )
	{
		if ( *p == '#' )	// key
		{
			// get key, kill newline at end
			key = ++p;
			while ( *p && !ISEOL(*p) ) { p++; }
			while ( ISEOL(*p) )             { *p++ = 0; }

			// grab the text
			text = p;
			char* end = p;
			while ( *p && *p != '#' )
			{
				while ( *p && !ISEOL(*p) )  { p++; }
				end = p;
				while ( ISEOL(*p) )         { p++; }
			}
			*end = 0;

			dict->AddEntry( key, text );
		}
		else
		{
			p++;
		}
	}

	delete buf;

	return dict;
}

//============================================================================

void Dict::AddEntry( const char* key, const char* text )
{
	if ( key && *key && text && *text )
	{
		Entry ent;
		ent.key = strdup( key );
		ent.text = strdup( text );
		d_dict.Add( ent );
	}
}

//============================================================================

void Dict::Dump()
{
	for ( int i = 0 ; i < d_dict.Count() ; i++ )
		printf( "ENTRY '%s' : '%s'\n", d_dict[i].key, d_dict[i].text );
}

//============================================================================

int Dict::CmpLength( const void* A, const void* B )
{
	Entry* a = (Entry*) A;
	Entry* b = (Entry*) B;
	return int( strlen( a->key ) - strlen( b->key ) );
}

//============================================================================

void Dict::Sort()
{
	// sort by length, since longer keys are likely to be looked up less.
	qsort( d_dict.GetPtr(), d_dict.Count(), sizeof(Entry), Dict::CmpLength );
}

//============================================================================

DictHash* Dict::MakeHash( int grow, int iterations, unsigned int firstSeed, bool verbose )
{
	int size = ( d_dict.Count() + 1 ) * ( 100 + grow ) / 100;
	DictHash* bestHash = NULL;

	unsigned int seed = firstSeed;

	for ( int tries = 0 ; tries < iterations ; tries++ )
	{
		DictHash* hash = new DictHash( size, seed );

		int i;
		for ( i = 0 ; i < d_dict.Count() ; i++ )
			if ( !hash->Add( d_dict[i].key, d_dict[i].text ) )
				break;

		if ( i >= d_dict.Count() && ( !bestHash || ( hash->Cost() < bestHash->Cost() ) ) )
		{
			if ( verbose )
				printf( "hash: seed=%08X cost=%7.2f avemiss=%.1f sd=%.1f worst=%d\n", seed, hash->Cost(), hash->MissAve(), hash->MissStandardDeviation(), hash->MissWorst() );
			delete bestHash;
			bestHash = hash;
		}
		else
		{
			delete hash;
		}

		seed = ( rand() << 16 ) ^ rand();
	}
	return bestHash;
}

//============================================================================

DictHash::DictHash( int size, unsigned int seed )
{
	d_size = size;
	d_seed = seed;
	d_table = new Entry[ d_size ];
	memset( d_table, 0, sizeof(Entry) * d_size );

	// stats
	d_missTotal = 0;
	d_missWorst = 0;
	d_missCount = 0;
	d_missTotalSquared = 0;
	d_missOneShots = 0;
}

//============================================================================

DictHash::~DictHash()
{
	delete d_table;
}

//============================================================================

bool DictHash::Add( const char* key, const char* text )
{
	int miss = 1;
	unsigned int hash = Hash::CalcChecksum( key, (int)strlen(key), d_seed );
	for ( int i = hash % d_size ; d_table[i].text ; i = (i + 1) % d_size, miss++ )
		if ( d_table[i].hash == hash )
			return false;
	d_table[i].hash = hash;
	d_table[i].text = text;
	d_missTotal += miss;
	d_missCount += 1;
	d_missTotalSquared += miss * miss;
	if ( miss > d_missWorst ) d_missWorst = miss;
	if ( miss == 1 ) d_missOneShots++;
	return true;
}

//============================================================================

float DictHash::MissAve()
{
	return (float)d_missTotal / (float)d_missCount;
}

//============================================================================

int DictHash::MissWorst()
{
	return d_missWorst;
}

//============================================================================

float DictHash::MissStandardDeviation()
{
	float ave = MissAve();
	return sqrt( ( d_missTotalSquared / d_missCount ) - ( ave * ave ) );
}

//============================================================================

float DictHash::MissOneShots()
{
	return (float)d_missOneShots / (float)d_missCount;
}

//============================================================================

float DictHash::Cost()
{
	return ( MissAve() + 1.7f * MissStandardDeviation() ) * 8 + MissWorst();
}

//============================================================================

void DictHash::Export( const char* filename )
{
	int i;

	// how big are we
	int headerSize = sizeof( CTranslatorDict );
	int tableSize  = d_size * sizeof( CTranslatorDict::HashEntry );
	int textSize   = 0;
	for ( i = 0 ; i < d_size ; i++ )
		if ( d_table[i].text )
			textSize += strlen( d_table[i].text ) + 1;
	int fileSize   = headerSize + tableSize + textSize;

	char* buf = new char[ fileSize ];
	memset( buf, 0, fileSize );

	CTranslatorDict* dict = (CTranslatorDict*) buf;
	dict->d_id            = DICT_ID;
	dict->d_version       = DICT_VERSION;
	dict->d_seed          = d_seed;
	dict->d_hashTable     = (CTranslatorDict::HashEntry*) ( buf + headerSize );
	dict->d_hashTableSize = d_size;

	char* strings = (char*) ( (int)dict->d_hashTable + tableSize );

	for ( i = 0 ; i < d_size ; i++ )
	{
		if ( d_table[i].text )
		{
			dict->d_hashTable[ i ].hash = d_table[i].hash;
			dict->d_hashTable[ i ].text = strings;
			strcpy( strings, d_table[i].text );
			strings += strlen( d_table[i].text ) + 1;
		}
		else
		{
			dict->d_hashTable[ i ].hash = 0;
			dict->d_hashTable[ i ].text = 0;
		}
	}

	if ( strings != buf + fileSize )
	{
		fprintf( stderr, "INTERNAL ERROR: string table size not as expected\n" );
		exit( 1 );
	}

	// write it out
	dict->Deinit();
	FILE* fp = fopen( filename, "wb" );
	if ( !fp )
	{
		fprintf( stderr, "ERROR: couldn't open %s for write\n", filename );
		exit( 1 );
	}
	fwrite( buf, 1, fileSize, fp );
	fclose( fp );

	delete[] buf;
}

//============================================================================

void usage( const char* message )
{
	if ( message && message[0] )
		fprintf( stderr, "%s", message );
	fprintf( stderr, "USAGE: langpack [options] input output\n" );
	fprintf( stderr, "  OPTIONS:\n" );
	fprintf( stderr, "    -g pct  -- hash table growth percentage (default %d)\n", DEFAULT_GROW );
	fprintf( stderr, "    -i iter -- number of iterations to optimize seed (default %d)\n", DEFAULT_ITERATIONS );
	fprintf( stderr, "    -d      -- dump compiled file\n" );
	fprintf( stderr, "    -v      -- verbose\n" );
	fprintf( stderr, "    -s seed -- specify seed (in hex)\n" );
	exit( 1 );
}

//============================================================================

char* LoadFile( const char* filename, int* o_length )
{
	FILE* fp = fopen( filename, "rb" );
	if ( !fp )
	{
		fprintf( stderr, "ERROR: couldn't open %s for read\n", filename );
		exit( 1 );
	}
	fseek( fp, 0, SEEK_END );
	size_t len = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	char* buf = new char[ len ];
	if ( fread( buf, 1, len, fp ) != len )
	{
		fprintf( stderr, "ERROR: couldn't read from %s\n", filename );
		exit( 1 );
	}
	fclose( fp );

	if ( o_length ) *o_length = len;
	return buf;
}

//============================================================================

int main( int argc, char** argv )
{
	int parms = 0;
	const char* input  = NULL;
	const char* output = NULL;
	int grow = DEFAULT_GROW;
	int iterations = DEFAULT_ITERATIONS;
	bool dump = false;
	bool verbose = false;
	unsigned int firstSeed = 0;

	// parse command line
	for ( int i = 1 ; i < argc ; i++ )
	{
		if ( argv[i][0] == '-' )
		{
			switch ( tolower( argv[i][1] ) )
			{
				case 'g':
					if ( i + 1 >= argc ) { usage( "ERROR: -g missing parameter\n" ); }
					grow = atoi( argv[ ++i ] );
					break;
				case 'i':
					if ( i + 1 >= argc ) { usage( "ERROR: -i missing parameter\n" ); }
					iterations = atoi( argv[ ++i ] );
					break;
				case 's':
					if ( i + 1 >= argc ) { usage( "ERROR: -s missing parameter\n" ); }
					sscanf( argv[ ++i ], "%x", &firstSeed );
					break;
				case 'd':
					dump = true;
					break;
				case 'v':
					verbose = true;
					break;
				default:
					fprintf( stderr, "ERROR: unknown option '%s'\n", argv[i][0] );
					usage( "" );
					break;
			}
		}
		else
		{
			switch( parms++ )
			{
				case 0:	input = argv[i];	break;
				case 1:	output = argv[i];	break;
				case 2:	usage( "ERROR: Too many parameters\n" );
			}
		}
	}
	if ( ( dump && parms != 1 ) || ( !dump && parms != 2 ) )
		usage( "ERROR: not enough parameters\n" );

	if ( dump )
	{
		CTranslatorDict* dict = (CTranslatorDict*) LoadFile( input, NULL );
		if ( !dict->Init() )
		{
			fprintf( stderr, "ERROR: %s is not a valid dict file\n", dict );
			exit( 1 );
		}
		dict->Dump();
		exit( 0 );
	}

	Dict* dict = Dict::Load( input );
	dict->Sort();
//	dict->Dump();

	DictHash* hash = dict->MakeHash( grow, iterations, firstSeed, verbose );
	if ( !hash )
	{
		fprintf( stderr, "ERROR: internal error, unable to create hash\n" );
		exit( 1 );
	}

	printf( "Hash Created: Seed=%08X, AveMiss=%.1f SD=%.1f Worst=%d OneShots=%3.0f%%\n", hash->Seed(), hash->MissAve(), hash->MissStandardDeviation(), hash->MissWorst(), hash->MissOneShots() * 100 );
	hash->Export( output );

	return 0;
}

//============================================================================
