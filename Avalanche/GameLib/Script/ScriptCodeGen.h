//===========================================================================
//=
//= ScriptCodeGen.h
//=
//===========================================================================

#ifndef _SCRIPT_CODE_GEN_H
#define _SCRIPT_CODE_GEN_H

#ifdef SCRIPT_COMPILER

//===========================================================================

class ScriptSwitchTable
{
public:
	struct Entry
	{
		Entry* next;
		int	 value;
		int	 targetLabel;
	};

	ScriptSwitchTable()
	{
		m_head         = 0;
		m_count        = 0;
		m_defaultLabel = -1;
	}
	~ScriptSwitchTable()
	{
		Entry* n;
		Entry* next;
		for ( n = m_head ; n ; n = next )
		{
			next = n->next;
			delete n;
		}
	}
	bool Add( int value, int label )
	{
		// find the right place to insert this (sorted)
		Entry** parent = &m_head;
		Entry*  n;
		for ( n = *parent ; n && n->value < value ; n = *parent )
			parent = &n->next;

		// make sure we don't have a duplicate
		if ( n && n->value == value )
			return false;

		// create our node
		n              = new Entry;
		n->value       = value;
		n->targetLabel = label;

		// insert it
		n->next        = *parent;
		*parent        = n;
		m_count++;

		return true;
	}
	bool SetDefault( int label )
	{
		if ( m_defaultLabel >= 0 )
			return false;
		m_defaultLabel = label;
		return true;
	}
	inline int		Count()   const { return m_count; }
	inline Entry*	Head()    const { return m_head; }
	inline int		Default() const { return m_defaultLabel; }

private:
	Entry*	m_head;
	int		m_count;
	int		m_defaultLabel;
};

//===========================================================================

class ScriptSwitchTableList
{
public:
	struct Entry
	{
		int	value;
		int	targetLabel;
	};
	ScriptSwitchTableList()
	{
		m_data = 0;
		Reset();
	}
	~ScriptSwitchTableList()
	{
		Reset();
	}
	void Reset()
	{
		if ( m_data )
			delete [] m_data;
		m_maxSize = 0;
		m_size    = 0;
		m_data    = 0;
	}
	int AddTable( ScriptSwitchTable* table )
	{
		int i;

		// we need one for the first entry (count & default pc), +1 for each case
		int needed = 1 + table->Count();

		// grow table if necessary
		if ( m_size + needed > m_maxSize )
		{
			m_maxSize = ( m_size + needed ) * 3 / 2;
			Entry* newTable = new Entry [ m_maxSize ];
			for ( i = 0 ; i < m_size ; i++ )
				newTable[ i ] = m_data[ i ];
			if ( m_data )
				delete [] m_data;
			m_data = newTable;
		}

		// remember entry location
		int tableIndex = m_size;

		// add default entry.  value is the count for the rest of the entries.
		m_data[ m_size ].value       = table->Count();
		m_data[ m_size ].targetLabel = table->Default();
		m_size++;

		ScriptSwitchTable::Entry* n;
		for ( n = table->Head() ; n ; n = n->next )
		{
			m_data[ m_size ].value       = n->value;
			m_data[ m_size ].targetLabel = n->targetLabel;
			m_size++;
		}

		ASSERT( m_size <= m_maxSize );

		return tableIndex;
	}
	inline int Count() const { return m_size; }

	inline Entry& operator [] ( const int index )
	{
		ASSERT( index >= 0 && index < m_size );
		return m_data[ index ];
	}

private:
	int		m_maxSize;
	int		m_size;
	Entry*	m_data;
};

//===========================================================================

class ScriptCodeGen
{
public:
	ScriptCodeGen( ScriptSymbols& syms, ScriptError& err );
	~ScriptCodeGen();

	ScriptExecutable* CreateExecutable( ScriptParseTree* tree, const char* sourceFilename, bool debug );
	void PrintSymbols() { m_syms.Print(); }

private:
	struct Instruction
	{
		ScriptOpcode	op;
		ScriptValue		subcode;
		int				file;
		int				line;
		Instruction*	next;
	};

	void	Reset();
	int	CodeGen( ScriptParseTree* node );
	void	AddInstruction( ScriptOpcode op, ScriptValue subcode );
	void	AddInstruction( ScriptOpcode op, int value );
	void	Optimize();
	int	CountOpcodes();

	static int CountParameters( ScriptParseTree* node );

	int CodeGen_FuncCall( ScriptParseTree* node );
	int CodeGen_FuncDecl( ScriptParseTree* node );
	int CodeGen_ScopeBegin( ScriptParseTree* node );
	int CodeGen_For( ScriptParseTree* node );
	int CodeGen_If( ScriptParseTree* node );
	int CodeGen_While( ScriptParseTree* node );
	int CodeGen_Do( ScriptParseTree* node );
	int CodeGen_And( ScriptParseTree* node );
	int CodeGen_Or( ScriptParseTree* node );
	int CodeGen_StatementExpr( ScriptParseTree* node );
	int CodeGen_String( ScriptParseTree* node );
	int CodeGen_Variable( ScriptParseTree* node );
	int CodeGen_Label( ScriptParseTree* node );
	int CodeGen_Switch( ScriptParseTree* node );
	int CodeGen_Case( ScriptParseTree* node );
	int CodeGen_Goto( ScriptParseTree* node );
	int CodeGen_PostIncDec( ScriptParseTree* node );
	int CodeGen_Assignment( ScriptParseTree* node );
	int CodeGen_ArithUnary( ScriptParseTree* node );
	int CodeGen_ArithBinary( ScriptParseTree* node );
	int CodeGen_Print( ScriptParseTree* node );
	int CodeGen_Sleep( ScriptParseTree* node );
	int CodeGen_Return( ScriptParseTree* node );
	int CodeGen_ListAssign( ScriptParseTree* node );
	int CodeGen_ListAssignLHS( ScriptParseTree* node );
	int CodeGen_BreakContinue( ScriptParseTree* node );
	int CodeGen_Sizeof( ScriptParseTree* node );
	int CodeGen_CondExpr( ScriptParseTree* node );

private:
	ScriptSymbols&				m_syms;
	ScriptError&				m_err;
	Instruction*				m_head;
	Instruction*				m_tail;
	int							m_currentScope;
	ScriptSwitchTableList	m_switchTable;
	const char*					m_lastFile;
	const char*					m_currentFile;
	int							m_currentFileIndex;
	int							m_currentLine;
};

#endif //SCRIPT_COMPILER

#endif //_SCRIPT_CODE_GEN_H