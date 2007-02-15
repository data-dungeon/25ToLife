///////////////////////////////////////////////////////////////////////////////
// The world2view component is traditionally what is thought as the "camera".
// It defines the matrix that takes points in world space and places them into
// the view.
//
// It would be nice here if I stated which axis the view looks down, and
// whether or not the coordinate system is right or left handed, but I don't
// know yet...as the engine is seemingly purposely obscure on this point.
///////////////////////////////////////////////////////////////////////////////
#ifndef WORLD2VIEW_H
#define WORLD2VIEW_H

class Camera;

//////////////////////////////////////////////////////////////////////////
// The world to view controller sets up where the eye and focal point are
// It also must provide a "reasonable" up vector, that does not have to be
// perpendicular to the direction vector
//////////////////////////////////////////////////////////////////////////
class WorldToViewController
{
public:
	WorldToViewController() {}
	virtual ~WorldToViewController() {}

	// Update 
	virtual void Update(Camera &parent) {}

	// Eye position
	virtual Vector3 Eye() = 0;

	// Focal point
	virtual Vector3 FocalPoint() = 0;

	// Up vector (if coincident with direction vector, weird things will happen)
	virtual Vector3 Up() = 0;
};

#endif
