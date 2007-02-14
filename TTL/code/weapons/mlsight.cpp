/////////////////////////////////////////////////////////////////////////////
// Weapon instantiation class for a ZoomSight
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"
#include "weapons/mlsight.h"
#include "Interfaces/ViewIF.h"
#include "camera/shootercam.h"

// Create this
MagliteSightCreator g_magliteSightCreator;

/////////////////////////////////////////////////////////////////////////////
// Do the work
/////////////////////////////////////////////////////////////////////////////
MagliteSightCreator::MagliteSightCreator() :
AimSightCreator("maglite")
{
}

/////////////////////////////////////////////////////////////////////////////
// Create
/////////////////////////////////////////////////////////////////////////////
AimSight *MagliteSightCreator::CreateInstance(
const VarObjProperties<AimSight> &properties)
{
	return new MagliteSight((const MagliteSightProperties &)properties);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
AimSightProperties *MagliteSightCreator::NewProperties(
int index)
{
	return new MagliteSightProperties(*this, index);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MagliteSightProperties::MagliteSightProperties(
AimSightCreator &creator,
int index) : ZoomSightProperties(creator, index)
{
	m_maglite = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MagliteSightProperties::~MagliteSightProperties()
{
	delete m_maglite;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void MagliteSightProperties::RegisterContext(
const char *context)
{
	// Call the parent
	ZoomSightProperties::RegisterContext(context);

	// Load the tracer
	g_console.CreateVar(context, "MagliteColorStart", &m_magliteColorStart);
	g_console.CreateVar(context, "MagliteColorEnd", &m_magliteColorEnd);
	g_console.CreateVar(context, "MagliteRadiusStart", m_magliteRadiusStart, CONVAR_REAL);
	g_console.CreateVar(context, "MagliteRadiusEnd", m_magliteRadiusEnd, CONVAR_REAL);
	g_console.CreateVar(context, "MagliteAttenuation", m_magliteAttenuation, CONVAR_REAL);
	g_console.CreateVar(context, "MagliteIntensity", m_magliteIntensity, CONVAR_REAL);
	g_console.CreateVar(context, "MagliteRange", m_magliteRange, CONVAR_REAL);
	g_console.CreateVar(context, "MagliteLightDistance", m_magliteLightD, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
// Setup our stuff
/////////////////////////////////////////////////////////////////////////////
void MagliteSightProperties::PostLoadSetup()
{
	// Try to create the laser
	LaserRenderParameters params;
	memset(&params, '\0', sizeof(params));

	params.rayStartRadius = m_magliteRadiusStart;
	params.rayEndRadius = m_magliteRadiusEnd;

	SetColor(m_magliteColorStart, params.colorStart);
	SetColor(m_magliteColorEnd, params.colorEnd);
	SetColor(m_magliteColorEnd, params.colorMiss);

	params.maxRange = m_magliteRange;

	// Make it!
	m_maglite = new LaserRender("weapons\\laser.dbl", params);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MagliteSight::MagliteSight(
const MagliteSightProperties &properties) :
ZoomSight(properties),
m_mlProps(properties)
{
	if (m_mlProps.m_maglite)
		m_magliteHandle = m_mlProps.m_maglite->CreateInstance();
	m_light = NULL;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MagliteSight::~MagliteSight()
{
	if (m_mlProps.m_maglite && m_magliteHandle)
		m_mlProps.m_maglite->DestroyInstance(m_magliteHandle);
	if (m_light)
	{
		g_lightSet.removeDynamicLight(m_light);
		//delete m_light;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MagliteSight::Deactivate(Weapon &weapon)
{
	ZoomSight::Deactivate(weapon);

	if (m_mlProps.m_maglite && m_magliteHandle)
		m_mlProps.m_maglite->Hide(m_magliteHandle);
	if (m_light)
		m_light->enableLight(false);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void MagliteSight::Update(Weapon &weapon)
{
	ZoomSight::Update(weapon);

	Vector3 sight = m_weapon->GetBoneInWorld(0, BONE_SIGHT);
	Vector3 dir = m_weapon->GetBoneDir(0, BONE_SIGHT);
	CActor *m_shooter = m_weapon->GetOwner();

	// Are we the camera owner
	bool ownsCamera = (g_shooterCam.Active() && g_shooterCam.GetFollow() == m_shooter);

	if (m_mlProps.m_maglite && m_magliteHandle)
	{
		if (ZoomLevel() == 0)
			m_mlProps.m_maglite->Hide(m_magliteHandle);
		else
		{
			if (ownsCamera)
				m_mlProps.m_maglite->Hide(m_magliteHandle);
			else
				m_mlProps.m_maglite->Update(m_magliteHandle, m_shooter, sight, sight, dir);
		}
	}

	// Create the light
	if (!m_light && ownsCamera && ZoomLevel() != 0)
	{
		// Create the source
		m_light = new CLightRecord;
		ASSERT(m_light);
		if (m_light)
		{
			// set this bad boy up
			Vector4 color;
			color.Set(m_mlProps.m_magliteColorStart.X() / 255.0f,
							m_mlProps.m_magliteColorStart.Y() / 255.0f,
							m_mlProps.m_magliteColorStart.Z() / 255.0f,
							0.0f);
			m_light->setColor(color);
			m_light->setType(CLightRecord::LIGHT_OMNI);
			m_light->setAttenuation(m_mlProps.m_magliteAttenuation);
			g_lightSet.addDynamicLight(m_light);
			m_light->setIntensity(m_mlProps.m_magliteIntensity);
		}
	}

	// Update the light
	if (m_light)
	{
		if (ZoomLevel() == 0 || !ownsCamera)
			m_light->enableLight(false);
		else
		{
			// Toss out the ray
			CLineEntity testLine;
			testLine.Line().SetOriginBasis(sight, dir);
			testLine.Line().SetMaxBoundary(m_mlProps.m_magliteLightD);
			testLine.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
			testLine.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
			testLine.SetIgnoreEntity(m_shooter);

			// Go!
			g_collisionSystem.ResetStack();
			CLineQuery lineQuery;
			float d;
			const float minD = 0.5f;
			if (!lineQuery.Test(testLine))
				d = m_mlProps.m_magliteLightD;
			else
				d = Math::Clamp(lineQuery.d - minD, -minD, m_mlProps.m_magliteLightD);

			// Set it!
			m_light->setPosition(sight + dir * d);
			m_light->setIntensity(m_mlProps.m_magliteIntensity);
			m_light->enableLight(true);
		}
	}
}

