///////////////////////////////////////////////////////////////////////////////
// Implementations of decal stuff
///////////////////////////////////////////////////////////////////////////////
#ifndef RDECAL_H
#define RDECAL_H

#include "Effects/decal/decal.h"

// A real usable class!
class RoundDecalEmitter : public TTLDecalEmitter
{
public:
	RoundDecalEmitter(DecalSet &set, const DecalTextureLayer *layer, int layers, float ttl = 0.0f);
	virtual ~RoundDecalEmitter();

	// Will this decal work?
	bool CheckCoplanar(Vector3CRef point, Vector3CRef normal, float radius,
								DecalCoplanarCheck check = DECAL_COPLANARCHECK_CHEAP);

	// Place a decal
	bool Create(Vector3CRef point, Vector3CRef normal, float radius, const ts_bRGBA *color, DecalCoplanarCheck check, float rotateRadians = 0.0f);
};

#endif
