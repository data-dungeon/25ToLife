/*
**
**  File:   lod.cpp
**  Date:   April 18, 2000
**  By:     Bryant Collard
**  Desc:   Classes used to determine level of detail.
**
**  Copyright (c) 2000, Avalanche Software Inc.
**  All Rights Reserved.
**
*/

// Pre-compiled header
#include "GameHelper/GameHelperPCH.h"
#include "cmdcon/cmdcon.h"

// A global for accessing this bad-boy
DeclareSingleton(CCameraLOD);

/***************************************************************************/
/***************************************************************************/
CCameraLOD::CCameraLOD()
{
	int i;
	for (i = 0; i < MAX_LOD_VIEWPORTS; i++)
		d_worldToCamera[i] = NULL;
	d_highCameraIndex = 0;
	d_scale = 100.0f;
	for (i = 0; i < NUM_LODS; i++)
		d_distance[i] = 0.0f;
}

/***************************************************************************/
/***************************************************************************/
CCameraLOD::CCameraLOD(Graphics4x4* i_worldToCamera, char* i_fileName)
{
	int i;
	for (i = 0; i < MAX_LOD_VIEWPORTS; i++)
		d_worldToCamera[i] = NULL;
	d_highCameraIndex = 0;
	d_scale = 100.0f;
	for (i = 0; i < NUM_LODS; i++)
		d_distance[i] = 0.0f;

	Initialize(i_worldToCamera, i_fileName);
}

/***************************************************************************/
/***************************************************************************/
void CCameraLOD::Initialize(Graphics4x4* i_worldToCamera, char* i_fileName)
{
	SetCamera(i_worldToCamera);
	LoadTable(i_fileName);
}

/***************************************************************************/
/***************************************************************************/
void CCameraLOD::SetCameraN(int i_camera, Graphics4x4* i_worldToCamera)
{
	ASSERT((i_camera >= 0) && (i_camera < MAX_LOD_VIEWPORTS));
	d_worldToCamera[i_camera] = i_worldToCamera;

	// Reset high index
	 d_highCameraIndex = MAX_LOD_VIEWPORTS;
	 while ((d_highCameraIndex > 0) &&
			 (d_worldToCamera[d_highCameraIndex - 1] == NULL))
		 d_highCameraIndex--;
}

/***************************************************************************/
// These values are typically loaded from the file ActorLOD.var
/***************************************************************************/
void CCameraLOD::LoadTable(char* i_fileName)
{
	d_distance[ABSENT] = -0.1f;

	char *context = i_fileName;
	g_console.BindScript(context, i_fileName);
	g_console.CreateVar(context, "LODScale", d_scale);
	g_console.CreateVar(context, "ONCAMERA", d_distance[FOCUS]);
	g_console.CreateVar(context, "NEARCAMERA", d_distance[HIGH]);
	g_console.CreateVar(context, "MEDIUMCAMERA", d_distance[MEDIUM]);
	g_console.CreateVar(context, "FARCAMERA", d_distance[LOW]);
	g_console.CreateVar(context, "OFFCAMERA", d_distance[PERIPHERY]);

	g_console.ExecuteContext(context); 

	// Baking the scale in and precomputing distance squared pretty much broke
	// run-time modification of the values. So, clean up here instead of coming
	// up with a mechanism to clean up later. Sigh.
	g_console.DeleteContext(context);

	// Bake scale into distance array, initialize distance-squared array.
	for (int i = 0; i < NUM_LODS; i++)
	{
		d_distance[i] *= d_scale;
		d_distanceSquared[i] = d_distance[i] * d_distance[i];
	}
}

/***************************************************************************/
/***************************************************************************/
CBaseLOD::ELOD CCameraLOD::ComputeLOD(Vector3CRef i_subjectInWorld,
		Graphics4x4 &i_camera)
{
	// Calculate distance to camera plane. Distance < 0 means behind camera
	// plane. If far enough behind plane, return ABSENT (which is the lowest
	// level of detail).
	Vector3 subjectInCamera = i_subjectInWorld * i_camera;
	float distanceToCameraPlane = -subjectInCamera.z();
	if (distanceToCameraPlane < d_distance[ABSENT])
		return(ABSENT);

	// Search for the lod starting with the closest.
	float distanceToCameraSquared = subjectInCamera.LengthSquared();
	for (int i = 0; i < NUM_LODS - 2; i++)
	{
		if (distanceToCameraSquared < d_distanceSquared[i])
			return((ELOD)i);
	}

	// The subject is in front of the camera but beyond the furthest lod.
	return(PERIPHERY);
}

/***************************************************************************/
/***************************************************************************/
CBaseLOD::ELOD CCameraLOD::GetLOD(Vector3CRef i_subjectInWorld)
{
	ELOD bestLOD = ABSENT;
	for (int i = 0; i < d_highCameraIndex; i++)
   {
		// valid?
		if (!d_worldToCamera[i])
			continue;

		// Get the LOD based on this camera
		ELOD lod = ComputeLOD(i_subjectInWorld, *d_worldToCamera[i]);

		// Is it "better" than the current LOD
		if (lod < bestLOD)
			bestLOD = lod;
	}

   return(bestLOD);
}

/***************************************************************************/
/***************************************************************************/
float CCameraLOD::GetDistanceToCameraSquared(Vector3CRef i_subjectInWorld)
{
	float bestDistanceSquared = MAXFLOAT32;
	for (int i = 0; i < d_highCameraIndex; i++)
   {
		// valid?
		if (!d_worldToCamera[i])
			continue;

		// Get distance to this camera
		Vector3 subjectInCamera = i_subjectInWorld * *d_worldToCamera[i];
		float distanceSquared = subjectInCamera.LengthSquared();

		// Is it less than current distance?
		if (distanceSquared < bestDistanceSquared)
			bestDistanceSquared = distanceSquared;
	}

   return(bestDistanceSquared);
}
