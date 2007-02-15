//============================================================================
//=
//= InventoryListIF.h - Interface for basic player inventory, such as rotating and
//=							weapons, ammo, keys, etc.
//=
//============================================================================

#ifndef _INVENTORYLISTIF_H
#define _INVENTORYLISTIF_H

#include "Interfaces/Interface.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CInventoryListIF : public CInterface
{
public:
	// interface ID
	static uint GetID(void) { return CInterface::GetValidID( s_inventoryListID ); }
	static CInventoryListIF* GetInterface(CActor* i_actor);
	static CInventoryListIF* GetInterface(ActorHandle i_handle);

	virtual void	Clear(void) {}
	virtual void	AddInventory( const char* key, int value )	{}
	virtual int		GetInventory( const char* key )	{ return false; }
	virtual void	RemoveInventory( const char* key, int value )	{}
	virtual void	CheckpointHit() {}
	virtual void	CheckpointReset() {}

private:
	static uint s_inventoryListID;
};

#endif //_INVENTORYLISTIF_H

//============================================================================
