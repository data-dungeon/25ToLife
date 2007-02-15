/*
**
**  File:   DynamicEntities.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   Protagonists stored for general testing and fast culling.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/becCollide/DynamicEntities.h $
**  $Revision: 1 $
**      $Date: 3/13/03 9:43a $
**    $Author: Bcollard $
**
*/

#ifndef DYNAMIC_ENTITIES_H
#define DYNAMIC_ENTITIES_H

class XZRect;
class CStageEntity;
class CCollisionCull;
template <class T> class MultiResolutionMap;

class CDynamicEntities
{
  public:
	CDynamicEntities();
	~CDynamicEntities();

	void Initialize(const XZRect* i_rect);
	void Terminate(void);
	bool Add(CStageEntity* i_stageEntity);
	bool Remove(CStageEntity* i_stageEntity);
	void Update(void);
	int GetEntityList(CCollisionCull &i_cull, CStageEntity** &o_list) const;

  private:
	MultiResolutionMap<CStageEntity>* d_mrm;
};

#define DYNAMIC_ENTITIES_HPP
#include "collide/DynamicEntities.hpp"

#endif // DYNAMIC_ENTITIES_H
