///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "hud/HudPCH.h"

#include "enginehelper/timer.h"
#include "hud/ipanel/igroup.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InstrumentGroup::InstrumentGroup(const char *groupName) :
m_instruments(true)
{
	ASSERT(groupName);
	ASSERT(strlen(groupName) > 0 && strlen(groupName) < sizeof(m_name) - 1);
	strcpy(m_name, groupName);

	m_transitionState = TRANSITIONSTATE_NONE;
	m_transitionRate = 0.0f;
	m_transitionValue = 1.0f;

	// Defaults
	SetTransition(2.0f, TRANSITION_FADE);
}

//////////////////////////////////////////////////////////////////////////////
// Setup the transition for this group
//////////////////////////////////////////////////////////////////////////////
void InstrumentGroup::SetTransition(
float transitionTime,
int transitionType)
{
	m_transitionTime = transitionTime;
	m_transitionType = transitionType;
}

//////////////////////////////////////////////////////////////////////////////
// Add/remove an instrument (calls delete & I own on add)
//////////////////////////////////////////////////////////////////////////////
bool InstrumentGroup::Add(Instrument *instrument)
{
	if (!instrument)
		return false;

	// Give them the root sprite
	instrument->SetRoot(m_root);

	// they are in our list
	m_instruments.AddTail(instrument);

	// Weee!
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// Add/remove an instrument (calls delete & I own on add)
//////////////////////////////////////////////////////////////////////////////
bool InstrumentGroup::Remove(Instrument *instrument)
{
	if (!instrument)
		return false;

	m_instruments.Remove(instrument);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// Get an instrument by name
///////////////////////////////////////////////////////////////////////////////
Instrument *InstrumentGroup::Find(
const char *instrumentName)
{
	Instrument *instrument = m_instruments.Head();
	while (instrument)
	{
		if (strcmpi(instrument->Name(), instrumentName) == 0)
			return instrument;
		instrument = instrument->next;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Show/activate
///////////////////////////////////////////////////////////////////////////////
bool InstrumentGroup::Show(
bool show,
float transitionTime)
{
	// Is this a transitionless show?
	float useTransitionTime = transitionTime < 0.0f ? m_transitionTime : transitionTime;
	if (Math::Zero(useTransitionTime))
	{
		ShowImmediate(show);
		m_transitionValue = show ? 1.0f : 0.0f;
	}
	else
	{
		// Change the state?
		if (show && m_transitionState != TRANSITIONSTATE_TOSHOW)
		{
			m_transitionState = TRANSITIONSTATE_TOSHOW;
			m_transitionRate = 1.0f / useTransitionTime;
			UpdateTransitionState(0.0f);
			ShowImmediate(true); // have to turn them on now
		}
		else if (!show && m_transitionState != TRANSITIONSTATE_TOHIDE)
		{
			m_transitionState = TRANSITIONSTATE_TOHIDE;
			m_transitionRate = 1.0f / useTransitionTime;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool InstrumentGroup::Activate(
bool activate)
{
	Instrument *instrument = m_instruments.Head();
	while (instrument)
	{
		instrument->Activate(activate);
		instrument = instrument->next;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Call this once per frame!
///////////////////////////////////////////////////////////////////////////////
void InstrumentGroup::Update(
float dt)
{
	// Are we in a transition?
	UpdateTransitionState(g_timer.GetOSFrameSec());

	Instrument *instrument = m_instruments.Head();
	while (instrument)
	{
		UpdateTransition(*instrument);
		instrument->Update(dt);
		instrument = instrument->next;
	}

	// Wee!
	m_root.Update();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InstrumentGroup::ShowImmediate(bool show)
{
	Instrument *instrument = m_instruments.Head();
	while (instrument)
	{
		instrument->Show(show);
		instrument = instrument->next;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InstrumentGroup::UpdateTransitionState(float dt)
{
	if (dt < 0.0f)
		dt = 9999999.9f;
	if (m_transitionState == TRANSITIONSTATE_TOSHOW)
	{
		Math::MoveToValue(&m_transitionValue, 1.0f, m_transitionRate * dt);
		if (Math::Equal(m_transitionValue, 1.0f))
		{
			ShowImmediate(true);
			m_transitionState = TRANSITIONSTATE_NONE;
		}
	}
	else if (m_transitionState == TRANSITIONSTATE_TOHIDE)
	{
		Math::MoveToZero(&m_transitionValue, m_transitionRate * dt);
		if (Math::Zero(m_transitionValue))
		{
			m_transitionState = TRANSITIONSTATE_NONE;
			ShowImmediate(false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void InstrumentGroup::UpdateTransition(
Instrument &instrument)
{
	if (m_transitionType & TRANSITION_FADE)
		instrument.SetBaseAlpha(m_transitionValue);
	if (m_transitionType & TRANSITION_SCALE)
		instrument.SetBaseScale(m_transitionValue);
}

