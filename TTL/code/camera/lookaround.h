///////////////////////////////////////////////////////////////////////////////
// A simple lookaround camera
///////////////////////////////////////////////////////////////////////////////
#ifndef LOOKAROUND_H
#define LOOKAROUND_H

#include "camera/generalcam/generalcam.h"

struct LookAroundParams
{
	float			dwell;
	float			newFocalMinTime, newFocalMaxTime;
	float			focalFilter;

	float			baseFOV;
	float			fovFilter;
	float			newZoomMinTime, newZoomMaxTime;
	float			maxZoom;
};

class LookAroundWTVC : public GeneralWTVC
{
public:
	LookAroundWTVC(LookAroundParams *&params);
	~LookAroundWTVC() {}

	// Focal point
	void SetFocalPoint(const Vector3 &focal);

  	// Update
	void Update(Camera &parent);

private:
	// What are the settings?
	LookAroundParams		*&m_params;

	// Our innards
	Vector3					m_baseFocal;
	Vector3					m_targetFocal;
	Vector3					m_dir;

	// Next new point
	float						m_nextFocalTime;
};

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
class LookAroundCamera : public GeneralCamera
{
public:
	// Cameras must have at least a name and w2v controller
	LookAroundCamera(const char *name,
					ProjectionController *projectionController = NULL,
					ViewportController *viewportController = NULL);

	void Setup( const Vector3 &pos , const Vector3 &target)
		{ m_lookAround.Setup(pos, target); }
	void SetFocalPoint(const Vector3 &point)
		{ m_lookAround.SetFocalPoint(point); }
	void SetPosition(const Vector3 &position)
		{ m_lookAround.SetEye(position); }

	void Activate();
	void Update();

protected:
	// Wee!
	LookAroundWTVC			m_lookAround;

	// The parameters for this camera
	LookAroundParams		*m_params;

	// Wee
	float						m_nextFOVChange;
};

#endif
