//
// File			-> IdManager.cpp
// Author		-> Dwain Skinner
// Created		-> 11/11/02
// Description	->
//						A simple integer id manager, that was mainly created for use with the CPickup class
//						Each time a pickup is created, it needs a unique id to be associated with it( for save game purposes ).
//						This manager will supply that id.  It returns the current id, and increments to the next id.
// 
/******************* includes ****************************/

#include "Game/GamePCH.h"



#define INTERNAL_START_ID 0x100

CIdManager g_CIdManager;

// ===========================================================================
CIdManager::CIdManager( )
{
	m_iStartId		= INTERNAL_START_ID;
	m_iCurId			= INTERNAL_START_ID;

	// --- must begin with id's > INVALID_ID
	ASSERT( m_iStartId > INVALID_ID );

}
// ===========================================================================


// ===========================================================================
CIdManager::~CIdManager( )
{
}
// ===========================================================================


// ===========================================================================
void CIdManager::initialize( int iStart )
{
	// --- must begin with id's > INVALID_ID
	ASSERT( iStart > INVALID_ID );
	ASSERT( iStart >= (int)m_iStartId );	// should never start below start value

	m_iCurId		= iStart;
}
// ===========================================================================


// ===========================================================================
void CIdManager::reset( )
{
	m_iCurId		= m_iStartId;

	// --- must begin with id's > INVALID_ID
	ASSERT( m_iStartId > INVALID_ID );
}
// ===========================================================================


// ===========================================================================
void CIdManager::markPermanent( )
{
	/* record current id as high water mark. subsequent calls to reset() will start with this value. */
	/* this supports pre-loaded permanent data, and prevents duplicate id's being assigned */

	m_iStartId	= m_iCurId;

	// --- must begin with id's > INVALID_ID
	ASSERT( m_iStartId > INVALID_ID );
}
// ===========================================================================


// ===========================================================================
u32 CIdManager::getUniqueId( )
{
	m_iCurId++;

	// --- must begin with id's > INVALID_ID
	ASSERT( ( m_iCurId - 1 ) > INVALID_ID );

	return( m_iCurId - 1 );
}
// ===========================================================================

