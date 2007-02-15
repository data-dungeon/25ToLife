/////////////////////////////////////////////////////////////////////////////
// Generalized scene parameters read from a VAR file.
/////////////////////////////////////////////////////////////////////////////
#ifndef SCENESETUP_H
#define SCENESETUP_H

class SceneSetup
{
public:
	SceneSetup() {}

	// Load the scene setup var file
	static void Load(const char *defaultPath, const char *levelPath);

	// Reset to defaults
	static void Reset();

	// Enable/disable updates
	static void Enable(bool enable)
		{ m_enabled = enable; }

	// Override the fog stuff
	static void SetFogOverride(float fogNear, float fogFar, float fogDensity, const ts_bRGBA &fogColor);
	static void ClearFogOverride();

	// Apply changes (call this every frame)
	static void Update();

	//////////////////
	// Access the data
	//////////////////

	// Clear color
	static ts_bRGBA ClearColor()
		{ return ToColor(m_current.clearColor); }

	// Clipping parameters
	static float ClipNear()
		{ return m_current.clipNear; }
	static float ClipFar()
		{ return m_current.clipFar; }

	// Fog parameters
	static bool FogEnabled()
		{ return	m_fogOverride || m_current.fogEnable; }
	static float FogNear()
		{ return m_fogOverride ? m_fogOverrideNear : m_current.fogNear; }
	static float FogFar()
		{ return m_fogOverride ? m_fogOverrideFar : m_current.fogFar; }
	static float FogDensity()
		{ return m_fogOverride ? m_fogOverrideDensity : m_current.fogDensity; }
	static ts_bRGBA FogColor()
		{ return ToColor(m_fogOverride ? m_fogOverrideColor : m_current.fogColor); }

	// Base camera parameters
	static float BaseFOV()
		{ return m_current.baseFOV; }

	// Worry about things getting too far away (0.0f means don't worry)
	static float MaxClipFar()
		{ return m_maxClipFar; }
	static void SetMaxClipFar(float maxFar)
		{ m_maxClipFar = maxFar; }

private:
	// The sky color (also referred to as the clear color)
	struct Parameters
	{
		// FB clearing
		int		clearColor[4];
		bool		forceClear;

		// Buffer depths
		int		backBufferDepth;
		int		frontBufferDepth;

		// The clip planes
		float 	clipNear;
		float 	clipFar;

		// The fog parameters
		bool		fogEnable;
		float		fogNear;
		float		fogFar;
		float		fogDensity;
		int		fogColor[4];

		// The base FOV for everything (cameras may ignore)
		float		baseFOV;
	};
	static void DefaultParameters(Parameters &parameters);

	static bool			m_enabled;

	static Parameters m_current;
	static Parameters	m_lastApplied;

	// Fog override
	static bool			m_fogOverride;
	static bool			m_fogOverrideApplied;
	static float		m_fogOverrideNear;
	static float		m_fogOverrideFar;
	static float		m_fogOverrideDensity;
	static int			m_fogOverrideColor[4];

	static float		m_maxClipFar; 

	// Apply fog
	static void AppyFog();

	// Apply to the scene
	static void ApplyToScene();

	// did stuff change...
	static bool ParametersChanged();

	// A utility
	static inline ts_bRGBA ToColor(int color[4])
		{ ts_bRGBA c = { color[0], color[1], color[2], color[3] }; return c; }
};

#endif
