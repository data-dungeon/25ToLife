/////////////////////////////////////////////////////////////////////////////
// Weapons that affect the user: i.e. flashbangs, smoke, teargas, stunned
/////////////////////////////////////////////////////////////////////////////
#ifndef AFFLICTION_H
#define AFFLICTION_H

#include "Components/Component.h"

class Afflictions : public CActorComponent
{
public:
	// Pass in a list (terminated with a NULL string)
	// List must be static
	Afflictions(CCompActor &actor);
	~Afflictions() {}

	// Our interface support
	const char *ComponentName()
		{ return "Afflictions"; }

	// The initialize
	void Initialize();

	// Reset!
	void Reset();

	// What is our current state? (many can be set)
	enum Affliction
	{
		BLIND,
		DEAF,
		INSMOKE,
		TEARGAS,
		STUNNED,
		ONFIRE,
		SHOCK,

		AFFLICTIONS
	};

	// Get the strength of the affliction, zero means not affected
	float GetStrength(Affliction affliction)
		{ return m_affliction[affliction].strength; }

	// Get the remaining duration of the affliciton
	// < 0.0f means forever!
	float GetDuration(Affliction affliction)
		{ return m_affliction[affliction].duration; }

	// Are they afflicted with this
	bool Afflicted(Affliction affliction)
		{ return (!Math::Zero(GetStrength(affliction))); }

	// What is the source of this affliction (may not be relevant)
	const Vector3 &GetSource(Affliction affliction)
		{ return m_affliction[affliction].source; }

	// Update the afflictions
	void EndUpdate(float i_deltaSec);

private:
	struct AfflictionInfo
	{
		float		strength;
		float		duration;
		int		inflictorID;
		Vector3	source;
	};
	AfflictionInfo		m_affliction[AFFLICTIONS];

	// Clear an affliction
	void Clear(Affliction affliction, bool sendEndAffliction = true);

	// Set an affliction!
	void Set(Affliction affliction, float strength, float duration, int inflictorID, const Vector3 *source = NULL);
	void Set(Affliction affliction, float strength, int inflictorID, const Vector3 *source = NULL);

	// How do we modulate afflictions
	enum UpdateType
	{
		AUT_NONE,
		AUT_LINEAR,
		AUT_FILTERED
	};
	struct UpdateInfo
	{
		UpdateType	type;
		float			tc;
	};
	static UpdateInfo s_afflictionUpdate[AFFLICTIONS];
	void AfflictionUpdate(Affliction affliction, float deltaT);

	REF_SEND_MESSAGE_HANDLER(FlashBang, Afflictions) m_flashBangHandler;
	REF_BASIC_MESSAGE_HANDLER(Deafness, MESSAGE_VALUE, Afflictions) m_deafnessHandler;
	REF_BASIC_MESSAGE_HANDLER(Stun, MESSAGE_VALUE, Afflictions) m_stunHandler;
	REF_BASIC_MESSAGE_HANDLER(Fire, MESSAGE_VALUE, Afflictions) m_fireHandler;
	REF_BASIC_MESSAGE_HANDLER(Smoke, MESSAGE_VALUE, Afflictions) m_smokeHandler;
	REF_BASIC_MESSAGE_HANDLER(TearGas, MESSAGE_VALUE, Afflictions) m_tearGasHandler;
	REF_BASIC_MESSAGE_HANDLER(Shock, MESSAGE_VALUE, Afflictions) m_shockHandler;

	bool m_invincible;
};

#endif
