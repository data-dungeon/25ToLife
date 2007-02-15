/*
**
**  File:   DRPathFollower.cpp
**  Date:   October 15, 2004
**  By:     Bryce Thomsen
**  Desc:   Implementation of pPath follower dead reackoning. This model
**          assumes that the object is a basic ground follower, but it also 
**				has the pPath follower attributes that control its movement, 
**				orientation, etc.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Network/NetZ/DRPathFollower.cpp $
**  $Revision: #22 $
**  $DateTime: 2005/07/30 13:45:34 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DRPathFollower.h"

// Local includes
#include "Network/Session.h"
#include "Network/NetZ/SessionTime.h"
#include "Network/NetZ/DSPathFollower.h"

// Gamelib includes
#include "GameHelper/ParamMgr.h"
#include "Components/Component.h"
#include "EngineHelper/timer.h"
#include "stage/CCompActor.h"
#include "Interfaces/MoveIF.h"
#include "mathUtil/mathcons.h"

#ifdef DEBUG_PATH_FOLLOWER
#include "EngineHelper/drawutil.h"
#endif

#define OUTPUT_WHEN_SENDING
#ifdef OUTPUT_WHEN_SENDING
#include "cmdcon/cmdcon.h"
#endif

// Engine headers
#include "Math/MathClass.h"

#define PF_DISCONTINUITY_ANY (DISCONTINUITY_STATE | \
										DISCONTINUITY_SPEED_TARGET | \
										DISCONTINUITY_PATH_DISTANCE )

#ifdef DEBUG_PATH_FOLLOWER
#define FLAG_DEBUG      0x80
#endif

// --- local defines ---
#define MAX_DR_SERVICE_TIME (.08f)
#define MAX_SPEED (64.f)
#define MAX_DISTANCE (512.f)

//	--- Gravity ---
const float constGeesToAccel = 9.8f;	// meters/second^2

//////////////////////////// DRPathFollowerParam /////////////////////////////

/******************************************************************************/
/******************************************************************************/
enum
{
	PARAM_CHANGE_SPEED_TARGET_THRESHOLD,
	PARAM_CHANGE_SPEED_CURRENT_THRESHOLD,
	PARAM_CHANGE_DISTANCE_THRESHOLD,
};

/******************************************************************************/
/******************************************************************************/
void CParam<DRPathFollowerParam>::RegisterVariables(void)
{
	RegisterVariable(d_data.d_speedTargetThreshold, 
			"SpeedTargetMetersPerSec", 0.25f, 0.0f, 1.0f, 
			(void*)PARAM_CHANGE_SPEED_TARGET_THRESHOLD);
	RegisterVariable(d_data.d_speedCurrentThreshold, 
			"SpeedCurrentMetersPerSec", 0.2f, 0.0f, 1.0f, 
			(void*)PARAM_CHANGE_SPEED_CURRENT_THRESHOLD);
	RegisterVariable(d_data.d_distanceThreshold, "DistanceMeters",
			1.0f, 0.0f, 3.0f, (void*)PARAM_CHANGE_DISTANCE_THRESHOLD);
}

/******************************************************************************/
/******************************************************************************/
void CParam<DRPathFollowerParam>::CParamMessageHandler::HandleMessage(
		void* i_data, ActorHandle i_sender, bool i_posted)
{
	switch((uint)i_data)
	{
		case PARAM_CHANGE_SPEED_TARGET_THRESHOLD:
			break;
		case PARAM_CHANGE_SPEED_CURRENT_THRESHOLD:
			break;
		case PARAM_CHANGE_DISTANCE_THRESHOLD:
			break;
	}
}

//////////////////////////// DRPathFollowerModel /////////////////////////////

/******************************************************************************/
/******************************************************************************/
DRPathFollowerModel::DRPathFollowerModel(void)
{
#ifdef LATENCY_COMPENSATION
	d_updateTime = 0.0f;
#else
	d_updateTime = 0;
#endif
	d_lastTimeChecked = 0.0f;
	d_flags = 0;
	d_state = STATE_DEAD;
	d_speedTarget = 0.0f;
	d_speedCurrent = 0.0f;
	d_distance = 0.0f;

	d_param = CParam<DRPathFollowerParam>::GetParam("DRPathFollower", "var");
	ASSERT_PTR(d_param);

#ifdef DEBUG_PATH_FOLLOWER
	d_reason = REASON_NONE;
	d_drawDebug = false;
	d_drawDebugHandler.Initialize("ToggleDrawDeadReckoning",
			MESSAGE_HANDLE_IF_MASTER, d_messageOwner, this);
	d_actorFile = NULL;
	d_drFile = NULL;
	d_updateFile = NULL;
#endif
}

#ifdef DEBUG_PATH_FOLLOWER
/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::CDrawDebugHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	d_ref->d_drawDebug = !d_ref->d_drawDebug;
}
#endif

/******************************************************************************/
/******************************************************************************/
qBool DRPathFollowerModel::IsAccurateEnough(const DSPathFollower &i_master, 
		Time i_time, DuplicatedObject* i_masterDO, Station* i_duplicaStation)
{
	if (!i_master.IsActive())
		return (true);

#ifdef DEBUG_PATH_FOLLOWER
	OutputActorState(i_master, GetActor());
#endif

	// Keep the number of messages down during heavy discontinuity breaks.
	float curTime = g_timer.GetOSStartSec();
	if (d_lastTimeChecked  > curTime)
		return (true);

	// Always send on a discontinuity.
	if (i_master.d_flags != d_flags)
	{
#ifdef DEBUG_PATH_FOLLOWER
		d_reason = REASON_DISCONTINUITY;
#endif

#ifdef OUTPUT_WHEN_SENDING
		g_console.Echo( "DRPFSend Discontinuity-> time=%d\n", g_timer.GetOSTime() );
#endif

		d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
		return (false);
	}

	DSPathFollower deadReckoned;
	deadReckoned.SetPathFollowerObject(*i_master.GetPathFollowerObject());
	ComputeValue(&deadReckoned, i_time);

	if (i_master.d_state != deadReckoned.d_state)
	{
#ifdef DEBUG_PATH_FOLLOWER
		d_reason = REASON_STATE;
#endif

#ifdef OUTPUT_WHEN_SENDING
		g_console.Echo( "DRSend State-> time=%d\n", g_timer.GetOSTime() );
#endif

		d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
		return (false);
	}

	// Check target speed threshold. 
	// TODO: Ignore flee state until ComputeValue is updated.
	if (Math::Abs(	Math::SignedDecompress(i_master.d_speedTarget, MAX_SPEED) 
			- 	Math::SignedDecompress(deadReckoned.d_speedTarget, MAX_SPEED)) 
			> d_param->d_speedTargetThreshold && deadReckoned.d_state != STATE_FLEE)
	{
#ifdef DEBUG_PATH_FOLLOWER
		d_reason = REASON_SPEED_TARGET;
#endif

#ifdef OUTPUT_WHEN_SENDING
		g_console.Echo( "DRSend TargetSpeed-> time=%d\n", g_timer.GetOSTime() );
#endif

		d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
		return (false);
	}

//	// Check current speed threshold.
//	if (Math::Abs(	Math::SignedDecompress(i_master.d_speedCurrent, MAX_SPEED) 
//		- 	Math::SignedDecompress(deadReckoned.d_speedCurrent, MAX_SPEED)) 
//		> d_param->d_speedCurrentThreshold)
//	{
//#ifdef DEBUG_PATH_FOLLOWER
//		d_reason = REASON_SPEED_CURRENT;
//#endif
//
//#ifdef OUTPUT_WHEN_SENDING
//		g_console.Echo( "DRSend CurrentSpeed-> time=%d\n", g_timer.GetOSTime() );
//#endif
//
//		d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
//		return (false);
//	}

	// Check distance threshold.
	if (Math::Abs(	Math::SignedDecompress(i_master.d_distance, MAX_DISTANCE) 
			- 	Math::SignedDecompress(deadReckoned.d_distance, MAX_DISTANCE)) 
			> d_param->d_distanceThreshold)
	{
#ifdef DEBUG_PATH_FOLLOWER
		d_reason = REASON_DISTANCE;
#endif

#ifdef OUTPUT_WHEN_SENDING
		g_console.Echo( "DRSend Distance-> time=%d\n", g_timer.GetOSTime() );
#endif

		d_lastTimeChecked = curTime + MAX_DR_SERVICE_TIME;
		return (false);
	}

#ifdef DEBUG_PATH_FOLLOWER
	d_reason = REASON_NONE;
#endif
	return (true);
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::ComputeValue(DSPathFollower* o_deadReckoned, 
		Time i_time)
{
	if (o_deadReckoned->GetPathFollowerObject() == NULL)
	{
#ifdef LATENCY_COMPENSATION
		o_deadReckoned->d_time = 0.0f;
#endif
		o_deadReckoned->d_state = STATE_DEAD;
		o_deadReckoned->d_speedTarget = 0;
		o_deadReckoned->d_speedCurrent = 0;
		o_deadReckoned->d_distance = 0;
	}
	else
	{
		// Find the current time and the time since the last update.
#ifdef LATENCY_COMPENSATION
		o_deadReckoned->d_time = g_sessionTime.ConvertTimeNetToHost(GetActor(),
				i_time);
		float deltaSec = o_deadReckoned->d_time - d_updateTime;
#else
		float deltaSec = (float)(g_sessionTime.ConvertTimeNetToSession(i_time) -
				d_updateTime) * 0.001f;
#endif

		CPathFollower* pPF = o_deadReckoned->GetPathFollowerObject();

		// Extract current values
		float newSpeedTarget = d_speedTarget;
		float newSpeedCurrent = d_speedCurrent;
		float newDistance = d_distance;

		// Has the actor state changed?
		if (d_state != d_prevState)
		{
			// Exit the old state
			switch (d_prevState)
			{
				case STATE_DO_JOB:
				case STATE_FLEE:
					newSpeedTarget = 0.0f;
					newSpeedCurrent = DRPathFollowerUtility::AdjustSpeed(pPF, 
							newSpeedTarget, newSpeedCurrent, newDistance, deltaSec);
					break;
				case STATE_WATCH:
				case STATE_COWER:
				case STATE_FIGHT:
				case STATE_HOSTAGE:
				case STATE_SITTING:
				case STATE_STUNNED:
				case STATE_ON_FIRE:
				case STATE_VAULTING:
				case STATE_DEAD:
				default:
					break;
			}

			// Enter the new state
			switch (d_state)
			{
				case STATE_DO_JOB:
					if(pPF->IsEvacuator())
					{
						newSpeedTarget = 0.0f;
						newSpeedCurrent = DRPathFollowerUtility::AdjustSpeed(pPF, 
								newSpeedTarget, newSpeedCurrent, newDistance, deltaSec);
					}
					else
					{
						if (newSpeedCurrent < 0.0f)
							newSpeedTarget = pPF->GetOrgSpeedTarget() * -1.0f;
						else
							newSpeedTarget = pPF->GetOrgSpeedTarget();
					}
					break;
				case STATE_FLEE:
					newSpeedTarget = 6.0f;
					break;
				case STATE_WATCH:
				case STATE_COWER:
				case STATE_FIGHT:
				case STATE_HOSTAGE:
				case STATE_SITTING:
				case STATE_STUNNED:
				case STATE_ON_FIRE:
				case STATE_VAULTING:
				case STATE_DEAD:
					newSpeedCurrent = 0.0f;
					newSpeedTarget = 0.0f;
				default:
					break;
			}

			d_prevState = d_state;
		}

		// Handle running state
		switch (d_state)
		{
			case STATE_FLEE:
				newSpeedTarget = 6.0f;
				// TODO: handle reaching end of path and reversing
			case STATE_DO_JOB:
				// Actor needs to speed up
				if (d_speedTarget != d_speedCurrent)
				{
					// Proceed along the path until we reach our target speed
					float extraSec = 0.f;
					DRPathFollowerUtility::ProceedAlongPartialPath(
							o_deadReckoned->GetPathFollowerObject(), newSpeedTarget, 
							newSpeedCurrent, newDistance, deltaSec, extraSec);

					// Proceed along the path at target speed with the time we have leftover
					if (!Math::Zero(extraSec))
						DRPathFollowerUtility::ProceedAlongPath(
								o_deadReckoned->GetPathFollowerObject(), newSpeedTarget, 
								newSpeedCurrent, newDistance, deltaSec);
				}
				else
				{
					DRPathFollowerUtility::ProceedAlongPath(
							o_deadReckoned->GetPathFollowerObject(), newSpeedTarget, 
							newSpeedCurrent, newDistance, deltaSec);
				}
				break;
			case STATE_WATCH:
			case STATE_COWER:
			case STATE_FIGHT:
			case STATE_HOSTAGE:
			case STATE_SITTING:
			case STATE_STUNNED:
			case STATE_ON_FIRE:
			case STATE_VAULTING:
			case STATE_DEAD:
			default:
				break;
		}

		o_deadReckoned->d_state = d_state;
		o_deadReckoned->d_speedTarget = Math::SignedCompressToWord(newSpeedTarget, MAX_SPEED);
		o_deadReckoned->d_speedCurrent = Math::SignedCompressToWord(newSpeedCurrent, MAX_SPEED);
		o_deadReckoned->d_distance = Math::SignedCompressToWord(newDistance, MAX_DISTANCE);
	}

	// Copy flags.
	o_deadReckoned->d_flags = d_flags;

#ifdef DEBUG_PATH_FOLLOWER
	OutputDRState(*o_deadReckoned, GetActor());
#endif
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::UpdateModelOnMaster(const DSPathFollower &i_master, Time i_time)
{
#ifdef DEBUG_PATH_FOLLOWER
	ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), true);
#endif
	UpdateModel(i_master);
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::UpdateModelOnDuplica(const DSPathFollower &i_master)
{
	if (g_session.IsOpen())
	{
#ifdef DEBUG_PATH_FOLLOWER
		ManageOutput(((i_master.d_flags & FLAG_DEBUG) != 0), false);
#endif
		UpdateModel(i_master);
	}
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::UpdateModel(const DSPathFollower &i_master)
{
#ifdef LATENCY_COMPENSATION
	d_updateTime = i_master.d_time;
#else
	d_updateTime = g_sessionTime.GetSessionTime();
#endif
	d_flags = i_master.d_flags;
	d_prevState = d_state;
	d_state = i_master.d_state;
	d_speedTarget = 	Math::SignedDecompress(i_master.d_speedTarget, MAX_SPEED);
	d_speedCurrent = 	Math::SignedDecompress(i_master.d_speedCurrent, MAX_SPEED);
	d_distance = 	Math::SignedDecompress(i_master.d_distance, MAX_DISTANCE);

	// Set discontinuity flags. They will be cleared only on the duplica and
	// only after they have been dealt with.
	SetDiscontinuityBits(d_flags & PF_DISCONTINUITY_ANY);

#ifdef DEBUG_PATH_FOLLOWER
	OutputUpdateState(i_master, GetActor());
#endif
}

#ifdef DEBUG_PATH_FOLLOWER
/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::ManageOutput(bool i_debug, bool i_master)
{
	if (i_debug)
	{
		if (d_actorFile == NULL)
		{
			if (i_master)
			{
				d_actorFile = OpenOutput("Master", "Actor", "Mas", "Act", false);
				d_drFile = OpenOutput("Master", "Dr", "Mas", "Dr", false);
				d_updateFile = OpenOutput("Master", "Update", "Mas", "Up", true);
			}
			else
			{
				d_actorFile = OpenOutput("Duplica", "Actor", "Dup", "Act", false);
				d_drFile = OpenOutput("Duplica", "Dr", "Dup", "Dr", false);
				d_updateFile = OpenOutput("Duplica", "Update", "Dup", "Up", false);
			}
		}
	}
	else if (d_actorFile != NULL)
	{
		fclose(d_actorFile);
		fclose(d_drFile);
		fclose(d_updateFile);
		d_actorFile = NULL;
		d_drFile = NULL;
		d_updateFile = NULL;
	}
}

/******************************************************************************/
/******************************************************************************/
FILE* DRPathFollowerModel::OpenOutput(const char* i_longRole,
		char* i_longFile, const char* i_shortRole, const char* i_shortFile,
		bool i_reason)
{
	char name[20];
	sprintf(name, "%s%s.txt", i_longRole, i_longFile);
	FILE* fp = fopen(name, "w");
	ASSERT_PTR(fp);
	
	sprintf(name, "%s%s", i_shortRole, i_shortFile);
	fprintf(fp, "%sTime", name);
	fprintf(fp, "\t%sActive", name);
	fprintf(fp, "\t%sSpeedTarget", name);
	fprintf(fp, "\t%sSpeedCurrent", name);
	fprintf(fp, "\t%sDistance", name);
	fprintf(fp, "\t%sGnd", name);
	if (i_reason)
		fprintf(fp, "\tReason");
	fprintf(fp, "\n");

	return(fp);
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerModel::OutputState(FILE* i_fp,
		const DSPathFollower &i_data, DOActor* i_actor, int i_reason) const
{
	if (i_fp != NULL)
	{
#ifdef LATENCY_COMPENSATION
		fprintf(i_fp, "%f", (float)g_sessionTime.ConvertTimeHostToSession(
				i_actor, i_data.d_time) * 0.001f);
#else
		fprintf(i_fp, "%f", (float)g_sessionTime.GetSessionTime() * 0.001f);
#endif
		fprintf(i_fp, "\t%c",
				((i_data.d_flags & DISCONTINUITY_STATE) != 0) ? '*' : ' ');
		fprintf(i_fp, "\t%c",
				((i_data.d_flags & DISCONTINUITY_SPEED_TARGET) != 0) ? '*' : ' ');
		fprintf(i_fp, "\t%c",
				((i_data.d_flags & DISCONTINUITY_SPEED_CURRENT) != 0) ? '*' : ' ');
		fprintf(i_fp, "\t%c",
				((i_data.d_flags & DISCONTINUITY_PATH_DISTANCE) != 0) ? '*' : ' ');
		switch(i_reason)
		{
		 case REASON_NONE:
			break;
		 case REASON_DISCONTINUITY:
			fprintf(i_fp, "\tDIS");
			break;
		 case REASON_STATE:
			fprintf(i_fp, "\tACTIVE");
			break;
		 case REASON_SPEED_TARGET:
			fprintf(i_fp, "\tSPEED_TARGET");
			break;
		 case REASON_SPEED_CURRENT:
			fprintf(i_fp, "\tSPEED_CURRENT");
			break;
		 case REASON_DISTANCE:
			fprintf(i_fp, "\tDISTANCE");
			break;
		}
		fprintf(i_fp, "\n");
	}
}
#endif

////////////////////////// DRPathFollowerUtility ///////////////////////////

//-------------------------------------------------------------------------
//	adjust our speed for turns
//-------------------------------------------------------------------------
float DRPathFollowerUtility::AdjustSpeed(DSPathFollower* i_dsPF, 
		float i_deltaSec)
{
	return (AdjustSpeed(i_dsPF->GetPathFollowerObject(), 
			i_dsPF->d_speedTarget, i_dsPF->d_speedCurrent, 
			i_dsPF->d_distance, i_deltaSec));
}

//-------------------------------------------------------------------------
//	adjust our speed for turns
//-------------------------------------------------------------------------
float DRPathFollowerUtility::AdjustSpeed(CPathFollower* i_pPF, 
		float i_speedTarget, float i_speedCurrent, float i_distance, 
		float i_deltaSec)
{
	float newSpeed = i_speedCurrent;

	// slow down if we can't make the turn
	if (i_pPF->IsSegmentPathMode() == false)
	{
		// can we make the turn at current speed
		if (CanMakeTurn(i_pPF, i_speedCurrent, i_distance, i_deltaSec))
		{
			// attempt to speed up
			newSpeed = SpeedUp(i_pPF, i_speedTarget, i_speedCurrent, i_deltaSec);
		}
		else
		{
			// we can't make the turn at current speed, slow down
			newSpeed -= (i_pPF->GetAccelerationMaximum() * i_deltaSec);

			// min 1 m/s?
			if (newSpeed <= 0.0f)
				newSpeed = 1.0f;
		}
	}
	else
	{
		newSpeed = SpeedUp(i_pPF, i_speedTarget, i_speedCurrent, i_deltaSec);
	}

	return (newSpeed);
}

//-------------------------------------------------------------------------
//	Proceed on the path with the current parameters
//-------------------------------------------------------------------------
void DRPathFollowerUtility::ProceedAlongPath(CPathFollower* i_pPF, 
		float &io_speedTarget, float &io_speedCurrent, float &io_distance, 
		float i_deltaSec)
{
	// proceed along the path with the current speed
	io_distance += io_speedCurrent * i_deltaSec;

	// See if we've already reached an endpoint
	if (io_distance == i_pPF->GetPath()->Length())
	{}
	else if (io_distance > i_pPF->GetPath()->Length() ) // check for completed circuits
	{
		// Wrap (for circuits)
		if (i_pPF->GetPath()->ClosedLoop())
		{
			io_distance -= i_pPF->GetPath()->Length();
		}
		else
		{
			if (i_pPF->IsReverseAtEnd())
			{
				io_distance = i_pPF->GetPath()->Length();
				io_speedTarget = -io_speedCurrent;
				io_speedCurrent = 0.0f;

				// Account for delay at path end
				i_deltaSec -= i_pPF->GetResetDelay();
				if (i_deltaSec < 0.0f)
					i_deltaSec = 0.0f;

				io_speedCurrent = SpeedUp(i_pPF, io_speedTarget, io_speedCurrent, i_deltaSec);
			}
			else
			{
				io_speedCurrent = 0;
				io_distance = i_pPF->GetPath()->Length();
			}
		}
	}
	else if ((io_distance <= 0.0f) && (io_speedCurrent < 0.0f))
	{
		if (!i_pPF->IsReverseAtEnd() && io_speedTarget < 0.0f)  //if you are going backward in a loop, continue looping backward
		{
			io_distance = i_pPF->GetPath()->Length() - 0.0001f;
		}
		else
		{
			io_distance = 0.0f;
			io_speedTarget = -io_speedCurrent;
			io_speedCurrent = 0.0f;

			// Account for delay at path end
			i_deltaSec -= i_pPF->GetResetDelay();
			if (i_deltaSec < 0.0f)
				i_deltaSec = 0.0f;

			io_speedCurrent = SpeedUp(i_pPF, io_speedTarget, io_speedCurrent, i_deltaSec);
		}
	}
}

//-------------------------------------------------------------------------
//	Find the new maximum velocity.
// If the velocity reaches the target before time is up then return
// the leftover time.
//-------------------------------------------------------------------------
void DRPathFollowerUtility::ProceedAlongPartialPath(CPathFollower* i_pPF, 
		float &io_speedTarget, float &io_speedCurrent, float &io_distance, 
		float i_deltaSec, float &o_extraSec)
{
	o_extraSec = 0.0f;
	float newSpeed;

	if (i_pPF->IsSegmentPathMode())
		newSpeed = io_speedTarget;
	else
	{
		if (Math::Zero(i_pPF->GetAccelerationMaximum()))
			newSpeed = io_speedCurrent;

		float accel = i_pPF->GetAccelerationMaximum() * i_deltaSec;
		newSpeed = io_speedCurrent;

		// try to accelerate or decelerate to target speed
		if (io_speedTarget > 0.0f)
		{
			if (newSpeed < io_speedTarget)
			{
				if ((newSpeed + accel) > io_speedTarget)
				{
					newSpeed = io_speedTarget;
					o_extraSec = ((newSpeed + accel) - io_speedTarget) / i_pPF->GetAccelerationMaximum();
				}
				else
				{
					newSpeed += accel;
				}
			}
			else if (newSpeed > io_speedTarget)
			{
				if ((newSpeed - accel) < io_speedTarget)
				{
					newSpeed = io_speedTarget;
					o_extraSec = (io_speedTarget - (newSpeed - accel)) / i_pPF->GetAccelerationMaximum();
				}
				else
				{
					newSpeed -= accel;
				}
			}
		}
		else
		{
			if (newSpeed > io_speedTarget)
			{
				if ((newSpeed - accel) < io_speedTarget)
				{
					newSpeed = io_speedTarget;
					o_extraSec = (io_speedTarget - (newSpeed - accel)) / i_pPF->GetAccelerationMaximum();
				}
				else
				{
					newSpeed -= accel;
				}
			}
			else if (newSpeed < io_speedTarget)
			{
				newSpeed = io_speedTarget;
				o_extraSec = i_deltaSec;
			}
		}
	}

	// Change the time to include only that necessary to reach the target speed
	i_deltaSec -= o_extraSec;

	// proceed along the path with the current speed
	io_distance += ((io_speedCurrent + newSpeed) / 2.f) * i_deltaSec;
	io_speedCurrent = newSpeed;

	// See if we've already reached an endpoint
	if (io_distance == i_pPF->GetPath()->Length())
	{} 
	else if (io_distance > i_pPF->GetPath()->Length()) // check for completed circuits
	{
		// Wrap (for circuits)
		if (i_pPF->GetPath()->ClosedLoop())
		{
			io_distance -= i_pPF->GetPath()->Length();
		}
		else
		{
			if (i_pPF->IsReverseAtEnd())
			{
				io_distance = i_pPF->GetPath()->Length();
				io_speedTarget = -io_speedCurrent;
				io_speedCurrent = 0.0f;

				// Account for delay at path end
				i_deltaSec -= i_pPF->GetResetDelay();
				if (i_deltaSec < 0.0f)
					i_deltaSec = 0.0f;

				io_speedCurrent = SpeedUp(i_pPF, io_speedTarget, io_speedCurrent, i_deltaSec);
			}
			else
			{
				io_speedCurrent = 0;
				io_distance = i_pPF->GetPath()->Length();
			}
		}
	}
	else if ((io_distance <= 0.0f) && (io_speedCurrent < 0.0f))
	{
		if (!i_pPF->IsReverseAtEnd() && io_speedTarget < 0.0f)  //if you are going backward in a loop, continue looping backward
		{
			io_distance = i_pPF->GetPath()->Length() - 0.0001f;
		}
		else
		{
			io_distance = 0.0f;
			io_speedTarget = -io_speedCurrent;
			io_speedCurrent = 0.0f;

			// Account for delay at path end
			i_deltaSec -= i_pPF->GetResetDelay();
			if (i_deltaSec < 0.0f)
				i_deltaSec = 0.0f;

			io_speedCurrent = SpeedUp(i_pPF, io_speedTarget, io_speedCurrent, i_deltaSec);
		}
	}
}

//-------------------------------------------------------------------------
//	Test to see if we can handle the coming turn
//-------------------------------------------------------------------------
bool DRPathFollowerUtility::CanMakeTurn(DSPathFollower* i_dsPF, 
		float i_deltaSec)
{
	return (CanMakeTurn(i_dsPF->GetPathFollowerObject(), 
			i_dsPF->d_speedCurrent, i_dsPF->d_distance, i_deltaSec));
}

//-------------------------------------------------------------------------
//	Test to see if we can handle the coming turn
//-------------------------------------------------------------------------
bool DRPathFollowerUtility::CanMakeTurn(CPathFollower* i_pPF, 
		float i_speedCurrent, float i_distance, float i_deltaSec)
{
	if (i_pPF->IsSegmentPathMode())
		return (true);
	else
	{
		// anything traveling under 30 miles/hour can take the turn!
		if (i_speedCurrent < (MPHToMPS(30.0f)))
			return (true);
		else
		{
			float temp, minR, currR, deltaRadians;
			Vector3 tangentOld, tangentNew, positionOld, positionNew;
			Segment radialDistance;

			tangentOld.Set(0.0f, 0.0f, 0.0f);

			// calculate minimum radius of circle able to drive
			// r = v^2/a;
			temp = i_speedCurrent;
			minR = ((temp * temp) / (i_pPF->GetTurningGeesMaximum() 
					* constGeesToAccel)) * i_deltaSec;

			// record the current orientation and position
			tangentOld = i_pPF->GetDirectionCurrent();
			positionOld = i_pPF->GetPosition();

			// calc new orientation and position
			// distance we want to travel this frame
			float d = i_distance + (i_deltaSec * i_speedCurrent);

			positionNew = i_pPF->GetPath()->PointFromDistance(d);
			tangentNew = i_pPF->GetPath()->TangentFromDistance(d);

			// find difference in radians ( about the y-axis ), and the distance traveled
			// find the distance between the two vectors
			radialDistance.start = positionNew;
			radialDistance.end = positionOld;

			float cosRadians = tangentNew.Dot(tangentOld);
			if (cosRadians > 1.0f)
				cosRadians = 1.0f;
			else if (cosRadians < -1.0f)
				cosRadians = -1.0f;
			deltaRadians = Math::ArcCos(cosRadians);

			// calc radius of desired circle
			// ((distance) / (dTheta))
			currR = (radialDistance.Length( ) / deltaRadians);

			// if circleR is < minR, can't make it, slow down
			if (currR >= minR)
				return (true);
			else
				return (false);
		}
	}
}

//-------------------------------------------------------------------------
//	Speed up to maximum velocity
//-------------------------------------------------------------------------
float DRPathFollowerUtility::SpeedUp(DSPathFollower* i_dsPF, 
		float i_deltaSec)
{
	return (SpeedUp(i_dsPF->GetPathFollowerObject(), 
			i_dsPF->d_speedTarget, i_dsPF->d_speedCurrent, i_deltaSec));
}

//-------------------------------------------------------------------------
//	Speed up to maximum velocity
//-------------------------------------------------------------------------
float DRPathFollowerUtility::SpeedUp(CPathFollower* i_pPF, 
		float i_speedTarget, float i_speedCurrent, float i_deltaSec)
{
	if (i_pPF->IsSegmentPathMode())
		return (i_speedTarget);
	else
	{
		if (Math::Zero(i_pPF->GetAccelerationMaximum()))
			return (i_speedCurrent);

		float accel = i_pPF->GetAccelerationMaximum() * i_deltaSec;
		float newSpeed = i_speedCurrent;

		// try to accelerate or decelerate to target speed
		if (i_speedTarget > 0.0f)
		{
			if (newSpeed < i_speedTarget)
			{
				if ((newSpeed + accel) > i_speedTarget)
				{
					newSpeed = i_speedTarget;
				}
				else
				{
					newSpeed += accel;
				}
			}
			else if (newSpeed > i_speedTarget)
			{
				if ((newSpeed - accel) < i_speedTarget)
				{
					newSpeed = i_speedTarget;
				}
				else
				{
					newSpeed -= accel;
				}
			}
		}
		else
		{
			if (newSpeed > i_speedTarget)
			{
				if ((newSpeed - accel) < i_speedTarget)
				{
					newSpeed = i_speedTarget;
				}
				else
				{
					newSpeed -= accel;
				}
			}
			else if (newSpeed < i_speedTarget)
			{
				newSpeed = i_speedTarget;
			}
		}

		return (newSpeed);
	}
}

////////////////////////// DRPathFollowerComponent ///////////////////////////

/******************************************************************************/
/******************************************************************************/
DRPathFollowerComponent::DRPathFollowerComponent(CCompActor &i_actor,
		DuplicatedObject &i_do, DSPathFollower &i_ds) :
	CActorComponent(i_actor),
	d_do(i_do),
	d_ds(i_ds)
{
	d_param = CParam<DRPathFollowerParam>::GetParam("DRPathFollower", "var");
	ASSERT_PTR(d_param);

	// Create the PathFollower object
	d_pathFollower = (CPathFollower*)actor.AttachComponent("PathFollower");
	ASSERT_PTR(d_pathFollower);
	d_ds.SetPathFollowerObject(*d_pathFollower);

	d_beginState = STATE_DEAD;
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::BeginFrame(void)
{
	d_currTime = g_timer.GetStartSec();
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::BeginUpdate(void)
{
	d_beginTime = d_currTime;
	d_beginState = d_pathFollower->GetState();
	d_beginSpeedTarget = d_pathFollower->GetSpeedTarget();
	d_beginSpeedCurrent = d_pathFollower->GetSpeedCurrent();
	d_beginDistance = d_pathFollower->GetPathDistance();

	// Begin update of PathFollower component
	//d_pathFollower->BeginUpdate();
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::AttemptUpdate(float i_deltaSec)
{
	// Move dead reckoned state to the target time.
	d_currTime = d_beginTime + i_deltaSec;
	d_do.Refresh(d_ds, g_sessionTime.ConvertTimeGameToNet(d_currTime));
	
	// Get the active discontinuity flags.
	uint8 flags;
	DRPathFollowerModel* model = d_ds.GetLocalDRModel();
	ASSERT_PTR(model);
	flags = model->GetDiscontinuityFlags();

	// Extract target state.
	d_pathFollower->SetState(d_ds.d_state);
	d_pathFollower->SetSpeedTarget(Math::SignedDecompress(d_ds.d_speedTarget, MAX_SPEED));
	d_pathFollower->SetSpeedCurrent(Math::SignedDecompress(d_ds.d_speedCurrent, MAX_SPEED));
	d_pathFollower->SetPathDistance(Math::SignedDecompress(d_ds.d_distance, MAX_DISTANCE));

	// Exit the old state and enter the new one
	if (d_ds.d_state != d_beginState)
	{
		// Exit the old state
		switch (d_beginState)
		{
			case STATE_DO_JOB:
			case STATE_FLEE:
				d_pathFollower->SetFaceMovementDirection(false);
				d_pathFollower->Deactivate();
				break;
			case STATE_WATCH:
			case STATE_COWER:
			case STATE_FIGHT:
			case STATE_HOSTAGE:
			case STATE_SITTING:
			case STATE_STUNNED:
			case STATE_ON_FIRE:
			case STATE_DEAD:
			default:
				break;
		}

		// Enter the new state
		switch (d_ds.d_state)
		{
			case STATE_DO_JOB:
				if(d_pathFollower->IsEvacuator())
				{
					d_pathFollower->SetFaceMovementDirection(false);
					d_pathFollower->Deactivate();
				}
				else
				{
					d_pathFollower->SetFaceMovementDirection(true);
					d_pathFollower->Activate();
				}
				break;
			case STATE_FLEE:
				d_pathFollower->SetFaceMovementDirection(true);
				d_pathFollower->Activate();
				if (d_pathFollower->IsRemoveAtPathEnd() && (d_pathFollower->IsAtEndpoint() || d_pathFollower->GetPathDistanceFromT() >= 1.0f))
					d_pathFollower->Deactivate();
				break;
			case STATE_WATCH:
			case STATE_COWER:
			case STATE_FIGHT:
			case STATE_HOSTAGE:
			case STATE_SITTING:
			case STATE_STUNNED:
			case STATE_ON_FIRE:
			case STATE_DEAD:
				d_pathFollower->Deactivate();
			default:
				break;
		}

		d_beginState = d_ds.d_state;
	}
	else
	{
		// Run the current state
		switch (d_ds.d_state)
		{
			case STATE_FLEE:
			case STATE_DO_JOB:
				//d_pathFollower->AttemptUpdate(i_deltaSec);
				break;
			case STATE_WATCH:
			case STATE_COWER:
			case STATE_FIGHT:
			case STATE_HOSTAGE:
			case STATE_SITTING:
			case STATE_STUNNED:
			case STATE_ON_FIRE:
			case STATE_DEAD:
			default:
				break;
		}
	}

	actor.SyncState();

#ifdef DEBUG_PATH_FOLLOWER
	DSPathFollower data;
	Push(&actor, data, d_currTime);
	data.d_flags = d_ds.d_flags;
	model->OutputActorState(data, model->GetActor());
#endif
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::EndUpdate(float i_deltaSec)
{
	DRPathFollowerModel* model = d_ds.GetLocalDRModel();
	if (model)
		model->ClearDiscontinuityFlags();
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::Pull(void)
{
	d_pathFollower->SetState(d_ds.d_state);
	d_pathFollower->SetSpeedTarget(	Math::SignedDecompress(d_ds.d_speedTarget, MAX_SPEED));
	d_pathFollower->SetSpeedCurrent(	Math::SignedDecompress(d_ds.d_speedCurrent, MAX_SPEED));
	d_pathFollower->SetPathDistance(	Math::SignedDecompress(d_ds.d_distance, MAX_DISTANCE));

	d_pathFollower->PlaceActor();
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::Push(CActor* i_actor, DSPathFollower &i_ds,
		float i_time, bool i_onPath, bool i_debug)
{
#ifdef LATENCY_COMPENSATION
	i_ds.d_time = i_time;
#endif
	i_ds.d_flags = (qByte)(i_actor->GetDiscontinuity() & PF_DISCONTINUITY_ANY);
#ifdef DEBUG_PATH_FOLLOWER
	if (i_debug)
		i_ds.d_flags |= (qByte)FLAG_DEBUG;
#endif

	ASSERTS(i_ds.GetPathFollowerObject()->GetState() >= 0 && i_ds.GetPathFollowerObject()->GetState() <= STATE_DEAD, "pathfollower state out of range, contact Bryce");
	i_ds.d_state = i_ds.GetPathFollowerObject()->GetState();
	i_ds.d_speedTarget = Math::SignedCompressToWord(i_ds.GetPathFollowerObject()->GetSpeedTarget(), MAX_SPEED);
	i_ds.d_speedCurrent = Math::SignedCompressToWord(i_ds.GetPathFollowerObject()->GetSpeedCurrent(), MAX_SPEED);
	i_ds.d_distance = Math::SignedCompressToWord(i_ds.GetPathFollowerObject()->GetPathDistance(), MAX_DISTANCE);
}

/******************************************************************************/
/******************************************************************************/
void DRPathFollowerComponent::SetPathFollowerObject(
		CPathFollower* i_pathFollower, DSPathFollower &i_ds)
{
	i_ds.SetPathFollowerObject(*i_pathFollower);
}
