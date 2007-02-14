///////////////////////////////////////////////////////////////////////////////
// Tizag, beotch!
///////////////////////////////////////////////////////////////////////////////
#include "TTLPCH.h"

#if defined(PS2)
#define VTXBUFFER_NEED_INLINES
#endif
#include "Display/Common/VtxBuffer.h"
#include "camera/camera.h"
#include "Effects/tizag.h"

// Our texture array for fast rendering
Matrix4x4 Tizag::s_texMatrix[8];
bool Tizag::s_builtStatics = false;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Tizag::Tizag(
DecalSet &decalSet,
int textureIndex,
int xPoints,
int yPoints,
bool useSceneMgr) :
m_decalSet(decalSet),
m_textureIndex(textureIndex),
m_xPoints(xPoints),
m_yPoints(yPoints),
m_useSceneMgr(useSceneMgr)
{
	// Make sure we build this crizap
	if (!s_builtStatics)
	{
  		// init all matrices to identity
		for (int m = 0; m < 8; m++)
			s_texMatrix[m].Identity();
		s_builtStatics = true;
	}

	// Not visible yet
	m_visible = false;

	// The scene manager sucks
	m_instance = NULL;
	m_geometryCreated = false;
	m_normal.Set(0.0f, 1.0f, 0.0f);

	// No fade yet
	ClearFade();

	// Make the geometry now
	if (!m_useSceneMgr)
		CreateGeometry();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Tizag::~Tizag()
{
	// Get rid of all the info
	DestroyGeometry();
}

///////////////////////////////////////////////////////////////////////////////
// Create a new instance (does a show)
///////////////////////////////////////////////////////////////////////////////
bool Tizag::Setup(
const Vector3 &pos,
const Vector3 &normal,
const Vector3 &up,
float width,
float height,
float paintTime,
float finalAlpha)
{
	// Make sure the geometry is around
	if (!CreateGeometry())
		return false;

	// Make an orthonormal basis
	Vector3 right = Vector3::Cross(up, normal);
	right.SafeNormalize();
	Vector3 orthoUp = Vector3::Cross(right, normal);

	// Setup the instance matrix
	ASSERT(m_instance);
	Graphics4x4 &matrix = m_instance->Matrix();
	matrix.Identity();
	matrix.SetRow0(right * width);
	matrix.SetRow1(orthoUp * height);
	matrix.SetRow2(normal);
	m_normal = normal;
	matrix.SetRow3(pos);

	// Start the fade in
	StartFade(paintTime, finalAlpha);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set a new position!
///////////////////////////////////////////////////////////////////////////////
bool Tizag::SetPosition(const Vector3 &pos)
{
 	ASSERT(m_instance);
	if (!m_instance)
		return false;

	Graphics4x4 &matrix = m_instance->Matrix();
	matrix.SetRow3(pos);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set a new position!
///////////////////////////////////////////////////////////////////////////////
bool Tizag::ShiftOffset(float newOffset)
{
 	ASSERT(m_instance);
	if (!m_instance)
		return false;

	Graphics4x4 &matrix = m_instance->Matrix();
	Vector3 currentPos = matrix.GetRow3();
	matrix.SetRow3(currentPos + m_normal * newOffset);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Render the Shattered Glass pieces
///////////////////////////////////////////////////////////////////////////////
void Tizag::Render(
const CInstance *engineInstance,
const Graphics4x4 *localToCamera,
float zCamDist)
{
	ASSERT(m_decalSet.GetTextureArray());

	if (m_visible && m_geometryCreated)
	{
		// Update the alpha
		UpdateFade();

		// Weee
		TextureMgr::Cache().Unlock();
		m_decalSet.GetTextureArray()->Lock();

		// render it
		m_stripBuffer.Render(*localToCamera, s_texMatrix, 0);

		// Done
		m_decalSet.GetTextureArray()->Unlock();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tizag::ChangePaintTime(float newPaintTime)
{
	// Are we already done?
	if (Math::Zero(m_fadeRate))
		return;

	// Don't allow it to be zero
	if (Math::Zero(newPaintTime))
		newPaintTime = 0.01f;

	// Set this
	m_fadeRate = (float)((m_xPoints - 1) * m_yPoints) / newPaintTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Tizag::CreateGeometry()
{
	if (m_geometryCreated)
		return true;

	// Make the instance
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

	m_instance->InitFromDynamicObject(this);
	m_instance->SetName("tizag");

	m_instance->SetRenderBucket(INSTANCE_RENDER_BUCKET_MAX - 1);

	// Assume alpha
	m_instance->SetAlphaSort(true);
	m_instance->SetZWriteDisabled(true);
	m_instance->SetReceiveShadows(false);

	// Make sure to disable lighting
	m_instance->SetDynamicallyLit(false);
	m_instance->SetFogDisabled(true);

	// calculate initial bounding data
	m_instance->UpdateBoundingVolumesAccurate();

	// Init the counts
	u32 vtxFlags = VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY;
	u32 texFlags[8] = { VtxBuffer::VERT_TEX_DIM2, 0, 0, 0, 0, 0, 0, 0 };
	m_vertexBuffer.Init(m_xPoints * m_yPoints, vtxFlags, texFlags);

	// init our strip buffer
	m_stripBuffer.Init();

 	// set number layers to render
	m_stripBuffer.SetNumTexturePasses(1);
	m_stripBuffer.SetTextureArray(m_decalSet.GetTextureArray(), 0);
	m_stripBuffer.SetVtxBuffer(&m_vertexBuffer);

	// set texture
	Texture *tex = m_decalSet.GetTexture(m_textureIndex);
	ASSERT(tex);
	m_stripBuffer.SetPassTexture(0, tex,
											TS_METHOD_MODULATE,
											TS_WRAP_CLAMP,
											TS_WRAP_CLAMP,
											TS_FILTER_BILINEAR,
											RS_BLENDFUNC_DEFAULT);

	// Setup the vertex buffer
	m_vertexBuffer.Lock();
	int v = 0;
	float px = -0.5f;
	const float pxStep = 1.0f / (float)(m_xPoints - 1);
	for (int x = 0; x < m_xPoints; x++, px += pxStep)
	{
		float py = -0.5f;
		const float pyStep = 1.0f / (float)(m_yPoints - 1);
		for (int y = 0; y < m_yPoints; y++, py += pyStep, v++)
		{
	 		// Hide the vertices
			ASSERT(v < m_xPoints * m_yPoints);
			Vector3 pos(px, py, 0.0f);
			m_vertexBuffer.Vertex(v).Set(pos);
			m_vertexBuffer.TexCoordv2(0, v) = GetUV(x, y);
			//SetVertexAlpha(v, 1.0f);
		}
	}

	// Make the strips
	int strips = m_xPoints - 1;
	int baseVertex = 0;
#define MAX_STRIP_YPOINTS		128
	ASSERT(m_yPoints * 2 < MAX_STRIP_YPOINTS);
	u16 strip[MAX_STRIP_YPOINTS];
	for (int s = 0; s < strips; s++, baseVertex += m_yPoints)
	{
		// Create the strip
		int onV = 0;
		for (int add = 0; add < m_yPoints; add++, onV += 2)
		{
			ASSERT(onV < m_yPoints * 2);
			strip[onV] = baseVertex + add;
			ASSERT(onV + 1 < m_yPoints * 2);
			strip[onV + 1] = strip[onV] + m_yPoints;
		}

		// Add the strip
		m_stripBuffer.AddStrip(strip, m_yPoints * 2);
	}
	m_vertexBuffer.UnLock();
#undef MAX_STRIP_YPOINTS

	// add him to the scene
	if (m_useSceneMgr)
	{
		SceneMgr::AddInstance(m_instance);
		SceneMgr::SetInstanceVisRegionStatic(m_instance, true);
	}

	// We are out of here!
	m_geometryCreated = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tizag::DestroyGeometry()
{
	if (m_geometryCreated)
	{
		// remove the instance from scene
		if (m_instance)
		{
			if (m_useSceneMgr)
				SceneMgr::RemoveInstance(m_instance);
			delete m_instance;
			m_instance = NULL;
		}

		m_geometryCreated = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tizag::ClearFade()
{
	m_fadeX = m_fadeY = 0;
	m_fadeAlpha = 0.0f;
	m_fadeTarget = 1.0f;
	m_fadeRate = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tizag::StartFade(
float fadeTime,
float finalAlpha)
{
	m_vertexBuffer.Lock();
	m_fadeTarget = finalAlpha;

	if (Math::Zero(fadeTime))
	{
		m_fadeX = m_xPoints;
		m_fadeY = m_yPoints;
		m_fadeRate = 0.0f;

		// Start the first column at full alpha, all others at zero
		for (int x = 0; x < m_xPoints; x++)
			for (int y = 0; y < m_yPoints; y++)
				SetVertexAlpha(x, y, m_fadeTarget);
	}
	else
	{
		m_fadeX = 1;
		m_fadeY = 0;
		m_fadeRate = (float)((m_xPoints - 1) * m_yPoints) / fadeTime;

		// Start the first column at full alpha, all others at zero
		for (int x = 0; x < m_xPoints; x++)
			for (int y = 0; y < m_yPoints; y++)
				SetVertexAlpha(x, y, x == 0 ? m_fadeTarget : 0.0f);
	}

	m_vertexBuffer.UnLock();
	m_visible = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tizag::UpdateFade()
{
	// Are we fading?
	if (Math::Zero(m_fadeRate) || Math::Zero(m_fadeTarget))
		return;

	float dt = g_timer.GetFrameSec();
	while (dt > 0.0f)
	{
		// Do the fade for the current pint
		float moveFade = dt * m_fadeRate * m_fadeTarget;
		float alphaDiff = m_fadeTarget - m_fadeAlpha;
		if (moveFade <= alphaDiff)
		{
			m_fadeAlpha += moveFade;
			dt = 0.0f;
		}
		else
		{
			m_fadeAlpha = m_fadeTarget;
			moveFade -= alphaDiff;
			dt = moveFade / (m_fadeRate * m_fadeTarget);
		}

		m_vertexBuffer.Lock();
		SetVertexAlpha(m_fadeX, m_fadeY, m_fadeAlpha);
		m_vertexBuffer.UnLock();

		// This point done?
		if (Math::Equal(m_fadeAlpha, m_fadeTarget))
		{
			// Next point
			m_fadeAlpha = 0.0f;
			bool even = ((m_fadeX & 1) == 0);
			if (even)
			{
				if (--m_fadeY < 0)
				{
					m_fadeY = 0;
					++m_fadeX;
				}
			}
			else
			{
				if (++m_fadeY == m_yPoints)
				{
					m_fadeY = m_yPoints - 1;
					++m_fadeX;
				}
			}

			// All done?
			if (m_fadeX == m_xPoints)
			{
				m_fadeRate = 0.0f;
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int Tizag::VertexIndex(
int x,
int y)
{
	ASSERT(x >= 0 && x < m_xPoints);
	ASSERT(y >= 0 && y < m_yPoints);
	int v = x * m_yPoints + y;
	ASSERT(v >= 0 && v < m_xPoints * m_yPoints);
	return v;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Vector2 Tizag::GetUV(
int x,
int y)
{
	return Vector2((float)x / (float)(m_xPoints - 1), (float)y / (float)(m_yPoints - 1));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Tizag::SetVertexAlpha(
int v,
float alpha)
{
	ts_bRGBA white = { 255, 255, 255, 0 };
	white.A = (int)Math::Clamp(alpha * 255.0f, 0.0f, 255.0f);

	ASSERT(v >= 0 && v < m_xPoints * m_yPoints);
	m_vertexBuffer.Diffuse(v) = VtxBuffer::cvtRGBA(white);
}


