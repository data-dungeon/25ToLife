/*
**
**  File:   DynamicEntities.hpp
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   Protagonists stored for general testing and fast culling.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/DynamicEntities.hpp $
**  $Revision: 6 $
**      $Date: 7/25/03 11:24a $
**    $Author: Tyler Colbert $
**
*/

#ifdef DYNAMIC_ENTITIES_HPP
#undef DYNAMIC_ENTITIES_HPP

// Engine includes //
#include "Layers/Assert/Assert.h"

// Gamelib includes //
#include "collide/Cull.h"
#include "collide/mrm.h"
#include "collide/xzrect.h"
#include "container/dynlist.h"
#include "stage/StageEntity.h"

/***************************************************************************/
/***************************************************************************/
inline CDynamicEntities::CDynamicEntities()
{
	d_mrm = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline void CDynamicEntities::Terminate(void)
{
	delete d_mrm;
	d_mrm = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline CDynamicEntities::~CDynamicEntities()
{
	Terminate();
}

/***************************************************************************/
/***************************************************************************/
inline void CDynamicEntities::Initialize(const XZRect* i_rect)
{
	if (d_mrm == NULL)
	{
		if (i_rect != NULL)
			d_mrm = new MultiResolutionMap<CStageEntity>(*i_rect, 4.0f);
		else
		{
			XZRect rect(-2000.0f, -2000.0f, 2000.0f, 2000.0f);
			d_mrm = new MultiResolutionMap<CStageEntity>(rect, 4.0f);
		}
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool CDynamicEntities::Add(CStageEntity* i_stageEntity)
{
	// Did we get initialized
	ASSERT(d_mrm != NULL);

	// Add this guy to the list!
	return(d_mrm->Add(i_stageEntity));
}

/***************************************************************************/
/***************************************************************************/
inline bool CDynamicEntities::Remove(CStageEntity* i_stageEntity)
{
	// Did we get initialized
	ASSERT(d_mrm != NULL);

	// Prune them from the tree
	return(d_mrm->Remove(i_stageEntity));
}

/***************************************************************************/
/***************************************************************************/
inline void CDynamicEntities::Update(void)
{
	// Only update if initialized
	if (d_mrm == NULL)
		return;

	// Update all the rects
	DynList<MultiResolutionMap<CStageEntity>::Entity> &list =
			d_mrm->GetEntityList();
	int count = list.Count();
	for (int i = 0; i < count; i++)
	{
		MultiResolutionMap<CStageEntity>::Entity *entity = list[i];

		// Setup the size
		CCollisionCull cull;
		bool hasSize = entity->entity->GetCollisionCull(cull);
		entity->size = cull.GetRect();

		// Are we active?
		d_mrm->Activate(entity,
				hasSize && entity->entity->CanInteract(QUERY_CATEGORY));
	}

	// Now update the world
	d_mrm->Update();
}

/***************************************************************************/
/***************************************************************************/
inline int CDynamicEntities::GetEntityList(CCollisionCull &i_cull,
		CStageEntity** &o_list) const
{
	// Make sure we live
	ASSERT(d_mrm);

	int count;
	// Build the list of stuff that we touch
	if (i_cull.GetType() == CCollisionCull::CULL_CHORD)
		o_list = d_mrm->Touches(i_cull.Start(), i_cull.Start() + i_cull.Chord(), count);
	else	
		o_list = d_mrm->Touches(i_cull.GetRect(), count);
	return(count);
}

#endif // DYNAMIC_ENTITIES_HPP
