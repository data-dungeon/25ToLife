//===========================================================================
//=
//= ScriptError.h
//=
//===========================================================================

#ifndef _SCRIPT_ERROR_H
#define _SCRIPT_ERROR_H

#ifdef SCRIPT_COMPILER

class ScriptError
{
public:
	ScriptError()
	{
		m_errorCount = 0;
		m_warningCount = 0;
		m_head = NULL;
		m_tail = NULL;
	}

	~ScriptError()
	{
		ErrorNode* n;
		ErrorNode* next;

		for ( n = m_head ; n ; n = next )
		{
			next = n->next;
			n->next = NULL;
			if ( n->error ) { delete n->error; n->error = NULL; }
			delete n;
		}
		m_head = m_tail = NULL;
		m_errorCount = 0;
		m_warningCount = 0;
	}

	void Warning( const char* file, int line, const char* fmt, ... )
	{
		char buf[ 512 ];
		int  len;
		len = sprintf( buf, "%s(%d) : warning SLA0000: ", file, line );

		va_list args;
		va_start( args, fmt );
		vsprintf( buf + len, fmt, args );
		va_end( args );

		Add( buf );

		m_warningCount++;
	}

	void Error( const char* file, int line, const char* fmt, ... )
	{
		char buf[ 512 ];
		int  len;
		len = sprintf( buf, "%s(%d) : error SLA0000: ", file, line );

		va_list args;
		va_start( args, fmt );
		vsprintf( buf + len, fmt, args );
		va_end( args );

		Add( buf );

		m_errorCount++;
	}

	inline int WarningCount() const { return m_warningCount; }
	inline int ErrorCount()   const { return m_errorCount;   }

	void Print()
	{
		ErrorNode* n;
		for ( n = m_head ; n ; n = n->next )
		{
			printf( "%s\n", n->error );
		}
		printf( "%d errors, %d warnings\n", m_errorCount, m_warningCount );
	}

private:

	void Add( const char* buf )
	{
		ErrorNode* n = new ErrorNode;
		n->error = strdup( buf );
		n->next  = NULL;

		if ( m_tail ) { m_tail->next = n; }
		else          { m_head       = n; }
		m_tail = n;
	}

	struct ErrorNode
	{
		const char*	error;
		ErrorNode*	next;
	};

	int m_warningCount;
	int m_errorCount;
	ErrorNode* m_head;
	ErrorNode* m_tail;
};

#endif //SCRIPT_COMPILER
#endif //_SCRIPT_ERROR_H
