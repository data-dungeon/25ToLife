//===========================================================================
//=
//= ScriptParseTree.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

#ifdef SCRIPT_COMPILER

//===========================================================================

ScriptParseTree::ScriptParseTree( ScriptToken token, int line, const char* file )
{
	m_token      = token;
	m_value.i    = 0;
	m_child[ 0 ] = NULL;
	m_child[ 1 ] = NULL;
	m_child[ 2 ] = NULL;
	m_child[ 3 ] = NULL;
	m_line       = line;
	m_file       = file;
}

//===========================================================================

ScriptParseTree::~ScriptParseTree()
{
	if ( m_child[ 0 ] ) { delete m_child[ 0 ]; m_child[ 0 ] = NULL; }
	if ( m_child[ 1 ] ) { delete m_child[ 1 ]; m_child[ 1 ] = NULL; }
	if ( m_child[ 2 ] ) { delete m_child[ 2 ]; m_child[ 2 ] = NULL; }
	if ( m_child[ 3 ] ) { delete m_child[ 3 ]; m_child[ 3 ] = NULL; }

	switch( m_token )
	{
		case TOKEN_LABEL:
		case TOKEN_IDENT:
		case TOKEN_FUNC_DECL:
		case TOKEN_STRING:
		case TOKEN_STRUCT_DECL:
			if ( m_value.p )
				delete m_value.p;
			break;

		default:
			break;
	}
}

//===========================================================================

void ScriptParseTree::PrintToken()
{
	if      ( m_token == TOKEN_CONST_INT   ) { printf( "I:%d", m_value.i ); }
	else if ( m_token == TOKEN_CONST_FLOAT ) { printf( "F:%f", m_value.f ); }
	else if ( m_token == TOKEN_IDENT       ) { printf( "V:%s", m_value.p ); }
	else if ( m_token == TOKEN_LABEL       ) { printf( "L:%s", m_value.p ); }
	else if ( m_token == TOKEN_FUNC_DECL   ) { printf( "[FUNC DEFINE]" ); }
	else if ( m_token == TOKEN_STRING      ) { printf( "[STRING CONST:%s]", m_value.p ); }
	else if ( m_token == TOKEN_SCOPE_BEGIN ) { printf( "[SCOPE:%d]", m_value.i ); }
	else if ( m_token == TOKEN_CASE        ) { printf( "[CASE:%d]", m_value.i ); }
	else                                     { printf( "[%s]", ScriptUtil::TokenName( m_token ) ); }
}

//===========================================================================

void ScriptParseTree::Print( int level )
{
	for ( int i = 0 ; i < level ; i++ )
		printf( "   " );
	PrintToken();

	printf( "\n" );

	if ( m_token == TOKEN_GROUP )
	{
		ScriptParseTree* node = this;
		while ( node && node->m_token == TOKEN_GROUP )
		{
			if ( node->m_child[ 0 ] ) { node->m_child[ 0 ]->Print( level + 1 ); }
			node = node->m_child[ 1 ];
		}
		if ( node )
			node->Print( level + 1 );
	}
	else
	{
		if ( m_child[ 0 ] ) { m_child[ 0 ]->Print( level + 1 ); }
		if ( m_child[ 1 ] ) { m_child[ 1 ]->Print( level + 1 ); }
		if ( m_child[ 2 ] ) { m_child[ 2 ]->Print( level + 1 ); }
		if ( m_child[ 3 ] ) { m_child[ 3 ]->Print( level + 1 ); }
	}
}

//===========================================================================

void ScriptParseTree::SetConst( float value )
{
	m_value.f = value;
	m_token   = TOKEN_CONST_FLOAT;
}

//===========================================================================

void ScriptParseTree::SetConst( int value )
{
	m_value.i = value;
	m_token   = TOKEN_CONST_INT;
}

//===========================================================================

void ScriptParseTree::ReduceUnary()
{
	bool reduce = false;

	if ( m_child[0]->m_token == TOKEN_CONST_INT )
	{
		int i1 = m_child[ 0 ]->m_value.i;
		switch ( m_token )
		{
			case TOKEN_UNARY_MINUS:  SetConst( -i1 ); reduce = true; break;
			case TOKEN_LOGICAL_NOT:	 SetConst( !i1 ); reduce = true; break;
			case TOKEN_BITWISE_NOT:	 SetConst( ~i1 ); reduce = true; break;
			case TOKEN_ABS:			 SetConst( i1 < 0 ? -i1 : i1 ); reduce = true; break;
			default: break;
		}
	}
	else if ( m_child[0]->m_token == TOKEN_CONST_FLOAT )
	{
		float f1 = m_child[ 0 ]->m_value.f;
		switch ( m_token )
		{
			case TOKEN_UNARY_MINUS:  SetConst( -f1 ); reduce = true; break;
			case TOKEN_LOGICAL_NOT:	 SetConst( !f1 ); reduce = true; break;
			case TOKEN_ABS:          SetConst( f1 < 0.0f ? -f1 : f1 ); reduce = true; break;
			default: break;
		}
	}

	if ( reduce )
	{
		delete m_child[ 0 ];
		m_child[ 0 ] = NULL;
	}
}

//===========================================================================

void ScriptParseTree::ReduceBinary()
{
	bool reduce = false;

	int n1 = 0;
	int n2 = 0;
	if      ( m_child[ 0 ]->m_token == TOKEN_CONST_INT   ) { n1 = 1; }
	else if ( m_child[ 0 ]->m_token == TOKEN_CONST_FLOAT ) { n1 = 2; }
	if      ( m_child[ 1 ]->m_token == TOKEN_CONST_INT   ) { n2 = 1; }
	else if ( m_child[ 1 ]->m_token == TOKEN_CONST_FLOAT ) { n2 = 2; }

	if ( n1 == 1 && n2 == 1 )
	{
		int i1 = m_child[ 0 ]->m_value.i;
		int i2 = m_child[ 1 ]->m_value.i;
		switch ( m_token )
		{
			case TOKEN_PLUS:            SetConst( i1 + i2 );  reduce = true; break;
			case TOKEN_MINUS:           SetConst( i1 - i2 );  reduce = true; break;
			case TOKEN_MULT:            SetConst( i1 * i2 );  reduce = true; break;
			case TOKEN_DIV:             SetConst( i1 / i2 );  reduce = true; break;
			case TOKEN_MOD:             SetConst( i1 % i2 );  reduce = true; break;
			case TOKEN_LESS_THAN:       SetConst( i1 < i2 );  reduce = true; break;
			case TOKEN_LESS_THAN_EQUAL: SetConst( i1 <= i2 ); reduce = true; break;
			case TOKEN_GTR_THAN:        SetConst( i1 > i2 );  reduce = true; break;
			case TOKEN_GTR_THAN_EQUAL:  SetConst( i1 >= i2 ); reduce = true; break;
			case TOKEN_EQUALS:          SetConst( i1 == i2 ); reduce = true; break;
			case TOKEN_NOT_EQUALS:      SetConst( i1 != i2 ); reduce = true; break;
			case TOKEN_LOGICAL_AND:     SetConst( i1 && i2 ); reduce = true; break;
			case TOKEN_LOGICAL_OR:      SetConst( i1 || i2 ); reduce = true; break;
			case TOKEN_BITWISE_AND:     SetConst( i1 & i2 );  reduce = true; break;
			case TOKEN_BITWISE_OR:      SetConst( i1 | i2 );  reduce = true; break;
			case TOKEN_BITWISE_XOR:     SetConst( i1 ^ i2 );  reduce = true; break;
			case TOKEN_SHIFT_L:         SetConst( i1 << i2 ); reduce = true; break;
			case TOKEN_SHIFT_R:         SetConst( i1 >> i2 ); reduce = true; break;
			default: break;
		}
	}
	else if ( n1 && n2 )
	{
		float f1 = ( n1 == 1 ) ? m_child[ 0 ]->m_value.i : m_child[ 0 ]->m_value.f;
		float f2 = ( n2 == 1 ) ? m_child[ 1 ]->m_value.i : m_child[ 1 ]->m_value.f;
		switch ( m_token )
		{
			case TOKEN_PLUS:            SetConst( f1 + f2 );  reduce = true; break;
			case TOKEN_MINUS:           SetConst( f1 - f2 );  reduce = true; break;
			case TOKEN_MULT:            SetConst( f1 * f2 );  reduce = true; break;
			case TOKEN_DIV:             SetConst( f1 / f2 );  reduce = true; break;
			case TOKEN_LESS_THAN:       SetConst( f1 < f2 );  reduce = true; break;
			case TOKEN_LESS_THAN_EQUAL: SetConst( f1 <= f2 ); reduce = true; break;
			case TOKEN_GTR_THAN:        SetConst( f1 > f2 );  reduce = true; break;
			case TOKEN_GTR_THAN_EQUAL:  SetConst( f1 >= f2 ); reduce = true; break;
			case TOKEN_EQUALS:          SetConst( f1 == f2 ); reduce = true; break;
			case TOKEN_NOT_EQUALS:      SetConst( f1 != f2 ); reduce = true; break;
			case TOKEN_LOGICAL_AND:     SetConst( f1 && f2 ); reduce = true; break;
			case TOKEN_LOGICAL_OR:      SetConst( f1 || f2 ); reduce = true; break;
			default: break;
		}
	}

	if ( reduce )
	{
		delete m_child[ 0 ];
		delete m_child[ 1 ];
		m_child[ 0 ] = NULL;
		m_child[ 1 ] = NULL;
	}
}

//===========================================================================

void ScriptParseTree::ReduceTernary()
{
	if ( m_token == TOKEN_COND_EXPR )
	{
		bool eval;
		bool isConst = false;
		if      ( m_child[ 0 ]->m_token == TOKEN_CONST_INT   ) { eval = ( m_child[ 0 ]->m_value.i != 0    ); isConst = true; }
		else if ( m_child[ 0 ]->m_token == TOKEN_CONST_FLOAT ) { eval = ( m_child[ 0 ]->m_value.f != 0.0f ); isConst = true; }
		if ( isConst )
		{
			// erase the condition tree
			delete m_child[ 0 ]; m_child[ 0 ] = NULL;

			if ( eval )
			{
				delete m_child[ 2 ]; m_child[ 2 ] = NULL;
				BecomeTree( m_child[ 1 ] );
			}
			else
			{
				delete m_child[ 1 ]; m_child[ 1 ] = NULL;
				BecomeTree( m_child[ 2 ] );
			}
		}
	}
}

//===========================================================================

void ScriptParseTree::ReduceLogical()
{
	if ( m_token == TOKEN_LOGICAL_AND || m_token == TOKEN_LOGICAL_OR )
	{
		bool eval;
		bool isConst = false;
		if      ( m_child[ 0 ]->m_token == TOKEN_CONST_INT   ) { eval = ( m_child[ 0 ]->m_value.i != 0    ); isConst = true; }
		else if ( m_child[ 0 ]->m_token == TOKEN_CONST_FLOAT ) { eval = ( m_child[ 0 ]->m_value.f != 0.0f ); isConst = true; }
		if ( isConst )
		{
			if ( ( m_token == TOKEN_LOGICAL_AND && !eval ) || ( m_token == TOKEN_LOGICAL_OR && eval ) )
			{
				// because of early termination on and's and or's, this entire node is evaluated by the left constant
				SetConst( eval );
				delete m_child[ 0 ];	m_child[ 0 ] = NULL;
				delete m_child[ 1 ]; m_child[ 1 ] = NULL;
			}
			else
			{
				// we don't need to ever evaulate the left constant, so copy the right tree into this node and kill the left.
				delete m_child[ 0 ]; m_child[ 0 ] = NULL;
				BecomeTree( m_child[ 1 ] );
			}
		}
	}
}

//===========================================================================

void ScriptParseTree::Simplify()
{
	if ( m_child[ 0 ] ) { m_child[ 0 ]->Simplify(); }
	if ( m_child[ 1 ] ) { m_child[ 1 ]->Simplify(); }
	if ( m_child[ 2 ] ) { m_child[ 2 ]->Simplify(); }
	if ( m_child[ 3 ] ) { m_child[ 3 ]->Simplify(); }

	// try reducing terms
	if ( m_child[0] && !m_child[1] && !m_child[2] && !m_child[3] )
		ReduceUnary();
	else if ( m_child[0] && m_child[1] && !m_child[2] && !m_child[3] )
		ReduceBinary();
	else if ( m_child[0] && m_child[1] && m_child[2] && !m_child[3] )
		ReduceTernary();

	// logical reduction
	ReduceLogical();
}

//===========================================================================

ScriptParseTree* ScriptParseTree::Copy()
{
	ScriptParseTree* copy = new ScriptParseTree( m_token, m_line, m_file );
	copy->m_value = m_value;

	if ( m_child[ 0 ] ) { copy->m_child[ 0 ] = m_child[ 0 ]->Copy(); }
	if ( m_child[ 1 ] ) { copy->m_child[ 1 ] = m_child[ 1 ]->Copy(); }
	if ( m_child[ 2 ] ) { copy->m_child[ 2 ] = m_child[ 2 ]->Copy(); }
	if ( m_child[ 3 ] ) { copy->m_child[ 3 ] = m_child[ 3 ]->Copy(); }

	return copy;
}

//===========================================================================

int ScriptParseTree::Count( ScriptToken token )
{
	int count = 0;
	if ( m_token == token ) count++;
	if ( m_child[ 0 ] )     count += m_child[ 0 ]->Count( token );
	if ( m_child[ 1 ] )     count += m_child[ 1 ]->Count( token );
	if ( m_child[ 2 ] )     count += m_child[ 2 ]->Count( token );
	if ( m_child[ 3 ] )     count += m_child[ 3 ]->Count( token );
	return count;
}

//===========================================================================

void ScriptParseTree::BecomeTree( ScriptParseTree* src )
{
	// duplicate all values into us from source tree
	m_token      = src->m_token;
	m_value      = src->m_value;
	m_line       = src->m_line;
	m_file       = src->m_file;
	m_child[ 0 ] = src->m_child[ 0 ];
	m_child[ 1 ] = src->m_child[ 1 ];
	m_child[ 2 ] = src->m_child[ 2 ];
	m_child[ 3 ] = src->m_child[ 3 ];

	// make the guy we just copied from inert, so we can kill it without any side effects
	src->m_token = TOKEN_NONE;
	src->m_child[ 0 ] = src->m_child[ 1 ] = src->m_child[ 2 ] = src->m_child[ 3 ] = NULL;
	delete src;
}

//===========================================================================

#endif //SCRIPT_COMPILER