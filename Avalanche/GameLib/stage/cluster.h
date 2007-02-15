/*
**
**  File:   Cluster.h
**  Date:   June 4, 2003
**  By:     Bryant Collard
**  Desc:   Clusters
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/stage/cluster.h $
**  $Revision: 21 $
**      $Date: 6/17/03 12:15p $
**    $Author: Bcollard $
**
*/

#ifndef CCLUSTER_H
#define CCLUSTER_H

#include "stage/cast.h"

class CCastMemberPool;
class CCollisionList;
class CCollisionEvent;
class CStageEntity;
class CProtagonist;

class CCluster: public CCast
{
public:
	CCluster(CCastMemberPool &i_castMemberPool);
	virtual ~CCluster();

	// Advance to the end of the frame.
	virtual void Advance(void);

private:
	CCluster();

	enum ECollisionResult
	{
		NO_DISCONTINUITY,
		FOUND_DISCONTINUITY,
		IRRECONCILABLE
	};
	ECollisionResult CollisionDetect(CCollisionList* &o_list);
	uint CollideJuggernaut(CCollisionList* &o_list, CProtagonist* i_actor,
			CCast* i_cast, CCastMember *i_member);
	uint CollideNormal(CCollisionList* &o_list, CProtagonist* i_actor,
			CCast* i_cast, CCastMember *i_member);
	uint CollideNormal(CCollisionList* &o_list, CProtagonist* i_actor,
			CCast* i_cast, CCastMember *i_member, CCast* i_displacedCast);
	uint CollideImpotent(CCollisionList* &o_list, CProtagonist* i_actor,
			CCast* i_cast, CCastMember *i_member);
	void CollisionAccept(CCollisionList* i_list);
	CCollisionEvent* BuildEventList(CCollisionList* i_list,
			const CStageEntity* i_entityA, const CStageEntity* i_entityB);
	CCollisionEvent* BuildOrderedEventList(CCollisionList* i_list,
			const CStageEntity* i_query, const CStageEntity* i_target);

public:
	// We are usually grouped into a doubly linked list
	CCluster							*prev, *next;

	// The frame this was used.
	uint32							usedFrame;
};

#endif // CCLUSTER_H
