/////////////////////////////////////////////////////////////////////////////
// Taunt class
/////////////////////////////////////////////////////////////////////////////
#ifndef TAUNT_H
#define TAUNT_H

#include "Components/Component.h"
#include "container/Darray.h"

// Forwards
class CommonStreamInterface;
class CSIHandle;

class Taunt : public CActorComponent
{
public:
	// Pass in a list (terminated with a NULL string)
	// List must be static
	Taunt(CCompActor &actor);
	~Taunt();

	// Our interface support
	const char *ComponentName()
		{ return "Taunt"; }

	// The initialize
	void Initialize();

	// The perframe update
	void EndUpdate(float i_deltaSec);

	// Is a taunt playing?
	bool Playing();

	// Stop taunts from playing (returns true if a taunt was playing)
	bool Stop();

	// How many taunts are there?
	int Count();

	// Play a specific taunt (returns false if already playing)
	bool Say(int index, float volume = 1.0f);

	// Play a random taunt (returns false if already playing)
	bool SayRandom(float volume = 1.0f);

	// Get the last stream played
	const char *GetLastPlayedName();

	// What time was the last taunt played?
	float GetLastPlayedTime()
		{ return m_lastPlayedTime; }

	bool Play(const char *taunt, float volume = 1.0f);

private:
	// Our local info
	DynamicArray<int>				m_taunt;
	int								m_lastPlayed;
	float								m_lastPlayedTime;

	// Our taunt
	CSIHandle						*m_handle;

	// Our message handler
	REF_BASIC_MESSAGE_HANDLER(Say, MESSAGE_VALUE, Taunt) m_sayHandler;

	// Convert an index into a taunt
	const char *GetTaunt(int index);
};

#endif
