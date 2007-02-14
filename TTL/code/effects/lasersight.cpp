///////////////////////////////////////////////////////////////////////////////
// Laser sight!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif //
#define VTXBUFFER_NEED_INLINES
#include "Display/Common/VtxBuffer.h"

// Our texture array for fast rendering
Matrix4x4 LaserRender::s_texMatrix[8];

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LaserRender::LaserRender(
const char *textureDBL,
LaserRenderParameters &params,
int maxInstances) :
m_maxInstances(maxInstances)
{
  	// init all matrices to identity
	for (int m = 0; m < 8; m++)
		s_texMatrix[m].Identity();

	// Load the textures
	LoadTextures(textureDBL);

	// save our settings
	m_rayRadiusStart = params.rayStartRadius;
	m_rayRadiusEnd = params.rayEndRadius;
	m_dotRadius = params.dotRadius;
	m_colorStart = *((int *)&params.colorStart);
	m_colorEnd = *((int *)&params.colorEnd);
	m_colorDot = *((int *)&params.colorDot);
	m_colorMiss = *((int *)&params.colorMiss);
	m_maxRange = params.maxRange;

	// Create the geometry
	m_locked = false;
	BuildGeometry();

	// Nothing is in use yet
	m_instanceList = new Instance[m_maxInstances];
	ASSERT(m_instanceList);
	for (int i = 0; i < m_maxInstances; i++)
		m_instanceList[i].handle = NULL;

	// The scene manager sucks
	m_addedToScene = false;

  	m_rangeFactor = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LaserRender::~LaserRender()
{
	// Get rid of all the info
	// remove the instance from scene
	if (m_instance && m_addedToScene)
		SceneMgr::RemoveInstance(m_instance);

	// Clear all the allocated thingies
	delete m_instance;

	delete [] m_instanceList;
	delete m_textureArray;
}

///////////////////////////////////////////////////////////////////////////////
// Create an instance of this sight
///////////////////////////////////////////////////////////////////////////////
LaserRender::Handle LaserRender::CreateInstance()
{
	// What is the best
	for (int i = 0; i < m_maxInstances; i++)
	{
		// Find an unsed one?
		if (!m_instanceList[i].handle)
		{
			m_instanceList[i].handle = (Handle)(i + 1);
			m_instanceList[i].hidden = true;
			return m_instanceList[i].handle;
		}
	}

	// Ouch
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LaserRender::DestroyInstance(
Handle handle)
{
	// Anything to see here
	if (!handle)
		return;

	// Free it
	int instanceIndex = (int)handle - 1;
	m_instanceList[instanceIndex].handle = NULL;

	// Hide it
	Lock();
	Hide(instanceIndex);
	Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Update the position of this bad boy (automatically ensures it is shown)
///////////////////////////////////////////////////////////////////////////////
void LaserRender::Update(
Handle handle,
CActor *shooter,
Vector3CRef laserOrigin,
Vector3CRef startTrace,
Vector3CRef dir,
bool startIsOrigin,
float inflate,
bool forceMiss)
{
	// Ooops
	if (!handle)
		return;

	// What is the thingie we are working on
	int instanceIndex = (int)handle - 1;
	Instance &instance = m_instanceList[instanceIndex];

	// save it
	instance.hidden = false;
	instance.start = laserOrigin;
	instance.dir = dir;
	instance.inflate = inflate;

	// Doh
	const float range = m_maxRange * m_rangeFactor;

	// Toss out the ray
	CLineEntity testLine;
	testLine.Line().SetOriginBasis(startTrace, dir);
	testLine.Line().SetMaxBoundary(range);
	testLine.Line().SetDetectionAbility(CLineCollisionObject::DETECT_FRONT_SIDE);
	testLine.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	testLine.SetIgnoreEntity(shooter);

	// Go!
	g_collisionSystem.ResetStack();
	CLineQuery lineQuery;
	instance.hit = lineQuery.Test(testLine, CStageEntity::USE_COMPLEX);
	if (!instance.hit)
	{
		instance.end = instance.start + (instance.dir * range);
		HideDot(instanceIndex);
	}
	else
	{
		// Re-adjust this
		if (!startIsOrigin)
			instance.dir = (lineQuery.intersection - startTrace).Normalized();

		// Adjust for interpenetration
		const float heightTweak = 0.025f;
		instance.end = lineQuery.intersection - (instance.dir * heightTweak);

		// Compute the coordinate system for the dot
		if (forceMiss)
		{
			HideDot(instanceIndex);
			instance.hit = false;
		}
		else
		{
			Vector3Packed center = instance.end;
			Vector3Packed right = Vector3::Cross(lineQuery.normal, instance.dir);

			// I need to fix this
			if (!Math::Zero(right.LengthSquared()))
				right.Normalize();
			else
				right.Set(1.0f, 0.0f, 0.0f);
			Vector3Packed up = Vector3::Cross(lineQuery.normal, right);

			PlaceDot(instanceIndex, center, up, right);
		}
	}

	// Make sure the fucking scene is ready
	EnsureSceneBullshit();
}

///////////////////////////////////////////////////////////////////////////////
// Update the position of this bad boy (automatically ensures it is shown)
///////////////////////////////////////////////////////////////////////////////
void LaserRender::Update(
Handle handle,
CActor *shooter,
Vector3CRef laserOrigin,
Vector3CRef laserEnd,
float inflate)
{
	// Ooops
	if (!handle)
		return;

	// What is the thingie we are working on
	int instanceIndex = (int)handle - 1;
	Instance &instance = m_instanceList[instanceIndex];

	// how far out is this
	float length = (laserEnd - laserOrigin).Length();

	// save it
	instance.hidden = false;
	instance.start = laserOrigin;
	instance.dir = (laserEnd - laserOrigin) / length;
	instance.inflate = inflate;

	if (length > m_maxRange)
	{
		instance.hit = false;
		instance.end = instance.start + (instance.dir * m_maxRange);
		HideDot(instanceIndex);
	}
	else
	{
		// We have a hit!
		instance.hit = true;

		// Adjust for interpenetration
		const float heightTweak = 0.025f;
		instance.end = laserEnd - (instance.dir * heightTweak);

		// Compute the coordinate system for the dot
		Vector3Packed center = instance.end;
		Vector3Packed normal(0.0f, 1.0f, 0.0f);
		Vector3Packed right = Vector3::Cross(normal, instance.dir);

		// I need to fix this
		if (!Math::Zero(right.LengthSquared()))
			right.Normalize();
		else
			right.Set(1.0f, 0.0f, 0.0f);
		Vector3Packed up = Vector3::Cross(normal, right);

		PlaceDot(instanceIndex, center, up, right);
	}

	// Make sure the fucking scene is ready
	EnsureSceneBullshit();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LaserRender::Hide(
Handle handle)
{
  // Anything to see here
	if (!handle)
		return;

	int instanceIndex = (int)handle - 1;
	m_instanceList[instanceIndex].hidden = true;

	Lock();
	Hide(instanceIndex);
	Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Render the Shattered Glass pieces
///////////////////////////////////////////////////////////////////////////////
void LaserRender::Render(
const CInstance *engineInstance,
const Graphics4x4 *localToCamera,
float zCamDist)
{
	// Ouch
	if (!m_textureArray)
		return;

	// Get the cameraDir
	Camera &camera = g_cameraDirector.CurrentCamera();
	Vector3 cameraDir = camera.Dir();

	// Do it
	m_rayVertex.Lock();
	int visible = 0;
	for (int i = 0; i < m_maxInstances; i++)
	{
		if (Update(i, cameraDir))
			++visible;
	}
	m_rayVertex.UnLock();

	// anything to do?
	if (!visible)
		return;

	// Weee
	TextureMgr::Cache().Unlock();
	m_textureArray->Lock();

	// render it
	m_rayStrip.Render(*localToCamera, s_texMatrix, 0);
	m_dotStrip.Render(*localToCamera, s_texMatrix, 0);

	// Done
	m_textureArray->Unlock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LaserRender::BuildGeometry()
{
	// Hack up a bunch of this shizit
	m_textureLayers = 1;

	// Setup texture flags
	u32 texFlags[8];
	for (int t = 0; t < 8; t++)
	{
		if (t < m_textureLayers)
			texFlags[t] = VtxBuffer::VERT_TEX_DIM2;
		else
			texFlags[t] = 0;
	}

	// Dummy up the istance
	m_instance = new CInstance;
	ASSERT( m_instance );
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
	m_instance->SetName( "LaserRender" );

	// Assume alpha
	m_instance->SetAlphaSort( false );
	m_instance->SetZWriteDisabled( true );
	m_instance->SetReceiveShadows( false );
	m_instance->SetFogDisabled( true );

	// Make sure to disable lighting
	m_instance->SetDynamicallyLit( false );

	m_instance->SetRenderBucket( INSTANCE_RENDER_BUCKET_MAX - 2 );

	// calculate initial bounding data
	m_instance->UpdateBoundingVolumesAccurate();

	// Init the counts
	m_rayVertex.Init(m_maxInstances * 4, VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY, texFlags);

	// init our strip buffer
	m_rayStrip.Init();

#ifdef PS2
	// turn off the divide by 2 for PS2
	m_rayStrip.Overbrighting(true);
#endif

 	// set number layers to render
	m_rayStrip.SetNumTexturePasses(m_textureLayers);
	m_rayStrip.SetTextureArray(GetTextureArray(), 0);
	m_rayStrip.SetVtxBuffer(&m_rayVertex);

	// set texture
	m_rayStrip.SetPassTexture(0, GetTexture(0),
											TS_METHOD_MODULATE,
											TS_WRAP_CLAMP,
											TS_WRAP_CLAMP,
											TS_FILTER_BILINEAR,
											RS_BLENDFUNC_ADDITIVE);

	// Init the counts
	m_dotVertex.Init(m_maxInstances * 4, VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY, texFlags);

	// init our strip buffer
	m_dotStrip.Init();

#ifdef PS2
	// turn off the divide by 2 for PS2
	m_dotStrip.Overbrighting(true);
#endif

 	// set number layers to render
	m_dotStrip.SetNumTexturePasses(m_textureLayers);
	m_dotStrip.SetTextureArray(GetTextureArray(), 0);
	m_dotStrip.SetVtxBuffer(&m_dotVertex);

	// set texture
	m_dotStrip.SetPassTexture(0, GetTexture(1),
											TS_METHOD_MODULATE,
											TS_WRAP_CLAMP,
											TS_WRAP_CLAMP,
											TS_FILTER_BILINEAR,
											RS_BLENDFUNC_ADDITIVE);

	// Dummy the vertex buffer
	Lock();
	Vector3 origin(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < m_maxInstances; i++)
	{
		// Hide the vertices
		Hide(i);

		// create the strip
		u16 strip[4];
		strip[0] = i * 4;
		strip[1] = strip[0] + 1;
		strip[2] = strip[0] + 2;
		strip[3] = strip[0] + 3;

		// Add the strip
		m_rayStrip.AddStrip(strip, 4);
		m_dotStrip.AddStrip(strip, 4);
	}
	Unlock();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LaserRender::EnsureSceneBullshit()
{
	if (!m_addedToScene)
	{
		// add him to the scene
		SceneMgr::AddInstance(m_instance);
		SceneMgr::SetInstanceVisRegionStatic(m_instance,true);
		m_addedToScene = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Update an instance for rendering
///////////////////////////////////////////////////////////////////////////////
bool LaserRender::Update(
int instanceIndex,
Vector3CRef cameraDir)
{
	// Alias
	Instance &instance = m_instanceList[instanceIndex];

	// has this guy died?
	if (!instance.handle || instance.hidden)
		return false;

	// Get some stuff in camera space
	Vector3 basis = Vector3::Cross(instance.dir, cameraDir);
	basis.SafeNormalize();

	const Vector3 halfStart = basis * (m_rayRadiusStart * instance.inflate);
	const Vector3 halfEnd = basis * (m_rayRadiusEnd * instance.inflate);
	const Vector3Packed a = instance.end + halfEnd;
	const Vector3Packed b = instance.end - halfEnd;
	const Vector3Packed c = instance.start + halfStart;
	const Vector3Packed d = instance.start - halfStart;

	// Update the vertices for the ray
	int onVertex = instanceIndex * 4;

	m_rayVertex.Vertex(onVertex).Set(a);
	m_rayVertex.TexCoordv2(0, onVertex).Set(0.0f, 0.0f);
	m_rayVertex.Diffuse(onVertex) = VtxBuffer::cvtRGBA( instance.hit ? m_colorEnd : m_colorMiss );
	onVertex++;

	m_rayVertex.Vertex(onVertex).Set(b);
	m_rayVertex.TexCoordv2(0, onVertex).Set(1.0f, 0.0f);
	m_rayVertex.Diffuse(onVertex) = VtxBuffer::cvtRGBA( instance.hit ? m_colorEnd : m_colorMiss );
	onVertex++;

	m_rayVertex.Vertex(onVertex).Set(c);
	m_rayVertex.TexCoordv2(0, onVertex).Set(0.0f, 1.0f);
	m_rayVertex.Diffuse(onVertex) = VtxBuffer::cvtRGBA( m_colorStart );
	onVertex++;

	m_rayVertex.Vertex(onVertex).Set(d);
	m_rayVertex.TexCoordv2(0, onVertex).Set(1.0f, 1.0f);
	m_rayVertex.Diffuse(onVertex) = VtxBuffer::cvtRGBA( m_colorStart );
	onVertex++;

	// we are live
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Hide an instance
///////////////////////////////////////////////////////////////////////////////
void LaserRender::Hide(
int instanceIndex)
{
	static Vector3 origin(0.0f, 0.0f, 0.0f);
	static int clear = 0;

	int vertexBase = instanceIndex * 4;
	for (int i = 0; i < 4; i++, vertexBase++)
	{
		m_dotVertex.Vertex(vertexBase).Set(origin);
		m_dotVertex.Diffuse(vertexBase) = clear;
		m_rayVertex.Vertex(vertexBase).Set(origin);
		m_rayVertex.Diffuse(vertexBase) = clear;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Hide just the dot (does a lock)
///////////////////////////////////////////////////////////////////////////////
void LaserRender::HideDot(
int instanceIndex)
{
	static Vector3 origin(0.0f, 0.0f, 0.0f);
	static int clear = 0;

	int vertexBase = instanceIndex * 4;
	m_dotVertex.Lock();
	for (int i = 0; i < 4; i++, vertexBase++)
	{
		m_dotVertex.Vertex(vertexBase).Set(origin);
		m_dotVertex.Diffuse(vertexBase) = clear;
	}
	m_dotVertex.UnLock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LaserRender::PlaceDot(
int instanceIndex,
const Vector3Packed &center,
const Vector3Packed &up,
const Vector3Packed &right)
{
	float radius = m_dotRadius * m_instanceList[instanceIndex].inflate;
	Vector3Packed halfUp = up * radius;
	Vector3Packed halfRight = right * radius;

	int vertexBase = instanceIndex * 4;
	m_dotVertex.Lock();

	m_dotVertex.Vertex(vertexBase) = center + halfUp + halfRight;
	m_dotVertex.TexCoordv2(0, vertexBase).Set(0.0f, 0.0f);
	m_dotVertex.Diffuse(vertexBase++) = VtxBuffer::cvtRGBA(m_colorDot);

	m_dotVertex.Vertex(vertexBase) = center - halfUp + halfRight;
	m_dotVertex.TexCoordv2(0, vertexBase).Set(0.0f, 1.0f);
	m_dotVertex.Diffuse(vertexBase++) = VtxBuffer::cvtRGBA(m_colorDot);

	m_dotVertex.Vertex(vertexBase) = center + halfUp - halfRight;
	m_dotVertex.TexCoordv2(0, vertexBase).Set(1.0f, 0.0f);
	m_dotVertex.Diffuse(vertexBase++) = VtxBuffer::cvtRGBA(m_colorDot);

	m_dotVertex.Vertex(vertexBase) = center - halfUp - halfRight;
	m_dotVertex.TexCoordv2(0, vertexBase).Set(1.0f, 1.0f);
	m_dotVertex.Diffuse(vertexBase++) = VtxBuffer::cvtRGBA(m_colorDot);

	m_dotVertex.UnLock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LaserRender::LoadTextures(
const char *textureDBL)
{
	// Assume the worst
	m_texture = INVALID_HANDLE;
	m_textureDB = NULL;
	m_textureArray = NULL;
	m_textures = 0;

	// Load it
	if (textureDBL[0] == '\0')
		return false;

	// Load the dataset
	m_texture = dblLoad(textureDBL);
	if (m_texture == INVALID_HANDLE)
		return false;

	m_textureDB = dblGetTextures(m_texture, DEFAULT_CHUNK_KEY_STRING);
	m_textureArray = new TextureArray;
	m_textureArray->AddEntireDatabase(m_textureDB);
	m_textures = m_textureDB->TextureCount();

	return true;
}



