//
// File			-> IdManager.h
// Author		-> Dwain Skinner
// Created		-> 11/11/02
// Description	->
//						A simple integer id manager, that was mainly created for use with the CPickup class
//						Each time a pickup is created, it needs a unique id to be associated with it( for save game purposes ).
//						This manager will supply that id.  It returns the current id, and increments to the next id.
// 

#ifndef __IDMANAGER__H
#define __IDMANAGER__H

#define DYNAMIC_ID_BIT		0x80000000
#define INVALID_ID			0x00000000

class CIdManager
{

public:
		CIdManager( );
		~CIdManager( );

	// --- initialize the id to some starting value
	void		initialize						( int iStart );

	// --- reset the id to an internal start value
	void		reset								( void );

	// --- record high water mark after loading permanent files-- reset then starts at this value
	void		markPermanent					( void );

	// --- get the current id and increment
	u32		getUniqueId						( void );
		
	// --- dynamic or static entity
	u32		setAsDynamicId					( u32 id ) { return( id | DYNAMIC_ID_BIT ); }

	// --- is this a dynamic id?
	bool		isDynamicId						( u32 id ) { return( ( id & DYNAMIC_ID_BIT ) != 0 ); }

	// --- get the current id without incrementing
	u32		getCurrentId					( void )	{ return( m_iCurId ); }

private:
	u32		m_iStartId;
	u32		m_iCurId;

};

extern CIdManager g_CIdManager;
#endif