/////////////////////////////////////////////////////////////////////////////
// Scope class
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/scope.h"
#include "camera/shootercam.h"
#include "weapons/ttlweapon.h"

// Create this
ScopeSightCreator g_scopeSightCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
ScopeSightCreator::ScopeSightCreator() :
AimSightCreator("scopesight")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
AimSight *ScopeSightCreator::CreateInstance(
const VarObjProperties<AimSight> &properties)
{
	return new ScopeSight((const ScopeSightProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
// Subclass this to provide new
/////////////////////////////////////////////////////////////////////////////
AimSightProperties *ScopeSightCreator::NewProperties(
int index)
{
	return new ScopeSightProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
// We are the constructor
/////////////////////////////////////////////////////////////////////////////
ScopeSightProperties::ScopeSightProperties(
AimSightCreator &creator,
int index) : ZoomSightProperties(creator, index)
{
	m_hudOverlayLevel = -1;
	m_focalShakeAmp = 1.0f;
	m_focalShakeFreq = 2.0f;
	m_focalShakeNoise = true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ScopeSightProperties::~ScopeSightProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void ScopeSightProperties::RegisterContext(
const char *context)
{
	// Call the parent
	ZoomSightProperties::RegisterContext(context);

	g_console.CreateVar(context, "hudOverlayLevel", m_hudOverlayLevel);
	g_console.CreateVar(context, "focalShakeAmp", m_focalShakeAmp);
	g_console.CreateVar(context, "focalShakeFreq", m_focalShakeFreq, CONVAR_DEGREES);
	g_console.CreateVar(context, "focalShakeNoise", m_focalShakeNoise);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ScopeSight::ScopeSight(
const ScopeSightProperties &properties) :
ZoomSight(properties),
m_ssProps(properties)
{
	m_lastZoomLevel = 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ScopeSight::~ScopeSight()
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ScopeSight::Activate(Weapon &weapon)
{
	ZoomSight::Activate(weapon);

	m_lastZoomLevel = m_zoomLevel;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ScopeSight::Deactivate(Weapon &weapon)
{
	ZoomSight::Deactivate(weapon);

	if (m_owner)
	{
		GameHUD::SendMessage(GAMEHUD_SCOPEOVERLAY, CMessageDispatcher::IntToData(0), m_owner);
		if (g_shooterCamPtr && g_shooterCam.GetFollow() == CActor::FromHandle(m_owner))
		{
			g_shooterCam.FirstPersonLock(false);
			g_shooterCam.ClearFocalPointShake();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ScopeSight::Update(Weapon &weapon)
{
	CActor *ownerActor = CActor::FromHandle(m_owner);
	if (m_lastZoomLevel != m_zoomLevel && ownerActor)
	{
		bool scopeOn = (m_zoomLevel >= m_ssProps.m_hudOverlayLevel);

		// monkey with the scope
		GameHUD::SendMessage(GAMEHUD_SCOPEOVERLAY, CMessageDispatcher::IntToData(scopeOn), m_owner);

		// Mess with the camera
		if (g_shooterCamPtr && g_shooterCam.GetFollow() == ownerActor)
		{
			g_shooterCam.FirstPersonLock(scopeOn);
			if (scopeOn)
				g_shooterCam.SetFocalPointShake(m_ssProps.m_focalShakeAmp, m_ssProps.m_focalShakeFreq, -1.0f, m_ssProps.m_focalShakeNoise);
			else
				g_shooterCam.ClearFocalPointShake();
		}
	}

	m_lastZoomLevel = m_zoomLevel;
}

