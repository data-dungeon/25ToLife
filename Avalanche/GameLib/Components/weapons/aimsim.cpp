/////////////////////////////////////////////////////////////////////////////
// Aiming simulation
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"

#include "cmdcon/cmdcon.h"
#include "Math/sfilter.h"
#include "Math/fastrand.h"
#include "EngineHelper/timer.h"
#include "CollideHelper/LineEntity.h"
#include "CollideHelper/LineQuery.h"
#include "collide/collsys.h"
#include "Components/weapons/aimsim.h"
#include "Components/InteractionMonitor.h"

DeclareSingleton(AimingSimProps);

// Scanning range
float AimingSimulation::s_scanRange = 80.0f;
AimingSimulation::CameraAimType AimingSimulation::s_defaultAimType = AimingSimulation::CAT_TRIANGULATE;

#define DEFAULT_AIMPOINT_FILTER 0.033f

#ifndef CONSUMER_BUILD
bool g_debugAimSim = false;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AimingSimProps::AimingSimProps() :
m_impulseEvent(0, 8)
{
	m_maxV.Set(10.0f, 10.0f, 10.0f);
	m_vImpulse = 0.1f;
	m_vMaxImpulse = 1.0f;
	m_maxA = Math::Pi * 0.5f;
	m_aImpulse = 0.25f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AimingSimProps::~AimingSimProps()
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AimingSimProps::Reset()
{
	m_impulseEvent.Reset();
#ifdef INTERACTIVE_CONSOLE
	g_console.DeleteContext("aimsim");
#endif //INTERACTIVE_CONSOLE
}

/////////////////////////////////////////////////////////////////////////////
// Query the speed stuff
/////////////////////////////////////////////////////////////////////////////
float AimingSimProps::GetVelocityImpulse(
Vector3CRef currentV,
float multiplier) const
{
	float currentSpeed = currentV.Length() * multiplier;
	float maxSpeed = m_maxV.Length();
	float s = Math::Clamp(currentSpeed / maxSpeed);
	return Math::Clamp(m_vImpulse * s);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AimingSimProps::GetAngularImpulse(
float deltaA,
float multiplier) const
{
	float ratio = deltaA / m_maxA;
	return Math::Clamp(m_aImpulse * ratio * multiplier);
}

/////////////////////////////////////////////////////////////////////////////
// Add an inpulse event
/////////////////////////////////////////////////////////////////////////////
void AimingSimProps::AddEvent(
const char *eventName,
float impulse,
ImpulseType type)
{
	ImpulseEvent event;
	strncpy(event.name, eventName, sizeof(event.name) - 1);
	event.name[sizeof(event.name) - 1] = '\0';
	event.impulse = impulse;
	event.type = type;

	m_impulseEvent.Add(event);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const AimingSimProps::ImpulseEvent *AimingSimProps::FindEvent(
const char *lookFor) const
{
	int count = m_impulseEvent.Count();
	const ImpulseEvent *list = m_impulseEvent.GetPtr();
	for (int i = 0; i < count; i++)
	{
		if (strcmp(lookFor, list[i].name) == 0)
			return &list[i];
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Load the stuff from the var (also looks up your events!)
/////////////////////////////////////////////////////////////////////////////
bool AimingSimProps::LoadScript(
const char *script)
{
	// Create & execute the context
	const char *context = "aimsim";

	g_console.CreateVar(context, "maxV", &m_maxV);
	g_console.CreateVar(context, "vImpulse", m_vImpulse, CONVAR_REAL);
	g_console.CreateVar(context, "vMaxImpulse", m_vMaxImpulse, CONVAR_REAL);
	g_console.CreateVar(context, "aImpulse", m_aImpulse, CONVAR_REAL);
	g_console.CreateVar(context, "maxA", m_maxA, CONVAR_REAL);

	// Do the list (this is cool)
	int count = m_impulseEvent.Count();
	for (int i = 0; i < count; i++)
	{
		ImpulseEvent &event = m_impulseEvent[i];
		char var[64];

		sprintf(var, "%sImpulse", event.name);
		g_console.CreateVar(context, var, event.impulse);
		sprintf(var, "%sType", event.name);
		g_console.CreateVar(context, var, (int&)event.type);
	}

	// do it!
	g_console.BindScript(context, script);
	g_console.ExecuteContext(context);

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(context);
#endif //INTERACTIVE_CONSOLE

	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AimingSimulation::AimingSimulation(
const AimingSimProps &aimProps) :
m_sim(aimProps)
{
	m_owner = (ActorHandle)INVALID_OBJECT_HANDLE;

	m_weapon = NULL;

	m_penetrateCB = NULL;
	m_penetrateCBContext = NULL;

	m_maxStances = 0;

	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// Set the weapon info
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::SetWeapon(
const WeaponAimProps *weaponProps,
int stances)
{
	m_weapon = weaponProps;
	m_maxStances = stances;
}

/////////////////////////////////////////////////////////////////////////////
// Reset everything
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::Reset()
{
	InvalidateAim();
	m_aimStyle = AIM_USEGUN;
	m_hitPoint.Clear();
	m_aimPoint.Clear();
	m_hitActor = (ActorHandle)INVALID_OBJECT_HANDLE;

	m_hitRange = s_scanRange;

	m_stance = 0;
	m_aiming = false;

	m_error = 0.0f;
	m_vError = 0.0f;

	m_lastUpdate = 0.0f;
	m_lastDir.Clear();
	m_onUpdate = 0;

	m_wantFilteredAimPoint = false;
	m_haveFilteredAimPoint = false;
	m_filteredAimPoint.Clear();

	RestoreAimType();

	// Use the reference bone
	m_referenceBoneID = INVALID_BONEID;
}

/////////////////////////////////////////////////////////////////////////////
// The weapon should update this frequently
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::SetMuzzle(
const Vector3 &muzzleEnd,
const Vector3 &muzzleDir)
{
	m_muzzleEnd = muzzleEnd;
	m_muzzleDir = muzzleDir;
	InvalidateAim();
}

/////////////////////////////////////////////////////////////////////////////
// What are we aiming at?
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::Aim(
const Vector3 &eye,
const Vector3 &dir)
{
	m_aimStyle = AIM_LOOKAT;
	// stupid, stupid, stupid but necessary check.  Somewhere there's a numeric instability
	// entering the aiming simulation.  Rather than try to spend all the necessary time trying
	// to track it down, just make sure we don't propogate very small changes through the system.
	bool invalidate = false;
	float len = (m_eye - eye).LengthSquared();
	if( len > 0.00001f )
	{
		m_eye = eye;
		invalidate = true;
	}

	len = (m_dir - dir).LengthSquared();
	if( len > 0.00001f )
	{
		m_dir = dir;
		invalidate = true;
	}
	if( invalidate )
		InvalidateAim();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::Aim(
const Vector3 &target,
float deviance)
{
	m_aimStyle = AIM_ATPOINT;
	m_deviance = deviance;
	m_target = target;
	InvalidateAim();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::Aim()
{
	if (m_aimStyle != AIM_USEGUN)
	{
		m_aimStyle = AIM_USEGUN;
		InvalidateAim();
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Vector3 &AimingSimulation::AimOrigin()
{
	ComputeAim();
	return m_aimOrigin;
}

/////////////////////////////////////////////////////////////////////////////
// Call this just before you launch a projectile
/////////////////////////////////////////////////////////////////////////////
Vector3 AimingSimulation::AimDir()
{
	ComputeAim();
	if (!Ready())
		return m_aimDir;

	float minFireCone = Math::Max(MinFireCone(), 0.0f);
	float maxFireCone = Math::Max(MaxFireCone(), 0.0f);
	float angle = minFireCone + (maxFireCone - minFireCone) * GetError();
#if 0
	return g_random.InCone(m_aimDir, Math::Deg2Rad(angle));
#else
	// Make a random vector
	const Vector3 perp = MakeOrthogonal(m_aimDir);
	Vector3 spray = m_aimDir;
	const float randomAngle = g_random.InRange(0.0f, Math::Deg2Rad(angle));
	spray = RotatePointAroundAxis(spray, perp, randomAngle);
	const float rotVariance = g_random.InRange(-Math::Pi, Math::Pi);
	spray = RotatePointAroundAxis(spray, m_aimDir, rotVariance);
	return spray;
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Vector3 &AimingSimulation::AimPoint()
{
	ComputeAim();
	return m_aimPoint;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Vector3 &AimingSimulation::FilteredAimPoint()
{
	m_wantFilteredAimPoint = true;
	if (!m_haveFilteredAimPoint)
		return AimPoint();
	else
		return m_filteredAimPoint;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Vector3 &AimingSimulation::TrueAimDir()
{
	ComputeAim();
	return m_aimDir;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const Vector3 &AimingSimulation::TargetPoint()
{
	ComputeTarget();
	return m_hitPoint;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CActor *AimingSimulation::TargetActor()
{
	ComputeTarget();
	return CActor::FromHandle(m_hitActor);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AimingSimulation::TargetRange()
{
	ComputeTarget();
	return m_hitRange;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 AimingSimulation::TargetDir()
{
	ComputeTarget();
	Vector3 dir = m_hitPoint - m_muzzleEnd;
	float length = dir.Length();
	if (Math::Zero(length))
		return m_muzzleDir;
	else
		return (dir / length);
}

/////////////////////////////////////////////////////////////////////////////
// The material (-1 mean no material)
/////////////////////////////////////////////////////////////////////////////
int AimingSimulation::TargetMaterial()
{
	ComputeTarget();
	if (m_hitMaterials == 0)
		return -1;
	else
		return m_hitMaterial[m_hitMaterials - 1];
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int *AimingSimulation::TargetMaterials(
int &count)
{
	ComputeTarget();
	count = m_hitMaterials;
	if (m_hitMaterials == 0)
		return NULL;
	else
		return m_hitMaterial;
}

/////////////////////////////////////////////////////////////////////////////
// What is the point on the bullet trajectory at d
/////////////////////////////////////////////////////////////////////////////
Vector3 AimingSimulation::PointOnTrajectory(
float d)
{
	// Compute the aim
	ComputeAim();

	if (m_aimStyle == AIM_LOOKAT && m_aimType == CAT_SIGHTLINE)
		return m_aimOrigin + m_aimDir * Math::Sqrt(Math::Square(d) + Math::Square((m_aimOrigin - m_muzzleEnd).Length()));
	else
		return m_aimOrigin + m_aimDir * d;
}

/////////////////////////////////////////////////////////////////////////////
// What stance are we in?
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::SetStance(
int newStance)
{
	ASSERT(newStance >= 0 && newStance < m_maxStances);

	if (newStance >= 0 && newStance < m_maxStances)
		m_stance = newStance;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::SetAiming(
bool aiming)
{
	// bail out
	if (!Ready())
		return;

	if (m_aiming != aiming)
		AddImpulse(GetProps().aimingImpulse);
	m_aiming = aiming;
}

/////////////////////////////////////////////////////////////////////////////
// Weapon was fired (add in recoil!)
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::Recoil()
{
	// bail out
	if (!Ready())
		return;

	// Add the impulse
	AddImpulse(GetProps().recoilImpulse);
}

/////////////////////////////////////////////////////////////////////////////
// Trigger an event
/////////////////////////////////////////////////////////////////////////////
bool AimingSimulation::TriggerEvent(
const char *event,
float strength)
{
	const AimingSimProps::ImpulseEvent *findEvent = m_sim.FindEvent(event);
	ASSERT(findEvent);
	if (!findEvent)
		return false;

	// put it into the pot
	AddImpulse(findEvent->impulse * strength, findEvent->type);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// call this to update (only once per frame please)
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::Update()
{
	const float deltaT = g_timer.GetEndSec() - m_lastUpdate;
	if (Math::Zero(deltaT))
		return;

	// Play with the TC
	float tc = GetProps().timeConstant;
	if (m_aiming)
		tc *= GetProps().aimingBonus;

	// Get the current direction
	Vector3 currentDir = RawAimDir();

	// Figure out the contribution from velocity
	CActor *owner = CActor::FromHandle(m_owner);
	ASSERT(owner);
	if (owner)
		m_vError += m_sim.GetVelocityImpulse(owner->GetVelocityInWorld(), GetProps().movementImpulseMultiplier) * deltaT;
	float deltaAngle = 0.0f;
	if (m_onUpdate > 0)
	{
		deltaAngle = ComputeAngle(currentDir, m_lastDir);
		m_vError += m_sim.GetAngularImpulse(deltaAngle, GetProps().turnImpulseMultiplier) * deltaT;
	}

	// Allow the additional error to fade to zero
	ASSERT(!Math::IsNaN(m_error));
	m_vError = Math::Clamp(m_vError, 0.0f, m_sim.GetMaxVelocityImpulse());
#if 1
	float frameTime = deltaT;
	static float updateTime = 0.015f;
	do {
		float dt = Math::Min(frameTime, updateTime);
		m_vError = FOFilterImplicit(m_vError, 0.0f, dt, tc);
		m_error = FOFilterImplicit(m_error, 0.0f, dt, tc);
		frameTime -= dt;
	} while (!Math::Zero(frameTime));
#else
	if (Math::Zero(tc))
		m_error = m_vError = 0.0f;
	else
	{
		Math::MoveToZero(&m_error, deltaT * (tc * 4.0f));
		Math::MoveToZero(&m_vError, deltaT * (tc * 4.0f));
	}
#endif
	ASSERT(!Math::IsNaN(m_error));

	// Do the aim point filtering
	if (m_wantFilteredAimPoint)
	{
		ComputeAim();
		if (m_haveFilteredAimPoint)
			m_filteredAimPoint = FOFilterImplicit(m_filteredAimPoint, m_aimPoint, deltaT, DEFAULT_AIMPOINT_FILTER);
		else
		{
			m_filteredAimPoint = m_aimPoint;
			m_haveFilteredAimPoint = true;
		}
	}

#if 0
	if ((m_onUpdate % 10) == 0)
	{
		TRACE("%x: (dt %0.2f) ActorSpeed: %.1f Error %0.2f vError %0.2f dA %0.2f tc %0.2f update %d\n", this, deltaT, owner->GetVelocityInWorld().Length(), m_error, m_vError, Math::Rad2Deg(deltaAngle), tc, m_onUpdate);
	}
#endif

	// One more update
	m_lastDir = currentDir;
	++m_onUpdate;
	m_lastUpdate = g_timer.GetEndSec();
}

/////////////////////////////////////////////////////////////////////////////
// Get the sizes for the stuff
/////////////////////////////////////////////////////////////////////////////
float AimingSimulation::GetMaxSpread()
{
	// bail out
	if (!Ready())
		return 0.0f;

	return MaxFireCone();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AimingSimulation::GetMinSpread()
{
	// bail out
	if (!Ready())
		return 0.0f;

	return MinFireCone();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Vector3 AimingSimulation::RawAimDir()
{
	if (m_aimStyle == AIM_LOOKAT)
		return m_dir;
	else
	{
		ComputeAim();
		return m_aimDir;
	}
}

/////////////////////////////////////////////////////////////////////////////
// What actor are we aiming at
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::ComputeAim()
{
	// Already up to date?
	if (m_aimValid)
		return;
	m_aimValid = true;

	// We always fire from the muzzle
	const float muzzleBackup = 0.5f;

	// Assume we are on no actor
	m_hitActor = (ActorHandle)INVALID_OBJECT_HANDLE;
	m_hitRange = s_scanRange;

	InteractionMonitor *interact = NULL;
	CCompActor *owner = CCompActor::FromHandle(m_owner);
	if (owner)
	{
		ASSERT( owner->IsA( CCompActor::ClassType() ) );
		interact = (InteractionMonitor *)owner->GetComponentByName( "InteractionMonitor" );
		if( interact )
			interact->ClearViewCursorEntity();
	}

	// Special case, just using the gun
	if (m_aimStyle == AIM_USEGUN)
	{
		m_aimOrigin = m_muzzleEnd;
		m_aimDir = m_muzzleDir;
		m_aimPoint = m_aimOrigin + m_aimDir * s_scanRange;
	}
	else if (m_aimStyle == AIM_ATPOINT)
	{
		m_aimOrigin = m_muzzleEnd - m_muzzleDir * muzzleBackup;
		m_aimDir = (m_target - m_aimOrigin);
		float range = m_aimDir.Length();
		if (!Math::Zero(range))
			m_aimDir /= range;
		else
			m_aimDir = m_muzzleDir;
		if (!Math::Zero(m_deviance))
			m_aimDir = g_random.InCone(m_aimDir, m_deviance);
		m_aimPoint = m_aimOrigin + m_aimDir * range;
	}
	else if (m_aimStyle == AIM_LOOKAT && m_aimType == CAT_SIGHTLINE)
	{
		m_aimOrigin = m_eye;
		m_aimDir = m_dir;
		m_aimPoint = m_aimOrigin + m_aimDir * s_scanRange;
	}
	else if (m_aimStyle == AIM_LOOKAT && m_aimType == CAT_TRIANGULATE)
	{
		if (owner && m_referenceBoneID == INVALID_BONEID)
		{
			int foundBone = boneGetBoneID(owner->Instance()->Body(), &m_referenceBoneID, (char *)"head");
			ASSERT(foundBone != 0);
		}

		if (!owner || m_referenceBoneID == INVALID_BONEID)
			m_aimOrigin = m_muzzleEnd - m_muzzleDir * muzzleBackup;
		else
		{
			Vector3 planeP;
			boneGetStartPosition(owner->Instance()->Body(), m_referenceBoneID, &planeP);
			planeP = planeP * owner->Instance()->Matrix();
			const Vector3 planeN = m_dir;

			Ray ray(m_muzzleEnd, -m_muzzleDir);
			Vector3 intersection;
			bool ret = ray.IntersectWithPlane(planeP, planeN, intersection);
			const float maxD = Math::Square(0.25f);
			const float d = (planeP - intersection).LengthSquared();
			if (!ret || d > maxD)
				m_aimOrigin = planeP + planeN * 0.15f;
			else
			{
				m_aimOrigin = intersection + m_muzzleDir * 0.25f;
#ifndef CONSUMER_BUILD
				if (g_debugAimSim)
				{
					DrawUtility::Sphere(intersection, DrawUtility::BLUE, 0.05f);
					DrawUtility::Line(intersection, m_aimOrigin, DrawUtility::BLUE);
					DrawUtility::NormalPlane(planeP, planeN, DrawUtility::MAGENTA, DrawUtility::MAGENTA);
				}
#endif
			}
		}

		// Adjust the eye
		Vector3 eyeToMuzzle = m_aimOrigin - m_eye;
		const float eyeTweak = Vector3::Dot(eyeToMuzzle, m_dir);
		Vector3 startScan = m_eye + m_dir * eyeTweak;

		// compute what we are using for the aim
		bool done = false;
		int depth = 0;
		float range = s_scanRange;
		m_hitMaterials = 0;
		do {
			CLineEntity line;
			line.Line().SetOriginBasis(startScan, m_dir);
			line.Line().SetMaxBoundary(range);
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
			line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			line.SetIgnoreEntity(owner);

			// Now cast it
			g_collisionSystem.ResetStack();
			CLineQuery test;
			if (!test.Test(line, CStageEntity::USE_COMPLEX))
			{
				m_aimPoint = m_eye + m_dir * range;	// Total miss, just use the range as the hitpoint
				done = true;
			}
			else
			{
				// Save this!
				if (m_hitMaterials < MAX_AIMSIM_MATERIALS)
					m_hitMaterial[m_hitMaterials++] = test.material;

				// let the interaction monitor know that our query hit another entity
				if( interact )
					interact->ViewCursorEntity( test.target, test.material );
				if (m_penetrateCB && m_penetrateCB(m_penetrateCBContext, test.material, depth++))
				{
					const float tweak = 0.005f;
					startScan = test.intersection + m_dir * tweak;
					range -= test.d;
				}
				else
				{
					// Save the results
					m_aimPoint = test.intersection;
					done = true;
				}
			}
		} while (!done);

		// now setup the real dir
		m_aimDir = (m_aimPoint - m_aimOrigin).Normalized();

#ifndef CONSUMER_BUILD
		if (g_debugAimSim)
		{
			DrawUtility::Normal(m_muzzleEnd, m_muzzleDir, DrawUtility::GREEN, 1.0f);
			DrawUtility::Line(startScan, startScan + m_dir * s_scanRange, DrawUtility::MAGENTA);
			DrawUtility::Sphere(m_aimOrigin, DrawUtility::RED, 0.1f);
			DrawUtility::Sphere(m_aimPoint, DrawUtility::YELLOW, 0.25f);
			DrawUtility::Line(m_aimOrigin, m_aimPoint, DrawUtility::YELLOW);
		}
#endif
	}
	else
	{
		ASSERT(false);
	}
}

/////////////////////////////////////////////////////////////////////////////
// What actor are we aiming at
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::ComputeTarget()
{
	// Already up to date?
	if (m_targetValid)
		return;

	// Compute the aim
	ComputeAim();

	// Did they validate the target?
	if (!m_targetValid)
	{
		// compute what we are using for the aim
		bool done = false;
		int depth = 0;
		float range = s_scanRange;
		m_hitMaterials = 0;
		Vector3 startScan = m_aimOrigin;
		do {
			CLineEntity line;
			line.Line().SetOriginBasis(startScan, m_aimDir);
			line.Line().SetMaxBoundary(range);
			line.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
			line.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			line.SetIgnoreEntity(CActor::FromHandle(m_owner));

			// Now cast it
			g_collisionSystem.ResetStack();
			CLineQuery test;
			if (!test.Test(line, CStageEntity::USE_COMPLEX))
			{
				m_hitPoint = startScan +  m_aimDir * range;	// Total miss, just use the range as the hitpoint
				m_hitActor = (ActorHandle)INVALID_OBJECT_HANDLE;
				m_hitRange = range;
				done = true;
			}
			else
			{
				// Save this!
				if (m_hitMaterials < MAX_AIMSIM_MATERIALS)
					m_hitMaterial[m_hitMaterials++] = test.material;

				if (m_aimStyle == AIM_LOOKAT && m_penetrateCB && m_penetrateCB(m_penetrateCBContext, test.material, depth++))
				{
					const float tweak = 0.005f;
					startScan = test.intersection + m_aimDir * tweak;
					range -= test.d;
				}
				else
				{
					// Save the results
					m_hitPoint = test.intersection;
					m_hitActor = test.actor ? test.actor->GetHandle() : (ActorHandle)INVALID_OBJECT_HANDLE;
					m_hitRange = test.d;
					done = true;
				}
			}
		} while (!done);

		m_targetValid = true;
	}

#ifndef CONSUMER_BUILD
	if (g_debugAimSim)
	{
		DrawUtility::Sphere(m_hitPoint, DrawUtility::CYAN, 0.25f);
		DrawUtility::Line(m_aimOrigin, m_hitPoint, DrawUtility::CYAN);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
inline float ArcLengthTrig(
const Vector3 &currentDir,
const Vector3 &lastDir)
{
	float dot = Vector3::Dot(lastDir, currentDir);
	if (dot <= -1.0f || dot >= 1.0f)
		return 0.0f;
	else
		return Math::Abs(Math::ArcCos(dot));
}

inline float TrueArcLength(
const Vector3 &currentDir,
const Vector3 &lastDir)
{
	return (lastDir - currentDir).Length();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AimingSimulation::ComputeAngle(
const Vector3 &currentDir,
const Vector3 &lastDir)
{
#if 1
	return TrueArcLength(currentDir, lastDir);
#else
	float d1 = ArcLengthTrig(currentDir, lastDir);
	float d2 = TrueArcLength(currentDir, lastDir);

	return d2;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Add in an impulse
/////////////////////////////////////////////////////////////////////////////
void AimingSimulation::AddImpulse(
float impulse,
AimingSimProps::ImpulseType type)
{
	ASSERT(!Math::IsNaN(impulse));
	switch (type)
	{
		case AimingSimProps::IMPULSE_ADD:
			m_error += impulse;
			break;

		case AimingSimProps::IMPULSE_REPLACE:
			m_error = impulse;
			break;
	}

	m_error = Math::Clamp(m_error);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AimingSimulation::MinFireCone()
{
	if (m_aiming)
		return GetProps().minAimFireCone;
	else
		return GetProps().minFireCone;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float AimingSimulation::MaxFireCone()
{
	float cone = GetProps().maxFireCone;
#if 0
	if (m_aiming)
		cone *= GetProps().aimingBonus;
#endif
	return cone;
}

