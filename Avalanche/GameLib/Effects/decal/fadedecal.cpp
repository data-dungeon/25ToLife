///////////////////////////////////////////////////////////////////////////////
// Static decals require a handle and must be deleted.
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"
#include "Math/sfilter.h"
#include "Effects/decal/fadedecal.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FadeDecalEmitter::FadeDecalEmitter(
DecalSet &set,
const DecalTextureLayer *layer,
int layers,
FadeType fadeType) :
StaticDecalEmitter<FadeExtraData>(set, layer, layers),
m_fadeType(fadeType)
{
}

///////////////////////////////////////////////////////////////////////////////
// Create a decal
///////////////////////////////////////////////////////////////////////////////
StaticDecalHandle FadeDecalEmitter::Create(
const Vector3 corner[4],
const ts_bRGBA color[4],
const Vector2 uv[4],
float fadeTimeConstant,
float alphaTarget)
{
	StaticDecalHandle handle = StaticDecalEmitter<FadeExtraData>::Create(corner, color, uv);
	int index = HandleToIndex(handle);
	if (index < 0)
		return handle;

	// Update our shizna
	if (m_fadeType == FADETYPE_FILTER)
		m_decalList[index].extra.timeConstant = fadeTimeConstant;
	else
		m_decalList[index].extra.fadeRate = fadeTimeConstant;
	m_decalList[index].extra.alpha = 0.0f;
	m_decalList[index].extra.alphaTarget = alphaTarget;
	SetFadeState(m_decalList[index], DECALFADE_IN);
	return handle;
}

///////////////////////////////////////////////////////////////////////////////
// Create a decal using all defaults
///////////////////////////////////////////////////////////////////////////////
StaticDecalHandle FadeDecalEmitter::Create(
const Vector3 corner[4],
float fadeTimeConstant,
float alphaTarget)
{
	return Create(corner, m_defaultColor, m_defaultUV, fadeTimeConstant, alphaTarget);
}

///////////////////////////////////////////////////////////////////////////////
// Create a decal using all defaults
///////////////////////////////////////////////////////////////////////////////
StaticDecalHandle FadeDecalEmitter::Create(
const Vector3 corner[4],
const ts_bRGBA color[4],
float fadeTimeConstant,
float alphaTarget)
{
	return Create(corner, color, m_defaultUV, fadeTimeConstant, alphaTarget);
}

///////////////////////////////////////////////////////////////////////////////
// Free a decal!
///////////////////////////////////////////////////////////////////////////////
bool FadeDecalEmitter::Destroy(
StaticDecalHandle handle,
float fadeTimeConstant,
float alphaTarget)
{
	int index = HandleToIndex(handle);
	if (index < 0)
		return false;

	// Update our shizna
	if (m_fadeType == FADETYPE_FILTER)
		m_decalList[index].extra.timeConstant = fadeTimeConstant;
	else
		m_decalList[index].extra.fadeRate = fadeTimeConstant;
	m_decalList[index].extra.alphaTarget = alphaTarget;
	SetFadeState(m_decalList[index], DECALFADE_OUT);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// You write this, gets called per render
///////////////////////////////////////////////////////////////////////////////
void FadeDecalEmitter::Update(
const CInstance &instance,
const Graphics4x4 &localToCamera,
float zCamDist)
{
	// duh
	if (m_decals == 0)
		return;

	// How much time has elapsed
	float deltaT = g_timer.GetFrameSec();

	// Update all of them
	Lock();
	for (int i = 0; i < m_maxDecals; i++)
	{
		// Do we need to to any work?
		if (!Allocated(i) || Hidden(i))
			continue;

		// What is the fade state
		PerDecal &decal = m_decalList[i];
		FadeFlags fade = GetFadeState(decal);
		if (fade == DECALFADE_NONE)
			continue;

		// Update the alpha
		if (m_fadeType == FADETYPE_FILTER)
			decal.extra.alpha = FOFilterImplicit(decal.extra.alpha, decal.extra.alphaTarget, deltaT, decal.extra.timeConstant);
		else
			Math::MoveToValue(&decal.extra.alpha, decal.extra.alphaTarget, deltaT / decal.extra.fadeRate);

		// Alpply the alpha
		u8 alpha = (u8)Math::Clamp(decal.extra.alpha * 255.0f, 0.0f, 255.0f);
		decal.color[0].A = alpha;
		decal.color[1].A = alpha;
		decal.color[2].A = alpha;
		decal.color[3].A = alpha;

		// Munch the vertex buffer
		int onVertex = i * 4;
		m_vertexBuffer.Diffuse(onVertex++) = VtxBuffer::cvtRGBA(*((int *)&decal.color[0]));
		m_vertexBuffer.Diffuse(onVertex++) = VtxBuffer::cvtRGBA(*((int *)&decal.color[1]));
		m_vertexBuffer.Diffuse(onVertex++) = VtxBuffer::cvtRGBA(*((int *)&decal.color[2]));
		m_vertexBuffer.Diffuse(onVertex++) = VtxBuffer::cvtRGBA(*((int *)&decal.color[3]));

		// Are wedone
		if (FadeDone(decal))
		{
			if (fade == DECALFADE_OUT)
				Free(i);
			else
				SetFadeState(decal, DECALFADE_NONE);
		}
	}

	// Outta here
	Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// You write this, it gets called on successful create
///////////////////////////////////////////////////////////////////////////////
void FadeDecalEmitter::OnCreate(
int index,
PerDecal &decal)
{
	decal.extra.timeConstant = 0.0f;
	decal.extra.alpha = 1.0f;
	decal.extra.alphaTarget = 1.0f;
	SetFadeState(decal, DECALFADE_NONE);
}
