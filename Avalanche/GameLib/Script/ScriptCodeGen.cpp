//===========================================================================
//=
//= ScriptCodeGen.cpp
//=
//===========================================================================

#include "ScriptPCH.h"

#ifdef SCRIPT_COMPILER

//===========================================================================

ScriptCodeGen::ScriptCodeGen( ScriptSymbols& syms, ScriptError& err ) :
	m_syms( syms ),
	m_err( err )
{
	m_head         = NULL;
	m_tail         = NULL;

	Reset();
}

//===========================================================================

ScriptCodeGen::~ScriptCodeGen()
{
	Reset();
}

//===========================================================================

void ScriptCodeGen::Reset()
{
	Instruction* n;
	Instruction* next;
	for ( n = m_head ; n ; n = next )
	{
		next = n->next;
		delete n;
	}
	m_head = NULL;
	m_tail = NULL;
	m_currentScope = 0;

	m_lastFile         = NULL;
	m_currentFile      = NULL;
	m_currentFileIndex = -1;
	m_currentLine      = 0;

	m_switchTable.Reset();
}

//===========================================================================

void ScriptCodeGen::AddInstruction( ScriptOpcode op, ScriptValue subcode )
{
	// add the filename to the string table
	if ( m_currentFileIndex < 0 || strcmp( m_currentFile, m_lastFile ) != 0 )
	{
		m_currentFileIndex = m_syms.Strings().Add( m_currentFile );
		m_lastFile         = m_currentFile;
	}

	Instruction* instr = new Instruction;
	instr->op        = op;
	instr->subcode   = subcode;
	instr->next      = NULL;
	instr->file      = m_currentFileIndex;
	instr->line      = m_currentLine;

	if ( m_tail == NULL )	m_head = instr;
	else                    m_tail->next = instr;
	m_tail = instr;
}

//===========================================================================

void ScriptCodeGen::AddInstruction( ScriptOpcode op, int value )
{
	ScriptValue v;
	v.i = value;
	AddInstruction( op, v );
}

//===========================================================================

void ScriptCodeGen::Optimize()
{
	// remove needless instructions
	Instruction* n;
	Instruction** parentLink = &m_head;

	for ( n = m_head ; n ; )
	{
		// pushing then immediately popping is wasteful.
		if ( ( n->op == OPCODE_PUSH_INT || n->op == OPCODE_PUSH_FLOAT || n->op == OPCODE_PUSH_STRING || n->op == OPCODE_PUSH_VAR ) &&
			  ( n->next && n->next->op == OPCODE_POP ) )
		{
			*parentLink = n->next->next;
			delete n->next;
			delete n;
			n = *parentLink;
		}
		// if we're storing to a variable and then doing a null pop next instruction, collapse
		else if ( n->op == OPCODE_STORE && n->next && n->next->op == OPCODE_POP )
		{
			Instruction* kill = n->next;
			n->op = OPCODE_POP_AND_STORE;
			n->next = n->next->next;
			delete kill;
		}
		// two back to back jumps, kill the second
		else if ( n->op == OPCODE_JUMP && n->next && n->next->op == OPCODE_JUMP )
		{
			Instruction* kill = n->next;
			n->next = n->next->next;
			delete kill;
		}
		// always type jumps
		else if ( n->op == OPCODE_PUSH_INT && n->next &&
			       ( (  n->subcode.i && n->next->op == OPCODE_BRANCH_TRUE  ) ||
					   ( !n->subcode.i && n->next->op == OPCODE_BRANCH_FALSE ) ) )
		{
			Instruction* kill = n->next;
			n->op      = OPCODE_JUMP;
			n->subcode = kill->subcode;
			n->next    = kill->next;
			delete kill;
		}
		// never type jumps
		else if ( n->op == OPCODE_PUSH_INT && n->next &&
			       ( (  n->subcode.i && n->next->op == OPCODE_BRANCH_FALSE ) ||
					   ( !n->subcode.i && n->next->op == OPCODE_BRANCH_TRUE ) ) )
		{
			*parentLink = n->next->next;
			delete n->next;
			delete n;
			n = *parentLink;
		}
		else
		{
			parentLink = &n->next;
			n = n->next;
		}
	}
}

//===========================================================================

int ScriptCodeGen::CodeGen_FuncCall( ScriptParseTree* node )
{
	int funcIndex             = node->m_value.i;
	ScriptParseTree* argsNode = node->m_child[ 0 ];

	// how many arguments do we need?
	int neededArgs = m_syms[ funcIndex ].numIn & 0xfff;
	bool variableArgs = ( m_syms[ funcIndex ].numIn & 0x1000 ) != 0;

	// do the codegen for the parameters to the function
	int suppliedArgs = ( argsNode ) ? CodeGen( argsNode ) : 0;

	// check for variable number of arguments
	if ( variableArgs )
	{
		if ( suppliedArgs < neededArgs )
		{
			m_err.Error( node->m_file, node->m_line, "wrong number of arguments to function '%s', variable arg function needs at least %d args", m_syms[ funcIndex ].name, neededArgs );
		}
		AddInstruction( OPCODE_PUSH_INT, suppliedArgs );
	}
	else
	{
		// make sure we've supplied the correct number of arguments
		if ( neededArgs != suppliedArgs )
		{
			m_err.Error( node->m_file, node->m_line, "wrong number of arguments to function '%s'", m_syms[ funcIndex ].name );
		}
	}

	// and call the function
	if ( m_syms[ funcIndex ].type == ScriptSymbols::SYMBOL_C_FUNCTION )
	{
		// register it to be exported
		if ( m_syms[ funcIndex ].data < 0 )
		{
			m_syms[ funcIndex ].stringID = m_syms.Strings().Add( m_syms[ funcIndex ].name );
			m_syms[ funcIndex ].data     = m_syms.NumOutputCFunctions();
			m_syms.IncOutputCFunctions( 1 );
		}
		AddInstruction( OPCODE_CALL_C, funcIndex );
	}
	else
	{
		AddInstruction( OPCODE_CALL, funcIndex );
	}

	return m_syms[ funcIndex ].numOut;
}

//===========================================================================

int ScriptCodeGen::CodeGen_FuncDecl( ScriptParseTree* node )
{
	int i;

	int funcIndex = node->m_value.i;

	// jump over function
	int skipLabel = m_syms.AddUniqueLabel();
	AddInstruction( OPCODE_JUMP, skipLabel );

	// function entry point
	AddInstruction( OPCODE_TARGET, funcIndex );

	// pop parameters
	for ( i = m_syms[ funcIndex ].parmEnd ; i >= m_syms[ funcIndex ].parmStart ; i-- )
		for ( int j = m_syms[ i ].size - 1 ; j >= 0 ; j-- )
			AddInstruction( OPCODE_POP_AND_STORE, m_syms[ i ].data + j );

	// function body
	if ( node->m_child[ 0 ] )
		CodeGen( node->m_child[ 0 ] );

	// implicit return, in case user forgot
	for ( i = 0 ; i < m_syms[ funcIndex ].numOut ; i++ )
		AddInstruction( OPCODE_PUSH_INT, 0 );
	AddInstruction( OPCODE_RETURN, 0 );

	AddInstruction( OPCODE_TARGET, skipLabel );

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_ScopeBegin( ScriptParseTree* node )
{
	if ( node->m_child[ 0 ] )
	{
		int parentScope = m_currentScope;
		m_currentScope = node->m_value.i;

		int numReturn = CodeGen( node->m_child[ 0 ] );

		m_currentScope = parentScope;

		return numReturn;
	}
	else
	{
		return 0;
	}
}

//===========================================================================

int ScriptCodeGen::CodeGen_For( ScriptParseTree* node )
{
	int loopLabel = m_syms.AddUniqueLabel();
	int endLabel  = m_syms.AddUniqueLabel();
	int contLabel = m_syms.AddUniqueLabel();

	// initializer
	if ( node->m_child[ 0 ] )
	{
		int stackEntries = CodeGen( node->m_child[ 0 ] );
		while ( stackEntries-- )
		{
			AddInstruction( OPCODE_POP, 0 );
		}
	}

	AddInstruction( OPCODE_TARGET, loopLabel );

	// loop condition
	if ( node->m_child[ 1 ] )
	{
		if ( CodeGen( node->m_child[ 1 ] ) != 1 )
		{
			m_err.Error( node->m_file, node->m_line, "for loop condition not a valid expression" );
			return 0;
		}
		AddInstruction( OPCODE_BRANCH_FALSE, endLabel );
	}

	// loop block
	if ( node->m_child[ 3 ] )
	{
		// activate break/continue for this block
		m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_BREAK;
		m_syms[ contLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_CONTINUE;

		if ( CodeGen( node->m_child[ 3 ] ) != 0 )
		{
			m_err.Error( node->m_file, node->m_line, "for statement needs to have net zero stack input" );
			return 0;
		}

		// deactivate break/continue
		m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
		m_syms[ contLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
	}

	AddInstruction( OPCODE_TARGET, contLabel );

	// incrementer
	if ( node->m_child[ 2 ] )
	{
		int stackEntries = CodeGen( node->m_child[ 2 ] );
		while ( stackEntries-- )
		{
			AddInstruction( OPCODE_POP, 0 );
		}
	}

	AddInstruction( OPCODE_JUMP, loopLabel );
	AddInstruction( OPCODE_TARGET, endLabel );

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_If( ScriptParseTree* node )
{
	ScriptParseTree* condNode  = node->m_child[ 0 ];
	ScriptParseTree* trueNode  = node->m_child[ 1 ];
	ScriptParseTree* falseNode = node->m_child[ 2 ];

	// if condition is always true, only codegen the true block
	if ( ( condNode->m_token == TOKEN_CONST_INT   && condNode->m_value.i ) ||
		  ( condNode->m_token == TOKEN_CONST_FLOAT && condNode->m_value.f ) )
	{
		if ( CodeGen( trueNode ) != 0 )
			m_err.Error( node->m_file, node->m_line, "if statement can't leave anything on stack" );
	}

	// if condition is always false, only codegen the false block
	else if ( ( condNode->m_token == TOKEN_CONST_INT   && !condNode->m_value.i ) ||
		       ( condNode->m_token == TOKEN_CONST_FLOAT && !condNode->m_value.f ) )
	{
		if ( falseNode && CodeGen( falseNode ) != 0 )
			m_err.Error( node->m_file, node->m_line, "else statement can't leave anything on stack" );
	}

	// full if
	else
	{
		int elseLabel = m_syms.AddUniqueLabel();
		int endLabel  = m_syms.AddUniqueLabel();

		// condition
		if ( CodeGen( condNode ) != 1 )
		{
			m_err.Error( node->m_file, node->m_line, "if expression not valid" );
			return 0;
		}
		AddInstruction( OPCODE_BRANCH_FALSE, elseLabel );

		// "true" block
		if ( CodeGen( trueNode ) != 0 )
		{
			m_err.Error( node->m_file, node->m_line, "if statement needs net zero stack input" );
			return 0;
		}

		// "false" block
		if ( falseNode )
		{
			AddInstruction( OPCODE_JUMP, endLabel );
			AddInstruction( OPCODE_TARGET, elseLabel );
			if ( CodeGen( falseNode ) != 0 )
			{
				m_err.Error( node->m_file, node->m_line, "else statement needs net zero stack input" );
				return 0;
			}
			AddInstruction( OPCODE_TARGET, endLabel );
		}
		else
		{
			AddInstruction( OPCODE_TARGET, elseLabel );
		}
	}

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_While( ScriptParseTree* node )
{
	int loopLabel = m_syms.AddUniqueLabel();
	int endLabel  = m_syms.AddUniqueLabel();
	int contLabel = loopLabel;

	AddInstruction( OPCODE_TARGET, loopLabel );

	// loop condition
	if ( node->m_child[ 0 ] )
	{
		if ( CodeGen( node->m_child[ 0 ] ) != 1 )
		{
			m_err.Error( node->m_file, node->m_line, "while condition is invalid expression" );
			return 0;
		}
		AddInstruction( OPCODE_BRANCH_FALSE, endLabel );
	}

	// loop block
	if ( node->m_child[ 1 ] )
	{
		// activate break/continue for this block
		m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_BREAK;
		m_syms[ contLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_CONTINUE;

		if ( CodeGen( node->m_child[ 1 ] ) != 0 )
		{
			m_err.Error( node->m_file, node->m_line, "while statement needs to have zero stack impact" );
			return 0;
		}

		// deactivate break/continue
		m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
		m_syms[ contLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
	}

	AddInstruction( OPCODE_JUMP, loopLabel );

	AddInstruction( OPCODE_TARGET, endLabel );

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Do( ScriptParseTree* node )
{
	int loopLabel = m_syms.AddUniqueLabel();
	int endLabel  = m_syms.AddUniqueLabel();
	int contLabel = m_syms.AddUniqueLabel();

	AddInstruction( OPCODE_TARGET, loopLabel );

	// loop block
	if ( node->m_child[ 0 ] )
	{
		// activate break/continue for this block
		m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_BREAK;
		m_syms[ contLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_CONTINUE;

		CodeGen( node->m_child[ 0 ] );

		// deactivate break/continue
		m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
		m_syms[ contLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
	}

	AddInstruction( OPCODE_TARGET, contLabel );

	// loop condition
	if ( node->m_child[ 1 ] )
	{
		CodeGen( node->m_child[ 1 ] );
		AddInstruction( OPCODE_BRANCH_TRUE, loopLabel );
	}

	AddInstruction( OPCODE_TARGET, endLabel );

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_And( ScriptParseTree* node )
{
	int endLabel = m_syms.AddUniqueLabel();
	CodeGen( node->m_child[ 0 ] );
	AddInstruction( OPCODE_BRANCH_FALSE_NO_POP, endLabel );
	CodeGen( node->m_child[ 1 ] );
	AddInstruction( OPCODE_LOGICAL_AND, 0 );
	AddInstruction( OPCODE_TARGET, endLabel );

	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Or( ScriptParseTree* node )
{
	int endLabel = m_syms.AddUniqueLabel();
	CodeGen( node->m_child[ 0 ] );
	AddInstruction( OPCODE_BRANCH_TRUE_NO_POP, endLabel );
	CodeGen( node->m_child[ 1 ] );
	AddInstruction( OPCODE_LOGICAL_OR, 0 );
	AddInstruction( OPCODE_TARGET, endLabel );

	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_StatementExpr( ScriptParseTree* node )
{
	int stackUsed = CodeGen( node->m_child[ 0 ] );
	while ( stackUsed-- )
	{
		AddInstruction( OPCODE_POP, 0 );
	}

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_String( ScriptParseTree* node )
{
	// add this string to the global string list
	int stringID = m_syms.Strings().Add( node->m_value.p );

	AddInstruction( OPCODE_PUSH_STRING, stringID );

	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Variable( ScriptParseTree* node )
{
	ScriptParseTree* indexNode = node->m_child[ 0 ];
	int pushed = 0;
	int index = node->m_value.i;

	if ( indexNode && indexNode->m_token != TOKEN_CONST_INT )
	{
		if ( CodeGen( indexNode ) != 1 )
		{
			m_err.Error( node->m_file, node->m_line, "array index expression not valid" );
			return 0;
		}
		AddInstruction( OPCODE_PUSH_VAR_INDEXED, m_syms[ index ].data );
		pushed++;
	}
	else
	{
		if ( indexNode )
		{
			int offset = indexNode->m_value.i;
			if ( offset < 0 || offset >= m_syms[ index ].size )
			{
				m_err.Error( indexNode->m_file, indexNode->m_line, "array index out of range (%d)", offset );
				return 0;
			}
			AddInstruction( OPCODE_PUSH_VAR, m_syms[ index ].data + offset );
			pushed++;
		}
		else
		{
			// push var, or all elements if this var is an array
			for ( int i = 0 ; i < m_syms[ index ].size ; i++ )
			{
				if ( m_syms[ index ].data >= 0 )
					AddInstruction( OPCODE_PUSH_VAR, m_syms[ index ].data + i );
				else
					AddInstruction( OPCODE_PUSH_VAR, m_syms[ index ].data - i );

				pushed++;
			}
		}
	}

	return pushed;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Label( ScriptParseTree* node )
{
	// make sure the label isn't also a function name
	if ( m_syms.FindFunctionByName( node->m_value.p, m_currentScope, true ) >= 0 )
	{
		m_err.Error( node->m_file, node->m_line, "label '%s' has function as same name", node->m_value.p );
		return 0;
	}

	// make sure label isn't already defined by us
	int index = m_syms.FindGotoLabelByName( node->m_value.p, m_currentScope, false, true );
	if ( index >= 0 )
	{
		// it could just be a forward decl, make sure.
		int data = m_syms[ index ].data;
		if ( data >= 0 )
		{
			m_err.Error( node->m_file, node->m_line, "label '%s' defined multiple times", node->m_value.p );
			return 0;
		}
		m_syms[ index ].data = 0;
	}
	else
	{
		index = m_syms.AddSymbol( ScriptSymbols::SYMBOL_LABEL, node->m_value.p, 0, m_currentScope );
	}
	AddInstruction( OPCODE_TARGET, index );

	// add to exported labels if in the global scope
	if ( m_currentScope == 0 )
	{
		m_syms[ index ].stringID = m_syms.Strings().Add( m_syms[ index ].name );
		m_syms.IncOutputLabels( 1 );
	}

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Switch( ScriptParseTree* node )
{
	// we need a label for the end and for the condition tree code
	int endLabel  = m_syms.AddUniqueLabel();
	int condLabel = m_syms.AddUniqueLabel();

	// jump to our condition block below
	AddInstruction( OPCODE_JUMP, condLabel );

	// add a label for the switch so the "case" and "default" tokens can associate themselves
	// with us.  when we're finished we kill the data tree so we don't look active anymore.
	ScriptSwitchTable* table = new ScriptSwitchTable;
	int switchIndex = m_syms.AddSymbol( ScriptSymbols::SYMBOL_SWITCH, "-switch", 0, m_currentScope );
	m_syms[ switchIndex ].data = (int) table;
	m_syms[ endLabel ].subtype  = ScriptSymbols::SYMBOL_SUBTYPE_BREAK;

	// insert switch code
	if ( CodeGen( node->m_child[ 1 ] ) != 0 )
	{
		m_err.Error( node->m_file, node->m_line, "internal error: switch block has stack residue!" );
		return 0;
	}

	// end of the block
	AddInstruction( OPCODE_JUMP, endLabel );

	// code to divert the switch to the right code
	AddInstruction( OPCODE_TARGET, condLabel );
	if ( CodeGen( node->m_child[ 0 ] ) != 1 )
	{
		m_err.Error( node->m_file, node->m_line, "switch expression invalid" );
		return 0;
	}
	AddInstruction( OPCODE_SWITCH, m_switchTable.AddTable( table ) );

	// final end label
	AddInstruction( OPCODE_TARGET, endLabel );

	// we're done with this block, mark it as inactive and free our local memory
	m_syms[ endLabel ].subtype = ScriptSymbols::SYMBOL_SUBTYPE_NORMAL;
	m_syms[ switchIndex ].data = 0;
	delete table;

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Case( ScriptParseTree* node )
{
	// find our innermost switch
	int switchLabel = m_syms.FindParentSwitch( m_currentScope );
	if ( switchLabel < 0 )
	{
		m_err.Error( node->m_file, node->m_line, "invalid case statement, not inside switch block" );
		return 0;
	}

	// add our branch target
	int labelIndex = m_syms.AddUniqueLabel();
	AddInstruction( OPCODE_TARGET, labelIndex );

	// add it to our values table
	ScriptSwitchTable* values = (ScriptSwitchTable*) m_syms[ switchLabel ].data;
	if ( node->m_token == TOKEN_DEFAULT )
	{
		if ( ! values->SetDefault( labelIndex ) )
		{
			m_err.Error( node->m_file, node->m_line, "default case used multiple times" );
			return 0;
		}
	}
	else
	{
		if ( ! values->Add( node->m_value.i, labelIndex ) )
		{
			m_err.Error( node->m_file, node->m_line, "case '%d' used multiple times", node->m_value.i );
			return 0;
		}
	}

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Goto( ScriptParseTree* node )
{
	// add this identifier if it's not defined already, since we can forward decl gotos
	int index = m_syms.FindGotoLabelByName( node->m_child[ 0 ]->m_value.p, m_currentScope, true, true );
	if ( index < 0 )
	{
		// assign it a bogus value, so we can trap if not defined at code-gen.
		index = m_syms.AddSymbol( ScriptSymbols::SYMBOL_LABEL, node->m_child[ 0 ]->m_value.p, -1, m_currentScope );
	}
	AddInstruction( OPCODE_JUMP, index );
	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_PostIncDec( ScriptParseTree* node )
{
	ScriptParseTree* ident = node->m_child[ 0 ];
	int index = ident->m_value.i;

	// different code if this is an indexed variable
	if ( ident->m_child[ 0 ] )
	{
		if ( CodeGen( ident->m_child[ 0 ] ) != 1 )	// array index            STACK: index
		{
			m_err.Error( node->m_file, node->m_line, "post ++/-- has invalid array index" );
			return 0;
		}
		AddInstruction( OPCODE_DUP, 0 );							// remember it for store  STACK: index, index
		AddInstruction( OPCODE_PUSH_VAR_INDEXED, m_syms[ index ].data );	//			  STACK: index, arr[index]
		AddInstruction( OPCODE_DUP, 0 );							// save value of exp      STACK: index, arr[index], arr[index]
		AddInstruction( OPCODE_PUSH_INT, 1 );					// value to add/sub       STACK: index, arr[index], arr[index], 1
		AddInstruction( ScriptUtil::TokenOpcode( node->m_token ), 0 );  //           STACK: index, arr[index], arr[index] + 1
		AddInstruction( OPCODE_SWAP, 0x0102 );             //                        STACK: arr[index], index, arr[index] + 1
		AddInstruction( OPCODE_SWAP, 0x0001 );             //                        STACK: arr[index], arr[index] + 1, index
		AddInstruction( OPCODE_STORE_INDEXED, m_syms[ index ].data );     //			  STACK: arr[index], arr[index] + 1
		AddInstruction( OPCODE_POP, 0 );                   //                        STACK: arr[index]
	}
	else
	{
		AddInstruction( OPCODE_PUSH_VAR, m_syms[ index ].data );
		AddInstruction( OPCODE_DUP, 0 );
		AddInstruction( OPCODE_PUSH_INT, 1 );
		AddInstruction( ScriptUtil::TokenOpcode( node->m_token ), 0 );
		AddInstruction( OPCODE_POP_AND_STORE, m_syms[ index ].data );
	}

	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Assignment( ScriptParseTree* node )
{
	// find the variable
	ScriptParseTree* nameNode  = node->m_child[ 0 ];
	ScriptParseTree* exprNode  = node->m_child[ 1 ];
	ScriptParseTree* indexNode = nameNode->m_child[ 0 ];
	int index                  = nameNode->m_value.i;

	// straight assignment with '='
	if ( node->m_token == TOKEN_ASSIGN )
	{
		// if we have an array with no index, we better use list assignment...
		if ( m_syms[ index ].size > 1 && !indexNode )
		{
			return CodeGen_ListAssign( node );
		}

		if ( CodeGen( exprNode ) != 1 )
		{
			m_err.Error( node->m_file, node->m_line, "assignment has bogus expression on right-hand-side" );
			return 0;
		}

		if ( indexNode && indexNode->m_token != TOKEN_CONST_INT )
		{
			if ( CodeGen( indexNode ) != 1 )
			{
				m_err.Error( node->m_file, node->m_line, "array in assignment has invalid index expression" );
				return 0;
			}
			AddInstruction( OPCODE_STORE_INDEXED, m_syms[ index ].data );
		}
		else
		{
			int offset = ( indexNode ) ? indexNode->m_value.i : 0;
			if ( offset < 0 || offset >= m_syms[ index ].size )
			{
				m_err.Error( indexNode->m_file, indexNode->m_line, "array index out of range (%d)", offset );
				return 0;
			}
			AddInstruction( OPCODE_STORE, m_syms[ index ].data + offset );
		}
	}

	// assignement with '+=', '-=', ...
	else
	{
		// with arrays we have special code so we don't evaluate the index more than once.
		if ( indexNode && indexNode->m_token != TOKEN_CONST_INT )
		{
			if ( CodeGen( indexNode ) != 1 )				// array index					STACK: index
			{
				m_err.Error( node->m_file, node->m_line, "array in assignment has invalid index expression" );
				return 0;
			}
			AddInstruction( OPCODE_DUP, 0 );							// remember it for store	STACK: index, index
			AddInstruction( OPCODE_PUSH_VAR_INDEXED, m_syms[ index ].data );	//				STACK: index, arr[index]
			if ( CodeGen( exprNode ) != 1 )					// expression					STACK: index, arr[index], expr
			{
				m_err.Error( node->m_file, node->m_line, "assignment has invalid right-hand-side expression" );
				return 0;
			}
			AddInstruction( ScriptUtil::TokenOpcode( node->m_token ), 0 ); //					STACK: index, arr[index] op expr
			AddInstruction( OPCODE_SWAP, 0x0001 );					//									STACK: arr[index] op expr, index
			AddInstruction( OPCODE_STORE_INDEXED, m_syms[ index ].data );		//				STACK: arr[index] op expr
		}
		else
		{
			int offset = ( indexNode ) ? indexNode->m_value.i : 0;
			if ( offset < 0 || offset >= m_syms[ index ].size )
			{
				m_err.Error( node->m_file, node->m_line, "array index out of range (%d)", offset );
				return 0;
			}
			AddInstruction( OPCODE_PUSH_VAR, m_syms[ index ].data + offset );
			if ( CodeGen( exprNode ) != 1 )
			{
				m_err.Error( node->m_file, node->m_line, "assignment has invalid right-hand-side expression" );
				return 0;
			}
			AddInstruction( ScriptUtil::TokenOpcode( node->m_token ), 0 );
			AddInstruction( OPCODE_STORE, m_syms[ index ].data + offset );
		}
	}

	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_ArithUnary( ScriptParseTree* node )
{
	if ( !node->m_child[ 0 ] || CodeGen( node->m_child[ 0 ] ) != 1 )
	{
		m_err.Error( node->m_file, node->m_line, "unary math requires expression left of %s", ScriptUtil::TokenName( node->m_token ) );
		return 0;
	}
	AddInstruction( ScriptUtil::TokenOpcode( node->m_token ), 0 );
	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_ArithBinary( ScriptParseTree* node )
{
	if ( !node->m_child[ 0 ] || CodeGen( node->m_child[ 0 ] ) != 1 )
	{
		m_err.Error( node->m_file, node->m_line, "binary math requires expression left of %s", ScriptUtil::TokenName( node->m_token ) );
		return 0;
	}
	if ( !node->m_child[ 1 ] || CodeGen( node->m_child[ 1 ] ) != 1 )
	{
		m_err.Error( node->m_file, node->m_line, "binary math requires expression right of %s", ScriptUtil::TokenName( node->m_token ) );
		return 0;
	}
	AddInstruction( ScriptUtil::TokenOpcode( node->m_token ), 0 );
	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Print( ScriptParseTree* node )
{
	if ( !node->m_child[ 0 ] )
	{
		m_err.Error( node->m_file, node->m_line, "print has invalid expression" );
	}
	int parms = CodeGen( node->m_child[ 0 ] );
	AddInstruction( OPCODE_PUSH_INT, parms );
	AddInstruction( OPCODE_PRINT, 0 );
	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Sleep( ScriptParseTree* node )
{
	if ( !node->m_child[ 0 ] || CodeGen( node->m_child[ 0 ] ) != 1 )
	{
		m_err.Error( node->m_file, node->m_line, "sleep has invalid expression" );
		return 0;
	}
	AddInstruction( OPCODE_SLEEP, 0 );
	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Return( ScriptParseTree* node )
{
	int funcIndex = node->m_value.i;

	// check to see if return requires return parameters
	int returnNeeded = m_syms[ funcIndex ].numOut;
	int returnHave   = ( node->m_child[ 0 ] ) ? CodeGen( node->m_child[ 0 ] ) : 0;
	if ( returnNeeded != returnHave )
	{
		m_err.Error( node->m_file, node->m_line, "return has invalid number of return parameters" );
		return 0;
	}

	AddInstruction( OPCODE_RETURN, 0 );

	// even though we technically leave a result on the stack, this result isn't to be
	// interpreted by this function, but rather by the calling function, so lie here
	// and say we have zero stack impact.
	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_ListAssign( ScriptParseTree* node )
{
	int rightArgs = 0;

	// evaluate all the expressions on the right
	rightArgs += CodeGen( node->m_child[ 1 ] );

	// fill in the idents, in reverse order since the results are on the stack
	// right-to-left now.
	int leftArgs = CodeGen_ListAssignLHS( node->m_child[ 0 ] );

	if ( leftArgs != rightArgs )
	{
		m_err.Error( node->m_file, node->m_line, "list assignment has mismatching number of args on left than right" );
		return 0;
	}

	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_ListAssignLHS( ScriptParseTree* node )
{
	int poppedArgs = 0;

	// and then us
	switch ( node->m_token )
	{
		case TOKEN_VARIABLE:
			{
				int index = node->m_value.i;
				ScriptParseTree* indexNode = node->m_child[ 0 ];
				if ( indexNode && indexNode->m_token != TOKEN_CONST_INT )
				{
					if ( CodeGen( indexNode ) != 1 )
					{
						m_err.Error( node->m_file, node->m_line, "array in assignment has invalid index expression" );
						return 0;
					}
					AddInstruction( OPCODE_STORE_INDEXED, m_syms[ index ].data );
					AddInstruction( OPCODE_POP, 0 );
					poppedArgs++;
				}
				else
				{
					if ( indexNode )
					{
						int offset = indexNode->m_value.i;
						if ( offset < 0 || offset >= m_syms[ index ].size )
						{
							m_err.Error( node->m_file, node->m_line, "array index out of range (%d)", offset );
							return 0;
						}
						AddInstruction( OPCODE_POP_AND_STORE, m_syms[ index ].data + offset );
						poppedArgs++;
					}
					else
					{
						// pop all vars in array
						for ( int i = m_syms[ index ].size - 1 ; i >= 0 ; i-- )
						{
							if ( m_syms[ index ].data >= 0 )
								AddInstruction( OPCODE_POP_AND_STORE, m_syms[ index ].data + i );
							else
								AddInstruction( OPCODE_POP_AND_STORE, m_syms[ index ].data - i );

							poppedArgs++;
						}
					}
				}
			}
			break;

		case TOKEN_EXPR_LIST:
			// do all the siblings..., in reverse order
			if ( node->m_child[ 3 ] ) poppedArgs += CodeGen_ListAssignLHS( node->m_child[ 3 ] );
			if ( node->m_child[ 2 ] ) poppedArgs += CodeGen_ListAssignLHS( node->m_child[ 2 ] );
			if ( node->m_child[ 1 ] ) poppedArgs += CodeGen_ListAssignLHS( node->m_child[ 1 ] );
			if ( node->m_child[ 0 ] ) poppedArgs += CodeGen_ListAssignLHS( node->m_child[ 0 ] );
			break;

		default:
			m_err.Error( node->m_file, node->m_line, "bogus token in listAssignLHS '%s'", ScriptUtil::TokenName( node->m_token ) );
			break;
	}

	return poppedArgs;
}

//===========================================================================

int ScriptCodeGen::CodeGen_BreakContinue( ScriptParseTree* node )
{
	int subType = ( node->m_token == TOKEN_BREAK ) ? ScriptSymbols::SYMBOL_SUBTYPE_BREAK : ScriptSymbols::SYMBOL_SUBTYPE_CONTINUE;
	int label = m_syms.FindLabelBySubtype( subType, m_currentScope, true, false );
	if ( label < 0 )
	{
		m_err.Error( node->m_file, node->m_line, "%s used outside of valid scope to use it", ScriptUtil::TokenName( node->m_token ) );
		return 0;
	}
	AddInstruction( OPCODE_JUMP, label );
	return 0;
}

//===========================================================================

int ScriptCodeGen::CodeGen_Sizeof( ScriptParseTree* node )
{
	ScriptParseTree* nameNode  = node->m_child[ 0 ];
	ScriptParseTree* indexNode = nameNode->m_child[ 0 ];
	int index                  = nameNode->m_value.i;

	if ( indexNode )
	{
		AddInstruction( OPCODE_PUSH_INT, 1 );
	}
	else
	{
		AddInstruction( OPCODE_PUSH_INT, m_syms[ index ].size );
	}

	return 1;
}

//===========================================================================

int ScriptCodeGen::CodeGen_CondExpr( ScriptParseTree* node )
{
	int expr2Label = m_syms.AddUniqueLabel();
	int doneLabel  = m_syms.AddUniqueLabel();

	if ( CodeGen( node->m_child[ 0 ] ) != 1 )
	{
		m_err.Error( node->m_file, node->m_line, "conditional expression has invalid condition" );
		return 0;
	}

	AddInstruction( OPCODE_BRANCH_FALSE, expr2Label );

	int returnExp1 = CodeGen( node->m_child[ 1 ] );
	AddInstruction( OPCODE_JUMP, doneLabel );

	AddInstruction( OPCODE_TARGET, expr2Label );
	int returnExp2 = CodeGen( node->m_child[ 2 ] );

	AddInstruction( OPCODE_TARGET, doneLabel );

	if ( returnExp1 != returnExp2 )
	{
		m_err.Error( node->m_file, node->m_line, "conditional expression has different number of elements for each expression" );
		return 0;
	}

	return returnExp1;
}

//===========================================================================

int ScriptCodeGen::CodeGen( ScriptParseTree* node )
{
	int stackUsed = 0;

	// mark node's source file and line for debug info
	m_currentFile = node->m_file;
	m_currentLine = node->m_line;

	switch ( node->m_token )
	{
		case TOKEN_FUNC_CALL:			return CodeGen_FuncCall( node );
		case TOKEN_FUNC_DECL:			return CodeGen_FuncDecl( node );
		case TOKEN_SCOPE_BEGIN:			return CodeGen_ScopeBegin( node );
		case TOKEN_FOR:					return CodeGen_For( node );
		case TOKEN_IF:						return CodeGen_If( node );
		case TOKEN_WHILE:					return CodeGen_While( node );
		case TOKEN_DO:						return CodeGen_Do( node );
		case TOKEN_LOGICAL_AND:			return CodeGen_And( node );
		case TOKEN_LOGICAL_OR:			return CodeGen_Or( node );
		case TOKEN_STATEMENT_EXPR:		return CodeGen_StatementExpr( node );
		case TOKEN_STRING:				return CodeGen_String( node );
		case TOKEN_VARIABLE:				return CodeGen_Variable( node );
		case TOKEN_LABEL:					return CodeGen_Label( node );
		case TOKEN_GOTO:					return CodeGen_Goto( node );
		case TOKEN_SWITCH:				return CodeGen_Switch( node );
		case TOKEN_CASE:					return CodeGen_Case( node );
		case TOKEN_DEFAULT:				return CodeGen_Case( node );
		case TOKEN_POST_INCREMENT:		return CodeGen_PostIncDec( node );
		case TOKEN_POST_DECREMENT:		return CodeGen_PostIncDec( node );
		case TOKEN_ASSIGN:				return CodeGen_Assignment( node );
		case TOKEN_PLUS_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_MINUS_EQUALS:		return CodeGen_Assignment( node );
		case TOKEN_MULT_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_DIV_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_MOD_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_SHIFT_L_EQUALS:		return CodeGen_Assignment( node );
		case TOKEN_SHIFT_R_EQUALS:		return CodeGen_Assignment( node );
		case TOKEN_AND_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_OR_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_XOR_EQUALS:			return CodeGen_Assignment( node );
		case TOKEN_LESS_THAN:			return CodeGen_ArithBinary( node );
		case TOKEN_LESS_THAN_EQUAL:	return CodeGen_ArithBinary( node );
		case TOKEN_GTR_THAN:				return CodeGen_ArithBinary( node );
		case TOKEN_GTR_THAN_EQUAL:		return CodeGen_ArithBinary( node );
		case TOKEN_EQUALS:				return CodeGen_ArithBinary( node );
		case TOKEN_NOT_EQUALS:			return CodeGen_ArithBinary( node );
		case TOKEN_PLUS:					return CodeGen_ArithBinary( node );
		case TOKEN_MINUS:					return CodeGen_ArithBinary( node );
		case TOKEN_MULT:					return CodeGen_ArithBinary( node );
		case TOKEN_DIV:					return CodeGen_ArithBinary( node );
		case TOKEN_MOD:					return CodeGen_ArithBinary( node );
		case TOKEN_SHIFT_L:				return CodeGen_ArithBinary( node );
		case TOKEN_SHIFT_R:				return CodeGen_ArithBinary( node );
		case TOKEN_BITWISE_OR:			return CodeGen_ArithBinary( node );
		case TOKEN_BITWISE_AND:			return CodeGen_ArithBinary( node );
		case TOKEN_BITWISE_XOR:			return CodeGen_ArithBinary( node );
		case TOKEN_LOGICAL_NOT:			return CodeGen_ArithUnary( node );
		case TOKEN_BITWISE_NOT:			return CodeGen_ArithUnary( node );
		case TOKEN_UNARY_MINUS:			return CodeGen_ArithUnary( node );
		case TOKEN_ABS:               return CodeGen_ArithUnary( node );
		case TOKEN_PRINT:					return CodeGen_Print( node );
		case TOKEN_SLEEP:					return CodeGen_Sleep( node );
		case TOKEN_RETURN:				return CodeGen_Return( node );
		case TOKEN_LIST_ASSIGN:			return CodeGen_ListAssign( node );
		case TOKEN_BREAK:					return CodeGen_BreakContinue( node );
		case TOKEN_CONTINUE:				return CodeGen_BreakContinue( node );
		case TOKEN_SIZEOF:				return CodeGen_Sizeof( node );
		case TOKEN_COND_EXPR:			return CodeGen_CondExpr( node );
		case TOKEN_CONST_INT:			AddInstruction( OPCODE_PUSH_INT, node->m_value );		return 1;
		case TOKEN_CONST_FLOAT:			AddInstruction( OPCODE_PUSH_FLOAT, node->m_value );	return 1;
		case TOKEN_THIS:              AddInstruction( OPCODE_PUSH_OWNER, node->m_value );	return 1;
		case TOKEN_GROUP:					break;
		case TOKEN_ARG:					break;
		case TOKEN_EXPR_LIST:			break;
		case TOKEN_NONE:					return 0;

		default:
			printf( "ERROR: unknown token in codegen '%s'\n", ScriptUtil::TokenName( node->m_token ) );
			break;
	}

	// we're just a container node, so do children.
	if ( node->m_child[ 0 ] ) { stackUsed += CodeGen( node->m_child[ 0 ] ); }
	if ( node->m_child[ 1 ] ) { stackUsed += CodeGen( node->m_child[ 1 ] ); }
	if ( node->m_child[ 2 ] ) { stackUsed += CodeGen( node->m_child[ 2 ] ); }
	if ( node->m_child[ 3 ] ) { stackUsed += CodeGen( node->m_child[ 3 ] ); }

	return stackUsed;
}

//===========================================================================

int ScriptCodeGen::CountOpcodes()
{
	int count = 0;
	Instruction* n;

	for ( n = m_head ; n ; n = n->next )
		count += ScriptUtil::OpcodeSize( n->op );

	count++;		// we're going to add a final "end" opcode

	// align up to long word alignment.
	while ( ( count * sizeof( ScriptOpcode ) ) & 3 )
		count++;

	return count;
}

//===========================================================================

ScriptExecutable* ScriptCodeGen::CreateExecutable( ScriptParseTree* tree, const char* sourceFilename, bool debug )
{
	Instruction* n;
	int i, j;
	int pc;
	char* executableBuf;

	// first pass gen of the opcodes
	Reset();
	CodeGen( tree );

	// add the source filename to the string table
	int sourceFilenameString = m_syms.Strings().Add( sourceFilename );

	// optimize our code
	Optimize();

	// we need to calculate the total size for the executable block
	int numOpcodes   = CountOpcodes();
	int numDebugInfo = debug ? numOpcodes : 0;
	int numVariables = m_syms.NumOutputVariables();
	int numStatics   = m_syms.NumOutputStatics();
	int numFunctions = m_syms.NumOutputCFunctions();
	int numStrings   = m_syms.Strings().Count();
	int numSwitch    = m_switchTable.Count();

	// count exported variables
	int numExportedVariables = 0;
	for ( i = 0 ; i < m_syms.Count() ; i++ )
		if ( m_syms[ i ].size > 0 && m_syms[ i ].data >= 0 && m_syms[ i ].scope == 0 )
			numExportedVariables++;
	if ( numExportedVariables )
		numExportedVariables = 2 + ( numExportedVariables * 3 ) / 2;	// pad up so hash table has some breathing room

	// exported label hash
	int numExportedLabels = m_syms.NumOutputLabels();
	if ( numExportedLabels )
		numExportedLabels = 2 + ( numExportedLabels * 3 ) / 2;	// pad up so hash table has some breathing room

	int fileSize     = sizeof( ScriptExecutable )														+
						    sizeof( ScriptOpcode )								* numOpcodes				+
							 sizeof( ScriptExecutable::DebugInfo )       * numDebugInfo          +
							 sizeof( ScriptExecutable::Variable )			* numVariables				+
							 sizeof( ScriptExecutable::StaticVariable )  * numStatics            +
							 sizeof( ScriptExecutable::Function )			* numFunctions				+
							 sizeof( ScriptExecutable::VariableLookup )	* numExportedVariables	+
							 sizeof( ScriptExecutable::LabelLookup )		* numExportedLabels		+
							 sizeof( ScriptExecutable::SwitchTable )     * numSwitch             +
							 sizeof( char* )										* numStrings				+
							 m_syms.Strings().ByteSize();

	fileSize = ( fileSize + 3 ) & ~3;		// always multiples of 4 bytes long

	// create the block
	executableBuf = new char [ fileSize ];
	memset( executableBuf, 0, fileSize );

	// fill in the header
	ScriptExecutable* exe = (ScriptExecutable*) executableBuf;
	memcpy( exe->m_fileID, SCRIPT_FILE_ID, sizeof( exe->m_fileID ) );
	exe->m_fileVersion  = SCRIPT_FILE_VERSION;
	exe->m_fileStatus   = SCRIPT_STATUS_READY;
	exe->m_fileSize     = fileSize;
	exe->m_numOpcodes   = numOpcodes;
	exe->m_numDebugInfo = numDebugInfo;
	exe->m_numVariables = numVariables;
	exe->m_numStatics   = numStatics;
	exe->m_numFunctions = numFunctions;
	exe->m_numStrings   = numStrings;
	exe->m_numVariableLookup = numExportedVariables;
	exe->m_numLabelLookup    = numExportedLabels;
	exe->m_numSwitchTable = numSwitch;
	exe->m_opcodes        = (ScriptOpcode*) &( exe[ 1 ] );
	exe->m_debugInfo      = (ScriptExecutable::DebugInfo*)      &( exe->m_opcodes[ numOpcodes ] );
	exe->m_variables      = (ScriptExecutable::Variable*)			&( exe->m_debugInfo[ numDebugInfo ] );
	exe->m_statics        = (ScriptExecutable::StaticVariable*) &( exe->m_variables[ numVariables ] );
	exe->m_functions      = (ScriptExecutable::Function*)			&( exe->m_statics[ numStatics ] );
	exe->m_variableLookup = (ScriptExecutable::VariableLookup*) &( exe->m_functions[ numFunctions ] );
	exe->m_labelLookup    = (ScriptExecutable::LabelLookup*)		&( exe->m_variableLookup[ exe->m_numVariableLookup ] );
	exe->m_switchTable    = (ScriptExecutable::SwitchTable*)    &( exe->m_labelLookup[ exe->m_numLabelLookup ] );
	exe->m_strings        = (char**)										&( exe->m_switchTable[ exe->m_numSwitchTable ] );
	char* stringBase      = (char*)										&( exe->m_strings[ numStrings ] );

	// preinit the hash tables to empty
	for ( i = 0 ; i < exe->m_numVariableLookup ; i++ )
	{
		exe->m_variableLookup[ i ].hash = 0;
		exe->m_variableLookup[ i ].varIndex = -1;
	}
	for ( i = 0 ; i < exe->m_numLabelLookup ; i++ )
	{
		exe->m_labelLookup[ i ].hash = 0;
		exe->m_labelLookup[ i ].name = NULL;
		exe->m_labelLookup[ i ].pc   = -1;
	}

	// pre-calculate branch target destinations
	for ( n = m_head, pc = 0 ; n ; pc += ScriptUtil::OpcodeSize( n->op ), n = n->next )
	{
		if ( n->op == OPCODE_TARGET )
		{
			m_syms[ n->subcode.i ].data = pc;
		}
	}

	// verify all branch targets are defined
	for ( i = 0 ; i < m_syms.Count() ; i++ )
	{
		if ( m_syms[ i ].type == ScriptSymbols::SYMBOL_LABEL && m_syms[ i ].data < 0 )
		{
			// if this label is defined in a parent scope, copy its data
			int index = m_syms.FindGotoLabelByName( m_syms[ i ].name, m_syms[ i ].scope, true, false );
			if ( index >= 0 )
				m_syms[ i ].data = m_syms[ index ].data;
			else
			{
				m_err.Error( "codegen", 0, "branch target '%s' used but never declared", m_syms[ i ].name );
			}
		}
		else if ( m_syms[ i ].type == ScriptSymbols::SYMBOL_FUNCTION && m_syms[ i ].data < 0 )
		{
			m_err.Error( "codegen", 0, "function '%s' has no body", m_syms[ i ].name );
		}
	}

	// bail now if we've accumulated any errors
	if ( m_err.ErrorCount() ) goto Error;

	// fill in all the strings
	for ( i = 0 ; i < m_syms.Strings().Count() ; i++ )
	{
		exe->m_strings[ i ] = stringBase;
		int len = (int)strlen( m_syms.Strings()[ i ] ) + 1;
		memcpy( stringBase, m_syms.Strings()[ i ], len );
		stringBase += len;
	}

	// fill in the source filename
	exe->m_sourceFilename = exe->m_strings[ sourceFilenameString ];

	// fill in all the variable names and types
	int varIndex, staticVarIndex;
	varIndex = staticVarIndex = 0;
	for ( i = 0 ; i < m_syms.Count() ; i++ )
	{
		if ( m_syms[ i ].size > 0 )
		{
			ScriptVar::VarType targetType = (ScriptVar::VarType) -1;
			switch ( m_syms[ i ].type )
			{
				case ScriptSymbols::SYMBOL_VARIABLE_INT:		targetType = ScriptVar::TYPE_INT;    break;
				case ScriptSymbols::SYMBOL_VARIABLE_FLOAT:	targetType = ScriptVar::TYPE_FLOAT;  break;
				case ScriptSymbols::SYMBOL_VARIABLE_STRING: targetType = ScriptVar::TYPE_STRING; break;
			}

			if ( m_syms[ i ].data >= 0 )	// non-static
			{
				// enter into lookup table if global variable
				if ( m_syms[ i ].scope == 0 )
				{
					unsigned long hash = Hash::CalcChecksum( m_syms[ i ].name, strlen( m_syms[ i ].name ) );
					for ( j = hash % exe->m_numVariableLookup ; exe->m_variableLookup[ j ].varIndex >= 0 ; )
						j = ( j + 1 ) % exe->m_numVariableLookup;
					exe->m_variableLookup[ j ].hash     = hash;
					exe->m_variableLookup[ j ].varIndex = varIndex;
				}

				for ( j = 0 ; j < m_syms[ i ].size ; j++ )
				{
					exe->m_variables[ varIndex ].name = exe->m_strings[ m_syms[ i ].stringID ];
					exe->m_variables[ varIndex ].type = targetType;
					exe->m_variables[ varIndex ].size = m_syms[ i ].size - j;
					varIndex++;
				}
			}
			else	// static
			{
				for ( j = 0 ; j < m_syms[ i ].size ; j++ )
				{
					ScriptValue v;
					v.i = 0;
					exe->m_statics[ staticVarIndex ].name = exe->m_strings[ m_syms[ i ].stringID ];
					exe->m_statics[ staticVarIndex ].var.Set( targetType, v );
					exe->m_statics[ staticVarIndex ].size = m_syms[ i ].size - j;
					staticVarIndex++;
				}
			}
		}
	}
	ASSERT( varIndex == exe->m_numVariables );
	ASSERT( staticVarIndex == exe->m_numStatics );

	// fill in function names
	int externFuncIndex;
	externFuncIndex = 0;
	for ( i = 0 ; i < m_syms.Count() ; i++ )
	{
		if ( m_syms[ i ].type == ScriptSymbols::SYMBOL_C_FUNCTION && m_syms[ i ].data >= 0 )
		{
			int funcIndex = m_syms[ i ].data;
			ASSERT( exe->m_functions[ funcIndex ].name == 0 );
			exe->m_functions[ funcIndex ].name = exe->m_strings[ m_syms[ i ].stringID ];
			exe->m_functions[ funcIndex ].nameHash = Hash::CalcChecksum( exe->m_functions[ funcIndex ].name, strlen( exe->m_functions[ funcIndex ].name ) );
			exe->m_functions[ funcIndex ].func = 0;
			if ( m_syms[ i ].numIn & 0x1000 )	// variable args?
				exe->m_functions[ funcIndex ].stackDelta = 0x1000;
			else
				exe->m_functions[ funcIndex ].stackDelta = m_syms[ i ].numOut - m_syms[ i ].numIn;
			externFuncIndex++;
		}
	}
	ASSERT( externFuncIndex == exe->m_numFunctions );

	// fill in label names
	int labelIndex;
	labelIndex = 0;
	for ( i = 0 ; i < m_syms.Count() ; i++ )
	{
		if ( ( m_syms[ i ].type == ScriptSymbols::SYMBOL_LABEL || m_syms[ i ].type == ScriptSymbols::SYMBOL_FUNCTION ) && m_syms[ i ].stringID >= 0 )
		{
			// enter into hash table
			unsigned int hash = Hash::CalcChecksum( m_syms[ i ].name, strlen( m_syms[ i ].name ) );
			for ( j = hash % exe->m_numLabelLookup ; exe->m_labelLookup[ j ].pc >= 0 ; )
				j = ( j + 1 ) % exe->m_numLabelLookup;
			exe->m_labelLookup[ j ].hash = hash;
			exe->m_labelLookup[ j ].name = exe->m_strings[ m_syms[ i ].stringID ];
			exe->m_labelLookup[ j ].pc   = m_syms[ i ].data;
		}
	}

	// copy switch table over
	for ( i = 0 ; i < m_switchTable.Count() ; i++ )
	{
		exe->m_switchTable[ i ].value = m_switchTable[ i ].value;
		if ( m_switchTable[ i ].targetLabel >= 0 )
			exe->m_switchTable[ i ].pc = m_syms[ m_switchTable[ i ].targetLabel ].data;
		else
			exe->m_switchTable[ i ].pc = -1;
	}

	// fill in the debug Info
	if ( debug )
	{
		for ( n = m_head, pc = 0 ; n ; n = n->next )
		{
			for ( i = 0 ; i < ScriptUtil::OpcodeSize( n->op ) ; i++ )
			{
				exe->m_debugInfo[ pc ].file = exe->m_strings[ n->file ];
				exe->m_debugInfo[ pc ].line = n->line;
				pc++;
			}
		}
	}
	else
	{
		exe->m_debugInfo = NULL;
	}

	// copy the instructions over
	for ( n = m_head, pc = 0 ; n ; pc += ScriptUtil::OpcodeSize( n->op ), n = n->next )
	{
		// instruction
		exe->m_opcodes[ pc ] = n->op;

		// fill in any data this instruction may need
		switch ( n->op )
		{
			// branch target
			case OPCODE_BRANCH_TRUE:
			case OPCODE_BRANCH_FALSE:
			case OPCODE_BRANCH_TRUE_NO_POP:
			case OPCODE_BRANCH_FALSE_NO_POP:
			case OPCODE_JUMP:
			case OPCODE_CALL:
			case OPCODE_CALL_C:
				exe->m_opcodes[ pc + 1 ] = m_syms[ n->subcode.i ].data;
				break;

			// some instructions need their data
			case OPCODE_SWAP:
			case OPCODE_PUSH_VAR:
			case OPCODE_PUSH_VAR_INDEXED:
			case OPCODE_PUSH_STRING:
			case OPCODE_POP_AND_STORE:
			case OPCODE_STORE:
			case OPCODE_STORE_INDEXED:
			case OPCODE_SWITCH:
				exe->m_opcodes[ pc + 1 ] = n->subcode.i;
				break;

			// ops with extra arguments in the command stream
			case OPCODE_PUSH_INT:
			case OPCODE_PUSH_FLOAT:
				{
					unsigned int rawdata = *(unsigned int*)&n->subcode;
					exe->m_opcodes[ pc + 1 ] = ( rawdata >> 16 );
					exe->m_opcodes[ pc + 2 ] = ( rawdata & 0xffff );
				}
				break;
		}
	}

	// pad to end with end opcodes
	while ( pc < exe->m_numOpcodes )
		exe->m_opcodes[ pc++ ] = OPCODE_END;

	return exe;

Error:
	if ( executableBuf ) delete [] executableBuf;
	return NULL;
}

//===========================================================================

#endif //SCRIPT_COMPILER