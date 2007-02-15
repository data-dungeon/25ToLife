///////////////////////////////////////////////////////////////////////////////
// Decal implementations
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"
#include "Effects/decal/rdecal.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoundDecalEmitter::RoundDecalEmitter(
DecalSet &set,
const DecalTextureLayer *layer,
int layers,
float ttl) :
TTLDecalEmitter(set, layer, layers, ttl)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RoundDecalEmitter::~RoundDecalEmitter()
{
}

///////////////////////////////////////////////////////////////////////////////
// Will this decal work?
///////////////////////////////////////////////////////////////////////////////
bool RoundDecalEmitter::CheckCoplanar(
Vector3CRef point,
Vector3CRef normal,
float radius,
DecalCoplanarCheck check)
{
	Vector3 up = GenerateUp(normal);
	return TTLDecalEmitter::CheckCoplanar(point, normal, up, radius, radius, m_defaultHeightTweak, check);
}

///////////////////////////////////////////////////////////////////////////////
// Place a decal
///////////////////////////////////////////////////////////////////////////////
bool RoundDecalEmitter::Create(
Vector3CRef point,
Vector3CRef normal,
float radius,
const ts_bRGBA *color,
DecalCoplanarCheck check,
float rotateRadians)
{
	Vector3 up = GenerateUp(normal);
	if (!Math::Zero(rotateRadians))
		up = RotatePointAroundAxis(up, normal, rotateRadians);

	// Make a color array
	if (!color)
		return TTLDecalEmitter::Create(point, normal, up, radius, radius, m_defaultHeightTweak, NULL, m_defaultUV, check);
	else
	{
		ts_bRGBA cornerColor[4];
		for (int c = 0; c < 4; c++)
			cornerColor[c] = *color;
		return TTLDecalEmitter::Create(point, normal, up, radius, radius, m_defaultHeightTweak, cornerColor, m_defaultUV, check);
	}
}

