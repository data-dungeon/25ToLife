/*
**
**  File:   StageEntity.h
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   The base class for all entities in the world.
**
**  Copyright (c) 2003, 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/stage/StageEntity.h $
**  $Revision: #21 $
**  $DateTime: 2005/08/25 14:58:50 $
**    $Author: Bryant_Collard $
**
*/

#ifndef STAGE_ENTITY_H
#define STAGE_ENTITY_H

// Engine includes //
#include "Layers/Assert/Assert.h"

// GameLib includes //
#include "stage/EntityType.h"
#include "stage/InteractionCategory.h"

class CCollisionCull;
class CCollisionEvent;
class CCollisionNode;
class CCollisionObject;
class CCollisionPair;

class CStageEntity
{
  public:
	// Construct as type.
	CStageEntity();
	virtual ~CStageEntity();

	// Get the type of the class. Each derived class should override this.
	static const CEntityType &ClassType(void);

	// Get the type of the instance.
	const CEntityType &InstanceType(void) const;

	// Test the type of the instance.
	bool IsA(const CEntityType &i_type) const;
	bool IsABase(const CEntityType &i_type) const;

	// Enable/disable interactions.
	void EnableInteraction(bool i_enable) {d_enableInteraction = i_enable;}
	bool IsInteractionEnabled(void) {return(d_enableInteraction);}

	// Get the interaction category.
	CInteractionCategory &Category(void);

	// Set interaction category masks.
	void SetPositiveInclude(const CInteractionCategory &i_mask);
	void SetPositiveInclude(int i_maskBit);
	void SetNegativeInclude(const CInteractionCategory &i_mask);
	void SetNegativeInclude(int i_maskBit);
	void SetPositiveExclude(const CInteractionCategory &i_mask);
	void SetPositiveExclude(int i_maskBit);
	void SetNegativeExclude(const CInteractionCategory &i_mask);
	void SetNegativeExclude(int i_maskBit);

	// Get interaction category masks.
	CInteractionCategory &PositiveInclude(void);
	CInteractionCategory &NegativeInclude(void);
	CInteractionCategory &PositiveExclude(void);
	CInteractionCategory &NegativeExclude(void);

	// See if the entity can interact with another entity.
	bool CanInteract(const CInteractionCategory &i_category) const;
	bool CanInteract(const CStageEntity &i_entity) const;

	// See if the entity can interact with actors.
	bool ActorInteract(void) const;

	// Establish the bounds of where the entity is to cull collision checks.
	virtual bool GetCollisionCull(CCollisionCull &o_cull);

	// Type of collision configurations.
	enum ECollisionConfiguration
	{
		SIMPLE = 0,
		SELECT = 2,
		COMPLEX = 3
	};

	// Type of configuration to use (if optional).
	enum ESelect
	{
		USE_SIMPLE = 0,
		USE_COMPLEX = 1
	};

	// Get a collision node for a collision test with the given entity.
	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);

	// Tell the entity that the collision test is complete.
	virtual bool EndCollisionTest(CCollisionEvent* i_event);

	// Indicate whether the entity is complex or not.
	uint ComplexBit(ESelect i_select);

	// Test if ignore entity for a simple collision configuration.
	bool IgnoreEntity(CStageEntity* i_entity);

	// Get the collision object for a simple collision configuration.
	CCollisionObject* GetCollisionObject(void);

	// Get properties associated with a collision.
	virtual uint16 Color(CCollisionPair* i_pair);
	virtual uint8 Material(CCollisionPair* i_pair);

	// Draw a collision event where the entity is the query entity.
	virtual void Draw(CCollisionEvent* i_event);

	// Describe the entity.
	virtual const char* GetClassName(void);
	virtual void AppendDescription(char* io_msg);

	static bool IsValid(CStageEntity* i_entity);

	void SetIgnoreEntity( CStageEntity* i_entity ) {d_ignoreEntity = i_entity;}

  protected:
	// Construct as base type.
	CStageEntity(const CEntityType &i_type);

	// The type of the instance. Used to figure out derived classes.
	CEntityType d_instanceType;

	// Enable/disable interactions.
	bool d_enableInteraction;

	// The interaction category of the entity.
	CInteractionCategory d_category;

	// The interaction category masks used to test other entities.
	CInteractionCategory d_positiveInclude;
	CInteractionCategory d_negativeInclude;
	CInteractionCategory d_positiveExclude;
	CInteractionCategory d_negativeExclude;

	// Collision configuration.
	ECollisionConfiguration d_collisionConfiguration;
	CStageEntity* d_ignoreEntity;
	CCollisionObject* d_collisionObject;

  private:
	// The type of the class. Each derived class should have a similar member.
	static const CEntityType s_stageEntityClass;
#ifndef DISABLE_ASSERTS
	uint32 d_magicNumber;
#endif
};

#define STAGE_ENTITY_HPP
#include "stage/StageEntity.hpp"

#endif // STAGE_ENTITY_H
