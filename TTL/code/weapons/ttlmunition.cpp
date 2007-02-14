/////////////////////////////////////////////////////////////////////////////
// TTL Munition systems
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Components/weapons/weaplib.h"
#include "Environ/Munition.h"
#include "Environ/World.h"
#include "stage/SurfaceType.h"
#include "weapons/ttlprojectile.h"
#include "Effects/collisionparticle.h"
#include "effects/blood.h"
#include "weapons/explodefx.h"
#include "weapons/ttlweapon.h"
#include "weapons/lobbed.h"
#include "weapons/ttlmunition.h"

DeclareSingleton(TTLMunition);

#ifdef PS2
const int defaultDecals = 64;
#else
const int defaultDecals = 128;
#endif
const float defaultSize = 0.03f;
const float defaultEnergyD = 800.0f;
const float defaultNScale = 0.5f;
const float maxD = 70.0f;
const float maxDSquared = 70.0f * 70.0f;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLMunition::TTLMunition()
{
	// Name our events
	g_aimSimProps.AddEvent("jump", 0.0f);
	g_aimSimProps.AddEvent("crouch", 0.0f);
	g_aimSimProps.AddEvent("stand", 0.0f);
	g_aimSimProps.AddEvent("hit", 0.0f);
	g_aimSimProps.AddEvent("explode", 0.0f);
	g_aimSimProps.AddEvent("fall", 0.0f);
	g_aimSimProps.AddEvent("kick", 0.0f);
	g_aimSimProps.AddEvent("sprint", 0.0f);

	// Setup the stuff for our events
	g_aimSimProps.LoadScript("aimsim");

	// The nothing material
	d_nothingMaterial = g_surfaceType.GetIndex("nothing");

	d_munitionImpact.Initialize(MUNITION_IMPACT,
											MESSAGE_HANDLE_IF_MASTER |	MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS,
											g_world.GetMessageOwner(), this);

	d_explodeFX.Initialize(MUNITION_EXPLODEFX,
											MESSAGE_HANDLE_IF_MASTER |	MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS,
											g_world.GetMessageOwner(), this);

	d_checkpointReset.Initialize("KillAllAtCheckpoint",
											MESSAGE_HANDLE_IF_MASTER,
											g_world.GetMessageOwner(), this);

	// Setup the array of decal emitters
	d_decalEmitter = NULL;
	d_decalEmitters = 0;
	if (d_decalSet.Load("weapons\\bhole.dbl"))
	{
		// How many emitters do we need?
#ifdef PS2
		d_decalEmitters = d_decalSet.Textures();
#else
		// We have a bump map here!
		ASSERT((d_decalSet.Textures() & 1) == 0);
		d_decalEmitters = d_decalSet.Textures() / 2;
#endif

		// Create it
		if (d_decalEmitters > 0)
		{
			d_decalEmitter = new RoundDecalEmitter *[d_decalEmitters];
			ASSERT(d_decalEmitter);
			for (int s = 0; s < d_decalEmitters; s++)
			{
#ifdef PS2
				DecalTextureLayer textureLayer(s);
				d_decalEmitter[s] = new RoundDecalEmitter(d_decalSet, &textureLayer, 1);
#else

				DecalTextureLayer textureLayer[2];
				textureLayer[0].DefaultBump(s + d_decalEmitters);
				textureLayer[1].DefaultPostBump(s);
				d_decalEmitter[s] = new RoundDecalEmitter(d_decalSet, textureLayer, 2);
#endif
				if (d_decalEmitter[s])
					d_decalEmitter[s]->Setup(defaultDecals, -3);
			}
		}
	}

	// Create the sound emitters
	const int maxImpactVoices = 4;
	bool ret = g_audio.Create3DGroup("ImpactGroup", maxImpactVoices, Reclaim::DISTANCE);
	ASSERT(ret);
	const int maxFXVoices = 2;
	ret = g_audio.Create3DGroup("MunitionFXGroup", maxFXVoices, Reclaim::OLDEST);
	ASSERT(ret);

	// Setup the impact occluder
	d_impactSoundEmitter = g_audio.Create3DEmitter("Impact", "ImpactGroup");
	ASSERT(d_impactSoundEmitter);
	if (Sfx::CurrentOcclusion() & Sfx::SFX_SFX3D_OCCLUSIONS)
		d_impactSoundEmitter->GetParams().SetupOcclusionTest(Sfx::OcclusionTest());

	// Setup the whiz occluder
	d_fxSoundEmitter = g_audio.Create3DEmitter("MunitionFX", "MunitionFXGroup");
	ASSERT(d_fxSoundEmitter);

	// Make sure the grenades are kosher
	ASSERT(LobbedActor::ActiveTotal() == 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLMunition::~TTLMunition()
{
	// Kill the emitters!
	g_audio.DestroyEmitter(d_impactSoundEmitter, false);
	g_audio.DestroyEmitter(d_fxSoundEmitter, false);

	g_audio.DestroyGroup("ImpactGroup");
	g_audio.DestroyGroup("MunitionFXGroup");

	for (int s = 0; s < d_decalEmitters; s++)
		delete d_decalEmitter[s];
	delete [] d_decalEmitter;
	g_aimSimProps.Reset();
}

/////////////////////////////////////////////////////////////////////////////
 // Call this to send a munition message
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::SendMunitionImpact(
ActorHandle shooter,
CMunitionImpactSurface &impact)
{
	// First, is this decal cool?
	impact.d_decalOkay = DecalOkay(impact);

	ASSERT(!Math::Zero((impact.d_shotOrigin - impact.d_intersection).LengthSquared()));
	g_messageDispatcher.SendMessageToOwner(MUNITION_IMPACT, &impact, shooter, g_world.GetMessageOwner());

	// The shooter is local, we have a combatant victim, and this isn't a deflected shot.
	CActor* shooterActor = CActor::FromHandle(shooter);
	uint8 attacker = CParticipantIF::GetParticipantIdFromActor(shooterActor);
	uint8 victim = CParticipantIF::GetParticipantIdFromActor(impact.d_victim);
	if (shooterActor && shooterActor->IsNetMaster() && impact.d_victim &&
		 (shooterActor != impact.d_victim) && impact.d_victim->IsA(CCombatant::ClassType()) &&
		 g_referee.GetTeamingModel().AreEnemies( attacker, victim ))
	{
			// Weapon stats - local avatar hit combatant with weapon.
			g_messageDispatcher.SendMessageToActor("WeaponStatHit", CMessageDispatcher::IntToData(impact.d_salvoSize), impact.d_victim->GetHandle(), shooter);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Send an explosion!
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::SendExplodeFX(
ActorHandle shooter,
const SMunitionExplodeFX &fx)
{
	g_messageDispatcher.SendMessageToOwner(MUNITION_EXPLODEFX, (void *)&fx, shooter, g_world.GetMessageOwner());
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLMunition::PlaySFX(
const char *sfxName,
const Vector3 &pos,
float volume,
float pitch,
float presence)
{                      	// Play the sound
	if (*sfxName == '\0' || !d_fxSoundEmitter)
		return false;

	SoundHandle sound = d_fxSoundEmitter->Cue(sfxName);
	if (!sound)
		return false;

	d_fxSoundEmitter->SetVolume(sound, volume);
	d_fxSoundEmitter->SetPitch(sound, pitch);
	d_fxSoundEmitter->SetPresence(sound, presence);
	d_fxSoundEmitter->SetPosition(sound, pos);
	d_fxSoundEmitter->Play(sound);
	delete sound;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
u8 TTLMunition::NothingMaterial()
{
	return d_nothingMaterial;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::AckMunitionImpact(
ActorHandle shooter,
CMunitionImpact *impact,
CActor *victim,
bool ack)
{
	// Did it work?
	if (ack)
	{
		ActorHandle victimHandle = CActor::ToHandle(victim);
		g_messageDispatcher.SendMessageToActor(MUNITION_IMPACT_ACK, (void *)ack, victimHandle, shooter);
		g_messageDispatcher.SendMessageToActor(MUNITION_IMPACT_ACTORFX, impact, shooter, victimHandle);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::MunitionImpact(
ActorHandle shooter,
CMunitionImpactSurface &impact)
{
	// First shot and not an explosion?
	if (impact.d_firstShot)
	{
		TTLWeaponProperties *props = (TTLWeaponProperties *)g_weaponLib.GetProperties(impact.d_weaponID);
		ASSERT( props );
		if (props && props->m_weaponClass == WEAPONCLASS_FIREARM)
			g_messageDispatcher.SendMessageToActor(MUNITION_FIREDFX, &impact, shooter, shooter);
	}
	g_messageDispatcher.SendMessageToLocalHost(MUNITION_FIRED, &impact, shooter);

	// Is there a victim?
	if (impact.d_victim)
	{
		ActorHandle victimHandle = impact.d_victim->GetHandle();
		if (impact.d_victim->IsNetMaster())
			g_messageDispatcher.SendMessageToActor(MUNITION_IMPACT_ACTOR, &impact, shooter, victimHandle);
		else
			g_messageDispatcher.SendMessageToActor(MUNITION_IMPACT_ACTORFX, &impact, shooter, victimHandle);
	}

	// Run the FX
	MunitionImpactFX(impact);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::MunitionImpactFX(
const CMunitionImpactSurface &impact)
{
	// Look up the projectile that is hitting
	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties(impact.d_munitionID);
	ASSERT(projectile);

	// Let the projectile have a crack at it
	if (projectile)
		projectile->MunitionImpactFX(impact);

	// Look it up!
	SCollisionParticleData *impactFX = g_collisionParticle.GetEntry(projectile->m_surfaceType, impact.d_surfaceMaterial);
	if (!impactFX)
		return;

	// Play the sound
	if (*impactFX->d_sound && d_impactSoundEmitter)
	{
		SoundHandle sound = d_impactSoundEmitter->Cue(impactFX->d_sound);
		if (sound)
		{
 			d_impactSoundEmitter->SetPosition(sound, impact.d_intersection);
			d_impactSoundEmitter->Play(sound);
			delete sound;
		}
	}

	// Is blood on?
	bool showParticle = true;
	if (!g_blood.Enabled() && impactFX->d_particle[0])
	{
		if (strcmp(impactFX->d_particle, "BloodSpray") == 0 || strcmp(impactFX->d_particle, "Blood_splash") == 0)
			showParticle = false;
	}

	// Do the particle
	float d;
	if (showParticle && impactFX->d_particle[0] && InView(impact.d_intersection, d))
	{
		const float offset = 0.02f;
		Vector3Padded pos = impact.d_intersection + impact.d_surfaceNormal * offset;
		Vector3Padded velUnitVector = impact.d_surfaceNormal;
		const int randomScalePercent = 15;
		const float minScale = 1.5f;
		const float maxScale = 3.0f;
		float scale = impactFX->d_particleScale * Math::LinearMap(d, 0.0f, maxD, minScale, maxScale);
		ParticleMgr__CreateEffect(impactFX->d_particle, &pos,
											&velUnitVector, NULL, NULL,
											0.0f, scale, randomScalePercent);
	}

	// Place a decal
	if (impact.d_decalOkay && impactFX->d_decalIndex >= 0 && impactFX->d_decalIndex < d_decalEmitters)
	{
		RoundDecalEmitter *emitter = d_decalEmitter[impactFX->d_decalIndex];
		if (emitter)
		{
			const float randomScaleFactor = 0.1f;
			ts_bRGBA intensity = ToIntensity(impact.d_surfaceShade);
			if (impactFX->d_decalRadius > 0.0f)
			{
				float diameter = impactFX->d_decalRadius * 2.0f * g_random.InRange(1.0f - randomScaleFactor, 1.0f + randomScaleFactor);
				emitter->Create(impact.d_intersection, impact.d_surfaceNormal, diameter, &intensity, DECAL_COPLANARCHECK_NONE);
			}
			else
			{
				float diameter = -impactFX->d_decalRadius * 2.0f * g_random.InRange(1.0f - randomScaleFactor, 1.0f + randomScaleFactor);
				float rotate = g_random.InRange(0.0f, Math::TwoPi);
				emitter->Create(impact.d_intersection, impact.d_surfaceNormal, diameter, &intensity, DECAL_COPLANARCHECK_NONE, rotate);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::MunitionImpact::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	CMunitionImpactSurface *hit = (CMunitionImpactSurface *)i_data;
	ASSERT(hit);
	d_ref->MunitionImpact(i_sender, *hit);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::ExplodeFX::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	SMunitionExplodeFX *fx = (SMunitionExplodeFX *)i_data;
	ASSERT(fx);
	if (!fx)
		return;

	ASSERT(ValidSingleton(ExplodeFXLibrary));
	g_explodeFXLibrary.HandleTrigger(*fx);

	TTLProjectileProperties *props = (TTLProjectileProperties *)g_projectileLib.GetProperties(fx->d_munitionID);
	if (props)
		props->HandleExplodeFX(i_sender, *fx);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLMunition::CheckpointReset::HandleMessage(
void *i_data,
ActorHandle i_sender,
bool i_posted)
{
	// Reset all the decals
	for (int d = 0; d < d_ref->d_decalEmitters; d++)
	{
		if (d_ref->d_decalEmitter[d])
			d_ref->d_decalEmitter[d]->Clear();
	}

	// Reset all the projectile stuff
	int projectiles = g_projectileLib.Types();
	for (int p = 0; p < projectiles; p++)
	{
		const TTLProjectileProperties *props = (const TTLProjectileProperties *)g_projectileLib.GetProperties(p);
		ASSERT(props);
		props->CheckpointReset();
	}
}

/////////////////////////////////////////////////////////////////////////////
// A handy utility (why aren't these in the enigne)
/////////////////////////////////////////////////////////////////////////////
inline ts_bRGBA TTLMunition::ToIntensity(
u8 shade,
u8 alpha)
{
	ts_bRGBA c;
	c.R = c.G = c.B = shade;
	c.A = alpha;
	return c;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool TTLMunition::DecalOkay(
CMunitionImpactSurface &impact)
{
 	// Look up the projectile that is hitting
	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties(impact.d_munitionID);
	if (!projectile)
		return false;

	SCollisionParticleData *impactFX = g_collisionParticle.GetEntry(projectile->m_surfaceType, impact.d_surfaceMaterial);
	if (!impactFX)
		return false;

	// Can we place a decal
	if (impactFX->d_decalIndex < 0 || impactFX->d_decalIndex >= d_decalEmitters || Math::Zero(impactFX->d_decalRadius))
		return false;

	RoundDecalEmitter *emitter = d_decalEmitter[impactFX->d_decalIndex];
	if (!emitter)
		return false;

	return emitter->CheckCoplanar(impact.d_intersection, impact.d_surfaceNormal, impactFX->d_decalRadius, DECAL_COPLANARCHECK_CHEAP);
}

/////////////////////////////////////////////////////////////////////////////
// Is this event in view?
/////////////////////////////////////////////////////////////////////////////
bool TTLMunition::InView(
Vector3CRef pos,
float &d)
{
	Camera &camera = g_cameraDirector.CurrentCamera();

	// Make an adjusted eye point
	const Vector3 dir = camera.Dir();
	const float backupD = 2.0f;
	Vector3 eye = camera.Eye() + dir * backupD;

	// In front?
	const Vector3 delta = pos - eye;
	if (Vector3::Dot(delta, dir) < 0.0f)
		return false;

	// Distance
	d = delta.LengthSquared();
	if (d > maxDSquared)
		return false;

	// Give them the real d
	d = Math::Sqrt(d);
	return true;
}

