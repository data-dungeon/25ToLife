/*
**
**  File:   DRSixDOF.cpp
**  Date:   April 30, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of 6 degree of freedom dead reckoned state.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRSixDOF.cpp $
**  $Revision: #36 $
**  $DateTime: 2005/07/30 13:45:34 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DRSixDOF.h"

// Local includes
#include "Network/Session.h"
#include "Network/NetZ/SessionTime.h"
#include "Network/NetZ/DSSixDOF.h"
#include "Network/NetZ/DSVector.h"

// Gamelib includes
#include "GameHelper/ParamMgr.h"
#include "Components/Component.h"
#include "EngineHelper/timer.h"
#include "stage/CCompActor.h"
#include "Environ/World.h"

#ifdef DEBUG_SIX_DOF
#include "EngineHelper/drawutil.h"
#endif

//#define OUTPUT_WHEN_SENDING
#ifdef OUTPUT_WHEN_SENDING
#include "Network/NetZ/DOActor.h"
#endif

// Record why an update is sent.
#define REASON_NONE             0x00
#define REASON_DISCONTINUITY    0x01
#define REASON_POSITION         0x02
#define REASON_VELOCITY         0x04
#define REASON_ORIENTATION_RATE 0x08
#define REASON_ORIENTATION      0x10
#define NON_REASON_TOO_SOON     0x20

#define DISCONTINUITY_ANY (DISCONTINUITY_POSITION | DISCONTINUITY_VELOCITY | \
			DISCONTINUITY_ORIENTATION | DISCONTINUITY_ANGULAR_VELOCITY)

// Debugging
#ifdef DEBUG_SIX_DOF
#define FLAG_DEBUG       0x80
#define CONTINUE_TEST(t) true
#else
#define FLAG_DEBUG       0x00
#define CONTINUE_TEST(t) ((t) == 0)
#endif

// Compress/decompress for network transmission.
#define MAX_VEL                100.0f
#define MAX_OMEGA              100.0f
#define MAX_DR_SERVICE_TIME    (0.08f)

/******************************************************************************/
/******************************************************************************/
static void VecToComp(Vector3CRef i_vec, float i_maxValue, int16 &o_x,
		int16 &o_y, int16 &o_z)
{
	o_x = Math::SignedCompressToWord(i_vec.X(), i_maxValue);
	o_y = Math::SignedCompressToWord(i_vec.Y(), i_maxValue);
	o_z = Math::SignedCompressToWord(i_vec.Z(), i_maxValue);
}

/******************************************************************************/
/******************************************************************************/
static void CompToVec(int16 i_x, int16 i_y, int16 i_z, float i_maxValue,
		Vector3 &o_vec)
{
	o_vec.Set(Math::SignedDecompress(i_x, i_maxValue),
			Math::SignedDecompress(i_y, i_maxValue),
			Math::SignedDecompress(i_z, i_maxValue));
}

/******************************************************************************/
/******************************************************************************/
static void CompToPackedVec(int16 i_x, int16 i_y, int16 i_z, float i_maxValue,
		Vector3Packed &o_vec)
{
	o_vec.Set(Math::SignedDecompress(i_x, i_maxValue),
			Math::SignedDecompress(i_y, i_maxValue),
			Math::SignedDecompress(i_z, i_maxValue));
}

/******************************************************************************/
/******************************************************************************/
static void QuatToComp(const ts_Quatvw &i_quat, int16 &o_x, int16 &o_y,
		int16 &o_z, int16 &o_w)
{
	o_x = Math::SignedCompressToWord(i_quat.V.X(), 1.0f);
	o_y = Math::SignedCompressToWord(i_quat.V.Y(), 1.0f);
	o_z = Math::SignedCompressToWord(i_quat.V.Z(), 1.0f);
	o_w = Math::SignedCompressToWord(i_quat.W, 1.0f);
}

/******************************************************************************/
/******************************************************************************/
static void CompToQuat(int16 i_x, int16 i_y, int16 i_z, int16 i_w,
		ts_Quatvw &o_quat)
{
	o_quat.V.Set(Math::SignedDecompress(i_x, 1.0f),
			Math::SignedDecompress(i_y, 1.0f), Math::SignedDecompress(i_z, 1.0f));
	o_quat.W = Math::SignedDecompress(i_w, 1.0f);
}

//////////////////////////////// DRSixDOFParam /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
enum
{
	PARAM_CHANGE_POSITION_THRESHOLD,
	PARAM_CHANGE_VELOCITY_THRESHOLD,
	PARAM_CHANGE_ORIENTATION_THRESHOLD,
	PARAM_CHANGE_ORIENTATION_RATE_THRESHOLD,
	PARAM_CHANGE_TRANSLATION_FILTER,
	PARAM_CHANGE_POSITION_CLAMP,
	PARAM_CHANGE_ORIENTATION_CLAMP
};

/******************************************************************************/
/******************************************************************************/
void CParam<DRSixDOFParam>::RegisterVariables(void)
{
	RegisterVariable(d_data.d_positionThreshold, "PositionMeters",
			0.15f, 0.0f, 1.0f, (void*)PARAM_CHANGE_POSITION_THRESHOLD);
	RegisterVariable(d_data.d_velocityThreshold,
			"VelocityMetersPerSec", 0.5f, 0.0f, 5.0f,
			(void*)PARAM_CHANGE_VELOCITY_THRESHOLD);
	RegisterVariable(d_data.d_degOrientationThreshold, "OrientationDeg",
			10.0f, 0.0f, 45.0f, (void*)PARAM_CHANGE_ORIENTATION_THRESHOLD);
	RegisterVariable(d_data.d_degOrientationRateThreshold,
			"OrientationRateDegPerSec", 30.0f, 0.0f, 180.0f,
			(void*)PARAM_CHANGE_ORIENTATION_RATE_THRESHOLD);
	RegisterVariable(d_data.d_hzTranslationNaturalFrequency,
			"TranslationNaturalFrequencyHz", 2.0f, 0.0f, 10.0f,
			(void*)PARAM_CHANGE_TRANSLATION_FILTER);
	RegisterVariable(d_data.d_translationDampingFactor,
			"TranslationDampingFactor", 0.9f, 0.0f, 2.0f,
			(void*)PARAM_CHANGE_TRANSLATION_FILTER);
	RegisterVariable(d_data.d_secOrientationTimeConstant,
			"OrientationTimeConstantSec", 0.2f, 0.0f, 2.0f);
	RegisterVariable(d_data.d_secOrientationRateTimeConstant,
			"OrientationRateTimeConstantSec", 0.4f, 0.0f, 2.0f);
	RegisterVariable(d_data.d_positionClamp, "PositionClampMeters",
			0.03f, 0.0f, 0.5f, (void*)PARAM_CHANGE_POSITION_CLAMP);
	RegisterVariable(d_data.d_degOrientationClamp, "OrientationClampDeg",
			3.0f, 0.0f, 20.0f, (void*)PARAM_CHANGE_ORIENTATION_CLAMP);

	d_data.d_positionThresholdSq =
			d_data.d_positionThreshold * d_data.d_positionThreshold;
	d_data.d_velocityThresholdSq = d_data.d_velocityThreshold *
			d_data.d_velocityThreshold;
	d_data.d_orientationThresholdCosHalfAngle =
			Math::CosDeg(0.5f * d_data.d_degOrientationThreshold);
	d_data.d_radOrientationRateThresholdSq =
			Math::Deg2Rad(d_data.d_degOrientationRateThreshold) *
			Math::Deg2Rad(d_data.d_degOrientationRateThreshold);
	float naturalFrequency = Math::TwoPi * d_data.d_hzTranslationNaturalFrequency;
	d_data.d_translationStiffness = naturalFrequency * naturalFrequency;
	d_data.d_translationDamping = 2.0f * d_data.d_translationDampingFactor *
			naturalFrequency;
	d_data.d_positionClampSq =
			d_data.d_positionClamp * d_data.d_positionClamp;
	d_data.d_orientationClampCosHalfAngle =
			Math::CosDeg(0.5f * d_data.d_degOrientationClamp);
}

/******************************************************************************/
/******************************************************************************/
void CParam<DRSixDOFParam>::CParamMessageHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	switch((uint)i_data)
	{
	 case PARAM_CHANGE_POSITION_THRESHOLD:
		d_data->d_positionThresholdSq =
				d_data->d_positionThreshold * d_data->d_positionThreshold;
		break;
	 case PARAM_CHANGE_VELOCITY_THRESHOLD:
		d_data->d_velocityThresholdSq = d_data->d_velocityThreshold *
				d_data->d_velocityThreshold;
		break;
	 case PARAM_CHANGE_ORIENTATION_THRESHOLD:
		d_data->d_orientationThresholdCosHalfAngle =
				Math::CosDeg(0.5f * d_data->d_degOrientationThreshold);
		break;
	 case PARAM_CHANGE_ORIENTATION_RATE_THRESHOLD:
		d_data->d_radOrientationRateThresholdSq =
				Math::Deg2Rad(d_data->d_degOrientationRateThreshold) *
				Math::Deg2Rad(d_data->d_degOrientationRateThreshold);
		break;
	 case PARAM_CHANGE_TRANSLATION_FILTER:
	 {
		float naturalFrequency =
				Math::TwoPi * d_data->d_hzTranslationNaturalFrequency;
		d_data->d_translationStiffness = naturalFrequency * naturalFrequency;
		d_data->d_translationDamping = 2.0f * d_data->d_translationDampingFactor *
				naturalFrequency;
		break;
	 }
	 case PARAM_CHANGE_POSITION_CLAMP:
		d_data->d_positionClampSq =
				d_data->d_positionClamp * d_data->d_positionClamp;
		break;
	 case PARAM_CHANGE_ORIENTATION_CLAMP:
		d_data->d_orientationClampCosHalfAngle =
				Math::CosDeg(0.5f * d_data->d_degOrientationClamp);
		break;
	}
}

//////////////////////////////// DRSixDOFModel /////////////////////////////////

/******************************************************************************/
/******************************************************************************/
DRSixDOFModel::DRSixDOFModel()
{
#ifdef LATENCY_COMPENSATION
	d_updateTime = 0.0f;
#else
	d_updateTime = 0;
#endif
	d_flags = 0;
	d_reason = REASON_NONE;
	d_pos.Clear();
	d_vel.Clear();
	qatIdentity(&d_orient);
	d_axis.Set(0.0f, 1.0f, 0.0f);
	d_omega = 0.0f;
	d_lastTimeChecked = 0.0f;
	d_acc = NULL;

	d_param = CParam<DRSixDOFParam>::GetParam("DRSixDOF", "var");
	ASSERT_PTR(d_param);

#ifdef DEBUG_SIX_DOF
	d_drawDebug = false;
	d_drawDebugHandler.Initialize("ToggleDrawDeadReckoning",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
#endif
}

#ifdef DEBUG_SIX_DOF
/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::CDrawDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_drawDebug = !d_ref->d_drawDebug;
}
#endif

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::Initialize(DuplicatedObject* i_do, DSSixDOF* i_dataset)
{
	if (i_dataset != NULL)
		d_acc = &i_dataset->GetAccelerationDataset();
}

/******************************************************************************/
/******************************************************************************/
qBool DRSixDOFModel::IsAccurateEnough(const DSSixDOF &i_master,
		Time i_time, DuplicatedObject* i_masterDO, Station* i_duplicaStation)
{
#ifdef DEBUG_SIX_DOF
	OutputActorState(i_master, GetActor());
#endif

	// Clear the reason to send.
	d_reason = REASON_NONE;

	// Always send on a discontinuity.
	if (((i_master.d_flags & DISCONTINUITY_ANY) != 0) ||
			((d_flags & FLAG_DEBUG)) != (i_master.d_flags & FLAG_DEBUG))
		d_reason = REASON_DISCONTINUITY;

	// DAS: Ok I notice that when we are hammering the DR with continuity breaks
	// we can easily end up sending a new packet every single frame. I want to
	// set a time limit on this behavior so that a certain amount of time has
	// passed before sending a new packet, say 50-80ms with bundling enabled we
	// get this kind of behavior anyway since 2 or 3 packets may get get sent in
	// a single bundle and the last packet is the only one that will really be
	// used. This should cut our bandwidth in half, for real bad continuity
	// breaks.
	float curTime = g_timer.GetOSStartSec();
	if (d_lastTimeChecked  > curTime)
		d_reason |= NON_REASON_TOO_SOON;

	// Check position threshold.
	DSSixDOF deadReckoned;
	if (CONTINUE_TEST(d_reason))
	{
		Vector3 masterPos, drPos;
		ComputeValue(&deadReckoned, i_time);
		g_world.DecompressPosition(i_master.d_posX, i_master.d_posY,
				i_master.d_posZ, masterPos);
		g_world.DecompressPosition(deadReckoned.d_posX, deadReckoned.d_posY,
				deadReckoned.d_posZ, drPos);
		if ((masterPos - drPos).LengthSquared() > d_param->d_positionThresholdSq)
			d_reason |= REASON_POSITION;
	}

	// Check velocity threshold.
	if (CONTINUE_TEST(d_reason))
	{
		Vector3 masterVel, drVel;
		CompToVec(i_master.d_velX, i_master.d_velY, i_master.d_velZ, MAX_VEL,
				masterVel);
		CompToVec(deadReckoned.d_velX, deadReckoned.d_velY, deadReckoned.d_velZ,
				MAX_VEL, drVel);
		if ((masterVel - drVel).LengthSquared() > d_param->d_velocityThresholdSq)
			d_reason |= REASON_VELOCITY;
	}

	// Check orientation rate threshold.
	if (CONTINUE_TEST(d_reason))
	{
		Vector3 masterOmega, drOmega;
		CompToVec(i_master.d_omegaX, i_master.d_omegaY, i_master.d_omegaZ,
				MAX_OMEGA, masterOmega);
		CompToVec(deadReckoned.d_omegaX, deadReckoned.d_omegaY,
				deadReckoned.d_omegaZ, MAX_OMEGA, drOmega);
		if ((masterOmega - drOmega).LengthSquared() >
				d_param->d_radOrientationRateThresholdSq)
			d_reason |= REASON_ORIENTATION_RATE;
	}

	// Check orientation threshold. Note that negating all components of a
	// quaternion yields an equivalent quaternion; therefore, all quaternions can
	// be expressed with a positive scalar component and that can be used for
	// checking the threshold.
	if (CONTINUE_TEST(d_reason))
	{
		ts_Quatvw drOrient, masterInvOrient, deltaOrient;
		CompToQuat(deadReckoned.d_orientX, deadReckoned.d_orientY,
				deadReckoned.d_orientZ, deadReckoned.d_orientW, drOrient);
		CompToQuat(-i_master.d_orientX, -i_master.d_orientY,
				-i_master.d_orientZ, i_master.d_orientW, masterInvOrient);
		qatMul(&drOrient, &masterInvOrient, &deltaOrient);
		if (Math::Abs(deltaOrient.W) <= d_param->d_orientationThresholdCosHalfAngle)
			d_reason |= REASON_ORIENTATION;
	}

	// Just return true if an update is not triggered.
	if ((d_reason & (REASON_DISCONTINUITY | REASON_POSITION | REASON_VELOCITY |
			REASON_ORIENTATION_RATE | REASON_ORIENTATION)) == 0)
		return(true);

	// Return true if an update is prevented.
	if (((d_reason & REASON_DISCONTINUITY) == 0) &&
			((d_reason & NON_REASON_TOO_SOON) != 0))
// I was seeing repeated discontinuities when they occurred too soon. Debug.
//	if ((d_reason & NON_REASON_TOO_SOON) != 0)
	{
#ifdef DEBUG_SIX_DOF
		// Record times when an update should have been sent but was prevented.
		ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), true);
		OutputUpdateState(i_master, GetActor());
#endif
		return(true);
	}

#ifdef OUTPUT_WHEN_SENDING
	const char* instance = ((DOActor*)i_masterDO)->GetActor()->InstanceName();
	if ((d_reason & REASON_DISCONTINUITY) != 0)
		g_console.Echo("DRSend Discontinuity (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_POSITION) != 0)
		g_console.Echo("DRSend Position (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_VELOCITY) != 0)
		g_console.Echo("DRSend Velocity (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_ORIENTATION_RATE) != 0)
		g_console.Echo("DRSend Orientation Rate (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_ORIENTATION) != 0)
		g_console.Echo("DRSend Orientation (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
#endif

	// Save the send time and return false.
	d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::ComputeValue(DSSixDOF* o_deadReckoned, Time i_time)
{
	// Find the curent time and the time since the last update.
#ifdef LATENCY_COMPENSATION
	o_deadReckoned->d_time = g_sessionTime.ConvertTimeNetToHost(GetActor(),
			i_time);
	float deltaSec = o_deadReckoned->d_time - d_updateTime;
#else
	float deltaSec = 0.001f *
			(float)(g_sessionTime.ConvertTimeNetToSession(i_time) - d_updateTime);
#endif

	// Form the dead reckoned orientation.
	ts_Quatvw orient;
	Vector3 axis = d_axis;
	qatFromAxisAngle(&axis, Math::Rad2Deg(d_omega * deltaSec), &orient);
	qatMul(&orient, &d_orient, &orient);

	// Compute and save state.
	ASSERT_PTR(d_acc);
	g_world.CompressPosition(
			d_pos + (d_vel + (0.5f * deltaSec) * d_acc->GetVector()) * deltaSec,
			o_deadReckoned->d_posX, o_deadReckoned->d_posY, o_deadReckoned->d_posZ);
	VecToComp(d_vel + d_acc->GetVector() * deltaSec, MAX_VEL,
			o_deadReckoned->d_velX, o_deadReckoned->d_velY, o_deadReckoned->d_velZ);
	QuatToComp(orient, o_deadReckoned->d_orientX, o_deadReckoned->d_orientY, 
			o_deadReckoned->d_orientZ, o_deadReckoned->d_orientW);
	VecToComp(d_omega * d_axis, MAX_OMEGA, o_deadReckoned->d_omegaX, 
			o_deadReckoned->d_omegaY, o_deadReckoned->d_omegaZ);

	// Copy flags.
	o_deadReckoned->d_flags = (qByte)d_flags;

#ifdef DEBUG_SIX_DOF
	OutputDRState(*o_deadReckoned, GetActor());
#endif
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::UpdateModelOnMaster(const DSSixDOF &i_master,
		Time i_time)
{
#ifdef DEBUG_SIX_DOF
	ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), true);
#endif
	UpdateModel(i_master);
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::UpdateModelOnDuplica(const DSSixDOF &i_master)
{
	if (g_session.IsOpen())
	{
#ifdef DEBUG_SIX_DOF
		ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), false);
#endif
		UpdateModel(i_master);
	}
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::UpdateModel(const DSSixDOF &i_master)
{
#ifdef LATENCY_COMPENSATION
	d_updateTime = i_master.d_time;
#else
	d_updateTime = g_sessionTime.GetSessionTime();
#endif
	d_flags = (uint8)i_master.d_flags;
	g_world.DecompressPackedPosition(i_master.d_posX, i_master.d_posY,
			i_master.d_posZ, d_pos);
	CompToPackedVec(i_master.d_velX, i_master.d_velY, i_master.d_velZ, MAX_VEL,
			d_vel);
	CompToQuat(i_master.d_orientX, i_master.d_orientY, i_master.d_orientZ,
			i_master.d_orientW, d_orient);
	Vector3 omega;
	CompToVec(i_master.d_omegaX, i_master.d_omegaY, i_master.d_omegaZ, MAX_OMEGA,
			omega);
	d_omega = omega.Length();
	if (!Math::DivByZero(1.0f, d_omega))
		d_axis = (1.0f / d_omega) * omega;
	else
	{
		d_omega = 0.0f;
		d_axis.Set(0.0f, 1.0f, 0.0f);
	}

	// Set discontinuity flags. They will be cleared only on the duplica and
	// only after they have been dealt with.
	SetDiscontinuityBits(d_flags & DISCONTINUITY_ANY);

#ifdef DEBUG_SIX_DOF
	OutputUpdateState(i_master, GetActor());

	if (d_drawDebug)
	{
#if 1
		Vector3 pos(d_pos);
		pos.Y(pos.Y() + 1.5f);
		Vector3 dir(0.0f, 0.0f, 1.0f);
		qatVectMul(&d_orient, &dir, &dir);
		switch(d_reason)
		{
		 case REASON_NONE:
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::WHITE, 5.0f);
			break;
		 case REASON_DISCONTINUITY:
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::MAGENTA, 5.0f);
			break;
		 case REASON_POSITION:
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::RED, 5.0f);
			break;
		 case REASON_VELOCITY:
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::GREEN, 5.0f);
			break;
		 case REASON_ORIENTATION:
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::BLUE, 5.0f);
			break;
		 case REASON_ORIENTATION_RATE:
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::YELLOW, 5.0f);
			break;
		}
#else
		switch(d_reason)
		{
		 case REASON_NONE:
			DrawUtility::QueuePoint(d_pos, DrawUtility::WHITE, 0.1f, 5.0f);
			break;
		 case REASON_DISCONTINUITY:
			DrawUtility::QueuePoint(d_pos, DrawUtility::MAGENTA, 0.1f, 5.0f);
			break;
		 case REASON_POSITION:
			DrawUtility::QueuePoint(d_pos, DrawUtility::RED, 0.1f, 5.0f);
			break;
		 case REASON_VELOCITY:
			DrawUtility::QueuePoint(d_pos, DrawUtility::GREEN, 0.1f, 5.0f);
			break;
		 case REASON_ORIENTATION:
			DrawUtility::QueuePoint(d_pos, DrawUtility::BLUE, 0.1f, 5.0f);
			break;
		 case REASON_ORIENTATION_RATE:
			DrawUtility::QueuePoint(d_pos, DrawUtility::YELLOW, 0.1f, 5.0f);
			break;
		}
#endif
	}
#endif
}

#ifdef DEBUG_SIX_DOF
/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::ManageOutput(bool i_debug, bool i_master)
{
	if (i_debug)
	{
		if (i_master)
		{
			OpenOutput(d_actorFile, "Master", "Actor", "Mas", "Act", false);
			OpenOutput(d_drFile, "Master", "Dr", "Mas", "Dr", false);
			OpenOutput(d_updateFile, "Master", "Update", "Mas", "Up", true);
		}
		else
		{
			OpenOutput(d_actorFile, "Duplica", "Actor", "Dup", "Act", false);
			OpenOutput(d_drFile, "Duplica", "Dr", "Dup", "Dr", false);
			OpenOutput(d_updateFile, "Duplica", "Update", "Dup", "Up", false);
		}
	}
	else
	{
		d_actorFile.Reclose();
		d_drFile.Reclose();
		d_updateFile.Reclose();
	}
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::OpenOutput(CLogFile &i_file, const char* i_longRole,
		char* i_longFile, const char* i_shortRole, const char* i_shortFile,
		bool i_reason)
{
	if (i_file.IsClosed())
	{
		char name[20];
		sprintf(name, "%s%s.txt", i_longRole, i_longFile);
		const char* fileName = g_logUtil.Enabled() ?
				g_logUtil.MakeFullName(name) : name;
#ifdef LATENCY_COMPENSATION
		if (!i_file.Open(fileName, i_reason ? 26 : 24))
#else
		if (!i_file.Open(fileName, i_reason ? 22 : 20))
#endif
		{
			ASSERT(false);
			return;
		}

		int index = 0;
#ifdef LATENCY_COMPENSATION
		sprintf(name, "%s%sSess", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sGameSess", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sHostTime", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sHostSess", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sSlope", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sIntcp", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
#else
		sprintf(name, "%s%sTime", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sEnd", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
#endif
		sprintf(name, "%s%sPos", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sVel", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOrient", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOmega", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sPosX", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sPosY", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sPosZ", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sVelX", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sVelY", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sVelZ", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOrientX", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOrientY", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOrientZ", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOrientW", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOmegaX", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOmegaY", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sOmegaZ", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		if (i_reason)
		{
			i_file.SetField(index++, "Reason");
			i_file.SetField(index++, "Prevent");
		}
		i_file.Print();
	}
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFModel::OutputState(CLogFile &i_file, const DSSixDOF &i_data,
		DOActor* i_actor, uint8 i_reason) const
{
	if (i_file.IsOpen())
	{
		int index = 0;
#ifdef LATENCY_COMPENSATION
		i_file.SetField(index++,
				(float)g_sessionTime.GetSessionFrameEndTime() * 0.001f);
		i_file.SetField(index++, (float)g_sessionTime.ConvertTimeGameToSession(
				g_timer.GetEndSec()) * 0.001f);
		i_file.SetField(index++, i_data.d_time);
		i_file.SetField(index++, (float)g_sessionTime.ConvertTimeHostToSession(
				i_actor, i_data.d_time) * 0.001f);
		float hostTimePerSessionTime, hostVsSessionIntercept;
		g_sessionTime.GetTimeProperties(i_actor,
				hostTimePerSessionTime, hostVsSessionIntercept);
		i_file.SetField(index++, hostTimePerSessionTime);
		i_file.SetField(index++, hostVsSessionIntercept);
#else
		i_file.SetField(index++, (float)g_sessionTime.GetSessionTime() * 0.001f);
		i_file.SetField(index++,
				(float)g_sessionTime.GetSessionFrameEndTime() * 0.001f);
#endif
		if ((i_data.d_flags & DISCONTINUITY_POSITION) != 0)
			i_file.SetField(index, "*");
		index++;
		if ((i_data.d_flags & DISCONTINUITY_VELOCITY) != 0)
			i_file.SetField(index, "*");
		index++;
		if ((i_data.d_flags & DISCONTINUITY_ORIENTATION) != 0)
			i_file.SetField(index, "*");
		index++;
		if ((i_data.d_flags & DISCONTINUITY_ANGULAR_VELOCITY) != 0)
			i_file.SetField(index, "*");
		index++;
		i_file.SetField(index++, g_world.DecompressPositionX(i_data.d_posX));
		i_file.SetField(index++, g_world.DecompressPositionY(i_data.d_posY));
		i_file.SetField(index++, g_world.DecompressPositionZ(i_data.d_posZ));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_velX, MAX_VEL));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_velY, MAX_VEL));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_velZ, MAX_VEL));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_orientX, 1.0f));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_orientY, 1.0f));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_orientZ, 1.0f));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_orientW, 1.0f));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_omegaX, MAX_OMEGA));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_omegaY, MAX_OMEGA));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_omegaZ, MAX_OMEGA));

		char text[33];
		text[0] = '\0';
		if ((i_reason & REASON_DISCONTINUITY) != 0)
			strcat(text, " | DIS");
		if ((i_reason & REASON_POSITION) != 0)
			strcat(text, " | POS");
		if ((i_reason & REASON_VELOCITY) != 0)
			strcat(text, " | VEL");
		if ((i_reason & REASON_ORIENTATION) != 0)
			strcat(text, " | ORIENT");
		if ((i_reason & REASON_ORIENTATION_RATE) != 0)
			strcat(text, " | RATE");
		if (text[0] != '\0')
			i_file.SetField(index, text + 3);
		index++;

		text[0] = '\0';
		if ((i_reason & NON_REASON_TOO_SOON) != 0)
			strcat(text, " | SOON");
		if (text[0] != '\0')
			i_file.SetField(index, text + 3);
		index++;

		i_file.Print();
	}
}
#endif

////////////////////////////// DRSixDOFComponent ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
DRSixDOFComponent::DRSixDOFComponent(CCompActor &i_actor,
		DuplicatedObject &i_do, DSSixDOF &i_ds) :
	CActorComponent(i_actor),
	d_do(i_do),
	d_ds(i_ds)
{
	d_param = CParam<DRSixDOFParam>::GetParam("DRSixDOF", "var");
	ASSERT_PTR(d_param);
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFComponent::BeginFrame(void)
{
	d_currTime = g_timer.GetStartSec();
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFComponent::BeginUpdate(void)
{
	d_beginTime = d_currTime;
	d_beginBodyInWorld = actor.GetBodyInWorld();
	d_beginVelocityInWorld = actor.GetVelocityInWorld();
	Graphics4x4 orient;
	orient.Set(actor.GetBodyToWorld());
	qatFromMat(&orient, &d_beginOrient);
	d_beginOmegaInWorld = actor.GetAngVelocityInBody() * actor.GetBodyToWorld();
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFComponent::AttemptUpdate(float i_deltaSec)
{
	// NOTE: Still need to provide for collisions.

	// Move dead reckoned state to the target time.
	d_currTime = d_beginTime + i_deltaSec;
	d_do.Refresh(d_ds, g_sessionTime.ConvertTimeGameToNet(d_currTime));

	// Get the active discontinuity flags.
	DRSixDOFModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);
	uint8 flags = model->GetDiscontinuityFlags();

	// Extract target state.
	ts_Quatvw targetOrient, beginInv;
	Vector3 targetPos, targetVel;
	g_world.DecompressPosition(d_ds.d_posX, d_ds.d_posY, d_ds.d_posZ, targetPos);
	CompToVec(d_ds.d_velX, d_ds.d_velY, d_ds.d_velZ, MAX_VEL, targetVel);
#ifdef LATENCY_COMPENSATION
	g_sessionTime.ConvertRateHostToGame(model->GetActor(), targetVel, targetVel);
#else
	g_sessionTime.ConvertRateSessionSecToGame(targetVel, targetVel);
#endif
	CompToQuat(d_ds.d_orientX, d_ds.d_orientY, d_ds.d_orientZ, d_ds.d_orientW,
			targetOrient);
	beginInv.V = d_beginOrient.V.Negated();
	beginInv.W = d_beginOrient.W;
	qatMul(&targetOrient, &beginInv, &targetOrient);
	Vector3 targetOmega;
	CompToVec(d_ds.d_omegaX, d_ds.d_omegaY, d_ds.d_omegaZ, MAX_OMEGA,
			targetOmega);
#ifdef LATENCY_COMPENSATION
	g_sessionTime.ConvertRateHostToGame(model->GetActor(), targetOmega,
			targetOmega);
#else
	g_sessionTime.ConvertRateSessionSecToGame(targetOmega, targetOmega);
#endif

	// If position is discontinuous, don't filter position or velocity
	if ((flags & DISCONTINUITY_POSITION) != 0)
	{
		actor.GetBodyInWorld() = targetPos;
		actor.GetVelocityInWorld() = targetVel;
	}

	// Filter position and velocity using second order implicit integration.
	else
	{
		float timeTerm = 1.0f + d_param->d_translationDamping * i_deltaSec +
				d_param->d_translationStiffness * i_deltaSec * i_deltaSec;
		if (!Math::DivByZero(1.0f, timeTerm))
		{
			// If just velocity is discontinuous, behave as if the actor's velocity
			// was at the target velocity when the last update was received.
			Vector3 beginVelocity;
			if ((flags & DISCONTINUITY_VELOCITY) != 0)
				beginVelocity = targetVel;
			else
				beginVelocity = d_beginVelocityInWorld;
			actor.GetVelocityInWorld() = (1.0f / timeTerm) * (beginVelocity +
					(d_param->d_translationDamping * targetVel +
					d_param->d_translationStiffness * (targetPos -
					d_beginBodyInWorld)) * i_deltaSec);
			actor.GetBodyInWorld() = d_beginBodyInWorld +
					actor.GetVelocityInWorld() * i_deltaSec;

			// If within range, clamp.
			if ((actor.GetBodyInWorld() - targetPos).LengthSquared() <
					d_param->d_positionClampSq)
			{
				actor.GetBodyInWorld() = targetPos;
				actor.GetVelocityInWorld() = targetVel;
			}
		}
		else
		{
			actor.GetVelocityInWorld() = d_beginVelocityInWorld;
			actor.GetBodyInWorld() = d_beginBodyInWorld;
		}
	}

	// If orientation is discontinuous, don't filter orientation or angular
	// velocity.
	if ((flags & DISCONTINUITY_ORIENTATION) != 0)
	{
		Cartesian4x4 bodyToWorld;
		qatMul(&targetOrient, &d_beginOrient, &targetOrient);
		qatToRotMat(&targetOrient, &bodyToWorld);
		actor.GetBodyToWorld() = bodyToWorld;
		actor.GetAngVelocityInBody() = targetOmega / actor.GetBodyToWorld();
	}

	// Orientation is continuous.
	else
	{
		// Filter orientation using first order implicit integration.
		ts_Quatvw orient;
		float mag = targetOrient.V.Length();
		if (Math::DivByZero(1.0f, mag))
			qatIdentity(&orient);
		else
		{
			float timeTerm = d_param->d_secOrientationTimeConstant + i_deltaSec;
			Vector3 axis = (1.0f / mag) * targetOrient.V;
			float angleTerm;
			if (targetOrient.W >= 0.0f)
				angleTerm = 2.0f * Math::ArcTan2(mag, targetOrient.W) * i_deltaSec;
			else
				angleTerm = 2.0f * Math::ArcTan2(-mag, -targetOrient.W) * i_deltaSec;
			if (Math::DivByZero(angleTerm, timeTerm))
				qatIdentity(&orient);
			else
				qatFromAxisAngle(&axis, Math::Rad2Deg(angleTerm / timeTerm),
						&orient);
		}

		// Don't filter angular velocity if discontinuous.
		Vector3 omega;
		if ((flags & DISCONTINUITY_ANGULAR_VELOCITY) != 0)
			omega = targetOmega;

		// Filter omega using first order implicit integration.
		else
		{
			float timeTerm = d_param->d_secOrientationRateTimeConstant + i_deltaSec;
			if (!Math::DivByZero(1.0f, timeTerm))
			{
				omega = (1.0f / timeTerm) * (d_beginOmegaInWorld *
						d_param->d_secOrientationRateTimeConstant +
						targetOmega * i_deltaSec);
			}
			else
				omega = d_beginOmegaInWorld;
		}

		// If within range, clamp.
		ts_Quatvw deltaOrient;
		deltaOrient.V = orient.V.Negated();
		deltaOrient.W = orient.W;
		qatMul(&targetOrient, &deltaOrient, &deltaOrient);
		if (deltaOrient.W > d_param->d_orientationClampCosHalfAngle)
		{
			orient.V = targetOrient.V;
			orient.W = targetOrient.W;
			omega = targetOmega;
		}

		qatMul(&orient, &d_beginOrient, &orient);
		Cartesian4x4 bodyToWorld;
		qatToRotMat(&orient, &bodyToWorld);
		actor.GetBodyToWorld() = bodyToWorld;
		actor.GetAngVelocityInBody() = omega / actor.GetBodyToWorld();
	}

	actor.SyncState();

#ifdef DEBUG_SIX_DOF
	DSSixDOF data;
	Push(&actor, data, d_currTime);
	data.d_flags = d_ds.d_flags;
	model->OutputActorState(data, model->GetActor());
#endif
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFComponent::EndUpdate(float i_deltaSec)
{
	DRSixDOFModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);
	model->ClearDiscontinuityFlags();
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFComponent::Pull(void)
{
	ts_Quatvw quat;
	Cartesian4x4 orient;
	CompToQuat(d_ds.d_orientX, d_ds.d_orientY, d_ds.d_orientZ, d_ds.d_orientW,
			quat);
	qatToRotMat(&quat, &orient);

	DRSixDOFModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);

	Vector3 vel, omega;
	CompToVec(d_ds.d_omegaX, d_ds.d_omegaY, d_ds.d_omegaZ, MAX_OMEGA, omega);
#ifdef LATENCY_COMPENSATION
	g_sessionTime.ConvertRateHostToGame(model->GetActor(), omega, omega);
#else
	g_sessionTime.ConvertRateSessionSecToGame(omega, omega);
#endif

	g_world.DecompressPosition(d_ds.d_posX, d_ds.d_posY, d_ds.d_posZ,
			actor.GetBodyInWorld());
	CompToVec(d_ds.d_velX, d_ds.d_velY, d_ds.d_velZ, MAX_VEL, vel);
#ifdef LATENCY_COMPENSATION
	g_sessionTime.ConvertRateHostToGame(model->GetActor(), vel,
			actor.GetVelocityInWorld());
#else
	g_sessionTime.ConvertRateSessionSecToGame(vel, actor.GetVelocityInWorld());
#endif
	actor.GetBodyToWorld() = orient;
	actor.GetAngVelocityInBody() = omega / actor.GetBodyToWorld();

	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void DRSixDOFComponent::Push(CActor* i_actor, DSSixDOF &i_ds, float i_time,
		bool i_debug)
{
	ts_Quatvw quat;
	Graphics4x4 orient;
	orient.Set(i_actor->GetBodyToWorld());
	qatFromMat(&orient, &quat);

#ifdef LATENCY_COMPENSATION
	i_ds.d_time = i_time;
#endif
	i_ds.d_flags = (qByte)(i_actor->GetDiscontinuity() & DISCONTINUITY_ANY);
#ifdef DEBUG_SIX_DOF
	if (i_debug)
		i_ds.d_flags |= (qByte)FLAG_DEBUG;
#endif
	g_world.CompressPosition(i_actor->GetBodyInWorld(), i_ds.d_posX, i_ds.d_posY,
			i_ds.d_posZ);
#ifdef LATENCY_COMPENSATION
	VecToComp(i_actor->GetVelocityInWorld(), MAX_VEL, i_ds.d_velX, i_ds.d_velY,
			i_ds.d_velZ);
#else
	Vector3 sessionVelocity;
	g_sessionTime.ConvertRateGameToSessionSec(i_actor->GetVelocityInWorld(),
			sessionVelocity);
	VecToComp(sessionVelocity, MAX_VEL, i_ds.d_velX, i_ds.d_velY, i_ds.d_velZ);
#endif
	QuatToComp(quat, i_ds.d_orientX, i_ds.d_orientY, i_ds.d_orientZ,
			i_ds.d_orientW);
#ifdef LATENCY_COMPENSATION
	VecToComp(i_actor->GetAngVelocityInBody() * i_actor->GetBodyToWorld(),
			MAX_OMEGA, i_ds.d_omegaX, i_ds.d_omegaY, i_ds.d_omegaZ);
#else
	g_sessionTime.ConvertRateGameToSessionSec(i_actor->GetAngVelocityInBody() *
			i_actor->GetBodyToWorld(), sessionVelocity);
	VecToComp(sessionVelocity, MAX_OMEGA, i_ds.d_omegaX, i_ds.d_omegaY,
			i_ds.d_omegaZ);
#endif
}
