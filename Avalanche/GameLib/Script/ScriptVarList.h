//===========================================================================
//=
//= ScriptVarList.h
//=
//===========================================================================

#ifndef _SCRIPT_VAR_LIST_H
#define _SCRIPT_VAR_LIST_H

class ScriptVar
{
public:
	enum VarType { TYPE_INT, TYPE_FLOAT, TYPE_STRING };

	inline VarType			Type()		const { return m_type; }
	inline ScriptValue	Value()		const { return m_value; }
	inline bool				IsInt()		const { return m_type == TYPE_INT; }
	inline bool				IsFloat()	const { return m_type == TYPE_FLOAT; }
	inline bool				IsString()	const { return m_type == TYPE_STRING; }
	inline int				GetInt()		const { return IsInt() ? m_value.i : IsFloat() ? (int)m_value.f : 0; }
	inline float			GetFloat()	const { return IsFloat() ? m_value.f : IsInt() ? (float)m_value.i : 0.0f; }
	inline char*			GetString()	const { return IsString() ? m_value.p : NULL; }
	inline void				Set( int value )   { m_type = TYPE_INT;    m_value.i = value; }
	inline void				Set( float value ) { m_type = TYPE_FLOAT;  m_value.f = value; }
	inline void				Set( char* value ) { m_type = TYPE_STRING; m_value.p = value; }
	inline void				Set( VarType type, ScriptValue value ) { m_type = type; m_value = value; }
	inline void				SetKeepType( const ScriptVar& v )
	{
		if      ( m_type == TYPE_INT    ) m_value.i = v.GetInt();
		else if ( m_type == TYPE_FLOAT  ) m_value.f = v.GetFloat();
		else if ( m_type == TYPE_STRING ) m_value.p = v.GetString();
	}

	friend class ScriptExecutable;

private:
	VarType					m_type;
	ScriptValue				m_value;
};

class ScriptVarList
{
public:
	ScriptVarList( int size )
	{
		if ( size > 0 )
		{
			m_vars    = new ScriptVar[ size ];
			m_numVars = size;
		}
		else
		{
			m_vars    = NULL;
			m_numVars = 0;
		}
	}

	~ScriptVarList()
	{
		if ( m_vars )
		{
			delete[] m_vars;
			m_vars = NULL;
		}
		m_numVars = 0;
	}

	inline ScriptVar& Var( const int index ) { ASSERT( index >= 0 && index < m_numVars ); return m_vars[ index ]; }
	inline void  Set( int index, const ScriptVar& var ) { ASSERT( index >= 0 && index < m_numVars ); m_vars[ index ] = var; }

private:
	ScriptVar*		m_vars;
	int				m_numVars;
};

#endif //_SCRIPT_VAR_LIST_H
