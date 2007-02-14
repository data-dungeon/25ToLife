///////////////////////////////////////////////////////////////////////////////
// Targeting reticle
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "math/sfilter.h"
#include "weapons/ttlweapinv.h"
#include "weapons/ttlweapon.h"
#include "weapons/zoomsight.h"
#include "Effects/scenesetup.h"
#include "weapons/seethru.h"
#include "components/kinecontrol/viewdriver.h"
#include "hud/ireticle.h"

#define BASE_PIP_ALPHA		0.5f

Vector2 ReticleInstrument::s_hitDir[4] =
{
	Vector2(1.0f, -1.0f),
	Vector2(1.0f, 1.0f),
	Vector2(-1.0f, 1.0f),
	Vector2(-1.0f, -1.0f),
};

Vector2 ReticlePipPlus::s_pipDir[4] =
{
	Vector2(1.0f, 0.0f),
	Vector2(0.0f, 1.0f),
	Vector2(-1.0f, 0.0f),
	Vector2(0.0f, -1.0f),
};

Vector2 ReticlePipX::s_pipDir[4] =
{
	Vector2(1.0f, -1.0f),
	Vector2(1.0f, 1.0f),
	Vector2(-1.0f, 1.0f),
	Vector2(-1.0f, -1.0f),
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticleInstrument::ReticleInstrument() :
SpriteInstrument("reticle")
{
	t_Handle handle = dblLoad("newhud/reticle.dbl");
	ASSERT(handle != INVALID_HANDLE);

	// Create all the reticles
	int baseTexture = 1;
	int circle = 8;
	m_pip[BOTHMELEE] = new ReticlePipSimple(m_sprite, handle, baseTexture);
	m_pip[COPNORMAL] = new ReticlePipX(m_sprite, handle, baseTexture);
	m_pip[GANGNORMAL] = new ReticlePipPlus(m_sprite, handle, baseTexture);
	m_pip[COPSHOTGUN] = new ReticlePipScale(m_sprite, handle, baseTexture, circle);
	m_pip[GANGSHOTGUN] = new ReticlePipScale(m_sprite, handle, baseTexture, circle);

	// Setup the hits
	const float hitOffset = 20.0f;

	// Setup the angles
	float angle = 0.0f;
	for (int i = 0; i < 4; i++, angle += 90.0f)
	{
		// Setup the 4 hits
		m_hit[i].SetTextureSet(handle, 0);
		m_hit[i].Reset();
		m_hit[i].SetOrigin(0.5f, 0.5f);
		m_hit[i].SetRotation(angle);
		m_sprite.AddChild(&m_hit[i]);
		m_hit[i].SetAlpha(0.0f);
		m_hit[i].SetPosition(s_hitDir[i].X() * hitOffset, s_hitDir[i].Y() * hitOffset);
		m_hit[i].SetBlendMode(SPRITEBLEND_ADD);
	}

	// No hits yet
	m_hitStrength = 0.0f;

	// Clear the color
	m_currentColor = GetPipColor(TARGET_NONE);

	// Headshot shit!
	m_onHead = false;
	m_scale = m_scaleTarget = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticleInstrument::~ReticleInstrument()
{
	for (int p = 0; p < RETICLETYPES; p++)
		delete m_pip[p];
}

///////////////////////////////////////////////////////////////////////////////
// The update (called once per frame)
///////////////////////////////////////////////////////////////////////////////
void ReticleInstrument::Update(float dt)
{
	// This always goes down regardless of active/hidden
	m_hitStrength = FOFilterImplicit(m_hitStrength, 0.0f, dt, 0.4f);

	// Exit early if nothing to do
	if (!m_hidden && m_active)
	{
		// Get the current weapon
		CCompActor *actor = g_hud.GetSubject();
		if (!actor)
			m_sprite.HideAll();
		else
		{
			// What weapon are we holding?
			TTLWeapon *weapon = GetCurrentWeapon(actor);

			// Update the targeting
			if ((g_timer.GetFrameCount()) % 3 == 0)
				m_currentColor = GetPipColor(HandleTarget(actor, weapon, m_onHead));

			m_scaleTarget = m_onHead ? 1.5f : 1.0f;
			m_scale = FOFilterImplicit(m_scale, m_scaleTarget, dt, 0.3f);

			// Update the hit strength stuff
		  	for (int h = 0; h < 4; h++)
			{
				m_hit[h].Show();
 				m_hit[h].SetColor(m_currentColor.X(), m_currentColor.Y(), m_currentColor.Z(), m_hitStrength * BASE_PIP_ALPHA);
			}

			// Update the pip
			UpdatePips(actor, weapon, dt, m_scale);
		}
	}

	// Update everything else
	SpriteInstrument::Update(dt);
}

///////////////////////////////////////////////////////////////////////////////
// Register a hit
///////////////////////////////////////////////////////////////////////////////
void ReticleInstrument::RegisterHit()
{
	m_hitStrength = Math::Clamp(m_hitStrength + 0.75f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReticleInstrument::UpdatePips(
CCompActor *me,
TTLWeapon *weapon,
float dt,
float scale)
{
 	bool melee = IsMelee(weapon);
 	bool shotgun = IsShotgun(weapon);
 	bool gangster = IsGang(me);
	bool reloading = (weapon && weapon->GetState() == Weapon::RELOADING);

	// What is the active pip
	int activePip;
	if (reloading)
		activePip = -1;
	else if (melee)
		activePip = BOTHMELEE;
	else
	{
		if (gangster)
			activePip = shotgun ? GANGSHOTGUN : GANGNORMAL;
		else
			activePip = shotgun ? COPSHOTGUN : COPNORMAL;
	}

	// Update the pips
	for (int p = 0; p < RETICLETYPES; p++)
	{
		if (activePip == p)
			m_pip[p]->Update(weapon, m_currentColor, dt, scale);
		else
			m_pip[p]->Hide(dt);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ReticleInstrument::IsGang(
CCompActor *who)
{
	if (!who)
		return false;
	else
	{
		char *role = who->Parm().GetString( "role" );
		return (role && strcmp(role, "gang") == 0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TTLWeapon *ReticleInstrument::GetCurrentWeapon(
CCompActor *actor)
{
	// Duh?
	if (!actor)
		return NULL;

	// Get weapon information
	TTLWeaponInventory *inventory = (TTLWeaponInventory *)actor->GetComponentByName("WeaponInventory");
	if (inventory)
		return (TTLWeapon *)inventory->GetSelected();
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ReticleInstrument::IsShotgun(
TTLWeapon *weapon)
{
	return (weapon && weapon->GetWeaponType() == WEAPONTYPE_SHOTGUN);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ReticleInstrument::IsMelee(
TTLWeapon *weapon)
{
	return (weapon && weapon->GetWeaponType() == WEAPONTYPE_MELEE);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticleInstrument::ReticleColor ReticleInstrument::HandleTarget(
CCompActor *actor,
TTLWeapon *weapon,
bool &onHead)
{
	ReticleInstrument::ReticleColor color = TARGET_NONE;

	onHead = false;
	if (actor)
	{
		// Can we even see this guy?
		int materials = 0;
		int *targetMaterial = weapon ? weapon->GetAimSim().TargetMaterials(materials) : NULL;
		bool obscured = false;
		for (int m = 0; m < materials - 1; m++)
		{
			if (!SeeThru::TargetThru(targetMaterial[m]))
			{
				obscured = true;
				break;
			}
		}

		if (!obscured)
		{
			// Get the target
			CActor *target = weapon ? weapon->GetAimSim().TargetActor() : NULL;
			CParticipantIF *targetIF = target ? (CParticipantIF*)target->GetInterface(CParticipantIF::GetID()) : NULL;
			uint8 targetID = targetIF ? targetIF->GetParticipantId() : INVALID_PARTICIPANT;

			// Try to get my actor's participant interface.
			CParticipantIF *actorIF = (CParticipantIF *)actor->GetInterface(CParticipantIF::GetID());
			uint8 actorID = actorIF ? actorIF->GetParticipantId() : INVALID_PARTICIPANT;
			const CTeamingModel &teamingModel = g_referee.GetTeamingModel();

			// If my actor is not a participant or if the target is neutral, draw
			// a neutral target bar.
			if (!g_referee.IsParticipant(actorID) || teamingModel.IsAmbient(targetID))
			{
				g_messageDispatcher.SendMessageToActor( "AtGunPoint", NULL, actor->GetHandle(), target->GetHandle());
				color = TARGET_NEUTRAL;
			}
			else if (teamingModel.AreTeammates(actorID, targetID) || teamingModel.IsPlayerFriend(targetID)) 		// Draw a friendly reticle for friends.
				color = TARGET_FRIENDLY;
			else if (teamingModel.AreEnemies(actorID, targetID)) 		// Draw an enemy reticle for enemies.
				color = TARGET_ENEMY;

			// Are we on the head?
			if (color == TARGET_ENEMY && weapon &&
					weapon->GetWeaponClass() == WEAPONCLASS_FIREARM &&
					weapon->GetAimSim().TargetMaterial() == g_surfaceType.GetIndex("HumanHead"))
				onHead = true;
		}

		// Should we really be doing this here...
		ViewDriver *viewDriver = (ViewDriver *)actor->GetComponentByName("ViewDriver");
		if (viewDriver)
			viewDriver->OnTarget(color != TARGET_NONE);
	}

	return color;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector4 ReticleInstrument::GetPipColor(
ReticleColor color)
{
	const Vector4 colorLookup[] =
	{
		Vector4(0.0f, 1.0f, 0.0f, BASE_PIP_ALPHA),
		Vector4(1.0f, 0.0f, 0.0f, BASE_PIP_ALPHA),
		Vector4(0.0f, 1.0f, 0.0f, BASE_PIP_ALPHA),
		Vector4(0.9f, 0.9f, 0.9f, BASE_PIP_ALPHA)
	};

	return colorLookup[color];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticlePip::ReticlePip(
Sprite &parent,
t_Handle dblHandle,
int &baseTexture)
{
	m_minOffset = 12.0f;
	m_maxOffset = 64.0f;
 	m_minSqueeze = -3.0f;
	m_maxSqueeze = 4.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float ReticlePip::ComputeOffset(
TTLWeapon *weapon)
{
	// No weapon, no error
	if (!weapon)
		return 0.0f;

	// Figure out our FOV
	float fov = g_cameraDirector.GetCurrentProjection().FOV();
	float minFOV, maxFOV, baseFOV = SceneSetup::BaseFOV();
 	ZoomSight *zoomSight = (ZoomSight *)weapon->GetSight();
 	if (!zoomSight)
	{
		minFOV = baseFOV * 0.2f;
		maxFOV = baseFOV;
	}
	else
 	{
 		minFOV = zoomSight->MinFOV() * baseFOV;
 		maxFOV = zoomSight->MaxFOV() * baseFOV;
 	}

 	AimingSimulation &aimSim = weapon->GetAimSim();
 	const float aimingError = aimSim.GetError();

 	float scale = fov / 120.0f;
 	const float minCone = aimSim.GetMinSpread() * scale;
 	const float maxCone = aimSim.GetMaxSpread() * scale;

	const float error = minCone + (aimingError * (maxCone - minCone)) * 24.0f;
	const float squeeze = Math::LinearMap(fov, minFOV, maxFOV, m_minSqueeze, m_maxSqueeze);
	float offset = error + squeeze;
	return Math::Clamp(m_minOffset + offset, 0.0f, m_maxOffset);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticlePipSimple::ReticlePipSimple(
Sprite &parent,
t_Handle dblHandle,
int &baseTexture,
float scale) :
ReticlePip(parent, dblHandle, baseTexture)
{
	// Create the pieces
	m_dot.SetTextureSet(dblHandle, baseTexture);
	m_dot.Reset();
	m_dot.SetOrigin(0.5f, 0.5f);
	m_dot.SetScale(scale, scale);
	m_dot.SetBlendMode(SPRITEBLEND_ADD);
	parent.AddChild(&m_dot);

	m_baseScale = scale;

	// We used one texture
	baseTexture += 1;
}

///////////////////////////////////////////////////////////////////////////////
// You write these!
///////////////////////////////////////////////////////////////////////////////
void ReticlePipSimple::Hide(float dt)
{
	m_dot.Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReticlePipSimple::Update(
TTLWeapon *weapon,
const Vector4 &color,
float dt,
float scale)
{
	// Do the dot
	m_dot.Show();
	m_dot.SetColor(color.X(), color.Y(), color.Z(), color.W());
	float realScale = m_baseScale + (scale - 1.0f);
	m_dot.SetScale(realScale, realScale);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticlePipPlus::ReticlePipPlus(
Sprite &parent,
t_Handle dblHandle,
int &baseTexture) :
ReticlePip(parent, dblHandle, baseTexture)
{
	// Create the pieces
	m_dot.SetTextureSet(dblHandle, baseTexture);
	m_dot.Reset();
	m_dot.SetOrigin(0.5f, 0.5f);
	m_dot.SetBlendMode(SPRITEBLEND_ADD);
	parent.AddChild(&m_dot);

	// Setup the angles
	float angle = 0.0f;
	for (int p = 0; p < 4; p++, angle += 90.0f)
	{
		// Setup the 4 pips
		m_pip[p].SetTextureSet(dblHandle, baseTexture + 1);
		m_pip[p].Reset();
		m_pip[p].SetOrigin(0.5f, 0.5f);
		m_pip[p].SetRotation(angle);
		parent.AddChild(&m_pip[p]);
		m_pip[p].SetPosition(s_pipDir[p].X() * m_minOffset, s_pipDir[p].Y() * m_minOffset);
		m_pip[p].SetBlendMode(SPRITEBLEND_ADD);
	}

	// We used two textures
	baseTexture += 2;
}

///////////////////////////////////////////////////////////////////////////////
// You write these!
///////////////////////////////////////////////////////////////////////////////
void ReticlePipPlus::Hide(float dt)
{
	m_dot.Hide();
	for (int p = 0; p < 4; p++)
		m_pip[p].Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReticlePipPlus::Update(
TTLWeapon *weapon,
const Vector4 &color,
float dt,
float scale)
{
	// Do the dot
	m_dot.Show();
	m_dot.SetColor(color.X(), color.Y(), color.Z(), color.W());

	// Setup the pip position
	float position = ComputeOffset(weapon);
	for (int p = 0; p < 4; p++)
	{
		m_pip[p].Show();
		m_pip[p].SetColor(color.X(), color.Y(), color.Z(), color.W());
		m_pip[p].SetPosition(s_pipDir[p].X() * position, s_pipDir[p].Y() * position);
		m_pip[p].SetScale(scale, scale);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticlePipX::ReticlePipX(
Sprite &parent,
t_Handle dblHandle,
int &baseTexture) :
ReticlePip(parent, dblHandle, baseTexture)
{
	// Create the pieces
	m_dot.SetTextureSet(dblHandle, baseTexture);
	m_dot.Reset();
	m_dot.SetOrigin(0.5f, 0.5f);
	m_dot.SetBlendMode(SPRITEBLEND_ADD);
	parent.AddChild(&m_dot);

	// Setup the angles
	float angle = 0.0f;
	for (int p = 0; p < 4; p++, angle += 90.0f)
	{
		// Setup the 4 pips
		m_pip[p].SetTextureSet(dblHandle, baseTexture + 1);
		m_pip[p].Reset();
		m_pip[p].SetOrigin(0.5f, 0.5f);
		m_pip[p].SetRotation(angle);
		parent.AddChild(&m_pip[p]);
		m_pip[p].SetPosition(s_pipDir[p].X() * m_minOffset, s_pipDir[p].Y() * m_minOffset);
		m_pip[p].SetBlendMode(SPRITEBLEND_ADD);
	}

	// We used two textures
	baseTexture += 2;
}

///////////////////////////////////////////////////////////////////////////////
// You write these!
///////////////////////////////////////////////////////////////////////////////
void ReticlePipX::Hide(float dt)
{
	m_dot.Hide();
	for (int p = 0; p < 4; p++)
		m_pip[p].Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReticlePipX::Update(
TTLWeapon *weapon,
const Vector4 &color,
float dt,
float scale)
{
	// Do the dot
	m_dot.Show();
	m_dot.SetColor(color.X(), color.Y(), color.Z(), color.W());

	// Setup the pip position
	float position = ComputeOffset(weapon);
	for (int p = 0; p < 4; p++)
	{
		m_pip[p].Show();
		m_pip[p].SetColor(color.X(), color.Y(), color.Z(), color.W());
		m_pip[p].SetPosition(s_pipDir[p].X() * position, s_pipDir[p].Y() * position);
		m_pip[p].SetScale(scale, scale);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ReticlePipScale::ReticlePipScale(
Sprite &parent,
t_Handle dblHandle,
int &baseTexture,
int circle) :
ReticlePip(parent, dblHandle, baseTexture)
{
	m_minScale = 0.4f;
	m_maxScale = 0.75f;

	// Create the pieces
	m_inner.SetTextureSet(dblHandle, baseTexture++);
	m_inner.Reset();
	m_inner.SetOrigin(0.5f, 0.5f);
	m_inner.SetBlendMode(SPRITEBLEND_ADD);
	parent.AddChild(&m_inner);

	// Setup the 4 pips
	m_outer.SetTextureSet(dblHandle, circle);
	m_outer.Reset();
	m_outer.SetOrigin(0.5f, 0.5f);
	m_outer.SetBlendMode(SPRITEBLEND_ADD);
	parent.AddChild(&m_outer);
}

///////////////////////////////////////////////////////////////////////////////
// You write these!
///////////////////////////////////////////////////////////////////////////////
void ReticlePipScale::Hide(float dt)
{
	m_inner.Hide();
 	m_outer.Hide();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ReticlePipScale::Update(
TTLWeapon *weapon,
const Vector4 &color,
float dt,
float scale)
{
	// Do the dot
	m_inner.Show();
	m_inner.SetColor(color.X(), color.Y(), color.Z(), color.W());
	m_inner.SetScale(scale, scale);

 	m_outer.Show();
 	m_outer.SetColor(color.X(), color.Y(), color.Z(), color.W());

	float realScale = ComputeScale(weapon);
	m_outer.SetScale(realScale, realScale);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
float ReticlePipScale::ComputeScale(
TTLWeapon *weapon)
{
	// No weapon, no error
	if (!weapon)
		return m_minScale;

 	AimingSimulation &aimSim = weapon->GetAimSim();
 	return m_minScale + (m_maxScale - m_minScale) * aimSim.GetError();
}


