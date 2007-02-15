/*
**
**  File:   SurfaceTypeIF.h
**  Date:   July 2, 2004
**  By:     Bryant Collard
**  Desc:   An interface to get at surface types.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/SurfaceTypeIF.h $
**  $Revision: #2 $
**  $DateTime: 2005/03/29 12:30:12 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef SURFACE_TYPE_IF_H
#define SURFACE_TYPE_IF_H

#include "Interfaces/Interface.h"
#include "Math/Vector.h"
#include "Game/Database/Som.h"     // For INVALID_MATERIALINDEX
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CSurfaceTypeIF : public CInterface
{
  public:
	static uint GetID(void) {return(CInterface::GetValidID(s_surfaceTypeID));}
	static CSurfaceTypeIF* GetInterface(CActor* i_actor);
	static CSurfaceTypeIF* GetInterface(ActorHandle i_handle);

	virtual int GetSurfaceType(Vector3CRef i_point, Vector3CRef i_normal);

  private:
	static uint s_surfaceTypeID;
};

#endif // SURFACE_TYPE_IF_H
