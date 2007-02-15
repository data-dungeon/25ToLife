/*
**
**  File:   MeshLOD.h
**  Date:   April 25, 2002
**  By:     Bryant Collard
**  Desc:   LOD object for meshes.
**
**  Copyright (c) 2002, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshLOD.h $
**  $Revision: 4 $
**      $Date: 6/11/03 2:31p $
**    $Author: Tyler Colbert $
**
*/

#ifndef MESH_LOD_H
#define MESH_LOD_H

// GameLib includes.
#include "GameHelper/lod.h"

#include "Math/Vector.h"
class Graphics4x4;

class CMeshLOD : public CBaseLOD
{
  public:
	CMeshLOD();
	~CMeshLOD();
	void Initialize(int i_numCamera = 1, char* i_fileName = NULL);
	void Terminate(void);
	void SetCamera(int i_camera, Graphics4x4* i_worldToCamera);
	void LoadTable(char* i_fileName);
	virtual ELOD GetLOD(Vector3CRef i_subjectInWorld)
		{return(GetMeshLOD(i_subjectInWorld, true));}
	ELOD GetMeshLOD(Vector3CRef i_subjectInWorld, bool i_inScene = true);
  private:
	int d_numCamera;
	Graphics4x4** d_worldToCamera;
	float d_inSceneDistSq[PERIPHERY - 1];
	float d_peripheryAcceptDistSq;
	float d_peripheryRejectDistSq;
	float d_peripheryRejectCosAngleSq;

	static float CosDegSq(float i_angle);
	static float ArcCosDegSqrt(float i_cosDegSq);
};

// MeshLOD Singleton
extern CMeshLOD g_defaultMeshLOD;

#endif // MESH_LOD_H
