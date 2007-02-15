//============================================================================
//=
//= ScriptScriptFunc.h - Script interfact functions for the "script" library
//=
//============================================================================

#include "ScriptPCH.h"
#include "stage/cactor.h"
#include "EngineHelper/PackBuffer.h"

//============================================================================

void Script_ClearStacks( ScriptVM* vm )
{
	vm->ClearStacks();
}

//============================================================================

void Script_SetMessageHandlerRules( ScriptVM* vm )
{
	unsigned int rules = vm->PopInt();
	const char* message = vm->PopString();

	ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
	vm->SetMessageHandlerRules( message, rules );
}

//============================================================================

void Script_RemoveMessageHandler( ScriptVM* vm )
{
	const char* message = vm->PopString();
	ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
	vm->RemoveMessageHandler( message );
}

//============================================================================

void Script_RouteMessageToFunction( ScriptVM* vm )
{
	const char* functionName = vm->PopString();
	const char* message      = vm->PopString();
	ASSIGN_MESSAGE_DISPATCHER_CALL_LOCATION( vm->GetScriptFileNameOfPC(), vm->GetScriptLineNumberOfPC() );
	vm->RouteMessageToFunction( message, functionName );
}

//============================================================================

void Script_GetScriptInt( ScriptVM* vm )
{
	const char *varName = vm->PopString();
	int script = vm->PopInt();
	CActor *actor = vm->PopActor();
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( actor->Script( script )->GetInt( varName ) );
}

//============================================================================

void Script_GetScriptFloat( ScriptVM* vm )
{
	const char *varName = vm->PopString();
	int script = vm->PopInt();
	CActor *actor = vm->PopActor();
	ASSERT( actor->IsA( CActor::ClassType() ) );
	vm->Push( actor->Script( script )->GetFloat( varName ) );
}

//============================================================================

void Script_PackBuffer( ScriptVM* vm )
{
	void* buffer;
	const char* names[ 32 ];
	int i;

	// get the number of parameters passed in (excluding buffer pointer)
	int parms = vm->PopInt() - 1;
	ASSERT( parms >= 0 );
	ASSERT( parms < ( sizeof(names)/sizeof(names[0]) ) );

	// pop variable names and put in forward order
	for ( i = 0 ; i < parms ; i++ )
		names[ parms - 1 - i ] = vm->PopString();

	// pop pointer to the buffer
	buffer = vm->PopTypeless(); 

	// check for variables to process
	if ( ( buffer == NULL ) || ( names[ 0 ] == NULL ) )
		return;

	// get the associated values of the variables and store in the buffer
	for ( i = 0 ; i < parms ; i++ )
	{
		switch ( vm->GetType( names[ i ] ) )
		{
			case ScriptVar::TYPE_INT:		buffer = PackBuffer::Pack( buffer, vm->GetInt( names[ i ] ) );		break;
			case ScriptVar::TYPE_FLOAT:	buffer = PackBuffer::Pack( buffer, vm->GetFloat( names[ i ] ) );	break;
			case ScriptVar::TYPE_STRING:	buffer = PackBuffer::Pack( buffer, vm->GetString( names[ i ] ) );	break;
		}
	}
}

//============================================================================

void Script_UnpackBuffer( ScriptVM* vm )
{
	void* buffer;
	const char* names[ 32 ];
	int i;

	// get the number of parameters passed in (excluding buffer pointer)
	int parms = vm->PopInt() - 1;
	ASSERT( parms >= 0 );
	ASSERT( parms < ( sizeof(names)/sizeof(names[0]) ) );

	// pop variable names and put in forward order
	for ( i = 0 ; i < parms ; i++ )
		names[ parms - 1 - i ] = vm->PopString();

	// pop pointer to the buffer
	buffer = vm->PopTypeless(); 

	// check for variables to process
	if ( ( buffer == NULL ) || ( names[ 0 ] == NULL ) )
		return;

	// get the associated values of the variables and store in the buffer
	for ( i = 0 ; i < parms ; i++ )
	{
		//vm->SetVar( names[ i ], vm->GetType( names[ i ]), buffer );
		switch ( vm->GetType( names[ i ] ) )
		{
			case ScriptVar::TYPE_INT:
			{
				int iValue;
				buffer = PackBuffer::Unpack( buffer, iValue );
				vm->SetVar( names[ i ], iValue );
				break;
			}
			case ScriptVar::TYPE_FLOAT:
			{
				float fValue;
				buffer = PackBuffer::Unpack( buffer, fValue );
				vm->SetVar( names[ i ], fValue );
				break;
			}
			case ScriptVar::TYPE_STRING:
			{
				char* sValue;
				buffer = PackBuffer::Unpack( buffer, sValue );
				vm->SetVar( names[ i ], sValue );
				break;
			}
		}
	}
}

//============================================================================

void Script_BufferSize( ScriptVM* vm )
{
	const char* names[ 32 ];
	int size = 0;
	int i;

	// get the number of parameters passed in
	int parms = vm->PopInt();
	ASSERT( parms >= 0 );
	ASSERT( parms < ( sizeof(names)/sizeof(names[0]) ) );

	// pop variable names and put in forward order
	for ( i = 0 ; i < parms ; i++ )
		names[ parms - 1 - i ] = vm->PopString();

	// check for variables to process
	if ( ( parms > 0 ) && ( names[ 0 ] != NULL ) )
	{
		// get the associated values of the variables and store in the buffer
		for ( i = 0 ; i < parms ; i++ )
		{
			switch ( vm->GetType( names[ i ] ) )
			{
				case ScriptVar::TYPE_INT:		size += sizeof(int);		break;
				case ScriptVar::TYPE_FLOAT:	size += sizeof(float);	break;
				case ScriptVar::TYPE_STRING:	size += sizeof(char*);	break;
			}
		}
	}

	// return size of buffer
	vm->Push( size );
}

//============================================================================

void Script_sprintf( ScriptVM* vm )
{
	static char buffer[ 4 ][ 512 ];
	static int  index = 0;
	ScriptVar args[ 32 ];
	int i;

	int parms = vm->PopInt() - 1;		// ignore format string at start
	ASSERT( parms >= 0 );
	ASSERT( parms < ( sizeof(args)/sizeof(args[0]) ) );

	// pop args, put in forward order, and remember their types
	for ( i = 0 ; i < parms ; i++ )
		args[ parms - 1 - i ] = vm->PopVar();

	// pop format string
	const char* fmt = vm->PopString();

	// build a var arg list using this data
	u64 argDataBuffer[ sizeof(args)/sizeof(args[0]) ];
	va_list varArgs = (va_list) argDataBuffer;
	for ( i = 0 ; i < parms ; i++ )
	{
		switch ( args[ i ].Type() )
		{
			case ScriptVar::TYPE_INT:    va_arg( varArgs, int    ) = args[ i ].Value().i;	break;
			case ScriptVar::TYPE_FLOAT:  va_arg( varArgs, double ) = args[ i ].Value().f;	break;
			case ScriptVar::TYPE_STRING: va_arg( varArgs, char*  ) = args[ i ].Value().p;	break;
		}
	}

	// do sprintf
	varArgs = (va_list) argDataBuffer;
	vsprintf( buffer[index], fmt, varArgs );
	ASSERT( strlen( buffer[index] ) < 512 );

	// return buffer pointer to script
	vm->Push( buffer[index] );

	// rotate our buffer
	if ( ++index >= 4 )
		index = 0;
}

//============================================================================

void Script_RegisterScriptFuncs()
{
	SCRIPT_REGISTER( ClearStacks );
	SCRIPT_REGISTER( SetMessageHandlerRules );
	SCRIPT_REGISTER( RemoveMessageHandler );
	SCRIPT_REGISTER( RouteMessageToFunction );
	SCRIPT_REGISTER( GetScriptInt );
	SCRIPT_REGISTER( GetScriptFloat );
	SCRIPT_REGISTER( PackBuffer );
	SCRIPT_REGISTER( UnpackBuffer );
	SCRIPT_REGISTER( BufferSize );
	SCRIPT_REGISTER( sprintf );
}

//============================================================================
