/////////////////////////////////////////////////////////////////////////////
// Generalized scene parameters read from a VAR file.
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"
#include "EngineHelper/pathfile.h"
#include "cmdcon/cmdcon.h"

// The data
SceneSetup::Parameters SceneSetup::m_current;
SceneSetup::Parameters SceneSetup::m_lastApplied;

bool	SceneSetup::m_enabled = true;
bool	SceneSetup::m_fogOverride = false;
bool	SceneSetup::m_fogOverrideApplied = false;
float	SceneSetup::m_fogOverrideNear;
float	SceneSetup::m_fogOverrideFar;
float	SceneSetup::m_fogOverrideDensity;
int	SceneSetup::m_fogOverrideColor[4];
float SceneSetup::m_maxClipFar = 0.0f;

/////////////////////////////////////////////////////////////////////////////
// Load the scene setup var file
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::Load(
const char *defaultPath,
const char *levelPath)
{
	const char *context = "scene";

	// Make the filename for the level specific
	char pathFile[256];
	PathFile::MakePath(pathFile, levelPath, "scene");
	char scriptFile[256];
	g_console.MakeScriptFileName(pathFile, scriptFile);

  	g_console.BindScript(context, scriptFile);

	g_console.CreateVar(context, "clearColorR", m_current.clearColor[0]);
	g_console.CreateVar(context, "clearColorG", m_current.clearColor[1]);
	g_console.CreateVar(context, "clearColorB", m_current.clearColor[2]);
	g_console.CreateVar(context, "clearColorA", m_current.clearColor[3]);
	g_console.CreateVar(context, "forceClear", m_current.forceClear);

	g_console.CreateVar(context, "backBufferDepth", m_current.backBufferDepth);
	g_console.CreateVar(context, "frontBufferDepth", m_current.frontBufferDepth);

	g_console.CreateVar(context, "clipNear", m_current.clipNear, CONVAR_REAL);
	g_console.CreateVar(context, "clipFar", m_current.clipFar, CONVAR_REAL);

	g_console.CreateVar(context, "fogEnabled", m_current.fogEnable);
	g_console.CreateVar(context, "fogDensity", m_current.fogDensity, CONVAR_REAL);
	g_console.CreateVar(context, "fogNear", m_current.fogNear, CONVAR_REAL);
	g_console.CreateVar(context, "fogFar", m_current.fogFar, CONVAR_REAL);

	g_console.CreateVar(context, "fogColorR", m_current.fogColor[0]);
	g_console.CreateVar(context, "fogColorG", m_current.fogColor[1]);
	g_console.CreateVar(context, "fogColorB", m_current.fogColor[2]);
	g_console.CreateVar(context, "fogColorA", m_current.fogColor[3]);

	g_console.CreateVar(context, "baseFOV", m_current.baseFOV, CONVAR_REAL);

	// Clear out the current set to defaults
	DefaultParameters(m_current);

	// Execute the default first
	PathFile::MakePath(pathFile, defaultPath, "scene");
	g_console.ExecuteFile(pathFile);

	// Execute the context!
	g_console.ExecuteContext(context);

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(context);
#endif //INTERACTIVE_CONSOLE

#ifdef ENABLE_WATCHCLIENT
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.clearColor[0], m_current.clearColor[0], 0, 255, "clearColorR");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.clearColor[1], m_current.clearColor[1], 0, 255, "clearColorG");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.clearColor[2], m_current.clearColor[2], 0, 255, "clearColorB");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.clearColor[3], m_current.clearColor[3], 0, 255, "clearColorA");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.clipNear, m_current.clipNear, .001f, 1000.0f, "clipNear");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.clipFar, m_current.clipFar, .001f, m_maxClipFar, "clipFar");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogDensity, m_current.fogDensity, 0, 1, "fogDensity");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogNear, m_current.fogNear, 0.0f, 1000.0f, "fogNear");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogFar, m_current.fogFar, 0.0f, 1000.0f, "fogFar");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogColor[0], m_current.fogColor[0], 0, 255, "fogColorR");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogColor[1], m_current.fogColor[1], 0, 255, "fogColorG");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogColor[2], m_current.fogColor[2], 0, 255, "fogColorB");
	WatchClient::Instance()->RegisterVariable((char *)context, &m_current.fogColor[3], m_current.fogColor[3], 0, 255, "fogColorA");
#endif

	// Doh!
	if (!Math::Zero(m_maxClipFar))
	{
		ASSERT(m_current.clipFar <= m_maxClipFar);
		if (m_current.clipFar > m_maxClipFar)
			m_current.clipFar = m_maxClipFar;
	}

	// Do an update now!
	Update();
}

/////////////////////////////////////////////////////////////////////////////
// Override the fog stuff
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::Reset()
{
	// Clear out the current set to defaults
	DefaultParameters(m_current);
	m_enabled = true;
	Update();
}

/////////////////////////////////////////////////////////////////////////////
// Override the fog stuff
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::SetFogOverride(
float fogNear,
float fogFar,
float fogDensity,
const ts_bRGBA &fogColor)
{
	m_fogOverride = true;
	m_fogOverrideApplied = false;

	m_fogOverrideNear = fogNear;
	m_fogOverrideFar = fogFar;
	m_fogOverrideDensity = fogDensity;
	m_fogOverrideColor[0] = fogColor.R;
	m_fogOverrideColor[1] = fogColor.G;
	m_fogOverrideColor[2] = fogColor.B;
	m_fogOverrideColor[3] = fogColor.A;
}

/////////////////////////////////////////////////////////////////////////////
void SceneSetup::ClearFogOverride()
{
	m_fogOverride = false;
	m_fogOverrideApplied = false;
}

/////////////////////////////////////////////////////////////////////////////
// Apply changes (call this every frame)
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::Update()
{
	// Ouch
	if (!m_enabled)
		return;

	// Do we need to apply it?
	bool applyIt = false;

	// Anything to do here?
	if (ParametersChanged())
	{
		ASSERT(m_current.backBufferDepth == 16 || m_current.backBufferDepth == 32);
		ASSERT(m_current.frontBufferDepth == 16 || m_current.frontBufferDepth == 32);
		if (m_current.backBufferDepth != 16 && m_current.backBufferDepth != 32)
			m_current.backBufferDepth = 16;
		if (m_current.frontBufferDepth != 16 && m_current.frontBufferDepth != 32)
			m_current.frontBufferDepth = 16;
#ifdef PS2
		// Back buffer depth
		TextureMgr::Pause();  // Also reloads a white texture.
		if (m_current.backBufferDepth != Display__GetDrawBufferBPP() ||
			 m_current.frontBufferDepth != Display__GetDisplayBufferBPP())
		{
	      Display__ClearGraphics();
			Display__Shutdown(false);
			Display__SetFrameBufferBPP(m_current.backBufferDepth, m_current.frontBufferDepth);
			Display__Init();
	      Display__ClearGraphics();
		}
		TextureMgr::Resume();
#endif

		// Set the clear color
		Display__SetClearMode(m_current.forceClear);
		Display__SetClearColor(ToColor(m_current.clearColor));

		// Setup the clipping settings
		ASSERT(m_current.clipNear < m_current.clipFar);
		Display__SetNearClip(m_current.clipNear);
		Display__SetFarClip(m_current.clipFar);

		// Set the fog settings
		AppyFog();
		applyIt = true;

		// Make sure we keep track
		m_lastApplied = m_current;
	}

	// Handle overriden fog
	if (m_fogOverride && !m_fogOverrideApplied)
	{
		AppyFog();
		applyIt = true;
	}

	// Appy it
	if (applyIt)
		ApplyToScene();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::DefaultParameters(
Parameters &parameters)
{
	memset(&parameters, '\0', sizeof(parameters));

	parameters.forceClear = true;

	parameters.backBufferDepth = 32;
	parameters.frontBufferDepth = 32;

	parameters.clipNear = 0.5f;
	parameters.clipFar = 100.0f;

	parameters.baseFOV = 65.0f;
}

/////////////////////////////////////////////////////////////////////////////
// Apply fog
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::AppyFog()
{
	ASSERT(FogNear() <= FogFar());
	Display__SetFog(FogEnabled());
	Display__SetNearFog(FogNear());
	Display__SetFarFog(FogFar());
	Display__SetFogColor(FogColor());
	Display__SetFogMode(RS_FOGMODE_LINEAR);
//#ifdef DIRECTX_PC
//	Display__SetFogMode(3);
//#endif
	Display__SetFogDensity(FogDensity());
	m_fogOverrideApplied = m_fogOverride;
}

/////////////////////////////////////////////////////////////////////////////
// Apply to the scene (god)
/////////////////////////////////////////////////////////////////////////////
void SceneSetup::ApplyToScene()
{
	// Only do the first scene, mu
	CScene *scene = SceneMgr::GetFirstScene();
	while (scene)
	{
		CSceneView &sceneView = scene->d_sceneView;

		sceneView.SetNearClip(DisplayState.fNearClip);
		sceneView.SetFarClip(DisplayState.fFarClip);
		sceneView.SetFieldOfViewY(DisplayState.fFieldOfViewY);
		sceneView.SetNearFog(DisplayState.fNearFog);
		sceneView.SetFarFog(DisplayState.fFarFog);
		sceneView.SetFog(DisplayState.bFog);
		sceneView.SetFogColor(DisplayState.FogColor);
		sceneView.SetFogMode(DisplayState.nFogMode);
		sceneView.SetFogDensity(DisplayState.fFogDensity);
		sceneView.SetClearColor(DisplayState.ClearColor);
		sceneView.SetClearMode((DisplayState.u32ClearMode & RS_COLOR_BUFFER_BIT) != 0);

		scene = SceneMgr::GetNextScene(scene);
	}
}

/////////////////////////////////////////////////////////////////////////////
// stuff changed...
/////////////////////////////////////////////////////////////////////////////
bool SceneSetup::ParametersChanged()
{
	return (memcmp(&m_current, &m_lastApplied, sizeof(m_current)) != 0);
}
