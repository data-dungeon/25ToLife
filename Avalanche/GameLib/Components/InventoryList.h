//============================================================================
//=
//= InventoryList.h - Component that keeps track of a player's Inventory
//=
//============================================================================

#ifndef _INVENTORY_LIST_H
#define _INVENTORY_LIST_H

#include "Components/Component.h"
#include "container/Darray.h"
#include "Interfaces/InventoryListIF.h"


//============================================================================

class CInventoryList : public CActorComponent, public CInventoryListIF
{
public:
	// component base stuff
	CInventoryList( CCompActor& i_actor );
	virtual ~CInventoryList();
	const char *ComponentName()  { return "InventoryList"; }
	CInterface *GetInterface( uint i_id );

	void	Purge();

	virtual void	Clear();
	virtual void	AddInventory( const char* key, int value );
	virtual int		GetInventory( const char* key );
	virtual void	RemoveInventory( const char* key, int value );
	virtual void	CheckpointHit();
	virtual void	CheckpointReset();

	struct Param
	{
		Param*	next;
		char*		key;
		int		value;
		int		lastCheckpointValue;
	};

private:
	Param*	m_head;
};

//============================================================================

#endif //_INVENTORY_LIST_H
