///////////////////////////////////////////////////////////////////////////////
// Virtualization for a full-screen fx
///////////////////////////////////////////////////////////////////////////////
#ifndef FSDFX_H
#define FSDFX_H

#include "Math/Vector.h"
#include "Effects/sfx/sfxfss.h"

class CFSDEffect;

class FullScreenFXBase
{
public:
	FullScreenFXBase(const char *name);
	virtual ~FullScreenFXBase() {}

	// Setup defaults
	virtual void SetDefaults(const Vector4 &color, float baseNoiseAlpha = 0.0f, float noiseModulate = 0.0f) = 0;

	// setup a blend mode (sprite only)
	virtual void SetBlendMode(u8 spriteBlendMode) {}

	// Register our variables
	virtual void RegisterContext(const char *context) = 0;

	// Create
	virtual bool Initialize() = 0;

	// Reset
	virtual void Reset() = 0;

	// Update!
	virtual void Update(float intensity, const Vector3 *pos = NULL) = 0;

	// are we active?
	virtual bool OnScreen() = 0;

protected:
	char			m_name[32];
};

class FullScreenFXSprite : public FullScreenFXBase
{
public:
	FullScreenFXSprite(const char *name);
	virtual ~FullScreenFXSprite();

	// Setup defaults
	void SetDefaults(const Vector4 &color, float baseNoiseAlpha = 0.0f, float noiseModulate = 0.0f);

	// Register our variables
	void RegisterContext(const char *context);

	// setup a blend mode (sprite only) call before initialize
	void SetBlendMode(u8 spriteBlendMode)
		{ m_spriteBlendMode = spriteBlendMode; }

	// Create
	bool Initialize();

	// Reset
	void Reset();

	// Update!
	void Update(float intensity, const Vector3 *pos = NULL);

	// are we active?
	bool OnScreen();

private:
	Vector4					m_color;
	char						m_requestSpriteBlend[16];
	u8							m_spriteBlendMode;
	float						m_baseNoiseAlpha;
	float						m_noiseModulate;
	SFXFullScreenSprite	*m_sprite;

	// Utiltiy
	static ts_bRGBA ToColor(const Vector4 &v);
};

class FullScreenFXDistort : public FullScreenFXBase
{
public:
	FullScreenFXDistort(const char *name);
	virtual ~FullScreenFXDistort();

	// Setup defaults
	void SetDefaults(const Vector4 &color, float baseNoiseAlpha = 0.0f, float noiseModulate = 0.0f) {}

	// Register our variables
	void RegisterContext(const char *context);

	// Create
	bool Initialize();

	// Reset
	void Reset();

	// Update!
	void Update(float intensity, const Vector3 *pos);

	// are we active?
	bool OnScreen();

private:
	char						m_fsdVar[32];
	CFSDEffect				*m_fsd;

	// Full screen distort
	void Delete();
};

// Virutalize it!
#if !defined(PS2) && !defined(_XBOX) && !defined(DIRECTX_PC)
class FullScreenFX : public FullScreenFXSprite
{
public:
	FullScreenFX(const char *name) : FullScreenFXSprite(name) {}
	virtual ~FullScreenFX() {}
};
#else
class FullScreenFX : public FullScreenFXDistort
{
public:
	FullScreenFX(const char *name) : FullScreenFXDistort(name) {}
	virtual ~FullScreenFX() {}
};
#endif

#endif
