///////////////////////////////////////////////////////////////////////////////
// decal system!
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif
#include "Display/Common/VtxBuffer.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalTextureLayer::DefaultGloss(int index)
{
	m_index = index;
	m_method = TS_METHOD_GLOSS;
	m_blend = RS_BLENDFUNC_GLOSS;
	m_clampU = false;
	m_clampV = false;
	m_filter = TS_FILTER_BILINEAR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalTextureLayer::DefaultPostBump(int index)
{
	m_index = index;
	m_method = TS_METHOD_MODULATESELECT;
	m_blend = RS_BLENDFUNC_DEFAULT;
	m_clampU = true;
	m_clampV = true;
	m_filter = TS_FILTER_BILINEAR;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalTextureLayer::DefaultBump(int index)
{
	m_index = index;
	m_method = TS_METHOD_DOT3;
	m_blend = RS_BLENDFUNC_DOT3;
	m_clampU = true;
	m_clampV = true;
	m_filter = TS_FILTER_BILINEAR;//TS_FILTER_POINTSAMPLE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DecalEmitter::DecalEmitter(
DecalSet &set,
const DecalTextureLayer *layer,
int layers) :
m_set(set)
{
	// Clear out this shizna
	m_maxDecals = 0;
	m_decals = 0;

	// clear our instances
	m_instance = NULL;

	// The scene manager sucks
	m_addedToScene = false;

	// Assume no bump mapping
	m_hasBump = false;

	// Setup the layers
	ASSERT(layers > 0 && layers < MAX_DECAL_TEXTURE_LAYERS);
	m_textureLayers = 0;
	for (int l = 0; l < layers; l++)
	{
#if defined(PS2)
		// PS2 cannot do gloss mapping
		if (layer[l].m_method == TS_METHOD_GLOSS)
			continue;
		if (layer[l].m_method == TS_METHOD_DOT3)
			continue;
#endif
		// Set this flag if we have gloss!
		if (layer[l].m_method == TS_METHOD_DOT3)
	 		m_hasBump = true;

		// Copy this one in!
		m_textureLayer[m_textureLayers++] = layer[l];
	}
	ASSERT(m_textureLayers > 0);

	// Setup the texture context & layer flags
	for (int t = 0; t < MAX_DECAL_TEXTURE_LAYERS; t++)
	{
		if (t < m_textureLayers)
			m_texFlags[t] = VtxBuffer::VERT_TEX_DIM2;
		else
			m_texFlags[t] = 0;
		m_texMatrix[t].Identity();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DecalEmitter::~DecalEmitter()
{
	// Get rid of all the info
	DestroyGeometry();
	DestroyInstance();
}

///////////////////////////////////////////////////////////////////////////////
// Set it up!
///////////////////////////////////////////////////////////////////////////////
bool DecalEmitter::Setup(
int maxDecals,
int renderBucket,
bool overbrighting)
{
	m_maxDecals = maxDecals;

	if (renderBucket < 0)
		renderBucket = INSTANCE_ARTIST_RENDER_BUCKET_MAX + (-1 - renderBucket);
	if (!CreateInstance(renderBucket))
		return false;

	if (!CreateGeometry(overbrighting))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DecalEmitter::CreateInstance(
int renderBucket)
{
	// Dummy up the instance
	MEM_SET_ALLOC_NAME("Decal inst");
	m_instance = new CInstance;
	if (!m_instance)
		return false;

	// setup the bounding data
	BoundingBox__Clear(&BoundingBox);
	BoundingBox__Include(&BoundingBox, Vector3(10000.0f, 10000.0f, 10000.0f));
	BoundingBox__Include(&BoundingBox, Vector3(-10000.0f, -10000.0f, -10000.0f));
	BoundingSphere.Center = BoundingBox__GetCenter(&BoundingBox);
	BoundingSphere.Radius = (BoundingSphere.Center - BoundingBox.Max).Length();

	// Make the instance
	m_instance->InitFromDynamicObject( this );
	m_instance->SetName( "decal" );

	// Assume alpha
	m_instance->SetAlphaSort(false);
	m_instance->SetZWriteDisabled(true);
	m_instance->SetReceiveShadows(false);

	m_instance->SetRenderBucket(renderBucket);//INSTANCE_ARTIST_RENDER_BUCKET_MAX + m_renderBucket );

	// Make sure to disable lighting
	m_instance->SetDynamicallyLit( false );

	// calculate initial bounding data
	m_instance->UpdateBoundingVolumesAccurate();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalEmitter::DestroyInstance()
{
	// remove the instance from scene
	if (m_instance && m_addedToScene)
		SceneMgr::RemoveInstance(m_instance);

	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalEmitter::SetupStripTextures(
StripBuffer &strip,
bool overbright)
{
 	// set number layers to render
	strip.SetNumTexturePasses(m_textureLayers);
	strip.SetTextureArray(m_set.GetTextureArray(), 0);

	for (int layer = 0; layer < m_textureLayers; layer++)
	{
		// Get the texture
		Texture *texture = m_set.GetTexture(m_textureLayer[layer].m_index);
		ASSERT(texture);

		// Figure out the blend
		u8 blend;
		if (overbright && layer == 0)
			blend = RS_BLENDFUNC_ADD;
		else
			blend = m_textureLayer[layer].m_blend;

		// Set it up!
		strip.SetPassTexture(layer, texture,
		 							m_textureLayer[layer].m_method,
		 							m_textureLayer[layer].WrapU(),
		 							m_textureLayer[layer].WrapV(),
		 							m_textureLayer[layer].m_filter,
		 							blend);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalEmitter::EnsureSceneBullshit()
{
	if (!m_addedToScene && m_instance)
	{
		// add him to the scene
		SceneMgr::AddInstance(m_instance);
		SceneMgr::SetInstanceVisRegionStatic(m_instance,true);
		m_addedToScene = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalEmitter::SetTFactor()
{
#ifdef DIRECTX
	// Fake a light for testing
	Camera &camera = g_cameraDirector.CurrentCamera();
	Vector3 light = camera.Dir();

	static float z = 0.5f;
	light.Z(z);
	light.Normalize();

	_RenderState__SetTFactor(light.X(), light.Y(), light.Z(), 0.0f);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DecalEmitter::DefaultUV(
Vector2 uv[4])
{
	uv[0].Set(1.0, 1.0f);
	uv[1].Set(0.0, 1.0f);
	uv[2].Set(1.0, 0.0f);
	uv[3].Set(0.0, 0.0f);
}



