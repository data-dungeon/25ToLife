/*
**
**  File:   EntityList.h
**  Date:   March 14, 2003
**  By:     Bryant Collard
**  Desc:   Build a list of entites.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/EntityList.h $
**  $Revision: 2 $
**      $Date: 4/10/03 11:41a $
**    $Author: Bcollard $
**
*/

#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

class CStageEntity;
class CCollisionStack;
template<class T> class CCollisionStackArray;

class CEntityList
{
  public:
	// Construct/destruct.
	CEntityList() {d_list = NULL;}
	~CEntityList();

	// Create the list using the global collision system stack. Fails if a
	// list already exists or if it cannot be created.
	bool Create(void);

	// Create the list using a specified stack (NULL uses dynamic memory).
	// Fails if a list already exists or if it cannot be created.
	bool Create(CCollisionStack* i_stack);

	// Add an entity to the list. All entities should be added after the
	// list is created and before any other routine uses the specified stack.
	// Fails if no list exists, the existing list is closed or a new item
	// cannot be added.
	bool Add(CStageEntity* i_entity);

	// Close the list. The list will be destroyed if it is empty.
	CCollisionStackArray<CStageEntity*>* Close(void);

	// Destroy the list.
	void Destroy(void);

	// Get the list.
	CCollisionStackArray<CStageEntity*>* Get(void) {return(d_list);}

	// See if an entity is a member of the list.
	bool IsMember(CStageEntity* i_entity);

	// Build a list using the global collision system stack. Terminate list
	// with a NULL entry.
	static CCollisionStackArray<CStageEntity*>* Build(
			CStageEntity* i_entity, ...);

	// Build a list using a specified stack (NULL uses dynamic memory).
	// Terminate list with a NULL entry.
	static CCollisionStackArray<CStageEntity*>* Build(
			CCollisionStack* i_stack, ...);

	// See if an entity is a member of a list.
	static bool IsMember(CCollisionStackArray<CStageEntity*>* i_list,
			CStageEntity* i_entity);

  private:
	CCollisionStackArray<CStageEntity*>* d_list;
};

#define ENTITY_LIST_HPP
#include "collide/EntityList.hpp"

#endif // ENTITY_LIST_H
