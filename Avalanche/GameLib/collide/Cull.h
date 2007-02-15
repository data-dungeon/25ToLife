/*
**
**  File:   Cull.h
**  Date:   March 6, 2003
**  By:     Bryant Collard
**  Desc:   A structure used for collision culling.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Cull.h $
**  $Revision: 2 $
**      $Date: 6/11/03 2:33p $
**    $Author: Tyler Colbert $
**
*/

#ifndef COLLISION_CULL_H
#define COLLISION_CULL_H

// Engine includes //
#include "Math/Vector.h"

// Gamelib includes //
#include "collide/xzrect.h"
#include "collide/aabb.h"

class CCollisionCull
{
  public:
	enum ECullType
	{
		CULL_XZ_RECT,
		CULL_CHORD,
		CULL_AABB,
		CULL_UNDEFINED
	};

	CCollisionCull(void)
		{ d_type = CULL_UNDEFINED; d_rect.Clear(); d_rectValid = false; }

	ECullType GetType(void) {return(d_type);}

	XZRect &SelectRect(void); 
	Vector3 &SelectStart(void); 
	Vector3 &SelectChord(void);
	AABB &SelectAABB(void);

	XZRect &GetRect(void);

	XZRect &Rect(void);
	Vector3 &Start(void);
	Vector3 &Chord(void);
	AABB &Aabb(void);

  private:
	ECullType	d_type;
	bool			d_rectValid;
	XZRect		d_rect;
	Vector3		d_start;
	Vector3		d_chord;
	AABB			d_aabb;
};

///////////////////////////////////////////////////////////////////////////////
inline XZRect &CCollisionCull::SelectRect(void) 
{
	ASSERT(d_type == CULL_UNDEFINED);
	d_type = CULL_XZ_RECT; 
	return(d_rect);
}

///////////////////////////////////////////////////////////////////////////////
inline Vector3 &CCollisionCull::SelectStart(void) 
{
	ASSERT(d_type == CULL_UNDEFINED || d_type == CULL_CHORD);
	d_type = CULL_CHORD; 
	return(d_start);
}

///////////////////////////////////////////////////////////////////////////////
inline Vector3 &CCollisionCull::SelectChord(void) 
{
	ASSERT(d_type == CULL_UNDEFINED || d_type == CULL_CHORD);
	d_type = CULL_CHORD; 
	return(d_chord);
}

///////////////////////////////////////////////////////////////////////////////
inline AABB &CCollisionCull::SelectAABB(void) 
{
	ASSERT(d_type == CULL_UNDEFINED);
	d_type = CULL_AABB; 
	return(d_aabb);
}

///////////////////////////////////////////////////////////////////////////////
inline XZRect &CCollisionCull::Rect(void) 
{
	ASSERT(d_type == CULL_XZ_RECT || d_rectValid);
	return(d_rect);
}

///////////////////////////////////////////////////////////////////////////////
inline Vector3 &CCollisionCull::Start(void) 
{
	ASSERT(d_type == CULL_CHORD);
	return(d_start);
}

///////////////////////////////////////////////////////////////////////////////
inline Vector3 &CCollisionCull::Chord(void) 
{
	ASSERT(d_type == CULL_CHORD);
	return(d_chord);
}

///////////////////////////////////////////////////////////////////////////////
inline AABB &CCollisionCull::Aabb(void) 
{
	ASSERT(d_type == CULL_AABB);
	return(d_aabb);
}

///////////////////////////////////////////////////////////////////////////////
inline XZRect& CCollisionCull::GetRect(void)
{
	switch(d_type)
	{
	 case CULL_CHORD:
		d_rect.Set(d_start, d_chord);
		d_rectValid = true;
		break;
	 case CULL_AABB:
		d_rect.Set(d_aabb);
		d_rectValid = true;
		break;
	case CULL_XZ_RECT:
		break;
	default:
	case CULL_UNDEFINED:
		// Undefined type
		ASSERT(false);
		break;
	}
	return(d_rect);
}

#endif // COLLISION_CULL_H
