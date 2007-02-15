///////////////////////////////////////////////////////////////////////////////
// SpecialFX full screen sprite
///////////////////////////////////////////////////////////////////////////////
#ifndef SFXFSS_H
#define SFXFSS_H

#include "Effects/sfx/specialfx.h"

class SFXFullScreenSprite : public SpecialFX
{
public:
	// Auto destructs after ttl has expired
	SFXFullScreenSprite(SpecialFXSystem &sfx, const char *name, const ts_bRGBA color[4], float ttl, u8 spriteBlendMode = SPRITEBLEND_NORMAL);
	~SFXFullScreenSprite();

	// give back the subclass
	const char *Class()
		{ return "fss"; }

	// These create and add! (negative ttl means ask the emitter)
	static SFXFullScreenSprite *Create(const char *name, const ts_bRGBA &color, float ttl = -1.0f, u8 spriteBlendMode = SPRITEBLEND_NORMAL);
	static SFXFullScreenSprite *Create(const char *name, const ts_bRGBA color[4], float ttl = -1.0f, u8 spriteBlendMode = SPRITEBLEND_NORMAL);
	static void Destroy(SFXFullScreenSprite *&sfx);

	void SetColor(const ts_bRGBA &color);
	void SetColor(const ts_bRGBA color[4]);
	void SetAlpha(float alpha);
	void SetAlpha(const float alpha[4]);

	void Hide();
	void Show();
	bool Hidden()
		{ return !m_spriteOn; }

protected:
	// Our update!
	void Update(float now, float deltaT) {}

	// The data
	t_Handle			m_sprite;
	bool				m_spriteOn;
	ts_bRGBA			m_color[4];
	void ApplyColors();
};

#endif

