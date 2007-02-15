/*
**
**  File:   LoopScenes.h
**  Date:   April 2, 2004
**  By:     Bryant Collard
**  Desc:   Manage scenes.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/Loop/LoopScenes.h $
**  $Revision: #2 $
**  $DateTime: 2004/04/30 16:14:42 $
**    $Author: Food $
**
*/

#pragma once

#ifndef LOOP_SCENES_H
#define LOOP_SCENES_H

#include "GameHelper/lod.h"

class CameraScene;

class CLoopScenes
{
  public:
	// Construct/Destruct
	CLoopScenes();
	~CLoopScenes();

	// Main management functions.
	void Initialize(int i_numScenes);
	void Run(void);
	void Terminate(void);

	// Accessors.
	int NumScenes(void) {return(d_numScenes);}
	inline CameraScene* Scene(int i_scene);

	// Functions to attach a free camera to all the scenes.
	void CreateFreeCameras(void);
	void DeleteFreeCameras(void);
	bool ActivateFreeCamera(int i_scene);

  private:
	int d_numScenes;
	CameraScene** d_scene;
	CCameraLOD d_cameraLOD;
};

inline CameraScene* CLoopScenes::Scene(int i_scene)
{
	if ((i_scene < 0) || (i_scene >= d_numScenes))
		return(NULL);
	return(d_scene[i_scene]);
}

#endif // LOOP_SCENES_H
