/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a supersonic projectile
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/tracerset.h"

// Our singleton
DeclareSingleton(TracerSetLibrary);

// Our array of creation functions
VarObjCreator<TracerSet> *VarObjLibrary<TracerSet>::m_creatorList = NULL;

volatile TracerSetDependency g_tracerSetDependency;

// Our only creator right now
TracerSetCreator g_tracerSetCreator;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TracerSetProperties::TracerSetProperties(
TracerSetCreator &creator,
int index) : VarObjProperties<TracerSet>(creator, index)
{
	m_tracer = NULL;
	m_tracerInstances = 0;

	m_tracerWiggle = 0.01f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TracerSetProperties::~TracerSetProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void TracerSetProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<TracerSet>::RegisterContext(context);

	// Load the tracer
	g_console.CreateVar(context, "headColor", &m_tracerHeadColor);

	g_console.CreateVar(context, "tailColor", &m_tracerTailColor);

	g_console.CreateVar(context, "headWidth", m_tracerHeadWidth, CONVAR_REAL);
	g_console.CreateVar(context, "tailWidth", m_tracerTailWidth, CONVAR_REAL);

	g_console.CreateVar(context, "minStartD", m_tracerMinStartD, CONVAR_REAL);
	g_console.CreateVar(context, "maxStartD", m_tracerMaxStartD, CONVAR_REAL);

	g_console.CreateVar(context, "minLength", m_tracerMinLength, CONVAR_REAL);
	g_console.CreateVar(context, "maxLength", m_tracerMaxLength, CONVAR_REAL);

	g_console.CreateVar(context, "percentage", m_tracerPercentage);

	g_console.CreateVar(context, "textureWrap", m_tracerTextureWrap);
	g_console.CreateVar(context, "wiggle", m_tracerWiggle);

	g_console.CreateVar(context, "distort", m_tracerDistort);

	g_console.CreateVar(context, "instances", m_tracerInstances);

	g_console.CreateVar(context, "TTL", m_tracerTTL, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
// Setup our stuff
/////////////////////////////////////////////////////////////////////////////
void TracerSetProperties::InitializeEvery()
{
	// Call the parent
	VarObjProperties<TracerSet>::InitializeEvery();

	// Try to create the tracer
	if (m_tracerInstances > 0)
	{
		TracerModel model;

		model.textureIndex = 0;
		model.headColor.R = (u8)Math::Clamp(m_tracerHeadColor.X(), 0.0f, 255.0f);
		model.headColor.G = (u8)Math::Clamp(m_tracerHeadColor.Y(), 0.0f, 255.0f);
		model.headColor.B = (u8)Math::Clamp(m_tracerHeadColor.Z(), 0.0f, 255.0f);
		model.headColor.A = (u8)Math::Clamp(m_tracerHeadColor.W(), 0.0f, 255.0f);

		model.tailColor.R = (u8)Math::Clamp(m_tracerTailColor.X(), 0.0f, 255.0f);
		model.tailColor.G = (u8)Math::Clamp(m_tracerTailColor.Y(), 0.0f, 255.0f);
		model.tailColor.B = (u8)Math::Clamp(m_tracerTailColor.Z(), 0.0f, 255.0f);
		model.tailColor.A = (u8)Math::Clamp(m_tracerTailColor.W(), 0.0f, 255.0f);

		model.headWidth = m_tracerHeadWidth;
		model.tailWidth = m_tracerTailWidth;

		model.minStartD = m_tracerMinStartD;
		model.maxStartD = m_tracerMaxStartD;
		model.minLength = m_tracerMinLength;
		model.maxLength = m_tracerMaxLength;

		model.percent = m_tracerPercentage;

		model.textureWrap = m_tracerTextureWrap;
		model.wiggleScale = m_tracerWiggle;

		model.maxInstances = m_tracerInstances;
		model.ttl = m_tracerTTL;

		// Make it!
		m_tracer = g_tracerManager.New(model, m_tracerDistort);
	}
}

// Setup our stuff
/////////////////////////////////////////////////////////////////////////////
void TracerSetProperties::TerminateEvery()
{
	// Call the parent
	VarObjProperties<TracerSet>::TerminateEvery();

	delete m_tracer;
	m_tracer = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TracerSet::TracerSet(
const TracerSetProperties &properties) :
m_properties(properties)
{
}


/////////////////////////////////////////////////////////////////////////////
// Create a tracer!
/////////////////////////////////////////////////////////////////////////////
bool TracerSet::CreateChord(
const Vector3 &origin,
const Vector3 &intersection)
{
	if (!m_properties.m_tracer)
		return false;

	m_properties.m_tracer->CreateInstanceChord(origin, intersection);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Create a tracer!
/////////////////////////////////////////////////////////////////////////////
bool TracerSet::CreateRay(
const Vector3 &origin,
const Vector3 &dir)
{
	if (!m_properties.m_tracer)
		return false;

	m_properties.m_tracer->CreateInstanceRay(origin, dir);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TracerSetLibrary::InitializeEvery()
{
  	TracerManager::Create("weapons\\tracer.dbl");
	VarObjLibrary<TracerSet>::InitializeEvery();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TracerSetLibrary::TerminateEvery()
{
	VarObjLibrary<TracerSet>::TerminateEvery();
	TracerManager::Destroy();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TracerSetDependency::InitializeOnce()
{
	NewSingleton(TracerSetLibrary);
	g_tracerSetLibrary.InitializeOnce();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TracerSetDependency::TerminateOnce()
{
	g_tracerSetLibrary.TerminateOnce();
	DeleteSingleton(TracerSetLibrary);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TracerSetDependency::InitializeEvery()
{
	g_tracerSetLibrary.InitializeEvery();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void TracerSetDependency::TerminateEvery()
{
	g_tracerSetLibrary.TerminateEvery();
}


