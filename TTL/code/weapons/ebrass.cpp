/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Game/Database/lightset.h"
#include "EngineHelper/pathfile.h"
#include "Effects/widget.h"
#include "weapons/ebrassLib.h"
#include "weapons/ebrass.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EjectingBrassProperties::EjectingBrassProperties(
EjectingBrassCreator &creator,
int index) : VarObjProperties<EjectingBrass>(creator, index)
{
	m_model[0] = '\0';
	m_instances = 0;
	m_ejectSpeedMin = 1.0f;
	m_ejectSpeedMax = 1.0f;
	m_ejectSpinMin.Clear();
	m_ejectSpinMax.Clear();
	m_ejectCone = 0.0f;
	m_shellSize.Clear();
	m_shellDensity = 1.0f;
	m_ttl = 0.0f;
	m_fadeTime = 0.0f;

	m_hitSound[0] = '\0';
	m_hitSoundDelay = 0.8f;
	m_hitSoundGate = 0.8f;

	m_brassHandle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void EjectingBrassProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<EjectingBrass>::RegisterContext(context);

	g_console.CreateVar(context, "model", m_model, sizeof(m_model) - 1);
	g_console.CreateVar(context, "instances", m_instances);

	g_console.CreateVar(context, "ejectSpeedMin", m_ejectSpeedMin, CONVAR_REAL);
	g_console.CreateVar(context, "ejectSpeedMax", m_ejectSpeedMax, CONVAR_REAL);
	g_console.CreateVar(context, "ejectSpinMin", &m_ejectSpinMin);
	g_console.CreateVar(context, "ejectSpinMax", &m_ejectSpinMax);
	g_console.CreateVar(context, "ejectCone", m_ejectCone, CONVAR_DEGREES);

	g_console.CreateVar(context, "shellSize", &m_shellSize);
	g_console.CreateVar(context, "shellDensity", m_shellDensity, CONVAR_REAL);

	g_console.CreateVar(context, "ttl", m_ttl, CONVAR_REAL);
	g_console.CreateVar(context, "fadeTime", m_fadeTime, CONVAR_REAL);

	g_console.CreateVar(context, "hitSound", m_hitSound, sizeof(m_hitSound) - 1);
	g_console.CreateVar(context, "hitSoundDelay", m_hitSoundDelay, CONVAR_REAL);
	g_console.CreateVar(context, "hitSoundGate", m_hitSoundGate, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
// Setup our stuff
/////////////////////////////////////////////////////////////////////////////
void EjectingBrassProperties::InitializeEvery()
{
	// Call the parent
	VarObjProperties<EjectingBrass>::InitializeEvery();

	// Find the widget for brass
	if (m_model[0] == '\0' || m_instances == 0)
		m_brassHandle = NULL;
	else
	{
		char pathFile[128];
		PathFile::MakePath(pathFile, "weapons\\", m_model);
		m_brassHandle = g_widgetFactory.LoadModel(g_loadSOM, pathFile, m_model,
																0, WidgetFactory::SIMPLE,
																m_ttl, m_fadeTime);
	}
	if (m_brassHandle)
	{
		// Create the pool
		g_widgetFactory.CreatePool(m_brassHandle, m_instances);

		// Setup the props
		RigidBodyProps props;
		props.SetBody(m_shellSize, m_shellDensity);
		g_widgetFactory.SetupProperties(m_brassHandle, props);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void EjectingBrassProperties::TerminateEvery()
{
	if (m_brassHandle)
		g_widgetFactory.DestroyPool(m_brassHandle);

	VarObjProperties<EjectingBrass>::TerminateEvery();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
EjectingBrass::EjectingBrass(const EjectingBrassProperties &properties) :
m_properties(properties)
{
	m_nextHitSound = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
EjectingBrass::~EjectingBrass()
{
}

/////////////////////////////////////////////////////////////////////////////
// Show
/////////////////////////////////////////////////////////////////////////////
void EjectingBrass::Eject(
Vector3CRef onGround,
Vector3CRef breech,
Vector3CRef ejectDir,
const DirCos3x3 &orientation,
Vector3CRef weaponV)
{
	if (!m_properties.m_brassHandle)
		return;

	Vector3 velocity = weaponV + g_random.InCone(ejectDir, m_properties.m_ejectCone) * g_random.InRange(m_properties.m_ejectSpeedMin, m_properties.m_ejectSpeedMax);
	Vector3 spin = Vector3(g_random.InRange(m_properties.m_ejectSpinMin.X(), m_properties.m_ejectSpinMax.X()),
									g_random.InRange(m_properties.m_ejectSpinMin.Y(), m_properties.m_ejectSpinMax.Y()),
									g_random.InRange(m_properties.m_ejectSpinMin.Z(), m_properties.m_ejectSpinMax.Z()));
	// Create the widget
	g_widgetFactory.CreateWidget(m_properties.m_brassHandle, breech, orientation, &velocity, &spin);

	// Make the sound
	float now = g_timer.GetEndSec();
	if (m_properties.m_hitSound[0] && now > m_nextHitSound)
	{
		Vector3 pos = onGround + ejectDir - Vector3(0.0f, ejectDir.Y(), 0.0f);
		Sfx::QueueStationary(m_properties.m_hitSound, m_properties.m_hitSoundDelay, pos);
		m_nextHitSound = now + m_properties.m_hitSoundGate;
	}
}

