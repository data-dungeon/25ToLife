/*
**
**  File:   SurfaceTypeIF.cpp
**  Date:   July 2, 2004
**  By:     Bryant Collard
**  Desc:   An interface to get at surface types.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/SurfaceTypeIF.cpp $
**  $Revision: #3 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#include "Interfaces/InterfacesPCH.h"
#include "Interfaces/SurfaceTypeIF.h"
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX
#include "stage/cactor.h"

uint CSurfaceTypeIF::s_surfaceTypeID = INTERFACE_ID_INVALID;

CSurfaceTypeIF* CSurfaceTypeIF::GetInterface(CActor* i_actor)
{
	if (i_actor != NULL)
		return((CSurfaceTypeIF*)i_actor->GetInterface(GetID()));
	return(NULL);
}

CSurfaceTypeIF* CSurfaceTypeIF::GetInterface(ActorHandle i_handle)
{
	CActor* actor = CActor::FromHandle(i_handle);
	if (actor != NULL)
		return((CSurfaceTypeIF*)actor->GetInterface(GetID()));
	return(NULL);
}

int CSurfaceTypeIF::GetSurfaceType(Vector3CRef i_point, Vector3CRef i_normal)
{
	return(INVALID_MATERIALINDEX);
}
