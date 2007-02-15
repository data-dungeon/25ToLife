//===========================================================================
//=
//= ScriptVM.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
#include "stage/cactor.h"
#include "GameHelper/Message.h"
#define SCRIPT_HANDLES_MESSAGES
#endif

#if !defined(CONSUMER_BUILD)
#define SCRIPT_DEBUG
#endif

//===========================================================================

#ifdef SCRIPT_DEBUG
static int PCcurrent = 0;
static int PClast    = 0;
static ScriptVM* RunningScript = NULL;
#endif

//===========================================================================

#define ARITHMETIC_STRING( oper ) \
{ \
	ASSERTS( m_stackDepth >= 2, "script stack underflow" ); \
	ScriptVar& A = m_stack->Var( m_stackDepth - 2 ); \
	ScriptVar& B = m_stack->Var( m_stackDepth - 1 ); \
	if ( A.IsInt() && B.IsInt() ) \
		m_stack->Var( m_stackDepth - 2 ).Set( A.Value().i oper B.Value().i ); \
	else if ( A.IsString() && B.IsString() ) \
		m_stack->Var( m_stackDepth - 2 ).Set( strcmp( A.Value().p, B.Value().p ) oper 0 ); \
	else \
		m_stack->Var( m_stackDepth - 2 ).Set( A.GetFloat() oper B.GetFloat() ); \
	m_stackDepth--; \
	pc++; \
}

#define ARITHMETIC( oper ) \
{ \
	ASSERTS( m_stackDepth >= 2, "script stack underflow" ); \
	ScriptVar& A = m_stack->Var( m_stackDepth - 2 ); \
	ScriptVar& B = m_stack->Var( m_stackDepth - 1 ); \
	if ( A.IsInt() && B.IsInt() ) \
		m_stack->Var( m_stackDepth - 2 ).Set( A.Value().i oper B.Value().i ); \
	else \
		m_stack->Var( m_stackDepth - 2 ).Set( A.GetFloat() oper B.GetFloat() ); \
	m_stackDepth--; \
	pc++; \
}

#define ARITHMETIC_INT( oper ) \
{ \
	ASSERTS( m_stackDepth >= 2, "script stack underflow" ); \
	ScriptVar& A = m_stack->Var( m_stackDepth - 2 ); \
	ScriptVar& B = m_stack->Var( m_stackDepth - 1 ); \
	if ( A.IsInt() && B.IsInt() ) \
		m_stack->Var( m_stackDepth - 2 ).Set( A.Value().i oper B.Value().i ); \
	else \
		m_stack->Var( m_stackDepth - 2 ).Set( 0 ); \
	m_stackDepth--; \
	pc++; \
}

#define ARITHMETIC_UNARY( oper ) \
{ \
	ASSERTS( m_stackDepth >= 1, "script stack underflow" ); \
	ScriptVar& A = m_stack->Var( m_stackDepth - 1 ); \
	if ( A.IsInt() ) \
		m_stack->Var( m_stackDepth - 1 ).Set( oper A.Value().i ); \
	else \
		m_stack->Var( m_stackDepth - 1 ).Set( oper A.GetFloat() ); \
	pc++; \
}

#define ARITHMETIC_UNARY_INT( oper ) \
{ \
	ASSERTS( m_stackDepth >= 1, "script stack underflow" ); \
	ScriptVar& A = m_stack->Var( m_stackDepth - 1 ); \
	if ( A.IsInt() ) \
		m_stack->Var( m_stackDepth - 1 ).Set( oper A.Value().i ); \
	else \
		m_stack->Var( m_stackDepth - 1 ).Set( 0 ); \
	pc++; \
}

#define ARITHMETIC_IMMEDIATE( op ) \
{ \
	ASSERTS( m_stackDepth >= 1, "script stack underflow" ); \
	int varIndex = pc[ 1 ]; \
	ScriptVar& A = m_vars->Var( varIndex ); \
	if      ( A.IsInt() )   { A.Value().i op m_stack->GetInt( m_stackDepth - 1 ); } \
	else if ( A.IsFloat() ) { A.Value().f op m_stack->GetFloat( m_stackDepth - 1 ); } \
	m_stack->Var( m_stackDepth - 1 ).Set( A ); \
}

#define ARITHMETIC_IMMEDIATE_INDEXED( op ) \
{ \
	ASSERTS( m_stackDepth >= 2, "script stack underflow" ); \
	int varIndex = pc[ 1 ]; \
	int offset = m_stack->GetInt( --m_stackDepth ); \
	ASSERTF( offset >= 0 && offset < m_executable->m_variables[ varIndex ].size, ( "array index for '%s' out of range (index=%d)", m_executable->m_variables[ varIndex ].name, offset ) ); \
	ScriptVar& A = m_vars->Var( varIndex + offset ); \
	if      ( A.IsInt() )   { A.Value().i op m_stack->GetInt( m_stackDepth - 1 ); } \
	else if ( A.IsFloat() ) { A.Value().f op m_stack->GetFloat( m_stackDepth - 1 ); } \
	m_stack->Var( m_stackDepth - 1 ).Set( A ); \
}

//===========================================================================

ScriptVM::ScriptVM( ScriptExecutable* executable, ScriptVarList*& stack, int& stackDepth ) :
	m_stack( stack ), m_stackDepth( stackDepth )
{
	// make sure the executable is ready to run and functions are registered
	if ( ! g_ScriptSystem.Initialize( executable ) )
	{
		ASSERTS( false, "couldn't initialize executable" );
	}

	m_executable   = executable;
	m_vars         = new ScriptVarList( executable->m_numVariables );
//	m_stack        = new ScriptVarList( SCRIPT_STACK_SIZE );
	m_pcStack	   = new short [ SCRIPT_PC_STACK_SIZE ];
	m_pc			   = 0;
	m_stackDepth   = 0;
	m_pcStackDepth = 0;
	m_owner        = NULL;
	m_action       = 0;
	m_sleepTime    = 0.0f;
	m_running      = 0;
	m_messageHandlers = NULL;
	m_messageOwner    = NULL;

	// copy variable types from executable
	for ( int i = 0 ; i < executable->m_numVariables ; i++ )
	{
		switch ( executable->m_variables[ i ].type )
		{
			case ScriptVar::TYPE_INT:		m_vars->Var( i ).Set( (int)   0 ); break;
			case ScriptVar::TYPE_FLOAT:	m_vars->Var( i ).Set( (float) 0 ); break;
			case ScriptVar::TYPE_STRING:	m_vars->Var( i ).Set( (char*) 0 ); break;
		}
	}
}

//===========================================================================

ScriptVM::~ScriptVM()
{
	// shut down message handlers
	RemoveAllMessageHandlers();

	// call script shutdown function
	if ( HasFunction( "Terminate" ) )
		CallFunction( "Terminate", 0 );

	if ( m_vars )    { delete m_vars;  m_vars = NULL;  }
//	if ( m_stack )   { delete m_stack; m_stack = NULL; }
	if ( m_pcStack ) { delete [] m_pcStack; m_pcStack = NULL; }
}

//===========================================================================

void ScriptVM::Run( float elapsedTime, int stackDelta /*=0*/ )
{
	#ifdef SCRIPT_DEBUG
	RunningScript = this;
	#endif

	bool done = false;
	ScriptOpcode* pc = &m_executable->m_opcodes[ m_pc ];
	int orgStackDepth = m_stackDepth;

	// if time is advancing, don't run if we're sleeping.
	if ( elapsedTime > 0.0f )
	{
		if ( m_sleepTime > 0.0f )
			m_sleepTime -= elapsedTime;
		if ( m_sleepTime > 0.0f )
			done = true;
	}

	m_running++;

	while ( !done )
	{
		#ifdef SCRIPT_DEBUG
		// printf( "\tpc=%04d stack=%04d pcStack=%04d op=%s\n", pc - m_executable->m_opcodes, m_stackDepth, m_pcStackDepth, ScriptUtil::OpcodeName( pc[ 0 ] ) );
		PClast    = PCcurrent;
		PCcurrent = pc - m_executable->m_opcodes;
		#endif

		switch ( pc[ 0 ] )
		{
			case OPCODE_PUSH_VAR:
				{
					ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
					int varIndex = (short)pc[ 1 ];
					if ( varIndex >= 0 )
						m_stack->Set( m_stackDepth++, m_vars->Var( varIndex ) );
					else
						m_stack->Set( m_stackDepth++, m_executable->m_statics[ -varIndex - 1 ].var );
					pc += 2;
				}
				break;

			case OPCODE_PUSH_VAR_INDEXED:
				{
					ASSERTS( m_stackDepth >= 1, "script stack underflow" );
					int varIndex = (short)pc[ 1 ];
					int offset   = m_stack->Var( m_stackDepth - 1 ).GetInt();
					if ( varIndex >= 0 )
					{
						ASSERTS( varIndex >= 0 && varIndex < m_executable->m_numVariables, "script varaible index bogus" );
						ASSERTF( offset >= 0 && offset < m_executable->m_variables[ varIndex ].size, ( "array index for '%s' out of range (index=%d)", m_executable->m_variables[ varIndex ].name, offset ) );
						m_stack->Set( m_stackDepth - 1, m_vars->Var( varIndex + offset ) );
					}
					else
					{
						varIndex = -varIndex - 1;
						ASSERTS( varIndex >= 0 && varIndex < m_executable->m_numStatics, "script static variable index bogus" );
						ASSERTF( offset >= 0 && offset < m_executable->m_statics[ varIndex ].size, ( "static array index for '%s' out of range (index=%d)", m_executable->m_statics[ varIndex ].name, offset ) );
						m_stack->Set( m_stackDepth - 1, m_executable->m_statics[ varIndex + offset ].var );
					}
					pc += 2;
				}
				break;

			case OPCODE_PUSH_INT:
				{
					unsigned int rawdata = ( pc[ 1 ] << 16 ) | pc[ 2 ];
					int v = *(int*)&rawdata;
					Push( v );
					pc += 3;
				}
				break;

			case OPCODE_PUSH_FLOAT:
				{
					unsigned int rawdata = ( pc[ 1 ] << 16 ) | pc[ 2 ];
					float v = *(float*)&rawdata;
					Push( v );
					pc += 3;
				}
				break;

			case OPCODE_PUSH_STRING:
				Push( m_executable->m_strings[ pc[ 1 ] ] );
				pc += 2;
				break;

			case OPCODE_PUSH_OWNER:
				#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
				Push( CActor::ToHandle( (CActor*)GetOwner() ) );
				#else
				Push( (int)GetOwner() );
				#endif
				pc += 1;
				break;

			case OPCODE_DUP:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow " );
				m_stack->Set( m_stackDepth, m_stack->Var( m_stackDepth - 1 ) );
				m_stackDepth++;
				pc++;
				break;

			case OPCODE_SWAP:
				{
					int data = pc[ 1 ];
					int index0 = m_stackDepth - 1 - SCRIPT_INSTR_DATA_SWAP0( data );
					int index1 = m_stackDepth - 1 - SCRIPT_INSTR_DATA_SWAP1( data );
					ASSERTS( index0 >= 0 && index0 < SCRIPT_STACK_SIZE, "script stack out of range" );
					ASSERTS( index1 >= 0 && index1 < SCRIPT_STACK_SIZE, "script stack out of range" );
					ScriptVar temp         = m_stack->Var( index0 );
					m_stack->Var( index0 ) = m_stack->Var( index1 );
					m_stack->Var( index1 ) = temp;
				}
				pc += 2;
				break;

			case OPCODE_POP:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				m_stackDepth--;
				pc++;
				break;

			case OPCODE_POP_AND_STORE:
				{
					ASSERTS( m_stackDepth >= 1, "script stack underflow" );
					int varIndex = (short)pc[ 1 ];
					if ( varIndex >= 0 )
					{
						ASSERTF( varIndex >= 0 && varIndex < m_executable->m_numVariables, ( "variable index %d out of range", varIndex ) );
						m_vars->Var( varIndex ).SetKeepType( m_stack->Var( m_stackDepth - 1 ) );
					}
					else
					{
						varIndex = -varIndex - 1;
						ASSERTF( varIndex >= 0 && varIndex < m_executable->m_numStatics, ( "static variable index %d out of range", varIndex ) );
						m_executable->m_statics[ varIndex ].var.SetKeepType( m_stack->Var( m_stackDepth - 1 ) );
					}
					m_stackDepth--;
				}
				pc += 2;
				break;

			case OPCODE_STORE:
				{
					ASSERTS( m_stackDepth >= 1, "script stack underflow" );
					int varIndex = (short)pc[ 1 ];
					if ( varIndex >= 0 )
					{
						m_vars->Var( varIndex ).SetKeepType( m_stack->Var( m_stackDepth - 1 ) );
					}
					else
					{
						varIndex = -varIndex - 1;
						m_executable->m_statics[ varIndex ].var.SetKeepType( m_stack->Var( m_stackDepth - 1 ) );
					}
				}
				pc += 2;
				break;

			case OPCODE_STORE_INDEXED:
				{
					ASSERTS( m_stackDepth >= 2, "script stack underflow" );
					int varIndex = (short)pc[ 1 ];
					int offset   = m_stack->Var( --m_stackDepth ).GetInt();
					if ( varIndex >= 0 )
					{
						ASSERTF( offset >= 0 && offset < m_executable->m_variables[ varIndex ].size, ( "array index for '%s' out of range (index=%d)", m_executable->m_variables[ varIndex ].name, offset ) );
						m_vars->Var( varIndex + offset ).SetKeepType( m_stack->Var( m_stackDepth - 1 ) );
					}
					else
					{
						varIndex = -varIndex - 1;
						ASSERTF( offset >= 0 && offset < m_executable->m_statics[ varIndex ].size, ( "static array index for '%s' out of range (index=%d)", m_executable->m_statics[ varIndex ].name, offset ) );
						m_executable->m_statics[ varIndex + offset ].var.SetKeepType( m_stack->Var( m_stackDepth - 1 ) );
					}
				}
				pc += 2;
				break;

			case OPCODE_LESS_THAN:		ARITHMETIC_STRING( <  );	break;
			case OPCODE_LESS_THAN_EQ:	ARITHMETIC_STRING( <= );	break;
			case OPCODE_GTR_THAN:		ARITHMETIC_STRING( >  );	break;
			case OPCODE_GTR_THAN_EQ:	ARITHMETIC_STRING( >= );	break;
			case OPCODE_EQUAL:			ARITHMETIC_STRING( == );	break;
			case OPCODE_NOT_EQUAL:		ARITHMETIC_STRING( != );	break;
			case OPCODE_ADD:				ARITHMETIC( +  );				break;
			case OPCODE_SUB:				ARITHMETIC( -  );				break;
			case OPCODE_MULT:				ARITHMETIC( *  );				break;
			case OPCODE_DIV:				ARITHMETIC( /  );				break;
			case OPCODE_LOGICAL_AND:	ARITHMETIC( && );				break;
			case OPCODE_LOGICAL_OR:		ARITHMETIC( || );				break;
			case OPCODE_MOD:				ARITHMETIC_INT( %  );		break;
			case OPCODE_SHIFT_L:			ARITHMETIC_INT( << );		break;
			case OPCODE_SHIFT_R:			ARITHMETIC_INT( >> );		break;
			case OPCODE_BITWISE_AND:	ARITHMETIC_INT( &  );		break;
			case OPCODE_BITWISE_OR:		ARITHMETIC_INT( |  );		break;
			case OPCODE_BITWISE_XOR:	ARITHMETIC_INT( ^  );		break;

			case OPCODE_NEGATE:			ARITHMETIC_UNARY( -  );		break;
			case OPCODE_BITWISE_NOT:	ARITHMETIC_UNARY_INT( ~  ); break;
			case OPCODE_LOGICAL_NOT:	ARITHMETIC_UNARY( !  );		break;

			case OPCODE_ABS:
				{
					ASSERTS( m_stackDepth >= 1, "script stack underflow" );
					ScriptValue            v = m_stack->Var( m_stackDepth - 1 ).Value();
					ScriptVar::VarType t = m_stack->Var( m_stackDepth - 1 ).Type();
					if      ( t == ScriptVar::TYPE_INT   ) { if ( v.i < 0 )    { m_stack->Var( m_stackDepth - 1 ).Set( -v.i ); } }
					else if ( t == ScriptVar::TYPE_FLOAT ) { if ( v.f < 0.0f ) { m_stack->Var( m_stackDepth - 1 ).Set( -v.f ); } }
				}
				pc++;
				break;

			case OPCODE_BRANCH_TRUE:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				if ( m_stack->Var( m_stackDepth - 1 ).GetInt() )
					pc = &m_executable->m_opcodes[ pc[ 1 ] ];
				else
					pc += 2;
				m_stackDepth--;
				break;

			case OPCODE_BRANCH_TRUE_NO_POP:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				if ( m_stack->Var( m_stackDepth - 1 ).GetInt() )
					pc = &m_executable->m_opcodes[ pc[ 1 ] ];
				else
					pc += 2;
				break;

			case OPCODE_BRANCH_FALSE:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				if ( ! m_stack->Var( m_stackDepth - 1 ).GetInt() )
					pc = &m_executable->m_opcodes[ pc[ 1 ] ];
				else
					pc += 2;
				m_stackDepth--;
				break;

			case OPCODE_BRANCH_FALSE_NO_POP:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				if ( ! m_stack->Var( m_stackDepth - 1 ).GetInt() )
					pc = &m_executable->m_opcodes[ pc[ 1 ] ];
				else
					pc += 2;
				break;

			case OPCODE_JUMP:
				pc = &m_executable->m_opcodes[ pc[ 1 ] ];
				ASSERTS( pc >= &m_executable->m_opcodes[0] && pc < &m_executable->m_opcodes[ m_executable->m_numOpcodes ], "script pc bogus" );
				break;

			case OPCODE_CALL:
				ASSERTS( m_pcStackDepth < SCRIPT_PC_STACK_SIZE, "script PC stack overflow" );
				m_pcStack[ m_pcStackDepth++ ] = ( pc + 2 ) - m_executable->m_opcodes;
				pc = &m_executable->m_opcodes[ pc[ 1 ] ];
				ASSERTS( pc >= &m_executable->m_opcodes[0] && pc < &m_executable->m_opcodes[ m_executable->m_numOpcodes ], "script pc bogus" );
				break;

			case OPCODE_RETURN:
				{
					ASSERTS( m_pcStackDepth >= 1, "script PC stack underflow" );
					int returnPC = m_pcStack[ --m_pcStackDepth ];
					if ( returnPC < 0 )
						done = true;	// return to C from CallFunction
					else
						pc = &m_executable->m_opcodes[ returnPC ];
					ASSERTS( pc >= &m_executable->m_opcodes[0] && pc < &m_executable->m_opcodes[ m_executable->m_numOpcodes ], "script pc bogus" );
				}
				break;

			case OPCODE_CALL_C:
				{
					#ifdef SCRIPT_DEBUG
					m_pc = pc - &m_executable->m_opcodes[ 0 ];
					#endif
					int funcIndex = pc[ 1 ];
					ASSERTS( funcIndex >= 0 && funcIndex < m_executable->m_numFunctions, "script C function index bogus" );
					ScriptFunction func = m_executable->m_functions[ funcIndex ].func;
					ASSERTF( func, ( "script function '%s' not registered", m_executable->m_functions[ funcIndex ].name ) );
					int curStack = m_stackDepth;
					func( this );
					ASSERTF( curStack + m_executable->m_functions[ funcIndex ].stackDelta == m_stackDepth ||
						      ( m_executable->m_functions[ funcIndex ].stackDelta == 0x1000 ),
						      ( "script C function '%s' stack use doesn't agree with script definition",
								  m_executable->m_functions[ funcIndex ].name ) );
				}
				pc += 2;
				break;

			case OPCODE_END:
				done = true;
				break;

			case OPCODE_PRINT:
				{
					ASSERTS( m_stackDepth >= 1, "script stack underflow" );
					int parms = m_stack->Var( --m_stackDepth ).GetInt();
					ASSERTS( m_stackDepth >= parms, "script stack underflow" );
					for ( int i = 0 ; i < parms ; i++ )
					{
						if      ( m_stack->Var( m_stackDepth - parms + i ).IsInt()    )	printf( "%d", m_stack->Var( m_stackDepth - parms + i ).GetInt() );
						else if ( m_stack->Var( m_stackDepth - parms + i ).IsFloat()  )	printf( "%f", m_stack->Var( m_stackDepth - parms + i ).GetFloat() );
						else if ( m_stack->Var( m_stackDepth - parms + i ).IsString() )	printf( "%s", m_stack->Var( m_stackDepth - parms + i ).GetString() );
						if ( i + 1 < parms ) printf( " " );
					}
					m_stackDepth -= parms;
					printf( "\n" );
				}
				pc++;
				break;

			case OPCODE_SLEEP:
				ASSERTS( m_stackDepth >= 1, "script stack underflow" );
				m_sleepTime = m_stack->Var( --m_stackDepth ).GetFloat();
				done = true;
				pc++;
				break;

			case OPCODE_SWITCH:
				{
					ASSERTS( m_stackDepth >= 1, "script stack underflow" );
					int v       = m_stack->Var( --m_stackDepth ).GetInt();
					int table   = pc[ 1 ];
					ASSERT( table >= 0 && table < m_executable->m_numSwitchTable );
					int jumpTo   = m_executable->m_switchTable[ table ].pc;	// the default case
					int numCases = m_executable->m_switchTable[ table ].value;

					// find if this case is in our table (binary search the sorted table)
					int lo = table + 1;
					int hi = table + numCases;
					while ( lo <= hi )
					{
						int md    = ( lo + hi ) / 2;
						int delta = m_executable->m_switchTable[ md ].value - v;
						if      ( delta == 0 ) { jumpTo = m_executable->m_switchTable[ md ].pc; break; }
						else if ( delta  < 0 ) { lo = md + 1; }
						else                   { hi = md - 1; }
					}

					// jump if we have a valid new pc
					if ( jumpTo >= 0 )
						pc = &m_executable->m_opcodes[ jumpTo ];
					else
						pc += 2;
				}
				break;

			default:
				ASSERTS( pc >= &m_executable->m_opcodes[0] && pc < &m_executable->m_opcodes[ m_executable->m_numOpcodes ], "script pc bogus" );
				ASSERTF( false, ( "script unknown opcode @ pc %d (opcode=%d)", pc - &m_executable->m_opcodes[ 0 ], pc[ 0 ] ) );
				break;
		}
	}

	m_running--;

	ASSERTS( pc >= &m_executable->m_opcodes[0] && pc < &m_executable->m_opcodes[ m_executable->m_numOpcodes ], "script pc bogus" );
	m_pc = pc - &m_executable->m_opcodes[ 0 ];

	ASSERTS( orgStackDepth + stackDelta == m_stackDepth, "Script run used unexpected amount of stack" );

	#ifdef SCRIPT_DEBUG
	RunningScript = NULL;
	#endif
}

//===========================================================================

void ScriptVM::SetVar( int varIndex, int value )
{
	switch ( m_vars->Var( varIndex ).Type() )
	{
		case ScriptVar::TYPE_INT:		m_vars->Var( varIndex ).Set( value );			break;
		case ScriptVar::TYPE_FLOAT:	m_vars->Var( varIndex ).Set( (float)value ); break;
		case ScriptVar::TYPE_STRING:	m_vars->Var( varIndex ).Set( (char*)0 );		break;
	}
}

//===========================================================================

void ScriptVM::SetVar( int varIndex, float value )
{
	switch ( m_vars->Var( varIndex ).Type() )
	{
		case ScriptVar::TYPE_INT:		m_vars->Var( varIndex ).Set( (int)value );	break;
		case ScriptVar::TYPE_FLOAT:	m_vars->Var( varIndex ).Set( value );			break;
		case ScriptVar::TYPE_STRING:	m_vars->Var( varIndex ).Set( (char*)0 );		break;
	}
}

//===========================================================================

void ScriptVM::SetVar( int varIndex, char* value )
{
	switch ( m_vars->Var( varIndex ).Type() )
	{
		case ScriptVar::TYPE_INT:		m_vars->Var( varIndex ).Set( 0     );	break;
		case ScriptVar::TYPE_FLOAT:	m_vars->Var( varIndex ).Set( 0.0f  );	break;
		case ScriptVar::TYPE_STRING:	m_vars->Var( varIndex ).Set( value );	break;
	}
}

//===========================================================================

void ScriptVM::SetVar( int varIndex, int type, ScriptValue value )
{
	switch ( type )
	{
		case ScriptVar::TYPE_INT:		SetVar( varIndex, value.i ); break;
		case ScriptVar::TYPE_FLOAT:	SetVar( varIndex, value.f ); break;
		case ScriptVar::TYPE_STRING:	SetVar( varIndex, value.p ); break;
	}
}

//===========================================================================

void ScriptVM::SetVar( const char* name, int value )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	SetVar( varIndex, value );
}

//===========================================================================

void ScriptVM::SetVar( const char* name, float value )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	SetVar( varIndex, value );
}

//===========================================================================

void ScriptVM::SetVar( const char* name, char* value )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	SetVar( varIndex, value );
}

//===========================================================================

void ScriptVM::SetVar( const char* name, int type, ScriptValue value )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	SetVar( varIndex, type, value );
}

//===========================================================================

int ScriptVM::GetInt( const char* name )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	return m_vars->Var( varIndex ).GetInt();
}

//===========================================================================

float ScriptVM::GetFloat( const char* name )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	return m_vars->Var( varIndex ).GetFloat();
}

//===========================================================================

char* ScriptVM::GetString( const char* name )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	return m_vars->Var( varIndex ).GetString();
}

//===========================================================================

ScriptVar::VarType ScriptVM::GetType( const char* name )
{
	int varIndex = m_executable->LookupVar( name );
	ASSERTF( varIndex >= 0, ( "variable '%s' not found", name ) );
	return m_vars->Var( varIndex ).Type();
}

//===========================================================================

void ScriptVM::Push( int value )
{
	ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
	m_stack->Var( m_stackDepth++ ).Set( value );
}

//===========================================================================

void ScriptVM::Push( float value )
{
	ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
	m_stack->Var( m_stackDepth++ ).Set( value );
}

//===========================================================================

void ScriptVM::Push( char* value )
{
	ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
	m_stack->Var( m_stackDepth++ ).Set( value );
}

//===========================================================================

void ScriptVM::Push( void* value, ScriptVar::VarType type )
{
	ScriptValue data;
	data.p = (char*)value;
	ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
	m_stack->Var( m_stackDepth++ ).Set( type, data );
}

//===========================================================================

#ifdef HAVE_VECTOR3
void ScriptVM::Push( Vector3CRef value )
{
	ASSERTS( m_stackDepth + 3 <= SCRIPT_STACK_SIZE, "script stack overflow" );
	m_stack->Var( m_stackDepth++ ).Set( value.X() );
	m_stack->Var( m_stackDepth++ ).Set( value.Y() );
	m_stack->Var( m_stackDepth++ ).Set( value.Z() );
}
#endif

//===========================================================================

int ScriptVM::PopInt()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	return m_stack->Var( --m_stackDepth ).GetInt();
}

//===========================================================================

bool ScriptVM::PopBool()
{
	return PopInt() != 0;
}

//===========================================================================

float ScriptVM::PopFloat()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	return m_stack->Var( --m_stackDepth ).GetFloat();
}

//===========================================================================

ScriptVar::VarType ScriptVM::PeekType()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	return m_stack->Var( m_stackDepth - 1 ).Type();
}

//===========================================================================

#ifdef HAVE_VECTOR3
Vector3 ScriptVM::PopVector3()
{
	ASSERTS( m_stackDepth >= 3, "script stack underflow" );
	float z = PopFloat();
	float y = PopFloat();
	float x = PopFloat();
	return Vector3( x, y, z );
}
#endif

//===========================================================================

char* ScriptVM::PopString()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	return m_stack->Var( --m_stackDepth ).GetString();
}

//===========================================================================

void* ScriptVM::PopTypeless()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	return (void*)m_stack->Var( --m_stackDepth ).Value().p;
}

//===========================================================================

ScriptVar ScriptVM::PopVar()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	return m_stack->Var( --m_stackDepth );
}

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
CActor* ScriptVM::PopActor()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	ActorHandle handle = (uint32)m_stack->Var( --m_stackDepth ).GetInt();
	return CActor::FromHandle( handle );
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
ActorHandle ScriptVM::PopActorHandle()
{
	ASSERTS( m_stackDepth >= 1, "script stack underflow" );
	ActorHandle handle = (uint32)m_stack->Var( --m_stackDepth ).GetInt();
	return handle;
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
void ScriptVM::Push( CActor* actor )
{
	ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
	ActorHandle handle = CActor::ToHandle( actor );
	m_stack->Var( m_stackDepth++ ).Set( (int)( (uint32)handle ) );
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
void ScriptVM::Push( ActorHandle handle )
{
	ASSERTS( m_stackDepth < SCRIPT_STACK_SIZE, "script stack overflow" );
	m_stack->Var( m_stackDepth++ ).Set( (int)( (uint32)handle ) );
}
#endif

//===========================================================================

void ScriptVM::ClearStacks()
{
	m_stackDepth = 0;
	m_pcStackDepth = 0;
}

//===========================================================================

void ScriptVM::JumpToLabel( const char* label )
{
	int pc = m_executable->LookupLabel( label );
	if ( pc >= 0 )
	{
		ASSERTF( !m_running, ( "script '%s' is already running, JumpToLabel '%s' will be ignored.", m_executable->m_sourceFilename, label ) );

		// jump
		ClearStacks();
		m_pc        = pc;
		m_sleepTime = 0.0f;
	}
	else
		ASSERTF( false, ( "label '%s' not found in script", label ) );
}

//===========================================================================

bool ScriptVM::HasFunction( const char* functionName )
{
	int pc = m_executable->LookupLabel( functionName );
	return( pc >= 0 );
}

//===========================================================================

void ScriptVM::CallFunction( const char* functionName, int stackDelta /*=0*/ )
{
	int pc = m_executable->LookupLabel( functionName );
	if ( pc >= 0 )
	{
		// ensure we get out with "return" and not with a sleep
		int savePCStackDepth = m_pcStackDepth;

		// push "return to C" value on return PC stack
		ASSERTS( m_pcStackDepth < SCRIPT_PC_STACK_SIZE, "script PC stack overflow" );
		m_pcStack[ m_pcStackDepth++ ] = -1;

		// call
		int savePC      = m_pc;
		float saveSleep = m_sleepTime;
		m_pc            = pc;
		Run( 0.0f, stackDelta );
		m_pc            = savePC;
		m_sleepTime     = saveSleep;

		// make sure life is as we expect
		ASSERTF( m_pcStackDepth == savePCStackDepth, ( "script CallFunction '%s' altered pc stack depth", functionName ) );
	}
	else
		ASSERTF( false, ( "function '%s' not found in script", functionName ) );
}

//===========================================================================

#ifdef SCRIPT_HANDLES_MESSAGES
class ScriptMessageHandler : public CMessageHandler
{
public:
	ScriptMessageHandler()
	{
		DeleteWhenDefunct( true );
		d_vm = NULL;
		d_labelIndex = -1;
	}
	virtual void HandleMessage( void* i_data, ActorHandle sender, bool i_posted );
	inline  void SetVM( ScriptVM* vm )                          { d_vm  = vm;                }
	inline  void SetLabelIndex( int labelIndex )                { d_labelIndex = labelIndex; }
	inline  void SetNextVMHandler( ScriptMessageHandler* next ) { d_nextVMHandler = next;    }
	inline  void SetDataType( ScriptVar::VarType type )			{ d_dataType = type;         }
	inline  int  LabelIndex() const                             { return d_labelIndex;       }
	inline  ScriptMessageHandler*& NextVMHandler()              { return d_nextVMHandler;    }
private:
	ScriptVM*				  d_vm;
	ScriptMessageHandler*  d_nextVMHandler;
	short						  d_labelIndex;
	unsigned char		     d_dataType;
};
#endif //SCRIPT_HANDLES_MESSAGES

//===========================================================================

#ifdef SCRIPT_HANDLES_MESSAGES
void ScriptMessageHandler::HandleMessage( void* i_data, ActorHandle sender, bool i_posted )
{
	ScriptExecutable::LabelLookup* label = &d_vm->m_executable->m_labelLookup[ d_labelIndex ];

	// dbgPrint( "%s got message, calling %s\n", d_vm->m_executable->m_sourceFilename, label->name );

	d_vm->Push( i_data, (ScriptVar::VarType)d_dataType );
	d_vm->Push( sender );
	d_vm->CallFunction( label->name, -1 );
	const char* jumpLabel = d_vm->PopString();

	if ( jumpLabel && *jumpLabel )
		d_vm->JumpToLabel( jumpLabel );
}
#endif //SCRIPT_HANDLES_MESSAGES

//===========================================================================

#define PREFIX			"Handle_"
#define PREFIX_LEN	7

void ScriptVM::RegisterMessageHandlers( CMessageOwner* messageOwner )
{
	#ifdef SCRIPT_HANDLES_MESSAGES

	ASSERT_PTR( messageOwner );

	// we should be clean coming in here
	ASSERT( m_messageOwner == NULL );
	ASSERT( m_messageHandlers == NULL );

	// remember owner so we can dynamically register later.
	m_messageOwner = messageOwner;

	for ( int i = 0 ; i < m_executable->m_numLabelLookup ; i++ )
	{
		ScriptExecutable::LabelLookup* label = &m_executable->m_labelLookup[ i ];
		if ( label->pc >= 0 && strlen( label->name ) > PREFIX_LEN && !memcmp( label->name, PREFIX, PREFIX_LEN ) )
		{
			if ( m_executable->m_debugInfo )
				ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( m_executable->m_debugInfo[ label->pc ].file, m_executable->m_debugInfo[ label->pc ].line );
			else
				ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( m_executable->m_sourceFilename, 0 );

			ScriptMessageHandler* handler = new ScriptMessageHandler;
			handler->SetVM( this );
			handler->SetLabelIndex( i );
			handler->SetDataType( DetermineMessageHandlerDataType( i ) );
			GetSingleton(CMessageDispatcher).Register( handler, label->name + PREFIX_LEN, *messageOwner );

			// add handler to our list of handlers so we can find us later
			handler->SetNextVMHandler( m_messageHandlers );
			m_messageHandlers = handler;
		}
	}

	#endif //SCRIPT_HANDLES_MESSAGES
}

//===========================================================================

void ScriptVM::RemoveAllMessageHandlers()
{
	#ifdef SCRIPT_HANDLES_MESSAGES

	ScriptMessageHandler* handler;
	ScriptMessageHandler* next;
	for ( handler = m_messageHandlers ; handler ; handler = next )
	{
		next = handler->NextVMHandler();
		GetSingleton(CMessageDispatcher).Unregister( handler );	// calls "delete" on handler
	}
	m_messageHandlers = NULL;
	m_messageOwner = NULL;

	#endif //SCRIPT_HANDLES_MESSAGES
}

//===========================================================================

void ScriptVM::SetMessageHandlerRules( const char* message, unsigned int rules )
{
	#ifdef SCRIPT_HANDLES_MESSAGES

	ScriptMessageHandler* handler;
	for ( handler = m_messageHandlers ; handler ; handler = handler->NextVMHandler() )
	{
		ASSERT( handler->LabelIndex() >= 0 && handler->LabelIndex() < m_executable->m_numLabelLookup );
		if ( strcmp( handler->GetName(), message ) == 0 )
		{
			handler->SetRules( rules );
			return;
		}
	}

	ASSERTF( false, ( "SetMessageHandlerRules: script '%s' couldn't find handler for message '%s'",
						   m_executable->m_sourceFilename, message ) );

	#endif //SCRIPT_HANDLES_MESSAGES
}

//===========================================================================

void ScriptVM::RemoveMessageHandler( const char* message )
{
	#ifdef SCRIPT_HANDLES_MESSAGES

	// remove any handlers attached to this script waiting for a given message

	ScriptMessageHandler** parent = &m_messageHandlers;
	ScriptMessageHandler* handler;
	for ( handler = *parent ; handler ; handler = *parent )
	{
		if ( strcmp( handler->GetName(), message ) == 0 )
		{
			// link parent to next guy
			*parent = handler->NextVMHandler();

			// unregister will delete handler as well since we set "DeleteWhenDefunct"
			GetSingleton(CMessageDispatcher).Unregister( handler );
		}
		else
		{
			parent = &handler->NextVMHandler();
		}
	}

	#endif //SCRIPT_HANDLES_MESSAGES
}

//===========================================================================

void ScriptVM::RouteMessageToFunction( const char* message, const char* functionName )
{
	#ifdef SCRIPT_HANDLES_MESSAGES

	if ( message && message[0] )
	{
		ASSERT( functionName && functionName[0] );
		ASSERT( m_messageOwner );

		// remove any existing handlers for this message
		RemoveMessageHandler( message );

		// find the function and point the message to it.
		for ( int i = 0 ; i < m_executable->m_numLabelLookup ; i++ )
		{
			ScriptExecutable::LabelLookup* label = &m_executable->m_labelLookup[ i ];
			if ( label->pc >= 0 && strcmp( label->name, functionName ) == 0 )
			{
				ScriptMessageHandler* handler = new ScriptMessageHandler;
				handler->SetVM( this );
				handler->SetLabelIndex( i );
				handler->SetDataType( DetermineMessageHandlerDataType( i ) );
				GetSingleton(CMessageDispatcher).Register( handler, message, *m_messageOwner );

				// add handler to our list of handlers so we can find us later
				handler->SetNextVMHandler( m_messageHandlers );
				m_messageHandlers = handler;
				return;
			}
		}

		ASSERTF( false, ( "RouteMessageToFunction msg:'%s' -- func '%s' doesn't exist", message, functionName ) );
	}

	#endif //SCRIPT_HANDLES_MESSAGES
}

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
int ScriptVM::GetMasterMetaDataSize()
{
	if (HasFunction("GetMasterMetaDataVars"))
	{
		CallFunction("GetMasterMetaDataVars", 1);
		return PopInt();
	}
	return 0;
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
bool ScriptVM::HasMasterMetaDataChangedSinceLastWrite()
{
	if (HasFunction("HasMasterMetaDataChangedSinceLastWrite"))
	{
		CallFunction("HasMasterMetaDataChangedSinceLastWrite", 1);
		return PopBool();
	}
	return false;
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
void ScriptVM::WriteMasterMetaData( void* &io_buffer )
{
	if (HasFunction("WriteMasterMetaData"))
	{
		Push(io_buffer);
		CallFunction("WriteMasterMetaData", -1);
	}
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
int ScriptVM::GetMasterDataSize()
{
	if (HasFunction("GetMasterDataSize"))
	{
		CallFunction("GetMasterDataSize", 1);
		return PopInt();
	}
	return 0;
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
bool ScriptVM::HasMasterDataChangedSinceLastWrite()
{
	if (HasFunction("HasMasterDataChangedSinceLastWrite"))
	{
		CallFunction("HasMasterDataChangedSinceLastWrite", 1);
		return PopBool();
	}
	return false;
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
void ScriptVM::WriteMasterData( void* &io_metaDataBuffer, void* &io_dataBuffer )
{
	if (HasFunction("WriteMasterMetaData"))
	{
		Push(io_metaDataBuffer);
		CallFunction("WriteMasterMetaData", -1);
	}

	if (HasFunction("WriteMasterData"))
	{
		Push(io_dataBuffer);
		CallFunction("WriteMasterData", -1);
	}
}
#endif

//===========================================================================

#if !defined( WIN32_TOOLS_DEBUG ) && !defined( WIN32_TOOLS_RELEASE )
void ScriptVM::ReadMasterData( void* &io_metaDataBuffer, void* &io_dataBuffer )
{
	if (HasFunction("ReadMasterMetaData"))
	{
		Push(io_metaDataBuffer);
		CallFunction("ReadMasterMetaData", -1);
	}

	if (HasFunction("ReadMasterData"))
	{
		Push(io_dataBuffer);
		CallFunction("ReadMasterData", -1);
	}
}
#endif

//===========================================================================

ScriptVar::VarType ScriptVM::DetermineMessageHandlerDataType( int labelIndex )
{
	// this is tricky - look at the opcodes for the message handler routine.
	// there should be two "POP_AND_STORE" instructions back to back.  the second
	// one is popping the "data" for the message, and we need to determine the
	// type of the variable it is popping into (INT, FLOAT, STRING).
	
	int pc = m_executable->m_labelLookup[ labelIndex ].pc;
	ASSERT( pc >= 0 && pc + 3 < m_executable->m_numOpcodes );
	if ( m_executable->m_opcodes[ pc + 0 ] == OPCODE_POP_AND_STORE &&
		  m_executable->m_opcodes[ pc + 2 ] == OPCODE_POP_AND_STORE )
	{
		int var = m_executable->m_opcodes[ pc + 3 ];
		ASSERT( var >= 0 && var < m_executable->m_numVariables );
		return (ScriptVar::VarType) m_executable->m_variables[ var ].type;
	}
	else
	{
		ASSERTF( false, ( "couldn't determine data type for %s", m_executable->m_labelLookup[ labelIndex ].name ) );
		return ScriptVar::TYPE_INT;
	}
}

//===========================================================================

void ScriptVM::GetDebugInfo( char* infoStr )
{
	infoStr[0] = 0;
#ifdef SCRIPT_DEBUG
	if ( RunningScript )
		sprintf( infoStr, "script:%s pc=%d lpc=%d\n", RunningScript->m_executable->m_sourceFilename, PCcurrent, PClast );
#endif
}

//===========================================================================
