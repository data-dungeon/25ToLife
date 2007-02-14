///////////////////////////////////////////////////////////////////////////////
// Radar!!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "math/sfilter.h"
#include "interfaces/viewif.h"
#include "sim/referee.h"
#include "sim/teamingmodel.h"
#include "interfacesTTL/healthIF.h"
#include "environ/ConfigMgr.h"
#include "InterfacesTTL/ObjectiveIF.h"
#include "environ/PlayerSpawnMgr.h"
#include "voice/netz/cvoicechat.h"
#include "NetZTTL/DOPlayerAgent.h"
#include "hud/iradar.h"

#define RADAR_SCALE 1.0f
#define RADAR_ALPHA 0.6f

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RadarInstrument::RadarInstrument() :
SpriteInstrument("radar"),
m_persistentBlips(false)
{
	m_textureDB = dblLoad("newhud/radar.dbl");
	ASSERT(m_textureDB != INVALID_HANDLE);

	// Range in meters the radar can see
	m_maxRange = 25.0f;

	// The radius!
	m_radius = 0.0f;

	m_oldestFired = 0;
	int i;
	for (i = 0; i < MAX_FIRED_INDICATORS; i++)
	{
		m_fired[i].SetTextureSet(m_textureDB, 1);
		m_fired[i].Reset();
		m_fired[i].SetOrigin(0.5f, 1.9f);
		m_firedAlpha[i] = 0.0f;
		m_fired[i].SetAlpha(0.0f);
		m_fired[i].SetPriority(96);
		m_firedPos[i].Clear();
		m_sprite.AddChild(&m_fired[i]);
	}

	m_overlay.SetTextureSet(m_textureDB, 8);
	m_overlay.Reset();
	m_overlay.SetOrigin(0.5f, 0.5f);
	m_overlay.SetPriority(97);
	m_overlay.SetAlpha(RADAR_ALPHA);
	m_sprite.AddChild(&m_overlay);

	// Setup the parts
	float angle = 0.0f;
	const Vector2 backgroundDir[4] =
	{
		Vector2(-0.5f, -0.5f),
		Vector2(0.5f, -0.5f),
		Vector2(0.5f, 0.5f),
		Vector2(-0.5f, 0.5f)
	};
	for (i = 0; i < 4; i++, angle += 90.0f)
	{
		// Setup the 4 backgrounds
		m_background[i].SetTextureSet(m_textureDB, 0);
		m_background[i].Reset();
		m_background[i].SetOrigin(0.5f, 0.5f);
		m_background[i].SetRotation(angle);
		m_background[i].SetPriority(99);
		m_background[i].SetAlpha(RADAR_ALPHA);
		m_sprite.AddChild(&m_background[i]);

		if (Math::Zero(m_radius))
		{
			float width, height;
			m_background[i].GetSize(width, height);
			ASSERT(Math::Equal(width, height));
			m_radius = width;
		}
		m_background[i].SetPosition(backgroundDir[i].X() * m_radius, backgroundDir[i].Y() * m_radius);
	}

	m_sprite.SetScale(RADAR_SCALE, RADAR_SCALE);

	// Haven't scanned the cast yet
	m_persistentScanned = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RadarInstrument::~RadarInstrument()
{
	PersistentBlip* blip;
	while ((blip = m_persistentBlips.Head()) != NULL)
	{
		m_persistentBlips.Unlink(blip);
		m_persistentBlipPool.Delete(blip);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Get our size
///////////////////////////////////////////////////////////////////////////////
Vector2 RadarInstrument::Extents()
{
	Vector2 extents(m_radius * 2.0f * RADAR_SCALE, m_radius * 2.0f * RADAR_SCALE);
	return extents;
}

///////////////////////////////////////////////////////////////////////////////
// The parent calls this to set your position
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::Place(float x, float y)
{
	m_sprite.SetPosition(x + m_radius, y + m_radius);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::Update(float dt)
{
	// Update the fired alphas
	for (int f = 0; f < MAX_FIRED_INDICATORS; f++)
		m_firedAlpha[f] = FOFilterImplicit(m_firedAlpha[f], 0.0f, dt, 0.2f);
}

///////////////////////////////////////////////////////////////////////////////
// Register a hit
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::NotifyFired(
CActor *shooter,
const Vector3 &pos)
{
	CCompActor *subject = g_hud.GetSubject();
	if (!subject)
		return;

	CParticipantIF *participant =	(CParticipantIF *)subject->GetInterface(CParticipantIF::GetID());
	int me = participant ? participant->GetParticipantId() : INVALID_PARTICIPANT;

	participant = shooter ? (CParticipantIF *)shooter->GetInterface(CParticipantIF::GetID()) : NULL;
	int shooterID = participant ? participant->GetParticipantId() : INVALID_PARTICIPANT;

	if (shooterID != INVALID_PARTICIPANT && !g_referee.GetTeamingModel().AreEnemies(me, shooterID))
		return;

	// To far?
	const float distanceSquared = (pos - subject->GetBodyInWorld()).LengthSquared();
	const float maxRange = Math::Square(m_maxRange * 1.5f);
	if (distanceSquared > maxRange)
		return;

	// Find an open slot
	m_firedAlpha[m_oldestFired] = 1.0f;
	m_firedPos[m_oldestFired] = pos;
	if (++m_oldestFired == MAX_FIRED_INDICATORS)
		m_oldestFired = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::UpdateFired(
const Vector3 &viewerPos,
const Vector3 &viewerDir)
{
	// Set & forget!
	for (int f = 0; f < MAX_FIRED_INDICATORS; f++)
	{
		m_fired[f].SetAlpha(m_firedAlpha[f]);
		if (!Math::Zero(m_firedAlpha[f]))
		{
			float angle = GetAngle(viewerPos, viewerDir, m_firedPos[f]);
			m_fired[f].SetRotation(angle);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::GetViewerInfo(
Vector3 &pos,
Vector3 &dir)
{
	CCompActor *me = g_hud.GetSubject();
	if (!me)
	{
		pos.Clear();
		dir.Set(0.0f, 0.0f, 1.0f);
	}
	else
	{
		ViewIF *viewIF = (ViewIF *)me->GetInterface(ViewIF::GetID());
		if (!viewIF)
		{
			pos = me->GetBodyInWorld();
			dir = me->GetBodyToWorld().GetRow2();
		}
		else
		{
#if 1
			pos = me->GetBodyInWorld();
#else
			viewIF->GetEyepoint(pos);
#endif
			viewIF->GetDirection(dir);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Transform a point into radar space
///////////////////////////////////////////////////////////////////////////////
float RadarInstrument::GetAngle(
const Vector3 &viewer,
const Vector3 &viewDir,
const Vector3 &pos)
{
	Vector3 p = pos - viewer;
	float distance = Vector3::Length(p);
	if (Math::Zero(distance))
		return 0.0f;

	// Normalize
	p /= distance;

	// Get the angle between these two vectors
	float viewAngle = Math::ArcTan2(viewDir.Z(), viewDir.X());
	float pAngle = Math::ArcTan2(p.Z(), p.X());
	return Math::Rad2Deg(pAngle - viewAngle);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 RadarInstrument::ToRadar(
const Vector3 &viewer,
const Vector3 &viewDir,
const Vector3 &pos,
float *range)
{
	Vector3 p = pos - viewer;
	float distance = Vector3::Length(p);
	if (Math::Zero(distance))
		return Vector2(0.0f, 0.0f);

	// Normalize
	p /= distance;
	if (range)
		*range = distance;

	// Get the angle between these two vectors
	float viewAngle = Math::ArcTan2(viewDir.Z(), viewDir.X());
	float pAngle = Math::ArcTan2(p.Z(), p.X());
	float angle = pAngle - viewAngle;

	float normalizedRange = Math::Clamp(distance / m_maxRange);
	float s, c;
	Math::SinCos(angle, s, c);
	return Vector2(s, -c) * normalizedRange;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RadarInstrument::BlipArray::BlipArray()
{
	m_blip = NULL;
	m_usedBlips = m_maxBlips = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RadarInstrument::BlipArray::~BlipArray()
{
	delete [] m_blip;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RadarInstrument::BlipArray::Create(
Sprite &parent,
int maxBlips,
const Vector4 &color,
t_Handle textureDB,
int texture)
{
	m_blip = new Sprite[maxBlips];
	ASSERT(m_blip);
	if (!m_blip)
		return false;

	m_maxBlips = maxBlips;
	m_usedBlips = 0;
	for (int b = 0; b < m_maxBlips; b++)
	{
		m_blip[b].SetTextureSet(textureDB, texture);
		m_blip[b].Reset();
		m_blip[b].SetOrigin(0.5f, 0.5f);
		m_blip[b].SetColor(color.X(), color.Y(), color.Z(), color.W());
		m_blip[b].SetPriority(50);
		m_blip[b].Hide();
		parent.AddChild(&m_blip[b]);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RadarInstrument::BlipArray::Allocate(
const Vector2 &pos,
float scale)
{
	if (m_usedBlips == m_maxBlips)
		return false;

	m_blip[m_usedBlips].SetPosition(pos.X(), pos.Y());
	m_blip[m_usedBlips].SetScale(scale, scale);
	m_blip[m_usedBlips].Show();
	++m_usedBlips;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::BlipArray::Clear()
{
	m_usedBlips = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::BlipArray::Update()
{
	for (int i = m_usedBlips; i < m_maxBlips; i++)
		m_blip[i].Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RadarInstrument::PersistentBlip::PersistentBlip(
Sprite &parent,
const Vector3 &pos,
t_Handle textureDB,
int texture,
float maxRange)
{
	m_actor == (ActorHandle)INVALID_OBJECT_HANDLE;
	m_pos = pos;
	m_maxRange = maxRange;
	m_sprite.SetTextureSet(textureDB, texture);
	m_sprite.Reset();
	m_sprite.SetOrigin(0.5f, 0.5f);
	m_sprite.SetPriority(25);
	parent.AddChild(&m_sprite);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RadarInstrument::PersistentBlip::PersistentBlip(
Sprite &parent,
ActorHandle follow,
t_Handle textureDB,
int texture,
float maxRange,
bool isPickup,
bool useTeamColor)
{
	m_actor = follow;
	m_pos = CActor::FromHandle(m_actor)->GetBodyInWorld();
	m_maxRange = maxRange;
	m_sprite.SetTextureSet(textureDB, texture);
	m_sprite.Reset();
	m_sprite.SetOrigin(0.5f, 0.5f);
	m_sprite.SetPriority(25);
	parent.AddChild(&m_sprite);
	m_isPickup = isPickup;
	m_useTeamColor = useTeamColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RadarInstrument::PersistentBlip::IsValid()
{
	return ((m_actor == (ActorHandle)INVALID_OBJECT_HANDLE) ||
			 (CActor::FromHandle(m_actor) != NULL));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector3 RadarInstrument::PersistentBlip::PositionInWorld()
{
	if (m_actor == (ActorHandle)INVALID_OBJECT_HANDLE)
		return m_pos;
	else
	{
		CActor *actor = CActor::FromHandle(m_actor);
		ASSERT(actor);
		if (!actor)
			return Vector3(-100000.0f, -100000.0f, -100000.0f);
		else
		{
			CActor *carrier = GetCarrier(actor);
			if (carrier)
				return carrier->GetBodyInWorld();
			else
				return actor->GetBodyInWorld();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool RadarInstrument::PersistentBlip::ObjectHidden()
{
	if (m_actor == (ActorHandle)INVALID_OBJECT_HANDLE)
		return false;
	{
		CActor *actor = CActor::FromHandle(m_actor);
		if (!actor)
			return true;
		else
		{
			CActor *carrier = GetCarrier(actor);
			if (carrier)
				return carrier->Hidden();
			else
			{
				const char* name = actor->Name();
				if (name && (strchr(name, '|') != NULL))
				{
					name = strchr(name, '|');
					name++;
				}

				if (name && (strcmpi(name, "tag_frame") == 0))
					return false;
				else
					return actor->Hidden();
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CActor *RadarInstrument::PersistentBlip::GetCarrier(
CActor *actor) const
{
	// Are we picked up?
	if (actor && m_isPickup)
	{
		CObjectiveIF* objectiveIF = CObjectiveIF::GetInterface(actor);
		if (objectiveIF && objectiveIF->GetResidence() == CObjectiveIF::TRANSIT)
			return CActor::FromHandle(objectiveIF->GetMostRecentHolder());
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int RadarInstrument::AttachToActors(
const char *actorKey,
bool couldMove,
int texture,
float maxRange,
bool isPickup,
bool useTeamColor)
{
	int found = 0;
	CCast &cast = g_stage.MasterCast();
	CCastMember *member = cast.GetNextMember(NULL);
	while (member)
	{
		CActor *actor = member->GetActor();
		ASSERT(actor);

		const char* name = actor->Name();
		if (name && (strchr(name, '|') != NULL))
		{
			name = strchr(name, '|');
			name++;
		}

		if (name && (strcmpi(name, actorKey) == 0))
		{
			// Make it!
			void *buffer = m_persistentBlipPool.NewPtr();
			ASSERT(buffer);
			if (buffer)
			{
				PersistentBlip *blip;
				if (couldMove || isPickup || useTeamColor)
					blip = new (buffer) PersistentBlip(m_sprite, actor->GetHandle(), m_textureDB, texture, maxRange, isPickup, useTeamColor);
				else
					blip = new (buffer) PersistentBlip(m_sprite, actor->GetBodyInWorld(), m_textureDB, texture, maxRange);
				ASSERT(blip);
				if (blip)
					m_persistentBlips.AddTail(blip);
			}
		}

		// Next!
		member = cast.GetNextMember(member);
	}

	return found;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RadarInstrument::UpdatePersistentBlips(
const Vector3 &viewerPos,
const Vector3 &viewerDir)
{
	if (!m_persistentScanned)
	{
		m_persistentScanned = CreatePersistentBlips();
		ASSERT(m_persistentScanned);
		if (!m_persistentScanned)
			return;
	}

	PersistentBlip *blip = m_persistentBlips.Head();
	while (blip)
	{
		PersistentBlip *next = blip->next;
		if (!blip->IsValid())
		{
			m_persistentBlips.Unlink(blip);
			m_persistentBlipPool.Delete(blip);
		}
		else
		{
			// Get their color
			if (blip->UseTeamColor() && (blip->GetActor() != NULL))
			{
				const char* name = blip->GetActor()->Name();
				if (name && (strchr(name, '|') != NULL))
				{
					name = strchr(name, '|');
					name++;
				}

				// Tag frames use the color of the team that controls the nearest spawn point
				if (name && (strcmpi(name, "tag_frame") == 0))
				{
					CTeamingModel::ERole role = g_PlayerSpawnMgr.FindRoleOfNearestSpawnPoint( blip->GetActor()->GetBodyInWorld() );
					if (role == CTeamingModel::BLUE)
						blip->SetColor(0.0f, 0.0f, 1.0f);
					else if (role == CTeamingModel::RED)
						blip->SetColor(1.0f, 0.0f, 0.0f);
					else
						blip->SetColor(1.0f, 1.0f, 1.0f);
				}
			}

			// Get their position
			float range;
			Vector2 radarPos = ToRadar(viewerPos, viewerDir, blip->PositionInWorld(), &range) * (m_radius * 0.95f);
			if (blip->ObjectHidden() || (!Math::Zero(blip->MaxRange()) && range > blip->MaxRange()))
				blip->Show(false);
			else
			{
				blip->SetPosition(radarPos);
				blip->Show(true);
			}
		}
		blip = next;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SPRadarInstrument::SPRadarInstrument()
{
	// Create the quick blip pools
	m_blip[BLIP_FRIENDLY].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(0.0f, 1.0f, 0.0f, 0.5f), m_textureDB, 4);
	m_blip[BLIP_MANDOWN].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(0.0f, 1.0f, 0.0f, 0.5f), m_textureDB, 2);
	m_blip[BLIP_NPC].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(0.5f, 0.5f, 1.0f, 0.5f), m_textureDB, 4);
	m_blip[BLIP_BADDIE].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(1.0f, 0.0f, 0.0f, 0.5f), m_textureDB, 4);
	m_blip[BLIP_DEADBADDIE].Create(m_sprite, MAX_BLIPS_PER_TYPE / 2, Vector4(1.0f, 0.0f, 0.0f, 0.5f), m_textureDB, 2);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void SPRadarInstrument::Update(float dt)
{
	// Call the parent
	RadarInstrument::Update(dt);

	// Update!
	if (m_hidden || !m_active)
	{
		for (int b = 0; b < BLIPTYPES; b++)
			m_blip[b].HideAll();
	}
	else
	{
		CCompActor *actor = g_hud.GetSubject();
		if (!actor)
			m_sprite.HideAll();
		else
		{
			// Wee!
			m_sprite.ShowAll();

			// Get the current viewer info
			Vector3 viewerPos, viewerDir;
			GetViewerInfo(viewerPos, viewerDir);

			// Update the fired stuff
			UpdateFired(viewerPos, viewerDir);

			// Do the referee stuff
			UpdateRefereeBlips(viewerPos, viewerDir);

			// Update persistent blips
			UpdatePersistentBlips(viewerPos, viewerDir);
		}
	}

	// Update everything else
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPRadarInstrument::UpdateRefereeBlips(
const Vector3 &viewerPos,
const Vector3 &viewerDir)
{
	// What is my team?
	CCompActor *subject = g_hud.GetSubject();CParticipantIF *participant =	(CParticipantIF*)subject->GetInterface(CParticipantIF::GetID());
	int me = participant ? participant->GetParticipantId() : INVALID_PARTICIPANT;

	// Get the teaming model
	CTeamingModel &teamingModel = g_referee.GetTeamingModel();

	// Assume nothing to draw
	for (int b = 0; b < BLIPTYPES; b++)
		m_blip[b].Clear();

	// Show enemies?
	bool showEnemies = (!ValidSingleton(CSession) || !g_session.IsNetworked());

	// Scan the referee
	uint8 id = g_referee.GetNextParticipant(INVALID_PARTICIPANT);
	while (id != INVALID_PARTICIPANT)
	{
		CActor *actor = g_referee.GetActor(id);
		if (actor && actor != (CActor *)subject)
		{
			// Are they alive?
			HealthIF *healthIF = (HealthIF *)actor->GetInterface(HealthIF::GetID());
			bool isDead = (healthIF && !healthIF->IsAlive());

			// Get their position
			float range;
			Vector2 radarPos = ToRadar(viewerPos, viewerDir, actor->GetBodyInWorld(), &range) * (m_radius * 0.95f);
			const float offRadarRange = m_maxRange * 1.25f;

			// What we going to do
			if (teamingModel.AreFriends(me, id))
			{
				if (isDead)
					m_blip[BLIP_MANDOWN].Allocate(radarPos);
				else
					m_blip[BLIP_FRIENDLY].Allocate(radarPos);
			}
			else if (range < offRadarRange && teamingModel.AreNeutral(me, id) && !isDead)
				m_blip[BLIP_NPC].Allocate(radarPos);
			else if (range < offRadarRange && teamingModel.AreEnemies(me, id) && showEnemies)
			{
				if (!isDead)
					m_blip[BLIP_BADDIE].Allocate(radarPos);
				else
					m_blip[BLIP_DEADBADDIE].Allocate(radarPos);
			}
		}

		id = g_referee.GetNextParticipant(id);
	}

	// Update
	for (int b = 0; b < BLIPTYPES; b++)
		m_blip[b].Update();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SPRadarInstrument::CreatePersistentBlips()
{
	// Try to find stuff in question
	if (g_configmgr.TestOptionValue("gameModes", "tag") ||
			g_configmgr.TestOptionValue("gameModes", "raid") ||
			g_configmgr.TestOptionValue("gameModes", "robbery"))
	{
		return false;
	}
	else if (g_configmgr.TestOptionValue("gameModes", "sp"))
	{
#ifdef CDCHANGEDTHEIRMINDSAGAIN
		AttachToActors("cashpickup", true, 5, 50.0f);
		AttachToActors("switchlevel", false, 3, 100.0f);
		AttachToActors("sp_objective", true, 5, 0.0f, true);
		AttachToActors("tag_frame", false, 6, 0.0f);
#endif
	}
	else if (g_configmgr.TestOptionValue("gameModes", "dev"))
	{
		AttachToActors("raid_objective", true, 5, 0.0f, true);
		AttachToActors("raidtrigger", false, 3, 0.0f);
		AttachToActors("robbery_objective", true, 5, 0.0f, true);
		AttachToActors("robberytrigger", false, 3, 0.0f);
		AttachToActors("cashpickup", true, 5, 50.0f);
		AttachToActors("switchlevel", false, 3, 100.0f);
		AttachToActors("sp_objective", true, 5, 0.0f, true);
		AttachToActors("tag_frame", false, 6, 0.0f);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MPRadarInstrument::MPRadarInstrument()
{
	// Create the quick blip pools
	m_blip[BLIP_FRIENDLY].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(0.0f, 1.0f, 0.0f, 0.5f), m_textureDB, 4);
	m_blip[BLIP_MANDOWN].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(0.0f, 1.0f, 0.0f, 0.5f), m_textureDB, 2);
	m_blip[BLIP_NPC].Create(m_sprite, MAX_BLIPS_PER_TYPE, Vector4(0.5f, 0.5f, 1.0f, 0.5f), m_textureDB, 4);
	m_blip[BLIP_TALKER].Create(m_sprite, 1, Vector4(1.0f, 1.0f, 0.5f, 0.5f), m_textureDB, 7);
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void MPRadarInstrument::Update(float dt)
{
	// Call the parent
	RadarInstrument::Update(dt);

	// Update!
	if (m_hidden || !m_active)
	{
		for (int b = 0; b < BLIPTYPES; b++)
			m_blip[b].HideAll();
	}
	else
	{
		CCompActor *actor = g_hud.GetSubject();
		if (!actor)
			m_sprite.HideAll();
		else
		{
			// Wee!
			m_sprite.ShowAll();

			// Get the current viewer info
			Vector3 viewerPos, viewerDir;
			GetViewerInfo(viewerPos, viewerDir);

			// Update the fired stuff
			UpdateFired(viewerPos, viewerDir);

			// Do the referee stuff
			UpdateRefereeBlips(viewerPos, viewerDir);

			// Update persistent blips
			UpdatePersistentBlips(viewerPos, viewerDir);
		}
	}

	// Update everything else
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MPRadarInstrument::UpdateRefereeBlips(
const Vector3 &viewerPos,
const Vector3 &viewerDir)
{
	// What is my team?
	CCompActor *subject = g_hud.GetSubject();
	CParticipantIF *participant =	(CParticipantIF*)subject->GetInterface(CParticipantIF::GetID());
	int me = participant ? participant->GetParticipantId() : INVALID_PARTICIPANT;

	// Get the teaming model
	CTeamingModel &teamingModel = g_referee.GetTeamingModel();

	// Assume nothing to draw
	for (int b = 0; b < BLIPTYPES; b++)
		m_blip[b].Clear();

	// Show enemies?
	bool showEnemies = (!ValidSingleton(CSession) || !g_session.IsNetworked());

	// Who is the talker?
	DOHandle handle = g_CVoiceChat.getWhosTalking();
	if (handle != INVALID_DOHANDLE && participant)
	{
		ASSERT(handle.IsAKindOf(DOCLASSID(DOTalker)));
		DOTalker::Ref refTalker(handle);
		if (refTalker.IsValid())
		{
			DOPlayerAgent *talkingAgent = ( DOPlayerAgent * )*refTalker;
			if (talkingAgent)
			{
				if (talkingAgent->GetRequestedTeam() == participant->GetTeamId())
				{
					float range;
					const Vector2 radarPos = ToRadar(viewerPos, viewerDir, talkingAgent->GetLocation(), &range) * (m_radius * 0.95f);
					const float scale = 0.5f + Math::Clamp((1.0f + Math::Cos(g_timer.GetEndSec() * 12.0f)) * 0.25f, 0.0f, 0.5f);
	  				m_blip[BLIP_TALKER].Allocate(radarPos, scale);
				}
			}
		}
	}
	// Scan the referee
	uint8 id = g_referee.GetNextParticipant(INVALID_PARTICIPANT);
	while (id != INVALID_PARTICIPANT)
	{
		CActor *actor = g_referee.GetActor(id);
		if (actor && actor != (CActor *)subject)
		{
			// Are they alive?
			HealthIF *healthIF = (HealthIF *)actor->GetInterface(HealthIF::GetID());
			bool isDead = (healthIF && !healthIF->IsAlive());

			// Get their position
			float range;
			const Vector3 actorPos = actor->GetBodyInWorld();
			const Vector2 radarPos = ToRadar(viewerPos, viewerDir, actorPos, &range) * (m_radius * 0.95f);
			const float offRadarRange = m_maxRange * 1.25f;

			// What we going to do
			if (teamingModel.AreTeammates(me, id))
			{
				if (isDead)
					m_blip[BLIP_MANDOWN].Allocate(radarPos);
				else
					m_blip[BLIP_FRIENDLY].Allocate(radarPos);
			}
			else if (range < offRadarRange && teamingModel.AreNeutral(me, id) && !isDead)
				m_blip[BLIP_NPC].Allocate(radarPos);
		}

		id = g_referee.GetNextParticipant(id);
	}

	// Update
	for (int b = 0; b < BLIPTYPES; b++)
		m_blip[b].Update();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MPRadarInstrument::CreatePersistentBlips()
{
	// Dev mode
	// Try to find stuff in question
	if (g_configmgr.TestOptionValue("gameModes", "tag"))
		AttachToActors("tag_frame", false, 6, 0.0f, false, true);
	else if (g_configmgr.TestOptionValue("gameModes", "raid"))
	{
		AttachToActors("raid_objective", true, 5, 0.0f, true);
		AttachToActors("raidtrigger", false, 3, 0.0f);
	}
	else if (g_configmgr.TestOptionValue("gameModes", "robbery"))
	{
		AttachToActors("robbery_objective", true, 5, 0.0f, true);
		AttachToActors("robberytrigger", false, 3, 0.0f);
	}
	else if (g_configmgr.TestOptionValue("gameModes", "sp") ||
				g_configmgr.TestOptionValue("gameModes", "dev"))
	{
		return false;
	}

	return true;
}


