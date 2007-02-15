//============================================================================
//=
//= ParameterSet.cpp
//=
//============================================================================

#include "Game/GamePCH.h"

//============================================================================

ParameterSet::ParameterSet()
{
	m_head = NULL;
	m_compactedBuffer = NULL;
	m_shareOwner = NULL;
	m_shareCount = 0;
}

//============================================================================

ParameterSet::ParameterSet( const char* keyValueString )
{
	m_head = NULL;
	m_compactedBuffer = NULL;
	m_shareOwner = NULL;
	m_shareCount = 0;
	SetFromString( keyValueString );
}

//============================================================================

ParameterSet::~ParameterSet()
{
	ASSERT( m_shareCount == 0 );
	Clear();
}

//============================================================================

void ParameterSet::Clear()
{
	if ( m_shareOwner )
	{
		m_shareOwner->m_shareCount--;
		m_shareOwner = NULL;
		m_compactedBuffer = NULL;
	}
	else if ( m_compactedBuffer == NULL )
	{
		Param* n;
		Param* next;
		for ( n = m_head ; n ; n = next )
		{
			next = n->next;
			delete n;
		}
	}
	else
	{
		delete [] m_compactedBuffer;
		m_compactedBuffer = NULL;
	}
	m_head = NULL;
}

//============================================================================
// convert a string in the format "key=value;key=value" into appropriate
// key/value pairs.  key must not have whitespace, value can have whitespace

void ParameterSet::SetFromString( const char* keyValueString )
{
	ASSERT( m_compactedBuffer == NULL );
	if ( keyValueString && keyValueString[ 0 ] )
	{
		char key[ 512 ];
		char value[ 512 ];

		// tokenize string into key/value pairs and add them
		const char* p = keyValueString;
		char* vp;
		while ( *p )
		{
			// skip leading whitespace
			while ( isspace( *p ) )
				p++;

			// make a copy of the key
			char* kp = key;
			while ( *p && !isspace( *p ) && ( *p != '=' ) && ( *p != ';' ) )
				*kp++ = *p++;
			*kp = 0;

			// skip whitespace
			while ( isspace( *p ) )
				p++;

			// we're suppose to have a '=' now.  if we don't bail and try the next option
			if ( *p != '=' )
				goto nextToken;
			p++;

			// skip whitespace
			while ( isspace( *p ) )
				p++;

			// make a copy of the value
			vp = value;
			while ( *p && ( *p != ';' ) )
				*vp++ = *p++;
			*vp = 0;

			// trim excess white space off end of value
			while ( vp > value && isspace( vp[ -1 ] ) )
				*(--vp) = 0;

			if ( vp == value )
				goto nextToken;

			// add the key/value pair
			Set( key, value );

nextToken:
			// scan forward to semicolon
			while ( *p && ( *p != ';' ) )
				p++;

			if ( *p == ';' )
				p++;
		}
	}
}

//============================================================================

void ParameterSet::SetFromParameterSet( ParameterSet& parms )
{
	ASSERT( m_compactedBuffer == NULL );
	for ( Param* n = parms.GetFirstParam() ; n ; n = parms.GetNextParam( n ) )
	{
		Set( n->key, n->value );
	}
}

//============================================================================
// Copy parameters from one set to another with a matching prefix, and strip
// the prefix in the copy.

void ParameterSet::SetFromParameterSetWithPrefix( ParameterSet& parms, const char* prefix )
{
	ASSERT( m_compactedBuffer == NULL );
	ASSERT_PTR( prefix );
	ASSERT( prefix[0] );

	int prefixLen = strlen( prefix );

	for ( Param* n = parms.GetFirstParam() ; n ; n = parms.GetNextParam( n ) )
	{
		if ( memcmp( n->key, prefix, prefixLen ) == 0 )
		{
			Set( n->key + prefixLen, n->value );
		}
	}
}

//============================================================================

void ParameterSet::Set( const char* key, const char* value )
{
	ASSERT( m_compactedBuffer == NULL );
	ASSERT_PTR( key );
	ASSERT( key[0] );
	ASSERT_PTR( value );

	// delete any existing nodes with this key
	Remove( key );

	// calculate length, add space at the end of the parameter node
	int keyLen = strlen( key ) + 1;
	int valLen = strlen( value ) + 1;
	
	// create node with space at end for key and value
	Param* n = (Param*) new char[ sizeof(Param) + keyLen + valLen ];
	n->key   = (char*)n + sizeof(Param);
	n->value = n->key + keyLen;
	strcpy( n->key,   key   );
	strcpy( n->value, value );

	// link it in
	n->next     = m_head;
	m_head      = n;
}

//============================================================================

void ParameterSet::Remove( const char* key )
{
	ASSERT( m_compactedBuffer == NULL );
	ASSERT_PTR( key );
	ASSERT( key[0] );

	// remove all keys that match from the list
	Param** parent = &m_head;
	Param*  n;
	for ( n = *parent ; n ; n = *parent )
	{
		if ( stricmp( key, n->key ) == 0 )
		{
			*parent = n->next;
			delete n;
		}
		else
		{
			parent = &n->next;
		}
	}
}

//============================================================================

void ParameterSet::RemoveKeysWithPrefix( const char* prefix )
{
	ASSERT( m_compactedBuffer == NULL );
	ASSERT_PTR( prefix );

	int prefixLen = strlen( prefix );
	ASSERT( prefixLen > 0 );

	// remove all keys that match from the list
	Param** parent = &m_head;
	Param*  n;
	for ( n = *parent ; n ; n = *parent )
	{
		if ( strnicmp( n->key, prefix, prefixLen ) == 0 )
		{
			*parent = n->next;
			delete n;
		}
		else
		{
			parent = &n->next;
		}
	}
}

//============================================================================

const char* ParameterSet::GetInitializerString()
{
	int len = 0;
	Param* n;
	for ( n = m_head ; n ; n = n->next )
		len += strlen( n->key ) + strlen( n->value ) + 2;	// +2 for "=" and ";"
	len++;

	char* string = new char [ len ];
	char* p = string;
	for ( n = m_head ; n ; n = n->next )
		p += sprintf( p, "%s=%s;", n->key, n->value );

	*p = 0;

	return string;
}

//============================================================================

void ParameterSet::TransferTo( ParameterSet& receiver )
{
	ASSERT( m_compactedBuffer == NULL );
	receiver.Clear();
	receiver.m_head = m_head;
	m_head = NULL;
}

//============================================================================

char* ParameterSet::GetString( const char* key ) const
{
	ASSERT_PTR( key );
	ASSERT( key[0] );

	Param* n;
	for ( n = m_head ; n ; n = n->next )
		if ( stricmp( key, n->key ) == 0 )
			return n->value;
	return NULL;
}

//============================================================================

int ParameterSet::GetInt( const char* key ) const
{
	const char* valueString = GetString( key );
	int value = MAXINT;
	if ( valueString != NULL )
	{
		if ( strcmpi( valueString, "true" ) == 0 )
			value = 1;
		else if ( strcmpi( valueString, "false" ) == 0 )
			value = 0;
		else
			sscanf( valueString, "%d", &value );
	}
	return value;
}

//============================================================================

bool ParameterSet::GetHasParameter( const char* key ) const
{
	ASSERT_PTR( key );
	ASSERT( key[0] );

	Param* n;
	for ( n = m_head ; n ; n = n->next )
		if ( stricmp( key, n->key ) == 0 )
			return true;
	return false;
}

//============================================================================

float ParameterSet::GetFloat( const char* key ) const
{
	const char* valueString = GetString( key );
	float value = MAXFLOAT;
	if ( valueString != NULL )
	{
		if ( strcmpi( valueString, "true" ) == 0 )
			value = 1.0f;
		else if ( strcmpi( valueString, "false" ) == 0 )
			value = 0.0f;
		else
			sscanf( valueString, "%f", &value );
	}
	return value;
}

//============================================================================

ParameterSet::Param* ParameterSet::GetFirstParam()
{
	return m_head;
}

//============================================================================

ParameterSet::Param* ParameterSet::GetNextParam( ParameterSet::Param* param )
{
	ASSERT_PTR( param );
	return param->next;
}

//============================================================================

ParameterSet* ParameterSet::Clone()
{
	ParameterSet* clone = new ParameterSet;

	for ( Param* n = m_head ; n ; n = n->next )
	{
		clone->Set( n->key, n->value );
	}

	return clone;
}

//============================================================================

char* ParameterSet::CreateCompactedBuffer()
{
	char* buffer = NULL;

	// calculate size
	int size = 0;
	int nodeCount = 0;
	for ( Param* n = m_head ; n ; n = n->next )
	{
		nodeCount++;
		size += strlen( n->key ) + 1;
		size += strlen( n->value ) + 1;
	}
	size += nodeCount * sizeof(Param);

	if ( nodeCount > 0 )
	{
		// create the compacted buffer
		buffer = new char[ size ];

		// fill it in
		Param* node   = (Param*) buffer;
		char* strings = (char*) ( (int)buffer + nodeCount * sizeof(Param) );
		for ( Param* n = m_head ; n ; n = n->next )
		{
			int keyLen = strlen( n->key   ) + 1;
			int valLen = strlen( n->value ) + 1;

			node->next     = n->next ? ( node + 1 ) : NULL;
			node->key      = strings;
			node->value    = strings + keyLen;
			memcpy( node->key,   n->key,   keyLen );
			memcpy( node->value, n->value, valLen );

			node++;
			strings += keyLen + valLen;
		}
	}

	return buffer;
}

//============================================================================

void ParameterSet::Compact()
{
	ASSERT( m_compactedBuffer == NULL );

	// create the compacted buffer
	char* buffer = CreateCompactedBuffer();

	// free the original data
	Clear();

	// and link us to the compacted buffer
	m_compactedBuffer = buffer;
	m_head            = (Param*) buffer;
}

//============================================================================

void ParameterSet::ShareFrom( ParameterSet* parms )
{
	ASSERT( m_compactedBuffer == NULL );
	ASSERT( m_head == NULL );

	ASSERT_PTR( parms );
	if ( parms->m_compactedBuffer && parms->m_head )
	{
		m_shareOwner      = parms;
		m_compactedBuffer = parms->m_compactedBuffer;
		m_head            = parms->m_head;

		parms->m_shareCount++;
	}
}

//============================================================================
