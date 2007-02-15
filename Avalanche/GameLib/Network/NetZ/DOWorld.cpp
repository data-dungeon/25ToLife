/*
**
**  File:   DOWorld.cpp
**  Date:   March 2, 2004
**  By:     Bryant Collard
**  Desc:   Implementation of the world distributed object.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "Network/NetworkPCH.h"

// My header
#include "Network/NetZ/DOWorld.h"

// NetZ headers
#include <netz.h>
#include "Network/NetZ/DOActor.h"

/******************************************************************************/
/******************************************************************************/
DOWorld::DOWorld() : CWorld(d_extents.d_set,
		d_extents.d_centerX, d_extents.d_centerY, d_extents.d_centerZ,
		d_extents.d_extentX, d_extents.d_extentY, d_extents.d_extentZ)
{
}

/******************************************************************************/
/******************************************************************************/
void DOWorld::OperationEnd(DOOperation* i_operation)
{
	switch(i_operation->GetType())
	{
	 case Operation::ChangeMasterStation:
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		GetMessageOwner().SetMaster(IsADuplicationMaster());
		break;

	 case Operation::AddToStore:
		ASSERT(!ValidSingleton(CWorld));
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		SetSingleton(CWorld, this);
		GetMessageOwner().SetMaster(IsADuplicationMaster());
		GetMessageOwner().SetSessionID(GetHandleValue());
		break;

	 case Operation::RemoveFromStore:
		SET_MESSAGE_DISPATCHER_CALL_LOCATION();
		GetMessageOwner().SetSessionID(INVALID_DOHANDLE_VALUE);
		break;
	}
}

/******************************************************************************/
/******************************************************************************/
void DOWorld::Update(void)
{
	CWorld::Update();
	DuplicatedObject::Update();
}

/******************************************************************************/
/******************************************************************************/
void DOWorld::ClearExtents(void)
{
	if (IsADuplicationMaster())
	{
		CWorld::ClearExtents();
		d_extents.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOWorld::SetExtents(Vector3CRef i_center, Vector3CRef i_extent)
{
	if (IsADuplicationMaster())
	{
		CWorld::SetExtents(i_center, i_extent);
		d_extents.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
void DOWorld::StartSessionClock(void)
{
	if (IsADuplicationMaster())
	{
		d_start.d_time = SessionClock::GetTime();
		d_start.RequestUpdate();
	}
}

/******************************************************************************/
/******************************************************************************/
Time DOWorld::GetStartTime(void)
{
	return(d_start.d_time);
}

/******************************************************************************/
/******************************************************************************/
CMessageOwner* DOWorld::MessageOwner(void)
{
	return(&GetMessageOwner());
}
