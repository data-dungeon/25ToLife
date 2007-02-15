//===========================================================================
//=
//= ScriptParse.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

#ifdef SCRIPT_COMPILER

#undef TEST_ERROR
#undef ERROR
#define TEST_ERROR		if ( m_err.ErrorCount() ) goto Error
#define ERROR( msg )		{ m_err.Error( m_lex.GetFile(), m_lex.GetLine(), msg ); goto Error; }

//===========================================================================

ScriptParse::ScriptParse( ScriptLex& lex, ScriptSymbols& syms, ScriptError& err ) :
	m_lex( lex ),
	m_syms( syms ),
	m_err( err )
{
	m_verbose            = false;
	m_numScopes          = 0;
	m_currentScope       = 0;
	m_preprocessorMode	= false;
}

//===========================================================================

ScriptParse::~ScriptParse()
{
}

//===========================================================================

void ScriptParse::SaveState( ParseState* state )
{
	m_lex.SaveState( &state->lexState );
	state->token = m_token;
}

//===========================================================================

void ScriptParse::RestoreState( ParseState* state )
{
	m_lex.RestoreState( &state->lexState );
	m_token = state->token;
	if ( m_verbose ) { printf( "\n<rewind, token=%s '%s'> ", ScriptUtil::TokenName( m_token ), m_lex.TokenText() ); }
}

//===========================================================================

ScriptParseTree* ScriptParse::Parse()
{
	ScriptParseTree* tree = NULL;

	// feed the very first token
	NextToken();								TEST_ERROR;

	if ( m_token != TOKEN_NONE )
	{
		tree = ParseStatementList();		TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParsePreprocessorExpr()
{
	ScriptParseTree* tree = NULL;

	m_preprocessorMode = true;

	// feed the very first token
	NextToken(); TEST_ERROR;

	tree = ParseExprConstant(); TEST_ERROR;

	if ( tree )
		tree->Simplify();

	m_preprocessorMode = false;

	return tree;

Error:
	m_preprocessorMode = false;
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

void ScriptParse::Match( ScriptToken token )
{
	if ( m_token != token )
	{
		m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "missing '%s' before %s '%s'",
			ScriptUtil::TokenName( token ), ScriptUtil::TokenName( m_token ), m_lex.TokenText() );
		return;
	}

	NextToken();
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementList()
{
	ScriptParseTree* tree = NULL;
	ScriptParseTree* tail = NULL;
	ScriptParseTree* statement;

	statement = ParseStatement();		TEST_ERROR;
	while ( statement )
	{
		ScriptParseTree* group = new ScriptParseTree( TOKEN_GROUP, m_lex.GetLine(), m_lex.GetFile() );
		group->m_child[ 0 ]    = statement;

		if ( tail )	tail->m_child[ 1 ] = group;
		else		   tree = group;
		tail = group;

		statement = ParseStatement();		TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatement()
{
	ScriptParseTree* tree = NULL;

	if ( m_token != TOKEN_NONE )
	{
		tree = ParseFunctionDeclaration();	TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementListAssign();	TEST_ERROR;	 if ( tree ) return tree;
		tree = ParseLabel();						TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementSelection();	TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementCompound();		TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementIteration();	TEST_ERROR;  if ( tree ) return tree;
		tree = ParseEnum();						TEST_ERROR;  if ( tree ) return tree;
		tree = ParseVariableDeclaration();	TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementJump();			TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementPrint();			TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementSleep();			TEST_ERROR;  if ( tree ) return tree;
		tree = ParseStatementExpr();			TEST_ERROR;  if ( tree ) return tree;
	}
	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementSelection()
{
	ScriptParseTree* tree = NULL;
	if ( m_token == TOKEN_IF )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();									TEST_ERROR;
		Match( TOKEN_PAREN_L );						TEST_ERROR;
		tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )				ERROR( "if missing expression" );
		Match( TOKEN_PAREN_R );						TEST_ERROR;
		tree->m_child[ 1 ] = ParseStatement();	TEST_ERROR;
		if ( ! tree->m_child[ 1 ] )				ERROR( "if missing statement" );
		if ( m_token == TOKEN_ELSE )
		{
			NextToken();									TEST_ERROR;
			tree->m_child[ 2 ] = ParseStatement();	TEST_ERROR;
			if ( ! tree->m_child[ 2 ] )				ERROR( "if missing valid statement after else" );
		}
	}
	else if ( m_token == TOKEN_SWITCH )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();									TEST_ERROR;
		Match( TOKEN_PAREN_L );						TEST_ERROR;
		tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )				ERROR( "switch missing expression" );
		Match( TOKEN_PAREN_R );						TEST_ERROR;
		tree->m_child[ 1 ] = ParseStatementCompound(); TEST_ERROR;
		if ( ! tree->m_child[ 1 ] )				ERROR( "switch statement missing body" );
	}
	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementCompound()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_BRACE_L )
	{
		tree = new ScriptParseTree( TOKEN_SCOPE_BEGIN, m_lex.GetLine(), m_lex.GetFile() );
		int parentScope = m_currentScope;
		m_currentScope  = ++m_numScopes;
		tree->m_value.i = m_currentScope;
		m_syms.AddSymbol( ScriptSymbols::SYMBOL_SCOPE, NORMAL_SCOPE, m_currentScope, parentScope );
		NextToken();												TEST_ERROR;
		tree->m_child[ 0 ] = ParseStatementList();		TEST_ERROR;
		Match( TOKEN_BRACE_R );									TEST_ERROR;
		m_currentScope = parentScope;
	}
	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementIteration()
{
	ScriptParseTree* tree = NULL;
	if ( m_token == TOKEN_WHILE )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();											TEST_ERROR;

		Match( TOKEN_PAREN_L );								TEST_ERROR;
		tree->m_child[ 0 ] = ParseExpr();				TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )						ERROR( "while missing expression" );
		Match( TOKEN_PAREN_R );								TEST_ERROR;

		tree->m_child[ 1 ] = ParseStatement();			TEST_ERROR;
		if ( ! tree->m_child[ 1 ] )						ERROR( "while missing statement" );
	}
	else if ( m_token == TOKEN_DO )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();											TEST_ERROR;
		tree->m_child[ 0 ] = ParseStatement();			TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )						ERROR( "do/while missing statement" );
		Match( TOKEN_WHILE );								TEST_ERROR;
		Match( TOKEN_PAREN_L );								TEST_ERROR;
		tree->m_child[ 1 ] = ParseExpr();				TEST_ERROR;
		if ( ! tree->m_child[ 1 ] )						ERROR( "do/while missing expression after while" );
		Match( TOKEN_PAREN_R );								TEST_ERROR;
		Match( TOKEN_SEMICOLON );							TEST_ERROR;
		return tree;
	}
	else if ( m_token == TOKEN_FOR )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();											TEST_ERROR;

		Match( TOKEN_PAREN_L );								TEST_ERROR;

		if ( m_token != TOKEN_SEMICOLON )
		{
			tree->m_child[ 0 ] = ParseExpr();			TEST_ERROR;
			if ( ! tree->m_child[ 0 ] )					ERROR( "for initializer bogus" );
		}

		Match( TOKEN_SEMICOLON );							TEST_ERROR;

		if ( m_token != TOKEN_SEMICOLON )
		{
			tree->m_child[ 1 ] = ParseExpr();			TEST_ERROR;
			if ( ! tree->m_child[ 1 ] )					ERROR( "for condition bogus" );
		}

		Match( TOKEN_SEMICOLON );							TEST_ERROR;

		if ( m_token != TOKEN_PAREN_R )
		{
			tree->m_child[ 2 ] = ParseExpr();			TEST_ERROR;
			if ( ! tree->m_child[ 2 ] )					ERROR( "for incrementor bogus" );
		}

		Match( TOKEN_PAREN_R );								TEST_ERROR;

		tree->m_child[ 3 ] = ParseStatement();			TEST_ERROR;
		if ( ! tree->m_child[ 3 ] )						ERROR( "for missing statement" );
	}
	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementExpr()
{
	ScriptParseTree* tree = NULL;

	if ( m_token != TOKEN_SEMICOLON )
	{
		tree = new ScriptParseTree( TOKEN_STATEMENT_EXPR, m_lex.GetLine(), m_lex.GetFile() );
		tree->m_child[ 0 ] = ParseExpr();				TEST_ERROR;
		if ( tree->m_child[ 0 ] )
		{
			Match( TOKEN_SEMICOLON );						TEST_ERROR;
		}
		else
		{
			delete tree;
			tree = NULL;
		}
	}
	else
	{
		tree = new ScriptParseTree( TOKEN_NONE, m_lex.GetLine(), m_lex.GetFile() );		// null tree
		NextToken(); TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementJump()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_GOTO )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();										TEST_ERROR;
		if ( m_token != TOKEN_IDENT )					ERROR( "goto is missing identifier" );
		tree->m_child[ 0 ] = new ScriptParseTree( TOKEN_IDENT, m_lex.GetLine(), m_lex.GetFile() );
		tree->m_child[ 0 ]->m_value.p = strdup( m_lex.TokenText() );
		NextToken();										TEST_ERROR;
		Match( TOKEN_SEMICOLON );						TEST_ERROR;
	}
	else if ( m_token == TOKEN_RETURN )
	{
		int funcIndex = m_syms.FindParentFunction( m_currentScope );
		if ( funcIndex < 0 )
		{
			m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "must use 'return' inside function declaration" );
			goto Error;
		}

		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		tree->m_value.i = funcIndex;

		NextToken();										TEST_ERROR;
		if ( m_token != TOKEN_SEMICOLON )
		{
			tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
			if ( ! tree->m_child[ 0 ] )				ERROR( "expecting expression after return" );
		}
		Match( TOKEN_SEMICOLON );						TEST_ERROR;
	}
	else if ( m_token == TOKEN_BREAK || m_token == TOKEN_CONTINUE )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken(); TEST_ERROR;
		Match( TOKEN_SEMICOLON ); TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementListAssign()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_PAREN_L )
	{
		tree = ParseExpr();									TEST_ERROR;

		if ( tree )
		{
			ScriptParseTree* newNode = new ScriptParseTree( TOKEN_LIST_ASSIGN, m_lex.GetLine(), m_lex.GetFile() );
			newNode->m_child[ 0 ] = tree;
			tree = newNode;

			Match( TOKEN_ASSIGN );								TEST_ERROR;

			tree->m_child[ 1 ] = ParseExpr();				TEST_ERROR;
			if ( ! tree->m_child[ 1 ] )
			{
				// perhaps it's a function call, look there.
				tree->m_child[ 1 ] = ParseExpr();			TEST_ERROR;
				if ( !tree->m_child[ 1 ] )						ERROR( "list assign missing valid right hand side" );
			}

			Match( TOKEN_SEMICOLON );							TEST_ERROR;
		}
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementPrint()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_PRINT )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();									TEST_ERROR;

		tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )				ERROR( "print missing expression" );
		Match( TOKEN_SEMICOLON );					TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStatementSleep()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_SLEEP )
	{
		tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();									TEST_ERROR;
		tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )				ERROR( "print missing expression" );
		Match( TOKEN_SEMICOLON );					TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseLabel()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_IDENT )
	{
		ParseState state;
		SaveState( &state );

		tree = new ScriptParseTree( TOKEN_LABEL, m_lex.GetLine(), m_lex.GetFile() );
		tree->m_value.p = strdup( m_lex.TokenText() );

		NextToken();								TEST_ERROR;
		if ( m_token == TOKEN_COLON )
		{
			NextToken();							TEST_ERROR;
		}
		else
		{
			RestoreState( &state );
			delete tree;
			tree = NULL;
		}
	}
	else if ( m_token == TOKEN_CASE )
	{
		tree = new ScriptParseTree( TOKEN_CASE, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();								TEST_ERROR;
		tree->m_child[ 0 ] = ParseExpr();	TEST_ERROR;
		if ( ! tree->m_child[ 0 ] )			ERROR( "case missing constant int" );
		tree->m_child[ 0 ]->Simplify();		TEST_ERROR;
		if ( tree->m_child[ 0 ]->m_token != TOKEN_CONST_INT )	ERROR( "case missing constant int" );
		tree->m_value.i = tree->m_child[ 0 ]->m_value.i;	// copy int value into this node
		delete tree->m_child[ 0 ];
		tree->m_child[ 0 ] = NULL;
		Match( TOKEN_COLON );					TEST_ERROR;
	}
	else if ( m_token == TOKEN_DEFAULT )
	{
		tree = new ScriptParseTree( TOKEN_DEFAULT, m_lex.GetLine(), m_lex.GetFile() );
		NextToken();								TEST_ERROR;
		Match( TOKEN_COLON );					TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseEnum()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_ENUM )
	{
		tree = new ScriptParseTree( TOKEN_NONE, m_lex.GetLine(), m_lex.GetFile() );	// null tree for codegen
		NextToken();							TEST_ERROR;
		if ( m_token == TOKEN_IDENT )
		{
			// we don't use the ident (for now)
			NextToken();						TEST_ERROR;
		}

		if ( m_token == TOKEN_BRACE_L )
		{
			NextToken();						TEST_ERROR;
			ParseEnumList();					TEST_ERROR;
			Match( TOKEN_BRACE_R );			TEST_ERROR;
		}

		Match( TOKEN_SEMICOLON );			TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseEnumList()
{
	ScriptParseTree* temp = NULL;
	int enumValue = 0;

	while ( m_token == TOKEN_IDENT )
	{
		// make sure this identifer doesn't already exist
		if ( m_syms.FindConstantByName( m_lex.TokenText(), m_currentScope, true ) >= 0 )
		{
			m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "enum identifier '%s' already defined", m_lex.TokenText() );
			goto Error;
		}

		char buf[ 256 ];
		strcpy( buf, m_lex.TokenText() );
		NextToken();											TEST_ERROR;

		if ( m_token == TOKEN_ASSIGN )
		{
			NextToken();										TEST_ERROR;
			temp = ParseExprConstant();					TEST_ERROR;
			if ( ! temp )										ERROR( "need expression after = in enum" );
			temp->Simplify();
			if ( temp->m_token != TOKEN_CONST_INT )	ERROR( "need constant int expression after = in enum" );
			enumValue = temp->m_value.i;
			delete temp; temp = NULL;
		}

		m_syms.AddSymbol( ScriptSymbols::SYMBOL_CONST_INT, buf, enumValue, m_currentScope );
		enumValue++;

		if ( m_token == TOKEN_COMMA )
		{
			NextToken();										TEST_ERROR;
		}
	}

	return NULL;

Error:
	if ( temp ) delete temp;
	return NULL;
}

//===========================================================================

int ScriptParse::DeclareVariable( ScriptToken type, ScriptParseTree* tree, bool staticVar )
{
	ASSERT( tree );
	ASSERT( tree->m_token == TOKEN_IDENT );

	// check that variable doesn't already exist
	int index = m_syms.FindVariableByName( tree->m_value.p, m_currentScope, false );
	if ( index >= 0 )
	{
		m_err.Error( m_lex.GetFile(), tree->m_line, "variable '%s' already defined in this scope", tree->m_value.p );
		return -1;
	}

	// add variable
	ScriptSymbols::SymbolType symType = ( type == TOKEN_INT_DECL )   ? ScriptSymbols::SYMBOL_VARIABLE_INT   :
													( type == TOKEN_FLOAT_DECL ) ? ScriptSymbols::SYMBOL_VARIABLE_FLOAT :
																								ScriptSymbols::SYMBOL_VARIABLE_STRING;

	// array dimension
	int size = 1;
	if ( tree->m_child[ 0 ] && tree->m_child[ 0 ]->m_token == TOKEN_CONST_INT && tree->m_child[ 0 ]->m_value.i > 0 )
		size = tree->m_child[ 0 ]->m_value.i;

	if ( !staticVar )
	{
		index = m_syms.AddSymbol( symType, tree->m_value.p, m_syms.NumOutputVariables(), m_currentScope );
		m_syms.IncOutputVariables( size );
	}
	else
	{
		index = m_syms.AddSymbol( symType, tree->m_value.p, -1 - m_syms.NumOutputStatics(), m_currentScope );
		m_syms.IncOutputStatics( size );
	}

	m_syms[ index ].stringID = m_syms.Strings().Add( tree->m_value.p );
	m_syms[ index ].size     = size;

	return index;
}

//===========================================================================

void ScriptParse::DeclareFunctionParameters( ScriptParseTree* tree )
{
	if ( tree )
	{
		if ( tree->m_token == TOKEN_INT_DECL || tree->m_token == TOKEN_FLOAT_DECL || tree->m_token == TOKEN_STRING_DECL )
		{
			DeclareVariable( tree->m_token, tree->m_child[ 0 ], false );
		}
		else
		{
			DeclareFunctionParameters( tree->m_child[ 0 ] );
			DeclareFunctionParameters( tree->m_child[ 1 ] );
			DeclareFunctionParameters( tree->m_child[ 2 ] );
			DeclareFunctionParameters( tree->m_child[ 3 ] );
		}
	}
}

//===========================================================================

int ScriptParse::CountParameters( ScriptParseTree* node )
{
	int count = 0;

	if ( node )
	{
		count += CountParameters( node->m_child[ 0 ] );
		count += CountParameters( node->m_child[ 1 ] );
		count += CountParameters( node->m_child[ 2 ] );
		count += CountParameters( node->m_child[ 3 ] );

		if ( node->m_token == TOKEN_INT_DECL || node->m_token == TOKEN_FLOAT_DECL || node->m_token == TOKEN_STRING_DECL )
		{
			if ( node->m_child[ 0 ] && node->m_child[ 0 ]->m_child[ 0 ] )
			{
				ScriptParseTree* sizeNode = node->m_child[ 0 ]->m_child[ 0 ];
				if ( sizeNode->m_token == TOKEN_CONST_INT && sizeNode->m_value.i > 0 )
				{
					count += sizeNode->m_value.i;
				}
			}
			else
			{
				count++;
			}
		}
		else if ( node->m_token == TOKEN_DOTDOTDOT )
		{
			count |= 0x1000;
		}
	}
	return count;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseFunctionDeclaration()
{
	ScriptParseTree*	tree			  = NULL;
	ScriptParseTree*	inParameters  = NULL;
	ScriptParseTree*	outParameters = NULL;
	char*					funcName      = NULL;
	bool					isExternFunc  = false;

	// is extern func?
	if ( m_token == TOKEN_EXTERN )
	{
		NextToken();	TEST_ERROR;
		isExternFunc = true;
	}

	// function declaration that returns more than one parameter?
	if ( m_token == TOKEN_PAREN_L )
	{
		ParseState state;
		SaveState( &state );

		NextToken();	TEST_ERROR;

		if ( m_token == TOKEN_INT_DECL || m_token == TOKEN_FLOAT_DECL || m_token == TOKEN_STRING_DECL )
		{
			// return parameters
			outParameters = ParseFunctionParameterDefinition(); TEST_ERROR;
			Match( TOKEN_PAREN_R );	TEST_ERROR;

			// function name
			if ( m_token != TOKEN_IDENT )	ERROR( "function definition missing identifier" );
			funcName = strdup( m_lex.TokenText() );
			NextToken();	TEST_ERROR;
		}
		else
		{
			RestoreState( &state );
		}
	}
	// single return parameter?
	else if ( m_token == TOKEN_INT_DECL || m_token == TOKEN_FLOAT_DECL || m_token == TOKEN_STRING_DECL || m_token == TOKEN_VOID_DECL )
	{
		ParseState state;
		SaveState( &state );

		// return parameters
		ScriptToken saveType = m_token;
		outParameters = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );

		NextToken();	TEST_ERROR;

		if ( m_token != TOKEN_IDENT )
		{
			m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "type %s must be followed by identifier, got %s instead.\n",
				ScriptUtil::TokenName( saveType ), ScriptUtil::TokenName( m_token ) );
			goto Error;
		}

		funcName = strdup( m_lex.TokenText() );
		NextToken();	TEST_ERROR;

		// at this point we could be either a variable decl or a func decl, the
		// existence of the paren determines which.
		if ( m_token != TOKEN_PAREN_L )
		{
			delete funcName;
			funcName = NULL;
		}

		if ( !funcName )
		{
			RestoreState( &state );
		}
	}

	// by now we should have a valid funcName if this is a func decl.
	if ( funcName )
	{
		// input parameters
		Match( TOKEN_PAREN_L ); TEST_ERROR;
		if ( m_token != TOKEN_PAREN_R )
		{
			inParameters = ParseFunctionParameterDefinition();	TEST_ERROR;
		}
		Match( TOKEN_PAREN_R );

		// put this guy in our symbol table
		int index;
		index = m_syms.FindFunctionByName( funcName, m_currentScope, false );
		if ( index < 0 )
		{
			index = m_syms.AddSymbol( isExternFunc ? ScriptSymbols::SYMBOL_C_FUNCTION : ScriptSymbols::SYMBOL_FUNCTION,
				                       funcName, -1, m_currentScope );
			m_syms[ index ].numIn  = CountParameters( inParameters );
			m_syms[ index ].numOut = CountParameters( outParameters );
			if ( m_syms[ index ].numIn & 0x1000 )
			{
				if ( !isExternFunc ) { m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "function '%s' can't have variable number of arguments unless externed.", funcName ); }
			}
			if ( m_syms[ index ].numOut & 0x1000 )
			{
				m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "function '%s' can't have a variable number of return arguments.", funcName );
			}
		}
		else
		{
			if ( m_syms[ index ].numIn != CountParameters( inParameters ) )
				m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "function '%s' has different number of input parameters from previous declaration", funcName );
			if ( m_syms[ index ].numOut != CountParameters( outParameters ) )
				m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "function '%s' has different number of return parameters from previous declaration", funcName );
		}

		// c function
		if ( isExternFunc )
		{
			Match( TOKEN_SEMICOLON ); TEST_ERROR;
			tree = new ScriptParseTree( TOKEN_NONE, m_lex.GetLine(), m_lex.GetFile() );
		}
		// internal function prototype
		else if ( m_token == TOKEN_SEMICOLON )
		{
			NextToken(); TEST_ERROR;
			tree = new ScriptParseTree( TOKEN_NONE, m_lex.GetLine(), m_lex.GetFile() );
		}
		// function body
		else
		{
			if ( m_syms[ index ].subtype == ScriptSymbols::SYMBOL_SUBTYPE_DEFINED )
			{
				m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "function '%s' already defined", funcName );
				goto Error;
			}

			// add us to external labels to export
			if ( m_currentScope == 0 )
			{
				m_syms[ index ].stringID = m_syms.Strings().Add( m_syms[ index ].name );
				m_syms.IncOutputLabels( 1 );
			}

			int parentScope = m_currentScope;
			m_currentScope = ++m_numScopes;
			int scopeIndex = m_syms.AddSymbol( ScriptSymbols::SYMBOL_SCOPE, FUNC_SCOPE, m_currentScope, parentScope );
			m_syms[ scopeIndex ].parentFunc = index;
			m_syms[ index ].parmStart = m_syms.Count();
			DeclareFunctionParameters( inParameters );						TEST_ERROR;
			m_syms[ index ].parmEnd   = m_syms.Count() - 1;
			Match( TOKEN_BRACE_L );													TEST_ERROR;
			tree = new ScriptParseTree( TOKEN_SCOPE_BEGIN, m_lex.GetLine(), m_lex.GetFile() );
			tree->m_value.i = m_currentScope;
			tree->m_child[ 0 ] = new ScriptParseTree( TOKEN_FUNC_DECL, m_lex.GetLine(), m_lex.GetFile() );
			tree->m_child[ 0 ]->m_value.i = index;
			tree->m_child[ 0 ]->m_child[ 0 ] = ParseStatementList();		TEST_ERROR;
			Match( TOKEN_BRACE_R );													TEST_ERROR;
			m_currentScope = parentScope;

			m_syms[ index ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_DEFINED;
		}
	}
	else if ( isExternFunc )
	{
		ERROR( "'extern' must be followed by function prototype" );
	}

	if ( inParameters )	delete inParameters;
	if ( outParameters ) delete outParameters;
	if ( funcName )		delete funcName;
	return tree;

Error:
	if ( inParameters )	delete inParameters;
	if ( outParameters ) delete outParameters;
	if ( funcName )		delete funcName;
	if ( tree )				delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseFunctionParameterDefinition()
{
	ScriptParseTree* tree = NULL;
	ScriptParseTree* tail = NULL;
	int expect   = 1;
	int argCount = 0;

	while ( expect )
	{
		argCount++;
		expect--;

		if ( m_token == TOKEN_INT_DECL || m_token == TOKEN_FLOAT_DECL || m_token == TOKEN_STRING_DECL || m_token == TOKEN_DOTDOTDOT )
		{
			ScriptParseTree* arg = new ScriptParseTree( TOKEN_ARG, m_lex.GetLine(), m_lex.GetFile() );
			arg->m_child[ 0 ]    = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );

			if ( tail )   tail->m_child[ 1 ] = arg;
			else          tree = arg;
			tail = arg;

			if ( m_token != TOKEN_DOTDOTDOT )
			{
				NextToken();																TEST_ERROR;

				arg->m_child[ 0 ]->m_child[ 0 ] = ParseDeclarator();			TEST_ERROR;
				if ( ! arg->m_child[ 0 ]->m_child[ 0 ] )
				{
					m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "parameter declaration, argment %d missing identifier", argCount );
					goto Error;
				}
				if ( m_token == TOKEN_COMMA )
				{
					NextToken();															TEST_ERROR;
					expect++;
				}
			}
			else
			{
				Match( TOKEN_DOTDOTDOT ); TEST_ERROR;
				expect = 0;
			}
		}
		else
		{
			m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "parameter declation, argument %d malformed.", argCount );
			goto Error;
		}
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseVariableDeclaration()
{
	ScriptParseTree* tree = NULL;
	bool staticVar = false;

	if ( m_token == TOKEN_STATIC )
	{
		staticVar = true;
		NextToken();		TEST_ERROR;
	}

	if ( m_token == TOKEN_INT_DECL || m_token == TOKEN_FLOAT_DECL || m_token == TOKEN_STRING_DECL )
	{
		ScriptToken type = m_token;

		ScriptParseTree* tail = NULL;

		do {
			NextToken();										TEST_ERROR;	// skips type the first time, commas after that
			ScriptParseTree* var = ParseDeclarator();	TEST_ERROR;
			if ( !var )											ERROR( "expecting identifer after type" );

			int index = DeclareVariable( type, var, staticVar );

			delete var;
			TEST_ERROR;

			// check for assignment
			if ( m_token == TOKEN_ASSIGN )
			{
				ScriptParseTree* group = new ScriptParseTree( TOKEN_GROUP, m_lex.GetLine(), m_lex.GetFile() );
				if ( tail ) tail->m_child[ 1 ] = group;
				else        tree = group;
				tail = group;

				tail->m_child[ 0 ] = new ScriptParseTree( TOKEN_STATEMENT_EXPR, m_lex.GetLine(), m_lex.GetFile() );
				tail->m_child[ 0 ]->m_child[ 0 ] = new ScriptParseTree( TOKEN_ASSIGN, m_lex.GetLine(), m_lex.GetFile() );
				ScriptParseTree* assign = tail->m_child[ 0 ]->m_child[ 0 ];
				assign->m_child[ 0 ] = new ScriptParseTree( TOKEN_VARIABLE, m_lex.GetLine(), m_lex.GetFile() );
				assign->m_child[ 0 ]->m_value.i = index;

				NextToken(); TEST_ERROR;
				assign->m_child[ 1 ] = ParseExpr(); TEST_ERROR;
				if ( !assign->m_child[ 1 ] ) ERROR( "expecting expression after '='" );
			}
		} while ( m_token == TOKEN_COMMA );

		// this was a valid assignment, we need to return something even if we have no initializers
		if ( !tree )
			tree = new ScriptParseTree( TOKEN_NONE, m_lex.GetLine(), m_lex.GetFile() );

		Match( TOKEN_SEMICOLON );													TEST_ERROR;
	}
	else if ( m_token == TOKEN_STRUCT_DECL )
	{
		tree = new ScriptParseTree( TOKEN_NONE, m_lex.GetLine(), m_lex.GetFile() );
		NextToken(); TEST_ERROR;

		// we have mandatory named structures, unlike C.
		if ( m_token != TOKEN_IDENT )
			ERROR( "struct must be followed by identifier." );

		// change scope
		int parentScope = m_currentScope;
		m_currentScope  = ++m_numScopes;

		m_syms.AddSymbol( ScriptSymbols::SYMBOL_STRUCT, m_lex.TokenText(), m_currentScope, parentScope );
		NextToken();							TEST_ERROR;

		Match( TOKEN_BRACE_L );				TEST_ERROR;
		ParseStructDeclarationList();		TEST_ERROR;
		Match( TOKEN_BRACE_R );				TEST_ERROR;

		// no initial value for now.
		Match( TOKEN_SEMICOLON );			TEST_ERROR;

		// restore scope
		m_currentScope = parentScope;
	}
	else if ( staticVar )
	{
		ERROR( "static must be followed by variable declaration" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseDeclarator()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_IDENT )
	{
		tree = new ScriptParseTree( TOKEN_IDENT, m_lex.GetLine(), m_lex.GetFile() );
		tree->m_value.p = strdup( m_lex.TokenText() );
		NextToken();															TEST_ERROR;

		if ( m_token == TOKEN_BRACKET_L )
		{
			NextToken();														TEST_ERROR;
			tree->m_child[ 0 ] = ParseExprConstant();					TEST_ERROR;
			if ( ! tree->m_child[ 0 ] )									ERROR( "declaration must have const-expression inside [ .. ]" );
			tree->m_child[ 0 ]->Simplify();
			if ( tree->m_child[ 0 ]->m_token != TOKEN_CONST_INT )	ERROR( "declaration must have const-expression inside [ .. ]" );
			Match( TOKEN_BRACKET_R );										TEST_ERROR;
		}
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseStructDeclarationList()
{
	while ( m_token == TOKEN_INT_DECL || m_token == TOKEN_FLOAT_DECL || m_token == TOKEN_STRING_DECL )
	{
		ScriptSymbols::SymbolType symType = ( m_token == TOKEN_INT_DECL )   ? ScriptSymbols::SYMBOL_VARIABLE_INT :
	                                       ( m_token == TOKEN_FLOAT_DECL ) ? ScriptSymbols::SYMBOL_VARIABLE_FLOAT :
														                                  ScriptSymbols::SYMBOL_VARIABLE_STRING;

		do {
			NextToken(); TEST_ERROR;
			if ( m_token != TOKEN_IDENT )
				ERROR( "must have identifier following type" );

			// make sure it doesn't already exist in this structure
			if ( m_syms.FindVariableByName( m_lex.TokenText(), m_currentScope, false ) >= 0 )
			{
				m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "structure element '%s' already used", m_lex.TokenText() );
				goto Error;
			}

			m_syms.AddSymbol( symType, m_lex.TokenText(), 0, m_currentScope );
			NextToken(); TEST_ERROR;
		} while ( m_token == TOKEN_COMMA );

		Match( TOKEN_SEMICOLON ); TEST_ERROR;
	}

	return NULL;

Error:
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExpr()
{
	ScriptParseTree* tree = ParseAssignmentExpr();		TEST_ERROR;
	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseAssignmentExpr()
{
	ScriptParseTree* tree = NULL;

	if ( m_token == TOKEN_IDENT )
	{
		ParseState state;
		SaveState( &state );

		tree = ParseExprUnary();									TEST_ERROR;

		if ( tree != NULL && ScriptUtil::TokenIsAssignment( m_token ) )
		{
			ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			newNode->m_child[ 0 ] = tree;
			tree = newNode;

			NextToken();												TEST_ERROR;
			tree->m_child[ 1 ] = ParseAssignmentExpr();		TEST_ERROR;
			if ( ! tree->m_child[ 1 ] )							ERROR( "missing expression following assignment" );
		}
		else
		{
			RestoreState( &state );
			if ( tree != NULL )
			{
				delete tree;
				tree = NULL;
			}
		}
	}

	if ( ! tree )
	{
		tree = ParseExprConditional();							TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprConditional()
{
	ScriptParseTree* tree = ParseExprLogicalOr();	TEST_ERROR;

	if ( m_token == TOKEN_QUESTION_MARK )
	{
		ScriptParseTree* newNode = new ScriptParseTree( TOKEN_COND_EXPR, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();											TEST_ERROR;
		tree->m_child[ 1 ] = ParseExpr();				TEST_ERROR;
		if ( ! tree->m_child[ 1 ] )						ERROR( "condition expression requires expression after '?'" );
		Match( TOKEN_COLON );								TEST_ERROR;
		tree->m_child[ 2 ] = ParseExprConditional(); TEST_ERROR;
		if ( ! tree->m_child[ 2 ] )						ERROR( "condition expression requires expression after ':'" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprConstant()
{
	return ParseExprLogicalOr();
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprLogicalOr()
{
	ScriptParseTree* tree = ParseExprLogicalAnd();		TEST_ERROR;

	while ( m_token == TOKEN_LOGICAL_OR )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprLogicalAnd();	TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "|| missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprLogicalAnd()
{
	ScriptParseTree* tree = ParseExprBitwiseOr();		TEST_ERROR;

	while ( m_token == TOKEN_LOGICAL_AND )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprBitwiseOr();	TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "&& missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprBitwiseOr()
{
	ScriptParseTree* tree = ParseExprBitwiseXor();		TEST_ERROR;

	while ( m_token == TOKEN_BITWISE_OR )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprBitwiseXor();	TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "| missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprBitwiseXor()
{
	ScriptParseTree* tree = ParseExprBitwiseAnd();		TEST_ERROR;

	while ( m_token == TOKEN_BITWISE_XOR )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprBitwiseAnd();	TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "^ missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprBitwiseAnd()
{
	ScriptParseTree* tree = ParseExprEqual();				TEST_ERROR;

	while ( m_token == TOKEN_BITWISE_AND )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprEqual();			TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "& missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprEqual()
{
	ScriptParseTree* tree = ParseExprCompare();			TEST_ERROR;

	if ( m_token == TOKEN_EQUALS || m_token == TOKEN_NOT_EQUALS )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprCompare();		TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "==,!= missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprCompare()
{
	ScriptParseTree* tree = ParseExprShift();				TEST_ERROR;

	if ( m_token == TOKEN_LESS_THAN || m_token == TOKEN_LESS_THAN_EQUAL ||
			m_token == TOKEN_GTR_THAN || m_token == TOKEN_GTR_THAN_EQUAL  )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprShift();			TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "<,>,<=,>= missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprShift()
{
	ScriptParseTree* tree = ParseExprSum();				TEST_ERROR;

	if ( m_token == TOKEN_SHIFT_L || m_token == TOKEN_SHIFT_R )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprSum();			TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "<<,>> missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprSum()
{
	ScriptParseTree* tree = ParseExprMult();				TEST_ERROR;

	while ( m_token == TOKEN_PLUS || m_token == TOKEN_MINUS )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprMult();			TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "+,- missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprMult()
{
	ScriptParseTree* tree = ParseExprUnary();				TEST_ERROR;

	while ( m_token == TOKEN_MULT || m_token == TOKEN_DIV || m_token == TOKEN_MOD )
	{
		ScriptParseTree* newNode = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
		newNode->m_child[ 0 ] = tree;
		tree = newNode;
		NextToken();												TEST_ERROR;
		newNode->m_child[ 1 ] = ParseExprUnary();			TEST_ERROR;
		if ( ! newNode->m_child[ 1 ] )						ERROR( "*,/,%% missing expression" );
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprUnary()
{
	ScriptParseTree* tree = NULL;

	switch ( m_token )
	{
		case TOKEN_MINUS:
			tree = new ScriptParseTree( TOKEN_UNARY_MINUS, m_lex.GetLine(), m_lex.GetFile() );
			NextToken();										TEST_ERROR;
			tree->m_child[ 0 ] = ParseExprPostfix();	TEST_ERROR;
			if ( ! tree->m_child[ 0 ] )					ERROR( "- missing expression" );
			break;

		case TOKEN_LOGICAL_NOT:
		case TOKEN_BITWISE_NOT:
			tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			NextToken();										TEST_ERROR;
			tree->m_child[ 0 ] = ParseExprUnary();	   TEST_ERROR;
			if ( ! tree->m_child[ 0 ] )					ERROR( "!,~ missing expression" );
			break;

		case TOKEN_INCREMENT:
		case TOKEN_DECREMENT:
			tree = new ScriptParseTree( m_token == TOKEN_INCREMENT ? TOKEN_PLUS_EQUALS : TOKEN_MINUS_EQUALS, m_lex.GetLine(), m_lex.GetFile() );
			NextToken();										TEST_ERROR;
			tree->m_child[ 0 ] = ParseExprPostfix();	TEST_ERROR;
			if ( ! tree->m_child[ 0 ] || tree->m_child[ 0 ]->m_token != TOKEN_VARIABLE ) ERROR( "expecting identifier after ++,--" );
			tree->m_child[ 1 ] = new ScriptParseTree( TOKEN_CONST_INT, m_lex.GetLine(), m_lex.GetFile() );
			tree->m_child[ 1 ]->m_value.i = 1;
			break;

		case TOKEN_SIZEOF:
			tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			NextToken();										TEST_ERROR;
			tree->m_child[ 0 ] = ParseExprPostfix();	TEST_ERROR;
			if ( ! tree->m_child[ 0 ] || tree->m_child[ 0 ]->m_token != TOKEN_VARIABLE ) ERROR( "expecting identifier after sizeof" );
			break;

		case TOKEN_ABS:
			tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			NextToken();										TEST_ERROR;
			tree->m_child[ 0 ] = ParseExprUnary();		TEST_ERROR;
			break;

		case TOKEN_IDENT:
			// hack for preprocessor to handle #if lines...
			if ( m_preprocessorMode && strcmp( m_lex.TokenText(), "defined" ) == 0 )
			{
				bool paren = false;
				NextToken();									TEST_ERROR;
				if ( m_token == TOKEN_PAREN_L )
				{
					paren = true;
					NextToken();								TEST_ERROR;
				}
				if ( m_token != TOKEN_IDENT )				ERROR( "expecting identifer after 'defined'" );

				int index = m_syms.FindConstantByName( m_lex.TokenText(), 0, false );
				tree = new ScriptParseTree( TOKEN_CONST_INT, m_lex.GetLine(), m_lex.GetFile() );
				tree->m_value.i = ( index >= 0 );

				NextToken();									TEST_ERROR;
				if ( paren )
				{
					Match( TOKEN_PAREN_R );					TEST_ERROR;
				}
			}
			else
			{
				tree = ParseExprPostfix();					TEST_ERROR;
			}
			break;

		default:
			tree = ParseExprPostfix();						TEST_ERROR;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprPostfix()
{
	ScriptParseTree* tree = ParseExprFactor();		TEST_ERROR;
	if ( !tree ) return NULL;

	while ( m_token == TOKEN_INCREMENT || m_token == TOKEN_DECREMENT ||
		     m_token == TOKEN_BRACKET_L || m_token == TOKEN_PAREN_L    )
	{
		switch ( m_token )
		{
			case TOKEN_INCREMENT:
				{
					if ( tree->m_token != TOKEN_VARIABLE )		ERROR( "++ must be used with identifier" );
					ScriptParseTree* newNode = new ScriptParseTree( TOKEN_POST_INCREMENT, m_lex.GetLine(), m_lex.GetFile() );
					newNode->m_child[ 0 ] = tree;
					tree = newNode;
					NextToken();									TEST_ERROR;
				}
				break;

			case TOKEN_DECREMENT:
				{
					if ( tree->m_token != TOKEN_VARIABLE )		ERROR( "-- must be used with identifier" );
					ScriptParseTree* newNode = new ScriptParseTree( TOKEN_POST_DECREMENT, m_lex.GetLine(), m_lex.GetFile() );
					newNode->m_child[ 0 ] = tree;
					tree = newNode;
					NextToken();									TEST_ERROR;
				}
				break;

			case TOKEN_BRACKET_L:
				{
					if ( tree->m_token != TOKEN_VARIABLE )		ERROR( "must use identifier with []" );
					NextToken();									TEST_ERROR;
					tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
					if ( ! tree->m_child[ 0 ] )				ERROR( "expecting expression inside [ .. ]" );
					Match( TOKEN_BRACKET_R );					TEST_ERROR;
				}
				break;

			case TOKEN_PAREN_L:
				{
					if ( tree->m_token == TOKEN_IDENT )
					{
						m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "function '%s' not defined", tree->m_value.p );
						goto Error;
					}
					if ( tree->m_token != TOKEN_FUNCTION )				ERROR( "function call syntax error" );
					ScriptParseTree* newNode = new ScriptParseTree( TOKEN_FUNC_CALL, m_lex.GetLine(), m_lex.GetFile() );
					newNode->m_value.i = tree->m_value.i;	// function index
					delete tree;
					tree = newNode;
					NextToken();												TEST_ERROR;
					if ( m_token != TOKEN_PAREN_R )
						tree->m_child[ 0 ] = ParseArgumentList();		TEST_ERROR;
					Match( TOKEN_PAREN_R );									TEST_ERROR;
				}
				break;

			default:
				break;
		}
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseArgumentList()
{
	ScriptParseTree* tree = new ScriptParseTree( TOKEN_ARG, m_lex.GetLine(), m_lex.GetFile() );
	tree->m_child[ 0 ] = ParseExpr();		TEST_ERROR;
	if ( ! tree->m_child[ 0 ] )				ERROR( "expecting function argument" );
	if ( m_token == TOKEN_COMMA )
	{
		NextToken();										TEST_ERROR;
		tree->m_child[ 1 ] = ParseArgumentList();	TEST_ERROR;
	}
	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

ScriptParseTree* ScriptParse::ParseExprFactor()
{
	ScriptParseTree* tree = NULL;

	switch( m_token )
	{
		case TOKEN_CONST_INT:
		case TOKEN_CONST_FLOAT:
			tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			tree->m_value = m_lex.TokenValue();
			NextToken();							TEST_ERROR;
			break;

		case TOKEN_THIS:
			tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			NextToken();							TEST_ERROR;
			break;

		case TOKEN_IDENT:
			// preprocessor and normal parser handle idents differently.
			if ( !m_preprocessorMode )
			{
				int index;
				if ( ( index = m_syms.FindConstantByName( m_lex.TokenText(), m_currentScope, true ) ) >= 0 )
				{
					tree = new ScriptParseTree( TOKEN_CONST_INT, m_lex.GetLine(), m_lex.GetFile() );
					tree->m_value.i = m_syms[ index ].data;
				}
				else if ( ( index = m_syms.FindVariableByName( m_lex.TokenText(), m_currentScope, true ) ) >= 0 )
				{
					tree = new ScriptParseTree( TOKEN_VARIABLE, m_lex.GetLine(), m_lex.GetFile() );
					tree->m_value.i = index;
				}
				else if ( ( index = m_syms.FindFunctionByName( m_lex.TokenText(), m_currentScope, true ) ) >= 0 )
				{
					tree = new ScriptParseTree( TOKEN_FUNCTION, m_lex.GetLine(), m_lex.GetFile() );
					tree->m_value.i = index;
				}
				else
				{
					m_err.Error( m_lex.GetFile(), m_lex.GetLine(), "'%s' undeclared", m_lex.TokenText() );
					goto Error;
				}
				NextToken();						TEST_ERROR;
			}
			else
			{
				// in the preprocesssor, if we get here it means our symbol was not in the symbol
				// table, because we've already expanded all symbols in the table... so the value is just zero.
				tree = new ScriptParseTree( TOKEN_CONST_INT, m_lex.GetLine(), m_lex.GetFile() );
				tree->m_value.i = 0;
				NextToken();						TEST_ERROR;
			}
			break;

		case TOKEN_STRING:
			tree = new ScriptParseTree( m_token, m_lex.GetLine(), m_lex.GetFile() );
			tree->m_value.p = strdup( m_lex.TokenText() );
			NextToken();							TEST_ERROR;
			break;

		case TOKEN_PAREN_L:
			Match( TOKEN_PAREN_L );				TEST_ERROR;
			tree = ParseExpr();					TEST_ERROR;

			// is this an expression list?
			if ( m_token == TOKEN_COMMA )
			{
				Match( TOKEN_COMMA );			TEST_ERROR;
				ScriptParseTree* tail = new ScriptParseTree( TOKEN_EXPR_LIST, m_lex.GetLine(), m_lex.GetFile() );
				tail->m_child[ 0 ] = tree;
				tree = tail;
				while ( m_token != TOKEN_PAREN_R )
				{
					tail->m_child[ 1 ] = new ScriptParseTree( TOKEN_EXPR_LIST, m_lex.GetLine(), m_lex.GetFile() );
					tail = tail->m_child[ 1 ];
					tail->m_child[ 0 ] = ParseExpr();	TEST_ERROR;
					if ( !tail->m_child[ 0 ] )				ERROR( "expression list has invalid expression" );
					if ( m_token == TOKEN_COMMA )
					{
						Match( TOKEN_COMMA );					TEST_ERROR;
					}
				}
			}

			Match( TOKEN_PAREN_R );				TEST_ERROR;
			break;

		default:
			break;
	}

	return tree;

Error:
	if ( tree ) delete tree;
	return NULL;
}

//===========================================================================

#endif //SCRIPT_COMPILER