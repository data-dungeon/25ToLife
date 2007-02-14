/////////////////////////////////////////////////////////////////////////////
// An AimSight that supports FOV
/////////////////////////////////////////////////////////////////////////////
#ifndef ZOOMSIGHT_H
#define ZOOMSIGHT_H

#include "Components/weapons/sightlib.h"

class Property;
class ZoomSightCreator;

class ZoomSightProperties : public AimSightProperties
{
public:
	// We are the constructor
	ZoomSightProperties(AimSightCreator &creator, int index);
	virtual ~ZoomSightProperties() {}

	// For parsing the data
	virtual void RegisterContext(const char *context);

	// How many zoom levels does this sight support
	int 	m_zoomLevels;

	// between min and max
	// if there are more than 2 zoomlevels, linearly distribute the extra levels
	float m_minFOV;
	float m_maxFOV;
	float m_filterFOV;
};

class ZoomSight : public AimSight
{
public:
	ZoomSight(const ZoomSightProperties &properties);
	virtual ~ZoomSight() {}

	// Handle activation/deactivation
	virtual void Reset()
		{ SelectZoomLevel(0); }
	virtual void Activate(Weapon &weapon)
		{ SelectZoomLevel(0); }

	// Pass in a property to hold the zoom level.
	void SetZoomProperty(Property *zoomProperty)
		{ m_zoomProperty = zoomProperty; }

	// What zoom level are we currently at
	virtual int ZoomLevel()
		{ return m_zoomLevel; }

	// Zero means use default Zoom
	virtual int ZoomLevels()
		{	return m_zoomProps.m_zoomLevels; }

	// Select a zoom level 0 - ZoomLevels INCLUSIVE
	virtual bool SelectZoomLevel(int level);

	// Next zoom level (returns it)
	int NextZoomLevel();

	// What Zoom are we at right now?
	virtual float FOV();

	// What is the Zoom filter value
	float FOVFilter()
		{ return m_zoomProps.m_filterFOV; }

	float MinFOV()
		{ return Math::Min(m_zoomProps.m_minFOV, m_zoomProps.m_maxFOV); }
	float MaxFOV()
		{ return 1.0f; }

protected:
	const ZoomSightProperties	&m_zoomProps;

	int								m_zoomLevel;
	Property							*m_zoomProperty;
};

class ZoomSightCreator : public AimSightCreator
{
public:
	// Do the work
	ZoomSightCreator() : AimSightCreator("zoomsight") {}

	// Create
	AimSight *CreateInstance(const VarObjProperties<AimSight> &properties)
		{ return new ZoomSight((const ZoomSightProperties &)properties); }

protected:
	// Subclass this to provide new
	AimSightProperties *NewProperties(int index)
		{ return new ZoomSightProperties(*this, index); }
};

#endif
