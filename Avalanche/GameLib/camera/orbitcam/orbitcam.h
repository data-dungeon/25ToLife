///////////////////////////////////////////////////////////////////////////////
// A simple orbit cam (proof of concept)
///////////////////////////////////////////////////////////////////////////////
#ifndef ORBITCAM_H
#define ORBITCAM_H

#include "camera/util/flextarg.h"
#include "camera/camera.h"

class OrbitWTVC : public FlexTargetW2VC
{
public:
	OrbitWTVC();
	~OrbitWTVC() {}

	// Setup the parameters
	void Setup(float distance, float height, float rotationSpeed);

  	// Update 
	void Update(Camera &parent);

  // Eye position
	Vector3 Eye()
		{ return m_eye; }

private:
	Vector3 		m_eye;

	float			m_distance;
	float			m_height;
	float			m_rotationSpeed;

	float			m_angle;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class OrbitCamera : public Camera
{
public:
	// Cameras must have at least a name and w2v controller
	OrbitCamera(const char *name,
					ProjectionController *projectionController = NULL,
					ViewportController *viewportController = NULL);

	// Set the parameters
	void SetTarget(Vector3 *target)
		{ m_orbit.SetTarget(target); }
	void SetTarget(CActor *target, bool useFocus = false)
		{ m_orbit.SetTarget(target, useFocus); }
	void SetTargetOffset(const Vector3 offset)
		{ m_orbit.SetOffset(offset); }

	void Setup(float distance, float height, float rotationSpeed)
		{ m_orbit.Setup(distance, height, rotationSpeed); }

protected:
	// Wee!
	OrbitWTVC	m_orbit;
};

#endif
