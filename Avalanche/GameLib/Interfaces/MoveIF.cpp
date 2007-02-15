/*
**
**  File:   Move.cpp
**  Date:   February 4, 2004
**  By:     Bryant Collard
**  Desc:   An interface to control an object's movement.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/MoveIF.cpp $
**  $Revision: #7 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/MoveIF.h"
#include "stage/cactor.h"
#include "Interfaces/ViewIF.h"
#include "stage/cactor.h"

uint CMoveIF::s_moveID = INTERFACE_ID_INVALID;

CMoveIF* CMoveIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CMoveIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

CMoveIF* CMoveIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CMoveIF*)actor->GetInterface(GetID()));
	return(NULL);
}

bool CMoveIF::CommandTurnDeg(float i_turnDeg)
{
	return(CommandTurnRad(Math::Deg2Rad(i_turnDeg)));
}

bool CMoveIF::CommandHeadingDeg(float i_headingDeg)
{
	return(CommandHeadingRad(Math::Deg2Rad(i_headingDeg)));
}

bool CMoveIF::CommandTurnToPoint(const CActor &actor, Vector3CRef toPoint)
{
	Vector3 v = toPoint - actor.GetBodyInWorld();
	if( v.SafeNormalize() )
	{
		Vector3 forward = actor.GetBodyToWorld().GetRow2();
		float heading = Math::ArcTan2( forward.z(), forward.x() );
		float newheading = Math::ArcTan2( v.z(), v.x() );

		return CommandHeadingRad( Math::NormalAngle( newheading - heading ) );
	}

	return false;
}

bool CMoveIF::CommandTurnToView(CActor &actor)
{
	Vector3 forward;
	ViewIF *view = (ViewIF *)actor.GetInterface( ViewIF::GetID() );
	if( view )
		view->GetDirection( forward );
	else
		forward = actor.GetBodyToWorld().GetRow2();

	return CommandTurnToPoint( actor, actor.GetBodyInWorld() + forward );
}

bool CMoveIF::CommandTurnRateDeg(float i_turnRateDeg, bool i_normalized)
{
	if (i_normalized)
		return(CommandTurnRateRad(i_turnRateDeg, i_normalized));
	return(CommandTurnRateRad(Math::Deg2Rad(i_turnRateDeg), i_normalized));
}

bool CMoveIF::CommandMoveDirection(Vector3CRef i_dir, float i_speed,
		bool i_normalized)
{
	return(CommandMoveDirection(i_speed * i_dir.Normalized(), i_normalized));
}
