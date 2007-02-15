/////////////////////////////////////////////////////////////////////////////
// Blooms & glare
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#include "Effects/bloom.h"
#include "Math/sfilter.h"
#include "cmdcon/cmdcon.h"

// There can be only one!
DeclareSingleton(CheapBlooms);

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CheapBlooms::CheapBlooms() :
m_pool(8, 8),
m_effect(4, 4, true)
{
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CheapBlooms::~CheapBlooms()
{
}

/////////////////////////////////////////////////////////////////////////////
// Load the effects
/////////////////////////////////////////////////////////////////////////////
bool CheapBlooms::LoadEffects(
const char *textureDBL)
{
	// Load it!
	if (!m_decalSet.Load(textureDBL) || m_decalSet.Textures() == 0)
		return false;

	// Create the emitters
	for (int i = 0; i < m_decalSet.Textures(); i++)
	{
		DecalTextureLayer textureLayer(i);
		Effect *effect = new Effect(m_pool, m_decalSet, &textureLayer, 1);
		ASSERT(effect);

		char varName[32];
		sprintf(varName, "bloom%d", i);
		effect->Initialize(varName);

		m_effect.Add(effect);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// load blooms from the world DBL
/////////////////////////////////////////////////////////////////////////////
bool CheapBlooms::DoWorldDBL(
t_Handle worldHandle)
{
	// Do the halos
	ts_RecordList *recordList = (ts_RecordList *)dblGetChunkData(DB_BLOOM, DEFAULT_CHUNK_KEY_STRING, worldHandle);
	if (!recordList)
		return false;

	// setup array
	int blooms = (int)recordList->usRecordCount;
	for (int i = 0; i < blooms; i++)
	{
		// Get the record
		ts_EnvBloom *bloom = (ts_EnvBloom *)GetRecordAddress(recordList, i);
		ASSERT(bloom);

		// Add it!
		Add(bloom->type, bloom->center, bloom->width, bloom->height,
				bloom->normal, bloom->right, bloom->up,
				bloom->color);
	}

	// It's loaded (we are now fat)
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Add a bloom!
/////////////////////////////////////////////////////////////////////////////
CheapBlooms::Handle CheapBlooms::Add(
int effectIndex,
const Vector3 &center,
float width,
float height,
const Vector3 &normal,
const Vector3 &right,
const Vector3 &up,
const ts_bRGBA &color)
{
	// doh!
	if (m_effect.Count() == 0)
		return NULL;

	// Allocate it
	Bloom *bloom = m_pool.New();
	if (!bloom)
		return NULL;

	// Set it up!
	bloom->m_effect = effectIndex % m_effect.Count();
	bloom->m_center = center;
	bloom->m_halfWidth = width * 0.5f;
	bloom->m_halfHeight = height * 0.5f;
	bloom->m_normal = normal;
	bloom->m_right = right;
	bloom->m_up = up;
	bloom->m_color = color;
	bloom->m_baseAlpha = (float)color.A / 255.0f;
	bloom->m_hidden = false;
	bloom->m_targetFactor = 0.0f;
	bloom->m_factor = bloom->m_targetFactor;
	bloom->m_visible = false;

	// Add it to the list
	m_effect[bloom->m_effect]->Add(bloom);
	return (Handle)bloom;
}

/////////////////////////////////////////////////////////////////////////////
// Hide the bloom
/////////////////////////////////////////////////////////////////////////////
void CheapBlooms::Hide(
Handle handle,
bool hide)
{
	if (!handle)
		return;

	Bloom *bloom = (Bloom *)handle;
	bloom->m_hidden = hide;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool CheapBlooms::Hidden(
Handle handle)
{
	if (!handle)
		return true;

	Bloom *bloom = (Bloom *)handle;
	return bloom->m_hidden;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool CheapBlooms::Remove(
Handle handle)
{
	if (!handle)
		return false;

	Bloom *bloom = (Bloom *)handle;
	m_effect[bloom->m_effect]->Remove(bloom);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Draw
/////////////////////////////////////////////////////////////////////////////
void CheapBlooms::Update(
CameraScene *scene,
const Vector3 *sun)
{
	// Anything to do?
	if (!m_decalSet.Okay())
		return;

	// Get the current camera
	Camera &camera = g_cameraDirector.CurrentCamera(scene);
	float deltaT = g_timer.GetFrameSec();

	// Figure out the sun
	Vector3 realSun;
	if (sun)
		realSun = *sun;
	else
	{
		realSun.Set(1.0f, 1.0f, 0.0f);
		realSun.Normalize();
	}

	// Go through the effects
	int effects = m_effect.Count();
	for (int effect = 0; effect < effects; effect++)
		m_effect[effect]->Update(camera, realSun, deltaT);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CheapBlooms::Effect::Effect(
Pool<Bloom> &pool,
DecalSet &decalSet,
const DecalTextureLayer *layer,
int layers) :
m_pool(pool),
m_emitter(decalSet, layer, layers),
m_list(false)
{
	m_maxInstances = 32;
	m_heightTweak = 0.015f;
	m_timeConstant = 0.1f;

	m_dotStart = 0.0f;
	m_dotEnd = 0.9f;
	m_alphaStart = 0.1f;
	m_alphaEnd = 1.0f;
	m_scaleMin = 1.0f;
	m_scaleMax = 1.3f;

	m_useReflect = false;
	m_power = 16.0f;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CheapBlooms::Effect::~Effect()
{
	Bloom *bloom = m_list.Head();
	while (bloom)
	{
		Bloom *next = bloom->next;
		m_list.Unlink(bloom);
		m_pool.Delete(bloom);
		bloom = next;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Initialize
/////////////////////////////////////////////////////////////////////////////
void CheapBlooms::Effect::Initialize(
const char *varName)
{
	const char *context = varName;
	g_console.BindScript(context, varName);

	g_console.CreateVar(context, "maxInstances", m_maxInstances);
	g_console.CreateVar(context, "heightTweak", m_heightTweak);
	g_console.CreateVar(context, "timeConstant", m_timeConstant);

	g_console.CreateVar(context, "dotStart", m_dotStart);
	g_console.CreateVar(context, "dotEnd", m_dotEnd);
	g_console.CreateVar(context, "alphaStart", m_alphaStart);
	g_console.CreateVar(context, "alphaEnd", m_alphaEnd);
	g_console.CreateVar(context, "scaleMin", m_scaleMin);
	g_console.CreateVar(context, "scaleMax", m_scaleMax);

	g_console.CreateVar(context, "useReflect", m_useReflect);
	g_console.CreateVar(context, "power", m_power);

	g_console.ExecuteContext(context);

#ifndef INTERACTIVE_CONSOLE
	g_console.DeleteContext(context);
#endif

	m_emitter.Setup(m_maxInstances, -8, true);
}

/////////////////////////////////////////////////////////////////////////////
// Update!
/////////////////////////////////////////////////////////////////////////////
void CheapBlooms::Effect::Update(
Camera &camera,
const Vector3 &sun,
float deltaT)
{
	// Sort the list based on camera distance
	Bloom *bloom = m_list.Head();
	int visible = 0;
	while (bloom)
	{
		Vector4 inCamera = camera.ToView(bloom->m_center);
		bloom->m_cameraD = -inCamera.Z();
		if (bloom->m_cameraD < 0.0f && Math::Abs(bloom->m_cameraD) > Math::Max(bloom->m_halfWidth, bloom->m_halfHeight))
			bloom->m_visible = false;
		else
		{
			bloom->m_targetFactor = ComputeFactor(camera, sun, *bloom);
			bloom->m_visible = !Math::Zero(bloom->m_targetFactor);
		}

		if (bloom->m_visible)
			++visible;
		else
			bloom->m_factor = bloom->m_targetFactor = 0.0f;

		bloom = bloom->next;
	}
	m_list.BubbleSortDescending();

	// Only draw the maximum dictated by the list
	bloom = m_list.Head();
	while (visible > m_maxInstances)
	{
		// Take this one off (farther away)
		if (bloom->m_visible)
			--visible;

		// next
		bloom = bloom->next;
	}

	// Now build the quad list
	m_emitter.Clear();
	while (bloom)
	{
		// Wee!
		if (!bloom->m_hidden && bloom->m_visible)
		{
			// filter the factor
			bloom->m_factor = FOFilterImplicit(bloom->m_factor, bloom->m_targetFactor, deltaT, m_timeConstant);

			// setup the color
			ts_bRGBA color;
			color.R = bloom->m_color.R;
			color.G = bloom->m_color.G;
			color.B = bloom->m_color.B;

			// Do the alpha
			float alpha = (m_alphaStart + bloom->m_factor * (m_alphaEnd - m_alphaStart)) * bloom->m_baseAlpha;
			color.A = (u8)Math::Clamp(alpha * 255.0f, 0.0f, 255.0f);

			// Compute the scale
			float scale = (m_scaleMin + bloom->m_factor * (m_scaleMax - m_scaleMin));

			// do it!















































































































































































































































			m_emitter.Create(bloom->m_center + bloom->m_normal * m_heightTweak,
												bloom->m_right, bloom->m_up,
												bloom->m_halfWidth * scale, bloom->m_halfHeight * scale,
												color);
		}

		bloom = bloom->next;
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
float CheapBlooms::Effect::ComputeFactor(
Camera &camera,
const Vector3 &sun,
const Bloom &bloom)
{
	Vector3 toEye = camera.Eye() - bloom.m_center;
	toEye.Normalize();
	float dot;
	if (!m_useReflect)
		dot = Vector3::Dot(bloom.m_normal, toEye);
	else
	{
		// Compute reflection vector
		Vector3 r = bloom.m_normal * (2.0f * Vector3::Dot(bloom.m_normal, sun)) - sun;

		// compute the angle
		dot = Vector3::Dot(r, toEye);
		if (dot < 0.0f)
			return 0.0f;

		// Raise to a power
		dot = Math::Pow(dot, m_power);
	}

	if (dot < 0.0f)
		return 0.0f;
	else
		return m_dotStart + dot * (m_dotEnd - m_dotStart);
}

