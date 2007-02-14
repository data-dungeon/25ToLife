/////////////////////////////////////////////////////////////////////////////
// Simple footfall component
/////////////////////////////////////////////////////////////////////////////
#ifndef FOOTFALL_H
#define FOOTFALL_H

#include "Components/Component.h"
#include "Effects/footstep.h"

//#define FOOTFALL_SORT

struct FootfallParams
{
	float speedDivisor;
	float minVolume;

	float bloodyPrintWidth, bloodyPrintLength;
	float	bloodyAngleJitter;
	int	bloodyStepFade;
	float	bloodyStepMinAlpha;
	float	bloodyStanceWidth;
};

class Footfall : public CActorComponent
{
public:
	// Pass in a list (terminated with a NULL string)
	// List must be static
	Footfall(CCompActor &actor);
	~Footfall();

	// Our interface support
	const char *ComponentName()
		{ return "Footfall"; }

	// The initialize
	void Initialize();

	// Reset
	void Reset();

	// The update
	void EndUpdate(float i_deltaSec);

	enum Foot
	{
		FOOTFALL_RIGHT,
		FOOTFALL_LEFT
	};
	enum Type
	{
		FOOTFALL_WALK,
		FOOTFALL_RUN,
		FOOTFALL_SIDESTEP,
		FOOTFALL_LANDING
	};

	// Notify of a footfall
	void Step(Type type, Foot foot);

private:
	FootfallParams		*m_params;
	SoundHandle			m_footFall;
	FootstepEmitter   *m_bloodyFootstep;
	int					m_bloodyFootsteps;

	// Landed from a jump!
	void Landed(const char *type);

	// Get at ground material
	u8 FindMaterial(Type type);

	// The message handler
	REF_BASIC_MESSAGE_HANDLER(BloodStep, MESSAGE_VALUE, Footfall) m_bloodStep;
	REF_BASIC_MESSAGE_HANDLER(CheckpointReset, MESSAGE_VALUE, Footfall) m_checkpointResetHandler;
	REF_BASIC_MESSAGE_HANDLER(Landed, MESSAGE_VALUE, Footfall) m_landedHandler;

	// The lookups
	struct StepLookup
	{
		const char *materialName;
		u8				material;
		const char *soundPrefix;
	};
	static StepLookup		s_stepLookup[];
	static int				s_stepLookups;
	static void SetupStepLookup();
	static const char *StepSoundPrefix(u8 material);
#ifdef FOOTFALL_SORT
	static int __cdecl CompareLookup(const void *a, const void *b);
#endif

	// Track the people using the above table to make sure it is
	// initialized every level load
	static int 				s_references;
	static void AquireReference()
		{ if (s_references++ == 0) SetupStepLookup(); }
	static void ReleaseReference()
		{ --s_references; ASSERT(s_references >= 0); }
};

#endif
