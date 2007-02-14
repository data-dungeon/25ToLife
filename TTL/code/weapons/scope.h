/////////////////////////////////////////////////////////////////////////////
// Scope implementation
/////////////////////////////////////////////////////////////////////////////
#ifndef SCOPESIGHT_H
#define SCOPESIGHT_H

#include "weapons/zoomsight.h"

class ScopeSight;

class ScopeSightCreator : public AimSightCreator
{
public:
	// Do the work
	ScopeSightCreator();

	// Create
	AimSight *CreateInstance(const VarObjProperties<AimSight> &properties);

protected:
	// Subclass this to provide new
	AimSightProperties *NewProperties(int index);
};

class ScopeSightProperties : public ZoomSightProperties
{
public:
	// We are the constructor
	ScopeSightProperties(AimSightCreator &creator, int index);
	~ScopeSightProperties();

	// For parsing the data
	void RegisterContext(const char *context);

	// Our data
	int		m_hudOverlayLevel;
	float		m_focalShakeAmp;
	float		m_focalShakeFreq;
	bool		m_focalShakeNoise;
};

class ScopeSight : public ZoomSight
{
public:
	ScopeSight(const ScopeSightProperties &properties);
	~ScopeSight();

	void Activate(Weapon &weapon);
	void Deactivate(Weapon &weapon);
	void Update(Weapon &weapon);

private:
	const ScopeSightProperties &m_ssProps;
	int								m_lastZoomLevel;
};

#endif
