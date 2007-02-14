///////////////////////////////////////////////////////////////////////////////
// Simple text based instrument
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "components/interactionmonitor.h"
#include "hud/iinteract.h"

#define INTERACTION_WIDTH		48.0f
#define INTERACTION_HEIGHT		48.0f
#define INTERACTION_TEXTGAP	2.0f
#define BASE_ALPHA				0.75f
#define BASELINE_OFFSET			64.0f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
InteractInstrument::InteractInstrument() :
SpriteInstrument("interact")
{
	m_sprite.AddChild(&m_interact.GetSprite());
	m_sprite.AddChild(&m_text);
	m_text.SetOrigin(0.5f, 0.0f);
	m_text.SetPosition(0.0f, -BASELINE_OFFSET + INTERACTION_TEXTGAP);
	m_text.SetAlpha(BASE_ALPHA);

 	m_text.SetBlendMode(SPRITEBLEND_ADD);
	m_interact.GetSprite().SetBlendMode(SPRITEBLEND_ADD);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 InteractInstrument::Extents()
{
	return Vector2(0.0f, 0.0f);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void InteractInstrument::Update(float dt)
{
	// Anything to do?
	if (!m_hidden && m_active)
	{
		// Who are we looking at
		CCompActor *who = g_hud.GetSubject();

		// Get the interaction dbl
		const char *dbl = GetInteractDBL(who);
		const char *text = GetInteractText(who);

		// Do this!
		if (dbl)
			m_interact.SetTextureDB(dbl);
		if (dbl && text)
			m_text.SetText(text);

		// Alias the sprite
		Sprite &m_interactSprite = m_interact.GetSprite();

		// Do we need to to any work
		if (m_interact.Process() == SSS_INITIALIZE)
		{
			// We should do something in here
			m_interactSprite.SetOrigin(0.0f, 0.0f);
			m_interactSprite.SetSize(INTERACTION_WIDTH, INTERACTION_HEIGHT);
			m_interactSprite.SetPosition(-INTERACTION_WIDTH * 0.5f, -INTERACTION_HEIGHT - BASELINE_OFFSET);
			m_interactSprite.SetAlpha(BASE_ALPHA);
		}

		// Make sure everything is cool
		if (dbl)
			m_interactSprite.ShowAll();
		else
			m_interactSprite.HideAll();
		if (!text)
			m_text.HideAll();
		else
			m_text.ShowAll();
	}

	// Do the rest of the wrok
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char *InteractInstrument::GetInteractDBL(
CCompActor *who)
{
	// Who's on first
	if (!who)
		return NULL;

	InteractionMonitor *monitor = (InteractionMonitor *)who->GetComponentByName("InteractionMonitor");
	if (!monitor)
		return NULL;

	switch (monitor->GetInteractionType())
	{
		case InteractionMonitor::INTERACTION_DOOR_LOCKED:
			return "newhud/idoorloc.dbl";
		case InteractionMonitor::INTERACTION_DOOR:
		case InteractionMonitor::INTERACTION_DOOR_SLIDE_OPEN:
		case InteractionMonitor::INTERACTION_DOOR_SLIDE_CLOSE:
		case InteractionMonitor::INTERACTION_DOOR_KICK_OPEN:
		case InteractionMonitor::INTERACTION_DOOR_KICK_CLOSE:
		case InteractionMonitor::INTERACTION_DOOR_PULL_CLOSE:
			return "newhud/idoor.dbl";
		case InteractionMonitor::INTERACTION_LADDER:
			return "newhud/iladder.dbl";
		case InteractionMonitor::INTERACTION_FENCE:
			return "newhud/ifence.dbl";
		case InteractionMonitor::INTERACTION_TAG_FRAME:
			return "newhud/itag.dbl";
		case InteractionMonitor::INTERACTION_CLIMB:
		case InteractionMonitor::INTERACTION_VAULT:
			return "newhud/ivault.dbl";
		case InteractionMonitor::INTERACTION_LIGHT_SWITCH_ON:
		case InteractionMonitor::INTERACTION_LIGHT_SWITCH_OFF:
		case InteractionMonitor::INTERACTION_SWITCH_OPEN:
		case InteractionMonitor::INTERACTION_SWITCH_CLOSE:
			return "newhud/iswitch.dbl";
		case InteractionMonitor::INTERACTION_ELEVATOR_LOCKED:
		case InteractionMonitor::INTERACTION_ELEVATOR_SWITCH:
			return "newhud/ielevato.dbl";
		case InteractionMonitor::INTERACTION_HOSTAGE:
		case InteractionMonitor::INTERACTION_RELEASEHOSTAGE:
			return "newhud/ihostage.dbl";
		case InteractionMonitor::INTERACTION_APPREHEND:
			return "newhud/iapprehe.dbl";
		case InteractionMonitor::INTERACTION_SWAP_WEAPON:
			return "newhud/iweapons.dbl";
		case InteractionMonitor::INTERACTION_SWAP_LOADOUT:
			return "newhud/iweapon.dbl";
		case InteractionMonitor::INTERACTION_JUKEBOX_AUDIO_TRAC:
			return "newhud/ijukebox.dbl";
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char *InteractInstrument::GetInteractText(
CCompActor *who)
{
	// Who's on first
	if (!who)
		return NULL;

	InteractionMonitor *monitor = (InteractionMonitor *)who->GetComponentByName("InteractionMonitor");
	if (!monitor)
		return NULL;

	switch (monitor->GetInteractionType())
	{
		case InteractionMonitor::INTERACTION_DOOR_LOCKED:
			return "Locked";
		case InteractionMonitor::INTERACTION_DOOR:
		case InteractionMonitor::INTERACTION_DOOR_SLIDE_OPEN:
		case InteractionMonitor::INTERACTION_DOOR_KICK_OPEN:
			return "Open";
		case InteractionMonitor::INTERACTION_DOOR_KICK_CLOSE:
		case InteractionMonitor::INTERACTION_DOOR_PULL_CLOSE:
		case InteractionMonitor::INTERACTION_DOOR_SLIDE_CLOSE:
			return "Close";
		case InteractionMonitor::INTERACTION_LADDER:
		case InteractionMonitor::INTERACTION_FENCE:
			return "Climb";
		case InteractionMonitor::INTERACTION_TAG_FRAME:
			return "Tag";
		case InteractionMonitor::INTERACTION_CLIMB:
		case InteractionMonitor::INTERACTION_VAULT:
			return "Vault";
		case InteractionMonitor::INTERACTION_LIGHT_SWITCH_ON:
		case InteractionMonitor::INTERACTION_LIGHT_SWITCH_OFF:
		case InteractionMonitor::INTERACTION_SWITCH_OPEN:
		case InteractionMonitor::INTERACTION_SWITCH_CLOSE:
		case InteractionMonitor::INTERACTION_ELEVATOR_LOCKED:
		case InteractionMonitor::INTERACTION_ELEVATOR_SWITCH:
			return "Switch";
		case InteractionMonitor::INTERACTION_HOSTAGE:
		case InteractionMonitor::INTERACTION_RELEASEHOSTAGE:
			return "Hostage";
		case InteractionMonitor::INTERACTION_APPREHEND:
			return "Apprehend";
		case InteractionMonitor::INTERACTION_SWAP_WEAPON:
		case InteractionMonitor::INTERACTION_SWAP_LOADOUT:
			return monitor->GetInteractionText();
		case InteractionMonitor::INTERACTION_JUKEBOX_AUDIO_TRAC:
			return "Next song";
	}

	return NULL;
}

//#undef so the SN pch won't take a shit
#undef INTERACTION_WIDTH
#undef INTERACTION_HEIGHT
#undef INTERACTION_TEXTGAP
#undef BASE_ALPHA
#undef BASELINE_OFFSET

