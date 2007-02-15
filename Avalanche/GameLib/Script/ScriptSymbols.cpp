//===========================================================================
//=
//= ScriptSymbols.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

#ifdef SCRIPT_COMPILER

//===========================================================================

ScriptSymbols::ScriptSymbols()
{
	m_symbols    = NULL;
	Reset();
}

//===========================================================================

ScriptSymbols::~ScriptSymbols()
{
	Reset();
}

//===========================================================================

void ScriptSymbols::Reset()
{
	if ( m_symbols )
	{
		for ( int i = 0 ; i < m_numSymbols ; i++ )
		{
			if ( m_symbols[ i ].name ) delete [] m_symbols[ i ].name;
			if ( m_symbols[ i ].type == SYMBOL_CONST_STRING && m_symbols[ i ].data ) delete [] (char*)m_symbols[ i ].data;
		}
		delete m_symbols;
	}

	m_symbols             = NULL;

	m_numSymbols          = 0;
	m_maxSymbols          = 0;

	m_numOutputVariables  = 0;
	m_numOutputStatics    = 0;
	m_numOutputLabels     = 0;
	m_numOutputCFunctions = 0;

	m_strings.Reset();
}

//===========================================================================

int ScriptSymbols::AddSymbol( SymbolType type, const char* name, int data, int scope )
{
	// grow if necessary
	if ( m_numSymbols + 1 > m_maxSymbols )
	{
		m_maxSymbols = ( m_numSymbols + 1 ) * 2;
		SymbolDef* newTable = new SymbolDef[ m_maxSymbols ];
		for ( int i = 0 ; i < m_numSymbols ; i++ )
			newTable[ i ] = m_symbols[ i ];
		delete [] m_symbols;
		m_symbols = newTable;
	}

	int symbolIndex = m_numSymbols;
	m_symbols[ symbolIndex ].type      = type;
	m_symbols[ symbolIndex ].subtype   = SYMBOL_SUBTYPE_NORMAL;
	m_symbols[ symbolIndex ].name      = strdup( name );
	m_symbols[ symbolIndex ].size      = 0;
	m_symbols[ symbolIndex ].data      = data;
	m_symbols[ symbolIndex ].scope     = scope;
	m_symbols[ symbolIndex ].numIn     = 0;
	m_symbols[ symbolIndex ].numOut    = 0;
	m_symbols[ symbolIndex ].parmStart = 0;
	m_symbols[ symbolIndex ].parmEnd   = 0;
	m_symbols[ symbolIndex ].stringID  = -1;
	m_symbols[ symbolIndex ].parentFunc = -1;

	m_numSymbols++;

	return symbolIndex;
}

//===========================================================================

int ScriptSymbols::AddUniqueLabel()
{
	char buf[ 256 ];
	sprintf( buf, "LL-%04d", m_numSymbols );
	return AddSymbol( SYMBOL_LABEL, buf, 0, 0 );
}

//===========================================================================

int ScriptSymbols::FindVariableByName( const char* name, int scope, int searchParentScopes )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			switch ( m_symbols[ i ].type )
			{
				case SYMBOL_VARIABLE_INT:
				case SYMBOL_VARIABLE_FLOAT:
				case SYMBOL_VARIABLE_STRING:
					if ( m_symbols[ i ].scope == scope && strcmp( m_symbols[ i ].name, name ) == 0 )
						return i;
					break;

				case SYMBOL_SCOPE:
					if ( m_symbols[ i ].data == scope && searchParentScopes )
						parentScope = m_symbols[ i ].scope;
					break;
			}
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

int ScriptSymbols::FindFunctionByName( const char* name, int scope, int searchParentScopes )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			if ( ( m_symbols[ i ].type == SYMBOL_FUNCTION || m_symbols[ i ].type == SYMBOL_C_FUNCTION ) &&
				m_symbols[ i ].scope == scope && strcmp( m_symbols[ i ].name, name ) == 0 )
				return i;
			else if ( m_symbols[ i ].type == SYMBOL_SCOPE && m_symbols[ i ].data == scope && searchParentScopes )
				parentScope = m_symbols[ i ].scope;
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

int ScriptSymbols::FindLabelBySubtype( int subType, int scope, bool searchParentScopes, bool definedOnly )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			if ( m_symbols[ i ].type == SYMBOL_LABEL && m_symbols[ i ].scope == scope && m_symbols[ i ].subtype == subType )
			{
				if ( !definedOnly || m_symbols[ i ].data >= 0 )
					return i;
			}
			else if ( m_symbols[ i ].type == SYMBOL_SCOPE && m_symbols[ i ].data == scope && searchParentScopes )
				parentScope = m_symbols[ i ].scope;
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

int ScriptSymbols::FindConstantByName( const char* name, int scope, bool searchParentScopes )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			switch ( m_symbols[ i ].type )
			{
				case SYMBOL_CONST_INT:
				case SYMBOL_CONST_STRING:
					if ( m_symbols[ i ].scope == scope && strcmp( m_symbols[ i ].name, name ) == 0 )
						return i;
					break;

				case SYMBOL_SCOPE:
					if ( m_symbols[ i ].data == scope && searchParentScopes )
						parentScope = m_symbols[ i ].scope;
					break;
			}
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

int ScriptSymbols::FindParentSwitch( int scope )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			if ( m_symbols[ i ].type  == SYMBOL_SWITCH && m_symbols[ i ].scope == scope && m_symbols[ i ].data )
				return i;
			else if ( m_symbols[ i ].type == SYMBOL_SCOPE && m_symbols[ i ].data == scope )
				parentScope = m_symbols[ i ].scope;
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

int ScriptSymbols::FindParentFunction( int scope )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			if ( m_symbols[ i ].type == SYMBOL_SCOPE && m_symbols[ i ].data == scope )
			{
				if ( strcmp( m_symbols[ i ].name, FUNC_SCOPE ) == 0 )
				{
					int funcIndex = m_symbols[ i ].parentFunc;
					ASSERT( m_symbols[ funcIndex ].type == SYMBOL_FUNCTION );
					return funcIndex;
				}
				parentScope = m_symbols[ i ].scope;
			}
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

int ScriptSymbols::FindGotoLabelByName( const char* name, int scope, bool searchParentScopes, bool allowUnresolved )
{
	while ( scope >= 0 )
	{
		int parentScope = -1;
		for ( int i = m_numSymbols - 1 ; i >= 0 ; i-- )
		{
			if ( m_symbols[ i ].type == SYMBOL_LABEL && m_symbols[ i ].scope == scope && strcmp( m_symbols[ i ].name, name ) == 0 )
			{
				if ( m_symbols[ i ].data >= 0 || allowUnresolved )
					return i;
			}
			else if ( searchParentScopes &&
						 m_symbols[ i ].type == SYMBOL_SCOPE &&
//				       strcmp( m_symbols[ i ].name, FUNC_SCOPE ) != 0 &&	// don't allow goto's to hop outside of a function
						 m_symbols[ i ].data == scope )
			{
				parentScope = m_symbols[ i ].scope;
			}
		}
		scope = parentScope;
	}
	return -1;
}

//===========================================================================

void ScriptSymbols::Print()
{
	for ( int i = 0 ; i < m_numSymbols ; i++ )
	{
		if ( m_symbols[ i ].type == SYMBOL_CONST_STRING )
		{
			printf( "Symbol %3d : n=%-20s t=%-10s data='%s' scope=%-3d size=%-3d\n", i, m_symbols[ i ].name, SymbolTypeName( m_symbols[ i ].type ),
						(char*)m_symbols[ i ].data, m_symbols[ i ].scope, m_symbols[ i ].size );
		}
		else
		{
			printf( "Symbol %3d : n=%-20s t=%-10s data=%-3d scope=%-3d size=%-3d\n", i, m_symbols[ i ].name, SymbolTypeName( m_symbols[ i ].type ),
						m_symbols[ i ].data, m_symbols[ i ].scope, m_symbols[ i ].size );
		}
	}
}

//===========================================================================

const char* ScriptSymbols::SymbolTypeName( SymbolType type )
{
	switch ( type )
	{
		case SYMBOL_SCOPE:				return "SCOPE";
		case SYMBOL_LABEL:				return "LABEL";
		case SYMBOL_VARIABLE_INT:		return "INT";
		case SYMBOL_VARIABLE_FLOAT:	return "FLOAT";
		case SYMBOL_VARIABLE_STRING:	return "STRING";
		case SYMBOL_FUNCTION:			return "FUNC";
		case SYMBOL_C_FUNCTION:			return "C FUNC";
		case SYMBOL_CONST_INT:			return "CONST INT";
		case SYMBOL_CONST_STRING:		return "CONST STRING";
		case SYMBOL_STRUCT:				return "STRUCT";
		default:								return "Unknown";
	}
}

//===========================================================================

#endif //SCRIPT_COMPILER
