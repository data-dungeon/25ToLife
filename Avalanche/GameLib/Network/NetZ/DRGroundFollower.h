/*
**
**  File:   DRGroundFollower.h
**  Date:   April 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of ground follower dead reackoning. This model
**          assumes that the object only rotates about its y-axis, the rate
**          of rotation about that axis is constant and that velocity relative
**          to the body is constant. Finally, if it is indicated that the
**          object is not on ground, constant vertical acceleration is assumed
**          and is set via a paramter file.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRGroundFollower.h $
**  $Revision: #22 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DR_GROUND_FOLLOWER_H
#define DR_GROUND_FOLLOWER_H

#ifndef CONSUMER_BUILD
#define DEBUG_GROUND_FOLLOWER
#endif

#include <netz.h>
#include "Network/NetZ/AvalancheDefines.h"
#include <extensions/deadreckoning.h>
#include "Network/NetZ/DRModel.h"
#include "Components/Component.h"
#include "Math/Vector.h"

#ifdef DEBUG_GROUND_FOLLOWER
#include "GameHelper/LogUtil.h"
#include "GameHelper/Message.h"

class DOActor;
#endif

class DSGroundFollower;
class DRDataset;

//////////////////////////// DRGroundFollowerParam ////////////////////////////

struct DRGroundFollowerParam
{
	// User set values
	float d_positionThreshold;
	float d_velocityThreshold;
	float d_degHeadingThreshold;
	float d_degHeadingRateThreshold;
	float d_gravity;
	float d_hzTranslationNaturalFrequency;
	float d_translationDampingFactor;
	float d_secHeadingTimeConstant;
	float d_secHeadingRateTimeConstant;
	float d_positionClamp;
	float d_degHeadingClamp;

	// Processed values
	float d_positionThresholdSq;
	float d_velocityThresholdSq;
	float d_radHeadingThreshold;
	float d_radHeadingRateThreshold;
	float d_translationStiffness;
	float d_translationDamping;
	float d_positionClampSq;
	float d_radHeadingClamp;
};

//////////////////////////// DRGroundFollowerModel ////////////////////////////

class DRGroundFollowerModel : public DRModel<DSGroundFollower>
{
  public:
	DRGroundFollowerModel();

	qBool IsAccurateEnough(const DSGroundFollower &i_master, Time i_time, 
			DuplicatedObject* i_masterDO, Station* i_duplicaStation);
	void ComputeValue(DSGroundFollower* o_deadReckoned, Time i_time);
	void UpdateModelOnMaster(const DSGroundFollower &i_master, Time i_time);
	void UpdateModelOnDuplica(const DSGroundFollower &i_master);
	void UpdateModel(const DSGroundFollower &i_master);

  private:
	DRGroundFollowerParam* d_param;
	Vector3Packed d_pos;
	Vector3Packed d_vel;
#ifdef LATENCY_COMPENSATION
	float d_updateTime;
#else
	int32 d_updateTime;
#endif
	float d_heading;
	float d_headingRate;
	float d_lastTimeChecked;
	uint8 d_flags;
	uint8 d_reason;

#ifdef DEBUG_GROUND_FOLLOWER
	bool d_drawDebug;
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CDrawDebugHandler, DRGroundFollowerModel)
		d_drawDebugHandler;

	void ManageOutput(bool i_debug, bool i_master);
	void OpenOutput(CLogFile &i_file, const char* i_longRole, char* i_longFile,
			const char* i_shortRole, const char* i_shortFile, bool i_reason);
	void OutputState(CLogFile &i_file, const DSGroundFollower &i_data,
			DOActor* i_actor, uint8 i_reason) const;
	CLogFile d_actorFile;
	CLogFile d_drFile;
	CLogFile d_updateFile;

  public:
	void OutputActorState(const DSGroundFollower &i_data, DOActor* i_actor)
		{OutputState(d_actorFile, i_data, i_actor, 0);}
	void OutputDRState(const DSGroundFollower &i_data, DOActor* i_actor)
		{OutputState(d_drFile, i_data, i_actor, 0);}
	void OutputUpdateState(const DSGroundFollower &i_data, DOActor* i_actor)
		{OutputState(d_updateFile, i_data, i_actor, d_reason);}
#endif
};

///////////////////////// DRGroundFollowerModelPolicy /////////////////////////

class DRGroundFollowerModelPolicy : public StartupSharedModelPolicy<
		DSGroundFollower, DRGroundFollowerModel>
{
};

///////////////////////// DRGroundFollowerComponent ///////////////////////////

class DRGroundFollowerComponent : public CActorComponent
{
  public:
	DRGroundFollowerComponent(CCompActor &i_actor, DuplicatedObject &i_do,
			DSGroundFollower &i_ds);
	virtual const char *ComponentName(void) {return("DRGroundFollower");}

	virtual void BeginFrame(void);
	virtual void BeginUpdate(void);
	virtual void AttemptUpdate(float i_deltaSec);
	virtual void EndUpdate(float i_deltaSec);
	virtual uint CollisionDetect(CStageEntity* i_entity,
			CCollisionEvent* i_event, bool i_mateDisplaced);
	bool ClampAtGround(void) {return(d_clampAtGround);}
	void Pull(void);
	static void Push(CActor* i_actor, DSGroundFollower &i_ds, float i_time,
			bool i_onGround = true, bool i_debug = false);

  private:
	DuplicatedObject &d_do;
	DSGroundFollower &d_ds;
	float d_beginTime;
	float d_currTime;
	Vector3 d_beginBodyInWorld;
	Vector3 d_beginVelocityInWorld;
	float d_beginHeading;
	float d_beginHeadingRate;
	bool d_clampAtGround;

	DRGroundFollowerParam* d_param;
};

#endif // DR_GROUND_FOLLOWER_H
