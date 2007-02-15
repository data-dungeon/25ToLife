//===========================================================================
//=
//= ScriptExecutable.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

//===========================================================================

#define SCRIPT_DEINIT_PTR( p ) \
{ \
	if ( *(int*)&(p) != 0 ) \
	{ \
		*(int*)&(p) = ( (int)(p) - (int)&(p) - 1 ); \
	} \
}

#define SCRIPT_INIT_PTR( p ) \
{ \
	if ( *(int*)&(p) != 0 ) \
	{ \
		*(int*)&(p) = (int)p + (int)&(p) + 1; \
	} \
}

#define SCRIPT_SWAP( v ) \
{ \
	if ( swap ) \
	{ \
		SwapValue( &(v), sizeof( v ) ); \
	} \
}

//============================================================================

static void SwapValue( void* v, int N )
{
	char T;

	switch ( N )
	{
		case 0:
		case 1:
			break;  // nothing to do
		case 2:
			T = ( ( char* ) v )[ 0 ];
			( ( char* ) v )[ 0 ] = ( ( char* ) v )[ 1 ];
			( ( char* ) v )[ 1 ] = T;
			break;
		case 4:
			T = ( ( char* ) v )[ 0 ];
			( ( char* ) v )[ 0 ] = ( ( char* ) v )[ 3 ];
			( ( char* ) v )[ 3 ] = T;
			T = ( ( char* ) v )[ 1 ];
			( ( char* ) v )[ 1 ] = ( ( char* ) v )[ 2 ];
			( ( char* ) v )[ 2 ] = T;
			break;
		default:
			fprintf( stderr, "ERROR: don't know how to swap '%d' byte element\n", N );
			break;
	}
}

//============================================================================

void ScriptExecutable::Deinit()
{
	int i;

	if ( m_fileStatus != SCRIPT_STATUS_READY )
		return;

	for ( i = 0 ; i < m_numStrings ; i++ )
		SCRIPT_DEINIT_PTR( m_strings[ i ] );

	for ( i = 0 ; i < m_numVariables ; i++ )
		SCRIPT_DEINIT_PTR( m_variables[ i ].name );

	for ( i = 0 ; i < m_numStatics ; i++ )
		SCRIPT_DEINIT_PTR( m_statics[ i ].name );

	for ( i = 0 ; i < m_numFunctions ; i++ )
	{
		m_functions[ i ].func = 0;
		SCRIPT_DEINIT_PTR( m_functions[ i ].name );
	}

	for ( i = 0 ; i < m_numLabelLookup ; i++ )
		SCRIPT_DEINIT_PTR( m_labelLookup[ i ].name );

	for ( i = 0 ; i < m_numDebugInfo ; i++ )
		SCRIPT_DEINIT_PTR( m_debugInfo[ i ].file );

	SCRIPT_DEINIT_PTR( m_sourceFilename );
	SCRIPT_DEINIT_PTR( m_opcodes			);
	SCRIPT_DEINIT_PTR( m_debugInfo      );
	SCRIPT_DEINIT_PTR( m_variables		);
	SCRIPT_DEINIT_PTR( m_statics        );
	SCRIPT_DEINIT_PTR( m_functions		);
	SCRIPT_DEINIT_PTR( m_variableLookup );
	SCRIPT_DEINIT_PTR( m_labelLookup		);
	SCRIPT_DEINIT_PTR( m_switchTable    );
	SCRIPT_DEINIT_PTR( m_strings			);

	m_fileStatus = SCRIPT_STATUS_NEEDS_INIT;
}

//============================================================================

bool ScriptExecutable::Init()
{
	bool swap;
	int i;

	// first make sure it's out file
	if ( memcmp( m_fileID, SCRIPT_FILE_ID, sizeof( m_fileID ) ) != 0 )
	{
		// ERROR, not our file!
		return false;
	}

	switch ( m_fileStatus )
	{
		case SCRIPT_STATUS_READY:           return true;
		case SCRIPT_STATUS_NEEDS_INIT:      swap = false; break;
		case SCRIPT_STATUS_NEEDS_SWAP_INIT:	swap = true; break;
		default:                            return false; // ERROR, not a script file
	}

	// swap values (if necessary) and fix up pointers
	SCRIPT_SWAP( m_fileVersion );
	if ( m_fileVersion != SCRIPT_FILE_VERSION )
	{
		ASSERTF( false, ( "script executable version mismatch.  got %04X, expecting %04X", m_fileVersion, SCRIPT_FILE_VERSION ) );
		return false;
	}

	SCRIPT_SWAP( m_fileSize );
	SCRIPT_SWAP( m_numOpcodes );
	SCRIPT_SWAP( m_numDebugInfo );
	SCRIPT_SWAP( m_numVariables );
	SCRIPT_SWAP( m_numStatics );
	SCRIPT_SWAP( m_numFunctions );
	SCRIPT_SWAP( m_numStrings );
	SCRIPT_SWAP( m_numVariableLookup );
	SCRIPT_SWAP( m_numLabelLookup );
	SCRIPT_SWAP( m_numSwitchTable );
	SCRIPT_SWAP( m_sourceFilename ); SCRIPT_INIT_PTR( m_sourceFilename );
	SCRIPT_SWAP( m_opcodes );			SCRIPT_INIT_PTR( m_opcodes );
	SCRIPT_SWAP( m_debugInfo );      SCRIPT_INIT_PTR( m_debugInfo );
	SCRIPT_SWAP( m_variables );		SCRIPT_INIT_PTR( m_variables );
	SCRIPT_SWAP( m_statics );        SCRIPT_INIT_PTR( m_statics );
	SCRIPT_SWAP( m_functions );		SCRIPT_INIT_PTR( m_functions );
	SCRIPT_SWAP( m_variableLookup ); SCRIPT_INIT_PTR( m_variableLookup );
	SCRIPT_SWAP( m_labelLookup );		SCRIPT_INIT_PTR( m_labelLookup );
	SCRIPT_SWAP( m_switchTable );    SCRIPT_INIT_PTR( m_switchTable );
	SCRIPT_SWAP( m_strings );			SCRIPT_INIT_PTR( m_strings );

	// opcodes
	for ( i = 0 ; i < m_numOpcodes ; i++ )
	{
		SCRIPT_SWAP( m_opcodes[ i ] );
	}

	// debug info
	for ( i = 0 ; i < m_numDebugInfo ; i++ )
	{
		SCRIPT_SWAP( m_debugInfo[ i ].file );	SCRIPT_INIT_PTR( m_debugInfo[ i ].file );
		SCRIPT_SWAP( m_debugInfo[ i ].line );
	}

	// variables
	for ( i = 0 ; i < m_numVariables ; i++ )
	{
		SCRIPT_SWAP( m_variables[ i ].name );	SCRIPT_INIT_PTR( m_variables[ i ].name );
		SCRIPT_SWAP( m_variables[ i ].type );
		SCRIPT_SWAP( m_variables[ i ].size );
	}

	// statics
	for ( i = 0 ; i < m_numStatics ; i++ )
	{
		SCRIPT_SWAP( m_statics[ i ].name );  SCRIPT_INIT_PTR( m_statics[ i ].name );
		SCRIPT_SWAP( m_statics[ i ].var.m_type );
		SCRIPT_SWAP( m_statics[ i ].var.m_value );
	}

	// functions
	for ( i = 0 ; i < m_numFunctions ; i++ )
	{
		SCRIPT_SWAP( m_functions[ i ].name );	SCRIPT_INIT_PTR( m_functions[ i ].name );
		SCRIPT_SWAP( m_functions[ i ].nameHash );
		SCRIPT_SWAP( m_functions[ i ].stackDelta );
	}

	// strings
	for ( i = 0 ; i < m_numStrings ; i++ )
	{
		SCRIPT_SWAP( m_strings[ i ] );	SCRIPT_INIT_PTR( m_strings[ i ] );
	}

	// variable lookup
	for ( i = 0 ; i < m_numVariableLookup ; i++ )
	{
		SCRIPT_SWAP( m_variableLookup[ i ].hash );
		SCRIPT_SWAP( m_variableLookup[ i ].varIndex );
	}

	// label lookup
	for ( i = 0 ; i < m_numLabelLookup ; i++ )
	{
		SCRIPT_SWAP( m_labelLookup[ i ].hash );
		SCRIPT_SWAP( m_labelLookup[ i ].name );	SCRIPT_INIT_PTR( m_labelLookup[ i ].name );
		SCRIPT_SWAP( m_labelLookup[ i ].pc );
	}

	// switch tables
	for ( i = 0 ; i < m_numSwitchTable ; i++ )
	{
		SCRIPT_SWAP( m_switchTable[ i ].value );
		SCRIPT_SWAP( m_switchTable[ i ].pc    );
	}

	m_fileStatus = SCRIPT_STATUS_READY;
	return true;
}

//============================================================================

#ifdef SCRIPT_COMPILER
void ScriptExecutable::Print()
{
	int i;
	ScriptDebug dbg;
	char* lastFile = NULL;
	int   lastLine = -1;
	char  buf[ 1024 ];

	printf( "Source Filename: %s\n\n", m_sourceFilename );

	for ( i = 0 ; i < m_numOpcodes ; i += ScriptUtil::OpcodeSize( m_opcodes[ i ] ) )
	{
//		// show line of source
//		if ( lastFile != m_debugInfo[ i ].file || lastLine != m_debugInfo[ i ].line )
//		{
//			lastFile = m_debugInfo[ i ].file;
//			lastLine = m_debugInfo[ i ].line;
//			printf( "\n%s", dbg.GetLine( lastFile, lastLine ) );
//		}

		// show any labels that might be on this line:
		for ( int j = 0 ; j < m_numLabelLookup ; j++ )
			if ( m_labelLookup[ j ].pc == i )
				printf( "%s:\n", m_labelLookup[ j ].name );

		int op = m_opcodes[ i ];
		sprintf( buf, "%04d : %-20s", i, ScriptUtil::OpcodeName( op ) );
		switch ( op )
		{
			case OPCODE_BRANCH_TRUE:
			case OPCODE_BRANCH_FALSE:
			case OPCODE_BRANCH_TRUE_NO_POP:
			case OPCODE_BRANCH_FALSE_NO_POP:
			case OPCODE_JUMP:
			case OPCODE_CALL:
			case OPCODE_SWITCH:
				sprintf( buf + strlen( buf ), "\t%d", m_opcodes[ i + 1 ] );
				break;

			case OPCODE_CALL_C:
				sprintf( buf + strlen( buf ), "\t%d (%s)", m_opcodes[ i + 1 ], m_functions[ m_opcodes[ i + 1 ] ].name );
				break;

			case OPCODE_PUSH_VAR:
			case OPCODE_PUSH_VAR_INDEXED:
			case OPCODE_STORE:
			case OPCODE_STORE_INDEXED:
			case OPCODE_POP_AND_STORE:
				{
					int v = (short) m_opcodes[ i + 1 ];
					if ( v >= 0 )
						sprintf( buf + strlen( buf ), "\t%d (%s)", v, m_variables[ v ].name );
					else
					{
						v = -v - 1;
						sprintf( buf + strlen( buf ), "\tstatic %d (%s)", v, m_statics[ v ].name );
					}
				}
				break;

			case OPCODE_SWAP:
				{
					int data = m_opcodes[ i + 1 ];
					sprintf( buf + strlen( buf ), "\t%d,%d", SCRIPT_INSTR_DATA_SWAP0( data ), SCRIPT_INSTR_DATA_SWAP1( data ) );
				}
				break;

			case OPCODE_PUSH_INT:
				{
					unsigned int rawdata = ( m_opcodes[ i + 1 ] << 16 ) | m_opcodes[ i + 2 ];
					int v = *(int*)&rawdata;
					sprintf( buf + strlen( buf ), "\t0x%08X (%d)", v, v );
				}
				break;

			case OPCODE_PUSH_FLOAT:
				{
					unsigned int rawdata = ( m_opcodes[ i + 1 ] << 16 ) | m_opcodes[ i + 2 ];
					float v = *(float*)&rawdata;
					sprintf( buf + strlen( buf ), "\t%f", v );
				}
				break;

			case OPCODE_PUSH_STRING:
				{
					int stringIndex = m_opcodes[ i + 1 ];
					sprintf( buf + strlen( buf ), "\t%d - \"%s\"", stringIndex, m_strings[ stringIndex ] ? m_strings[ stringIndex ] : "(null)" );
				}
				break;

			default:
				break;
		}

		printf( "%-50s", buf );

		// show line of source
		if ( i < m_numDebugInfo )
		{
			if ( lastFile != m_debugInfo[ i ].file || lastLine != m_debugInfo[ i ].line )
			{
				lastFile = m_debugInfo[ i ].file;
				lastLine = m_debugInfo[ i ].line;
				printf( "%s", dbg.GetLine( lastFile, lastLine ) );
			}
		}

		printf( "\n" );
	}

	printf( "\n" );

	if ( m_numVariables )
	{
		for ( i = 0 ; i < m_numVariables ; i++ )
			printf( "Variable %3d : %-20s type=%d  size=%d\n", i, m_variables[ i ].name, m_variables[ i ].type, m_variables[ i ].size );
		printf( "\n" );
	}

	if ( m_numStatics )
	{
		for ( i = 0 ; i < m_numStatics ; i++ )
		{
			printf( "Static Variable %3d : %-20s type=%d  size=%d value=%d\n", i, m_statics[ i ].name, m_statics[ i ].var.Type(), m_statics[ i ].size, m_statics[ i ].var.Value() );
		}
		printf( "\n" );
	}

	if ( m_numFunctions )
	{
		for ( i = 0 ; i < m_numFunctions ; i++ )
			printf( "Function %3d : %-20s hash=%08X func=0x%08X stackDelta=%d\n", i, m_functions[ i ].name, m_functions[ i ].nameHash,
					  m_functions[ i ].func, m_functions[ i ].stackDelta );
		printf( "\n" );
	}

	if ( m_numStrings )
	{
		for ( i = 0 ; i < m_numStrings ; i++ )
			printf( "String   %3d : \"%s\"\n", i, m_strings[ i ] );
		printf( "\n" );
	}

	if ( m_numVariableLookup )
	{
		int totSeek  = 0;
		int totVar   = 0;
		int totEmpty = 0;
		for ( i = 0 ; i < m_numVariableLookup ; i++ )
		{
			if ( m_variableLookup[ i ].varIndex >= 0 )
			{
				int seeks = 1 + ( i + m_numVariableLookup - ( m_variableLookup[ i ].hash % m_numVariableLookup ) ) % m_numVariableLookup;
				printf( "VarHash %3d  : hash=%08X varIndex=%-3d name=%-25s (seeks=%d)\n",
					     i, m_variableLookup[ i ].hash, m_variableLookup[ i ].varIndex, m_variables[ m_variableLookup[ i ].varIndex ].name, seeks );
				totSeek += seeks;
				totVar  += 1;
			}
			else
			{
				printf( "VarHash %3d  : -\n", i );
				totEmpty += 1;
			}
		}
		printf( "\nVarHash average seeks : %.1f  empty=%d  empty-bytes=%d\n", (float)totSeek / (float)totVar, totEmpty, totEmpty * sizeof( m_variableLookup[ 0 ] ) );
		printf( "\n" );
	}

	if ( m_numLabelLookup )
	{
		int totSeek  = 0;
		int totVar   = 0;
		int totEmpty = 0;
		for ( i = 0 ; i < m_numLabelLookup ; i++ )
		{
			if ( m_labelLookup[ i ].pc >= 0 )
			{
				int seeks = 1 + ( i + m_numLabelLookup - ( m_labelLookup[ i ].hash % m_numLabelLookup ) ) % m_numLabelLookup;
				printf( "LabHash %3d  : hash=%08X pc=%-3d name=%-25s (seeks=%d)\n",
					     i, m_labelLookup[ i ].hash, m_labelLookup[ i ].pc, m_labelLookup[ i ].name, seeks );
				totSeek += seeks;
				totVar  += 1;
			}
			else
			{
				printf( "LabHash %3d  : -\n", i );
				totEmpty += 1;
			}
		}
		printf( "\nLabHash average seeks : %.1f  empty=%d  empty-bytes=%d\n", (float)totSeek / (float)totVar, totEmpty, totEmpty * sizeof( m_labelLookup[ 0 ] ) );
		printf( "\n" );
	}

	if ( m_numSwitchTable )
	{
		int nextTable = 0;
		for ( i = 0 ; i < m_numSwitchTable ; i++ )
		{
			if ( i == nextTable )
			{
				int tableCount = m_switchTable[ i ].value;
				printf( "Switch %3d   : case=%-8s pc=%-4d [%d cases follow]\n", i, "default", m_switchTable[ i ].pc, tableCount );
				nextTable = i + tableCount + 1;
			}
			else
			{
				printf( "Switch %3d   : case=%-8d pc=%-4d\n", i, m_switchTable[ i ].value, m_switchTable[ i ].pc );
			}
		}
		ASSERT( i == nextTable );
		printf( "\n" );
	}
}
#endif //SCRIPT_COMPILER

//============================================================================

int ScriptExecutable::LookupLabel( const char* name )
{
	if ( m_numLabelLookup )
	{
		ASSERT( name );
		int len = strlen( name );
		ASSERT( len > 0 );
		unsigned int hash = Hash::CalcChecksum( name, len );

		for ( int i = hash % m_numLabelLookup ; m_labelLookup[ i ].pc >= 0 ; i = ( i + 1 ) % m_numLabelLookup )
			if ( m_labelLookup[ i ].hash == hash && ( strcmp( m_labelLookup[ i ].name, name ) == 0 ) )
				return m_labelLookup[ i ].pc;
	}

	return -1;
}

//============================================================================

int ScriptExecutable::LookupVar( const char* name )
{
	if ( m_numVariableLookup )
	{
		ASSERT( name );
		int len = strlen( name );
		ASSERT( len > 0 );
		unsigned int hash = Hash::CalcChecksum( name, len );

		int var;
		for ( int i = hash % m_numVariableLookup ; ( var = m_variableLookup[ i ].varIndex ) >= 0 ; i = ( i + 1 ) % m_numVariableLookup )
			if ( m_variableLookup[ i ].hash == hash && ( strcmp( m_variables[ var ].name, name ) == 0 ) )
				return var;
	}

	return -1;
}

//============================================================================
