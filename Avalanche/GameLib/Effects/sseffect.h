/////////////////////////////////////////////////////////////////////////////
// Screen Space Effect class
//
// Provides some basic functionality for doing stuff in screen space
// with sprites.
/////////////////////////////////////////////////////////////////////////////
#ifndef SSEFFECT_H
#define SSEFFECT_H

#include "Math/Matrix.h"
class CameraScene;

class ScreenSpaceEffect
{
public:
	ScreenSpaceEffect(void) {}
	virtual ~ScreenSpaceEffect(void) {}

	// Setup the tranform data
	static bool SetTransform(float fovy, float aspect, float zNear, float zFar);

	// Draw
	virtual void Update(CameraScene *scene = NULL) {}

protected:
	// Stuff for the screen
	static Matrix4x4		perspective;
	static float			aspect;
	static float			hither, yon;
	static float			invHitherYonDelta;

	// Transform using a clip factor (making the cliping region larger/smaller than the screen)
	inline static bool Transform(const Matrix4x4 &transform, Vector3CRef p, Vector2 &s, const Vector2 &clipFactor);

	// Transform normally
	inline static bool Transform(const Matrix4x4 &transform, Vector3CRef p, Vector2 &s, float &w);

	// Build a perspective matrix that mimics the engine
	static void PerspectiveMatrix(float fovy, float aspect, float zNear, float zFar, Matrix4x4 &perspective);

public:
	// We are in a doubly linked list
	ScreenSpaceEffect		*prev, *next;
};

// Include the inline implementation
#include "Effects/sseffect.hpp"

#endif
