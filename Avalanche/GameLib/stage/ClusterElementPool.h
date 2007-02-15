/*
**
**  File:   ClusterElementPool.h
**  Date:   January 30, 2002
**  By:     Bryant Collard
**  Desc:   Manages pools of cluster elements.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/stage/ClusterElementPool.h $
**  $Revision: 1 $
**      $Date: 1/31/02 3:12p $
**    $Author: Bcollard $
**
*/

#ifndef CLUSTER_ELEMENT_POOL_H
#define CLUSTER_ELEMENT_POOL_H

#include "platform/BaseType.h"
#include "stage/CastMemberPool.h"
#include "stage/clustere.h"
#include "container/pool.h"

class CActor;
class CCast;

class CClusterElementPool : public CCastMemberPool
{
  public:
	CClusterElementPool(uint i_startingSize = 0,
			uint i_automaticGrowSize = 64) :
		d_memberPool(i_startingSize, i_automaticGrowSize) {};
	virtual CCastMember* NewMember(CActor *i_actor, CCast *i_cast);
	virtual void DeleteMember(CCastMember *i_castMember);
  private:
	Pool<ClusterElement> d_memberPool;
};

inline CCastMember* CClusterElementPool::NewMember(CActor *i_actor,
		CCast *i_cast)
{
	void *memberBuffer = d_memberPool.NewPtr();
	CCastMember *castMember = (CCastMember*) new (memberBuffer) ClusterElement(
			i_actor, i_cast);
	ASSERT(castMember != NULL);

	return(castMember);
}

inline void CClusterElementPool::DeleteMember(CCastMember *i_castMember)
{
	if (i_castMember)
		d_memberPool.Delete((ClusterElement*)i_castMember);
}

#endif // CLUSTER_ELEMENT_POOL_H
