///////////////////////////////////////////////////////////////////////////////
// Static decals require a handle and must be deleted.
///////////////////////////////////////////////////////////////////////////////
#ifndef FADEDECAL_H
#define FADEDECAL_H

#include "Effects/decal/staticdecal.h"

// We are going to fade in, and fade out...
struct FadeExtraData
{
	union
	{
		float		timeConstant;
		float		fadeRate;
	};
	float		alpha;
	float		alphaTarget;
};

class FadeDecalEmitter : public StaticDecalEmitter<FadeExtraData>
{
public:
	// Fade in seconds or time constant
	enum FadeType
	{
		FADETYPE_FILTER,
		FADETYPE_LINEAR
	};
	FadeDecalEmitter(DecalSet &set,
							const DecalTextureLayer *layer, int layers,
							FadeType fadeType = FADETYPE_FILTER);
	virtual ~FadeDecalEmitter() {}

	// Create a decal
	StaticDecalHandle Create(const Vector3 corner[4],
										const ts_bRGBA color[4],
										const Vector2 uv[4],
										float fadeTimeConstant,
										float alphaTarget = 1.0f);

	// Create a decal using all defaults
	// Create a decal using all defaults
	StaticDecalHandle Create(const Vector3 corner[4],
										float fadeTimeConstant,
										float alphaTarget = 1.0f);
	StaticDecalHandle Create(const Vector3 corner[4],
										const ts_bRGBA color[4],
										float fadeTimeConstant,
										float alphaTarget = 1.0f);

	// Free a decal!
	bool Destroy(StaticDecalHandle handle,
											float fadeTimeConstant,
											float alphaTarget = 0.0f);

	// A handy utility (why aren't these in the enigne)
	inline static ts_bRGBA ToIntensity(u16 color, u8 alpha = 255);

protected:
	// How do we run our fade?
	FadeType	m_fadeType;

	// Some of our own flags
	enum FadeFlags
	{
		DECALFADE_NONE = 0x00,
		DECALFADE_IN = 0x10,
		DECALFADE_OUT = 0x20
	};
	// Set the fade state
	void SetFadeState(PerDecal &decal, FadeFlags fade)
		{ decal.flags = (decal.flags & ~0xf0) | fade; }
	FadeFlags GetFadeState(PerDecal &decal)
		{ return (FadeFlags)(decal.flags & 0xf0); }
	bool FadeDone(PerDecal &decal)
		{ return	Math::Equal(decal.extra.alpha, decal.extra.alphaTarget, 0.005f); }

	// You write this, it gets called on successful create
	void OnCreate(int index, PerDecal &decal);

	// You write this, gets called per render
	void Update(const CInstance &instance, const Graphics4x4 &localToCamera, float zCamDist);
};

/////////////////////////////////////////////////////////////////////////////
// A handy utility (why aren't these in the enigne)
/////////////////////////////////////////////////////////////////////////////
inline ts_bRGBA FadeDecalEmitter::ToIntensity(
u16 color,
u8 alpha)
{
	ts_bRGBA c;
	int r = ((color >> 0) & 0x1f) << 3;
	int g = ((color >> 5) & 0x3f) << 2;
	int b = ((color >> 11) & 0x1f) << 3;
	int intensity = Math::Clamp((r + g + b) / 3, 64, 255);
	c.R = c.G = c.B = intensity;
	c.A = alpha;
	return c;
}

#endif
