/*
**
**  File:   CollisionIF.h
**  Date:   July 13, 2004
**  By:     Bryant Collard
**  Desc:   Interfaces to get at collision attributes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Interfaces/CollisionIF.h $
**  $Revision: #4 $
**  $DateTime: 2005/03/29 12:30:12 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef COLLISION_IF_H
#define COLLISION_IF_H

#include "Interfaces/Interface.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "GameHelper/Handle.h"

class CActor;
handledef(CActor) ActorHandle;

class CCollisionIF : public CInterface
{
  public:
	static uint GetID(void) {return(CInterface::GetValidID(s_collisionID));}
	static CCollisionIF* GetInterface(CActor* i_actor);
	static CCollisionIF* GetInterface(ActorHandle i_handle);

	virtual void SetSurfaceType(uint8 i_surfaceType) {}
	virtual void SetSurfaceName(const char* i_surfaceName) {}
	virtual Vector3 GetCollisionInWorld(void);
	virtual Vector3 GetCollisionInActor(void);
	virtual void SetCollisionInActor(Vector3CRef i_collisionInActor) {}
	virtual void MoveCollisionInActor(Vector3CRef i_offsetInActor) {}

  private:
	static uint s_collisionID;
};

class COrientedCollisionIF : public CCollisionIF
{
  public:
	static uint GetID(void)
		{return(CInterface::GetValidID(s_orientedCollisionID));}
	static COrientedCollisionIF* GetInterface(CActor* i_actor);
	static COrientedCollisionIF* GetInterface(ActorHandle i_handle);

	virtual DirCos3x3 GetCollisionToWorld(void);
	virtual DirCos3x3 GetCollisionToActor(void);
	virtual void SetCollisionToActor(const DirCos3x3 &i_collisionToActor) {}
	virtual void RotateCollisionToActor(const DirCos3x3 &i_newToOldCollision) {}
	virtual float GetXSize(void);
	virtual float GetYSize(void);
	virtual float GetZSize(void);
	virtual void SetXSize(float i_size) {}
	virtual void SetYSize(float i_size) {}
	virtual void SetZSize(float i_size) {}
	virtual void SetSize(Vector3CRef i_size) {}
	virtual void ScaleXSize(float i_scale) {}
	virtual void ScaleYSize(float i_scale) {}
	virtual void ScaleZSize(float i_scale) {}
	virtual void ScaleSize(Vector3CRef i_scale) {}

  private:
	static uint s_orientedCollisionID;
};

#endif // COLLISION_IF_H
