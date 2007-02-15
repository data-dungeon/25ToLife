//===========================================================================
//=
//= ScriptTypes.h
//=
//===========================================================================

#ifndef _SCRIPT_TYPES_H
#define _SCRIPT_TYPES_H

// token list
enum ScriptToken
{
	#define SCRIPT_TOKEN( id, string, opcode ) id,
	#include "ScriptToken.h"
	#undef SCRIPT_TOKEN
	TOKEN_COUNT,
};

// opcode list
enum _ScriptOpcodeEnum
{
	#define SCRIPT_OPCODE( op, size ) op,
	#include "ScriptOpcode.h"
	#undef SCRIPT_OPCODE
	OPCODE_COUNT
};

// script opcode
typedef unsigned short ScriptOpcode;

// generic union to access data as different types
union ScriptValue
{
	int						i;
	float						f;
	char*						p;
// 	ScriptInstruction		instr;
};

#define SCRIPT_INSTR_PACK_SWAP( a, b )  ( ( ( (a) & 0xff ) << 8 ) | ( (b) & 0xff ) )
#define SCRIPT_INSTR_DATA_SWAP0( d )    ( (d) & 0xff )
#define SCRIPT_INSTR_DATA_SWAP1( d )    ( (d) >> 8 )

#endif //_SCRIPT_TYPES_H
