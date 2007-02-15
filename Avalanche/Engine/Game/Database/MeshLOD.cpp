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
**   $Archive: /Avalanche/EngineNew/Game/Database/MeshLOD.cpp $
**  $Revision: 9 $
**      $Date: 6/11/03 2:31p $
**    $Author: Tyler Colbert $
**
*/

/******************* includes ****************************/

#include "Game/GamePCH.h"
#include "cmdcon/cmdcon.h"





// MeshLOD Singleton
CMeshLOD g_defaultMeshLOD;

CMeshLOD::CMeshLOD()
{
	d_numCamera = 0;
	d_worldToCamera = NULL;
}

CMeshLOD::~CMeshLOD()
{
	Terminate();
}

void CMeshLOD::Initialize(int i_numCamera, char* i_fileName)
{
	delete[] d_worldToCamera;

	if (i_numCamera <= 0)
	{
		d_numCamera = 0;
		d_worldToCamera = NULL;
	}

	d_numCamera = i_numCamera;
	d_worldToCamera = new Graphics4x4*[d_numCamera];
	for (int i = 0; i < d_numCamera; i++)
		d_worldToCamera[i] = NULL;

	LoadTable(i_fileName);
}

void CMeshLOD::Terminate(void)
{
	delete[] d_worldToCamera;
	d_numCamera = 0;
	d_worldToCamera = NULL;
}

void CMeshLOD::SetCamera(int i_camera, Graphics4x4* i_worldToCamera)
{
	ASSERT((i_camera >=0) && (i_camera < d_numCamera));
	d_worldToCamera[i_camera] = i_worldToCamera;
}

void CMeshLOD::LoadTable(char* i_fileName)
{
	d_inSceneDistSq[FOCUS] = 100.0f;      // 10 m
	d_inSceneDistSq[HIGH] = 400.0f;       // 20 m
	d_inSceneDistSq[MEDIUM] = 900.0f;     // 30 m
	d_peripheryAcceptDistSq = 100.0f;     // 10 m
	d_peripheryRejectDistSq = 2500.0f;    // 500 m
	d_peripheryRejectCosAngleSq = 0.42f;  // 100 deg fov

	char *context = i_fileName;
	g_console.DeleteContext(context);
	g_console.BindScript(context, i_fileName);

	g_console.CreateVar(context, "FocusDist", d_inSceneDistSq[FOCUS], "real",
			Math::Square, Math::Sqrt);
	g_console.CreateVar(context, "HighDist", d_inSceneDistSq[HIGH], "real",
			Math::Square, Math::Sqrt);
	g_console.CreateVar(context, "MediumDist", d_inSceneDistSq[MEDIUM], "real",
			Math::Square, Math::Sqrt);
	g_console.CreateVar(context, "PeripheryAcceptDist", d_peripheryAcceptDistSq,
			"real", Math::Square, Math::Sqrt);
	g_console.CreateVar(context, "PeripheryRejectDist", d_peripheryRejectDistSq,
			"real", Math::Square, Math::Sqrt);
	g_console.CreateVar(context, "PeripheryRejectAngle",
			d_peripheryRejectCosAngleSq, "deg", CosDegSq, ArcCosDegSqrt);

	g_console.ExecuteContext(context); 
#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(context);
#endif
}

CBaseLOD::ELOD CMeshLOD::GetMeshLOD(Vector3CRef i_subjectInWorld, bool i_inScene)
{
	if (i_inScene)
	{
		float minDistSq = MAXFLOAT;
		for (int i = 0; i < d_numCamera; i++)
		{
			if (d_worldToCamera[i])
			{
				float distSq =
						(i_subjectInWorld * (*d_worldToCamera[i])).LengthSquared();
				if (distSq < minDistSq)
					minDistSq = distSq;
			}
		}

		for (int lod = 0; lod < (PERIPHERY - 1); lod++)
		{
			if (minDistSq < d_inSceneDistSq[lod])
				return((ELOD)lod);
		}
		return((ELOD)(PERIPHERY - 1));
	}

	Vector3 point;
	for (int i = 0; i < d_numCamera; i++)
	{
		if (d_worldToCamera[i])
		{
			point = i_subjectInWorld * (*d_worldToCamera[i]);
			float distSq = point.LengthSquared();
			if ((distSq < d_peripheryAcceptDistSq) ||
					((distSq < d_peripheryRejectDistSq) && (point.z() > 0.0f) &&
					((point.z() * point.z() / distSq) > d_peripheryRejectCosAngleSq)))
				return(PERIPHERY);
		}
	}

	return(ABSENT);
}

float CMeshLOD::CosDegSq(float i_angle)
{
	return(Math::Square(Math::CosDeg(i_angle)));
}

float CMeshLOD::ArcCosDegSqrt(float i_cosDegSq)
{
	return(Math::ArcCosDeg(Math::Sqrt(i_cosDegSq)));
}
