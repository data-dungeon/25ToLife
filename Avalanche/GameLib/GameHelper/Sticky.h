/*
**
**  File:   Sticky.h
**  Date:   July 1, 2004
**  By:     Bryant Collard
**  Desc:   Implements a class that represents a coordinate system that is
**          attached to (or stuck on) some entity or object. The manner of
**          attachment is highly dependent on the object the sticky is attached
**          to. A sticky on terrain, for example, is very simple while one on
**          a boned model could be quite complex. This diversity of attachment
**          cannot, however, be addressed by deriving classes from a sticky
**          base class. A goal of this class is to avoid dynamic allocation and
**          to not require entities to implement code to supply appropriate
**          stickies. Rather, a user of this class will generally have a
**          a sticky and stick it to any entity of interest.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/GameHelper/Sticky.h $
**  $Revision: #3 $
**  $DateTime: 2004/10/25 12:08:32 $
**    $Author: Food $
**
*/

#pragma once

#ifndef STICKY_H
#define STICKY_H

#include "Components/Component.h"
#include "Math/Vector.h"

class CActor;

#define STICKY_ACCESS_FUNCTIONS                                    \
	Vector3 &GetLocation(Vector3 &o_result) const;                  \
	DirCos3x3 &GetOrientation(DirCos3x3 &o_result) const;           \
	Cartesian4x4 &GetCartesian(Cartesian4x4 &o_result) const;       \
	Graphics4x4 &GetTransform(Graphics4x4 &o_result) const;         \
	Vector3 &GetVelocity(Vector3 &o_result) const;                  \
	Vector3 &GetAngularVelocity(Vector3 &o_result) const;

class Vector3Trivial
{
  public:
	void Set(Vector3CRef i_vector) {*(Vector3*)&d_block = i_vector;}
	Vector3 &Get(void) const {return(*(Vector3*)&d_block);}

  private:
	char d_block[sizeof(Vector3)];
} ALIGNED(16);

class CSticky
{
  public:
	// Construct.
	CSticky();

	// Establish attachement.
	bool SetInWorld(CStageEntity* i_entity, Vector3CRef i_locationInWorld);
	bool SetInBody(CStageEntity* i_entity, Vector3CRef i_locationInBody);

	// Get information.
	Vector3 GetLocation(void) const;
	DirCos3x3 GetOrientation(void) const;
	Cartesian4x4 GetCartesian(void) const;
	Graphics4x4 GetTransform(void) const;
	Vector3 GetVelocity(void) const;
	Vector3 GetAngularVelocity(void) const;

	// Fill in information.
	STICKY_ACCESS_FUNCTIONS

  private:
	enum EStickyType
	{
		STICKY_TERRAIN,
		STICKY_BODY,
		STICKY_BONE,
		STICKY_NONE
	};

	class CTerrainSticky
	{
	  public:
		STICKY_ACCESS_FUNCTIONS
	  private:
		Vector3Trivial d_locationInWorld;
		friend class CSticky;
	};

	class CBodySticky
	{
	  public:
		STICKY_ACCESS_FUNCTIONS
	  private:
		CActor* d_actor;
		Vector3Trivial d_locationInBody;
		friend class CSticky;
	};

	class CBoneSticky
	{
	  public:
		STICKY_ACCESS_FUNCTIONS
	  private:
		CActor* d_actor;
		Vector3Trivial d_locationInBody;
		friend class CSticky;
	};

	EStickyType d_type;
	union
	{
		CTerrainSticky d_terrain;
		CBodySticky d_body;
		CBoneSticky d_bone;
	};
};

inline Vector3 CSticky::GetLocation(void) const
{
	Vector3 result;
	return(GetLocation(result));
}

inline DirCos3x3 CSticky::GetOrientation(void) const
{
	DirCos3x3 result;
	return(GetOrientation(result));
}

inline Cartesian4x4 CSticky::GetCartesian(void) const
{
	Cartesian4x4 result;
	return(GetCartesian(result));
}

inline Graphics4x4 CSticky::GetTransform(void) const
{
	Graphics4x4 result;
	return(GetTransform(result));
}

inline Vector3 CSticky::GetVelocity(void) const
{
	Vector3 result;
	return(GetVelocity(result));
}

inline Vector3 CSticky::GetAngularVelocity(void) const
{
	Vector3 result;
	return(GetAngularVelocity(result));
}

#endif // STICKY_H
