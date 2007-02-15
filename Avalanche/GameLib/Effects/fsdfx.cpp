///////////////////////////////////////////////////////////////////////////////
// Virtualization for a full-screen fx
///////////////////////////////////////////////////////////////////////////////
#include "Effects/effectsPCH.h"

#include "cmdcon/cmdcon.h"
#include "Math/sfilter.h"
#include "Effects/scenesetup.h"
#include "Effects/fullscreendistort.h"
#include "Effects/fsdsys.h"
#include "Effects/fsdfx.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FullScreenFXBase::FullScreenFXBase(const char *name)
{
	ASSERT(name);
	ASSERT(strlen(name) < sizeof(m_name) - 1);
	strcpy(m_name, name);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FullScreenFXSprite::FullScreenFXSprite(const char *name) :
FullScreenFXBase(name)
{
	SetDefaults(Vector4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.0f);
	m_requestSpriteBlend[0] = '\0';
	m_spriteBlendMode = SPRITEBLEND_NORMAL;
	m_sprite = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FullScreenFXSprite::~FullScreenFXSprite()
{
	SFXFullScreenSprite::Destroy(m_sprite);
}

///////////////////////////////////////////////////////////////////////////////
// Setup defaults
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXSprite::SetDefaults(
const Vector4 &color,
float baseNoiseAlpha,
float noiseModulate)
{
	m_color = color;
	m_baseNoiseAlpha = baseNoiseAlpha;
	m_noiseModulate = noiseModulate;
}

///////////////////////////////////////////////////////////////////////////////
// Register our variables
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXSprite::RegisterContext(const char *context)
{
	char var[64];
	sprintf(var, "%sColor", m_name);
	g_console.CreateVar(context, var, &m_color);
	sprintf(var, "%sbaseNoiseAlpha", m_name);
	g_console.CreateVar(context, var, m_baseNoiseAlpha);
	sprintf(var, "%sNoiseModulate", m_name);
	g_console.CreateVar(context, var, m_noiseModulate);
#ifdef PS2
	sprintf(var, "%sBlendPS2", m_name);
#else
	sprintf(var, "%sBlendDX", m_name);
#endif
	g_console.CreateVar(context, var, m_requestSpriteBlend, sizeof(m_requestSpriteBlend) - 1);
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////
bool FullScreenFXSprite::Initialize()
{
	// override the sprite blend mode?
	if (m_requestSpriteBlend[0])
	{
		if (strcmpi(m_requestSpriteBlend, "SOLID") == 0)
			m_spriteBlendMode = SPRITEBLEND_SOLID;
		else if (strcmpi(m_requestSpriteBlend, "NORMAL") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_NORMAL;
		else if (strcmpi(m_requestSpriteBlend, "ADD") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_ADD;
		else if (strcmpi(m_requestSpriteBlend, "SUBTRACT") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_SUBTRACT;
		else if (strcmpi(m_requestSpriteBlend, "MODULATE") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_MODULATE;
		else if (strcmpi(m_requestSpriteBlend, "GLOSS") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_GLOSS;
		else if (strcmpi(m_requestSpriteBlend, "DIFFUSE") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_DIFFUSE;
		else if (strcmpi(m_requestSpriteBlend, "SATURATE") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_SATURATE;
		else if (strcmpi(m_requestSpriteBlend, "DOUBLE") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_DOUBLE;
		else if (strcmpi(m_requestSpriteBlend, "NEGATIVE") == 0)
			m_spriteBlendMode = 	SPRITEBLEND_NEGATIVE;
	}

	m_sprite = SFXFullScreenSprite::Create(m_name, ToColor(m_color), -1.0f, m_spriteBlendMode);
	return (m_sprite != NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Reset
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXSprite::Reset()
{
	if (m_sprite)
		m_sprite->Hide();
}

///////////////////////////////////////////////////////////////////////////////
// Update!
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXSprite::Update(
float intensity,
const Vector3 *pos)
{
	if (m_sprite)
	{
		if (Math::Zero(intensity))
			m_sprite->Hide();
		else
		{
			float alpha;
			m_sprite->Show();
			if	(Math::Zero(m_noiseModulate))
				alpha = Math::Clamp(intensity);
			else
				alpha = Math::Clamp(intensity) * (m_baseNoiseAlpha + (1.0f + g_perlinNoise.Noise(g_timer.GetEndSec())) * m_noiseModulate);
			m_sprite->SetAlpha(Math::Clamp(alpha));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// are we active?
///////////////////////////////////////////////////////////////////////////////
bool FullScreenFXSprite::OnScreen()
{
	return m_sprite && !m_sprite->Hidden();
}

///////////////////////////////////////////////////////////////////////////////
// Utiltiy
///////////////////////////////////////////////////////////////////////////////
ts_bRGBA FullScreenFXSprite::ToColor(const Vector4 &v)
{
	ts_bRGBA c =
	{
		(u8)Math::Clamp(v.X(), 0.0f, 255.0f),
		(u8)Math::Clamp(v.Y(), 0.0f, 255.0f),
		(u8)Math::Clamp(v.Z(), 0.0f, 255.0f),
		(u8)Math::Clamp(v.W(), 0.0f, 255.0f)
	};

	return c;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FullScreenFXDistort::FullScreenFXDistort(const char *name) :
FullScreenFXBase(name)
{
	m_fsdVar[0] = '\0';
	m_fsd = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FullScreenFXDistort::~FullScreenFXDistort()
{
	Delete();
}

///////////////////////////////////////////////////////////////////////////////
// Register our variables
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXDistort::RegisterContext(
const char *context)
{
	char var[64];
	sprintf(var, "%sFSD", m_name);
	g_console.CreateVar(context, var, m_fsdVar, sizeof(m_fsdVar) - 1);
}

///////////////////////////////////////////////////////////////////////////////
// Create
///////////////////////////////////////////////////////////////////////////////
bool FullScreenFXDistort::Initialize()
{
	return (m_fsdVar[0] != '\0');
}

///////////////////////////////////////////////////////////////////////////////
// Reset
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXDistort::Reset()
{
	Delete();
}

///////////////////////////////////////////////////////////////////////////////
// Update!
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXDistort::Update(
float intensity,
const Vector3 *pos)
{
	// Anything to do?
	if (Math::Zero(intensity))
	{
		if (m_fsd)
		{
			if (m_fsd->GetState() == CFSDEffect::FSDFX_DONE)
				Delete();
			else
				m_fsd->SetLoopedSustain(false);
		}
	}
	else // intensity > 0.0f
	{
		// Do we need to start the m_fsd?
		if (!m_fsd || m_fsd->GetState() == CFSDEffect::FSDFX_DONE)
		{
			if (m_fsd)
				Delete();

			m_fsd = g_fsdSystem.StartEffect(m_fsdVar, pos, DISTORT_FLAGS_OWNERDELETE);
			ASSERT(m_fsd);
			if (m_fsd)
				m_fsd->SetLoopedSustain(true);
		}

		if (m_fsd && m_fsd->GetState() == CFSDEffect::FSDFX_DECAY)
		{
			m_fsd->SetLoopedSustain(true);
			m_fsd->ResetToSustain();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FullScreenFXDistort::OnScreen()
{
	return (m_fsd != NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Full screen distort
///////////////////////////////////////////////////////////////////////////////
void FullScreenFXDistort::Delete()
{
	if (m_fsd)
	{
		if (ValidSingleton(CFSDSystem))
			g_fsdSystem.StopEffect(m_fsd);
		// We must delete it!
		delete m_fsd;
		m_fsd = NULL;
	}
}
