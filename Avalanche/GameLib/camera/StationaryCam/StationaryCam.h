///////////////////////////////////////////////////////////////////////////////
// A stationary target cam
///////////////////////////////////////////////////////////////////////////////
#ifndef STATIONARYCAM_H
#define STATIONARYCAM_H

#include "camera/util/flextarg.h"
#include "camera/camera.h"

class StationaryWTVC : public FlexTargetW2VC
{
public:
	StationaryWTVC();
	~StationaryWTVC() {}

	// Setup the parameters
	void Setup(const Vector3 &eye, const Vector3 &target);

	// Copy a target in instead of following it
	void SetLocalTarget(const Vector3 &target);

	// Eye position
	Vector3 Eye()
		{ return m_eye; }

private:
	Vector3		m_localTarget;
	Vector3 		m_eye;
};

////////////////////////////////////////////////////////////////
// Stationary projection controller
////////////////////////////////////////////////////////////////
class StationaryProjection : public DefaultProjectionController
{
public:
	StationaryProjection();
	~StationaryProjection() {}

  	// Update
	void Update(Camera &parent);
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class StationaryCamera : public Camera
{
public:
	// Cameras must have at least a name and w2v controller
//	StationaryCamera(const char *name,
//					ProjectionController *projectionController = NULL,
//					ViewportController *viewportController = NULL);

	StationaryCamera( const char *name , ViewportController *viewportC = NULL); 

	// Set the parameters
	void SetTarget(const Vector3 &target)
		{ m_stationary.SetLocalTarget(target); }
	void FollowTarget(const Vector3 *target)
		{ m_stationary.SetTarget(target); }
	void SetTarget(CActor *target)
		{ m_stationary.SetTarget(target); }

	void SetTargetOffset(const Vector3 &offset)
		{ m_stationary.SetOffset(offset); }

	void Setup(const Vector3 &eye, const Vector3 &target)
		{ m_stationary.Setup(eye, target); }

	void			SetHither( float hither ) { m_stationaryProj.SetHither( hither ); }
	float			GetHither( void ) { return m_stationaryProj.Hither(); }

protected:
	// Wee!
	StationaryWTVC			m_stationary;
	StationaryProjection	m_stationaryProj;
};

#endif
