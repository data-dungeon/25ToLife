//============================================================================
//
// slang.cpp - stand alone compiler for scripts, and test app
//
//============================================================================

#include "Script/ScriptPCH.h"
#include <math.h>

//============================================================================

void usage()
{
	fprintf( stderr, "USAGE:\n" );
	fprintf( stderr, "  slang [options] -s script.sla  -- compile from a script\n" );
	fprintf( stderr, "  slang [options] -e \"commands\"  -- compile from commands on command line\n" );
	fprintf( stderr, "  slang [options] -x input.sx    -- load script executable\n" );
	fprintf( stderr, "[OPTIONS]\n" );
	fprintf( stderr, "  -v             -- verbose output\n" );
	fprintf( stderr, "  -r             -- run the script\n" );
	fprintf( stderr, "  -o output.sx   -- output compiled script executable\n" );
	fprintf( stderr, "  -I includepath -- append to include path\n" );
	fprintf( stderr, "  -d             -- strip debug info\n" );
	exit( 1 );
}

//============================================================================

static void Script_sqrt( ScriptVM* vm )
{
	vm->Push( sqrt( vm->PopFloat() ) );
}

//============================================================================

static void Script_sin( ScriptVM* vm )
{
	vm->Push( sin( vm->PopFloat() ) );
}

//============================================================================

static void Script_cos( ScriptVM* vm )
{
	vm->Push( cos( vm->PopFloat() ) );
}

//============================================================================

static void Script_sort4( ScriptVM* vm )
{
	int i, j;
	float v[ 4 ];

	// pop the values, we always pop in reverse order
	for ( i = 3 ; i >= 0 ; i-- )
		v[ i ] = vm->PopFloat();

	// sort
	for ( i = 0 ; i < 4 - 1 ; i++ )
	{
		for ( j = i + 1 ; j < 4 ; j++ )
		{
			if ( v[i] > v[j] )
			{
				float t = v[ i ];
				v[ i ]  = v[ j ];
				v[ j ]  = t;
			}
		}
	}

	// return, always in left to right order
	for ( i = 0 ; i < 4 ; i++ )
		vm->Push( v[ i ] );
}

//============================================================================

static void Script_sum( ScriptVM* vm )
{
	// pop the number of arguments
	int nargs = vm->PopInt();

	// now sum all the arguments.  we pop right to left
	float sum = 0.0f;
	while ( nargs-- > 0 )
		sum += vm->PopFloat();

	vm->Push( sum );
}

//============================================================================

static void Script_vectorNormalize( ScriptVM* vm )
{
	float z = vm->PopFloat();
	float y = vm->PopFloat();
	float x = vm->PopFloat();

	float mag = sqrt( x*x + y*y + z*z );
	float oneOverMag = 1.0f / mag;

	vm->Push( x * oneOverMag );
	vm->Push( y * oneOverMag );
	vm->Push( z * oneOverMag );
}

//============================================================================

int main( int argc, char** argv )
{
	bool verbose = false;
	bool run     = false;
	char* input  = NULL;
	char* output = NULL;
	bool compile = false;
	bool load    = false;
	bool debug   = true;
	int i;
	ScriptError err;
	ScriptExecutable* exe = NULL;

	SCRIPT_REGISTER( sqrt            );
	SCRIPT_REGISTER( sin					);
	SCRIPT_REGISTER( cos					);
	SCRIPT_REGISTER( sort4				);
	SCRIPT_REGISTER( vectorNormalize	);
	SCRIPT_REGISTER( vectorNormalize	);
	SCRIPT_REGISTER( sum             );

	for ( i = 1 ; i < argc ; i++ )
	{
		if ( argv[ i ][ 0 ] != '-' && argv[ i ][ 0 ] != '/' ) usage();
		switch( argv[ i ][ 1 ] )
		{
			case 's': input = argv[ ++i ]; compile = true; load = true; break;
			case 'e': input = argv[ ++i ]; compile = true; load = false; break;
			case 'x': input = argv[ ++i ]; compile = false; load = true; break;
			case 'v': verbose = true; break;
			case 'r': run = true; break;
			case 'o': output = argv[ ++i ]; break;
			case 'p': input = argv[ ++i ]; compile = false; load = false; break;
			case 'I': g_ScriptSystem.AddIncludePath( argv[ ++i ] ); break;
			case 'd': debug = false; break;
		}
	}

	if ( input == NULL )
	{
		fprintf( stderr, "ERROR: no input file specified\n" );
		usage();
	}

	// preprocess
	if ( !compile && !load )
	{
		ScriptPreprocessor pp( &err );
		pp.ProcessFile( input );
		pp.PrintSymbols();
		if ( err.ErrorCount() || verbose )
			err.Print();
		if ( err.ErrorCount() )
			exit( 1 );
		exit( 0 );
	}

	if ( compile )
	{
		if ( load )
			exe = g_ScriptSystem.CompileFile( input, &err, verbose, debug );
		else
			exe = g_ScriptSystem.CompileBuffer( input, strlen( input ), &err, verbose, debug );
	}
	else
	{
		exe = g_ScriptSystem.LoadExecutable( input );
	}

	if ( !exe || err.ErrorCount() )
		goto Error;

	if ( verbose )
	{
		printf( "\n----- executable -----\n" );
		exe->Print();
	}

	if ( run )
	{
		if ( ! g_ScriptSystem.Initialize( exe ) )
		{
			printf( "error, couldn't initialize executable" );
			goto Error;
		}
		if ( verbose )
		{
			printf( "\n----- output -----\n" );
		}

		ScriptVM* vm = g_ScriptSystem.CreateVM( exe );

		// test c's ability to write to script variables
		if ( exe->LookupLabel( "ctestfunction" ) >= 0 )
		{
			vm->SetVar( "intOne", 1 );
			vm->SetVar( "floatTwo", 2.0f );
			vm->SetVar( "stringThree", "three" );
		}

		vm->Run( 0.0f );

		// sanity check
		ASSERT( vm->StackDepth() == 0 );
		ASSERT( vm->PCStackDepth() == 0 );

		// test c's ability to call functions
		if ( exe->LookupLabel( "ctestfunction" ) >= 0 )
		{
			for ( int i = 0 ; i < 10 ; i++ )
			{
				vm->Push( i );
				vm->CallFunction( "ctestfunction", 0 );	// -1 (one push above us), +1 (one pop below us) = 0
				int result = vm->PopInt();
				ASSERT( result == i * i );
			}

			vm->Run( 0.0f );
		}

		g_ScriptSystem.DestroyVM( vm );
	}

	if ( verbose )
	{
		printf( "\n----- executable details -----\n" );
		printf( "opcodes   : %d (%d bytes)\n", exe->m_numOpcodes, exe->m_numOpcodes * sizeof( exe->m_opcodes[0] ) );
		printf( "variables : %d\n", exe->m_numVariables );
		printf( "strings   : %d\n", exe->m_numStrings );
		printf( "size      : %d\n", exe->m_fileSize );
	}

	if ( output )
	{
		g_ScriptSystem.SaveExecutable( exe, output );

		if ( verbose )
		{
			printf( "Wrote output '%s', %d bytes\n", output, exe->m_fileSize );
		}
	}

Error:
	if ( verbose || err.ErrorCount() || err.WarningCount() )
	{
		err.Print();
	}

	if ( err.ErrorCount() )
		return 1;
	else
		return 0;
}