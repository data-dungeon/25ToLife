//===========================================================================
//=
//= ScriptParse.h
//=
//===========================================================================

#ifndef _SCRIPT_PARSE_H
#define _SCRIPT_PARSE_H

#ifdef SCRIPT_COMPILER

class ScriptParse
{
public:
	struct ParseState
	{
		ScriptLex::LexState lexState;
		ScriptToken         token;
	};

	ScriptParse( ScriptLex& lex, ScriptSymbols& syms, ScriptError& err );
	~ScriptParse();
	ScriptParseTree* Parse();
	ScriptParseTree* ParsePreprocessorExpr();
	void	SetVerbose( bool verbose ) { m_verbose = verbose; }
	void	PrintSymbols()					{ m_syms.Print(); }

private:
	inline ScriptToken Token() const	{ return m_token; }
	inline ScriptToken NextToken()   { return ( m_token = m_lex.NextToken() );   }
	void   Match( ScriptToken token );

	void   SaveState( ParseState* state );
	void	 RestoreState( ParseState* state );

	static int CountParameters( ScriptParseTree* node );

	int	 DeclareVariable( ScriptToken type, ScriptParseTree* tree, bool staticVar );
	void	 DeclareFunctionParameters( ScriptParseTree* tree );

	ScriptParseTree* ParseStatementList();
	ScriptParseTree* ParseStatement();
	ScriptParseTree* ParseStatementSelection();
	ScriptParseTree* ParseStatementCompound();
	ScriptParseTree* ParseStatementIteration();
	ScriptParseTree* ParseStatementExpr();
	ScriptParseTree* ParseStatementJump();
	ScriptParseTree* ParseStatementListAssign();
	ScriptParseTree* ParseStatementPrint();
	ScriptParseTree* ParseStatementSleep();
	ScriptParseTree* ParseLabel();
	ScriptParseTree* ParseEnum();
	ScriptParseTree* ParseEnumList();
	ScriptParseTree* ParseFunctionDeclaration();
	ScriptParseTree* ParseFunctionParameterDefinition();
	ScriptParseTree* ParseVariableDeclaration();
	ScriptParseTree* ParseDeclarator();
	ScriptParseTree* ParseStructDeclarationList();
	ScriptParseTree* ParseExpr();
	ScriptParseTree* ParseAssignmentExpr();
	ScriptParseTree* ParseExprConditional();
	ScriptParseTree* ParseExprConstant();
	ScriptParseTree* ParseExprLogicalOr();
	ScriptParseTree* ParseExprLogicalAnd();
	ScriptParseTree* ParseExprBitwiseOr();
	ScriptParseTree* ParseExprBitwiseXor();
	ScriptParseTree* ParseExprBitwiseAnd();
	ScriptParseTree* ParseExprEqual();
	ScriptParseTree* ParseExprCompare();
	ScriptParseTree* ParseExprShift();
	ScriptParseTree* ParseExprSum();
	ScriptParseTree* ParseExprMult();
	ScriptParseTree* ParseExprUnary();
	ScriptParseTree* ParseExprPostfix();
	ScriptParseTree* ParseArgumentList();
	ScriptParseTree* ParseExprFactor();

private:
	ScriptLex&		m_lex;
	ScriptSymbols&	m_syms;
	ScriptError&	m_err;

	ScriptToken		m_token;
	bool				m_verbose;
	int				m_numScopes;
	int				m_currentScope;
	bool				m_preprocessorMode;
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_PARSE_H
