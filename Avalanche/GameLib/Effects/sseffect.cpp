/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

// The statics
Matrix4x4 ScreenSpaceEffect::perspective;
float ScreenSpaceEffect::aspect = 4.0f / 3.0f;
float	ScreenSpaceEffect::hither;
float ScreenSpaceEffect::yon;
float	ScreenSpaceEffect::invHitherYonDelta;

/////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////
bool ScreenSpaceEffect::SetTransform(
float fovy,
float aspectRatio,
float zNear,
float zFar)
{
	aspect = aspectRatio;


	hither = zNear;
	yon = zFar;
	invHitherYonDelta = 1.0f / (yon - hither);

	PerspectiveMatrix(fovy, aspect, zNear, zFar, perspective);

	return(true);
}

/////////////////////////////////////////////////////////////////////////////
// generate a perspective projection matrix
/////////////////////////////////////////////////////////////////////////////
void ScreenSpaceEffect::PerspectiveMatrix(
float fovy,
float aspect,
float zNear,
float zFar,
Matrix4x4 &perspective)
{
	perspective.Identity();

   fovy = Math::Deg2Rad(fovy) / 2.0f;
   float f = Math::Cos(fovy) / Math::Sin(fovy);

	// Set it up
   perspective.e00 = f / aspect;
   perspective.e11 = -f;
   perspective.e22 = (zNear + zFar) / (zNear - zFar);
   perspective.e23 = -1.0f;
   perspective.e32 = (2.0f * zNear * zFar) / (zNear - zFar);
   perspective.e33 = 0.0f;
}


