///////////////////////////////////////////////////////////////////////////////
// Decal implementations
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Math/sfilter.h"
#include "Effects/decal/adecal.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
AnimatedDecalEmitter::AnimatedDecalEmitter(
DecalSet &set,
const DecalTextureLayer *layer,
int layers) :
AutomaticDecalEmitter<AnimatedDecal>(set, layer, layers)
{
	SetFilter(0.05f);
	m_updateTail = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Clear all instances
///////////////////////////////////////////////////////////////////////////////
void AnimatedDecalEmitter::Clear()
{
	AutomaticDecalEmitter<AnimatedDecal>::Clear();
	m_updateTail = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Place a decal
///////////////////////////////////////////////////////////////////////////////
bool AnimatedDecalEmitter::Create(
Vector3CRef point,
Vector3CRef normal,
float radius,
float twist,
float uvT[4],
const ts_bRGBA &color,
DecalCoplanarCheck check)
{
	Vector2 uv[4];

	// setup target uv's
	for (int i = 0; i < 4; i++)
		uv[i] = DenormalizeUV(NormalizeUV(m_defaultUV[i]) * uvT[i]);

	// Rotate the up
	Vector3 up = GenerateUp(normal);
	up = RotatePointAroundAxis(up, normal, twist);

	// Make a color array
	ts_bRGBA cornerColor[4];
	for (int c = 0; c < 4; c++)
		cornerColor[c] = color;

	return AutomaticDecalEmitter<AnimatedDecal>::Create(point, normal, up,
																		radius, radius, m_defaultHeightTweak,
																		cornerColor, uv, check);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnimatedDecalEmitter::OnCreate(
int decal,
Vector3CRef point,
Vector3CRef normal,
Vector3CRef up,
float halfWidth,
float halfHeight,
float heightTweak,
ts_bRGBA color[4],
Vector2 uv[4])
{
	// Who are we?
	AnimatedDecal &aDecal = m_decalList[decal];

	// Save off our info
	for (int i = 0; i < 4; i++)
	{
		aDecal.currentUV[i] = NormalizeUV(uv[i]);
		aDecal.targetUV[i] = NormalizeUV(m_defaultUV[i]);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Our update
///////////////////////////////////////////////////////////////////////////////
void AnimatedDecalEmitter::Update(
const CInstance &instance,
const Graphics4x4 &localToCamera,
float zCamDist)
{
	// Anything to do?
	if (m_decals == 0)
		return;

	// Get our time
	float deltaT = g_timer.GetFrameSec();

	// Scan the instances for dead ones
	int onDecal = m_updateTail;
	Lock();
	while (onDecal != m_head)
	{
		// some aliaes
		AnimatedDecal &aDecal = m_decalList[onDecal];
		const int onVertex = onDecal * 4;

		// Update the texture coords
		int cornerDone = 0;
		for (int i = 0; i < 4; i++)
		{
			aDecal.currentUV[i] = FOFilterImplicit(aDecal.currentUV[i], aDecal.targetUV[i], deltaT, m_filterT);
			if ((aDecal.currentUV[i] - aDecal.targetUV[i]).LengthSquared() < 0.01f)
				++cornerDone;
			Vector2 uv = DenormalizeUV(aDecal.currentUV[i]);
			for (int t = 0; t < m_textureLayers; t++)
				m_vertexBuffer.TexCoordv2(t, onVertex + i).Set(uv.X(), uv.Y());
		}

		// All four corners done, stop updating!
		if (cornerDone == 4)
			m_updateTail = NextDecal(m_updateTail);

		// Next decal!
		onDecal = NextDecal(onDecal);
	}
	Unlock();
}

