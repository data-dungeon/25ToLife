/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a firearm
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "EngineHelper/timer.h"
#include "Math/sfilter.h"
#include "Math/fastrand.h"
#include "Environ/Munition.h"
#include "avatar/avatarfx.h"
#include "weapons/seethru.h"
#include "weapons/ttlweapon.h"

// the slot names
const char *TTLWeapon::s_slotName[4] =
{
	"Melee", "Secondary", "Primary", "Grenade"
};

/////////////////////////////////////////////////////////////////////////////
// Weee
/////////////////////////////////////////////////////////////////////////////
TTLWeaponProperties::TTLWeaponProperties(
WeaponCreator &creator,
int index) : WeaponProperties(creator, index)
{
	// Our info
	m_slot = TTLWeapon::PRIMARY;

	m_restrict[0] = '\0';

	m_aimingName[0] = '\0';
	m_aiming = NULL;

	m_holdType[0] = '\0';
	m_reloadType[0] = '\0';

	m_weaponClass = WEAPONCLASS_NONE;
	m_weaponType = WEAPONTYPE_NONE;
	m_weaponSubType = WEAPONSUBTYPE_NONE;
	m_weaponHold = WEAPONHOLD_NONE;
	m_weaponReload = WEAPONRELOAD_NONE;

	m_weaponRestrict = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLWeaponProperties::~TTLWeaponProperties()
{
	delete m_aiming;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponProperties::RegisterContext(
const char *context)
{
	// Call the parent
	WeaponProperties::RegisterContext(context);

	g_console.CreateVar(context, "slot", m_slot);

	g_console.CreateVar(context, "restrict", m_restrict, sizeof(m_restrict) - 1);

	g_console.CreateVar(context, "holdType", m_holdType, sizeof(m_holdType) - 1);
	g_console.CreateVar(context, "reloadType", m_reloadType, sizeof(m_reloadType) - 1);

	g_console.CreateVar(context, "aiming", m_aimingName, sizeof(m_aimingName) - 1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeaponProperties::PostLoadSetup()
{
	// Call the parent
	WeaponProperties::PostLoadSetup();

	// Get our aiming info
	ASSERT(m_aimingName[0]);
	m_aiming = g_aimingLibrary.CreateInstance(m_aimingName);
	ASSERT(m_aiming);

	// Set up the class
	const char *classString = GetClass();
	if (strcmpi(classString, "melee") == 0)
		m_weaponClass = WEAPONCLASS_MELEE;
	else if (strcmpi(classString, "firearm") == 0)
		m_weaponClass = WEAPONCLASS_FIREARM;
	else if (strcmpi(classString, "lobbed") == 0)
		m_weaponClass = WEAPONCLASS_LOBBED;
	else
	{
		// Unknown class
		ASSERT(false);
	}

	// Setup the type
	const char *typeString = m_type;
	if (strcmpi(typeString, "MeleeSwing") == 0)
	{
		m_weaponType = WEAPONTYPE_MELEE;
		m_weaponSubType = WEAPONSUBTYPE_MELEE_SWING;
		m_weaponHold = WEAPONHOLD_MELEE_SWING;
	}
 	else if (strcmpi(typeString, "MeleeStab") == 0)
	{
		m_weaponType = WEAPONTYPE_MELEE;
		m_weaponSubType = WEAPONSUBTYPE_MELEE_STAB;
		m_weaponHold = WEAPONHOLD_MELEE_STAB;
	}
	else if (strcmpi(typeString, "MeleeSpray") == 0)
	{
		m_weaponType = WEAPONTYPE_MELEE;
		m_weaponSubType = WEAPONSUBTYPE_MELEE_SPRAY;
		m_weaponHold = WEAPONHOLD_MELEE_SPRAY;
	}
	else if (strcmpi(typeString, "MeleeTouch") == 0)
	{
		m_weaponType = WEAPONTYPE_MELEE;
		m_weaponSubType = WEAPONSUBTYPE_MELEE_TOUCH;
		m_weaponHold = WEAPONHOLD_MELEE_TOUCH;
	}
	else if (strcmpi(typeString, "MeleeProp") == 0)
	{
		m_weaponType = WEAPONTYPE_MELEE;
		m_weaponSubType = WEAPONSUBTYPE_MELEE_PROP;
		m_weaponHold = WEAPONHOLD_MELEE_SWING;
	}
	else if (strcmpi(typeString, "MeleeSmoke") == 0)
	{
		m_weaponType = WEAPONTYPE_MELEE;
		m_weaponSubType = WEAPONSUBTYPE_MELEE_PROPSMOKE;
		m_weaponHold = WEAPONHOLD_MELEE_TOUCH;
	}
	else if (strcmpi(typeString, "Pistol") == 0)
	{
		m_weaponType = WEAPONTYPE_PISTOL;
		m_weaponSubType = WEAPONSUBTYPE_PISTOL;
	}
	else if (strcmpi(typeString, "DualPistol") == 0)
	{
		m_weaponType = WEAPONTYPE_PISTOL;
		m_weaponSubType = WEAPONSUBTYPE_PISTOL_DUAL;
	}
	else if (strcmpi(typeString, "Shotgun") == 0)
		m_weaponType = WEAPONTYPE_SHOTGUN;
	else if (strcmpi(typeString, "Sub") == 0)
		m_weaponType = WEAPONTYPE_SUB;
	else if (strcmpi(typeString, "Assault") == 0)
		m_weaponType = WEAPONTYPE_ASSAULT;
	else if (strcmpi(typeString, "Sniper") == 0)
		m_weaponType = WEAPONTYPE_SNIPER;
	else if (strcmpi(typeString, "Grenade") == 0)
	{
		m_weaponType = WEAPONTYPE_LOBBED;
		m_weaponSubType = WEAPONSUBTYPE_LOBBED_FUSEEXPLODE;
	}
	else if (strcmpi(typeString, "ContactBomb") == 0)
	{
		m_weaponType = WEAPONTYPE_LOBBED;
		m_weaponSubType = WEAPONSUBTYPE_LOBBED_CONTACTEXPLODE;
	}
	else if (strcmpi(typeString, "Flashbang") == 0)
	{
		m_weaponType = WEAPONTYPE_LOBBED;
		m_weaponSubType = WEAPONSUBTYPE_LOBBED_FLASHBANG;
	}
	else if (strcmpi(typeString, "Smoke") == 0)
	{
		m_weaponType = WEAPONTYPE_LOBBED;
		m_weaponSubType = WEAPONSUBTYPE_LOBBED_SMOKE;
	}
	else if (strcmpi(typeString, "Teargas") == 0)
	{
		m_weaponType = WEAPONTYPE_LOBBED;
		m_weaponSubType = WEAPONSUBTYPE_LOBBED_TEARGAS;
	}
	else if (strcmpi(typeString, "Launcher") == 0)
		m_weaponType = WEAPONTYPE_LAUNCHER;
	else
	{
		// unkown type
		ASSERT(false);
	}

	// Some defaults
	if (m_weaponType == WEAPONTYPE_LOBBED)
		m_weaponHold = WEAPONHOLD_LOBBED;

	// Setup the hold type
	if (strcmpi(m_holdType, "swing") == 0)
		m_weaponHold = WEAPONHOLD_MELEE_SWING;
 	else if (strcmpi(m_holdType, "stab") == 0)
		m_weaponHold = WEAPONHOLD_MELEE_STAB;
	else if (strcmpi(m_holdType, "spray") == 0)
		m_weaponHold = WEAPONHOLD_MELEE_SPRAY;
	else if (strcmpi(m_holdType, "touch") == 0)
		m_weaponHold = WEAPONHOLD_MELEE_TOUCH;
	else if (strcmpi(m_holdType, "pistol") == 0)
		m_weaponHold = WEAPONHOLD_PISTOL;
	else if (strcmpi(m_holdType, "dual") == 0)
		m_weaponHold = WEAPONHOLD_DUALPISTOL;
	else if (strcmpi(m_holdType, "long2hand") == 0)
		m_weaponHold = WEAPONHOLD_LONG2HAND;
	else if (strcmpi(m_holdType, "short2hand") == 0)
		m_weaponHold = WEAPONHOLD_SHORT2HAND;
	else if (strcmpi(m_holdType, "lobbed") == 0)
		m_weaponHold = WEAPONHOLD_LOBBED;
	else if (strcmpi(m_holdType, "sniper") == 0)
		m_weaponHold = WEAPONHOLD_SNIPER;
	else if (strcmpi(m_holdType, "sniperp") == 0)
		m_weaponHold = WEAPONHOLD_SNIPERP;
	else if (strcmpi(m_holdType, "rocket") == 0)
		m_weaponHold = WEAPONHOLD_ROCKET;

	ASSERT(m_weaponHold != WEAPONHOLD_NONE);

	if (strcmpi(m_reloadType, "gripclip") == 0)
		m_weaponReload = WEAPONRELOAD_GRIPCLIP;
	else if (strcmpi(m_reloadType, "dual") == 0)
		m_weaponReload = WEAPONRELOAD_DUALPISTOL;
	else if (strcmpi(m_reloadType, "revolver") == 0)
		m_weaponReload = WEAPONRELOAD_REVOLVER;
	else if (strcmpi(m_reloadType, "shotgun") == 0)
		m_weaponReload = WEAPONRELOAD_SHOTGUN;
	else if (strcmpi(m_reloadType, "subclip") == 0)
		m_weaponReload = WEAPONRELOAD_SUBCLIP;
	else if (strcmpi(m_reloadType, "rifleclip") == 0)
		m_weaponReload = WEAPONRELOAD_RIFLECLIP;
	else if (strcmpi(m_reloadType, "bullpup") == 0)
		m_weaponReload = WEAPONRELOAD_BULLPUP;
	else if (strcmpi(m_reloadType, "bolt") == 0)
		m_weaponReload = WEAPONRELOAD_RIFLEBOLT;

	// Parse the restriction code(s)
	int len = strlen(m_restrict);
	for (int i = 0; i < len; i++)
	{
		switch (m_restrict[i])
		{
			case 'c':
				m_weaponRestrict |= ALLOWCOPS;
				break;
			case 'g':
				m_weaponRestrict |= ALLOWGANGSTER;
				break;
			case 'm':
				m_weaponRestrict |= ALLOWMULTIPLAYER;
				break;
			case 'n':
				m_weaponRestrict |= NPCONLY;
				break;
			case 's':
				m_weaponRestrict |= SUPERWEAPON;
				break;
			case '?':
				m_weaponRestrict |= TYPEDEFAULT;
				break;
			case '*':
				m_weaponRestrict |= SLOTDEFAULT;
				break;
		}
	}

	// This is an odd case
	if (!Test(ALLOWCOPS) && !Test(ALLOWGANGSTER))
		m_weaponRestrict |= (ALLOWCOPS | ALLOWGANGSTER);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLWeapon::TTLWeapon(
const TTLWeaponProperties &properties) :
Weapon(properties),
m_ttlProps(properties)
{
	// We require this
	ASSERT(m_aimSight);

	// Register the callback
	m_aimSim.RegisterPenetrateCallback(SeeThru::AimSimCallback, this);

	m_cameraShakeAim = false;

	// No fire sound
	m_fireHandle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
TTLWeapon::~TTLWeapon()
{
	Sfx::Stop(m_fireHandle);
}

/////////////////////////////////////////////////////////////////////////////
// Monkey with the aim
/////////////////////////////////////////////////////////////////////////////
void TTLWeapon::SetAim(
const Vector3 &eye,
const Vector3 &dir)
{
	if (IsDead(m_owner))
		ClearAim();
	else
	{
		if (m_cameraShakeAim && g_shooterCamPtr && g_shooterCam.GetFollow() == CActor::FromHandle(m_owner))
			Weapon::SetAim(g_shooterCam.Eye(), g_shooterCam.Dir());
		else
			Weapon::SetAim(eye, dir);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeapon::SetAim(
const Vector3 &target,
float deviance)
{
	if (IsDead(m_owner))
		ClearAim();
	else
		Weapon::SetAim(target, deviance);
}

/////////////////////////////////////////////////////////////////////////////
// Interacting with ammo
/////////////////////////////////////////////////////////////////////////////
const char *TTLWeapon::GetAmmoType()
{
	TTLProjectile *projectile = (TTLProjectile *)m_projectile;
	if (!projectile)
		return "";
	else
		return projectile->GetAmmoType();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeapon::Activate()
{
	Weapon::Activate();

	CCombatant *combatant = CCombatant::FromHandle(m_owner);
	if (combatant)
	{
		//Npcs don't follow movement restrictions
		if (!combatant->GetIsNpc())
		{
			Property *prop = combatant->QueryProperty("moveMultiplier");
			if (prop)
				prop->Set(m_ttlProps.MovementFactor());

			prop = combatant->QueryProperty( "aimTurning" );
			if (prop)
				prop->Set(m_ttlProps.AimTurningFactor());
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeapon::Deactivate()
{
	Weapon::Deactivate();

	CCombatant *combatant = CCombatant::FromHandle(m_owner);
	if (m_owner)
	{
		if (!combatant->GetIsNpc())
		{
			Property *prop = combatant->QueryProperty("moveMultiplier");
			if (prop)
				prop->Set(1.0f);
			prop = combatant->QueryProperty( "aimTurning" );
			if (prop)
				prop->Set(1.0f);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Play the sound
/////////////////////////////////////////////////////////////////////////////
void TTLWeapon::PlaySound(
const char *sound,
SoundLocation location,
int useModel)
{
	if (!sound || sound[0] == '\0')
		return;

	SAttachedSound msg;
	msg.d_actor = CActor::FromHandle(m_owner);
	if (!msg.d_actor)
		return;
	msg.d_name = sound;
	msg.d_volume = 1.0f;
	msg.d_pitch = 0.0f;
	g_messageDispatcher.SendMessageToAll("PlayAttachedSound", (void*)&msg, m_owner);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeapon::FireSound(
const char *sound,
float pitch,
int model)
{
	Sfx::Stop(m_fireHandle);

	CActor *actor = CActor::FromHandle(m_owner);
	if (actor)
		Sfx::Play(sound, *actor, Sfx::ACTORPLAYPOS_DEFAULT, 1.0f, pitch, &m_fireHandle);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLWeapon::InFront(
const Vector3 &pos,
float &dSquared)
{
	// Get the camera
	Camera &camera = g_cameraDirector.CurrentCamera();
	Vector3 toShooter = pos - camera.Eye();
	dSquared = toShooter.LengthSquared();
	return (Vector3::Dot(camera.Dir(), toShooter) > 0.0f);
}

/////////////////////////////////////////////////////////////////////////////
// are we dead?
/////////////////////////////////////////////////////////////////////////////
bool TTLWeapon::IsDead(
ActorHandle who)
{
	bool dead = false;
	CCompActor *actor = CCompActor::FromHandle(who);
	if (actor)
	{
		Property *health = actor->GetProperty("health");
		if (health && health->GetFloat() <= 0.0f)
			dead = true;
	}

	return dead;
}

/////////////////////////////////////////////////////////////////////////////
// Init stuff
/////////////////////////////////////////////////////////////////////////////
WeaponSoundParticle::WeaponSoundParticle()
{
	m_sound[0] = '\0';
	m_particle[0] = '\0';
	m_volume = 1.0f;
	m_scale = 1.0f;
}

/////////////////////////////////////////////////////////////////////////////
// Register the context
/////////////////////////////////////////////////////////////////////////////
void WeaponSoundParticle::RegisterContext(
const char *context,
const char *name)
{
	char var[128];

	sprintf(var, "%sSound", name);
	g_console.CreateVar(context, var, m_sound, sizeof(m_sound) - 1);

	sprintf(var, "%sParticle", name);
	g_console.CreateVar(context, var, m_particle, sizeof(m_particle) - 1);

	sprintf(var, "%sSoundVolume", name);
	g_console.CreateVar(context, var, m_volume);

	sprintf(var, "%sParticleScale", name);
	g_console.CreateVar(context, var, m_scale);
}

/////////////////////////////////////////////////////////////////////////////
// Do the particle/sound
/////////////////////////////////////////////////////////////////////////////
void WeaponSoundParticle::DoStationary(
const Vector3 &pos,
const Vector3 *normal,
float modifier,
SoundHandle *handle) const
{
	// Make the sound
	if (m_sound[0])
	{
		SStationarySound msg;
		msg.d_name = m_sound;
		msg.d_pos = pos;
		msg.d_volume = modifier;
		g_messageDispatcher.SendMessageToAll("PlayStationarySound", (void*)&msg, INVALID_OBJECT_HANDLE);
	}

	// Make the particle
	if (m_particle[0])
	{
		SParticleEffect msg;
		msg.d_name = m_particle;
		msg.d_pos = pos;
		if (normal)
			msg.d_normal = *normal;
		else
			msg.d_normal.Clear();
		msg.d_scale = m_scale * modifier;
		g_messageDispatcher.SendMessageToAll("ParticleEffect", (void*)&msg, INVALID_OBJECT_HANDLE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Do the particle/sound
/////////////////////////////////////////////////////////////////////////////
void WeaponSoundParticle::Do(
ActorHandle actor,
const Vector3 &pos,
const Vector3 *normal,
float modifier,
SoundHandle *handle) const
{
	// Doh
	CActor *realActor = CActor::FromHandle(actor);
	ASSERT(realActor);
	if (!realActor)
		return;

	// Make the sound
	if (m_sound[0] && realActor)
	{
		SAttachedSound msg;
		msg.d_actor = realActor;
		msg.d_name = m_sound;
		msg.d_volume = modifier;
		g_messageDispatcher.SendMessageToAll("PlayAttachedSound", (void*)&msg, actor);
	}

	// Make the particle
	if (m_particle[0])
	{
		SParticleEffect msg;
		msg.d_name = m_particle;
		msg.d_pos = pos;
		if (normal)
			msg.d_normal = *normal;
		else
			msg.d_normal.Clear();
		msg.d_scale = m_scale * modifier;
		g_messageDispatcher.SendMessageToAll("ParticleEffect", (void*)&msg, INVALID_OBJECT_HANDLE);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EmitterWrapper::EmitterWrapper()
{
	m_actor = (ActorHandle)INVALID_OBJECT_HANDLE;;
	m_boneID = INVALID_BONEID;
	m_attachPoints = NULL;
	m_attachPoint = INVALID_ATTACH_POINT;
	m_handle = INVALID_HANDLE;
	m_pos.Clear();
	m_velocity.Clear();
	m_up.Set(0.0f, 1.0f, 0.0f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EmitterWrapper::~EmitterWrapper()
{
	Destroy();
}

/////////////////////////////////////////////////////////////////////////////
// Create it
/////////////////////////////////////////////////////////////////////////////
bool EmitterWrapper::Create(
CActor *actor,
const char *emitter,
const char *bone)
{
	// nothing to do here
	if (!emitter || !emitter[0])
		return false;

	// duh!
	ASSERT(actor);
	m_actor = CActor::ToHandle(actor);

	// Already have one?
	Destroy();

	// Look up our bone
	if (bone && bone[0] && actor && actor->Instance() && actor->Instance()->Body())
	{
		int ret = boneGetBoneID(actor->Instance()->Body(), &m_boneID, (char *)bone);
		if (ret < 0)
			m_boneID = INVALID_BONEID;
	}

	// Do one update now
	Update();

	// Create it
	m_handle = ParticleEmitter__CreateFromRecord(emitter, &m_pos, &m_velocity, NULL, false, &m_up, false);

	// We are audi 5000
	return (m_handle != INVALID_HANDLE);
}

/////////////////////////////////////////////////////////////////////////////
// Create it
/////////////////////////////////////////////////////////////////////////////
bool EmitterWrapper::Create(
CActor *actor,
const char *emitter,
const AttachPoints *attachPoints,
AttachPointID usePoint)
{
	// nothing to do here
	if (!emitter || !emitter[0])
		return false;

	// duh!
	ASSERT(actor);
	m_actor = CActor::ToHandle(actor);

	// Already have one?
	Destroy();

	// Look up our bone
	if (attachPoints && usePoint != INVALID_ATTACH_POINT)
	{
		m_attachPoints = attachPoints;
		m_attachPoint = usePoint;
	}

	// Do one update now
	Update();

	// Create it
	m_handle = ParticleEmitter__CreateFromRecord(emitter, &m_pos, &m_velocity, NULL, false, &m_up, false);

	// We are audi 5000
	return (m_handle != INVALID_HANDLE);
}

/////////////////////////////////////////////////////////////////////////////
// destroy it
/////////////////////////////////////////////////////////////////////////////
void EmitterWrapper::Destroy()
{
	if (m_handle != INVALID_HANDLE)
	{
		ParticleEmitter__Kill(m_handle);
		m_handle = INVALID_HANDLE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Activate/deactivate
/////////////////////////////////////////////////////////////////////////////
void EmitterWrapper::Activate()
{
	if (m_handle != INVALID_HANDLE)
		ParticleEmitter__Activate(m_handle);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void EmitterWrapper::Deactivate()
{
	if (m_handle != INVALID_HANDLE)
		ParticleEmitter__Deactivate(m_handle);
}

/////////////////////////////////////////////////////////////////////////////
// Update
/////////////////////////////////////////////////////////////////////////////
void EmitterWrapper::Update()
{
	// Update the quantities
	CActor *actor = CActor::FromHandle(m_actor);
	if (!actor)
		return;

	// Everything has the same velocity...
	m_velocity = actor->GetVelocityInWorld();

	// Boned or not?
	if (m_boneID != INVALID_BONEID)
	{
		// Do the bone
		Graphics4x4 boneToModel;
		ASSERT(actor->Instance());
		ASSERT(actor->Instance()->Body());
	 	boneGetCurrentBoneToModel(actor->Instance()->Body(), m_boneID, &boneToModel);
		Graphics4x4 boneToWorld = boneToModel * actor->Instance()->Matrix();

		m_pos = boneToWorld.GetRow3();
		m_up = boneToWorld.GetRow2();
	}
	else if (m_attachPoint != INVALID_ATTACH_POINT)
	{
		Graphics4x4 ap = m_attachPoints->ComputeMatrix(m_attachPoint);
		m_pos = ap.GetRow3() * actor->Instance()->Matrix();
		m_up = ap.GetRow2();
	}
	else // Straight up actor center
	{
		m_pos = actor->GetBodyInWorld();
		m_up = actor->Instance()->Matrix().GetRow1();
	}
}

