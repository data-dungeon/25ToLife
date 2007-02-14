/////////////////////////////////////////////////////////////////////////////
// TTL weapons systems
/////////////////////////////////////////////////////////////////////////////
#ifndef TTLWEAPONS_H
#define TTLWEAPONS_H

#include "EngineHelper/Singleton.h"
#include "Effects/decal/rdecal.h"

class TTLWeapons
{
public:
	TTLWeapons();
	~TTLWeapons();

private:
	// This is private now
	void MunitionImpactFX(const SMunitionImpact &hit);
	REF_SEND_MESSAGE_HANDLER(CMunitionHit, TTLWeapons) d_munitionHit;

	// The decal emitter!
	DecalSet					d_decalSet;
	RoundDecalEmitter		**d_decalEmitter;
	int						d_decalEmitters;

	// A handy utility (why aren't these in the enigne)
	inline ts_bRGBA ToIntensity(u16 color, u8 alpha = 255);
};

#define g_ttlWeapons GetSingleton(TTLWeapons)

#endif
