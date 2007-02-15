///////////////////////////////////////////////////////////////////////////////
// Load screen!
///////////////////////////////////////////////////////////////////////////////
#include "EngineHelper/EngineHelperPCH.h"

#include "Display/Common/DisplayState.h"
#include "Display/Common/GfxFunc.h"
#include "Game/Database/SceneNode.h"
#include "Game/Managers/RenderMgr.h"
#include "Layers/Timing.h"
#include "EngineHelper/lsspin.h"

t_Handle LoadScreenSpin::s_dblHandle = INVALID_HANDLE;
float	   LoadScreenSpin::s_cameraD = 2.0f;
Vector3	LoadScreenSpin::s_cameraRotation(30.0f, 0.0f, 0.0f);
float		LoadScreenSpin::s_spinRate = 1.5f;
ts_bRGBA	LoadScreenSpin::s_clearColor = { 0, 0, 0, 255 };

///////////////////////////////////////////////////////////////////////////////
// Setup the loadscreen dbl
///////////////////////////////////////////////////////////////////////////////
bool LoadScreenSpin::SetModel(
const char *dbl)
{
	// Do we already have a model?
	if (s_dblHandle != INVALID_HANDLE)
		return true;

	s_dblHandle = dblLoad(dbl);
	ASSERT(s_dblHandle != INVALID_HANDLE);
	if (s_dblHandle == INVALID_HANDLE)
		return false;

	dblSetPermanent(s_dblHandle, true);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// All roations are in degrees
///////////////////////////////////////////////////////////////////////////////
void LoadScreenSpin::SetupCamera(
float cameraD,
const Vector3 &cameraRotation)
{
	s_cameraD = cameraD;
	s_cameraRotation = cameraRotation;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenSpin::SetupSpinRate(
float spinRate)
{
	s_spinRate = spinRate;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenModule *LoadScreenSpin::Create()
{
	if (s_dblHandle != INVALID_HANDLE)
		return new LoadScreenSpin;
	else
	{
		ASSERT(false);
		return NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenSpin::LoadScreenSpin()
{
	ASSERT(s_dblHandle != INVALID_HANDLE);
	if (s_dblHandle != INVALID_HANDLE)
	{
		ts_Geometry geometry = { 0 };
		dblGetGeometry(&geometry, s_dblHandle);
		m_instance.InitFromGeometry(geometry);
		m_instance.Hide(false);
		m_instance.SetAlphaSort(false);
		m_instance.SetReceiveShadows(false);
		m_instance.SetFogDisabled(true);
		m_instance.SetDynamicallyLit(false);
	}

	m_rotate = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LoadScreenSpin::~LoadScreenSpin()
{
	m_instance.Destroy();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenSpin::Startup()
{
	m_rotate = 0.0f;

	MatrixStack__Mode(MS_PROJECTION);
	MatrixStack__Push();
	MatrixStack__Identity();
	MatrixStack__Perspective(65.0f, 4.0f / 3.0f, 0.05f, 50.0f);

	g4WorldToCamera.Identity();
	g4WorldToCamera.SetXRotation(Math::Deg2Rad(s_cameraRotation.X()));
	g4WorldToCamera.CatYRotation(Math::Deg2Rad(s_cameraRotation.Y()));
	g4WorldToCamera.CatZRotation(Math::Deg2Rad(s_cameraRotation.Z()));
	g4WorldToCamera.CatZTranslation(-s_cameraD);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LoadScreenSpin::Render()
{
	m_instance.Matrix().SetYRotation(Math::Deg2Rad(m_rotate));

	gfxClearFrameBuffer(RS_COLOR_BUFFER_BIT | RS_DEPTH_BUFFER_BIT, &s_clearColor);

	// Render it!
	m_instance.Render(s_cameraD * 1.25f);

	m_rotate += s_spinRate;
	if (m_rotate >= 360.0f)
		m_rotate -= 360.0f;
}

