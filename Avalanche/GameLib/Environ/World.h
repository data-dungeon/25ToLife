/*
**
**  File:   World.h
**  Date:   March 2, 2004
**  By:     Bryant Collard
**  Desc:   An entity to manage consistant view of world state.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#pragma once

#ifndef WORLD_H
#define WORLD_H

// GameLib includes
#include "EngineHelper/Singleton.h"
#include "GameHelper/Message.h"
#include "Environ/partmsg.h"
#include "EngineHelper/sfx.h"
#include "Math/Vector.h"

class CWorld
{
  public:
	CWorld(bool &i_set, float &i_centerX, float &i_centerY, float &i_centerZ,
			float &i_extentX, float &i_extentY, float &i_extentZ);
	virtual ~CWorld() {}
	void Initialize(void);
	virtual void Update(void);
	CMessageOwner &GetMessageOwner(void) {return(d_messageOwner);}

	virtual void ClearExtents(void);
	virtual void SetExtents(Vector3CRef i_center, Vector3CRef i_extent);
	void SetExtents(Vector3CRef i_min, Vector3CRef i_max, float i_buffer);
	static void ComputeExtents(Vector3CRef i_min, Vector3CRef i_max,
			float i_buffer, Vector3 &o_center, Vector3 &o_extent);
	bool IsSet(void) {return(d_set);}
	void CompressPosition(Vector3CRef i_pos, int16 &o_x, int16 &o_y, int16 &o_z);
	int16 CompressPositionX(float i_uncompressed);
	int16 CompressPositionY(float i_uncompressed);
	int16 CompressPositionZ(float i_uncompressed);
	void DecompressPosition(int16 i_x, int16 i_y, int16 i_z, Vector3 &o_pos);
	void DecompressPackedPosition(int16 i_x, int16 i_y, int16 i_z,
			Vector3Packed &o_pos);
	float DecompressPositionX(int16 i_compressed);
	float DecompressPositionY(int16 i_compressed);
	float DecompressPositionZ(int16 i_compressed);
	Vector3 ClampPosition(Vector3CRef i_pos);
	float ClampPositionX(float i_pos);
	float ClampPositionY(float i_pos);
	float ClampPositionZ(float i_pos);

  protected:
	bool &d_set;
	float &d_centerX;
	float &d_centerY;
	float &d_centerZ;
	float &d_extentX;
	float &d_extentY;
	float &d_extentZ;

  private:
	CWorld();
	CMessageOwner d_messageOwner;
	SEND_MESSAGE_HANDLER(CStationarySoundHandler) d_stationarySound;
	SEND_MESSAGE_HANDLER(CAttachedSoundHandler) d_attachedSound;
	SEND_MESSAGE_HANDLER(CParticleEffectHandler) d_particleEffect;
};

class CStandAloneWorld : public CWorld
{
  public:
	CStandAloneWorld();

  protected:
	bool d_standAloneSet;
	float d_standAloneCenterX;
	float d_standAloneCenterY;
	float d_standAloneCenterZ;
	float d_standAloneExtentX;
	float d_standAloneExtentY;
	float d_standAloneExtentZ;
};

DefineSingleton(CWorld)
#define g_world GetSingleton(CWorld)

#endif // WORLD_H
