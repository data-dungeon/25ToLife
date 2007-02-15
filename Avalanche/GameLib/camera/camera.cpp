///////////////////////////////////////////////////////////////////////////////
// The Camera!
///////////////////////////////////////////////////////////////////////////////
#include "camera/CameraPCH.h"

#include "Math/sfilter.h"
#include "Math/fastrand.h"
#include "Effects/pnoise.h"

///////////////////////////////////////////////////////////////////////////////
// Cameras must have at least a name and w2v controller
///////////////////////////////////////////////////////////////////////////////
Camera::Camera(
const char *name,
WorldToViewController *w2vController,
ProjectionController *projectionController,
ViewportController *viewportController)
{
	// copy in the name
	strncpy(m_name, name, sizeof(m_name) - 1);
	m_name[sizeof(m_name) - 1] = '\0';

	// No group
	m_group = NULL;

	// No scene yet
	m_scene = NULL;

	// Assign the controllers
	m_worldToViewController = w2vController;
	m_projectionController = projectionController;
	m_viewportController = viewportController;

	// Default these things to something nice (even though they are not real)
	m_worldToView.Identity();
	m_viewToWorld.Identity();
	m_projection.Identity();
	m_worldToScreen.Identity();

	m_eye.Clear();
	m_dir.Set(0.0f, 0.0f, 1.0f);
	m_focalPoint = m_eye + m_dir;
	m_up.Set(0.0f, 1.0f, 0.0f );
	m_right.Set(1.0f, 0.0f, 0.0f);

	// Setup the filtering
	m_eyeTimeConstant = 0.05f;
	m_focalTimeConstant = 0.05f;
	m_noFilterDistanceSquared = 5.0f * 5.0f;

	// Shake
	ClearShake(m_shakeEye);
	ClearShake(m_shakeFocal);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Camera::~Camera()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::SetEyePointShake(
float amplitude,
float frequency,
float duration,
bool noise,
bool noFilter)
{
	SetupShake(m_shakeEye, amplitude, frequency, duration, noise, noFilter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::ClearEyePointShake()
{
	ClearShake(m_shakeEye);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::SetFocalPointShake(
float amplitude,
float frequency,
float duration,
bool noise,
bool noFilter)
{
	SetupShake(m_shakeFocal, amplitude, frequency, duration, noise, noFilter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::ClearFocalPointShake()
{
 	ClearShake(m_shakeFocal);
}

///////////////////////////////////////////////////////////////////////////////
// Per frame update
///////////////////////////////////////////////////////////////////////////////
void Camera::Update()
{
	ASSERT(m_worldToViewController);

	// Allow the camera to re-update
	m_worldToViewController->Update(*this);
	BuildCamera();

	GetProjectionController().Update(*this);
	BuildProjection();

	// Build the all important matrix
	m_worldToScreen = m_worldToView * m_projection;

	// Now do the viewport
	GetViewportController().Update(*this);
	BuildViewport();

	// Apply them to the rendering
	ApplyCamera();
	ApplyProjection();
	ApplyViewport();
}

///////////////////////////////////////////////////////////////////////////////
// Render right after the main render
///////////////////////////////////////////////////////////////////////////////
void Camera::PostRenderUpdate()
{
	// Not much to do here, other than call the viewport controller
	ViewportController &vc = GetViewportController();
	vc.PostRender();
}

///////////////////////////////////////////////////////////////////////////////
// Transform from world to screen
///////////////////////////////////////////////////////////////////////////////
Vector2 Camera::ToScreen(
Vector3CRef world) const
{
	// Do the math
	Vector4 t = world.Transform3(m_worldToScreen);

	// Figure out where it lies on the screen
	float oow = 1.0f / t.w();
	Vector2 screen(((t.x() * oow) * 0.5f + 0.5f), ((t.y() * oow) * 0.5f + 0.5f));
	return screen;
}

///////////////////////////////////////////////////////////////////////////////
// Transform from world to screen using the fustrum
// Returns true if the the point is on-screen, false if clipped
///////////////////////////////////////////////////////////////////////////////
bool Camera::ToScreen(
Vector3CRef world,
Vector2 &screen,
float &w) const
{
	// Do the math
	Vector4 t = world.Transform3(m_worldToScreen);

	// Outside of fustrum?
	if (t.z() < 0.0f || t.z() > t.w() ||
			t.x() < -t.w() || t.x() > t.w() ||
			t.y() < -t.w() || t.y() > t.w())
		return false;

	// Figure out where it lies on the screen
	float oow = 1.0f / t.w();
	screen.Set(((t.x() * oow) * 0.5f + 0.5f), ((t.y() * oow) * 0.5f + 0.5f));
	w = t.w();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Transform using a clip factor (making the cliping region larger/smaller
// than the screen)
// Returns true if the the point is on-screen, false if clipped
///////////////////////////////////////////////////////////////////////////////
bool Camera::ToScreen(
Vector3CRef world,
Vector2 &screen,
float &w,
const Vector2 &clipFactor) const
{
	// Do the math
	Vector4 t = world.Transform3(m_worldToScreen);

	// Behind us?
	if (t.w() < 0.0f)
		return false;

	// Other clipping
	Vector2 clip = clipFactor * t.w();
	if (t.x() < -clip.x() || t.x() > clip.x() ||
			t.y() < -clip.y() || t.y() > clip.y())
		return false;

	// Figure out where it lies (this should work with the viewport, but it doesn't yet)
	float oow = 1.0f / t.w();
	screen.Set(((t.x() * oow) * 0.5f + 0.5f), ((t.y() * oow) * 0.5f + 0.5f));
	w = t.w();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Simple object tests against the fustrum
///////////////////////////////////////////////////////////////////////////////
Camera::FRUSTUMSTATE Camera::Test(
Vector3CRef point)
{
	return INSIDE;
}

///////////////////////////////////////////////////////////////////////////////
// Simple object tests against the fustrum
///////////////////////////////////////////////////////////////////////////////
Camera::FRUSTUMSTATE Camera::Test(
Vector3CRef center,
float radius)
{
	return INSIDE;
}

///////////////////////////////////////////////////////////////////////////////
// Simple object tests against the fustrum
///////////////////////////////////////////////////////////////////////////////
Camera::FRUSTUMSTATE Camera::Test(
const AABB &aabb)
{
	return INSIDE;
}

///////////////////////////////////////////////////////////////////////////////
// Access the controllers
///////////////////////////////////////////////////////////////////////////////
ProjectionController &Camera::GetProjectionController()
{
	if (m_projectionController)
		return *m_projectionController;
	else
		return m_scene->MasterProjection();
}

///////////////////////////////////////////////////////////////////////////////
// Access the controllers
///////////////////////////////////////////////////////////////////////////////
ViewportController &Camera::GetViewportController()
{
	if (m_viewportController)
		return *m_viewportController;
	else
		return m_scene->MasterViewport();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::SetupShake(
Shake &shake,
float amplitude,
float frequency,
float duration,
bool noise,
bool noFilter)
{
	shake.active = true;
	shake.amplitude = amplitude;
	shake.frequency = Math::TwoPi * frequency;
	shake.seed.Set(g_random.Unit() * Math::Pi, g_random.Unit() * Math::Pi, g_random.Unit() * Math::Pi);
	shake.doneTime = (duration < 0.0f) ? duration : g_timer.GetEndSec() + duration;
	shake.noise = noise;
	shake.disableFilter = noFilter;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::ClearShake(
Shake &shake)
{
	shake.active = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Camera::ShakeDone(
Shake &shake)
{
	// Not shaking
	if (!shake.active)
		return true;

	// Shaking forever
	if (shake.doneTime < 0.0f)
		return false;

	// Done yet
	float time = g_timer.GetEndSec();
	bool done = (time > shake.doneTime);

	// If we are stop
	if (done)
		ClearShake(shake);

	return done;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 Camera::ComputeShake(
Shake &shake)
{
	// Get t
	float t = g_timer.GetEndSec() * shake.frequency;

	Vector3 d;
	if (shake.noise)
		d.Set(g_perlinNoise.Noise(t + shake.seed.X()),
				g_perlinNoise.Noise(t + shake.seed.Y()),
				g_perlinNoise.Noise(t + shake.seed.Z()));
	else
		d.Set(Math::Sin(t + shake.seed.X()),
				Math::Sin(t + shake.seed.Y()),
				Math::Sin(t + shake.seed.Z()));

	return d * shake.amplitude;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Camera::NoFilter(
Shake &shake)
{
	return (shake.active && shake.disableFilter);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::BuildCamera()
{
	// Get the camera quantities
	Vector3 targetEye = m_worldToViewController->Eye();
	Vector3 targetFocal = m_worldToViewController->FocalPoint();
	const Vector3 tryUp = m_worldToViewController->Up();

	// Some sanity checks (eventually we will repair)
	ASSERT(!Math::Zero((targetEye - targetFocal).LengthSquared()));

	// time
	float deltaT = g_timer.GetFrameSec();

	// Are we shaking?
	if (!ShakeDone(m_shakeEye))
		targetEye += ComputeShake(m_shakeEye);
	if (!ShakeDone(m_shakeFocal))
		targetFocal += ComputeShake(m_shakeFocal);

	// Filter the eye
	Vector3 deltaEye = m_eye - targetEye;
	if (deltaEye.LengthSquared() > m_noFilterDistanceSquared || NoFilter(m_shakeEye) || Math::Zero(deltaT))
		m_eye = targetEye;
	else
		m_eye = FOFilterImplicit(m_eye, targetEye, deltaT, m_eyeTimeConstant);

	// Filter the focal
	Vector3 deltaFocal = m_focalPoint - targetFocal;
	if (deltaFocal.LengthSquared() > m_noFilterDistanceSquared || NoFilter(m_shakeFocal) || Math::Zero(deltaT))
		m_focalPoint = targetFocal;
	else
		m_focalPoint = FOFilterImplicit(m_focalPoint, targetFocal, deltaT, m_focalTimeConstant);

	// What is the direction
	m_dir = m_focalPoint - m_eye;
	m_dir.Normalize();
	Vector3 dir = -m_dir;

	// What is the up
	m_up = tryUp - (dir * Vector3::Dot(tryUp, dir));
	m_up.SafeNormalize();

	m_right = Vector3::Cross(m_up, dir);

	// Setup our matrix
	m_viewToWorld.SetRow0(m_right);
	m_viewToWorld.SetRow1(m_up);
	m_viewToWorld.SetRow2(dir);
	m_viewToWorld.SetRow3(m_eye);

	// Now invert it for the real camera
	m_viewToWorld.Invert(m_worldToView);
}

///////////////////////////////////////////////////////////////////////////////
// Rather unamazingly there is no way to just ask the engine to give this to
// you.
///////////////////////////////////////////////////////////////////////////////
void Camera::BuildProjection()
{
	// Get the quantities
	ProjectionController &projection = Camera::GetProjectionController();
	const float hither = projection.Hither();
	const float yon = projection.Yon();
	const float halfFOV = Math::Deg2Rad(projection.FOV()) / 2.0f;
	const float aspectRatio = projection.AspectRatio();

	// Compute f (clever comment eh? See Foley et al.)
   const float f = Math::Cos(halfFOV) / Math::Sin(halfFOV);

	// Set it up
	m_projection.Identity();
   m_projection.e00 = f / aspectRatio;
   m_projection.e11 = -f;
   m_projection.e22 = (hither + yon) / (hither - yon);
   m_projection.e23 = -1.0f;
   m_projection.e32 = (2.0f * hither * yon) / (hither - yon);
   m_projection.e33 = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::BuildViewport()
{
	// Really nothing to do here...
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::ApplyCamera()
{
	// Enter the engine!
	ASSERT(m_scene);

	// Copy it in
	if (m_scene)
		m_scene->UpdateCameraMatrix(m_worldToView);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::ApplyProjection()
{
	// Get the quantities
	ProjectionController &projection = Camera::GetProjectionController();
	const float hither = projection.Hither();
	const float yon = projection.Yon();
	const float fov = projection.FOV();
	const float aspectRatio = projection.AspectRatio();

	// Rather unamazingly, there is no way to do this easily
	CScene *scene = m_scene->Scene();
	ASSERT(scene);
	CSceneView *sceneView = &scene->d_sceneView;

	// Set it
	sceneView->SetNearClip(hither);
	sceneView->SetFarClip(yon);
	sceneView->SetFieldOfViewY(fov);
	sceneView->SetAspectRatio(aspectRatio);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Camera::ApplyViewport()
{
	// Get the quantities
	ViewportController &viewport = Camera::GetViewportController();
	const float x = viewport.XOffset();
	const float y = viewport.YOffset();
	const float width = viewport.Width();
	const float height = viewport.Height();

	// Rather unamazingly, there is no way to do this easily
	CScene *scene = m_scene->Scene();
	ASSERT(scene);
	CSceneView &sceneView = scene->d_sceneView;

	// Apply it
	sceneView.SetViewport(x, y, width, height);
}

///////////////////////////////////////////////////////////////////////////////
// These are called by the CameraScene when we are init'd
///////////////////////////////////////////////////////////////////////////////
void Camera::BindScene(
CameraScene &scene)
{
	m_scene = &scene;
}

///////////////////////////////////////////////////////////////////////////////
// Used to inherit
///////////////////////////////////////////////////////////////////////////////
Camera &Camera::operator=(
const Camera &c)
{
	m_eye = c.m_eye;
	m_focalPoint = c.m_focalPoint;
	m_right = c.m_right;
	m_up = c.m_up;
	m_dir = c.m_dir;

	return *this;
}
