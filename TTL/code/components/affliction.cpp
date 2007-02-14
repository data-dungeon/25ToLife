/////////////////////////////////////////////////////////////////////////////
// Things that affect a combatant in the game
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Math/sfilter.h"
#include "Components/ComponentCreator.h"
#include "components/affliction.h"
#include "Components/weapons/projlib.h"
#include "weapons/fireproj.h"
#include "weapons/shockproj.h"

STANDARD_COMPONENT_CREATOR(Afflictions, Afflictions, COMPONENT_NEUTRAL_PRIORITY);

Afflictions::UpdateInfo Afflictions::s_afflictionUpdate[AFFLICTIONS] =
{
	{ AUT_FILTERED, 	0.7f },		// BLIND
	{ AUT_NONE, 		-1.0f },		// DEAF
	{ AUT_LINEAR,		2.0f },		// SMOKE
	{ AUT_LINEAR,		3.0f }, 		// TEARGAS
	{ AUT_NONE, 		-1.0f },		// STUNNED
	{ AUT_NONE, 		-1.0f },		// ONFIRE
	{ AUT_NONE, 		-1.0f },		// SHOCK
};

/////////////////////////////////////////////////////////////////////////////
// Pass in a list (terminated with a NULL string)
/////////////////////////////////////////////////////////////////////////////
Afflictions::Afflictions(
CCompActor &owner) :
CActorComponent(owner)
{
	for (int i = 0; i < AFFLICTIONS; i++)
		m_affliction[i].strength = 0.0f;

	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// Reset!
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Reset()
{
	for (int i = 0; i < AFFLICTIONS; i++)
		Clear((Affliction)i);
}

/////////////////////////////////////////////////////////////////////////////
// The initialize
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Initialize()
{
	// Call the parent
	CActorComponent::Initialize();

	m_flashBangHandler.Initialize("MunitionFlash",
											MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
											actor.GetMessageOwner(), this);

	m_deafnessHandler.Initialize("MunitionDeaf",
											MESSAGE_HANDLE_IF_MASTER | MESSAGE_ROUTE_TO_MASTER,
											actor.GetMessageOwner(), this);

	m_stunHandler.Initialize("MunitionStun",
											MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS,
											actor.GetMessageOwner(), this);

	m_fireHandler.Initialize("MunitionOnFire",
											MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS,
											actor.GetMessageOwner(), this);

	m_smokeHandler.Initialize("InSmoke",
										MESSAGE_HANDLE_IF_MASTER,
										actor.GetMessageOwner(), this);

	m_tearGasHandler.Initialize("InTearGas",
										MESSAGE_HANDLE_IF_MASTER,
										actor.GetMessageOwner(), this);

	m_shockHandler.Initialize("MunitionShock",
										MESSAGE_HANDLE_IF_MASTER | MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS,
										actor.GetMessageOwner(), this);

	if(actor.Parm().GetInt("invincible") == 1)
		m_invincible = true;
	else
		m_invincible = false;
}

/////////////////////////////////////////////////////////////////////////////
// Update the afflictions
/////////////////////////////////////////////////////////////////////////////
void Afflictions::EndUpdate(
float i_deltaSec)
{
	HealthIF *healthIF = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	if (healthIF)
	{
		bool alive = healthIF->IsAlive();
		bool onFire = Afflicted(ONFIRE);

		// Affect health
		if (actor.IsNetMaster() && alive && onFire)
			healthIF->DoDamage(m_affliction[ONFIRE].strength * i_deltaSec, HealthIF::ONFIRE, m_affliction[ONFIRE].inflictorID);

		// Burning corpses are cool
		if (!alive && onFire)
			m_affliction[ONFIRE].duration += 1.0f;
	}

	for (int i = 0; i < AFFLICTIONS; i++)
		AfflictionUpdate((Affliction)i, i_deltaSec);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::AfflictionUpdate(
Affliction affliction,
float deltaT)
{
	// Anything to do?
	if (Math::Zero(m_affliction[affliction].strength))
		return;

	// Modify strength
	if (s_afflictionUpdate[affliction].type != AUT_NONE)
	{
		if (s_afflictionUpdate[affliction].type == AUT_LINEAR)
			Math::MoveToZero(&m_affliction[affliction].strength, deltaT / s_afflictionUpdate[affliction].tc);
		else
		 	m_affliction[affliction].strength = FOFilterImplicit(m_affliction[affliction].strength, 0.0f, deltaT, s_afflictionUpdate[affliction].tc);

		if (Math::Zero(m_affliction[affliction].strength, 0.005f))
			Clear(affliction, true);
	}

	// Update the time left
	if (m_affliction[affliction].duration > 0.0f)
	{
		Math::MoveToZero(&m_affliction[affliction].duration, deltaT);
		if (Math::Zero(m_affliction[affliction].duration))
			Clear(affliction, true);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Clear an affliction
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Clear(Affliction affliction, bool forceSend)
{
	bool sendMessage = (forceSend ||
								!Math::Zero(m_affliction[affliction].strength) ||
								m_affliction[affliction].duration > 0.0f);

	m_affliction[affliction].strength = 0.0f;
	m_affliction[affliction].duration = 0.0f;
	m_affliction[affliction].source.Clear();

	if (sendMessage)
		g_messageDispatcher.SendMessageToActor("endaffliction", (void *)affliction, actor.GetHandle(), actor.GetHandle());
}

/////////////////////////////////////////////////////////////////////////////
// Set an affliction!
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Set(
Affliction affliction,
float strength,
float duration,
int inflictorID,
const Vector3 *source)
{
	bool sendMessage = Math::Zero(m_affliction[affliction].strength) && !Math::Zero(strength);

	m_affliction[affliction].strength = strength;
	m_affliction[affliction].duration = duration;
	m_affliction[affliction].inflictorID = inflictorID;
	if (source)
		m_affliction[affliction].source = *source;
	else
		m_affliction[affliction].source = actor.GetBodyInWorld();

	if (sendMessage)
		g_messageDispatcher.SendMessageToActor("startaffliction", (void *)affliction, actor.GetHandle(), actor.GetHandle());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Set(
Affliction affliction,
float strength,
int inflictorID,
const Vector3 *source)
{
	bool sendMessage = Math::Zero(m_affliction[affliction].strength) && !Math::Zero(strength);

	m_affliction[affliction].strength = strength;
	m_affliction[affliction].duration = -1.0f;
	m_affliction[affliction].inflictorID = inflictorID;
	if (source)
		m_affliction[affliction].source = *source;
	else
		m_affliction[affliction].source = actor.GetBodyInWorld();

	if (sendMessage)
		g_messageDispatcher.SendMessageToActor("startaffliction", (void *)affliction, actor.GetHandle(), actor.GetHandle());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::FlashBang::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;

	SMunitionFlash *flash = (SMunitionFlash *)i_data;
	ASSERT(flash);
	d_ref->Set(BLIND, flash->d_intensity, CParticipantIF::GetParticipantIdFromActor(i_sender), &flash->d_pos);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Deafness::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;
	float duration = CMessageDispatcher::DataToFloat(i_data);
	d_ref->Set(DEAF, 1.0f, duration, CParticipantIF::GetParticipantIdFromActor(i_sender));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Stun::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;
	float duration = CMessageDispatcher::DataToFloat(i_data);
	d_ref->Set(STUNNED, 1.0f, duration, CParticipantIF::GetParticipantIdFromActor(i_sender));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Fire::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;
	int munitionType = CMessageDispatcher::DataToInt(i_data);

	// This is for testing
	if (munitionType == -1)
		d_ref->Set(ONFIRE, 1.0f, 8.0f, CParticipantIF::GetParticipantIdFromActor(i_sender));
	else // This is the real code
	{
		FireProjectileProperties *projectile = (FireProjectileProperties *)g_projectileLib.GetProperties(munitionType);
		ASSERT(projectile);
		if (projectile)
			d_ref->Set(ONFIRE, projectile->m_fireDamage, projectile->m_fireDuration, CParticipantIF::GetParticipantIdFromActor(i_sender));
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Smoke::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;
	float d = CMessageDispatcher::DataToFloat(i_data);
	const float minSmoke = 0.4f;
	d_ref->Set(INSMOKE, Math::Max(minSmoke, d), CParticipantIF::GetParticipantIdFromActor(i_sender));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::TearGas::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;
	float d = CMessageDispatcher::DataToFloat(i_data);
	const float minTearGas = 0.8f;
	d_ref->Set(TEARGAS, Math::Max(minTearGas, d), CParticipantIF::GetParticipantIdFromActor(i_sender));
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Afflictions::Shock::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	if(d_ref->m_invincible)
		return;
	int munitionType = CMessageDispatcher::DataToInt(i_data);
	ShockProjectileProperties *projectile = (ShockProjectileProperties *)g_projectileLib.GetProperties(munitionType);
	ASSERT(projectile);
	if (projectile)
		d_ref->Set(SHOCK, 1.0f, projectile->m_shockDuration, CParticipantIF::GetParticipantIdFromActor(i_sender));
}

