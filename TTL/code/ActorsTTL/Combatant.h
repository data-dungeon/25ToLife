/*
**
**  File:   Combatant.h
**  Date:   January 6, 2004
**  By:     Bryant Collard
**  Desc:   A temporary class that prepares for that design and implementation
**          of a more general dynamically configurable actor.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/ActorsTTL/Combatant.h $
**  $Revision: #24 $
**  $DateTime: 2005/07/24 00:17:51 $
**    $Author: Food $
**
*/

#pragma once

#ifndef COMBATANT_H
#define COMBATANT_H

#include "stage/protagonist.h"
#include "stage/CCompActor.h"
#include "Components/Component.h"
#include "components/KineControl/BipedModel.h"
#include "Components/PathFollower.h"
#include "EngineHelper/timer.h"
#include "Components/weapons/weapinv.h"
#include "components/Effects.h"
#include "GameHelper/Message.h"

class CCombatantCollisionParam;
class CCapsuleBonesNode;

// A collision node that uses a sphere that completely contains the bone nodes
// of a capsule node with children.
class CCapsuleWrapperNode : public CCollisionNode
{
  public:
	CCapsuleWrapperNode();
	void Set(CCapsuleBonesNode* i_capsuleNode, const Vector3* i_base);

	virtual CCollisionObject* BeginObjectTest(void);
	virtual bool IgnoreIntersection(CCollisionIntersection* i_intersection)
		{return(true);}
	virtual void EndObjectTest (ETestResult i_result,
			CCollisionPair* i_pair = NULL);
	virtual CCollisionNode* GetChild(void);

  private:
	CCapsuleBonesNode* d_capsuleNode;
	const Vector3* d_base;
	CSphereCollisionObject d_sphere;
};

class CCombatantFocus : public CActorComponent
{
public:
	CCombatantFocus( CCompActor &i_actor );

	const char *ComponentName() { return "CombatantFocus"; }

	void RunAnimation( void );

private:
	Vector3 focusOffset;
};

class CCombatant : public CCompActor
{
	friend CCombatantFocus;

public:
	CCombatant();
	CCombatant(const CEntityType &i_type);
	void CommonConstruct( void );

	virtual ~CCombatant();
	virtual bool Initialize(void);
	void Reset();

	// Handle
	static CCombatant* FromHandle(ActorHandle i_handle);
/*
 *	Override from CActor/CProtagonist
 */
	virtual void RunAnimation( bool i_forceUpdate );
	virtual void EndUpdate(float i_deltaSec);
	virtual void EndFrame( void );

	virtual void Placed( void );
	virtual void SyncActor(void);
	virtual uint GetTailoredInteractionHint(CProtagonist* i_actor);

	virtual const char* GetClassName(void) {return("Combatant");}

	void UpdateCapsuleSize( void );

	inline Vector3& GetAccelerationInWorld( void ) { return d_accelerationInWorld; }
	//inline Vector3& GetGroundNormal( void ) { return d_groundNormal; }
	//inline Vector3& GetGroundPosition( void ) { return d_groundPosition; }
	//inline u8& GetGroundMaterial( void ) { return d_groundMaterial; }

	virtual Vector3 &GetFocusInWorld(void);

	virtual CCollisionNode* BeginCollisionTest(CStageEntity* i_entity,
			ESelect i_select);
	virtual uint CollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, bool i_mateDisplaced);

	// Network
	virtual CNetActor* CreateNetActor(void);

	virtual CInterface *GetInterface( uint i_id );

	void PlaceOnGround( void );
	void Respawn(void);
	void ResetWeaponInventory( bool legalize = false );
	//int GetTeamId(){return teamId;}
	bool GetIsNpc(){return isNpc;}
	void SetIsNpc(bool iAmNpc){isNpc = iAmNpc;}

  protected:
	Vector3 d_accelerationInWorld;
	//Vector3 d_groundNormal;
	//Vector3 d_groundPosition;
	//u8 d_groundMaterial;
	Vector3 d_focusInWorld;
	CSimpleNode d_simpleNode;
	CCapsuleWrapperNode d_capsuleWrapperNode;

public:
	static const CEntityType &ClassType(void) {return(s_combatantClass);}

private:
	static const CEntityType s_combatantClass;
	//int combatantId;
	//int teamId;
	bool isNpc;
	ts_Bone* hipsBone;
	ts_Bone* headBone;

	CCombatantCollisionParam* d_collisionParam;
	REF_BASIC_MESSAGE_HANDLER(CCollisionChangeHandler, MESSAGE_VALUE, CCombatant)
		d_collisionChangeHandler;

	REF_BASIC_MESSAGE_HANDLER(CResetWeapons, MESSAGE_VALUE, CCombatant) 
		d_resetWeaponsHandler;

	REF_SEND_MESSAGE_HANDLER(CIKilledHostage, CCombatant) d_iKilledHostageHandler;
};

class CCombatantCollisionParam
{
  public:
	CCombatantCollisionParam();
	~CCombatantCollisionParam();

	Vector3Packed* d_size;
	Vector3Packed* d_pos;
	char* d_surfaceName;
};

#endif // COMBATANT_H
