///////////////////////////////////////////////////////////////////////////////
// A simple, default projection
///////////////////////////////////////////////////////////////////////////////
#ifndef DEFPROJ_H
#define DEFPROJ_H

#include "camera/projection.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class DefaultProjectionController : public ProjectionController
{
public:
	DefaultProjectionController();
	virtual ~DefaultProjectionController() {}

	// Control hither and yon
	void SetHitherYon(float hither, float yon, bool instant = false);
	void SetHither(float hither, bool instant = false);
	void SetYon(float yon, bool instant = false);
	void SetHitherYonFilter(float filterT);
	float Hither();
	float Yon();

	// Control FOV (0...180) degrees (non-inclusive)
	void SetFOV(float fov, bool instant = false);
	void SetFOVFilter(float filterT);
	float FOV();

	// Return the aspect ratio
	void SetAspectRatio(float aspectRatio, bool instant = false);
	void SetAspectRatioFilter(float filterT);
	float AspectRatio();

	// Update
	virtual void Update(Camera &parent);

protected:
	float 		m_hither, m_hitherTarget;
	float 		m_yon, m_yonTarget;
	float			m_hitherYonFilterT;

	float			m_FOV, m_FOVTarget;
	float			m_FOVFilterT;

	float			m_aspectRatio, m_aspectRatioTarget;
	float			m_aspectRatioFilterT;
};

#endif
