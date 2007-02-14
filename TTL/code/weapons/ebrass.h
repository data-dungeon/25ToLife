/////////////////////////////////////////////////////////////////////////////
// Base class for ejecting brass
/////////////////////////////////////////////////////////////////////////////
#ifndef EBRASS_H
#define EBRASS_H

#include "EngineHelper/simpleinst.h"
#include "Components/weapons/weapon.h"
#include "Components/weapons/weapdep.h"

// Forwards
class EjectingBrass;
class EjectingBrassCreator;

// The script driven definition of a weapon
class EjectingBrassProperties : public VarObjProperties<EjectingBrass>
{
public:
	EjectingBrassProperties(EjectingBrassCreator &creator, int index);
	virtual ~EjectingBrassProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// Setup our stuff
	void InitializeEvery();
	void TerminateEvery();

	// our parameters
	char				m_model[WEAPONSTRING_SIZE];
	int				m_instances;
	float				m_ejectSpeedMin, m_ejectSpeedMax;
	Vector3Packed	m_ejectSpinMin, m_ejectSpinMax;
	float				m_ejectCone;
	Vector3Packed	m_shellSize;
	float				m_shellDensity;

	char				m_hitSound[WEAPONSTRING_SIZE];
	float				m_hitSoundDelay;
	float				m_hitSoundGate;

	float				m_ttl, m_fadeTime;

	// Our shizna!
	WidgetFactory::Handle m_brassHandle;
};

// The actual weapon
class EjectingBrass
{
public:
	EjectingBrass(const EjectingBrassProperties &properties);
	virtual ~EjectingBrass();

	// Do it!
	void Eject(Vector3CRef onGround,
					Vector3CRef breech, Vector3CRef ejectDir,
					const DirCos3x3 &orientation, Vector3CRef weaponV);

protected:
	// Our base data
	const EjectingBrassProperties	&m_properties;

	// The next time we can generate a sound
	float	m_nextHitSound;

public:

	// We appear in a list
	EjectingBrass		*prev, *next;
};

#endif
