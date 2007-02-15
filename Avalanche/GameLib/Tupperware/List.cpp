////////////////////////////////////////////////////////////////////////////
//
// TupperwareList
//
// An list entity (stores a collection of single data type).
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "List.h"
#include "Log.h"

////////////////////////////////////////////////////////////////////////////

TupperwareList::TupperwareList() 
{
	m_length = 0;
	m_pList = NULL;
	m_dataType = DATA_UNKNOWN;
}


TupperwareList::~TupperwareList() 
{
	FreeList();

	m_length = 0;
	m_dataType = DATA_UNKNOWN;
}

TupperwareList::TupperwareList(const char *key,const char *name,const float *values, int length) : TupperwareAtom(key,name)
{
	m_dataType = DATA_FLOAT;
	m_length = length;
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = TupMemoryManager::Allocate(sizeof(float)*m_length);
#else
		m_pList = new float[m_length];
#endif
		memcpy(m_pList, values, sizeof(float) * m_length);
	}
}

TupperwareList::TupperwareList(const char *key,const char *name,const int *values, int length) : TupperwareAtom(key,name)
{
	m_dataType = DATA_INT;
	m_length = length;
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = TupMemoryManager::Allocate(sizeof(int)*m_length);
#else
		m_pList = new int[m_length];
#endif
		memcpy(m_pList, values, sizeof(int) * m_length);
	}
}

TupperwareList::TupperwareList(const char *key,const char *name,const char **values, int length) : TupperwareAtom(key,name)
{
	m_dataType = DATA_STRING;
	m_length = length;
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = TupMemoryManager::Allocate(sizeof(char *)*m_length);
#else
		m_pList = new char*[m_length];
#endif
		for (int i = 0; i < m_length; i++) 
		{
#ifdef USE_TUP_MEMORY_MANAGER
			((char **)m_pList)[i] = (char *)TupMemoryManager::Allocate(strlen(values[i]) + 1);
#else
			((char **)m_pList)[i] = new char[strlen(values[i]) + 1];
#endif
			strcpy(((char **)m_pList)[i], values[i]);
		}
	}
}

void TupperwareList::FreeList() 
{
	if ( m_dataType == DATA_STRING ) 
	{
		for ( int i = 0; i < m_length; i++ ) 
		{
#ifdef USE_TUP_MEMORY_MANAGER
			TupMemoryManager::FreeBuffer((( char** )m_pList )[ i ]);
#else
			delete [] ( ( char** )m_pList )[ i ];
#endif
		}
	}
#ifdef USE_TUP_MEMORY_MANAGER
	TupMemoryManager::FreeBuffer(m_pList);
#else
	delete [] (char*)m_pList;
#endif
	m_pList = NULL;
}


TupperwareAtom::DataType TupperwareList::GetDataType() const 
{
	return m_dataType;
}


int TupperwareList::GetLength() const 
{
	return m_length;
}


const float* TupperwareList::GetAsFloat() const 
{
	if ( m_dataType != DATA_FLOAT ) 
	{
		TupperwareLog::Write( "GetAsFloat(): Data type does not match requested type.\n" );
		return NULL;
	}

	return ( float* )m_pList;
}

float* TupperwareList::GetAsFloat() 
{
	if ( m_dataType != DATA_FLOAT ) 
	{
		TupperwareLog::Write( "GetAsFloat(): Data type does not match requested type.\n" );
		return NULL;
	}
	return ( float* )m_pList;
}

void TupperwareList::GetAsFloat(TupArray<float> &valueArray) const
{
	valueArray.RemoveAll();
	if ( m_dataType != DATA_FLOAT ) 
	{
		TupperwareLog::Write( "GetAsFloat(TupArray<float>): Data type does not match requested type.\n" );
		return;
	}
	for (int index=0;index<m_length;index++)
	{
		valueArray.Add(((float *)m_pList)[index]);
	}
}

const int* TupperwareList::GetAsInt() const 
{
	if ( m_dataType != DATA_INT ) 
	{
		TupperwareLog::Write( "GetAsInt(): Data type does not match requested type.\n" );
		return NULL;
	}
	return ( int* )m_pList;
}

int* TupperwareList::GetAsInt()
{
	if ( m_dataType != DATA_INT ) 
	{
		TupperwareLog::Write( "GetAsInt(): Data type does not match requested type.\n" );
		return NULL;
	}
	return ( int* )m_pList;
}

void TupperwareList::GetAsInt(TupArray<int> &valueArray) const
{
	valueArray.RemoveAll();
	if ( m_dataType != DATA_INT ) 
	{
		TupperwareLog::Write( "GetAsInt(TupArray<int>): Data type does not match requested type.\n" );
		return;
	}
	for (int index=0;index<m_length;index++)
	{
		valueArray.Add(((int *)m_pList)[index]);
	}
}

const char** TupperwareList::GetAsString() const 
{
	if ( m_dataType != DATA_STRING ) 
	{
		TupperwareLog::Write( "GetAsString(): Data type does not match requested type.\n" );
		return NULL;
	}
	return ( const char** )m_pList;
}

char** TupperwareList::GetAsString()
{
	if ( m_dataType != DATA_STRING ) 
	{
		TupperwareLog::Write( "GetAsString(): Data type does not match requested type.\n" );
		return NULL;
	}
	return ( char** )m_pList;
}

void TupperwareList::GetAsString(TupArray<TupString> &valueArray) const
{
	valueArray.RemoveAll();
	if ( m_dataType != DATA_STRING ) 
	{
		TupperwareLog::Write( "GetAsString(TupArray<TupString>): Data type does not match requested type.\n" );
		return;
	}
	for (int index=0;index<m_length;index++)
	{
		valueArray.Add(TupString(((char **)m_pList)[index]));
	}
}

void TupperwareList::SetAsFloat(const float* values, int length) 
{
	FreeList();

	m_dataType = DATA_FLOAT;
	m_length = length;
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = (float *)TupMemoryManager::Allocate(sizeof(float)*m_length);
#else
		m_pList = new float[m_length];
#endif
		memcpy(m_pList, values, sizeof(float) * m_length);
	}
}

void TupperwareList::SetAsFloat(const TupArray<float> &valueArray) 
{
	FreeList();

	m_dataType = DATA_FLOAT;
	m_length = valueArray.GetSize();
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = (float *)TupMemoryManager::Allocate(sizeof(float)*m_length);
#else
		m_pList = new float[m_length];
#endif
		memcpy(m_pList, &valueArray[0], sizeof(float) * m_length);
	}
}

void TupperwareList::SetAsInt(const int* values, int length) 
{
	FreeList();

	m_dataType = DATA_INT;
	m_length = length;
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = (int *)TupMemoryManager::Allocate(sizeof(int)*m_length);
#else
		m_pList = new int[m_length];
#endif
		memcpy(m_pList, values, sizeof(int) * m_length);
	}
}

void TupperwareList::SetAsInt(const TupArray<int> &valueArray) 
{
	FreeList();

	m_dataType = DATA_INT;
	m_length = valueArray.GetSize();
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = (int *)TupMemoryManager::Allocate(sizeof(int)*m_length);
#else
		m_pList = new int[m_length];
#endif
		memcpy(m_pList, &valueArray[0], sizeof(int) * m_length);
	}
}

void TupperwareList::SetAsString(const char** values, int length) 
{
	FreeList();

	m_dataType = DATA_STRING;
	m_length = length;
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = (char **)TupMemoryManager::Allocate(sizeof(char *)*m_length);
#else
		m_pList = new char*[m_length];
#endif
		for (int i = 0; i < m_length; i++) 
		{
#ifdef USE_TUP_MEMORY_MANAGER
			((char **)m_pList)[i] = (char *)TupMemoryManager::Allocate(strlen(values[i]) + 1);
#else
			((char **)m_pList)[i] = new char[strlen(values[i]) + 1];
#endif
			strcpy(((char **)m_pList)[i], values[i]);
		}
	}
}

void TupperwareList::SetAsString( const TupArray<TupString> &values)
{
	FreeList();

	m_dataType = DATA_STRING;
	m_length = values.GetSize();
	if (m_length != 0) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pList = (char **)TupMemoryManager::Allocate(sizeof(char *)*m_length);
#else
		m_pList = new char*[m_length];
#endif
		for (int i = 0; i < m_length; i++) 
		{
#ifdef USE_TUP_MEMORY_MANAGER
			((char **)m_pList)[i] = (char *)TupMemoryManager::Allocate(values[i].GetLength() + 1);
#else
			((char **)m_pList)[i] = new char[values[i].GetLength() + 1];
#endif
			strcpy(((char **)m_pList)[i], (const char *)values[i]);
		}
	}
}

////////////////////////////////////////////////////////////////////////////
