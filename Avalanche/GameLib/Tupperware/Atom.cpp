////////////////////////////////////////////////////////////////////////////
//
// TupperwareAtom
//
// Base class for Tupperware entities.
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "TupperwarePch.h"
#include "Atom.h"
#include "Aggregate.h"
#include "memorymanager.h"

////////////////////////////////////////////////////////////////////////////

TupperwareAtom::TupperwareAtom() {
	m_pKey = NULL;
	m_pName = NULL;
	m_pNext = m_pPrev = NULL;
	m_pParent = NULL;
#ifdef TUP_EXT
	m_pData = NULL;
#endif // TUP_EXT
}

TupperwareAtom::TupperwareAtom(const char* key , const char* name)
{
	m_pKey = NULL;
	m_pName = NULL;
	m_pNext = m_pPrev = NULL;
	m_pParent = NULL;
#ifdef TUP_EXT
	m_pData = NULL;
#endif // TUP_EXT

	if ( key ) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pKey = (char *)TupMemoryManager::Allocate(strlen( key ) + 1);
#else
		m_pKey = new char[strlen( key ) + 1];
#endif
		strcpy( m_pKey, key );
	}
	if ( name ) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pName = (char *)TupMemoryManager::Allocate(strlen( name ) + 1);
#else
		m_pName = new char[strlen( name ) + 1];
#endif
		strcpy( m_pName, name );
	}

}

TupperwareAtom::~TupperwareAtom() 
{
#ifdef USE_TUP_MEMORY_MANAGER
	TupMemoryManager::FreeBuffer(m_pKey);
#else
	delete [] m_pKey;
#endif

#ifdef USE_TUP_MEMORY_MANAGER
	TupMemoryManager::FreeBuffer(m_pName);
#else
	delete [] m_pName;
#endif

	m_pKey = NULL;
	m_pName = NULL;
	m_pNext = m_pPrev = NULL;
	m_pParent = NULL;
}

void TupperwareAtom::Detatch(void)
{
	// if parent is looking at me then set to look at the next atom
	if (m_pParent && m_pParent->m_pFirstChild == this)
	{
		m_pParent->m_pFirstChild = m_pNext;
	}
	if (m_pParent && m_pParent->m_pLastChild == this)
	{
		m_pParent->m_pLastChild = m_pPrev;
	}
	// remove from sibling list
	if (m_pPrev)
		m_pPrev->m_pNext = m_pNext;
	if (m_pNext)
		m_pNext->m_pPrev = m_pPrev;
	m_pParent = NULL;
	m_pNext = m_pPrev = NULL;
}

TupperwareAggregate *TupperwareAtom::GetParent(void) const
{
	return m_pParent;
}

TupperwareAtom *TupperwareAtom::GetPrevAtom(void) const
{
	return m_pPrev;
}

TupperwareAtom *TupperwareAtom::GetNextAtom(void) const
{
	return m_pNext;
}

const char* TupperwareAtom::GetKey() const 
{
	return m_pKey;
}

const char* TupperwareAtom::GetName() const 
{
	return m_pName;
}


void TupperwareAtom::SetKey( const char* key ) 
{
#ifdef USE_TUP_MEMORY_MANAGER
	TupMemoryManager::FreeBuffer(m_pKey);
#else
	delete [] m_pKey;
#endif
	m_pKey = NULL;

	if ( key ) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pKey = (char *)TupMemoryManager::Allocate(strlen( key ) + 1);
#else
		m_pKey = new char[strlen( key ) + 1];
#endif
		strcpy( m_pKey, key );
	}
}


void TupperwareAtom::SetName( const char* name ) 
{
#ifdef USE_TUP_MEMORY_MANAGER
	TupMemoryManager::FreeBuffer(m_pName);
#else
	delete [] m_pName;
#endif
	m_pName = NULL;

	if ( name ) 
	{
#ifdef USE_TUP_MEMORY_MANAGER
		m_pName = (char *)TupMemoryManager::Allocate(strlen( name ) + 1);
#else
		m_pName = new char[strlen( name ) + 1];
#endif
		strcpy( m_pName, name );
	}
}

#ifdef USE_TUP_MEMORY_MANAGER
void *TupperwareAtom::operator new(size_t nSize)
{
	return TupMemoryManager::Allocate(nSize); // call this one if running in the game engine
}

void TupperwareAtom::operator delete(void *pBuffer)
{
	TupMemoryManager::FreeBuffer(pBuffer);
}
#endif

#ifdef TUP_EXT
void *TupperwareAtom::GetData() const
{
	return m_pData;
}

void TupperwareAtom::SetData(void *pData)
{
	m_pData = pData;
}
#endif // TUP_EXT

////////////////////////////////////////////////////////////////////////////
