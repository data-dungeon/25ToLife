/*
**
**  File:   StaticEntity.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A stage entity that manages all static collision objects in the
**          world.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/StaticEntity.h $
**  $Revision: #11 $
**  $DateTime: 2005/06/08 16:04:21 $
**    $Author: Bryant_Collard $
**
*/

#ifndef STATIC_ENTITY_H
#define STATIC_ENTITY_H

// Engine includes //
#include "Layers/LayersStruct.h"

// Gamelib includes //
#include "stage/StageEntity.h"
#include "collide/Node.h"
#include "collide/ObjectLayer.h"
#include "container/dllist.h"

// Forwards
class CCollisionStack;
class CCollisionObject;
class CStaticCollisionObject;
template<class T> class CCollisionStackArray;

class CStaticChild : public CCollisionNode
{
  public:
	CStaticChild();
	void Initialize(CCollisionStack* i_stack);
	bool InitCollisionTest(CCollisionCull &i_cull, ObjectLayerList* i_layerList);
	void TerminateCollisionTest(void);
	CCollisionNode* GetCurrentChild(void);

	// Override collision node
	virtual CCollisionObject* BeginObjectTest(void);

  private:
	CCollisionStack* d_stack;
	CCollisionStackArray<CStaticCollisionObject*>* d_list;
	int d_index;
};

class CStaticEntity : public CStageEntity, public CCollisionNode
{
  public:
	CStaticEntity(unsigned int &i_layerMask);

	static const CEntityType &ClassType(void);

	void Initialize(CCollisionStack* i_stack);
	void Terminate(void);
	ObjectLayerList* Activate(t_Handle i_handle);
	void Deactivate(ObjectLayerList* i_list);

	// Override stage entity
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);
	virtual bool EndCollisionTest(CCollisionEvent* i_event);
	virtual uint16 Color(CCollisionPair* i_pair);
	virtual uint8 Material(CCollisionPair* i_pair);
	virtual void Draw(CCollisionEvent* i_event);
	virtual const char* GetName(void);

	// Override collision node
	virtual CCollisionNode* GetChild(void);

  private:
	CStaticEntity();

	static const CEntityType s_staticEntityClass;

	unsigned int &d_layerMask;
	DoublyLinkedList<ObjectLayerList> d_dbList;

	CStaticChild d_child;
};

#define STATIC_ENTITY_HPP
#include "collide/StaticEntity.hpp"

#endif // STATIC_ENTITY_H
