/////////////////////////////////////////////////////////////////////////////
// Aimsim information
/////////////////////////////////////////////////////////////////////////////
#ifndef AIMING_H
#define AIMING_H

#include "Components/weapons/aimsim.h"
#include "Components/weapons/weaplib.h"
#include "Components/weapons/weapdep.h"

// This is how we do TTL
#define AIM_STANCES_STAND	0
#define AIM_STANCES_CROUCH	1
#define AIM_STANCES			2

class Aiming;
class AimingCreator;

// Our properties
class AimingProperties : public VarObjProperties<Aiming>
{
public:
	// Weee
	AimingProperties(AimingCreator &creator, int index);
	~AimingProperties() {}

	// For parsing the data
	void RegisterContext(const char *context);

	// Post load
	void PostLoadSetup();

	// The aim turning factor
	float				m_aimTurningFactor;

	// The movememnnt factor
	float				m_movementFactor;

	// Controller vibration stuff
	float				m_vibrateDuration;
	float				m_vibrateIntensity;

	// Stuff for the aiming simulation
	WeaponAimProps	m_weaponAim[AIM_STANCES];

	// These must be implemented by your subclass
	const WeaponAimProps *GetWeaponAimProps() const
		{ return m_weaponAim; }
	int GetStances() const
		{ return AIM_STANCES; }
};

// The actual implementation
class Aiming
{
public:
	Aiming(const AimingProperties &properties) : m_properties(properties) {}
	~Aiming() {}


	// The aim turning factor
	float AimTurningFactor() const
		{ return m_properties.m_aimTurningFactor; }

	// The movement factor stuff
	float MovementFactor() const
		{ return m_properties.m_movementFactor; }

	// Vibration support
	float VibrateDuration() const
		{ return m_properties.m_vibrateDuration; }
	float VibrateIntensity() const
		{ return m_properties.m_vibrateIntensity; }

	// The array of aiming info
	const WeaponAimProps *GetWeaponAimProps() const
		{ return m_properties.GetWeaponAimProps(); }
	int GetStances() const
		{ return m_properties.GetStances(); }

protected:
	// The Firearms propers
	const AimingProperties	&m_properties;
};

// The object factory
class AimingCreator : public VarObjCreator<Aiming>
{
public:
	// Hello
	AimingCreator() :
		VarObjCreator<Aiming>("aiming") {}

	// You must provide this for the type
	Aiming *CreateInstance(const VarObjProperties<Aiming> &properties)
		{ return new Aiming((const AimingProperties &)properties); }

protected:
	// Subclass this to provide new
	VarObjProperties<Aiming> *NewProperties(int index)
		{ return new AimingProperties(*this, index); }
};

// The manager of acive Aimings
class AimingLibrary : public VarObjLibrary<Aiming>
{
public:
	// Initialize all the known Aimings
	AimingLibrary() {}
	~AimingLibrary() {}

protected:
	// What is the path
	const char *Path()
		{ return "weapons\\"; }

	// What is the context name
	const char *Context()
		{ return "aiming"; }
};

class AimingDependency : public WeaponDependency
{
public:
	AimingDependency() {}
	
	void InitializeOnce();
	void TerminateOnce();

	WeaponDependency::Phase WantPhase()
		{ return WeaponDependency::PREINIT; }
};

// Aiming singleton
DefineSingleton(AimingLibrary)
#define g_aimingLibrary GetSingleton(AimingLibrary)

#endif
