#include "TTLPCH.h"

#include "components/Ai/AiWeaponControl.h"
#include "Components/ComponentCreator.h"
#include "weapons/firearm.h"
#include "frontend/savegame.h"
#include "weapons/armoryview.h"

// Define a creator and add it to the creator list.
STANDARD_COMPONENT_CREATOR(AiWeaponControl, AiWeaponControl, COMPONENT_NEUTRAL_PRIORITY);

#define STARTING_REACTION_FACTOR 2.0f
#define TRAILING_SHOTS_TIME 0.5f
#define EYEPOINT_OFFSET_VECTOR Vector3(0.0f, 1.92f, 0.15f) //copied from perception
#define WEAPON_CHOICE_TIME 1.4f
#define NADE_MIN_DIST_SQRD 100.0f
#define NADE_MAX_DIST_SQRD 4000.0f
#define SNIPER_MIN_DIST_SQRD 250.0f
#define MELEE_MAX_DIST_SQRD 50.0f
#define UNOPTIMAL_DISTANCE 10000.0f

//weapon type info
#define PISTOL_GAP_TIME 0.5f
#define PISTOL_DEVIANCE_MIN 3.0f
#define PISTOL_DEVIANCE_MAX 4.0f

#define SHOTGUN_GAP_TIME 1.0f
#define SHOTGUN_DEVIANCE_MIN 0.0f
#define SHOTGUN_DEVIANCE_MAX 1.0f

#define ASSAULT_GAP_TIME 1.5f
#define ASSAULT_DEVIANCE_MIN 1.5f
#define ASSAULT_DEVIANCE_MAX 3.0f

#define SNIPE_GAP_TIME 2.0f
#define SNIPE_DEVIANCE_MIN 0.01f
#define SNIPE_DEVIANCE_MAX 0.6f

#define LOBBED_EXPLD_GAP_TIME 2.6f
#define LOBBED_SMOKE_GAP_TIME 10.0f
#define LOBBED_DEVIANCE_MIN 0.0f
#define LOBBED_DEVIANCE_MAX 0.2f
#define LOBBED_GAP_WITH_PRIMARY 8.0f

#define MELEE_GAP_TIME 0.1f
#define MELEE_DEVIANCE_MIN 0.0f
#define MELEE_DEVIANCE_MAX 0.0f

#define HOSTAGE_BURST_MOD 0.5f

AiWeaponControl::WeaponControl AiWeaponControl::baseWeaponControl[WEAPONTYPES] =
{
	// burst count min, max, rateOfFire, gapTime
	{ 0, 0, 0.0f, 0.0f, 0.0f  }, // WEAPONTYPE_NONE
	{ 1, 1, MELEE_GAP_TIME, MELEE_DEVIANCE_MIN, MELEE_DEVIANCE_MAX }, // WEAPONTYPE_MELEE
	{ 1, 3, PISTOL_GAP_TIME, PISTOL_DEVIANCE_MIN, PISTOL_DEVIANCE_MAX }, // WEAPONTYPE_PISTOL
	{ 1, 2, SHOTGUN_GAP_TIME, SHOTGUN_DEVIANCE_MIN, SHOTGUN_DEVIANCE_MAX }, // WEAPONTYPE_SHOTGUN
	{ 4, 9, ASSAULT_GAP_TIME, ASSAULT_DEVIANCE_MIN, ASSAULT_DEVIANCE_MAX }, // WEAPONTYPE_SUB
	{ 2, 9, ASSAULT_GAP_TIME, ASSAULT_DEVIANCE_MIN, ASSAULT_DEVIANCE_MAX }, // WEAPONTYPE_ASSAULT
	{ 1, 1, SNIPE_GAP_TIME, SNIPE_DEVIANCE_MIN, SNIPE_DEVIANCE_MAX }, // WEAPONTYPE_SNIPER
	{ 1, 1, LOBBED_EXPLD_GAP_TIME, LOBBED_DEVIANCE_MIN, LOBBED_DEVIANCE_MAX } // WEAPONTYPE_LOBBED
};

//============================================================================
AiWeaponControl::AiWeaponControl( CCompActor &i_actor ) :
CActorComponent( i_actor )
{
	Reset();
	weaponControlEnabled = false;
}
//============================================================================
AiWeaponControl::~AiWeaponControl()
{
}
//============================================================================
void AiWeaponControl::Initialize()
{
	weaponControlEnabled = false;

	aiPerception = (AiPerception*)( actor.GetComponentByName( "AiPerception" ) );
	ASSERT(aiPerception);

	//get inventory
	weaponInventory = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
	ASSERT(weaponInventory);

	// Setup the weapon helper
	weaponHelper.Initialize(*weaponInventory, actor);

	// Select the best weapon
	weaponHelper.SelectBest();
	SelectWeapon(weaponHelper.SelectedWeapon());

	// Get the base settings
	memcpy(weaponControl, baseWeaponControl, sizeof(weaponControl));

	// Perform diffulty level weapon changes
	switch(g_GlobalProfile.GameSettings.Difficulty)
	{
	case 0: //easy (this game ain't for kids)...there are other factors that do make it easier though
		break;
	case 1: //normal
		break;
	case 2: //hard
		weaponControl[WEAPONTYPE_PISTOL].devianceMin *= 0.8f;
		weaponControl[WEAPONTYPE_PISTOL].devianceMax *= 0.8f;
		weaponControl[WEAPONTYPE_SHOTGUN].devianceMin *= 0.8f;
		weaponControl[WEAPONTYPE_SHOTGUN].devianceMax *= 0.8f;
		weaponControl[WEAPONTYPE_ASSAULT].devianceMax *= 0.8f;
		weaponControl[WEAPONTYPE_ASSAULT].devianceMin *= 0.8f;
		weaponControl[WEAPONTYPE_SNIPER].devianceMin *= 0.8f;
		weaponControl[WEAPONTYPE_SNIPER].devianceMax *= 0.8f;
		break;
	case 3: //omfg
		weaponControl[WEAPONTYPE_PISTOL].devianceMin *= 0.5f;
		weaponControl[WEAPONTYPE_PISTOL].devianceMax *= 0.5f;
		weaponControl[WEAPONTYPE_SHOTGUN].devianceMin *= 0.5f;
		weaponControl[WEAPONTYPE_SHOTGUN].devianceMax *= 0.5f;
		weaponControl[WEAPONTYPE_ASSAULT].devianceMax *= 0.5f;
		weaponControl[WEAPONTYPE_ASSAULT].devianceMin *= 0.5f;
		weaponControl[WEAPONTYPE_SNIPER].devianceMin *= 0.5f;
		weaponControl[WEAPONTYPE_SNIPER].devianceMax *= 0.5f;
		break;
	}

	// Init our state
	Reset();
}
//============================================================================
void AiWeaponControl::Reset()
{
	reactionTimer = 0.0f;
	weaponChoiceTimer = g_random.InRange(WEAPON_CHOICE_TIME, WEAPON_CHOICE_TIME + 0.3f);
	fireAtWill = false;
	trailingTimer = 0.0f;
	forceSomeFiring = 0.0f;
	weaponHelper.ClearFireQueue();
	aimingAnim = false;
	lobTimer = g_random.InRange(0.0f,LOBBED_EXPLD_GAP_TIME);
	gap = ASSAULT_GAP_TIME;
	burstModifier = 1.0f;
	storeWeapon = NULL;
}
//============================================================================
void AiWeaponControl::Update(float frameSec)
{
	// If we are in force some firing, all bets are off
	Math::MoveToZero(&forceSomeFiring, frameSec);
	bool forcingSomeFiring = !Math::Zero(forceSomeFiring);
	if (forcingSomeFiring)
	{
		if (!weaponHelper.FireQueued())
		{
			// Setup a phony target
			//Vector3 eyeOffset = EYEPOINT_OFFSET_VECTOR;
			Vector3 aimEye = actor.GetFocusInWorld();
			//aimEye.y(aimEye.y() + eyeOffset.y());
			Vector3 pos = aimEye + actor.GetBodyToWorld().GetRow2() * 2.0f;
			weaponHelper.SelectBest();
			SelectWeapon(weaponHelper.SelectedWeapon());
			weaponHelper.SetTarget(pos);

			// Do the work!
			QueueFire();
		}
	}
	else if (weaponControlEnabled)
	{
		// Fire at will means we do whatever we want
		Math::MoveToZero(&reactionTimer, frameSec);
		Math::MoveToZero(&trailingTimer,frameSec);
		Math::MoveToZero(&gap,frameSec);
		if (!fireAtWill && Math::Zero(trailingTimer))
		{
			weaponHelper.ClearFireQueue();
			weaponHelper.AbortFire();
		}
		else
		{
			if(Math::Zero(reactionTimer))
			{
				int weap = weaponHelper.SelectedWeapon();
				DecideBestWeapon(frameSec);
				if(weap != weaponHelper.SelectedWeapon())
				{
					//new weapon decided
					gap = 0.0f;
				}
				else if(gap == 0.0f && !weaponHelper.FireQueued())
				{
					weaponHelper.SetNexFireEvent(0.0f); //fire now
					CheatForAmmo();
					QueueFire();
					const WeaponControl &control = weaponControl[weaponHelper.GetType()];
					gap = control.gapTime;
					//override for smoke and tear
				}
			}
		}
	}

	// Handle the pose
	if(!aimingAnim && (aiPerception->inSight || weaponHelper.FireQueued()))
	{
		g_messageDispatcher.SendMessageToActor("StopFullBodyAnimation",NULL,actor.GetHandle(),actor.GetHandle());
		BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
		if( model )
			model->StartAim();
		aimingAnim = true;
	}

	// Give the weapon helper some time
	weaponHelper.Update();
}
//============================================================================
void AiWeaponControl::SetWeaponControlEnabled(bool enabled)
{
	weaponControlEnabled = enabled;
	if(!enabled)
	{
		if (!Dead())
		{
			BipedModel* model = (BipedModel*)actor.GetComponentByName( "BipedModel" );
			if( model )
				model->StopAim();
			aimingAnim = false;
			weaponHelper.SelectWeapon(-1);
		}
		weaponHelper.ClearFireQueue();
		reactionTimer = WEAPON_REACTION_TIME;
	}
	else
	{
		weaponChoiceTimer = 0.0f;
		DecideBestWeapon(0.0f); //initial weapon select
	}
}
//============================================================================
void AiWeaponControl::AimAtTarget()
{
	CActor* target = NULL;
	if(aiPerception && aiPerception->LastKnownTarget() != INVALID_OBJECT_HANDLE)
		target = CActor::FromHandle(aiPerception->LastKnownTarget());
	else
		return;

	Vector3 targetPos = aiPerception->GetLastCentroid();

	#define FOCUS_Y_OFFSET 0.4f
	//unless it lowers below the ground, bring the aim down a bit
	if(targetPos.y() - FOCUS_Y_OFFSET > target->GetBodyInWorld().y())
		targetPos.y(targetPos.y() - FOCUS_Y_OFFSET);

	//DrawUtility::Sphere(targetPos,DrawUtility::RED, 0.25f);

	//if (!target)
	weaponHelper.SetTarget(targetPos);
	//else
	//{
	//	Vector3 localpoint = (targetPos - target->GetBodyInWorld()) / target->GetBodyToWorld();
	//	weaponHelper.SetTarget(*target, localpoint);
	//}
}
//============================================================================
void AiWeaponControl::FireAtWill(bool i_fire)
{
	if(fireAtWill == i_fire)
		return;

	if(i_fire)
	{
		if(reactionTimer > 0.0f)
			return;
		else
			reactionTimer = WEAPON_REACTION_TIME;
	}

	// Are they turning it off
	if (fireAtWill && !i_fire)
		trailingTimer = TRAILING_SHOTS_TIME;

	fireAtWill = i_fire;
}
//============================================================================
void AiWeaponControl::ForceSomeFiring(float seconds)
{
	forceSomeFiring = seconds;
}
//============================================================================
void AiWeaponControl::SelectNoWeapon()
{	
	if (!Dead())
		weaponHelper.SelectWeapon(-1);
}
//============================================================================
void AiWeaponControl::DecideBestWeapon(float frameSec)
{
	float distanceSqrd = 0.0f;
	if(!aiPerception->GetLastDistanceSquared(distanceSqrd))
		return;  //must have never seen a target yet

	//manage time window
	Math::MoveToZero(&weaponChoiceTimer, frameSec);
	Math::MoveToZero(&lobTimer, frameSec);
	if(weaponChoiceTimer > 0.0f)
		return;
	weaponChoiceTimer = WEAPON_CHOICE_TIME;

	int currentWeapon = weaponHelper.SelectedWeapon();
   
	if(lobTimer == 0.0f && LobbedFeasible(distanceSqrd))
	{
		if(currentWeapon != TTLWeapon::LOBBED)
		{
			SelectWeapon(TTLWeapon::LOBBED);
			gap = 0.0f; //fire now
		}

		switch(weaponHelper.GetSubType())
		{
		case WEAPONSUBTYPE_LOBBED_FLASHBANG:
		case WEAPONSUBTYPE_LOBBED_SMOKE:
		case WEAPONSUBTYPE_LOBBED_TEARGAS:
			lobTimer = LOBBED_SMOKE_GAP_TIME;
			weaponChoiceTimer = 1.0f;
			break;
		default:
			if(weaponHelper.GetType(TTLWeapon::PRIMARY) == WEAPONTYPE_NONE && weaponHelper.GetType(TTLWeapon::SECONDARY) == WEAPONTYPE_NONE)
			{
				lobTimer = 0.0f;
				weaponChoiceTimer = 1.5f; //min to make sure he doesn't whip out a pistol while nade in air
			}
			else
			{
				lobTimer = LOBBED_GAP_WITH_PRIMARY;
				weaponChoiceTimer = 1.0f;
			}
			break;
		}
	return;
	}

	if(MeleeFeasible(distanceSqrd))
	{
		if(currentWeapon != TTLWeapon::MELEE)
			SelectWeapon(TTLWeapon::MELEE);
		return;
	}

	if(PrimaryFeasible(distanceSqrd))
	{
		if(currentWeapon != TTLWeapon::PRIMARY)
			SelectWeapon(TTLWeapon::PRIMARY);
		return;
	}

	if(SecondaryFeasible(distanceSqrd))
	{
		if(!weaponHelper.HasAmmo(TTLWeapon::SECONDARY))
			weaponHelper.CheatForMoreAmmo(TTLWeapon::SECONDARY);

		if(currentWeapon != TTLWeapon::SECONDARY)
			SelectWeapon(TTLWeapon::SECONDARY);
		return;
	}
}
//============================================================================
void AiWeaponControl::SelectWeapon(
int weapon)
{
	if(weapon == TTLWeapon::MELEE && storeWeapon != NULL)
	{
		WeaponInventory *inv = (WeaponInventory *)actor.GetComponentByName( "WeaponInventory" );
		if (inv)
		{
			WeaponLoadout loadout;
			loadout.AddWeapon(storeWeapon, 0);
			loadout.Build(*inv, false);
		}
		storeWeapon = NULL;
	}

	// Select the weapon
	weaponHelper.SelectWeapon(weapon);

	// Setup the gap
	weaponHelper.SetFireGap(weaponControl[weaponHelper.GetType()].gapTime);
}
//============================================================================
void AiWeaponControl::QueueFire(
int bursts)
{
	// Anything to do?
	if (weaponHelper.SelectedWeapon() < 0 || !weaponHelper.HasAmmo() || weaponHelper.GetType() == WEAPONTYPE_NONE)
		return;

	// What control struct are we using?
	const WeaponControl &control = weaponControl[weaponHelper.GetType()];

	// queue it!
	const float deviance = 0.0f;
	for (int i = 0; i < bursts; i++)
	{
		int burstCount = g_random.InRange(control.burstCountMin, control.burstCountMax);
		float deviance = g_random.InRange(control.devianceMin, control.devianceMax);
		weaponHelper.Fire(Math::Clamp(int(burstCount * burstModifier),1,999), deviance);
	}
}
//============================================================================
bool AiWeaponControl::SelectHostageWeapon(bool selectIt)
{
	if(selectIt == false)
	{
		burstModifier = 1.0f;
		if(weaponChoiceTimer > WEAPON_CHOICE_TIME)
			weaponChoiceTimer = WEAPON_CHOICE_TIME;
		return true;
	}
	WeaponType type = weaponHelper.GetType(TTLWeapon::SECONDARY);
	if(type != WEAPONTYPE_PISTOL)
		return false;
   SelectWeapon(TTLWeapon::SECONDARY);
	weaponChoiceTimer = 10000.0f;
	burstModifier = HOSTAGE_BURST_MOD;
	return true;
}
//============================================================================
void AiWeaponControl::CeaseFire(float minInterludeTime)
{
	fireAtWill = false;
	trailingTimer = 0.0f;
	reactionTimer = minInterludeTime;
	weaponHelper.ClearFireQueue();
	weaponHelper.AbortFire();
}
//============================================================================
bool AiWeaponControl::Dead()
{
	HealthIF *health = (HealthIF *)actor.GetInterface(HealthIF::GetID());
	return health ? !health->IsAlive() : false;
}
//============================================================================
bool AiWeaponControl::PrimaryFeasible(float distanceSqrd)
{
	if(weaponHelper.GetType(TTLWeapon::PRIMARY) == WEAPONTYPE_NONE)
		return false;
	if(!weaponHelper.HasAmmo(TTLWeapon::PRIMARY))
		weaponHelper.CheatForMoreAmmo(TTLWeapon::PRIMARY);
	if(!weaponHelper.HasAmmo(TTLWeapon::PRIMARY))
		return false;
	if(weaponHelper.GetType(TTLWeapon::PRIMARY) != WEAPONTYPE_SNIPER)
		return true;
	if(distanceSqrd < SNIPER_MIN_DIST_SQRD)
		return false;
	return true;
}
//============================================================================
bool AiWeaponControl::SecondaryFeasible(float distanceSqrd)
{
	if(weaponHelper.GetType(TTLWeapon::SECONDARY) == WEAPONTYPE_NONE)
		return false;
	return true;
}
//============================================================================
bool AiWeaponControl::MeleeFeasible(float distanceSqrd)
{
	if(weaponHelper.GetType(TTLWeapon::MELEE) == WEAPONTYPE_NONE)
		return false;
	if(distanceSqrd > MELEE_MAX_DIST_SQRD)
		return false;

	CActor* target = NULL;
	if(aiPerception && aiPerception->LastKnownTarget() != INVALID_OBJECT_HANDLE)
		target = CActor::FromHandle(aiPerception->LastKnownTarget());

	//only pull out melee if I am behind you
	if(target)
	{
		int melee_only = true;
		if(weaponHelper.GetType(TTLWeapon::PRIMARY) != WEAPONTYPE_NONE)
			melee_only = false;
		else if(weaponHelper.GetType(TTLWeapon::SECONDARY) != WEAPONTYPE_NONE)
			melee_only = false;	
		
		if(melee_only == false)
		{
			//don't club them if they have hostage
			if(((CCompActor*)target)->GetProperty("hostage")->GetHandle() != INVALID_OBJECT_HANDLE)
				return false;

			//only melee if they are behind target
			Vector3 toTarget = target->GetBodyInWorld() - actor.GetBodyInWorld();
			toTarget.Normalize();
			float dot = target->GetBodyToWorld().GetRow2().Dot(toTarget);
			if(dot < 0.5f)
				return false;
		}
	}
	else
		return false;

	return true;
}
//============================================================================
bool AiWeaponControl::LobbedFeasible(float distanceSqrd)
{
	if(weaponHelper.GetType(TTLWeapon::LOBBED) == WEAPONTYPE_NONE)
		return false;
	if(!weaponHelper.HasAmmo(TTLWeapon::LOBBED))
		weaponHelper.CheatForMoreAmmo(TTLWeapon::LOBBED);
	if(!weaponHelper.HasAmmo(TTLWeapon::LOBBED))
		return false;
	if(distanceSqrd < NADE_MIN_DIST_SQRD || distanceSqrd > NADE_MAX_DIST_SQRD)
		return false;

	Vector3 peak = actor.GetFocusInWorld();
	Vector3 lobbedDir;

	CActor* target = NULL;
	if(aiPerception && aiPerception->LastKnownTarget() != INVALID_OBJECT_HANDLE)
		target = CActor::FromHandle(aiPerception->LastKnownTarget());

	if(target)
		lobbedDir = target->GetFocusInWorld() - actor.GetFocusInWorld();
	else
		lobbedDir = actor.GetBodyToWorld().GetRow2();
	lobbedDir.SafeNormalize();
	lobbedDir *= 2.5f;
	peak += lobbedDir;
	peak.y(peak.y() + 2.0f);

	CLineEntity line;
	line.Line().SetOriginEnd(actor.GetFocusInWorld(),peak);
	line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	line.SetIgnoreEntity(&actor);
	line.SetPositiveInclude(WORLD_CATEGORY | ACTOR_CATEGORY);
	g_collisionSystem.ResetStack();
	CLineQuery test;
	if(test.Test(line))
		return false;

	lobbedDir.y(-0.3f);
	Vector3 trail = peak + lobbedDir;
	line.Line().SetOriginEnd(peak,trail);
	g_collisionSystem.ResetStack();
	if(test.Test(line))
		return false;
	return true;

	//DrawUtility::Line(actor.GetFocusInWorld(),peak,DrawUtility::RED);
	//DrawUtility::Line(peak,trail,DrawUtility::RED);
}
//============================================================================
bool AiWeaponControl::CheatForAmmo()
{
	if(!weaponHelper.HasAmmo(weaponHelper.SelectedWeapon()))
		weaponHelper.CheatForMoreAmmo(weaponHelper.SelectedWeapon());
	return true;
}
//============================================================================
