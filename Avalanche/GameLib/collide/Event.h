/*
**
**  File:   Event.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A pairing of colliding stage entities.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Event.h $
**  $Revision: #16 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef COLLISION_EVENT_H
#define COLLISION_EVENT_H

#include "collide/Stack.h"
#include "stage/StageEntity.h"

class CCollisionPair;
class CCollisionList;
class CCollisionNode;
class CCollisionObject;
template<class T> class CCollisionStackArray;

class CCollisionEvent : public CCollisionStackItem
{
  public:
	CCollisionEvent();
	inline ~CCollisionEvent();

	// Allocate a new event object.
	static inline CCollisionEvent* New(CCollisionStack* i_stack);

	static CCollisionEvent* TestEntityPair(CStageEntity* i_queryEntity,
			CStageEntity* i_targetEntity, CCollisionStack* i_stack,
			CStageEntity::ESelect i_select = CStageEntity::USE_SIMPLE);

	inline void SetEntities(CStageEntity* i_queryEntity,
			CStageEntity* i_targetEntity);

	inline CCollisionEvent* AddToList(CCollisionList* i_list,
			CCollisionEvent* i_head, bool i_listOwnsevent = false);
	inline bool RemoveFromList(CCollisionList* i_list, CCollisionEvent* &i_head);
	inline CCollisionList* GetList(void) const {return(d_list);}
	inline bool ListOwnsEvent(void) const {return(d_listOwnsEvent);}
	inline CCollisionEvent* GetPrev(void) const {return(d_prev);}
	inline CCollisionEvent* GetNext(void) const {return(d_next);}

	inline CStageEntity* GetQueryEntity(void) const {return(d_queryEntity);}
	inline CStageEntity* GetTargetEntity(void) const {return(d_targetEntity);}
	inline CCollisionPair* &GetHeadPair(void) {return(d_head);}

	inline CCollisionEvent* SetTempListNext(CCollisionEvent* i_tempListNext);
	inline CCollisionEvent* GetTempListNext(void);

#ifdef DEBUG_STACK
	virtual const char* GetName(void);
	virtual void AppendDescription(char* io_msg);
#endif

  protected:
	inline CCollisionEvent(CCollisionStack &i_stack);

  private:
	static bool TestSimplePair(CStageEntity* i_queryEntity,
			CStageEntity* i_targetEntity, CCollisionEvent* &o_event,
			CCollisionStack* i_stack);
	static bool TestMixedPair(CStageEntity* i_complexEntity,
			CStageEntity* i_simpleEntity, bool i_queryComplex,
			CCollisionEvent* &o_event, CCollisionStack* i_stack,
			CStageEntity::ESelect i_select);
	static bool TestComplexPair(CStageEntity* i_queryEntity,
			CStageEntity* i_targetEntity, CCollisionEvent* &o_event,
			CCollisionStack* i_stack, CStageEntity::ESelect i_select);

	// Test a node and a stage entity.
	void Test(CCollisionNode* i_node, CCollisionObject* i_entityObject,
			bool i_queryNode, CCollisionStack* i_stack);

	// Test a pair of nodes.
	void Test(CCollisionNode* i_queryNode, CCollisionNode* i_targetNode,
			CCollisionStack* i_stack);

	CStageEntity* d_queryEntity;
	CStageEntity* d_targetEntity;
	CCollisionPair* d_head;
	CCollisionList* d_list;
	CCollisionEvent* d_prev;
	CCollisionEvent* d_next;
	CCollisionEvent* d_tempListNext;
	bool d_listOwnsEvent;
};

#define COLLISION_EVENT_HPP
#include "collide/Event.hpp"

#endif // COLLISION_EVENT_H
