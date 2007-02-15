/*
**
**  File:   DRPathFollower.h
**  Date:   October 15, 2004
**  By:     Bryce Thomsen
**  Desc:   Implementation of path follower dead reackoning. This model
**          assumes that the object only rotates about its y-axis, the rate
**          of rotation about that axis is constant and that velocity relative
**          to the body is constant. Finally, if it is indicated that the
**          object is not on ground, constant vertical acceleration is assumed
**          and is set via a paramter file.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRPathFollower.h $
**  $Revision: #15 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DR_PATH_FOLLOWER_H
#define DR_PATH_FOLLOWER_H

#ifndef CONSUMER_BUILD
//#define DEBUG_PATH_FOLLOWER
#endif

#include <netz.h>
#include <extensions/deadreckoning.h>
#include "Network/NetZ/AvalancheDefines.h"
#include "Network/NetZ/DRModel.h"
#include "Components/Component.h"
#include "Components/PathFollower.h"
#include "Math/Vector.h"
#ifdef DEBUG_PATH_FOLLOWER
#include <stdio.h>
#include "GameHelper/Message.h"
#endif

typedef enum
{
	STATE_DO_JOB,
	STATE_WATCH,
	STATE_FLEE,
	STATE_COWER,
	STATE_FIGHT,
	STATE_HOSTAGE,
	STATE_SITTING,
	STATE_STUNNED,
	STATE_ON_FIRE,
	STATE_VAULTING,
	STATE_DEAD
} PF_BEHAVIOR_STATES;

class DSPathFollower;
class DRDataset;

//////////////////////////// DRPathFollowerParam ////////////////////////////

struct DRPathFollowerParam
{
	// User set values
	float d_distanceThreshold;
	float d_speedCurrentThreshold;
	float d_speedTargetThreshold;
};

//////////////////////////// DRPathFollowerModel ////////////////////////////

class DRPathFollowerModel : public DRModel<DSPathFollower>
{
public:
	DRPathFollowerModel(void);

	// Custom deadreckoning functions
	qBool IsAccurateEnough(const DSPathFollower &i_master, Time i_time, 
			DuplicatedObject* i_masterDO, Station* i_duplicaStation);
	void ComputeValue(DSPathFollower* o_deadReckoned, Time i_time);
	void UpdateModelOnMaster(const DSPathFollower &i_master, Time i_time);
	void UpdateModelOnDuplica(const DSPathFollower &i_master);

private:
	void UpdateModel(const DSPathFollower &i_master);

	DRPathFollowerParam* d_param;
#ifdef LATENCY_COMPENSATION
	float d_updateTime;
#else
	int32 d_updateTime;
#endif
	float d_lastTimeChecked;
	uint8 d_flags;

	// Vars used to calculate the local model
	uint8 d_prevState;
	uint8 d_state;
	float d_speedTarget;
	float d_speedCurrent;
	float d_distance;

#ifdef DEBUG_PATH_FOLLOWER
	enum
	{
		REASON_NONE,
		REASON_DISCONTINUITY,
		REASON_STATE,
		REASON_SPEED_CURRENT,
		REASON_SPEED_TARGET,
		REASON_DISTANCE
	} d_reason;
	bool d_drawDebug;
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CDrawDebugHandler, DRPathFollowerModel)
		d_drawDebugHandler;

	void ManageOutput(bool i_debug, bool i_master);
	FILE* OpenOutput(const char* i_longRole, char* i_longFile,
			const char* i_shortRole, const char* i_shortFile, bool i_reason);
	void OutputState(FILE* i_fp, const DSPathFollower &i_data,
			DOActor* i_actor, int i_reason) const;
	FILE* d_actorFile;
	FILE* d_drFile;
	FILE* d_updateFile;

  public:
	void OutputActorState(const DSPathFollower &i_data, DOActor* i_actor) const
		{OutputState(d_actorFile, i_data, i_actor, REASON_NONE);}
	void OutputDRState(const DSPathFollower &i_data, DOActor* i_actor) const
		{OutputState(d_drFile, i_data, i_actor, REASON_NONE);}
	void OutputUpdateState(const DSPathFollower &i_data, DOActor* i_actor) const
		{OutputState(d_updateFile, i_data, i_actor, d_reason);}
#endif
};

///////////////////////// DRPathFollowerModelPolicy /////////////////////////

class DRPathFollowerModelPolicy : public StartupSharedModelPolicy<
		DSPathFollower, DRPathFollowerModel>
{
};

///////////////////////// DRPathFollowerUtility ///////////////////////////

class DRPathFollowerUtility
{
public:
	static float AdjustSpeed(DSPathFollower* i_dsPF, float i_deltaSec);
	static float AdjustSpeed(CPathFollower* i_pPF, float i_speedTarget, 
			float i_speedCurrent, float i_distance, float i_deltaSec);
	static void ProceedAlongPath(CPathFollower* i_pPF, float &io_speedTarget, 
		float &io_speedCurrent, float &io_distance, float i_deltaSec);
	static void ProceedAlongPartialPath(CPathFollower* i_pPF, float &io_speedTarget, 
			float &io_speedCurrent, float &io_distance, float i_deltaSec,
			float &o_extraSec);
private:
	static bool CanMakeTurn(DSPathFollower* i_dsPF, float i_deltaSec);
	static bool CanMakeTurn(CPathFollower* i_pPF, float i_speedCurrent, 
			float i_distance, float i_deltaSec);
	static float SpeedUp(DSPathFollower* i_dsPF, float i_deltaSec);
	static float SpeedUp(CPathFollower* i_pPF, float i_speedTarget, 
			float i_speedCurrent, float i_deltaSec);
};

///////////////////////// DRPathFollowerComponent ///////////////////////////

class DRPathFollowerComponent : public CActorComponent
{
public:
	DRPathFollowerComponent(CCompActor &i_actor, DuplicatedObject &i_do,
			DSPathFollower &i_ds);
	virtual const char *ComponentName(void) {return("DRPathFollower");}

	virtual void BeginFrame(void);
	virtual void BeginUpdate(void);
	virtual void AttemptUpdate(float i_deltaSec);
	virtual void EndUpdate(float i_deltaSec);
	void Pull(void);
	static void Push(CActor* i_actor, DSPathFollower &i_ds, float i_time,
			bool i_onPath = true, bool i_debug = false);
	static void SetPathFollowerObject(CPathFollower* i_pathFollower, 
			DSPathFollower &i_ds);

private:
	DuplicatedObject &d_do;
	DSPathFollower &d_ds;
	float d_beginTime;
	float d_currTime;
	uint8 d_beginState;
	float d_beginSpeedTarget;
	float d_beginSpeedCurrent;
	float d_beginDistance;

	CPathFollower* d_pathFollower;
	DRPathFollowerParam* d_param;
};

#endif // DR_PATH_FOLLOWER_H
