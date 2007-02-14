/////////////////////////////////////////////////////////////////////////////
// TTL munition handling
/////////////////////////////////////////////////////////////////////////////
#ifndef TTLMUNITION_H
#define TTLMUNITION_H

#include "EngineHelper/Singleton.h"
#include "Effects/decal/rdecal.h"
#include "Environ/Munition.h"

class TTLMunition
{
public:
	TTLMunition();
	~TTLMunition();

	// Call this to send a munition message
	void SendMunitionImpact(ActorHandle shooter, CMunitionImpactSurface &impact);

	// Ack an impact
	void AckMunitionImpact(ActorHandle shooter, CMunitionImpact *impact, CActor *victim, bool ack);

	// Send an explosion!
	void SendExplodeFX(ActorHandle shooter, const SMunitionExplodeFX &fx);

	// Play an SFX
	bool PlaySFX(const char *sfxName, const Vector3 &pos, float volume = 1.0f, float pitch = 0.0f, float presence = 0.0f);

	// Get the "nothing" material
	u8 NothingMaterial();

private:
	// Handle the fx
	void MunitionImpactFX(const CMunitionImpactSurface &hit);

	// Handle the hit
	void MunitionImpact(ActorHandle shooter, CMunitionImpactSurface &hit);

	REF_SEND_MESSAGE_HANDLER(MunitionImpact, TTLMunition) d_munitionImpact;
	REF_SEND_MESSAGE_HANDLER(ExplodeFX, TTLMunition) d_explodeFX;
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, TTLMunition) d_checkpointReset;

	// What is the nothing material
	u8							d_nothingMaterial;

	// The decal emitter!
	DecalSet					d_decalSet;
	RoundDecalEmitter		**d_decalEmitter;
	int						d_decalEmitters;

	// The sound emitters
	SoundEmitter3D			*d_impactSoundEmitter;
	SoundEmitter3D			*d_fxSoundEmitter;

	// A handy utility (why aren't these in the enigne)
	inline ts_bRGBA ToIntensity(u8 shade, u8 alpha = 255);

	// Is this decal okay
	bool DecalOkay(CMunitionImpactSurface &hit);

	// Is this event in view?
	bool InView(Vector3CRef pos, float &d);
};

DefineSingleton(TTLMunition)
#define g_ttlMunition GetSingleton(TTLMunition)

#endif
