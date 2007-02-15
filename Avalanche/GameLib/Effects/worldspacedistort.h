
// HI!
// This represents a distor

#include "Effects/siggen.h"

#ifndef __WORLD_SPACE_DISTORT_H__
#define __WORLD_SPACE_DISTORT_H__


namespace WorldSpaceDistort
{
	//
	// here are the routines you call to set up visual effects for a world
	//


	// you must call init if your going to do effects.
	void Init(void);
	bool Inited();
	bool IsActive();

	// You must have a matching call to Shutdown for any call to Init
	void Shutdown(void);

	struct DistortParams
	{
		int 		slices;
		int 		rings;
		float 	aspectRatio;
		float 	innerradius;
		float 	outerradius;
		float 	attacktime;
		float 	sustaintime;
		float 	decaytime;
		bool 		useOmega;
		float 	signalScale;
		Vector4 	insideColor;
		Vector4 	midColor;
		Vector4 	outerColor;
		float 	spiralStrength;
		float 	spiralMaxAge;
		float 	spiralInitialAngle;
		float 	fixedSpiralAngle;
		float 	conicDepth;
	};

	// Add full screen distortions using this function.
	// returns a hendle to allow future deletion of the effect...
	u32 AddCircle(	Vector3CRef pos,
						Vector3CRef normal,
						const DistortParams &params,
						SigGen *generator,
						bool copyBackBuffer = false,
						bool autoDestruct = true);

	void MoveDistortion(u32 handle,
								Vector3CRef pos,
								Vector3CRef normal);

	// used to delete an effect before its demise.
	void Remove(u32 handle);

	// this is called by the engine to manage this stuff...
	void UpdateAll(Graphics4x4* worldToCamera);
};

#endif	// __WORLD_SPACE_DISTORT_H__
