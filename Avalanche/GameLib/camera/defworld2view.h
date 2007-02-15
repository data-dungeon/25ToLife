///////////////////////////////////////////////////////////////////////////////
// The world2view component is traditionally what is thought as the "camera".
// It defines the matrix that takes points in world space and places them into
// the view.
//
// It would be nice here if I stated which axis the view looks down, and
// whether or not the coordinate system is right or left handed, but I don't
// know yet...as the engine is seemingly purposely obscure on this point.
///////////////////////////////////////////////////////////////////////////////
#ifndef DEFWORLD2VIEW_H
#define DEFWORLD2VIEW_H

#include "camera/world2view.h"

//////////////////////////////////////////////////////////////////////////
// A simple default world to view
//////////////////////////////////////////////////////////////////////////
class DefaultWorldToViewController : public WorldToViewController
{
public:
	DefaultWorldToViewController() {}
	~DefaultWorldToViewController() {}

	// Eye position
	Vector3 Eye()
		{ return Vector3(0.0f, -10000.0f, 0.0f); }

	// Focal point
	Vector3 FocalPoint()
		{ return Vector3(0.0f, -11000.0f, 0.0f); }

	// Up vector (if coincident with direction vector, weird things will happen)
	Vector3 Up()
		{ return Vector3(1.0f, 0.0f, 0.0f); }
};

#endif
