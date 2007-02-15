//===========================================================================
//=
//= ScriptParseTree.h
//=
//===========================================================================

#ifndef _SCRIPT_PARSE_TREE_H
#define _SCRIPT_PARSE_TREE_H

#ifdef SCRIPT_COMPILER

class ScriptParseTree
{
public:
	ScriptParseTree( ScriptToken token, int line, const char* file );
	~ScriptParseTree();

	void					Print( int level );
	void					Simplify();
	ScriptParseTree*	Copy();
	int					Count( ScriptToken token );

private:
	void PrintToken();
	void SetConst( float value );
	void SetConst( int value );
	void ReduceUnary();
	void ReduceBinary();
	void ReduceTernary();
	void ReduceLogical();
	void BecomeTree( ScriptParseTree* srcNode );

public:
	ScriptToken			m_token;
	ScriptValue			m_value;
	ScriptParseTree*	m_child[ 4 ];
	int					m_line;
	const char*			m_file;
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_PARSE_TREE_H