/*
**
**  File:   WaterWake.cpp
**  Date:   June 8, 2005
**  By:     Bryant Collard
**  Desc:   Generate wakes when dragging through water.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/TTL/code/components/WaterWake.cpp $
**  $Revision: #1 $
**  $DateTime: 2005/06/08 21:24:46 $
**    $Author: Bryant_Collard $
**
*/

// Precompiled header.
#include "TTLPCH.h"

// My header
#include "components/WaterWake.h"

// GameLib headers
#include "Components/ComponentCreator.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CWaterWake, WaterWake, COMPONENT_NEUTRAL_PRIORITY);

// All emitters use the same up vector.
static Vector3 l_up(0.0f, 1.0f, 0.0f);

// Half-width of center.
#define CENTER_HALF_WIDTH  0.1f

/******************************************************************************/
/******************************************************************************/
CWaterWake::CWaterWake(CCompActor &i_actor) : CActorComponent(i_actor)
{
	for (int i = 0; i < NUM_LOCATIONS; i++)
	{
		d_emitter[i] = INVALID_HANDLE;
		d_active[i] = false;
	}
}

/******************************************************************************/
/******************************************************************************/
CWaterWake::~CWaterWake()
{
	Destroy();
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::Create(const char* i_name)
{
	// Make sure there is a valid emitter name.
	if ((i_name == NULL) || (i_name[0] == '\0'))
		return;

	// Clean up any existing emitters.
	Destroy();

	// Create emitters.
	for (int i = 0; i < NUM_LOCATIONS; i++)
	{
		d_emitter[i] = ParticleEmitter__CreateFromRecord(i_name, &d_pos[i],
				&actor.GetVelocityInWorld(), NULL, false, &l_up, false);
		Deactivate(i);
	}
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::Destroy(void)
{
	for (int i = 0; i < NUM_LOCATIONS; i++)
	{
		if (d_emitter[i] != INVALID_HANDLE)
		{
			ParticleEmitter__Kill(d_emitter[i]);
			d_emitter[i] = INVALID_HANDLE;
			d_active[i] = false;
		}
	}
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::Activate(int i_index)
{
	if (d_emitter[i_index] != INVALID_HANDLE)
	{
		ParticleEmitter__Activate(d_emitter[i_index]);
		d_active[i_index] = true;
	}
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::Deactivate(int i_index)
{
	if (d_emitter[i_index] != INVALID_HANDLE)
	{
		ParticleEmitter__Deactivate(d_emitter[i_index]);
		d_active[i_index] = false;
	}
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::BeginUpdate(void)
{
	for (int i = 0; i < NUM_LOCATIONS; i++)
	{
		d_pos[i].Clear();
		d_count[i] = 0;
	}
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::EndUpdate(float i_deltaSec)
{
	for (int i = 0; i < NUM_LOCATIONS; i++)
	{
		if (d_count[i] > 0)
		{
			d_pos[i] *= 1.0f / (float)d_count[i];
			if (!d_active[i])
				Activate(i);
		}
		else if (d_active[i])
			Deactivate(i);
	}
}

/******************************************************************************/
/******************************************************************************/
void CWaterWake::CollisionAccept(CStageEntity* i_entity,
		CCollisionEvent* i_event)
{
	if (i_entity->Category().PositiveMatch(WATER_CATEGORY))
	{
		bool waterIsQuery = (i_entity == i_event->GetQueryEntity());
		CCollisionPair* pair = i_event->GetHeadPair();
		while (pair != NULL)
		{
			if (pair->GetIntersectionType() == CCollisionIntersection::OBB_TO_OBB)
			{
				Vector3 center(0.0f, 0.0f, 0.0f);
				int count = 0;
				CObbToObbIntersection* intersection =
						(CObbToObbIntersection*)pair->GetIntersection();
				uint16 selectedFaceMask = intersection->SelectFaces(
						waterIsQuery, Vector3(0.0f, 1.0f, 0.0f), 0.98f);
				int numPoints = intersection->EdgeIntersections();
				for (int i = 0; i < numPoints; i++)
				{
					const Vector3* point = intersection->EdgeIntersection(i,
							selectedFaceMask);
					if (point != NULL)
					{
						center += *point;
						count++;
					}
				}
				if (count > 0)
				{
					center *= 1.0f / (float)count;
					Vector3 centerInActor = (center - actor.GetBodyInWorld()) /
							actor.GetBodyToWorld();
					int location = (int)LOCATE_CENTER;
					if (centerInActor.X() > CENTER_HALF_WIDTH)
						location = (int)LOCATE_LEFT;
					else if (centerInActor.X() < -CENTER_HALF_WIDTH)
						location = (int)LOCATE_RIGHT;
					d_pos[location] += center;
					d_count[location]++;
				}
			}
			pair = pair->GetNext();
		}
	}
}
