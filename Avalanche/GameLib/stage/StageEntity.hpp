/*
**
**  File:   StageEntity.hpp
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   The base class for all entities in the world.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/StageEntity.hpp $
**  $Revision: 6 $
**      $Date: 7/28/03 2:38p $
**    $Author: Bcollard $
**
*/

#ifdef STAGE_ENTITY_HPP
#undef STAGE_ENTITY_HPP

// Gamelib includes //
#include "stage/EntityCategory.h"

#ifndef DISABLE_ASSERTS
#define VALID_MAGIC_NUMBER   0x8FDB77BE
#define INVALID_MAGIC_NUMBER 0x00000000
#endif

/***************************************************************************/
// Construct as type.
/***************************************************************************/
inline CStageEntity::CStageEntity() :
	d_instanceType(s_stageEntityClass)
{
	d_enableInteraction = true;
	d_category = NO_CATEGORY;
	d_positiveInclude = ALL_CATEGORIES;
	d_negativeInclude = NO_CATEGORY;
	d_positiveExclude = NO_CATEGORY;
	d_negativeExclude = NO_CATEGORY;

	d_collisionConfiguration = COMPLEX;
	d_ignoreEntity = NULL;
	d_collisionObject = NULL;

#ifndef DISABLE_ASSERTS
	d_magicNumber = VALID_MAGIC_NUMBER;
#endif
}

/***************************************************************************/
// Construct as base type.
/***************************************************************************/
inline CStageEntity::CStageEntity(const CEntityType &i_type) :
	d_instanceType(i_type)
{
	d_enableInteraction = true;
	d_category = NO_CATEGORY;
	d_positiveInclude = ALL_CATEGORIES;
	d_negativeInclude = NO_CATEGORY;
	d_positiveExclude = NO_CATEGORY;
	d_negativeExclude = NO_CATEGORY;

	d_collisionConfiguration = COMPLEX;
	d_ignoreEntity = NULL;
	d_collisionObject = NULL;

#ifndef DISABLE_ASSERTS
	d_magicNumber = VALID_MAGIC_NUMBER;
#endif
}

/***************************************************************************/
/***************************************************************************/
inline CStageEntity::~CStageEntity()
{
#ifndef DISABLE_ASSERTS
	d_magicNumber = INVALID_MAGIC_NUMBER;
#endif
}

/***************************************************************************/
// Get the type of the class. Each derived class should override this.
/***************************************************************************/
inline const CEntityType &CStageEntity::ClassType(void)
{
	return(s_stageEntityClass);
}

/***************************************************************************/
// Get the type of the instance.
/***************************************************************************/
inline const CEntityType &CStageEntity::InstanceType(void) const
{
	return(d_instanceType);
}

/***************************************************************************/
// Test the type of the instance.
/***************************************************************************/
inline bool CStageEntity::IsA(const CEntityType &i_type) const
{
	return(d_instanceType.IsA(i_type));
}

/***************************************************************************/
// Test the type of the instance.
/***************************************************************************/
inline bool CStageEntity::IsABase(const CEntityType &i_type) const
{
	return(d_instanceType.IsABase(i_type));
}

/***************************************************************************/
// Get the interaction category.
/***************************************************************************/
inline CInteractionCategory &CStageEntity::Category(void)
{
	return(d_category);
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetPositiveInclude(const CInteractionCategory &i_mask)
{
	d_positiveInclude = i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetPositiveInclude(int i_maskBit)
{
	d_positiveInclude = i_maskBit;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetNegativeInclude(const CInteractionCategory &i_mask)
{
	d_negativeInclude = i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetNegativeInclude(int i_maskBit)
{
	d_negativeInclude = i_maskBit;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetPositiveExclude(const CInteractionCategory &i_mask)
{
	d_positiveExclude = i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetPositiveExclude(int i_maskBit)
{
	d_positiveExclude = i_maskBit;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetNegativeExclude(const CInteractionCategory &i_mask)
{
	d_negativeExclude = i_mask;
}

/***************************************************************************/
/***************************************************************************/
inline void CStageEntity::SetNegativeExclude(int i_maskBit)
{
	d_negativeExclude = i_maskBit;
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CStageEntity::PositiveInclude(void)
{
	return(d_positiveInclude);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CStageEntity::NegativeInclude(void)
{
	return(d_negativeInclude);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CStageEntity::PositiveExclude(void)
{
	return(d_positiveExclude);
}

/***************************************************************************/
/***************************************************************************/
inline CInteractionCategory &CStageEntity::NegativeExclude(void)
{
	return(d_negativeExclude);
}

/***************************************************************************/
// Test interaction.
/***************************************************************************/
inline bool CStageEntity::CanInteract(
		const CInteractionCategory &i_category) const
{
	return(d_enableInteraction &&
			(d_positiveInclude.PositiveMatch(i_category) ||
			d_negativeInclude.NegativeMatch(i_category)) &&
			!d_positiveExclude.PositiveMatch(i_category) &&
			!d_negativeExclude.NegativeMatch(i_category));
}

/***************************************************************************/
// Test interaction.
/***************************************************************************/
inline bool CStageEntity::CanInteract(const CStageEntity &i_entity) const
{
	return(d_enableInteraction && i_entity.d_enableInteraction &&
			((d_positiveInclude.PositiveMatch(i_entity.d_category) ||
			d_negativeInclude.NegativeMatch(i_entity.d_category)) &&
			!d_positiveExclude.PositiveMatch(i_entity.d_category) &&
			!d_negativeExclude.NegativeMatch(i_entity.d_category)) &&
			((i_entity.d_positiveInclude.PositiveMatch(d_category) ||
			i_entity.d_negativeInclude.NegativeMatch(d_category)) &&
			!i_entity.d_positiveExclude.PositiveMatch(d_category) &&
			!i_entity.d_negativeExclude.NegativeMatch(d_category)));
}

/***************************************************************************/
// See if the entity can interact with dynamic entities.
/***************************************************************************/
inline bool CStageEntity::ActorInteract(void) const
{
	return(d_enableInteraction &&
			(d_positiveInclude.PositiveMatch(ALL_ACTOR_CATEGORIES) ||
			d_negativeInclude.PositiveMatch(ALL_ACTOR_CATEGORIES)));
}

/***************************************************************************/
// Indicate whether the entity is complex or not.
/***************************************************************************/
inline uint CStageEntity::ComplexBit(ESelect i_select)
{
	return(((uint)d_collisionConfiguration >> (uint)i_select) & 0x1);
}

/***************************************************************************/
// Test if ignore entity for a simple collision configuration.
/***************************************************************************/
inline bool CStageEntity::IgnoreEntity(CStageEntity* i_entity)
{
	return(i_entity == d_ignoreEntity);
}

/***************************************************************************/
// Get the collision object for a simple collision configuration.
/***************************************************************************/
inline CCollisionObject* CStageEntity::GetCollisionObject(void)
{
	return(d_collisionObject);
}

/***************************************************************************/
/***************************************************************************/
inline bool CStageEntity::IsValid(CStageEntity* i_entity)
{
#ifndef DISABLE_ASSERTS
	return(i_entity->d_magicNumber == VALID_MAGIC_NUMBER);
#else
	return(true);
#endif
}

#endif // STAGE_ENTITY_HPP
