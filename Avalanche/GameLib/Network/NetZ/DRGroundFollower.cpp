/*
**
**  File:   DRGroundFollower.cpp
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
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRGroundFollower.cpp $
**  $Revision: #45 $
**  $DateTime: 2005/08/25 17:13:59 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DRGroundFollower.h"

// Local includes
#include "Network/Session.h"
#include "Network/NetZ/SessionTime.h"
#include "Network/NetZ/DSGroundFollower.h"

// Gamelib includes
#include "GameHelper/ParamMgr.h"
#include "Components/Component.h"
#include "EngineHelper/timer.h"
#include "stage/CCompActor.h"
#include "Environ/World.h"
#include "Interfaces/GroundHeightIF.h"
#include "collide/collsys.h"
#include "collide/CapsuleToObb.h"
#include "collide/CapsuleToTerrain.h"
#include "camera/director.h"
#include "camera/camera.h"

#ifdef DEBUG_GROUND_FOLLOWER
#include "EngineHelper/drawutil.h"
#endif

//#define OUTPUT_WHEN_SENDING
#ifdef OUTPUT_WHEN_SENDING
#include "Network/NetZ/DOActor.h"
#endif

// Engine headers
#include "Math/MathClass.h"

#define DR_COLLISIONS

// Record why an update is sent.
#define REASON_NONE           0x00
#define REASON_DISCONTINUITY  0x01
#define REASON_HEADING        0x02
#define REASON_HEADING_RATE   0x04
#define REASON_POSITION       0x08
#define REASON_VELOCITY       0x10
#define NON_REASON_TOO_SOON   0x20

// Discontinuity flags.
#define DISCONTINUITY_ANY (DISCONTINUITY_POSITION | DISCONTINUITY_VELOCITY | \
			DISCONTINUITY_ORIENTATION | DISCONTINUITY_ANGULAR_VELOCITY)
#define FLAG_ON_GROUND  0x10

// Debugging
#ifdef DEBUG_GROUND_FOLLOWER
#define FLAG_DEBUG       0x80
#define CONTINUE_TEST(t) true
#else
#define FLAG_DEBUG       0x00
#define CONTINUE_TEST(t) ((t) == 0)
#endif

// Compress/decompress for network transmission.
#define MAX_VEL                100.0f
#define MAX_HEADING_RATE       10.0f
#define MAX_HEADING            4.0f
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

//////////////////////////// DRGroundFollowerParam /////////////////////////////

/******************************************************************************/
/******************************************************************************/
enum
{
	PARAM_CHANGE_POSITION_THRESHOLD,
	PARAM_CHANGE_VELOCITY_THRESHOLD,
	PARAM_CHANGE_HEADING_THRESHOLD,
	PARAM_CHANGE_HEADING_RATE_THRESHOLD,
	PARAM_CHANGE_TRANSLATION_FILTER,
	PARAM_CHANGE_POSITION_CLAMP,
	PARAM_CHANGE_HEADING_CLAMP
};

/******************************************************************************/
/******************************************************************************/
void CParam<DRGroundFollowerParam>::RegisterVariables(void)
{
	RegisterVariable(d_data.d_positionThreshold, "PositionMeters",
			0.15f, 0.0f, 1.0f, (void*)PARAM_CHANGE_POSITION_THRESHOLD);
	RegisterVariable(d_data.d_velocityThreshold,
			"VelocityMetersPerSec", 1.7f, 0.0f, 5.0f,
			(void*)PARAM_CHANGE_VELOCITY_THRESHOLD);
	RegisterVariable(d_data.d_degHeadingThreshold, "HeadingDeg",
			50.0f, 0.0f, 60.0f, (void*)PARAM_CHANGE_HEADING_THRESHOLD);
	RegisterVariable(d_data.d_degHeadingRateThreshold,
			"HeadingRateDegPerSec", 35.0f, 0.0f, 180.0f,
			(void*)PARAM_CHANGE_HEADING_RATE_THRESHOLD);
	RegisterVariable(d_data.d_gravity, "GravityMetersPerSecSq",
			22.0f, 0.0f, 50.0f);
	RegisterVariable(d_data.d_hzTranslationNaturalFrequency,
			"TranslationNaturalFrequencyHz", 2.0f, 0.0f, 10.0f,
			(void*)PARAM_CHANGE_TRANSLATION_FILTER);
	RegisterVariable(d_data.d_translationDampingFactor,
			"TranslationDampingFactor", 0.9f, 0.0f, 2.0f,
			(void*)PARAM_CHANGE_TRANSLATION_FILTER);
	RegisterVariable(d_data.d_secHeadingTimeConstant, "HeadingTimeConstantSec",
			0.4f, 0.0f, 2.0f);
	RegisterVariable(d_data.d_secHeadingRateTimeConstant,
			"HeadingRateTimeConstantSec", 0.4f, 0.0f, 2.0f);
	RegisterVariable(d_data.d_positionClamp, "PositionClampMeters",
			0.03f, 0.0f, 0.5f, (void*)PARAM_CHANGE_POSITION_CLAMP);
	RegisterVariable(d_data.d_degHeadingClamp, "HeadingClampDeg",
			3.0f, 0.0f, 20.0f, (void*)PARAM_CHANGE_HEADING_CLAMP);

	d_data.d_positionThresholdSq =
			d_data.d_positionThreshold * d_data.d_positionThreshold;
	d_data.d_velocityThresholdSq = d_data.d_velocityThreshold *
			d_data.d_velocityThreshold;
	d_data.d_radHeadingThreshold = Math::Deg2Rad(d_data.d_degHeadingThreshold);
	d_data.d_radHeadingRateThreshold =
			Math::Deg2Rad(d_data.d_degHeadingRateThreshold);
	float naturalFrequency = Math::TwoPi * d_data.d_hzTranslationNaturalFrequency;
	d_data.d_translationStiffness = naturalFrequency * naturalFrequency;
	d_data.d_translationDamping = 2.0f * d_data.d_translationDampingFactor *
			naturalFrequency;
	d_data.d_positionClampSq =
			d_data.d_positionClamp * d_data.d_positionClamp;
	d_data.d_radHeadingClamp = Math::Deg2Rad(d_data.d_degHeadingClamp);
}

/******************************************************************************/
/******************************************************************************/
void CParam<DRGroundFollowerParam>::CParamMessageHandler::HandleMessage(
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
	 case PARAM_CHANGE_HEADING_THRESHOLD:
		d_data->d_radHeadingThreshold =
				Math::Deg2Rad(d_data->d_degHeadingThreshold);
		break;
	 case PARAM_CHANGE_HEADING_RATE_THRESHOLD:
		d_data->d_radHeadingRateThreshold =
				Math::Deg2Rad(d_data->d_degHeadingRateThreshold);
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
	 case PARAM_CHANGE_HEADING_CLAMP:
		d_data->d_radHeadingClamp = Math::Deg2Rad(d_data->d_degHeadingClamp);
		break;
	}
}

//////////////////////////// DRGroundFollowerModel /////////////////////////////

/******************************************************************************/
/******************************************************************************/
DRGroundFollowerModel::DRGroundFollowerModel()
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
	d_heading = 0.0f;
	d_headingRate = 0.0f;
	d_lastTimeChecked = 0.0f;

	d_param = CParam<DRGroundFollowerParam>::GetParam("DRGroundFollower", "var");
	ASSERT_PTR(d_param);

#ifdef DEBUG_GROUND_FOLLOWER
	d_drawDebug = false;
	d_drawDebugHandler.Initialize("ToggleDrawDeadReckoning",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
#endif
}

#ifdef DEBUG_GROUND_FOLLOWER
/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerModel::CDrawDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_drawDebug = !d_ref->d_drawDebug;
}
#endif

/******************************************************************************/
/******************************************************************************/
qBool DRGroundFollowerModel::IsAccurateEnough(const DSGroundFollower &i_master,
		Time i_time, DuplicatedObject* i_masterDO, Station* i_duplicaStation)
{
	// Make sure this model is being used.
	if (!i_master.GetIsActive())
		return(true);

#ifdef DEBUG_GROUND_FOLLOWER
	OutputActorState(i_master, GetActor());
#endif

	// Clear the reason to send.
	d_reason = REASON_NONE;

	// Always send on a discontinuity or ground state change.
	if (((i_master.d_flags & DISCONTINUITY_ANY) != 0) ||
			((d_flags & (FLAG_ON_GROUND | FLAG_DEBUG)) !=
			(i_master.d_flags & (FLAG_ON_GROUND | FLAG_DEBUG))))
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

	// Check heading threshold.
	DSGroundFollower deadReckoned;
	if (CONTINUE_TEST(d_reason))
	{
		ComputeValue(&deadReckoned, i_time);
		if (Math::Abs(Math::NormalAngle(
				Math::SignedDecompress(i_master.d_heading, MAX_HEADING) -
				(Math::SignedDecompress(deadReckoned.d_heading, MAX_HEADING)))) >
				d_param->d_radHeadingThreshold)
			d_reason |= REASON_HEADING;
	}

	// Check heading rate threshold.
	if (CONTINUE_TEST(d_reason))
	{
		float val =
				Math::SignedDecompress(i_master.d_headingRate, MAX_HEADING_RATE) -
				Math::SignedDecompress(deadReckoned.d_headingRate, MAX_HEADING_RATE);
		if (Math::Abs(val) > d_param->d_radHeadingRateThreshold)
			d_reason |= REASON_HEADING_RATE;
	}

	// Check position threshold.
	if (CONTINUE_TEST(d_reason))
	{
		Vector3 masterPos, drPos;
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
			d_reason |= REASON_POSITION;
	}

	// Just return true if an update is not triggered.
	if ((d_reason & (REASON_DISCONTINUITY | REASON_HEADING |
			REASON_HEADING_RATE | REASON_POSITION | REASON_VELOCITY)) == 0)
		return(true);

	// Return true if an update is prevented.
	if (((d_reason & REASON_DISCONTINUITY) == 0) &&
			((d_reason & NON_REASON_TOO_SOON) != 0))
// I was seeing repeated discontinuities when they occurred too soon. Debug.
//	if ((d_reason & NON_REASON_TOO_SOON) != 0)
	{
#ifdef DEBUG_GROUND_FOLLOWER
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
	else if ((d_reason & REASON_HEADING) != 0)
		g_console.Echo("DRSend Heading (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_HEADING_RATE) != 0)
		g_console.Echo("DRSend Heading Rate (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_POSITION) != 0)
		g_console.Echo("DRSend Position (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
	else if ((d_reason & REASON_VELOCITY) != 0)
		g_console.Echo("DRSend Velocity (%s)-> time=%d\n", instance,
				g_sessionTime.GetSessionTime());
#endif

	// Save the send time and return false.
	d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
	return(false);
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerModel::ComputeValue(DSGroundFollower* o_deadReckoned,
		Time i_time)
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

	// Vertical.
	if ((d_flags & FLAG_ON_GROUND) != 0)
	{
		o_deadReckoned->d_posY = g_world.CompressPositionY(d_pos.Y() +
				d_vel.Y() * deltaSec);
		o_deadReckoned->d_velY = Math::SignedCompressToWord(d_vel.Y(), MAX_VEL);
	}
	else
	{
#ifdef _DEBUG
		float yPos = d_pos.Y() +
				(d_vel.Y() - 0.5f * d_param->d_gravity * deltaSec) * deltaSec;
		float yVel = d_vel.Y() - d_param->d_gravity * deltaSec;
		if (deltaSec > 2.0f)
		{
			yPos = g_world.ClampPositionY(yPos);
			yVel = Math::Clamp(yVel, -MAX_VEL, MAX_VEL);
		}
		o_deadReckoned->d_posY = g_world.CompressPositionY(yPos);
		o_deadReckoned->d_velY = Math::SignedCompressToWord(yVel, MAX_VEL);
#else
		o_deadReckoned->d_posY = g_world.CompressPositionY(d_pos.Y() +
				(d_vel.Y() - 0.5f * d_param->d_gravity * deltaSec) * deltaSec);
		o_deadReckoned->d_velY = Math::SignedCompressToWord(
				d_vel.Y() - d_param->d_gravity * deltaSec, MAX_VEL);
#endif
	}

	// No rotation.
	if (Math::DivByZero(1.0f, d_headingRate))
	{
		o_deadReckoned->d_posX = g_world.CompressPositionX(d_pos.X() +
				d_vel.X() * deltaSec);
		o_deadReckoned->d_posZ = g_world.CompressPositionZ(d_pos.Z() +
				d_vel.Z() * deltaSec);
		o_deadReckoned->d_velX = Math::SignedCompressToWord(d_vel.X(), MAX_VEL);
		o_deadReckoned->d_velZ = Math::SignedCompressToWord(d_vel.Z(), MAX_VEL);
		o_deadReckoned->d_heading = Math::SignedCompressToWord(d_heading,
				MAX_HEADING);
	}

	// Rotation.
	else
	{
		// Compute the angle of rotation and its sin and cos.
		float angle = d_headingRate * deltaSec;
		float sinAngle, cosAngle;
		Math::SinCos(angle, sinAngle, cosAngle);

		// Rotate the velocity vector.
		o_deadReckoned->d_velX = Math::SignedCompressToWord(
				d_vel.X() * cosAngle + d_vel.Z() * sinAngle, MAX_VEL);
		o_deadReckoned->d_velZ = Math::SignedCompressToWord(
				d_vel.Z() * cosAngle - d_vel.X() * sinAngle, MAX_VEL);

		// Move the position around a circle.
		float rateInv = 1.0f / d_headingRate;
		sinAngle *= rateInv;
		cosAngle = rateInv * (1.0f - cosAngle);
		o_deadReckoned->d_posX = g_world.CompressPositionX(
				d_pos.X() + d_vel.X() * sinAngle + d_vel.Z() * cosAngle);
		o_deadReckoned->d_posZ = g_world.CompressPositionZ(
				d_pos.Z() + d_vel.Z() * sinAngle - d_vel.X() * cosAngle);

		// Compute the heading.
		o_deadReckoned->d_heading = Math::SignedCompressToWord(
				Math::NormalAngle(d_heading + angle), MAX_HEADING);
	}

	// Constant heading rate.
	o_deadReckoned->d_headingRate = Math::SignedCompressToWord(d_headingRate,
			MAX_HEADING_RATE);

	// Copy flags.
	o_deadReckoned->d_flags = d_flags;

#ifdef DEBUG_GROUND_FOLLOWER
	OutputDRState(*o_deadReckoned, GetActor());
#endif
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerModel::UpdateModelOnMaster(
		const DSGroundFollower &i_master, Time i_time)
{
#ifdef DEBUG_GROUND_FOLLOWER
	ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), true);
#endif
	UpdateModel(i_master);
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerModel::UpdateModelOnDuplica(
		const DSGroundFollower &i_master)
{
	if (g_session.IsOpen())
	{
#ifdef DEBUG_GROUND_FOLLOWER
		ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), false);
#endif
		UpdateModel(i_master);
	}
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerModel::UpdateModel(const DSGroundFollower &i_master)
{
#ifdef LATENCY_COMPENSATION
	d_updateTime = i_master.d_time;
#else
	d_updateTime = g_sessionTime.GetSessionTime();
#endif
	d_flags = i_master.d_flags;
	g_world.DecompressPackedPosition(i_master.d_posX, i_master.d_posY,
			i_master.d_posZ, d_pos);
	CompToPackedVec(i_master.d_velX, i_master.d_velY, i_master.d_velZ, MAX_VEL,
			d_vel);
	d_heading = Math::SignedDecompress(i_master.d_heading, MAX_HEADING);
	d_headingRate = Math::SignedDecompress(i_master.d_headingRate,
			MAX_HEADING_RATE);

	//g_console.Echo("master heading rate = %f\n", d_headingRate);

	// Set discontinuity flags. They will be cleared only on the duplica and
	// only after they have been dealt with.
	SetDiscontinuityBits(d_flags & DISCONTINUITY_ANY);

#ifdef DEBUG_GROUND_FOLLOWER
	OutputUpdateState(i_master, GetActor());

	if (d_drawDebug)
	{
#if 0
		Vector3 pos(d_pos);
		pos.Y(pos.Y() + 1.5f);
		Vector3 dir(0.5f * Math::Sin(d_heading), 0.0f,
				0.5f * Math::Cos(d_heading));
		if (d_reason == REASON_NONE)
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::WHITE, 5.0f);
		else if ((d_reason & REASON_DISCONTINUITY) != 0)
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::MAGENTA, 5.0f);
		else if ((d_reason & REASON_HEADING) != 0)
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::BLUE, 5.0f);
		else if ((d_reason & REASON_HEADING_RATE) != 0)
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::YELLOW, 5.0f);
		else if ((d_reason & REASON_POSITION) != 0)
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::RED, 5.0f);
		else if ((d_reason & REASON_VELOCITY) != 0)
			DrawUtility::QueueLine(pos, pos + dir, DrawUtility::GREEN, 5.0f);

		pos += dir;
		DrawUtility::QueueLine(pos, pos + 250.0f * d_vel, DrawUtility::CYAN, 5.0f);
		dir.Set(0.0f, 250.0f * d_headingRate, 0.0f);
		DrawUtility::QueueLine(pos, pos + dir, DrawUtility::MAGENTA, 5.0f);
#else
		Vector3 pos(d_pos);
		pos.Y(pos.Y() + 1.5f);
		if (d_reason == REASON_NONE)
			DrawUtility::QueuePoint(pos, DrawUtility::WHITE, 0.1f, 5.0f);
		else if ((d_reason & REASON_DISCONTINUITY) != 0)
			DrawUtility::QueuePoint(pos, DrawUtility::MAGENTA, 0.1f, 5.0f);
		else if ((d_reason & REASON_HEADING) != 0)
			DrawUtility::QueuePoint(pos, DrawUtility::BLUE, 0.1f, 5.0f);
		else if ((d_reason & REASON_HEADING_RATE) != 0)
			DrawUtility::QueuePoint(pos, DrawUtility::YELLOW, 0.1f, 5.0f);
		else if ((d_reason & REASON_POSITION) != 0)
			DrawUtility::QueuePoint(pos, DrawUtility::RED, 0.1f, 5.0f);
		else if ((d_reason & REASON_VELOCITY) != 0)
			DrawUtility::QueuePoint(pos, DrawUtility::GREEN, 0.1f, 5.0f);
#endif
	}
#endif
}

#ifdef DEBUG_GROUND_FOLLOWER
/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerModel::ManageOutput(bool i_debug, bool i_master)
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
void DRGroundFollowerModel::OpenOutput(CLogFile &i_file, const char* i_longRole,
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
		if (!i_file.Open(fileName, i_reason ? 21 : 19))
#else
		if (!i_file.Open(fileName, i_reason ? 17 : 15))
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
		sprintf(name, "%s%sHead", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sRate", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sGnd", i_shortRole, i_shortFile);
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
		sprintf(name, "%s%sHead", i_shortRole, i_shortFile);
		i_file.SetField(index++, name);
		sprintf(name, "%s%sRate", i_shortRole, i_shortFile);
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
void DRGroundFollowerModel::OutputState(CLogFile &i_file,
		const DSGroundFollower &i_data, DOActor* i_actor, uint8 i_reason) const
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
		if ((i_data.d_flags & FLAG_ON_GROUND) != 0)
			i_file.SetField(index, "*");
		index++;
		i_file.SetField(index++, g_world.DecompressPositionX(i_data.d_posX));
		i_file.SetField(index++, g_world.DecompressPositionY(i_data.d_posY));
		i_file.SetField(index++, g_world.DecompressPositionZ(i_data.d_posZ));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_velX, MAX_VEL));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_velY, MAX_VEL));
		i_file.SetField(index++, Math::SignedDecompress(i_data.d_velZ, MAX_VEL));
		i_file.SetField(index++,
				Math::SignedDecompress(i_data.d_heading, MAX_HEADING));
		i_file.SetField(index++,
				Math::SignedDecompress(i_data.d_headingRate, MAX_HEADING_RATE));

		char text[33];
		text[0] = '\0';
		if ((i_reason & REASON_DISCONTINUITY) != 0)
			strcat(text, " | DIS");
		if ((i_reason & REASON_HEADING) != 0)
			strcat(text, " | HEAD");
		if ((i_reason & REASON_HEADING_RATE) != 0)
			strcat(text, " | RATE");
		if ((i_reason & REASON_POSITION) != 0)
			strcat(text, " | POS");
		if ((i_reason & REASON_VELOCITY) != 0)
			strcat(text, " | VEL");
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

////////////////////////// DRGroundFollowerComponent ///////////////////////////

/******************************************************************************/
/******************************************************************************/
DRGroundFollowerComponent::DRGroundFollowerComponent(CCompActor &i_actor,
		DuplicatedObject &i_do, DSGroundFollower &i_ds) :
	CActorComponent(i_actor),
	d_do(i_do),
	d_ds(i_ds)
{
	d_clampAtGround = false;
	d_param = CParam<DRGroundFollowerParam>::GetParam("DRGroundFollower", "var");
	ASSERT_PTR(d_param);
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerComponent::BeginFrame(void)
{
	d_currTime = g_timer.GetStartSec();
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerComponent::BeginUpdate(void)
{
	d_beginTime = d_currTime;
	d_beginBodyInWorld = actor.GetBodyInWorld();
	d_beginVelocityInWorld = actor.GetVelocityInWorld();
	d_beginHeading = Math::ArcTan2(actor.GetBodyToWorld().e20,
			actor.GetBodyToWorld().e22);
	d_beginHeadingRate = actor.GetAngVelocityInBody().Y();
	d_clampAtGround = false;
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerComponent::AttemptUpdate(float i_deltaSec)
{
	// NOTE: Still need to provide for collisions.

	// Move dead reckoned state to the target time.
	d_currTime = d_beginTime + i_deltaSec;
	d_do.Refresh(d_ds, g_sessionTime.ConvertTimeGameToNet(d_currTime));

	// Get the active discontinuity flags.
	uint8 flags;
	DRGroundFollowerModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);
	flags = model->GetDiscontinuityFlags();

	// Extract target state.
	Vector3 targetPos, targetVel;
	g_world.DecompressPosition(d_ds.d_posX, d_ds.d_posY, d_ds.d_posZ, targetPos);
	CompToVec(d_ds.d_velX, d_ds.d_velY, d_ds.d_velZ, MAX_VEL, targetVel);
#ifdef LATENCY_COMPENSATION
	g_sessionTime.ConvertRateHostToGame(model->GetActor(), targetVel, targetVel);
#else
	g_sessionTime.ConvertRateSessionSecToGame(targetVel, targetVel);
#endif
	float targetHeading = Math::SignedDecompress(d_ds.d_heading, MAX_HEADING);
#ifdef LATENCY_COMPENSATION
	float targetHeadingRate = g_sessionTime.ConvertRateHostToGame(
			model->GetActor(),
			Math::SignedDecompress(d_ds.d_headingRate, MAX_HEADING_RATE));
#else
	float targetHeadingRate = g_sessionTime.ConvertRateSessionSecToGame(
			Math::SignedDecompress(d_ds.d_headingRate, MAX_HEADING_RATE));
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
		actor.GetBodyToWorld().SetYRotation(targetHeading);
		actor.GetAngVelocityInBody().Set(0.0f, targetHeadingRate, 0.0f);
	}

	// Orientation is continuous.
	else
	{
		// Filter heading using first order implicit integration.
		float timeTerm = d_param->d_secHeadingTimeConstant + i_deltaSec;
		float heading;
		if (!Math::DivByZero(1.0f, timeTerm))
			heading = d_beginHeading + i_deltaSec *
					Math::NormalAngle(targetHeading - d_beginHeading) / timeTerm;
		else
			heading = d_beginHeading;

		// Don't filter heading rate if discontinuous.
		float headingRate;
		if ((flags & DISCONTINUITY_ANGULAR_VELOCITY) != 0)
			headingRate = targetHeadingRate;

		// Filter omega using first order implicit integration.
		else
		{
			timeTerm = d_param->d_secHeadingRateTimeConstant + i_deltaSec;
			if (!Math::DivByZero(1.0f, timeTerm))
				headingRate = (1.0f / timeTerm) *
						(d_beginHeadingRate * d_param->d_secHeadingRateTimeConstant +
						targetHeadingRate * i_deltaSec);
			else
				headingRate = d_beginHeadingRate;
		}

		// If within range, clamp.
		if (Math::Abs(Math::NormalAngle(targetHeading - heading)) <
				d_param->d_radHeadingClamp)
		{
			heading = targetHeading;
			headingRate = targetHeadingRate;
		}

		actor.GetBodyToWorld().SetYRotation(heading);
		actor.GetAngVelocityInBody().Set(0.0f, headingRate, 0.0f);
	}

#ifdef DR_COLLISIONS
	// Stop ground penetration (in a simple, crude way). Only test if there is
	// not a position discontinuity and the target is in the air or not too far
	// below the smoothed position. This is to prevent the smoothed actor from
	// resting on some surface above the master's head.
	if (((flags & DISCONTINUITY_POSITION) == 0) &&
			(((d_ds.d_flags & FLAG_ON_GROUND) == 0) ||
			((actor.GetBodyInWorld().Y() - targetPos.Y()) < 1.5f)))
	{
		// Need a ground height interface to perform the test.
		CGroundHeightIF* groundHeight = CGroundHeightIF::GetInterface(actor);
		if (groundHeight != NULL)
		{
			// See if near the camera or in its view.
			bool nearCamera = true;
			Camera &camera = g_cameraDirector.CurrentCamera();
			if ((camera.Eye() - actor.GetFocusInWorld()).LengthSquared() > 25.0f)
			{
				float w;
				Vector2 screen;
				nearCamera = camera.ToScreen(actor.GetFocusInWorld(), screen, w,
						Vector2(1.1f, 1.1f));
			}
			if (nearCamera)
			{
				float base = actor.GetBodyInWorld().Y();
				float top = d_beginBodyInWorld.Y() - base;
				if (top < 0.0f)
					top = 0.0f;
				top += actor.GetFocusInWorld().Y() - base;
				CGroundHeightIF::SCheckResults* results;

				// Extend test below actor so results can be used for things like
				// shadows.
				if (groundHeight->Get(top, -1.5f, results) &&
						(results->d_bottomHeight >= 0.0f))
				{
					actor.GetBodyInWorld().Y(results->d_intersection.Y());
					d_clampAtGround = true;
				}
			}
		}
	}
#endif

	actor.SyncState();

#ifdef DEBUG_GROUND_FOLLOWER
	DSGroundFollower data;
	Push(&actor, data, d_currTime);
	data.d_flags = d_ds.d_flags;
	model->OutputActorState(data, model->GetActor());
#endif
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerComponent::EndUpdate(float i_deltaSec)
{
	DRGroundFollowerModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);
	model->ClearDiscontinuityFlags();
}

/******************************************************************************/
/******************************************************************************/
uint DRGroundFollowerComponent::CollisionDetect(CStageEntity* i_entity,
		CCollisionEvent* i_event, bool i_mateDisplaced)
{
#ifdef DR_COLLISIONS
	// The actor's master manages the interactions categories so collision
	// responses are limited to the world here.
	CCollisionPair* pair = i_event->GetHeadPair();
	if ((pair != NULL) && i_entity->Category().PositiveMatch(WORLD_CATEGORY))
	{
		Vector3 normal;
		float depth = 0.0f;
		while(pair != NULL)
		{
			if (pair->GetIntersectionType() ==
					CCollisionIntersection::CAPSULE_TO_OBB)
			{
				CCapsuleToObbIntersection *c2o =
						(CCapsuleToObbIntersection*)pair->GetIntersection();
				c2o->GetResolutionNormal(normal, depth);
			}
			else if (pair->GetIntersectionType() ==
					CCollisionIntersection::CAPSULE_TO_TERRAIN)
			{
				CCapsuleToTerrainIntersection *c2t =
						(CCapsuleToTerrainIntersection*)pair->GetIntersection();
				normal = c2t->GetResolutionInfo(depth);
			}
			else
				return(CProtagonist::EFFECT_NONE);
			pair = pair->GetNext();
		}

		// If we're the target, flip the normal so it points toward us.
		if (i_event->GetTargetEntity() == (CStageEntity*)(&actor))
			normal.Negate();

		Vector3 displacement = depth * normal;
		if (!Math::Zero(displacement.LengthSquared()))
		{
			actor.GetBodyInWorld() += displacement;
			actor.SyncState();
			return(CProtagonist::EFFECT_DISPLACEMENT);
		}
	}
#endif
	return(CProtagonist::EFFECT_NONE);
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerComponent::Pull(void)
{
	Vector3 vel;

	DRGroundFollowerModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);

	g_world.DecompressPosition(d_ds.d_posX, d_ds.d_posY, d_ds.d_posZ,
			actor.GetBodyInWorld());
	CompToVec(d_ds.d_velX, d_ds.d_velY, d_ds.d_velZ, MAX_VEL, vel);
#ifdef LATENCY_COMPENSATION
	g_sessionTime.ConvertRateHostToGame(model->GetActor(), vel,
			actor.GetVelocityInWorld());
#else
	g_sessionTime.ConvertRateSessionSecToGame(vel, actor.GetVelocityInWorld());
#endif
	actor.GetBodyToWorld().SetYRotation(Math::SignedDecompress(d_ds.d_heading,
			MAX_HEADING));
#ifdef LATENCY_COMPENSATION
	actor.GetAngVelocityInBody().Set(0.0f,
			g_sessionTime.ConvertRateHostToGame(model->GetActor(),
			Math::SignedDecompress(d_ds.d_headingRate, MAX_HEADING_RATE)), 0.0f);
#else
	actor.GetAngVelocityInBody().Set(0.0f,
			g_sessionTime.ConvertRateSessionSecToGame(
			Math::SignedDecompress(d_ds.d_headingRate, MAX_HEADING_RATE)), 0.0f);
#endif

	actor.SyncState();
}

/******************************************************************************/
/******************************************************************************/
void DRGroundFollowerComponent::Push(CActor* i_actor, DSGroundFollower &i_ds,
		float i_time, bool i_onGround, bool i_debug)
{
#ifdef LATENCY_COMPENSATION
	i_ds.d_time = i_time;
#endif
	i_ds.d_flags = (qByte)(i_actor->GetDiscontinuity() & DISCONTINUITY_ANY);
	if (i_onGround)
		i_ds.d_flags |= (qByte)FLAG_ON_GROUND;
#ifdef DEBUG_GROUND_FOLLOWER
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
	i_ds.d_heading = Math::SignedCompressToWord(
			Math::ArcTan2(i_actor->GetBodyToWorld().e20,
			i_actor->GetBodyToWorld().e22), MAX_HEADING);
#ifdef LATENCY_COMPENSATION
	i_ds.d_headingRate = Math::SignedCompressToWord(
			i_actor->GetAngVelocityInBody().Y(), MAX_HEADING_RATE);
#else
	i_ds.d_headingRate = Math::SignedCompressToWord(
			g_sessionTime.ConvertRateGameToSessionSec(
			i_actor->GetAngVelocityInBody().Y()), MAX_HEADING_RATE);
#endif
}
