/*
**
**  File:   lod.h
**  Date:   April 18, 2000
**  By:     Bryant Collard
**  Desc:   Classes used to determine level of detail.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

#ifndef LOD_H
#define LOD_H

#include "EngineHelper/Singleton.h"

class Graphics4x4;

// The enums are entries into the d_distance table, which is filled in with
// distance values from the Data\Var\ActorLOD.var table, read by
// CCameraLOD::LoadTable. Settings for juju as of this writing are:
//
// d_scale == ActorLOD.LODScale == 100.0000
//
// d_distance[FOCUS]     = ActorLOD.ONCAMERA     =  0.0500 =   5 meters
// d_distance[HIGH]      = ActorLOD.NEARCAMERA   =  0.1000 =  10 meters
// d_distance[MEDIUM]    = ActorLOD.MEDIUMCAMERA =  0.2500 =  25 meters
// d_distance[LOW]       = ActorLOD.FARCAMERA    =  0.5000 =  50 meters
// d_distance[PERIPHERY] = ActorLOD.OFFCAMERA    = -0.0500 =  -5 meters
// d_distance[ABSENT] is hard-wired to -0.1f               = -10 meters
//
// The camera LOD stuff is for rendering-- the distance used to calculate the
// lod is the distance to the camera plane, which is positive if the object in
// question is in front of the camera and negative behind. For example, the
// ABSENT LOD is for when the object is some distance behind the camera
// (negative distance)

class CBaseLOD
{
  public:
	// Order is important
	enum ELOD
	{
		FOCUS,
		HIGH,
		MEDIUM,
		LOW,
		PERIPHERY,
		ABSENT,
		NUM_LODS
	};
	virtual ~CBaseLOD() {}
	virtual ELOD GetLOD(Vector3CRef i_subjectInWorld) = 0;
};

#define MAX_LOD_VIEWPORTS 4

class CCameraLOD : public CBaseLOD
{
  public:
	CCameraLOD();
	CCameraLOD(Graphics4x4* i_worldToCamera, char* i_fileName = NULL);
	void Initialize(Graphics4x4* i_worldToCamera, char* i_fileName = NULL);
	void SetCamera(Graphics4x4* i_worldToCamera)
		{SetCameraN(0, i_worldToCamera);}
	void SetCamera2(Graphics4x4* i_worldToCamera)
		{SetCameraN(1, i_worldToCamera);}
	void SetCameraN(int camera, Graphics4x4* i_worldToCamera);
	void LoadTable(char* i_fileName);
	virtual ELOD GetLOD(Vector3CRef i_subjectInWorld);
	virtual float GetDistanceToCameraSquared(Vector3CRef i_subjectInWorld);
	float GetLODDistance(ELOD lod) { return d_distance[lod]; }
	 
  private:
	float d_scale;
	float d_distance[NUM_LODS];
	float d_distanceSquared[NUM_LODS];
	int d_highCameraIndex;
	Graphics4x4* d_worldToCamera[MAX_LOD_VIEWPORTS];

	CBaseLOD::ELOD ComputeLOD(Vector3CRef i_subjectInWorld,
			Graphics4x4 &i_camera);
};

// A global for accessing this bad-boy
DefineSingleton(CCameraLOD)
#define g_cameraLOD GetSingleton(CCameraLOD)

#endif // LOD_H
