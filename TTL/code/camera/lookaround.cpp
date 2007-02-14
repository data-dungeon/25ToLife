///////////////////////////////////////////////////////////////////////////////
// A simple death camera
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Math/sfilter.h"
#include "camera/lookaround.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<LookAroundParams>::RegisterVariables()
{
   RegisterVariable(d_data.dwell, "dwell", 15.0f, 0.0f, 89.0f);
   RegisterVariable(d_data.newFocalMinTime, "newFocalMinTime", 8.0f, 0.0f, 60.0f);
   RegisterVariable(d_data.newFocalMaxTime, "newFocalMaxTime", 12.0f, 0.0f, 60.0f);
   RegisterVariable(d_data.focalFilter, "focalFilter", 0.9f, 0.0f, 10.0f);

   RegisterVariable(d_data.baseFOV, "baseFOV", 65.0f, 1.0f, 179.0f);
   RegisterVariable(d_data.fovFilter, "fovFilter", 0.9f, 0.0f, 10.0f);
   RegisterVariable(d_data.newZoomMinTime, "newZoomMinTime", 10.0f, 0.0f, 60.0f);
   RegisterVariable(d_data.newZoomMaxTime, "newZoomMaxTime", 20.0f, 0.0f, 60.0f);

   RegisterVariable(d_data.maxZoom, "maxZoom", 0.4f, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LookAroundWTVC::LookAroundWTVC(
LookAroundParams *&params) :
m_params(params)
{
	m_eye.Clear();
	m_focalPoint.Set(0.0f, 0.0f, 1.0f);
	m_baseFocal = m_focalPoint;
	m_targetFocal = m_focalPoint;
	m_nextFocalTime = 0.0f;
	m_dir.Clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LookAroundWTVC::SetFocalPoint(
const Vector3 &focalPoint)
{
	m_focalPoint = focalPoint;
	m_baseFocal = m_focalPoint;
	m_targetFocal = m_focalPoint;
	m_nextFocalTime = g_timer.GetEndSec() + g_random.InRange(m_params->newFocalMinTime, m_params->newFocalMaxTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LookAroundWTVC::Update(
Camera &parent)
{
	GeneralCamera &generalCam = ((GeneralCamera &)parent);

	// Base direction
	Vector3 baseDir;
	if (generalCam.GetMode() != GeneralCamera::CAMMODE_FOCAL_DIRECTION)
		baseDir = (m_baseFocal - m_eye).Normalized();
	else
	{	
		baseDir = generalCam.GetDirection();
		if (!Math::Zero((baseDir - m_dir).LengthSquared()))
		{
			m_dir = baseDir;
			m_focalPoint = m_eye + m_dir;
			m_baseFocal = m_focalPoint;
			m_targetFocal = m_focalPoint;
		}
	}
	
	// Update the target
	if (g_timer.GetEndSec() > m_nextFocalTime)
	{
		if (!Math::Zero(m_nextFocalTime))
			m_targetFocal = m_eye + g_random.InCone(baseDir, Math::Deg2Rad(m_params->dwell));
		m_nextFocalTime = g_timer.GetEndSec() + g_random.InRange(m_params->newFocalMinTime, m_params->newFocalMaxTime);
	}

	// What is our target direction
	Vector3 targetDir = (m_targetFocal - m_eye).Normalized();
	Vector3 currentDir = (m_focalPoint - m_eye).Normalized();

	float dt = g_timer.GetFrameSec();
	Vector3 dir = FOFilterImplicit(currentDir, targetDir, dt, m_params->focalFilter);
	m_focalPoint = m_eye + dir;
}

///////////////////////////////////////////////////////////////////////////////
// Cameras must have at least a name and w2v controller
///////////////////////////////////////////////////////////////////////////////
LookAroundCamera::LookAroundCamera(
const char *name,
ProjectionController *projectionC,
ViewportController *viewportC) :
GeneralCamera(name, &m_lookAround, projectionC, viewportC),
m_lookAround(m_params)
{
	// Holy magic
	m_params = CParam<LookAroundParams>::GetParam("LookAround", "var");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LookAroundCamera::Activate()
{
	Camera::Activate();

	m_nextFOVChange = g_timer.GetEndSec() + g_random.InRange(m_params->newZoomMinTime, m_params->newZoomMaxTime);
	DefaultProjectionController &proj = (DefaultProjectionController &)GetProjectionController();
	proj.SetFOV(m_params->baseFOV);
	proj.SetFOVFilter(m_params->fovFilter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LookAroundCamera::Update()
{
	if (g_timer.GetEndSec() > m_nextFOVChange)
	{
		m_nextFOVChange = g_timer.GetEndSec() + g_random.InRange(m_params->newZoomMinTime, m_params->newZoomMaxTime);
		DefaultProjectionController &proj = (DefaultProjectionController &)GetProjectionController();
		proj.SetFOV(m_params->baseFOV * g_random.InRange(m_params->maxZoom, 1.0f));
	}

	Camera::Update();
}
