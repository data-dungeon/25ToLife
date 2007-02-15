///////////////////////////////////////////////////////////////////////////////
// SpecialFX particle emitter!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Effects/sfx/sfxsys.h"
#include "Effects/sfx/sfxfss.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXFullScreenSprite::~SFXFullScreenSprite()
{
	if (m_sprite != INVALID_HANDLE)
		SpriteMgr__KillSprite(m_sprite);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXFullScreenSprite *SFXFullScreenSprite::Create(
const char *name,
const ts_bRGBA &color,
float ttl,
u8 spriteBlendMode)
{
	ts_bRGBA corner[4];
	corner[0] = corner[1] = corner[2] = corner[3] = color;
	return Create(name, corner, ttl, spriteBlendMode);
}

///////////////////////////////////////////////////////////////////////////////
// These create and add!
///////////////////////////////////////////////////////////////////////////////
SFXFullScreenSprite *SFXFullScreenSprite::Create(
const char *name,
const ts_bRGBA color[4],
float ttl,
u8 spriteBlendMode)
{
	// Extreme case
	if (!ValidSingleton(SpecialFXSystem))
		return NULL;

	// Create the object
	SFXFullScreenSprite *sfx = new SFXFullScreenSprite(g_sfx, name, color, ttl, spriteBlendMode);
	if (!sfx)
		return NULL;

	g_sfx.Add(sfx);
	return sfx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::Destroy(
SFXFullScreenSprite *&sfx)
{
	if (sfx)
	{
		g_sfx.Delete(sfx);
		sfx = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SFXFullScreenSprite::SFXFullScreenSprite(
SpecialFXSystem &sfx,
const char *name,
const ts_bRGBA color[4],
float ttl,
u8 spriteBlendMode) :
SpecialFX(sfx, name, ttl)
{
	m_color[0] = color[0];
	m_color[1] = color[1];
	m_color[2] = color[2];
	m_color[3] = color[3];

	Vector3 v[4];
	v[0].Set(0.0f, 0.0f, 0.0f);
	v[1].Set(1.0f, 0.0f, 0.0f);
	v[2].Set(1.0f, 1.0f, 0.0f);
	v[3].Set(0.0f, 1.0f, 0.0f);

	Vector2 uv[4];
	uv[0].Set(0.0f, 0.0f);
	uv[1].Set(1.0f, 0.0f);
	uv[2].Set(1.0f, 1.0f);
	uv[3].Set(0.0f, 1.0f);

	// Register us!
	m_sprite = SpriteMgr__RegisterSprite(TS_NO_TEXTURE, 99, SPRITE_DEFAULT_SORT,
					  										SHADE_SPRITE | SPRITE_BLENDMODE_ENABLED,
					  										v, uv, m_color);
	SpriteMgr__SetBlendMode(m_sprite, spriteBlendMode);
	ApplyColors();
	SpriteMgr__DisableSprite(m_sprite);
	m_spriteOn = false;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::SetColor(
const ts_bRGBA &color)
{
	m_color[0] = color;
	m_color[1] = color;
	m_color[2] = color;
	m_color[3] = color;

	ApplyColors();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::SetColor(
const ts_bRGBA color[4])
{
	m_color[0] = color[0];
	m_color[1] = color[1];
	m_color[2] = color[2];
	m_color[3] = color[3];

	ApplyColors();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::SetAlpha(
float alpha)
{
	u8 a = (u8)Math::Clamp(alpha * 255.0f, 0.0f, 255.0f);
	m_color[0].A = a;
	m_color[1].A = a;
	m_color[2].A = a;
	m_color[3].A = a;

	ApplyColors();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::SetAlpha(
const float alpha[4])
{
	m_color[0].A = (u8)Math::Clamp(alpha[0] * 255.0f, 0.0f, 255.0f);
	m_color[1].A = (u8)Math::Clamp(alpha[1] * 255.0f, 0.0f, 255.0f);
	m_color[2].A = (u8)Math::Clamp(alpha[2] * 255.0f, 0.0f, 255.0f);
	m_color[3].A = (u8)Math::Clamp(alpha[3] * 255.0f, 0.0f, 255.0f);

	ApplyColors();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::Hide()
{
	if (m_spriteOn)
	{
		if (m_sprite != INVALID_HANDLE)
			SpriteMgr__DisableSprite(m_sprite);
		m_spriteOn = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::Show()
{
	if (!m_spriteOn)
	{
		if (m_sprite != INVALID_HANDLE)
			SpriteMgr__EnableSprite(m_sprite);
		m_spriteOn = true;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void SFXFullScreenSprite::ApplyColors()
{
	if (m_sprite != INVALID_HANDLE)
#ifndef PS2
		SpriteMgr__UpdateSprite(m_sprite, 99, SPRITE_DEFAULT_SORT, SHADE_SPRITE, 0, 0, m_color);
#else
	{
		ts_bRGBA fuckyoups2[4];
		fuckyoups2[0] = m_color[0];
		fuckyoups2[1] = m_color[1];
		fuckyoups2[2] = m_color[2];
		fuckyoups2[3] = m_color[3];
		fuckyoups2[0].A /= 2;
		fuckyoups2[1].A /= 2;
		fuckyoups2[2].A /= 2;
		fuckyoups2[3].A /= 2;
		SpriteMgr__UpdateSprite(m_sprite, 99, SPRITE_DEFAULT_SORT, SHADE_SPRITE, 0, 0, fuckyoups2);
	}
#endif
}

