//============================================================================
//=
//= InventoryList.cpp - Component that keeps track of a player's Inventory
//=
//============================================================================

#include "Components/ComponentsPCH.h"
#include "Components/InventoryList.h"
#include "Components/ComponentCreator.h"

//============================================================================

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CInventoryList, InventoryList, COMPONENT_NEUTRAL_PRIORITY);

//============================================================================

CInventoryList::CInventoryList( CCompActor& i_actor ) : CActorComponent( i_actor )
{
	m_head = NULL;
}

//============================================================================

CInventoryList::~CInventoryList()
{
	Clear();
}

//============================================================================

CInterface *CInventoryList::GetInterface( uint i_id )
{
	return ( i_id == CInventoryListIF::GetID() ) ? this : NULL;
}

//============================================================================

void CInventoryList::Clear()
{
	Param* n;
	Param* next;
	for ( n = m_head ; n ; n = next )
	{
		next = n->next;
		memFree( n );
	}
	m_head = NULL;
}

//============================================================================

void CInventoryList::Purge()
{
	Param** parent = &m_head;
	Param*  n;
	for ( n = *parent ; n ; n = *parent )
	{
		if ( n->value <= 0 && n->lastCheckpointValue <= 0 )
		{
			*parent = n->next;
			memFree( n );
		}
		else
		{
			parent = &n->next;
		}
	}
}

//============================================================================

void CInventoryList::AddInventory( const char* key, int value )
{
	ASSERT_PTR( key );
	ASSERT( key[0] );

	// find in the list
	Param* n;
	for ( n = m_head ; n ; n = n->next )
		if ( stricmp( key, n->key ) == 0 )
			break;

	if ( !n )												// key doesn't exist - create one
	{
		// create node, put space for key name at end
		n = (Param*) memAlloc( sizeof(Param) + strlen(key) + 1 );
		n->key                 = (char*)n + sizeof(Param);
		n->value               = value;
		n->lastCheckpointValue = 0;
		strcpy( n->key, key );

		// link us in
		n->next = m_head;
		m_head  = n;
	}
	else
	{
		n->value += value;							// found key - add new value to existing value
	}
}

//============================================================================

int CInventoryList::GetInventory( const char* key )
{
	ASSERT_PTR( key );
	ASSERT( key[0] );

	// find in the list
	Param* n;
	for ( n = m_head ; n ; n = n->next )
		if ( stricmp( key, n->key ) == 0 )
			break;

	return n ? n->value : 0;
}

//============================================================================
// remove 'value' from inventory
// - if value <= 0 - remove all 'key' inventory
//============================================================================

void CInventoryList::RemoveInventory( const char* key, int value )
{
	ASSERT_PTR( key );
	ASSERT( key[0] );

	for ( Param* n = m_head ; n ; n = n->next )
	{
		if ( stricmp( key, n->key ) == 0 )
		{
			if ( value <= 0 )
				n->value = 0;
			else
				n->value -= value;
		}
	}

	Purge();
}

//============================================================================

void CInventoryList::CheckpointHit()
{
	for ( Param* n = m_head ; n ; n = n->next )
		n->lastCheckpointValue = n->value;

	Purge();
}

//============================================================================

void CInventoryList::CheckpointReset()
{
	for ( Param* n = m_head ; n ; n = n->next )
		n->value = n->lastCheckpointValue;

	Purge();
}

//=================================eof========================================
