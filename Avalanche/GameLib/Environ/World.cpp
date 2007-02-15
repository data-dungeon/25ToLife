/*
**
**  File:   World.cpp
**  Date:   March 2, 2004
**  By:     Bryant Collard
**  Desc:   An entity to manage consistant view of world state.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Precompiled header
#include "Environ/EnvironPCH.h"

// My header (always first)
#include "Environ/World.h"

// Gamelib includes
#include "Environ/Munition.h"

// Declare singleton
DeclareSingleton(CWorld);

//////////////////////////////////// CWorld ////////////////////////////////////

/******************************************************************************/
/******************************************************************************/
CWorld::CWorld(bool &i_set, float &i_centerX, float &i_centerY,
		float &i_centerZ, float &i_extentX, float &i_extentY, float &i_extentZ) :
	d_set(i_set),
	d_centerX(i_centerX),
	d_centerY(i_centerY),
	d_centerZ(i_centerZ),
	d_extentX(i_extentX),
	d_extentY(i_extentY),
	d_extentZ(i_extentZ),
	d_messageOwner("World")
{
	ClearExtents();
}

/******************************************************************************/
/******************************************************************************/
void CWorld::Initialize(void)
{
	d_stationarySound.Initialize("PlayStationarySound", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS, d_messageOwner);

	d_attachedSound.Initialize("PlayAttachedSound", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS, d_messageOwner);

	d_particleEffect.Initialize("ParticleEffect", MESSAGE_HANDLE_IF_MASTER |
			MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS, d_messageOwner);
}

/******************************************************************************/
/******************************************************************************/
void CWorld::Update(void)
{
	d_messageOwner.HandlePostedMessages();
}

/******************************************************************************/
/******************************************************************************/
void CWorld::CStationarySoundHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	SStationarySound* msg = (SStationarySound*)i_data;
	Sfx::PlayStationary(msg->d_name, msg->d_pos, msg->d_volume, msg->d_pitch);
}

/******************************************************************************/
/******************************************************************************/
void CWorld::CAttachedSoundHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	SAttachedSound* msg = (SAttachedSound*)i_data;
	if (msg->d_actor != NULL)
		Sfx::Play(msg->d_name, *msg->d_actor, Sfx::ACTORPLAYPOS_DEFAULT, msg->d_volume, msg->d_pitch);
}

/******************************************************************************/
/******************************************************************************/
void CWorld::CParticleEffectHandler::HandleMessage(void* i_data,
		ActorHandle i_sender, bool i_posted)
{
	SParticleEffect* msg = (SParticleEffect*)i_data;
	bool useNormal = !Math::Zero(msg->d_normal.LengthSquared());
	if (useNormal)
	{
		ParticleMgr__CreateEffect(msg->d_name, &msg->d_pos, &msg->d_normal, NULL,
				NULL, 0.0f, msg->d_scale, 0);
	}
	else
	{
		ParticleMgr__CreateEffect(msg->d_name, &msg->d_pos, NULL, NULL, NULL,
				0.0f, msg->d_scale);
	}
}

/******************************************************************************/
/******************************************************************************/
void CWorld::ClearExtents(void)
{
	d_set = false;
}

/******************************************************************************/
/******************************************************************************/
void CWorld::SetExtents(Vector3CRef i_center, Vector3CRef i_extent)
{
	d_set = true;

	d_centerX = i_center.X();
	d_centerY = i_center.Y();
	d_centerZ = i_center.Z();

	d_extentX = i_extent.X();
	d_extentY = i_extent.Y();
	d_extentZ = i_extent.Z();
}

/******************************************************************************/
/******************************************************************************/
void CWorld::SetExtents(Vector3CRef i_min, Vector3CRef i_max, float i_buffer)
{
	Vector3 center;
	Vector3 extent;
	ComputeExtents(i_min, i_max, i_buffer, center, extent);
	SetExtents(center, extent);
}

/******************************************************************************/
/******************************************************************************/
void CWorld::ComputeExtents(Vector3CRef i_min, Vector3CRef i_max,
		float i_buffer, Vector3 &o_center, Vector3 &o_extent)
{
	o_center = 0.5f * (i_min + i_max);
	o_extent = 0.5f * (i_max - i_min);
	o_extent = o_extent.Abs() + i_buffer;
}

/******************************************************************************/
/******************************************************************************/
void CWorld::CompressPosition(Vector3CRef i_pos, int16 &o_x, int16 &o_y,
		int16 &o_z)
{
	ASSERT(d_set);
	if (d_set)
	{
		o_x = Math::SignedCompressToWord(i_pos.X() - d_centerX, d_extentX);
		o_y = Math::SignedCompressToWord(i_pos.Y() - d_centerY, d_extentY);
		o_z = Math::SignedCompressToWord(i_pos.Z() - d_centerZ, d_extentZ);
	}
	else
	{
		o_x = 0;
		o_y = 0;
		o_z = 0;
	}
}

/******************************************************************************/
/******************************************************************************/
int16 CWorld::CompressPositionX(float i_uncompressed)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::SignedCompressToWord(i_uncompressed - d_centerX, d_extentX));
	return(0);
}

/******************************************************************************/
/******************************************************************************/
int16 CWorld::CompressPositionY(float i_uncompressed)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::SignedCompressToWord(i_uncompressed - d_centerY, d_extentY));
	return(0);
}

/******************************************************************************/
/******************************************************************************/
int16 CWorld::CompressPositionZ(float i_uncompressed)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::SignedCompressToWord(i_uncompressed - d_centerZ, d_extentZ));
	return(0);
}

/******************************************************************************/
/******************************************************************************/
void CWorld::DecompressPosition(int16 i_x, int16 i_y, int16 i_z, Vector3 &o_pos)
{
	ASSERT(d_set);
	if (d_set)
	{
		o_pos.Set(Math::SignedDecompress(i_x, d_extentX) + d_centerX,
				Math::SignedDecompress(i_y, d_extentY) + d_centerY,
				Math::SignedDecompress(i_z, d_extentZ) + d_centerZ);
	}
	else
		o_pos.Set(d_centerX, d_centerY, d_centerZ);
}

/******************************************************************************/
/******************************************************************************/
void CWorld::DecompressPackedPosition(int16 i_x, int16 i_y, int16 i_z,
		Vector3Packed &o_pos)
{
	ASSERT(d_set);
	if (d_set)
	{
		o_pos.Set(Math::SignedDecompress(i_x, d_extentX) + d_centerX,
				Math::SignedDecompress(i_y, d_extentY) + d_centerY,
				Math::SignedDecompress(i_z, d_extentZ) + d_centerZ);
	}
	else
		o_pos.Set(d_centerX, d_centerY, d_centerZ);
}

/******************************************************************************/
/******************************************************************************/
float CWorld::DecompressPositionX(int16 i_compressed)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::SignedDecompress(i_compressed, d_extentX) + d_centerX);
	return(d_centerX);
}

/******************************************************************************/
/******************************************************************************/
float CWorld::DecompressPositionY(int16 i_compressed)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::SignedDecompress(i_compressed, d_extentY) + d_centerY);
	return(d_centerY);
}

/******************************************************************************/
/******************************************************************************/
float CWorld::DecompressPositionZ(int16 i_compressed)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::SignedDecompress(i_compressed, d_extentZ) + d_centerZ);
	return(d_centerZ);
}

/******************************************************************************/
/******************************************************************************/
Vector3 CWorld::ClampPosition(Vector3CRef i_pos)
{
	Vector3 result;
	ASSERT(d_set);
	if (d_set)
	{
		result.Set(
				Math::Clamp(i_pos.X() - d_centerX, -d_extentX, d_extentX) + d_centerX,
				Math::Clamp(i_pos.Y() - d_centerY, -d_extentY, d_extentY) + d_centerY,
				Math::Clamp(i_pos.Z() - d_centerZ, -d_extentZ, d_extentZ) + d_centerZ);
	}
	else
		result.Clear();
	return(result);
}

/******************************************************************************/
/******************************************************************************/
float CWorld::ClampPositionX(float i_pos)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::Clamp(i_pos - d_centerX, -d_extentX, d_extentX) + d_centerX);
	return(0.0f);
}

/******************************************************************************/
/******************************************************************************/
float CWorld::ClampPositionY(float i_pos)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::Clamp(i_pos - d_centerY, -d_extentY, d_extentY) + d_centerY);
	return(0.0f);
}

/******************************************************************************/
/******************************************************************************/
float CWorld::ClampPositionZ(float i_pos)
{
	ASSERT(d_set);
	if (d_set)
		return(Math::Clamp(i_pos - d_centerZ, -d_extentZ, d_extentZ) + d_centerZ);
	return(0.0f);
}

/////////////////////////////// CStandAloneWorld ///////////////////////////////

/******************************************************************************/
/******************************************************************************/
CStandAloneWorld::CStandAloneWorld() : CWorld(d_standAloneSet,
		d_standAloneCenterX, d_standAloneCenterY, d_standAloneCenterZ,
		d_standAloneExtentX, d_standAloneExtentY, d_standAloneExtentZ)
{
}
