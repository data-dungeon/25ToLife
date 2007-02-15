/*
**
**  File:   MoveIF.h
**  Date:   February 4, 2004
**  By:     Bryant Collard
**  Desc:   An interface to control an object's movement.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/MoveIF.h $
**  $Revision: #10 $
**  $DateTime: 2005/03/29 12:30:12 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef MOVE_IF_H
#define MOVE_IF_H

#include "Interfaces/Interface.h"
#include "Math/Vector.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CActor;

class CMoveIF : public CInterface
{
public:
	static uint GetID(void) {return(CInterface::GetValidID(s_moveID));}
	static CMoveIF* GetInterface(CActor* i_actor);
	static CMoveIF* GetInterface(ActorHandle i_handle);

	virtual bool CommandTurnToPoint(const CActor &actor, Vector3CRef toPoint);
	virtual bool CommandTurnToView(CActor &actor);
	virtual bool CommandTurnDeg(float i_turnDeg);
	virtual bool CommandTurnRad(float i_turnRad) {return(false);}
	virtual bool CommandHeadingDeg(float i_headingDeg);
	virtual bool CommandHeadingRad(float i_headingRad) {return(false);}
	virtual bool CommandTurnRateDeg(float i_turnRateDeg, bool i_normalized = false);
	virtual bool CommandTurnRateRad(float i_turnRateRad, bool i_normalized = false) {return(false);}
	virtual bool CommandMoveDirection(Vector3CRef i_dir, float i_speed, bool i_normalized = false);
	virtual bool CommandMoveDirection(Vector3CRef i_velocity, bool i_normalized = false) {return(false);}
	virtual bool CommandMoveToPoint(Vector3CRef i_point, float i_speed, bool i_normalized = false) {return(false);}
	virtual bool CommandForwardSpeed(float i_forwardSpeed, bool i_normalized = false) {return(false);}
	virtual bool CommandSideSpeed(float i_sideSpeed, bool i_normalized = false) {return(false);}
	virtual bool CommandVerticalSpeed(float i_speed, bool i_normalized = false) {return(false);}
	virtual float DistanceSquaredToPoint( Vector3CRef i_point ) { return 0.0f; }

	virtual bool Crouch( bool crouch = true ) {return(false);}  
	virtual bool JumpToTarget( Vector3CRef i_point ) {return(false);}
private:
	static uint s_moveID;
};

#endif // MOVE_IF_H
