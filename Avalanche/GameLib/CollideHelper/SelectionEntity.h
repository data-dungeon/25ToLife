/*
**
**  File:   SelectionEntity.h
**  Date:   September 2, 2005
**  By:     Bryant Collard
**  Desc:   A query entity that uses a provided entity to "select" collision
**          objects to be used in additional tests.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef SELECTION_ENTITY_H
#define SELECTION_ENTITY_H

#include "collide/Node.h"
#include "collide/Stack.h"
#include "stage/StageEntity.h"

class CCollisionList;
class CCollisionEvent;
class CCollisionPair;
class CCollisionObject;
class CStageEntity;

class CSelectionChildNode : public CCollisionNode
{
  public:
	CSelectionChildNode();
	void Setup(CCollisionList* i_list);
	void Reset(void);
	CCollisionNode* Next(void);
	virtual CCollisionObject* BeginObjectTest(void);

  private:
	CCollisionList* d_list;
	CCollisionEvent* d_event;
	CCollisionPair* d_pair;
};

class CSelectionNode : public CCollisionNode
{
  public:
	CSelectionNode() {d_cullObject = NULL;}
	void Setup(CCollisionList* i_list);
	void BeginNodeTest(CCollisionObject* i_object);
	virtual CCollisionObject* BeginObjectTest(void) {return(d_cullObject);}
	virtual bool IgnoreIntersection(CCollisionIntersection* i_intersection)
		{return(true);}
	virtual CCollisionNode* GetChild(void);

  private:
	CCollisionObject* d_cullObject;
	CSelectionChildNode d_child;
};

class CSelectionEntity : public CStageEntity
{
  public:
	// Construct
	CSelectionEntity();

	// Select collision objects using some selector entity, e.g. an ObbEntity.
	// If either the selector or any of the targets have a selectable
	// configuration, i_select controls which will be used when the selection is
	// performed. The selector's collision cull object will be returned in
	// subsequent uses of this entity. If i_useRootSelectorObject is true, the
	// first available collision object of the selector is used for initial
	// culling in any collision test. For example, suppose the selector entity is
	// an ObbEntity. Select() will perform a collision test between the OBB and
	// all the other entities. The result will contain a list of collision
	// objects that touch the OBB. If i_useRootSelectorObject is true, any test
	// between the selection entity and any other entity will first test for an
	// intersection with the OBB although the detected intersections will not be
	// returned in the results of the test. If no intersection is found, no other
	// tests will be performed. If it is false, all of the selected objects will
	// be tested regardless of whether or not the target object touches the OBB
	// and may even find intersections fully outside of the OBB.
	bool Select(CStageEntity* i_selector, ESelect i_select = USE_SIMPLE,
			bool i_useRootSelectorObject = true);

	// Clear the selector and anything associated with it.
	void Clear(void);

	// Override CStageEntity
	virtual bool GetCollisionCull(CCollisionCull &o_cull);
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);
	virtual const char* GetClassName(void) {return("Selection");}

  private:
	CStageEntity* d_selector;
	bool d_useRootSelectorObject;
	CCollisionStackLock d_lock;
	CSelectionNode d_node;
};

#endif // SELECTION_ENTITY_H
