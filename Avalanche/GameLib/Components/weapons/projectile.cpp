/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "Components/ComponentsPCH.h"
#include "stage/SurfaceType.h"
#include "Components/weapons/projectile.h"
#include "Components/weapons/projlib.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties::ProjectileProperties(
ProjectileCreator &creator,
int index) : VarObjProperties<Projectile>(creator, index)
{
	m_material[0] = '\0';
	m_surfaceType = -1;

	m_damage = 0.0f;

	m_spawnCount = 0;
	m_spawnAngle = 0;

	m_flightSound[0] = '\0';
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
ProjectileProperties::~ProjectileProperties()
{
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void ProjectileProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<Projectile>::RegisterContext(context);

	// Create the context
	g_console.CreateVar(context, "material", m_material, sizeof(m_material) - 1);

	g_console.CreateVar(context, "damage", m_damage, CONVAR_REAL);

	g_console.CreateVar(context, "spawnCount", m_spawnCount);
	g_console.CreateVar(context, "spawnAngle", m_spawnAngle, CONVAR_DEGREES);

	g_console.CreateVar(context, "flightSound", m_flightSound, sizeof(m_flightSound) - 1);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void ProjectileProperties::InitializeEvery()
{
	// Call the parent
	VarObjProperties<Projectile>::InitializeEvery();

	// Look up the material
	m_surfaceType = g_surfaceType.GetIndex(m_material);
	ASSERT(m_surfaceType >= 0);
	if (m_surfaceType < 0)
	{
		m_surfaceType = g_surfaceType.GetIndex("Default");
		if (m_surfaceType < 0)
			m_surfaceType = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// After the script is executed on the context
/////////////////////////////////////////////////////////////////////////////
Projectile *ProjectileProperties::CreateInstance()
{
	// Create an instance using these properties
	return VarObjProperties<Projectile>::CreateInstance();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Projectile::Projectile(const ProjectileProperties &properties) :
m_properties(properties)
{
	m_owner = (ActorHandle)INVALID_OBJECT_HANDLE;
}


