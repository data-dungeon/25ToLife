///////////////////////////////////////////////////////////////////////////////
// tracer.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif //
#include "Display/Common/VtxBuffer.h"
#include "Effects/tracer.h"
#include "Effects/dtracer.h"
#include "camera/camera.h"

// Our singleton
TracerManager	*g_tracerManagerPtr = NULL;

// A lookup table that may be slower than sin/cos
Vector2 Tracer::s_offsetTable[UVOMEGA_MAX];

// Our texture array for fast rendering
Matrix4x4 Tracer::s_texMatrix[8];

bool Tracer::s_builtStatics = false;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Tracer::Tracer(
TracerManager &manager,
TracerModel &model) :
m_parent(manager),
m_model(model)
{
	// We only do these in groups of 2
	ASSERT((m_model.verts % 2) == 0);

	if (!s_builtStatics)
	{
  		// init all matrices to identity
		for (int m = 0; m < 8; m++)
			s_texMatrix[m].Identity();

		for (int i = 0; i < UVOMEGA_MAX; i++)
		{
			float s, c;
			Math::SinCos(Math::TwoPi * (float)i / (float)UVOMEGA_MAX, s, c);
			s_offsetTable[i].Set(s, c);
		}

		s_builtStatics = true;
	}

	// Make the uvOmega pool
	m_uvOmega = new int[m_model.verts];
	ASSERT(m_uvOmega);
	for (int v = 0; v < m_model.verts; v++)
		m_uvOmega[v] = g_random.UpToExcludeHigh(UVOMEGA_MAX);

	// Nothing is in use yet
	m_instanceList = new Instance[m_model.maxInstances];
	ASSERT(m_instanceList);
	for (int i = 0; i < m_model.maxInstances; i++)
		m_instanceList[i].inUse = false;

	// Adjust the geometry
	m_model.tailWidth *= 0.5f;
	m_model.headWidth *= 0.5f;

	// The scene manager sucks
	m_geometryCreated = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Tracer::~Tracer()
{
	// Get rid of all the info
	DestroyGeometry();
	delete [] m_instanceList;
	delete [] m_uvOmega;
}

///////////////////////////////////////////////////////////////////////////////
// Create a new instance
///////////////////////////////////////////////////////////////////////////////
bool Tracer::CreateInstanceRay(
Vector3CRef muzzle,
Vector3CRef dir)
{
	// Even do the work?
	if (g_random.UpToExcludeHigh(100) > m_model.percent)
		return false;

	// Find a free instance
	int instanceIndex = AllocateInstance();
	if (instanceIndex < 0)
		return false;

	// Fill it out
	float startD = g_random.InRange(m_model.minStartD, m_model.maxStartD);
	Vector3 start = muzzle + (dir * startD);

	float length = g_random.InRange(m_model.minLength, m_model.maxLength);
	Vector3 end = m_instanceList[instanceIndex].start + (dir * length);

	// Create it!
	Create(instanceIndex, start, end, dir);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Create a new instance
///////////////////////////////////////////////////////////////////////////////
bool Tracer::CreateInstanceChord(
Vector3CRef muzzle,
Vector3CRef hit)
{
	// Even do the work?
	if (g_random.UpToExcludeHigh(100) > m_model.percent)
		return false;

	// What is the length of the event
	Vector3 dir = hit - muzzle;
	float d = dir.Length();
	if (Math::Zero(d))
		return false;

	// Fill it out
	float startD = g_random.InRange(m_model.minStartD, m_model.maxStartD);
	if (startD > d)
		return false;

	float length = g_random.InRange(m_model.minLength, m_model.maxLength);
	if (startD + length > d)
		length = d - startD;

	// Find a free instance
	int instanceIndex = AllocateInstance();
	if (instanceIndex < 0)
		return false;

	dir /= d;
	Vector3 start = muzzle + (dir * startD);
	Vector3 end = start + (dir * length);

	// Create it!
	Create(instanceIndex, start, end, dir);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Render the Shattered Glass pieces
///////////////////////////////////////////////////////////////////////////////
void Tracer::Render(
const CInstance *engineInstance,
const Graphics4x4 *localToCamera,
float zCamDist)
{
	// update all the instances
	int toDraw = 0;
	float now = g_timer.GetEndSec();

	// Get the cameraDir
	Camera &camera = g_cameraDirector.CurrentCamera();

	m_vertexBuffer.Lock();
	for (int i = 0; i < m_model.maxInstances; i++)
	{
		// Update this bad boy
		if (Update(camera, i, now))
			++toDraw;
	}
	m_vertexBuffer.UnLock();

	// anything to do?
	if (!toDraw)
		return;

	// Weee
	TextureMgr::Cache().Unlock();
	LockTextures();

	// render it
	m_stripBuffer.Render(*localToCamera, s_texMatrix, 0);

	// Done
	UnlockTextures();
}

///////////////////////////////////////////////////////////////////////////////
// Prep textures
///////////////////////////////////////////////////////////////////////////////
void Tracer::LockTextures()
{
	m_parent.GetTextureArray()->Lock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tracer::UnlockTextures()
{
	m_parent.GetTextureArray()->Unlock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Tracer::CreateGeometry(
int instances)
{
	if (m_geometryCreated)
		return true;

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
	m_instance->InitFromDynamicObject(this);
	m_instance->SetName("tracer");

	m_instance->SetRenderBucket( INSTANCE_RENDER_BUCKET_MAX - 1 );

	// Assume alpha
	m_instance->SetAlphaSort(false);
	m_instance->SetZWriteDisabled(true);
	m_instance->SetReceiveShadows(false);
	m_instance->SetFogDisabled(true);
	m_instance->SetFogDisabled(true);

	// Make sure to disable lighting
	m_instance->SetDynamicallyLit(false);

	// calculate initial bounding data
	m_instance->UpdateBoundingVolumesAccurate();

	// Init the counts
	u32 vtxFlags = VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY;
	u32 texFlags[8] = { VtxBuffer::VERT_TEX_DIM2, 0, 0, 0, 0, 0, 0, 0 };
	m_vertexBuffer.Init(instances * m_model.verts, vtxFlags, texFlags);

	// init our strip buffer
	m_stripBuffer.Init();

	// turn off the divide by 2 for PS2
#ifdef PS2
	m_stripBuffer.Overbrighting(true);
#endif

 	// set number layers to render
	m_stripBuffer.SetNumTexturePasses(1);
	m_stripBuffer.SetTextureArray(m_parent.GetTextureArray(), 0);
	m_stripBuffer.SetVtxBuffer(&m_vertexBuffer);

	// set texture
	Texture *tex = m_parent.GetTexture(m_model.textureIndex);
	m_stripBuffer.SetPassTexture(0, tex,
											TS_METHOD_MODULATE,
											TS_WRAP_REPEAT,
											TS_WRAP_CLAMP,
											TS_FILTER_BILINEAR,
											RS_BLENDFUNC_DEFAULT);

	// Dummy the vertex buffer
	ASSERT(m_model.verts < 32);
	u16 strip[32];
	Vector3 origin(0.0f, 0.0f, 0.0f);

	m_vertexBuffer.Lock();
	for (int i = 0; i < instances; i++)
	{
		// Hide the vertices
		Hide(i);

		// create the strip
		int offset = i * m_model.verts;
		for (int v = 0; v < m_model.verts; v++)
			strip[v] = offset + v;

		// Add the strip
		m_stripBuffer.AddStrip(strip, m_model.verts);
	}
	m_vertexBuffer.UnLock();

	// add him to the scene
	SceneMgr::AddInstance(m_instance);
	SceneMgr::SetInstanceVisRegionStatic(m_instance, true);

	// We are out of here!
	m_geometryCreated = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tracer::DestroyGeometry()
{
	if (!m_geometryCreated)
		return;

	// remove the instance from scene
	if (m_instance)
		SceneMgr::RemoveInstance(m_instance);

	// Clear all the allocated thingies
	if (m_instance)
	{
		delete m_instance;
		m_instance = NULL;
	}

	m_geometryCreated = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int Tracer::AllocateInstance()
{
	// What is the best
	int best = -1;
	for (int i = 0; i < m_model.maxInstances; i++)
	{
		// Find an unsed one?
		if (!m_instanceList[i].inUse)
		{
			m_instanceList[i].inUse = true;
			return i;
		}
		else if (best < 0)
			best = i;
		else if (m_instanceList[i].death < m_instanceList[best].death)
			best = i;
	}

	// Overtakng one
	return best;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tracer::FreeInstance(
int instanceIndex)
{
	if (instanceIndex < 0)
		return;

	Hide(instanceIndex);
	m_instanceList[instanceIndex].inUse = false;
}

///////////////////////////////////////////////////////////////////////////////
// Create an instance
///////////////////////////////////////////////////////////////////////////////
void Tracer::Create(
int instanceIndex,
const Vector3 &start,
const Vector3 &end,
const Vector3 &dir)
{
	// Make sure we have geometry!
	if (!m_geometryCreated)
		CreateGeometry(m_model.maxInstances);

	// Alias
	Instance &instance = m_instanceList[instanceIndex];

	// Setup the basics
	instance.start = start;
	instance.end = end;
	instance.dir = dir;

	// Set the ttl
	instance.death = g_timer.GetEndSec() + m_model.ttl;

	// Setup the stuff
	ASSERT(m_model.verts <= 32);
	Vector4 color[32];
	Vector2 uv[32];

	const int head0 = 0;
	const int head1 = 1;
	const int tail2 = m_model.verts - 2;
	const int tail3 = m_model.verts - 1;

	uv[head0].Set(0.0f, 0.0f);
	color[head0] = ConvertColor(m_model.headColor);

	uv[head1].Set(0.0f, 1.0f);
	color[head1] = color[head0];

	uv[tail2].Set(m_model.textureWrap, 0.0f);
	color[tail2] = ConvertColor(m_model.tailColor);

	uv[tail3].Set(m_model.textureWrap, 1.0f);
	color[tail3] = color[tail2];

	// Due the intermediate vertices
	float dt = 1.0f / (float)((m_model.verts / 2) - 1);
	float t = dt;
	for (int i = head0 + 2; i < tail2; i += 2, t += dt)
	{
		uv[i] = Lerp(uv[head0], uv[tail2], t);
		color[i] = Lerp(color[head0], color[tail2], t);
		uv[i + 1] = Lerp(uv[head1], uv[tail3], t);
		color[i + 1] = Lerp(color[head1], color[tail3], t);
	}

	// Make the vertices
	m_vertexBuffer.Lock();
	int vertexBase = instanceIndex * m_model.verts;
	for (int v = 0; v < m_model.verts; v++)
	{
 		const int onVertex = vertexBase + v;
		m_vertexBuffer.TexCoordv2(0, onVertex) = uv[v];
		m_vertexBuffer.Diffuse(onVertex) = VtxBuffer::cvtRGBA(ConvertColor(color[v]));
	}
	m_vertexBuffer.UnLock();
}

///////////////////////////////////////////////////////////////////////////////
// Update an instance for rendering
///////////////////////////////////////////////////////////////////////////////
bool Tracer::Update(
const Camera &camera,
int instanceIndex,
float now)
{
	// Alias
	Instance &instance = m_instanceList[instanceIndex];

	// has this guy died?
	if (!instance.inUse)
		return false;

	// Done?
	if (instance.death <= now)
	{
		FreeInstance(instanceIndex);
		return false;
	}

	// Get some stuff in camera space
	Vector3 up = Vector3::Cross(instance.dir, camera.Dir());
	up.SafeNormalize();

	const Vector3 head = up * m_model.headWidth;
	const Vector3 tail = up * m_model.tailWidth;

	ASSERT(m_model.verts <= 32);
	Vector3Packed vertex[32];
	Vector2 uv[32];

	const int head0 = 0;
	const int head1 = 1;
	const int tail2 = m_model.verts - 2;
	const int tail3 = m_model.verts - 1;

	vertex[head0] = instance.end + head;
	vertex[head1] = instance.end - head;
	vertex[tail2] = instance.start + tail;
	vertex[tail3] = instance.start - tail;

	uv[head0].Set(0.0f, 0.0f);
	uv[head1].Set(0.0f, 1.0f);
	uv[tail2].Set(m_model.textureWrap, 0.0f);
	uv[tail3].Set(m_model.textureWrap, 1.0f);

	// Due the intermediate vertices
	float dt = 1.0f / (float)((m_model.verts / 2) - 1);
	float t = dt;
	for (int i = head0 + 2; i < tail2; i += 2, t += dt)
	{
		vertex[i] = Lerp(vertex[head0], vertex[tail2], t);
		vertex[i + 1] = Lerp(vertex[head1], vertex[tail3], t);
		uv[i] = Lerp(uv[head0], uv[tail2], t);
		uv[i + 1] = Lerp(uv[head1], uv[tail3], t);
	}

	// Update the vertices
	const int vertexBase = instanceIndex * m_model.verts;
	const int omega = g_timer.GetFrameCount();
	for (int v = 0; v < m_model.verts; v++)
	{
		const int onVertex = vertexBase + v;
		m_vertexBuffer.Vertex(onVertex) = vertex[v];
		m_vertexBuffer.TexCoordv2(0, onVertex) = uv[v] + RandomUVOffset(omega + m_uvOmega[v], m_model.wiggleScale);
	}

	// we are live
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Hide an instance
///////////////////////////////////////////////////////////////////////////////
void Tracer::Hide(
int instanceIndex)
{
	static Vector3 origin(0.0f, 0.0f, 0.0f);
	static int clear = 0;

	int vertexBase = instanceIndex * m_model.verts;
	for (int v = 0; v < m_model.verts; v++)
	{
		m_vertexBuffer.Vertex(v + vertexBase).Set(origin);
		m_vertexBuffer.Diffuse(v + vertexBase) = clear;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TracerManager::TracerManager(
const char *textureDBL)
{
	// Assume the worst
	m_texture = INVALID_HANDLE;
	m_textureDB = NULL;
	m_textureArray = NULL;
	m_textures = 0;

	// Load it
	if (textureDBL)
	{
		// Load the dataset
		m_texture = dblLoad(textureDBL);
		if (m_texture != INVALID_HANDLE)
		{
			m_textureDB = dblGetTextures(m_texture, DEFAULT_CHUNK_KEY_STRING);
			m_textureArray = new TextureArray();
			m_textureArray->AddEntireDatabase(m_textureDB);
			m_textures = m_textureDB->TextureCount();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TracerManager::~TracerManager()
{
 	m_texture = INVALID_HANDLE;
	m_textureDB = NULL;
	m_textures = 0;

	if (m_textureArray)
	{
		delete m_textureArray;
		m_textureArray = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Initialize and terminate the system
///////////////////////////////////////////////////////////////////////////////
bool TracerManager::Create(
const char *textureDBL)
{
	ASSERT(g_tracerManagerPtr == NULL);
	g_tracerManagerPtr = new TracerManager(textureDBL);
	if (!g_tracerManagerPtr)
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TracerManager::Destroy()
{
	delete g_tracerManagerPtr;
	g_tracerManagerPtr = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Create a tracer model
///////////////////////////////////////////////////////////////////////////////
Tracer *TracerManager::New(
TracerModel &init,
bool distortion)
{
	// Make this real simple
	if (!distortion)
	{
		init.verts = 8;
		return new Tracer(*this, init);
	}
	else
	{
#ifdef PS2
		init.verts = 12;
#else
		init.verts = 24;
#endif
		return new DistortionTracer(*this, init);
	}
}


