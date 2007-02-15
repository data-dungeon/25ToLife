//===========================================================================
//=
//= ScriptSymbols.h
//=
//===========================================================================

#ifndef _SCRIPT_SYMBOLS_H
#define _SCRIPT_SYMBOLS_H

#ifdef SCRIPT_COMPILER

//===========================================================================

#define NORMAL_SCOPE		"-scope-"
#define FUNC_SCOPE		"-funcscope-"

//===========================================================================

class ScriptStringList
{
public:
	struct StringNode { char* string ; StringNode* next; };

	ScriptStringList()
	{
		m_strings = 0;
		Reset();
	}

	~ScriptStringList()
	{
		Reset();
	}

	void Reset()
	{
		if ( m_strings )
		{
			for ( int i = 0 ; i < m_size ; i++ )
				if ( m_strings[ i ] ) delete [] m_strings[ i ];
			delete m_strings;
		}
		m_strings  = 0;
		m_maxSize  = 0;
		m_size     = 0;
		m_byteSize = 0;
	}

	int Add( const char* string )
	{
		// don't add it if we already have it
		int i;
		for ( i = 0 ; i < m_size ; i++ )
			if ( !strcmp( m_strings[ i ], string ) )
				return i;

		// grow to fit it if we need to
		if ( m_size + 1 > m_maxSize )
		{
			m_maxSize = ( m_size + 1 ) * 3 / 2;
			char** newStrings = new char* [ m_maxSize ];
			for ( i = 0 ; i < m_size ; i++ )
				newStrings[ i ] = m_strings[ i ];
			delete m_strings;
			m_strings = newStrings;
		}

		// fill in the new data
		int newIndex = m_size;
		int len = (int) strlen( string ) + 1;
		m_strings[ newIndex ] = new char[ len ];
		memcpy( m_strings[ newIndex ], string, len );

		// bump our counters
		m_size++;
		m_byteSize += len;

		return newIndex;
	}

	inline char* operator [] ( const int index )
	{
		ASSERT( index >= 0 && index < m_size );
		return m_strings[ index ];
	}

	inline int Count()    const { return m_size;     }
	inline int ByteSize() const { return m_byteSize; }

private:
	char**			m_strings;
	int				m_maxSize;
	int				m_size;
	int				m_byteSize;
};

//===========================================================================

class ScriptSymbols
{
public:
	enum SymbolType
	{
		SYMBOL_SCOPE,					// data=0
		SYMBOL_LABEL,					// data=pc of label (filled in during CreateExecutable)
		SYMBOL_VARIABLE_INT,			// data=output variable index
		SYMBOL_VARIABLE_FLOAT,		// data=output variable index
		SYMBOL_VARIABLE_STRING,		// data=output variable index
		SYMBOL_FUNCTION,				// data=pc of label (filled in during CreateExecutable)
		SYMBOL_C_FUNCTION,			// data=output function index
		SYMBOL_CONST_INT,				// data=value of int
		SYMBOL_CONST_STRING,			// data=ptr to string
		SYMBOL_STRUCT,					// data=scope of structure variables
		SYMBOL_SWITCH,					// data=ptr to switch data keeping class
	};

	enum SymbolSubType
	{
		SYMBOL_SUBTYPE_NORMAL,
		SYMBOL_SUBTYPE_CONTINUE,
		SYMBOL_SUBTYPE_BREAK,
		SYMBOL_SUBTYPE_DEFINED,
	};

	struct SymbolDef
	{
		SymbolType		type;
		SymbolSubType	subtype;
		char*				name;
		int				size;
		int				data;				// means different things based on type (see above)
		short				numIn;			// for function and c-function, the number of input parmaeters
		short				numOut;			// for function and c-function, the number of output parameters
		short				parmStart;
		short				parmEnd;
		int				scope;
		int				stringID;
		int				parentFunc;
	};

	ScriptSymbols();
	~ScriptSymbols();

	void	Reset();
	int	AddSymbol( SymbolType type, const char* name, int data, int scope );
	int	AddUniqueLabel();

	int	FindVariableByName( const char* name, int scope, int searchParentScopes );
	int	FindFunctionByName( const char* name, int scope, int searchParentScopes );
	int	FindLabelBySubtype( int subType, int scope, bool searchParentScopes, bool definedOnly );
	int	FindConstantByName( const char* name, int scope, bool searchParentScopes );
	int	FindParentSwitch( int scope );
	int	FindParentFunction( int scope );
	int	FindGotoLabelByName( const char* name, int scope, bool searchParentScopes, bool allowUnresolved );

	ScriptStringList& Strings() { return m_strings; }
	void	Print();

	inline int Count() const { return m_numSymbols; }

	inline int NumOutputVariables()  const { return m_numOutputVariables; }
	inline int NumOutputStatics()    const { return m_numOutputStatics; }
	inline int NumOutputLabels()     const { return m_numOutputLabels; }
	inline int NumOutputCFunctions() const { return m_numOutputCFunctions; }

	inline void IncOutputVariables( int count )  { m_numOutputVariables += count;  }
	inline void IncOutputStatics( int count )    { m_numOutputStatics += count; }
	inline void IncOutputLabels( int count )     { m_numOutputLabels += count;     }
	inline void IncOutputCFunctions( int count ) { m_numOutputCFunctions += count; }

	inline SymbolDef& operator [] ( const int index )
	{
		ASSERT( index >= 0 && index < m_numSymbols );
		return m_symbols[ index ];
	}

	static const char* SymbolTypeName( SymbolType type );

private:
	SymbolDef*			m_symbols;
	int					m_numSymbols;
	int					m_maxSymbols;
	ScriptStringList	m_strings;
	int					m_numOutputVariables;
	int					m_numOutputStatics;
	int					m_numOutputLabels;
	int					m_numOutputCFunctions;
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_SYMBOLS_H