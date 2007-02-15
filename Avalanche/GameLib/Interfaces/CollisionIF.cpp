/*
**
**  File:   CollisionIF.cpp
**  Date:   July 13, 2004
**  By:     Bryant Collard
**  Desc:   Interfaces to get at collision attributes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/CollisionIF.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/CollisionIF.h"
#include "stage/cactor.h"

uint CCollisionIF::s_collisionID = INTERFACE_ID_INVALID;
uint COrientedCollisionIF::s_orientedCollisionID = INTERFACE_ID_INVALID;

/******************************************************************************/
/******************************************************************************/
CCollisionIF* CCollisionIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CCollisionIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
CCollisionIF* CCollisionIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CCollisionIF*)actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
COrientedCollisionIF* COrientedCollisionIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((COrientedCollisionIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
COrientedCollisionIF* COrientedCollisionIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((COrientedCollisionIF*)actor->GetInterface(GetID()));
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
Vector3 CCollisionIF::GetCollisionInWorld(void)
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}

/******************************************************************************/
/******************************************************************************/
Vector3 CCollisionIF::GetCollisionInActor(void)
{
	return(Vector3(0.0f, 0.0f, 0.0f));
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 COrientedCollisionIF::GetCollisionToWorld(void)
{
	DirCos3x3 m;
	m.Identity();
	return(m);
}

/******************************************************************************/
/******************************************************************************/
DirCos3x3 COrientedCollisionIF::GetCollisionToActor(void)
{
	DirCos3x3 m;
	m.Identity();
	return(m);
}

/******************************************************************************/
/******************************************************************************/
float COrientedCollisionIF::GetXSize(void)
{
	return(0.0f);
}

/******************************************************************************/
/******************************************************************************/
float COrientedCollisionIF::GetYSize(void)
{
	return(0.0f);
}

/******************************************************************************/
/******************************************************************************/
float COrientedCollisionIF::GetZSize(void)
{
	return(0.0f);
}
