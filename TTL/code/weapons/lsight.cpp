/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a ZoomSight
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/lsight.h"
#include "Interfaces/ViewIF.h"
#include "camera/shootercam.h"
#include "Math/sfilter.h"

// Create this
LaserSightCreator g_LaserSightCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
LaserSightCreator::LaserSightCreator() :
AimSightCreator("lasersight")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
AimSight *LaserSightCreator::CreateInstance(
const VarObjProperties<AimSight> &properties)
{
	return new LaserSight((const LaserSightProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AimSightProperties *LaserSightCreator::NewProperties(
int index)
{
	return new LaserSightProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LaserSightProperties::LaserSightProperties(
AimSightCreator &creator,
int index) : ZoomSightProperties(creator, index)
{
	m_maxAngle = Math::Deg2Rad(20.0f);
	m_maxChange = Math::Square(0.05f);
	m_filterAngle = 0.05f;

	m_laser = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LaserSightProperties::~LaserSightProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void LaserSightProperties::RegisterContext(
const char *context)
{
	// Call the parent
	ZoomSightProperties::RegisterContext(context);

	// Load the tracer
	g_console.CreateVar(context, "laserColorStart", &m_laserColorStart);
	g_console.CreateVar(context, "laserColorEnd", &m_laserColorEnd);
	g_console.CreateVar(context, "laserColorMiss", &m_laserColorMiss);
	g_console.CreateVar(context, "laserColorDot", &m_laserColorDot);
	g_console.CreateVar(context, "laserRayRadius", m_laserRayRadius, CONVAR_REAL);
	g_console.CreateVar(context, "laserDotRadius", m_laserDotRadius, CONVAR_REAL);
	g_console.CreateVar(context, "laserRange", m_laserRange, CONVAR_REAL);
	g_console.CreateVar(context, "laserAimInflate", m_laserAimInflate, CONVAR_REAL);

	g_console.CreateVar(context, "matchAimMaxAngle", m_maxAngle, CONVAR_DEGREES);
	g_console.CreateVar(context, "matchAimMaxChange", m_maxChange, CONVAR_REAL);
	g_console.CreateVar(context, "matchAimFilter", m_filterAngle, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
// the per-loop stuff
/////////////////////////////////////////////////////////////////////////////
void LaserSightProperties::InitializeEvery()
{
	LaserRenderParameters params;

	params.rayStartRadius = m_laserRayRadius;
	params.rayEndRadius = m_laserRayRadius;
	params.dotRadius = m_laserDotRadius;

	SetColor(m_laserColorStart, params.colorStart);
	SetColor(m_laserColorEnd, params.colorEnd);
	SetColor(m_laserColorDot, params.colorDot);
	SetColor(m_laserColorMiss, params.colorMiss);

	params.maxRange = m_laserRange;

	// Make it!
	m_laser = new LaserRender("weapons\\laser.dbl", params);
}

/////////////////////////////////////////////////////////////////////////////
// the per-loop stuff
/////////////////////////////////////////////////////////////////////////////
void LaserSightProperties::TerminateEvery()
{
	if (m_laser)
	{
		delete m_laser;
		m_laser = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LaserSight::LaserSight(
const LaserSightProperties &properties) :
ZoomSight(properties),
m_lsProps(properties)
{
	if (m_lsProps.m_laser)
		m_laserHandle = m_lsProps.m_laser->CreateInstance();

	m_laserDirValid = false;
	m_laserDir.Clear();
	m_lastPos.Clear();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LaserSight::~LaserSight()
{
	if (m_lsProps.m_laser && m_laserHandle)
		m_lsProps.m_laser->DestroyInstance(m_laserHandle);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LaserSight::Activate(Weapon &weapon)
{
	ZoomSight::Activate(weapon);

	CActor *m_shooter = m_weapon->GetOwner();
	if (!m_shooter || m_shooter->IsNetMaster())
		m_lsProps.m_laser->SetRangeFactor(1.0f);
	else
		m_lsProps.m_laser->SetRangeFactor(0.5f);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LaserSight::Deactivate(Weapon &weapon)
{
	ZoomSight::Deactivate(weapon);

	if (m_lsProps.m_laser && m_laserHandle)
		m_lsProps.m_laser->Hide(m_laserHandle);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void LaserSight::Update(Weapon &weapon)
{
	ZoomSight::Update(weapon);

	if (!m_laserHandle)
		return;

	// Are we hidden, then hide
	if (m_weapon->IsHidden())
	{
		m_lsProps.m_laser->Hide(m_laserHandle);
		ClearLaserDir();
		return;
	}

	// get the position of the sight
	Vector3 sight = m_weapon->GetBoneInWorld(0, BONE_SIGHT);
	Vector3 sightDir = m_weapon->GetBoneDir(0, BONE_SIGHT);
	CActor *m_shooter = m_weapon->GetOwner();
	bool isPlayer = g_shooterCamPtr && g_shooterCam.Active() && (g_shooterCam.GetFollow() == m_shooter);

	// Not the player, just come out of the gun
	if (!isPlayer)
	{
		m_lsProps.m_laser->Update(m_laserHandle, m_shooter, sight, sight, sightDir);
		SetLaserDir(sightDir);
		return;
	}

	// The player is real complicated
	if (ZoomLevel() == 0)
	{
		m_lsProps.m_laser->Hide(m_laserHandle);
		ClearLaserDir();
	}
	else
	{
		AimingSimulation &aimSim = m_weapon->GetAimSim();
		const Vector3 &muzzle = aimSim.GetMuzzleEnd();
		Vector3 aim = aimSim.TargetDir();
		if (!m_laserDirValid)
		{
			m_lsProps.m_laser->Update(m_laserHandle, m_shooter, sight, sight, sightDir, true, m_lsProps.m_laserAimInflate, true);
			SetLaserDir(sightDir);
		}
		else
		{
			const float angle = Math::ArcCos(Vector3::Dot(sightDir, aim));
			float deltaPos = (m_lastPos - muzzle).LengthSquared();
			if (deltaPos > m_lsProps.m_maxChange || angle > m_lsProps.m_maxAngle)
			{
				m_lsProps.m_laser->Update(m_laserHandle, m_shooter, sight, sight, sightDir, true, m_lsProps.m_laserAimInflate, true);
				SetLaserDir(sightDir);
			}
			else
			{
				// Interpolate
				Vector3 smoothDir = FOFilterImplicit(m_laserDir, aim, g_timer.GetFrameSec(), m_lsProps.m_filterAngle);
				smoothDir.Normalize();
				m_lsProps.m_laser->Update(m_laserHandle, m_shooter, sight, muzzle, smoothDir, false, m_lsProps.m_laserAimInflate);
				SetLaserDir(smoothDir);
			}
		}

		// Save this for next time
		m_lastPos = muzzle;
	}
}

