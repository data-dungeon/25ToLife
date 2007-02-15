///////////////////////////////////////////////////////////////////////////////
// Animated decals
///////////////////////////////////////////////////////////////////////////////
#ifndef ADECAL_H
#define ADECAL_H

#include "Effects/decal/decal.h"

class AnimatedDecal
{
public:
	Vector2		currentUV[4];
	Vector2		targetUV[4];
};

class AnimatedDecalEmitter : public AutomaticDecalEmitter<AnimatedDecal>
{
public:
	AnimatedDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers);
	virtual ~AnimatedDecalEmitter() {}

	// Set the filter value
	void SetFilter(float filterT)
		{ m_filterT = filterT; }

	// Clear all instances
	void Clear();

	// Place a decal
	bool Create(Vector3CRef point, Vector3CRef normal,
						float radius, float twist,
						float uvT[4], const ts_bRGBA &color,
						DecalCoplanarCheck check);

protected:
	// Our params
	float	m_filterT;

	// Start of update
	int	m_updateTail;

	// You write this, it gets called on successful create
	virtual void OnCreate(int decal,
								Vector3CRef point, Vector3CRef normal, Vector3CRef up,
								float halfWidth, float halfHeight, float heightTweak,
								ts_bRGBA color[4], Vector2 uv[4]);

	// Our update
	virtual void Update(const CInstance &instance, const Graphics4x4 &localToCamera, float zCamDist);

	inline Vector2 NormalizeUV(const Vector2 &uv)
		{ return Vector2(uv.X() * 2.0f - 1.0f, uv.Y() * 2.0f - 1.0f); }
	inline Vector2 DenormalizeUV(const Vector2 &uv)
		{ return Vector2((uv.X() + 1.0f) * 0.5f, (uv.Y() + 1.0f) * 0.5f); }
};

#endif
