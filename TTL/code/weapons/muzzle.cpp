/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#include "Game/Database/lightset.h"
#include "Math/sfilter.h"
#include "weapons/muzzleLib.h"
#include "weapons/muzzle.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MuzzleFlashProperties::MuzzleFlashProperties(
MuzzleFlashCreator &creator,
int index) : VarObjProperties<MuzzleFlash>(creator, index)
{
	m_modelDBL[0] = '\0';
	m_baseAlpha = 1.0f;
	m_scaleStart.Clear();
	m_scaleEnd.Clear();
	m_scaleTimeUp = m_scaleTimeDown = 0.2f;
	m_scalePingPong = false;
	m_minSpin = 0.0f;
	m_maxSpin = 45.0f;

	m_sourceColor.Clear();
	m_sourceAttenuation = 1.0f;
	m_sourceIntensity = 1.0f;
	m_sourceDuration = 0.1f;

	m_particle[0] = '\0';
	m_particleNScale = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////
// For parsing the data
/////////////////////////////////////////////////////////////////////////////
void MuzzleFlashProperties::RegisterContext(
const char *context)
{
	// Call the parent
	VarObjProperties<MuzzleFlash>::RegisterContext(context);

	g_console.CreateVar(context, "model", m_modelDBL, sizeof(m_modelDBL) - 1);
	g_console.CreateVar(context, "baseAlpha", m_baseAlpha, CONVAR_REAL);
	g_console.CreateVar(context, "scaleStart", &m_scaleStart);
	g_console.CreateVar(context, "scaleEnd", &m_scaleEnd);
	g_console.CreateVar(context, "scaleTimeUp", m_scaleTimeUp, CONVAR_REAL);
	g_console.CreateVar(context, "scaleTimeDown", m_scaleTimeDown, CONVAR_REAL);
	g_console.CreateVar(context, "scalePingPong", m_scalePingPong);

	g_console.CreateVar(context, "minSpin", m_minSpin, CONVAR_DEGREES);
	g_console.CreateVar(context, "maxSpin", m_maxSpin, CONVAR_DEGREES);

	g_console.CreateVar(context, "sourceColor", &m_sourceColor);
	g_console.CreateVar(context, "sourceAttenuation", m_sourceAttenuation, CONVAR_REAL);
	g_console.CreateVar(context, "sourceIntensity", m_sourceIntensity, CONVAR_REAL);
	g_console.CreateVar(context, "sourceDuration", m_sourceDuration, CONVAR_REAL);

	g_console.CreateVar(context, "particle", m_particle, sizeof(m_particle) - 1);
	g_console.CreateVar(context, "particleNScale", m_particleNScale, CONVAR_REAL);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MuzzleFlash::MuzzleFlash(const MuzzleFlashProperties &properties) :
m_properties(properties)
{
	m_model = new SimpleInstance;
	ASSERT(m_model);
	if (m_model)
	{
		int flags = /*SimpleInstance::ALPHA_SORT | */ SimpleInstance::NO_ZWRITE | SimpleInstance::ADDITIVE_BLEND;
		m_model->Create(m_properties.m_modelDBL, flags);
		if (!Math::Equal(m_properties.m_baseAlpha, 1.0f))
			m_model->SetConstantAlpha(m_properties.m_baseAlpha);
		m_model->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 2);
		m_model->Show(false);
	}
	m_state = INACTIVE;

	// The source
	m_light = NULL;
	m_lightVisible = false;
}

/////////////////////////////////////////////////////////////////////////////
// Clean up
/////////////////////////////////////////////////////////////////////////////
MuzzleFlash::~MuzzleFlash()
{
	delete m_model;
	if (m_light)
	{
 		g_lightSet.removeDynamicLight(m_light);
		//delete m_light;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Show
/////////////////////////////////////////////////////////////////////////////
void MuzzleFlash::Start(
Vector3CRef pos,
const Graphics4x4 &weaponToWorld,
bool doLight)
{
	/// Start the
	if (m_model)
	{
		m_state = PING;
		m_scaleStart = m_properties.m_scaleStart;
		m_scaleT = (m_properties.m_scaleEnd - m_scaleStart) / m_properties.m_scaleTimeUp;
		m_age = 0.0f;
		m_doneAge = m_properties.m_scaleTimeUp;
		m_spin = g_random.InRange(m_properties.m_minSpin, m_properties.m_maxSpin);
	}

	// Use lights for this gun?
	if (Math::Zero(m_properties.m_sourceDuration))
		return;

	// Create the light
	if (doLight && !m_light)
	{
		// Create the source
		m_light = new CLightRecord;
		ASSERT(m_light);
		if (!m_light)
			return;

		// set this bad boy up
		Vector4 color;
		color.X(m_properties.m_sourceColor.X() / 255.0f);
		color.Y(m_properties.m_sourceColor.Y() / 255.0f);
		color.Z(m_properties.m_sourceColor.Z() / 255.0f);
		color.W(0.0f);

		m_light->setColor(color);
		m_light->setType(CLightRecord::LIGHT_OMNI);
		m_light->setAttenuation(m_properties.m_sourceAttenuation);
		g_lightSet.addDynamicLight(m_light);
		m_light->setIntensity(m_properties.m_sourceIntensity);
	}

	// Setup the duration and shiz
	if (doLight && m_light)
	{
		m_light->enableLight(true);
		m_lightTTL = m_properties.m_sourceDuration;
		m_lightVisible = true;
	}

	// Do the particle
	if (m_properties.m_particle[0])
	{
		if (Math::Zero(m_properties.m_particleNScale))
			ParticleMgr__CreateEffect(m_properties.m_particle, &pos, NULL, NULL, NULL, 0.0f);
		else
		{
			Vector3 normal = weaponToWorld.GetRow2() * m_properties.m_particleNScale;
			ParticleMgr__CreateEffect(m_properties.m_particle, &pos, &normal, NULL, NULL, 0.0f);
		}
	}

	Update(0.0f, pos, weaponToWorld);
}

/////////////////////////////////////////////////////////////////////////////
// Hide
/////////////////////////////////////////////////////////////////////////////
void MuzzleFlash::Hide()
{
	if (m_model)
		m_model->Show(false);
	m_state = INACTIVE;

	if (m_light)
		m_light->enableLight(false);
	m_lightVisible = false;
}

/////////////////////////////////////////////////////////////////////////////
// Update
/////////////////////////////////////////////////////////////////////////////
void MuzzleFlash::Update(
float deltaT,
Vector3CRef pos,
const Graphics4x4 &weaponToWorld)
{
	// Any work to to?
	if (m_model && m_state != INACTIVE)
	{
		// Scale it up
		m_age += deltaT;
		Vector3 scale = m_scaleStart + m_scaleT * m_age;

		// Are we done?
		if (m_age > m_doneAge)
		{
	 		if (m_state == PONG || !m_properties.m_scalePingPong)
				m_state = INACTIVE;
			else // PING
			{
				m_state = PONG;
				m_scaleStart = scale;
				m_scaleT = (m_properties.m_scaleStart - scale) / m_properties.m_scaleTimeDown;
				m_age = 0.0f;
				m_doneAge = m_properties.m_scaleTimeDown;
			}
		}

		// Did we go inactive?
		if (m_state == INACTIVE)
			m_model->Show(false);
		else
		{
			// Create the matrix
			Graphics4x4 rotation;
			rotation.SetZRotation(m_spin);

			Graphics4x4 s;
			s.SetScale(scale.X(), scale.Y(), scale.Z());

			Graphics4x4 m;
			m = rotation;
			m *= s;
			m *= weaponToWorld;
			m.PokeTranslation(pos);

			// Set it's position
			m_model->Place(m);

			// Make sure it is shown
			if (m_model->Hidden())
				m_model->Show(true);
		}
	}

	// Do the dynamic light
	if (m_lightVisible)
	{
		// Update the ttl
		m_lightTTL -= deltaT;
		if (m_lightTTL > 0.0f)
			m_light->setPosition(pos);
		else
		{
			// We are dead
			m_light->enableLight(false);
			m_lightVisible = false;
		}
	}
}

