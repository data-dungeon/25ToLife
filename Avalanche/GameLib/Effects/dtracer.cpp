///////////////////////////////////////////////////////////////////////////////
// tracer.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif
#include "Display/Common/VtxBuffer.h"
#include "Effects/dtracer.h"

// Wee!
float DistortionTracer::s_uScale;
float DistortionTracer::s_vScale;

#if defined(PS2) || defined(_XBOX) || defined(DIRECTX_PC)
#define USEBACKBUFFER
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DistortionTracer::DistortionTracer(
TracerManager &manager,
TracerModel &model) :
Tracer(manager, model)
{
	// We only do these in groups of 3
	ASSERT((m_model.verts % 3) == 0);

	// Adjust the geometry
#ifdef PS2
	m_model.tailWidth *= 2.0f;
	m_model.headWidth *= 2.0f;
#endif

#ifdef USEBACKBUFFER
#ifdef DIRECTX_PC
	if (RenderToTexture::isRenderingToTextureOn)
		m_model.headColor.R = m_model.headColor.G = m_model.headColor.B = m_model.headColor.A = 255;
#else // DIRECTX_PC
	m_model.headColor.R = m_model.headColor.G = m_model.headColor.B = m_model.headColor.A = 255;
#endif // DIRECTX_PC
	m_model.ttl *= 3.0f;
#endif

#if 0
	m_model.ttl = 100000.0f;
	m_model.headWidth = 0.5f;
	m_model.tailWidth = 0.2f;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DistortionTracer::~DistortionTracer()
{
}

///////////////////////////////////////////////////////////////////////////////
// Prep textures
///////////////////////////////////////////////////////////////////////////////
void DistortionTracer::LockTextures()
{
#ifndef USEBACKBUFFER
	Tracer::LockTextures();
#else
#ifdef DIRECTX_PC
	if (!RenderToTexture::isRenderingToTextureOn)
		Tracer::LockTextures();
#else
	TextureMgr::GetBackBuffer()->Lock();
#endif // DIRECTX_PC
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void DistortionTracer::UnlockTextures()
{
#ifdef USEBACKBUFFER
#ifdef DIRECTX_PC
	if (!RenderToTexture::isRenderingToTextureOn)
		Tracer::UnlockTextures();
#else
	TextureMgr::GetBackBuffer()->Unlock();
#endif // DIRECTX_PC
#else
	Tracer::UnlockTextures();
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool DistortionTracer::CreateGeometry(
int instances)
{
	if (m_geometryCreated)
		return true;

	// Dummy up the istance
	m_instance = new CInstance;
	ASSERT(m_instance);
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

 	// set number layers to render
	m_stripBuffer.SetNumTexturePasses(1);
	m_stripBuffer.SetVtxBuffer(&m_vertexBuffer);

	// set texture
#ifdef USEBACKBUFFER
#ifdef DIRECTX_PC
	Texture* tex = NULL;
	if (RenderToTexture::isRenderingToTextureOn)
		tex = RenderToTexture::renderSurface;
	else
	{
		m_stripBuffer.SetTextureArray(m_parent.GetTextureArray(), 0);
		tex = m_parent.GetTexture(m_model.textureIndex);
	}
#else // DIRECTX_PC
	Texture *tex = TextureMgr::GetBackBuffer();
#endif // DIRECTX_PC
#else // USEBACKBUFFER
	m_stripBuffer.SetTextureArray(m_parent.GetTextureArray(), 0);
	Texture *tex = m_parent.GetTexture(m_model.textureIndex);
#endif // USEBACKBUFFER

	// Hack up a bunch of this shizit
	// TS_METHOD_PASSTHRU TS_METHOD_DECAL TS_METHOD_REPLACE TS_METHOD_MODULATE
	//	TS_METHOD_INTERPOLATE TS_METHOD_CONSTANTALPHA TS_METHOD_DOTPRODUCT,
	m_stripBuffer.SetPassTexture(0, tex,
#ifdef PS2
											TS_METHOD_REPLACE,
#else
											TS_METHOD_MODULATE,
#endif
											TS_WRAP_CLAMP,
											TS_WRAP_CLAMP,
											TS_FILTER_BILINEAR,
											RS_BLENDFUNC_DEFAULT);

	// How many verts in a strip
	m_stripVerts = (m_model.verts * 2) / 3;
	ASSERT(m_stripVerts < 32);
	u16 strip[32];
	Vector3 origin(0.0f, 0.0f, 0.0f);

	m_vertexBuffer.Lock();
	for (int i = 0; i < instances; i++)
	{
		// Hide the vertices
		Hide(i);

		// create the strips
		int baseOffset = i * m_model.verts;
		int edgeLength = m_stripVerts / 2;
		int edgeOffset = edgeLength;
		for (int s = 0; s < 2; s++)
		{
			int onVert = 0;
			for (int v = 0; v < edgeLength; v++)
			{
				strip[onVert++] = baseOffset + v;
				strip[onVert++] = baseOffset + v + edgeOffset;
			}

			// Add it!
			m_stripBuffer.AddStrip(strip, m_stripVerts);
			edgeOffset += edgeLength;
		}
	}
	m_vertexBuffer.UnLock();

	// add him to the scene
	SceneMgr::AddInstance(m_instance);
	SceneMgr::SetInstanceVisRegionStatic(m_instance, true);

	// Make sure these are set
#if defined(PS2) || defined(_XBOX)
	TextureMgr::GetBackBufferTextureScaling( s_uScale, s_vScale );
#elif defined(DIRECTX_PC)
	//TextureMgr::GetBackBufferTextureScaling( s_uScale, s_vScale );
	s_uScale = RenderToTexture::renderSurface->GetImage()->GetImageUScale();
	s_vScale = RenderToTexture::renderSurface->GetImage()->GetImageVScale();
#else
	s_uScale = 1.0f;
	s_vScale = 1.0f;
#endif

	// We are out of here!
	m_geometryCreated = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Create an instance
///////////////////////////////////////////////////////////////////////////////
void DistortionTracer::Create(
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
	ASSERT(m_stripVerts <= 32);
	Vector4 color[32];
	Vector2 uv[32];

	const Vector4 interior = ConvertColor(m_model.headColor);
	const Vector4 point(interior.X(), interior.Y(), interior.Z(), interior.W() * 0.5f);
	const Vector4 fade(255.0f, 255.0f, 255.0f, 0.0f);

	const int edgeLength = m_stripVerts / 2;
	const int leftEdgeOffset = edgeLength;
	const int rightEdgeOffset = 2 * edgeLength;

	const int headCenter = 0;
	const int headLeft = leftEdgeOffset;
	const int headRight = rightEdgeOffset;

	const int tailCenter = edgeLength - 1;
	const int tailLeft = leftEdgeOffset + edgeLength - 1;
	const int tailRight = rightEdgeOffset + edgeLength - 1;

	uv[headLeft].Set(0.0f, 0.0f);
	uv[headCenter].Set(0.0f, 0.5f);
	uv[headRight].Set(0.0f, 1.0f);
	color[headCenter] = point;
	color[headLeft] = color[headRight] = fade;

	uv[tailLeft].Set(m_model.textureWrap, 0.0f);
	uv[tailCenter].Set(m_model.textureWrap, 0.5f);
	uv[tailRight].Set(m_model.textureWrap, 1.0f);
	color[tailCenter] = point;
	color[tailLeft] = color[tailRight] = fade;

	// Due the intermediate vertices
	float dt = 1.0f / (float)(edgeLength - 1);
	float t = dt;
	for (int i = 1; i < edgeLength - 1; i++, t += dt)
	{
		uv[i] = Lerp(uv[headCenter], uv[tailCenter], t);
		color[i] = interior;

		uv[i + leftEdgeOffset] = Lerp(uv[headLeft], uv[tailLeft], t);
		color[i + leftEdgeOffset] = fade;

		uv[i + rightEdgeOffset] = Lerp(uv[headRight], uv[tailRight], t);
		color[i + rightEdgeOffset] = fade;
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
// some utility stuff
///////////////////////////////////////////////////////////////////////////////
inline Vector2 DistortionTracer::UVFromWorld(
const Camera &camera, 
const Vector3 &world)
{
	Vector2 screen = camera.ToScreen(world);
	return Vector2(screen.X(), screen.Y());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline Vector2 DistortionTracer::UVClamp(const Vector2 &uv)
{ 
#ifdef PS2
	Vector2 clamp(Math::Clamp(uv.X() * s_uScale, 0.0f, s_uScale), Math::Clamp(uv.Y() * s_vScale, 0.0f, s_vScale)); 
#else
#ifdef DIRECTX_PC
	Vector2 clamp(Math::Clamp(uv.X() * s_uScale, 0.0f, s_uScale), Math::Clamp(uv.Y() * s_vScale, 0.0f, s_vScale)); 
#else
	Vector2 clamp(uv.X() * s_uScale, uv.Y() * s_vScale); 
#endif
#endif
	return clamp; 
}

///////////////////////////////////////////////////////////////////////////////
// Update an instance for rendering
///////////////////////////////////////////////////////////////////////////////
bool DistortionTracer::Update(
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

	// Setup the stuff
	ASSERT(m_stripVerts <= 32);
	Vector3 vertex[32];
	Vector2 uv[32];

	const int edgeLength = m_stripVerts / 2;
	const int leftEdgeOffset = edgeLength;
	const int rightEdgeOffset = 2 * edgeLength;

	const int headCenter = 0;
	const int headLeft = leftEdgeOffset;
	const int headRight = rightEdgeOffset;

	const int tailCenter = edgeLength - 1;
	const int tailLeft = leftEdgeOffset + edgeLength - 1;
	const int tailRight = rightEdgeOffset + edgeLength - 1;

	vertex[headLeft] = instance.start + tail;
	uv[headLeft] = UVFromWorld(camera, vertex[headLeft]);

	vertex[headCenter] = instance.start;
	uv[headCenter] = UVFromWorld(camera, vertex[headCenter]);

	vertex[headRight] = instance.start - tail;
	uv[headRight] = UVFromWorld(camera, vertex[headRight]);

	vertex[tailLeft] = instance.end + head;
	uv[tailLeft] = UVFromWorld(camera, vertex[tailLeft]);

	vertex[tailCenter] = instance.end;
	uv[tailCenter] = UVFromWorld(camera, vertex[tailCenter]);

	vertex[tailRight] = instance.end- head;
	uv[tailRight] = UVFromWorld(camera, vertex[tailRight]);

	// Due the intermediate vertices
	float dt = 1.0f / (float)(edgeLength - 1);
	float t = dt;
	for (int i = 1; i < edgeLength - 1; i++, t += dt)
	{
		vertex[i] = Lerp(vertex[headCenter], vertex[tailCenter], t);
		uv[i] = UVFromWorld(camera, vertex[i]);

		vertex[i + leftEdgeOffset] = Lerp(vertex[headLeft], vertex[tailLeft], t);
		uv[i + leftEdgeOffset] = UVFromWorld(camera, vertex[i + leftEdgeOffset]);

		vertex[i + rightEdgeOffset] = Lerp(vertex[headRight], vertex[tailRight], t);
		uv[i + rightEdgeOffset] = UVFromWorld(camera, vertex[i + rightEdgeOffset]);
	}

	// Update the vertices
	const int vertexBase = instanceIndex * m_model.verts;
	const int omega = g_timer.GetFrameCount();
	static float scale = 0.01f;
	for (int v = 0; v < m_model.verts; v++)
	{
		const int onVertex = vertexBase + v;
		m_vertexBuffer.Vertex(onVertex) = vertex[v];
		m_vertexBuffer.TexCoordv2(0, onVertex) = UVClamp(uv[v] + RandomUVOffset(omega + m_uvOmega[v], scale));
	}

	// we are live
	return true;
}

