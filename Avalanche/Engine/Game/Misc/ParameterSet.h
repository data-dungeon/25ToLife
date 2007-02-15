//============================================================================
//=
//= Parametizer
//=
//============================================================================

#ifndef _PARAMETER_SET_H
#define _PARAMETER_SET_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

class ParameterSet
{
public:
	ParameterSet();
	ParameterSet( const char* keyValueString );
	~ParameterSet();

	void			Clear();
	void			SetFromString( const char* keyValueString );
	void			SetFromParameterSet( ParameterSet& parms );
	void			SetFromParameterSetWithPrefix( ParameterSet& parms, const char* prefix );
	void			Set( const char* key, const char* value );
	void			Remove( const char* key );
	void			RemoveKeysWithPrefix( const char* keyPrefix );
	const char* GetInitializerString();
	char*			GetString( const char* key ) const;
	int			GetInt( const char* key ) const;
	float			GetFloat( const char* key ) const;
	void			TransferTo( ParameterSet& receiver );
	void			Compact();
	void			ShareFrom( ParameterSet* parms );
	ParameterSet* Clone();
	bool GetHasParameter( const char* key ) const;

	struct Param
	{
		Param*	next;
		char*		key;
		char*		value;
	};

	Param*		GetFirstParam();
	Param*		GetNextParam( Param* param );

private:
	char*		CreateCompactedBuffer();

	Param*	m_head;
	char*		m_compactedBuffer;
	ParameterSet*	m_shareOwner;
	int		m_shareCount;
};

//============================================================================

#endif //_PARAMETER_SET_H