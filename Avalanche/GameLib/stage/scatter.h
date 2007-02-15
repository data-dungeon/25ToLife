/*
**
**  File:   scatter.h
**  Date:   March 30, 2000
**  By:     Bryant Collard
**  Desc:   Scattered protagonists.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/
#ifndef SCATTER_H
#define SCATTER_H

#include "stage/cast.h"

class CCastMemberPool;
class CCollisionList;
class CCollisionEvent;
class CStageEntity;
class CProtagonist;

class CScatter: public CCast
{
public:
	CScatter(CCastMemberPool &i_castMemberPool) : CCast(i_castMemberPool) {}
	virtual ~CScatter(void) {}

	// The the update
	virtual void Advance(void);

private:
	CScatter();

	uint CollisionDetect(CProtagonist* i_actor, CCollisionList* &o_list);
	void CollisionAccept(CProtagonist* i_actor, CCollisionList* i_list);
	CCollisionEvent* BuildEventList(CCollisionList* i_list,
			const CStageEntity* i_query, const CStageEntity* i_target);
};

#endif // SCATTER_H
