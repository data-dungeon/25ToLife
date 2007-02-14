//============================================================================
//=
//= PainStats - keeps track of what pain texture stage we have on body parts
//=
//============================================================================
#include "TTLPCH.h"
#include "components/painstats.h"
#include "Components/ComponentCreator.h"
#include "effects/pain_decal.h"
#include "GameHelper/SharedBody.h"
#include "Components/RigidBody.h"
#include "CollideHelper/ObbNode.h"
#include "weapons/ttlweapon.h"
#include "effects/blood.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(CPainStats, PainStats, COMPONENT_NEUTRAL_PRIORITY);

//#define YARDSALE

static int g_painOverride = -1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<PainStatsParams>::RegisterVariables()
{
   RegisterVariable(d_data.heavyDamage, "heavyDamage", 40.0f, 0.0f, 200.0f);
   RegisterVariable(d_data.criticalDamage, "criticalDamage", 80.0f, 0.0f, 200.0f);
   RegisterVariable(d_data.decapitationDamage, "decapDamage", 80.0f, 0.0f, 200.0f);
}

//============================================================================
CPainStats::CPainStats(CCompActor &i_actor) :
CActorComponent(i_actor)
{
	// Holy magic
	m_params = CParam<PainStatsParams>::GetParam("painstats", "var");
	ASSERT_PTR(m_params);

	m_wantToDecapitate = false;
	m_decapitated = false;
	m_showingWeapon = false;

	for (int i = 0; i < CPainDecal::PAINZONES; i++)
		m_painLevel[i] = 0;

	for (int i = 0; i < MAX_YARDSALE_PARTS; i++)
		m_yardSaleSOMIndex[i] = -1;
	m_yardSaleLevel = 0;

	m_health = -1.0f;
	m_droppedWeapon = DWS_PENDING;
}

//============================================================================
void	CPainStats::Initialize()
{
	BuildYardsaleParts();
	Reset();
}

//============================================================================
void CPainStats::Reset()
{
	m_wantToDecapitate = false;
	m_decapitated = false;
	m_showingWeapon = false;

	for (int i = 0; i < CPainDecal::PAINZONES; i++)
		m_painLevel[i] = 0;
	m_yardSaleLevel = 0;

	ApplyState();

	m_health = -1.0f;
	m_droppedWeapon = DWS_PENDING;

	ApplyState();
}

//=================================================================
void CPainStats::EndUpdate(float i_deltaSec)
{
	// oooh, should we do this here?
	if (actor.IsNetMaster())
	{
		if (g_painOverride >= 0)
		{
			for (int i = 0; i < CPainDecal::PAINZONES; i++)
				m_painLevel[i] = Math::Clamp(g_painOverride, 0, CPainDecal::GetStages((CPainDecal::Zone)i));;
			ApplyState();
		}
		else
		{
			// How are we doing?
			float currentHealth = GetHealthPercentage();
			if (m_health >= 0.0f && currentHealth > m_health)
				Reset();

			if (m_wantToDecapitate && !Alive())
				Decapitate(true);
			m_wantToDecapitate = false;

			// Drop the weapons?
			if (!Alive() && m_droppedWeapon == DWS_PENDING)
			{
				DropWeapons();
				ApplyState();
			}

			// Save this off
			m_health = currentHealth;
		}
	}

	// Need to do this regardless
	if (m_droppedWeapon == DWS_DROPPED)
		HideWeapons();
	else if (!m_showingWeapon)
		ShowWeapons();
}

//=================================================================
bool CPainStats::HandleHit(
Location location,
Direction direction,
int munitionID,
float damage)
{
	ASSERT(actor.IsNetMaster());
	if (!actor.IsNetMaster())
		return false;

	// How much damage are we going to apply?
	int painPoints;
	if (damage > m_params->criticalDamage)
		painPoints = 3;
	else if (damage > m_params->heavyDamage)
		painPoints = 2;
	else
		painPoints = 1;

	// Apply it!
	switch (location)
	{
		case LOCATION_HEAD:
			{
				// decapitate??
				if (damage > m_params->decapitationDamage)
				{ 
					// Is this a munition type that can decapitate?
					TTLProjectileProperties *props = (munitionID >= 0) ? (TTLProjectileProperties *)g_projectileLib.GetProperties(munitionID) : NULL;
					if (!props || !props->Stunner())
					{
						CCompActor *comp = (actor.IsA(CCompActor::ClassType())) ? (CCombatant *)&actor : NULL;
			
						if (comp && Stumpy::CanDecapitate(*comp))
							m_wantToDecapitate = true;
					}
				}

				// Apply head pain
				YardSaleIncrease();
				UpThePain(CPainDecal::PAINZONE_HEAD, painPoints);
			}
			break;

		case LOCATION_BODY:
			if (direction == DIRECTION_FORWARD)
				UpThePain(CPainDecal::PAINZONE_BACK, painPoints);
			else
				UpThePain(CPainDecal::PAINZONE_CHEST, painPoints);
			break;

		case LOCATION_LEFTSHOULDER:
			UpThePain(CPainDecal::PAINZONE_SHOULDER_L, painPoints);
			break;

		case LOCATION_RIGHTSHOULDER:
			UpThePain(CPainDecal::PAINZONE_SHOULDER_R, painPoints);
			break;

  		case LOCATION_LEFTLEG:
			UpThePain(CPainDecal::PAINZONE_LEG_L, painPoints);
			break;

  		case LOCATION_RIGHTLEG:
			UpThePain(CPainDecal::PAINZONE_LEG_R, painPoints);
			break;
	}

	return true;
}

//============================================================================
//============================================================================
bool CPainStats::HandleHit(
CCompActor &actor,
Location location,
Direction direction,
int munitionID,
float damage)
{
	CPainStats *painStats = (CPainStats *)actor.GetComponentByName("PainStats");
	if (!painStats)
		return false;

	return painStats->HandleHit(location, direction, munitionID, damage);
}

//============================================================================
int CPainStats::GetState()
{
	return FormulateState();
}

//============================================================================
void CPainStats::SetState(int i_state)
{
	DecimateState(i_state);
	ApplyState();
}

//============================================================================
void CPainStats::UpThePain(
CPainDecal::Zone zone,
int amount)
{
	// Allow this to go from 0 to == Stages
	int newPainLevel = Math::Clamp(m_painLevel[zone] + amount, 0, CPainDecal::GetStages(zone));
	if (m_painLevel[zone] != newPainLevel)
	{
		// Up the pain
		m_painLevel[zone] = newPainLevel;

		// Generate a message
		ApplyState();
	}
}

//============================================================================
void CPainStats::Decapitate(
bool decapitate)
{
	if (m_decapitated != decapitate)
	{
		m_decapitated = decapitate;
		ApplyState();
	}
}

//============================================================================
int CPainStats::FormulateState()
{
	int state = m_decapitated ? 0x80000000 : 0;
	if (m_droppedWeapon == DWS_DROPPED)
		state |= 0x40000000;
	int zone;
	for (zone = 0; zone < CPainDecal::PAINZONES; zone++)
	{
		int value = m_painLevel[zone] & 0xf;
		state |= value << (zone * 4);
	}
#ifdef YARDSALE
	state |= (m_yardSaleLevel & 0xf) << (zone * 4);
#endif

	return state;
}

//============================================================================
void CPainStats::DecimateState(
int state)
{
	m_decapitated = (state & 0x80000000) != 0;
	m_droppedWeapon = (state & 0x40000000) != 0 ? DWS_DROPPED : DWS_NOTDROPPED;
	state &= ~0x80000000;
	for (int zone = 0; zone < CPainDecal::PAINZONES; zone++)
	{
		m_painLevel[zone] = state & 0xf;
		state >>= 4;
	}
#ifdef YARDSALE
	m_yardSaleLevel = state & 0xf;
#endif
}

//============================================================================
void CPainStats::ApplyState()
{
	// Is blood allowed?
	const bool bloodEnabled = g_blood.Enabled();

	// Apply the headless state
	if (!m_decapitated || !bloodEnabled)
		Stumpy::Recapitate(actor);
	else
	{
		if (!Stumpy::Decapitated(actor))
		{
			BoneID boneID = INVALID_BONEID;
			int ret = boneGetBoneID(actor.Instance()->Body(), &boneID, "head");
			ASSERT(ret >=0);

			Graphics4x4 boneToModel;
	 		boneGetCurrentBoneToModel(actor.Instance()->Body(), boneID, &boneToModel);
			Graphics4x4 boneToWorld = boneToModel * actor.Instance()->Matrix();

			Vector3 pos = boneToWorld.GetRow3();
			Vector3 normal(0.0, 1.0f, 0.0f);
			const int randomScalePercent = 15;
			ParticleMgr__CreateEffect("Blood_Splash_Headshot", &pos,
												&normal, NULL, NULL,
												0.0f, 1.0f, randomScalePercent);
			Stumpy::Decapitate(actor);
		}
	}

	// Apply the other states
	for (int zone = 0; zone < CPainDecal::PAINZONES; zone++)
	{
		if (bloodEnabled)
			CPainDecal::ApplyPain(actor, (CPainDecal::Zone)zone, m_painLevel[zone] - 1);
		else
			CPainDecal::RemovePain(actor, (CPainDecal::Zone)zone);
	}

	// Refresh all zones and head textures
	CPainDecal::UpdateChanges(actor);

#ifdef YARDSALE
	ApplyYardSale();
#endif
}

//============================================================================
// Health utilities
//============================================================================
float CPainStats::GetHealthPercentage()
{
	HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	return health ? health->GetHealthPercentage() : 1.0f;
}

//============================================================================
//============================================================================
float CPainStats::GetHitPoints()
{
	HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	return health ? health->GetHealth() : 100.0f;
}

//============================================================================
//============================================================================
bool CPainStats::Alive()
{
	HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	return health ? health->IsAlive() : true;
}

//============================================================================
void CPainStats::BuildYardsaleParts()
{
#ifdef YARDSALE
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return;

	int partScan = 0;
	for (int i = 0; i < MAX_YARDSALE_PARTS; i++)
	{
		m_yardSaleSOMIndex[i] = body->FindSOMIndexByName("headwear", partScan);
		if (m_yardSaleSOMIndex[i] < 0)
			break;
		partScan = m_yardSaleSOMIndex[i] + 1;
	}
#endif
}

//============================================================================
void CPainStats::ApplyYardSale()
{
#ifdef YARDSALE
	int part;
	for (part = 0; part < m_yardSaleLevel; part++)
		YardSaleEject(part);
	for (; part < MAX_YARDSALE_PARTS; part++)
		YardSaleReattach(part);
#endif
}

//============================================================================
void CPainStats::YardSaleEject(
int part)
{
#ifdef YARDSALE
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return;

	if (body->Hidden(part))
		return;

	body->Hide(part, true);

	// Make the rigid body
#endif
}

//============================================================================
void CPainStats::YardSaleReattach(
int part)
{
#ifdef YARDSALE
	SharedBody *body = (SharedBody *)actor.Instance()->DynamicModel();
	if (!body)
		return;

	if (body->Hidden(part))
		body->Hide(part, false);
#endif
}

//============================================================================
void CPainStats::YardSaleIncrease()
{
#ifdef YARDSALE
	if (m_yardSaleLevel < MAX_YARDSALE_PARTS)
	{
		++m_yardSaleLevel;
		ApplyState();
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CParam<DroppedWeaponParams>::RegisterVariables()
{
   RegisterVariable(d_data.dropPercent, "dropPercent", 50, 0, 100);
	RegisterVariable(&d_data.omegaMin, "omegaMin", Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(50.0, 50.0f, 50.0f));
	RegisterVariable(&d_data.omegaMax, "omegaMax", Vector3(1.0f, 1.0f, 3.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(50.0, 50.0f, 50.0f));
	RegisterVariable(d_data.material, "material", "grenade", sizeof(d_data.material) - 1);
   RegisterVariable(d_data.mass, "mass", 4.0f, 0.0f, 25.0f);
	RegisterVariable(&d_data.inertiaTensor, "inertiaTensor", Vector3(125.0f, 120.0f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(200.0, 200.0f, 200.0f));
   RegisterVariable(d_data.ttl, "ttl", 3.0f, 0.0f, 10.0f);
   RegisterVariable(d_data.ignoreMs, "ignoreMS", 500, 0, 5000);
   RegisterVariable(d_data.updateHz, "updateHz", 40.0f, 1.0f, 120.0f);
	RegisterVariable(d_data.bounceSound, "bounceSound", "gun_drop", sizeof(d_data.bounceSound) - 1);
   RegisterVariable(d_data.bounceDivisor, "bounceDivisor", 4.0f, 1.0f, 20.0f);
   RegisterVariable(d_data.alphaFadeRate, "alphaFadeRate", 1.0f, 0.0f, 20.0f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CPainStats::DropWeapons()
{
	m_droppedWeapon = DWS_NOTDROPPED;
	WeaponInventory *inventory = (WeaponInventory *)actor.GetComponentByName("WeaponInventory");
	if (!inventory)
		return;

	TTLWeapon *weapon = (TTLWeapon *)inventory->GetSelected();
	if (!weapon)
		return;

	// Don't drop lobbed (let that code handle it)
	if (weapon->GetWeaponClass() == WEAPONCLASS_LOBBED)
		return;

	// Holy magic
	DroppedWeaponParams *params = CParam<DroppedWeaponParams>::GetParam("dropweap", "var");
	if (!params)
		return;

	// Don't drop!
	if (g_random.UpToExcludeHigh(100) > params->dropPercent)
		return;

	for (int w = 0; w < weapon->Models(); w++)
	{
		// Create it
		WeaponActor *weaponActor = new WeaponActor(*params);
		ASSERT(weaponActor);
		if (!weaponActor)
			return;

		// set up the geometry
		const char *model = weapon->GetModel(w).name;
		ts_Geometry *geometry = g_loadSOM.Geometry(model);
		if (!geometry)
		{
			delete weaponActor;
			return;
		}
		weaponActor->SetGeometry(0, geometry);
		weaponActor->Parm().Set("model_name", model);

		// Make sure we turn if off!
		if (weapon->GetWeaponClass() == WEAPONCLASS_FIREARM)
			weapon->ReleaseTrigger();

		// We really dropped it!
		m_droppedWeapon = DWS_DROPPED;

		// Setup initial conditions
		WeaponActor::InitialState initialState;
		initialState.placement = weapon->GetWeaponPlacement(w);
		initialState.velocity = actor.GetVelocityInWorld();
		initialState.omega.X(g_random.InRange(params->omegaMin.X(), params->omegaMax.X()));
		initialState.omega.Y(g_random.InRange(params->omegaMin.Y(), params->omegaMax.Y()));
		initialState.omega.Z(g_random.InRange(params->omegaMin.Z(), params->omegaMax.Z()));
		weaponActor->Initialize(&actor, initialState);

		// put into the world - is not persistent
		g_stage.AddProtagonist(weaponActor);
		weaponActor->SetDeleteWhenNoCast(true);
		weaponActor->DistributeNetActor(false);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CPainStats::HideWeapons()
{
	m_showingWeapon = false;

	WeaponInventory *inventory = (WeaponInventory *)actor.GetComponentByName("WeaponInventory");
	if (!inventory)
		return;

	Weapon *weapon = inventory->GetSelected();
	if (!weapon)
		return;

	// Hide it on the actor
	weapon->Hide();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void CPainStats::ShowWeapons()
{
	// Set it to true whether we have a weapon or not
	m_showingWeapon = true;

	WeaponInventory *inventory = (WeaponInventory *)actor.GetComponentByName("WeaponInventory");
	if (!inventory)
		return;

	Weapon *weapon = inventory->GetSelected();
	if (!weapon)
		return;

	// Show it on the actor
	weapon->Show();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponActor::WeaponActor(const DroppedWeaponParams &params) :
d_params(params)
{
	d_surfaceType = 0;
	d_die = 0.0f;
	d_clearIgnore = 0;
	d_impactSound = NULL;

	d_alphaFading = false;
	d_alpha = 1.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
WeaponActor::~WeaponActor()
{
	Sfx::Stop(d_impactSound);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool WeaponActor::Initialize(
CActor *ignore,
const InitialState &initialState)
{
	// place in world
	Place(initialState.placement);

	// Setup collision
	CObbNode* obbNode = new CObbNode;
	SetCollisionNode(obbNode, SIMPLE, true);
	obbNode->FitObbToActor();
	Vector3 size(obbNode->GetXSize(), obbNode->GetYSize(), obbNode->GetZSize());
	Vector3 maxExtent = obbNode->GetCollisionInActor() + 0.5f * size;
	obbNode->SetExtents(maxExtent - size, maxExtent);

	if (!CCompActor::Initialize())
		return false;

	// Look up our surface type
	d_surfaceType = g_surfaceType.GetValidDynamicIndex((char *)d_params.material, d_surfaceType, 0);

	// Create the rigid body
	CRigidBody *body = (CRigidBody*)AttachComponent("RigidBody");
	ASSERT_PTR(body);

	body->SetMass(d_params.mass);
	body->SetInertia(d_params.inertiaTensor.X(), d_params.inertiaTensor.Y(), d_params.inertiaTensor.Z());

	body->SetCG(obbNode->GetCollisionInActor(), obbNode->GetCollisionToActor());
	body->SetBodyAcceleration(Vector3(0.0f, -9.81f, 0.0f), true);

	DirCos3x3 m;
	m.Identity();
	body->SetCGToWorld(m);
	body->SetCGVelocityInWorld(initialState.velocity);
	body->SetAngVelocityInCG(initialState.omega);

	// Add a generic net actor
	AttachComponent("GenericNetActor");

	// Initialize all the components
	InitializeComponents();

	// Run Forrest, run!
	SetMaxUpdateSec(1.0f / d_params.updateHz);

	// Setup the ignore
	d_ignoreEntity = ignore;
	d_clearIgnore = g_timer.GetEndMS() + d_params.ignoreMs;
	SetPositiveInclude(WORLD_CATEGORY);
	//SetPositiveExclude(ALL_ACTOR_CATEGORIES);

	// when do we die?
	d_die = g_timer.GetEndSec() + d_params.ttl;

	// Wee
	d_alphaFading = false;
	d_alpha = 1.0f;

	// Travis, where are my dogs?
	d_category |= (NO_SUPPORT_CATEGORY | NO_WEAPON_HIT_CATEGORY);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Get the updates
/////////////////////////////////////////////////////////////////////////////
void WeaponActor::EndUpdate(
float i_deltaSec)
{
	// Call the parent
	CCompActor::EndUpdate(i_deltaSec);

	// Clear the ignore
	if (d_clearIgnore && g_timer.GetEndMS() > d_clearIgnore)
	{
		d_clearIgnore = 0;
		d_ignoreEntity = NULL;
	}

	// Handle alpha fade
	if (d_alphaFading)
	{
		Math::MoveToZero(&d_alpha, d_params.alphaFadeRate * i_deltaSec);
		Instance()->SetAlpha(d_alpha);
	}

	// Time to die?
	if ((g_timer.GetEndSec() > d_die || Math::Zero(d_alpha)) && !MarkedForDeath())
	{
		// Make us go away
		Hide();
		MarkForDeath();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Accept a collision.
/////////////////////////////////////////////////////////////////////////////
void WeaponActor::CollisionAccept(
CStageEntity *i_entity,
CCollisionEvent *i_event)
{
	// Call the parent
	CCompActor::CollisionAccept(i_entity, i_event);

	// start the fade now
	d_alphaFading = true;

	// How fast are we going
	float speed = GetVelocityInWorld().Length();
	float volume = Math::Clamp(speed / d_params.bounceDivisor);

	// Make the sound
	if (!Sfx::Active(d_impactSound))
	{
		Sfx::Stop(d_impactSound);
		Sfx::Play(d_params.bounceSound, *this, Sfx::ACTORPLAYPOS_BODY, volume, 0.0f, &d_impactSound);
	}
}
