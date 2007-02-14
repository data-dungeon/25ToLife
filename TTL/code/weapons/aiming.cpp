/////////////////////////////////////////////////////////////////////////////
// Aimsim information
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/aiming.h"

// Our singleton
DeclareSingleton(AimingLibrary);
volatile AimingDependency g_aimingDependency;

// Our array of creation functions
VarObjCreator<Aiming> *VarObjLibrary<Aiming>::m_creatorList = NULL;

// Our only creator right now
AimingCreator g_aimingCreator;

/////////////////////////////////////////////////////////////////////////////
// Weee
/////////////////////////////////////////////////////////////////////////////
AimingProperties::AimingProperties(
AimingCreator &creator,
int index) :
VarObjProperties<Aiming>(creator, index)
{
	m_aimTurningFactor = 1.0f;

	// Assume nothing about movement
	m_movementFactor = 1.0f;

	// Vibration
	m_vibrateDuration = 0.0f;
	m_vibrateIntensity = 0.0f;

	// Do some defaults
	WeaponAimProps &stand = m_weaponAim[0];
	stand.minFireCone = 0.0f;
	stand.minAimFireCone = 0.0f;
	stand.maxFireCone = 3.0f;
	stand.movementImpulseMultiplier = 1.0f;
	stand.turnImpulseMultiplier = 1.0f;
	stand.aimingBonus = 0.8f;
	stand.aimingImpulse = 0.2f;
	stand.recoilImpulse = 0.3f;
	stand.timeConstant = 1.0f;

	WeaponAimProps &crouch = m_weaponAim[1];
	crouch.minFireCone = 0.0f;
	crouch.minAimFireCone = 0.0f;
	crouch.maxFireCone = 2.0f;
	crouch.movementImpulseMultiplier = 1.0f;
	crouch.turnImpulseMultiplier = 1.0f;
	crouch.aimingBonus = 0.8f;
	crouch.aimingImpulse = 0.1f;
	crouch.recoilImpulse = 0.2f;
	crouch.timeConstant = 1.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void AimingProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<Aiming>::RegisterContext(context);

	g_console.CreateVar(context, "aimTurnFactor", m_aimTurningFactor);
	g_console.CreateVar(context, "movement", m_movementFactor);

	g_console.CreateVar(context, "vibrateDuration", m_vibrateDuration);
	g_console.CreateVar(context, "vibrateIntensity", m_vibrateIntensity);

	for (int w = 0; w < AIM_STANCES; w++)
	{
		const char *stanceName[2] = { "stand", "crouch" };
		char var[64];

		sprintf(var, "%sMinFireCone", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].minFireCone);
		sprintf(var, "%sAimMinFireCone", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].minAimFireCone);

		sprintf(var, "%sMaxFireCone", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].maxFireCone);

		sprintf(var, "%sMoveMult", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].movementImpulseMultiplier);
		sprintf(var, "%sTurnMult", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].turnImpulseMultiplier);

		sprintf(var, "%sAimingBonus", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].aimingBonus);

		sprintf(var, "%sAimingImpulse", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].aimingImpulse);

		sprintf(var, "%sRecoilImpulse", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].recoilImpulse);

		sprintf(var, "%stimeConstant", stanceName[w]);
		g_console.CreateVar(context, var, m_weaponAim[w].timeConstant);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Post load
/////////////////////////////////////////////////////////////////////////////
void AimingProperties::PostLoadSetup()
{
	// Call the parent
	VarObjProperties<Aiming>::PostLoadSetup();

	WeaponAimProps &stand = m_weaponAim[0];
	WeaponAimProps &crouch = m_weaponAim[1];

	if (crouch.aimingBonus < 0.0f)
		crouch.aimingBonus = stand.aimingBonus;

	if (crouch.aimingImpulse < 0.0f)
		crouch.aimingImpulse = stand.aimingImpulse;

	if (crouch.recoilImpulse < 0.0f)
		crouch.recoilImpulse = stand.recoilImpulse;

	if (crouch.timeConstant < 0.0f)
		crouch.timeConstant = stand.timeConstant;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AimingDependency::InitializeOnce()
{
	NewSingleton(AimingLibrary);
	g_aimingLibrary.InitializeOnce();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void AimingDependency::TerminateOnce()
{
	g_aimingLibrary.TerminateOnce();
	DeleteSingleton(AimingLibrary);
}
