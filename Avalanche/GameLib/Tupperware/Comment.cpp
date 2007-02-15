////////////////////////////////////////////////////////////////////////////
//
// TupperwareComment
//
// An atom that consists of a comment string
// used for adding comments that will be saved out in the ascii version of the file
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Comment.h"

////////////////////////////////////////////////////////////////////////////

TupperwareComment::TupperwareComment() 
{
	m_pValue = NULL;
}

TupperwareComment::TupperwareComment(const char *key,const char *name,const char *value) : TupperwareAtom(key,name)
{
#ifdef USE_TUP_MEMORY_MANAGER
	m_pValue = (char *)TupMemoryManager::Allocate(strlen( value ) + 1);
#else
	m_pValue = new char[ strlen( value ) + 1 ];
#endif
	strcpy( m_pValue, value );
}


TupperwareComment::~TupperwareComment() 
{
	FreeData();
}

void TupperwareComment::FreeData() 
{
#ifdef USE_TUP_MEMORY_MANAGER
		TupMemoryManager::FreeBuffer(m_pValue);
#else
		delete [] m_pValue;
#endif
	m_pValue = NULL;
}

const char* TupperwareComment::GetValue() const 
{
	return m_pValue;
}

char* TupperwareComment::GetValue()
{
	return m_pValue;
}

void TupperwareComment::SetValue( const char* value ) 
{
	FreeData();

#ifdef USE_TUP_MEMORY_MANAGER
	m_pValue = (char *)TupMemoryManager::Allocate(strlen( value ) + 1);
#else
	m_pValue = new char[ strlen( value ) + 1 ];
#endif

	strcpy( m_pValue, value );
}
