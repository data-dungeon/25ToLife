/////////////////////////////////////////////////////////////////////////////
// The aim sight is the sighting mechanism for the gun
/////////////////////////////////////////////////////////////////////////////
#ifndef AIMSIGHT_H
#define AIMSIGHT_H

#include "Components/weapons/volib.h"

class AimSightCreator;
class AimSight;
class Weapon;

class AimSightProperties : public VarObjProperties<AimSight>
{
public:
	// We are the constructor
	AimSightProperties(AimSightCreator &creator, int index);
	virtual ~AimSightProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);
};

class AimSight
{
public:
	AimSight(const AimSightProperties &properties);
	virtual ~AimSight() {}

	// What is our name?
	const char *Name();

	// What is our class
	const char *Class()
		{ return m_properties.GetClass(); }

	// Set the owner
	virtual void SetOwner(CActor *owner)
		{ m_owner = CActor::ToHandle(owner); }
	CActor *GetOwner()
		{ return CActor::FromHandle(m_owner); }

	// Set the weapon we are attached to
	virtual void SetWeapon(Weapon *weapon)
		{ m_weapon = weapon; }

	// Handle activation/deactivation
	virtual void Reset() {}
	virtual void Activate(Weapon &weapon) {}
	virtual void Deactivate(Weapon &weapon) {}
	virtual void Update(Weapon &weapon) {}

protected:
	const AimSightProperties	&m_properties;
	ActorHandle						m_owner;
	Weapon							*m_weapon;
};

#endif
