
#pragma once

#ifndef __IMM_WORLDTOVIEWCONTROLLER_H__
#define __IMM_WORLDTOVIEWCONTROLLER_H__

#include "Camera/world2view.h"
#include "Layers/Semaphore/Semaphore.h"
namespace IMM
{
	class WorldToViewController : public ::WorldToViewController
	{
		Vector3 mEye, mFocus, mUp;
		CSemaphore mUpdateSemaphore;

	public:
		WorldToViewController() :
			mEye  (  0.0f,  0.0f,  0.0f ),
			mFocus(  0.0f,  0.0f, -1.0f ),
			mUp   (  0.0f,  1.0f,  0.0f ),
			mUpdateSemaphore( true )
		{
		}

		// Eye position
		virtual Vector3 Eye()
		{
			Vector3 eye;
			mUpdateSemaphore.AquireUnconditionally();
			eye = mEye;
			mUpdateSemaphore.Release();
			return eye;
		}
		// Focal point
		virtual Vector3 FocalPoint()
		{
			Vector3 focus;
			mUpdateSemaphore.AquireUnconditionally();
			focus = mFocus;
			mUpdateSemaphore.Release();
			return focus;
		}
		// Up vector (if coincident with direction vector, weird things will happen)
		virtual Vector3 Up()
		{
			Vector3 up;
			mUpdateSemaphore.AquireUnconditionally();
			up = mUp;
			mUpdateSemaphore.Release();
			return up;
		}

		void Update( Vector3CRef eye, Vector3CRef focus, Vector3CRef up )
		{
			mUpdateSemaphore.AquireUnconditionally();
			mEye = eye;
			mFocus = focus;
			mUp = up;
			mUpdateSemaphore.Release();
		}
	};
}


#endif //__IMM_WORLDTOVIEWCONTROLLER_H__

