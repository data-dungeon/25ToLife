/*
**
**  File:   particle.h
**  Date:   March 3, 2004
**  Desc:   Temporary particle manager used to ease transition to message
**          system.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Environ/partmsg.h $
**  $Revision: #1 $
**  $DateTime: 2004/09/01 15:30:39 $
**    $Author: Food $
**
*/

#pragma once

#ifndef PARTICLE_H
#define PARTICLE_H

class SParticleEffect
{
public:
	SParticleEffect()
		{ d_normal.Clear(); d_scale = 1.0f; }

	const char *d_name;
	Vector3		d_pos, d_normal;
	float			d_scale;
};


#endif // MUNITION_H
