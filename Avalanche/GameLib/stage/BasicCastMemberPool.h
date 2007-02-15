/*
**
**  File:   BasicCastMemberPool.h
**  Date:   January 30, 2002
**  By:     Bryant Collard
**  Desc:   Manages pools of cast members.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLib/stage/BasicCastMemberPool.h $
**  $Revision: 1 $
**      $Date: 1/31/02 3:12p $
**    $Author: Bcollard $
**
*/

#ifndef BASIC_CAST_MEMBER_POOL_H
#define BASIC_CAST_MEMBER_POOL_H

#include "platform/BaseType.h"
#include "stage/CastMemberPool.h"
#include "stage/castmember.h"
#include "container/pool.h"

class CActor;
class CCast;

class CBasicCastMemberPool : public CCastMemberPool
{
  public:
	CBasicCastMemberPool(uint i_startingSize = 0,
			uint i_automaticGrowSize = 64) :
		d_memberPool(i_startingSize, i_automaticGrowSize) {};
	virtual CCastMember* NewMember(CActor *i_actor, CCast *i_cast);
	virtual void DeleteMember(CCastMember *i_castMember);
	void Clear(void);
  private:
	Pool<CCastMember> d_memberPool;
};

inline CCastMember* CBasicCastMemberPool::NewMember(CActor *i_actor,
		CCast *i_cast)
{
	void *memberBuffer = d_memberPool.NewPtr();
	CCastMember *castMember = new (memberBuffer) CCastMember(i_actor, i_cast);
	ASSERT(castMember != NULL);

	return(castMember);
}

inline void CBasicCastMemberPool::DeleteMember(CCastMember *i_castMember)
{
	if (i_castMember)
		d_memberPool.Delete(i_castMember);
}

inline void CBasicCastMemberPool::Clear(void)
{
	d_memberPool.Clear();
}

#endif // BASIC_CAST_MEMBER_POOL_H
