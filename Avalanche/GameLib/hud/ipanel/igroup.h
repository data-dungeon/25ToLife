///////////////////////////////////////////////////////////////////////////////
// Simple system for laying out an "Instrument Panel" i.e. HUD. Let's
// each instrument on a panel to be written and maintained separately.
///////////////////////////////////////////////////////////////////////////////
#ifndef IGROUP_H
#define IGROUP_H

#include "container\dynlist.h"
#include "hud\sprite.h"
#include "hud\ipanel\instrument.h"

///////////////////////////////////////
// A group is a named list of
// instruments. The group "owns" the
// instrument objects once given.
///////////////////////////////////////
class InstrumentGroup
{
public:
	InstrumentGroup(const char *groupName);
	~InstrumentGroup() {}

	const char *Name()
		{ return m_name; }

	// Setup the transition for this group
	enum TransitionType
	{
		TRANSITION_NONE = 0x0,
		TRANSITION_FADE = 0x1,
		TRANSITION_SCALE = 0x2
	};
	void SetTransition(float transitionTime, int transitionType = TRANSITION_FADE);

	// Add/remove an instrument (calls delete & I own on add)
	bool Add(Instrument *instrument);
	bool Remove(Instrument *instrument);

	// Get an instrument by name
	Instrument *Find(const char *instrumentName);

	// Show/activate all of these in a group
	bool Show(bool show = true, float transitionTime = -1.0f);
	bool Activate(bool activate = true);

	// Call this once per frame!
	void Update(float dt);

private:
	char									m_name[32];
	Sprite								m_root;
	DoublyLinkedList<Instrument>	m_instruments;

	// These are the setup values
	int											m_transitionType;
	float											m_transitionTime;

	// This is the current transition state
	enum TransitionState
	{
		TRANSITIONSTATE_NONE,
		TRANSITIONSTATE_TOSHOW,
		TRANSITIONSTATE_TOHIDE
	};
	TransitionState							m_transitionState;
	float											m_transitionRate;
	float											m_transitionValue; // ([0..1] from hide->show)

	void ShowImmediate(bool show);
	void UpdateTransitionState(float dt);
	void UpdateTransition(Instrument &instrument);

public:
	InstrumentGroup 					*prev, *next;
};

#endif
