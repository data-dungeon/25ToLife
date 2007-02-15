//===========================================================================
//=
//= ScriptUtil.h
//=
//===========================================================================

#ifndef _SCRIPT_UTIL_H
#define _SCRIPT_UTIL_H

#ifdef SCRIPT_COMPILER

#include "ScriptTypes.h"

class ScriptUtil
{
public:
	//===========================================================================

	static const char* TokenName( ScriptToken token )
	{
		static const char* names[ TOKEN_COUNT ] =
		{
			#define SCRIPT_TOKEN( id, string, opcode ) string,
			#include "ScriptToken.h"
			#undef SCRIPT_TOKEN
		};

		if ( token < TOKEN_COUNT )
			return names[ token ];
		else
			return "*Unknown*";
	}

	//===========================================================================

	static ScriptOpcode TokenOpcode( ScriptToken token )
	{
		static const ScriptOpcode opcodes[ TOKEN_COUNT ] =
		{
			#define SCRIPT_TOKEN( id, string, opcode ) opcode,
			#include "ScriptToken.h"
			#undef SCRIPT_TOKEN
		};

		if ( token < TOKEN_COUNT )
			return opcodes[ token ];
		else
			return OPCODE_NOP;
	}

	//===========================================================================

	static bool TokenIsAssignment( ScriptToken token )
	{
		switch ( token )
		{
			case TOKEN_ASSIGN:
			case TOKEN_PLUS_EQUALS:
			case TOKEN_MINUS_EQUALS:
			case TOKEN_MULT_EQUALS:
			case TOKEN_DIV_EQUALS:
			case TOKEN_MOD_EQUALS:
			case TOKEN_SHIFT_L_EQUALS:
			case TOKEN_SHIFT_R_EQUALS:
			case TOKEN_AND_EQUALS:
			case TOKEN_OR_EQUALS:
			case TOKEN_XOR_EQUALS:
				return true;
			default:
				return false;
		}
	}

	//===========================================================================

	static const char* OpcodeName( ScriptOpcode opcode )
	{
		static const char* names[ OPCODE_COUNT ] =
		{
			#define SCRIPT_OPCODE( id, size ) #id,
			#include "ScriptOpcode.h"
			#undef SCRIPT_OPCODE
		};

		if ( opcode < OPCODE_COUNT )
			return names[ opcode ];
		else
			return "*Unknown*";
	}

	//===========================================================================

	static int OpcodeSize( ScriptOpcode opcode )
	{
		static const char size[ OPCODE_COUNT ] =
		{
			#define SCRIPT_OPCODE( id, size ) size,
			#include "ScriptOpcode.h"
			#undef SCRIPT_OPCODE
		};

		if ( opcode < OPCODE_COUNT )
			return size[ opcode ];
		else
			return 0;
	}
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_UTIL_H
