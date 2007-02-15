/*
**
**  File:   QueryEntity.h
**  Date:   April 3, 2003
**  By:     Bryant Collard
**  Desc:   A base class for entities used to perform collision queries.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/QueryEntity.h $
**  $Revision: #11 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef QUERY_ENTITY_H
#define QUERY_ENTITY_H

#include "stage/StageEntity.h"
#include "collide/Stack.h"
#include "collide/Node.h"

template<class T> class CCollisionStackArray;

class CQueryEntity : public CStageEntity
{
  public:
	// Destruct
	~CQueryEntity();

	// Get the type of the class.
	static const CEntityType &ClassType(void);

	// Manage queries
	virtual void BeginQuery(void);
	virtual void EndQuery(void);

	// Set the ignore list. Note that no action will be taken on any previously
	// set list that the entity does not own and that the pointer to it will be
	// forgotten.
	void SetIgnoreList(CCollisionStackArray<CStageEntity*>* i_ignoreList,
			bool i_ownList = false);

	// Build a list with one entry using the global stack. Note that no action
	// will be taken on any previously set list that the entity does not own and
	// that the pointer to it will be forgotten.
	void SetIgnoreEntity(CStageEntity* i_entity);

	// Delete any ignore list whether or not the entity owns it.
	void DeleteIgnoreList(void);

	// Override CStageEntity
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);
	virtual const char* GetClassName(void);

  protected:
	// Derived type does not have its own entity type.
	CQueryEntity();

	// Derived type has its own entity type.
	CQueryEntity(const CEntityType &i_type);

	bool IsIgnoredEntity(CStageEntity* i_entity);
	void ManageCollisionComplexity(CCollisionObject* i_object);
	void DoNotManageCollisionComplexity(void);

	// Manage collisions
	bool d_manageCollisionComplexity;
	CSimpleNode d_node;

  private:
	// Construct
	void Construct(void);

	// Class type.
	static const CEntityType s_queryEntityClass;

	// Ignore list.
	CCollisionStackArray<CStageEntity*>* d_ignoreList;
	bool d_ownList;
	CCollisionStackLock d_lock;
};

#define QUERY_ENTITY_HPP
#include "collide/QueryEntity.hpp"

#endif // QUERY_ENTITY_H
