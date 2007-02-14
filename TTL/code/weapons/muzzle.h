/////////////////////////////////////////////////////////////////////////////
// Base class for muzzle flashes
/////////////////////////////////////////////////////////////////////////////
#ifndef MUZZLEFLASH_H
#define MUZZLEFLASH_H

#include "EngineHelper/simpleinst.h"
#include "Components/weapons/weapon.h"

// Forwards
class MuzzleFlash;
class MuzzleFlashCreator;

// The script driven definition of a weapon
class MuzzleFlashProperties : public VarObjProperties<MuzzleFlash>
{
public:
	MuzzleFlashProperties(MuzzleFlashCreator &creator, int index);
	virtual ~MuzzleFlashProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// What is the firing effect sprite
	char 				m_modelDBL[WEAPONSTRING_SIZE];
	float				m_baseAlpha;
	Vector3Packed	m_scaleStart, m_scaleEnd;
	float				m_scaleTimeUp, m_scaleTimeDown;
	bool				m_scalePingPong;
	float				m_minSpin, m_maxSpin;

	// Dynamic light
	Vector3Packed 	m_sourceColor;
	float				m_sourceAttenuation;
	float				m_sourceIntensity;
	float				m_sourceDuration;

	// Particle effect
	char				m_particle[WEAPONSTRING_SIZE];
	float				m_particleNScale;
};

// The actual weapon
class MuzzleFlash
{
public:
	MuzzleFlash(const MuzzleFlashProperties &properties);
	virtual ~MuzzleFlash();

	// Show
	void Start(Vector3CRef pos, const Graphics4x4 &weaponToWorld, bool doLight = true);

	// Hide
	void Hide();

	// Update
	void Update(float deltaT, Vector3CRef pos, const Graphics4x4 &weaponToWorld);

protected:
	// Our baes data
	const MuzzleFlashProperties	&m_properties;

	// Handle muzzleflash
	enum State
	{
		INACTIVE,
		PING,
		PONG
	};

	// Stuff for the model
	SimpleInstance 		*m_model;
	State						m_state;
	Vector3					m_scaleStart;
	Vector3					m_scaleT;
	float						m_age, m_doneAge;
	float						m_spin;

	// Muzzleflash light sources
	CLightRecord			*m_light;
	float						m_lightTTL;
	bool						m_lightVisible;

public:
	// We appear in a list
	MuzzleFlash		*prev, *next;
};

#endif
