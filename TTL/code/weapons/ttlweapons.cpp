/////////////////////////////////////////////////////////////////////////////
// TTL weapons systems
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "weapons/ttlweapons.h"
#include "Components/weapons/weaplib.h"
#include "Environ/Munition.h"
#include "Environ/World.h"
#include "stage/SurfaceType.h"
#include "weapons/ttlprojectile.h"
#include "Effects/collisionparticle.h"

DeclareSingleton(TTLWeapons);

const int defaultDecals = 128;
const float defaultSize = 0.03f;
const float defaultEnergyD = 800.0f;
const float defaultNScale = 0.5f;

bool g_superParticleHack = false;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLWeapons::TTLWeapons()
{
	// Name our events
	g_aimSimProps.AddEvent("jump", 0.0f);
	g_aimSimProps.AddEvent("crouch", 0.0f);
	g_aimSimProps.AddEvent("stand", 0.0f);
	g_aimSimProps.AddEvent("hit", 0.0f);
	g_aimSimProps.AddEvent("explode", 0.0f);
	g_aimSimProps.AddEvent("fall", 0.0f);
	g_aimSimProps.AddEvent("kick", 0.0f);

	// Setup the stuff for our events
	g_aimSimProps.LoadScript("aimsim");

	d_munitionHit.Initialize("MunitionHit",
										MESSAGE_HANDLE_IF_MASTER |	MESSAGE_HANDLE_IF_DUPLICA | MESSAGE_ROUTE_TO_NEIGHBORS,
										g_world.GetMessageOwner(), this);

	// Setup the array of decal emitters
	d_decalEmitter = NULL;
	if (d_decalSet.Load("weapons\\decals.dbl"))
	{
		// How many emitters do we need?
		d_decalEmitters = d_decalSet.Textures();

		// Create it
		if (d_decalEmitters > 0)
		{
			d_decalEmitter = new RoundDecalEmitter *[d_decalEmitters];
			ASSERT(d_decalEmitter);
			for (int s = 0; s < d_decalEmitters; s++)
			{
				d_decalEmitter[s] = new RoundDecalEmitter(d_decalSet, s);
				if (d_decalEmitter[s])
				{
					d_decalEmitter[s]->Setup(defaultDecals);
					d_decalEmitter[s]->SetCoplanarCheck(DECAL_COPLANARCHECK_CHEAP);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TTLWeapons::~TTLWeapons()
{
	for (int s = 0; s < d_decalEmitters; s++)
		delete d_decalEmitter[s];
	delete [] d_decalEmitter;
	g_aimSimProps.Reset();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeapons::MunitionImpactFX(
const SMunitionImpact &hit)
{
	// Look up the projectile that is hitting
	TTLProjectileProperties *projectile = (TTLProjectileProperties *)g_projectileLib.GetProperties(hit.d_munitionType);
	ASSERT(projectile);

	// Let the projectile have a crack at it
	if (projectile)
		projectile->MunitionImpactFX(hit);

	// Look it up!
	SCollisionParticleData *impactFX = g_collisionParticle.GetEntry(projectile->m_surfaceType, hit.d_surfaceMaterial);
	ASSERT(impactFX);
	if (!impactFX)
		return;

	// Play the sound
	if (impactFX->d_sound)
		Sfx::PlayStationary(impactFX->d_sound, hit.d_intersection);

	// Super hack!
	if (g_superParticleHack &&  hit.d_surfaceMaterial == 0 &&
			 impactFX->d_particle[0] == 'D' && impactFX->d_particle[1] == 'e' && impactFX->d_particle[2] == 'f')
		strcpy(impactFX->d_particle, "9mmStrike");

	// Do the particle
	if (impactFX->d_particle[0])
	{
		const float offset = 0.02f;
		Vector3Padded pos = hit.d_intersection + hit.d_surfaceNormal * offset;
		Vector3Padded velUnitVector = hit.d_surfaceNormal;
		const int randomScalePercent = 15;
		ParticleMgr__CreateEffect(impactFX->d_particle, &pos,
											&velUnitVector, NULL, NULL,
											0.0f, impactFX->d_particleScale, randomScalePercent);
	}

	// Place a decal
	if (impactFX->d_decalIndex >= 0 && impactFX->d_decalIndex < d_decalEmitters)
	{
		RoundDecalEmitter *emitter = d_decalEmitter[impactFX->d_decalIndex];
		if (emitter)
		{
			ts_bRGBA intensity = ToIntensity(hit.d_surfaceColor);
			emitter->Create(hit.d_intersection, hit.d_surfaceNormal, impactFX->d_decalRadius, &intensity);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TTLWeapons::CMunitionHit::HandleMessage(
void *i_data,
CActor *i_sender,
bool i_posted)
{
	SMunitionImpact *hit = (SMunitionImpact *)i_data;
	d_ref->MunitionImpactFX(*hit);
}

/////////////////////////////////////////////////////////////////////////////
// A handy utility (why aren't these in the enigne)
/////////////////////////////////////////////////////////////////////////////
inline ts_bRGBA TTLWeapons::ToIntensity(
u16 color,
u8 alpha)
{
	ts_bRGBA c;
	int r = ((color >> 0) & 0x1f) << 3;
	int g = ((color >> 5) & 0x3f) << 2;
	int b = ((color >> 11) & 0x1f) << 3;
	int intensity = Math::Clamp((r + g + b) / 3, 64, 255);
	c.R = c.G = c.B = intensity;
	c.A = alpha;
	return c;
}

