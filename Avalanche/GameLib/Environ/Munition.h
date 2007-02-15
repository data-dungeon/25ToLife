/*
**
**  File:   Munition.h
**  Date:   March 3, 2004
**  By:     Bryant Collard
**  Desc:   Temporary munitions manager used to ease transition to message
**          system. FOOD SHOULD REPLACE!
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Environ/Munition.h $
**  $Revision: #16 $
**  $DateTime: 2005/07/20 01:20:35 $
**    $Author: Food $
**
*/

#pragma once

#ifndef MUNITION_H
#define MUNITION_H

// The munition messages
#define MUNITION_FIRED				"MunitionFired"
#define MUNITION_FIREDFX			"MunitionFiredFX"
#define MUNITION_IMPACT				"MunitionImpact"
#define MUNITION_IMPACT_ACTOR		"MunitionImpactActor"
#define MUNITION_IMPACT_ACTORFX	"MunitionImpactActorFX"
#define MUNITION_IMPACT_ACK		"MunitionImpactAck"
#define MUNITION_FLASH				"MunitionFlash"
#define MUNITION_EXPLODEFX			"MunitionExplodeFX"

class CLineQuery;

// Munition message data
class CMunitionFired
{
public:
	CMunitionFired() { d_salvoSize = 1; }

	// First shot
	bool		d_firstShot;
	bool		d_deflected;
	int		d_salvoSize;

	/// Weapon ID
	int		d_weaponID;

	// Where did the shot come from
	Vector3	d_shotOrigin;

	// The intersection
	Vector3	d_intersection;

	// Direction of shot
	inline Vector3 Direction() const
		{ return (d_intersection - d_shotOrigin).Normalized(); }
};

class CMunitionImpact : public CMunitionFired
{
public:
	CMunitionImpact();

	// The base munition type
	int		d_munitionID;

	// Who is the victim?
	CActor	*d_victim;

	uint8		d_surfaceMaterial;

	// The damage/energy
	float		d_energy;
	float		d_damage;
};

class CMunitionImpactSurface : public CMunitionImpact
{
public:
	CMunitionImpactSurface() {}

	// Set from a collision
	void SetQuery(const CLineQuery &hit);

	// Surface interaction stuff
	bool		d_decalOkay;
	uint8		d_surfaceShade;
	Vector3	d_surfaceNormal;
};

struct SMunitionFlash
{
	int		d_munitionID;
	Vector3	d_pos;
	float		d_intensity;
};

struct SMunitionExplodeFX
{
	int		d_id;
	int		d_munitionID;
	Vector3	d_pos;
	Vector3	d_dir;
};

#endif // MUNITION_H
