//============================================================================
//=
//= CollisionInfo.cpp - A component to sends out data of normal, position of 
//=                     intersect etc on a collision.
//=
//============================================================================

#ifndef _COLLISION_INFO_H
#define _COLLISION_INFO_H

#include "Components/Component.h"

//============================================================================

struct SCollisionInfo
{
	Vector3	d_intersection;
	Vector3	d_normal;
	CActor*	d_sender;
};

//============================================================================

class CCollisionInfo : public CActorComponent
{
public:
	// component base stuff
	CCollisionInfo( CCompActor& i_actor );
	virtual ~CCollisionInfo();
	const char *ComponentName()  { return "CollisionInfo"; }
	const char *ComponentClass() { return "KinematicDriver"; }
	virtual void BeginUpdate();
	virtual void AttemptUpdate( float i_deltaSec );
	virtual uint CCollisionInfo::CollisionDetect(CStageEntity* i_entity, CCollisionEvent* i_event, bool i_mateDisplaced);
	void CCollisionInfo::CollisionAccept( CStageEntity* i_entity, CCollisionEvent* i_event );

public:

private:
	Vector3	m_CollisionPoint;
	Vector3	m_CollisionNormal;
};

//============================================================================

#endif //_COLLISION_INFO_H
