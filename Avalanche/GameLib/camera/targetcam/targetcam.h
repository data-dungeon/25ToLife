///////////////////////////////////////////////////////////////////////////////
// A simple target cam
///////////////////////////////////////////////////////////////////////////////
#ifndef TARGETCAM_H
#define TARGETCAM_H

#include "camera/util/flextarg.h"
#include "camera/camera.h"

class TargetWTVC : public FlexTargetW2VC
{
public:
	TargetWTVC();
	~TargetWTVC() {}

	// Setup the parameters
	void Setup(Vector3 Pos, Vector3 Target);

  	// Update 
	void Update(Camera &parent);

	// Focal point
	Vector3 FocalPoint()
		{ return m_focalPoint; }

  // Eye position
	Vector3 Eye()
		{ return m_eye; }

private:
	Vector3 		m_eye;
	Vector3		m_focalPoint;
	Vector3		m_Pos;
	Vector3		m_Target;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class TargetCamera : public Camera
{
public:
	// Cameras must have at least a name and w2v controller
	TargetCamera(const char *name,
					ProjectionController *projectionController = NULL,
					ViewportController *viewportController = NULL);

	// Set the parameters
	void SetTarget(Vector3 *target)
		{ m_target.SetTarget(target); }
	void SetTarget(CActor *target)
		{ m_target.SetTarget(target); }
	void SetTargetOffset(const Vector3 offset)
		{ m_target.SetOffset(offset); }

	void Setup(Vector3 Pos, Vector3 Target)
		{ m_target.Setup(Pos, Target); }

protected:
	// Wee!
	TargetWTVC	m_target;
};

#endif
