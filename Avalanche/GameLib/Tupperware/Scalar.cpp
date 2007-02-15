////////////////////////////////////////////////////////////////////////////
//
// TupperwareScalar
//
// A scalar entity (stores a single value of a specific data type).
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Scalar.h"
#include "Codec.h"
#include "Log.h"

////////////////////////////////////////////////////////////////////////////

TupperwareScalar::TupperwareScalar() {
	m_pString = NULL;
	m_dataType = DATA_UNKNOWN;
}

TupperwareScalar::TupperwareScalar(const char *key,const char *name,const float value) : TupperwareAtom(key,name)
{
	m_dataType = DATA_FLOAT;
	m_float = value;
}

TupperwareScalar::TupperwareScalar(const char *key,const char *name,const int value) : TupperwareAtom(key,name)
{
	m_dataType = DATA_INT;
	m_int = value;
}

TupperwareScalar::TupperwareScalar(const char *key,const char *name,const char *value) : TupperwareAtom(key,name)
{
	m_dataType = DATA_STRING;
#ifdef USE_TUP_MEMORY_MANAGER
	m_pString = (char *)TupMemoryManager::Allocate(strlen( value ) + 1);
#else
	m_pString = new char[ strlen( value ) + 1 ];
#endif
	strcpy( m_pString, value );
}

TupperwareScalar::TupperwareScalar(const char *key,const char *name,const void *value,int length) : TupperwareAtom(key,name)
{
	m_dataType = DATA_BINARY;
	m_binaryLength = length;
#ifdef USE_TUP_MEMORY_MANAGER
	m_pBinary = TupMemoryManager::Allocate(m_binaryLength);
#else
	m_pBinary = new char[ m_binaryLength ];
#endif
	memcpy( m_pBinary, value, m_binaryLength );
}


TupperwareScalar::~TupperwareScalar() 
{
	FreeData();
	m_dataType = DATA_UNKNOWN;
}


void TupperwareScalar::FreeData() 
{
	if ( m_dataType == DATA_STRING ) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		TupMemoryManager::FreeBuffer(m_pString);
#else
		delete [] m_pString;
#endif
	}
	else if ( m_dataType == DATA_BINARY ) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		TupMemoryManager::FreeBuffer(m_pBinary);
#else
		delete [] (char*)m_pBinary;
#endif
	}

	m_pString = NULL;
}


TupperwareAtom::DataType TupperwareScalar::GetDataType() const 
{
	return m_dataType;
}

int TupperwareScalar::GetBinaryLength(void) const 
{
	return m_binaryLength;
}


float TupperwareScalar::GetAsFloat() const 
{
	if ( m_dataType != DATA_FLOAT ) 
	{
		TupperwareLog::Write( "GetAsFloat(): Atom type does not match requested type.\n" );
		return 0.0f;
	}

	return m_float;
}


int TupperwareScalar::GetAsInt() const 
{
	if ( m_dataType != DATA_INT ) 
	{
		TupperwareLog::Write( "GetAsInt(): Atom type does not match requested type.\n" );
		return 0;
	}

	return m_int;
}


const char* TupperwareScalar::GetAsString() const 
{
	if ( m_dataType != DATA_STRING ) 
	{
		TupperwareLog::Write( "GetAsString(): Atom type does not match requested type.\n" );
		return NULL;
	}

	return m_pString;
}

char* TupperwareScalar::GetAsString()
{
	if ( m_dataType != DATA_STRING ) 
	{
		TupperwareLog::Write( "GetAsString(): Atom type does not match requested type.\n" );
		return NULL;
	}
	return m_pString;
}


const void* TupperwareScalar::GetAsBinary( int* length ) const 
{
	if ( m_dataType != DATA_BINARY ) {
		TupperwareLog::Write( "GetAsBinary(): Atom type does not match requested type.\n" );

		if (length)
			*length = 0;
		return NULL;
	}

	if ( length != NULL )
	{
		*length = m_binaryLength;
	}

	return m_pBinary;
}

void* TupperwareScalar::GetAsBinary( int* length )
{
	if ( m_dataType != DATA_BINARY ) 
	{
		TupperwareLog::Write( "GetAsBinary(): Atom type does not match requested type.\n" );

		if (length)
			*length = 0;
		return NULL;
	}

	if ( length != NULL ) 
	{
		*length = m_binaryLength;
	}
	return m_pBinary;
}


void TupperwareScalar::SetAsFloat( float value ) 
{
	FreeData();

	m_dataType = DATA_FLOAT;
	m_float = value;
}


void TupperwareScalar::SetAsInt( int value ) 
{
	FreeData();

	m_dataType = DATA_INT;
	m_int = value;
}


void TupperwareScalar::SetAsString( const char* value ) 
{
	FreeData();

	m_dataType = DATA_STRING;

#ifdef USE_TUP_MEMORY_MANAGER
	m_pString = (char *)TupMemoryManager::Allocate(strlen( value ) + 1);
#else
	m_pString = new char[ strlen( value ) + 1 ];
#endif

	strcpy( m_pString, value );
}


void TupperwareScalar::SetAsBinary( const void* value, int length) 
{
	FreeData();

	m_dataType = DATA_BINARY;
	m_binaryLength = length;

#ifdef USE_TUP_MEMORY_MANAGER
	m_pBinary = TupMemoryManager::Allocate(m_binaryLength);
#else
	m_pBinary = new char[m_binaryLength];
#endif

	memcpy( m_pBinary, value, m_binaryLength );
}

////////////////////////////////////////////////////////////////////////////
