/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/zoomsight.h"

// Create this
ZoomSightCreator g_zoomSightCreator;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ZoomSightProperties::ZoomSightProperties(
AimSightCreator &creator,
int index) : AimSightProperties(creator, index)
{
	m_zoomLevels = 0;

	m_minFOV = 0.0f;
	m_maxFOV = 0.0f;
	m_filterFOV = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void ZoomSightProperties::RegisterContext(
const char *context)
{
	// Call the parent
	AimSightProperties::RegisterContext(context);

	// Create the context
	g_console.CreateVar(context, "zoomLevels", m_zoomLevels);

	g_console.CreateVar(context, "minFOV", m_minFOV, CONVAR_REAL);
	g_console.CreateVar(context, "maxFOV", m_maxFOV, CONVAR_REAL);
	g_console.CreateVar(context, "filterFOV", m_filterFOV, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ZoomSight::ZoomSight(const ZoomSightProperties &properties) :
AimSight(properties),
m_zoomProps(properties)
{
	m_zoomLevel = 0;
	m_zoomProperty = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Select a zoom level
/////////////////////////////////////////////////////////////////////////////
bool ZoomSight::SelectZoomLevel(
int level)
{
	if (ZoomLevels() == 0)
	{
		if (m_zoomProperty)
			m_zoomProperty->Set(0);
		return false;
	}

	// Set it
	m_zoomLevel = Math::Clamp(level, 0, ZoomLevels());
	if (m_zoomProperty)
		m_zoomProperty->Set(m_zoomLevel);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Next zoom level (returns it)
/////////////////////////////////////////////////////////////////////////////
int ZoomSight::NextZoomLevel()
{
	++m_zoomLevel;
	if (m_zoomLevel > ZoomLevels())
		m_zoomLevel = 0;
	if (m_zoomProperty)
		m_zoomProperty->Set(m_zoomLevel);
	return m_zoomLevel;
}

/////////////////////////////////////////////////////////////////////////////
// What FOV are we at right now?
/////////////////////////////////////////////////////////////////////////////
float ZoomSight::FOV()
{
	// Error, can't tell you this (use default)
	if (m_zoomLevel == 0)
		return -1.0f;

	// Give them the min
	if (ZoomLevels() == 1)
		return m_zoomProps.m_minFOV;

	// lerp
	float t = (float)(m_zoomLevel - 1) / (float)(ZoomLevels() - 1);
	float fov = m_zoomProps.m_minFOV + (m_zoomProps.m_maxFOV - m_zoomProps.m_minFOV) * t;

	return fov;
}
