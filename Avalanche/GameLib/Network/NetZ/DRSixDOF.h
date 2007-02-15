/*
**
**  File:   DRSixDOF.h
**  Date:   April 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of 6 degree of freedom dead reckoned state.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRSixDOF.h $
**  $Revision: #19 $
**  $DateTime: 2005/08/15 17:04:26 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef DR_SIX_DOF_H
#define DR_SIX_DOF_H

#ifndef CONSUMER_BUILD
#define DEBUG_SIX_DOF
#endif

#include <netz.h>
#include <extensions/deadreckoning.h>
#include "Network/NetZ/AvalancheDefines.h"
#include "Network/NetZ/DRModel.h"
#include "Components/Component.h"
#include "Math/Vector.h"
#include "Math/Quatern.h"

#ifdef DEBUG_SIX_DOF
#include "GameHelper/LogUtil.h"
#include "GameHelper/Message.h"

class DOActor;
#endif

class DSSixDOF;
class DSVector;

//////////////////////////// DRSixDOFParam /////////////////////////////

struct DRSixDOFParam
{
	// User set values
	float d_positionThreshold;
	float d_velocityThreshold;
	float d_degOrientationThreshold;
	float d_degOrientationRateThreshold;
	float d_hzTranslationNaturalFrequency;
	float d_translationDampingFactor;
	float d_secOrientationTimeConstant;
	float d_secOrientationRateTimeConstant;
	float d_positionClamp;
	float d_degOrientationClamp;

	// Processed values
	float d_positionThresholdSq;
	float d_velocityThresholdSq;
	float d_orientationThresholdCosHalfAngle;
	float d_radOrientationRateThresholdSq;
	float d_translationStiffness;
	float d_translationDamping;
	float d_positionClampSq;
	float d_orientationClampCosHalfAngle;
};

//////////////////////////// DRSixDOFModel /////////////////////////////

class DRSixDOFModel : public DRModel<DSSixDOF>
{
  public:
	DRSixDOFModel();
	virtual void Initialize(DuplicatedObject* i_do, DSSixDOF* i_dataset);

	qBool IsAccurateEnough(const DSSixDOF &i_master, Time i_time, 
			DuplicatedObject* i_masterDO, Station* i_duplicaStation);
	void ComputeValue(DSSixDOF* o_deadReckoned, Time i_time);
	void UpdateModelOnMaster(const DSSixDOF &i_master, Time i_time);
	void UpdateModelOnDuplica(const DSSixDOF &i_master);
	void UpdateModel(const DSSixDOF &i_master);

  private:
	DRSixDOFParam* d_param;
#ifdef LATENCY_COMPENSATION
	float d_updateTime;
#else
	int32 d_updateTime;
#endif
	uint8 d_flags;
	uint8 d_reason;
	Vector3Packed d_pos;
	Vector3Packed d_vel;
	ts_Quatvw d_orient;
	Vector3Packed d_axis;
	float d_omega;
	float d_lastTimeChecked;
	DSVector* d_acc;

#ifdef DEBUG_SIX_DOF
	bool d_drawDebug;
	CMessageOwner d_messageOwner;
	REF_SEND_MESSAGE_HANDLER(CDrawDebugHandler, DRSixDOFModel)
		d_drawDebugHandler;

	void ManageOutput(bool i_debug, bool i_master);
	void OpenOutput(CLogFile &i_file, const char* i_longRole, char* i_longFile,
			const char* i_shortRole, const char* i_shortFile, bool i_reason);
	void OutputState(CLogFile &i_file, const DSSixDOF &i_data,
			DOActor* i_actor, uint8 i_reason) const;
	CLogFile d_actorFile;
	CLogFile d_drFile;
	CLogFile d_updateFile;

  public:
	void OutputActorState(const DSSixDOF &i_data, DOActor* i_actor)
		{OutputState(d_actorFile, i_data, i_actor, 0);}
	void OutputDRState(const DSSixDOF &i_data, DOActor* i_actor)
		{OutputState(d_drFile, i_data, i_actor, 0);}
	void OutputUpdateState(const DSSixDOF &i_data, DOActor* i_actor)
		{OutputState(d_updateFile, i_data, i_actor, d_reason);}
#endif
};

////////////////////////// DRSixDOFModelPolicy //////////////////////////

class DRSixDOFModelPolicy : public StartupSharedModelPolicy<
		DSSixDOF, DRSixDOFModel>
{
};

////////////////////////// DRSixDOFComponent ///////////////////////////

class DRSixDOFComponent : public CActorComponent
{
  public:
	DRSixDOFComponent(CCompActor &i_actor, DuplicatedObject &i_do,
			DSSixDOF &i_ds);
	virtual const char *ComponentName(void) {return("DRSixDOF");}

	virtual void BeginFrame(void);
	virtual void BeginUpdate(void);
	virtual void AttemptUpdate(float i_deltaSec);
	virtual void EndUpdate(float i_deltaSec);
	void Pull(void);
	static void Push(CActor* i_actor, DSSixDOF &i_ds, float i_time,
			bool i_debug = false);

  private:
	DuplicatedObject &d_do;
	DSSixDOF &d_ds;
	float d_beginTime;
	float d_currTime;
	Vector3 d_beginBodyInWorld;
	Vector3 d_beginVelocityInWorld;
	ts_Quatvw d_beginOrient;
	Vector3 d_beginOmegaInWorld;

	DRSixDOFParam* d_param;
};

#endif // DR_SIX_DOF_H
