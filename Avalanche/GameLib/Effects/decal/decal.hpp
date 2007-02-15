///////////////////////////////////////////////////////////////////////////////
// Decal implementations
///////////////////////////////////////////////////////////////////////////////
#include "CollideHelper/ccoplanar.h"
#include "camera/director.h"

///////////////////////////////////////////////////////////////////////////////
// Construct it!
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline AutomaticDecalEmitter<PerDecal>::AutomaticDecalEmitter(
DecalSet &set,
const DecalTextureLayer *layer,
int layers) :
DecalEmitter(set, layer, layers)
{
	// Vertex buffer locking
	m_locked = false;

	// No instances yet
	m_decalList = NULL;

	// Setup the defaults
	SetDefaultHeightTweak(0.01f);
	const ts_bRGBA white = { 255, 255, 255, 255 };
	SetDefaultColor(white);
	UseDefaultUV();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline AutomaticDecalEmitter<PerDecal>::~AutomaticDecalEmitter()
{
	delete [] m_decalList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::Setup(
int maxDecals,
int renderBucket,
bool overbrighting)
{
	// Call the parent
	if (!DecalEmitter::Setup(maxDecals, renderBucket, overbrighting))
		return false;

	// Nothing is in use yet
	m_decalList = new PerDecal[maxDecals];
	ASSERT(m_decalList);
	m_head = m_tail = 0;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Clear all instances
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::Clear()
{
	// do it!
	while (m_decals > 0)
		FreeDecal();
}

///////////////////////////////////////////////////////////////////////////////
// Will this decal work?
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::CheckCoplanar(
Vector3CRef point,
Vector3CRef normal,
Vector3CRef up,
float halfWidth,
float halfHeight,
float heightTweak,
DecalCoplanarCheck check)
{
	// duh
	if (check == DECAL_COPLANARCHECK_NONE)
		return true;

	// Make the right
	Vector3Packed right = Vector3::Cross(normal, up);
	right.Normalize();
	Vector3Packed realUp = Vector3::Cross(normal, right);

	// What is the point we are basing this on?
	Vector3Packed p = point + (normal * heightTweak);

	// Make the vertices
	Vector3Packed w = right * halfWidth;
	Vector3Packed h = realUp * halfHeight;

	Vector3Packed corner[4];
	corner[0] = p - w + h;
	corner[1] = p + w + h;
	corner[2] = p - w - h;
	corner[3] = p + w - h;

	// Make sure this is kosher
	return CheckCoplanar(corner, normal, heightTweak, check);
}

///////////////////////////////////////////////////////////////////////////////
// Create a new instance
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::Create(
Vector3CRef point,
Vector3CRef normal,
Vector3CRef up,
float halfWidth,
float halfHeight,
float heightTweak,
ts_bRGBA color[4],
Vector2 uv[4],
DecalCoplanarCheck check)
{
	// Make the right
	Vector3Packed right = Vector3::Cross(normal, up);
	right.Normalize();
	Vector3Packed realUp = Vector3::Cross(normal, right);

	// What is the point we are basing this on?
	Vector3Packed p = point + (normal * heightTweak);

	// Make the vertices
	Vector3Packed w = right * halfWidth;
	Vector3Packed h = realUp * halfHeight;

	Vector3Packed corner[4];
	corner[0] = p - w + h;
	corner[1] = p + w + h;
	corner[2] = p - w - h;
	corner[3] = p + w - h;

	// Make sure this is kosher
	if (!CheckCoplanar(corner, normal, heightTweak, check))
		return false;

	// scene
	EnsureSceneBullshit();

	// Allocate an instance
	int decal = AllocateDecal();
	if (decal < 0)
		return false;

	// Tell them about it
	OnCreate(decal, point, normal, up, halfWidth, halfHeight, heightTweak, color, uv);

	// Update the vertices
	Lock();
	SetupQuad(decal, corner, uv, color);
	Unlock();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Set some defaults
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::SetDefaultHeightTweak(
float heightTweak)
{
	m_defaultHeightTweak = heightTweak;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::SetDefaultColor(
const ts_bRGBA &c)
{
	for (int i = 0; i < 4; i++)
		m_defaultColor[i] = c;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::SetDefaultColors(
const ts_bRGBA c[4])
{
	for (int i = 0; i < 4; i++)
		m_defaultColor[i] = c[i];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::UseDefaultUV()
{
	DefaultUV(m_defaultUV);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::SetDefaultUV(
const Vector2 uv[4])
{
	for (int i = 0; i < 4; i++)
		m_defaultUV[i] = uv[i];
}

///////////////////////////////////////////////////////////////////////////////
// Create a decal using all defaults
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::Create(
Vector3CRef point,
Vector3CRef normal,
Vector3CRef up,
float halfWidth,
float halfHeight)
{
	return Create(point, normal, up, halfWidth, halfHeight,
						m_defaultHeightTweak, m_defaultColor, m_defaultUV);
}

///////////////////////////////////////////////////////////////////////////////
// Create a decal using all defaults and computed up
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::Create(
Vector3CRef point,
Vector3CRef normal,
float halfWidth,
float halfHeight)
{
	Vector3 up = GenerateUp(normal);
	return Create(point, normal, up, halfWidth, halfHeight);
}

///////////////////////////////////////////////////////////////////////////////
// get an up if you don't know one
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline Vector3 AutomaticDecalEmitter<PerDecal>::GenerateUp(
Vector3CRef normal)
{
	// Compute the coordinate system
	Vector3 up(0.0f, 1.0f, 0.0f);
	if (Math::Zero(Vector3::Cross(up, normal).LengthSquared()))
	{
		up.Set(1.0f, 0.0f, 0.0f);
		if (Math::Zero(Vector3::Cross(up, normal).LengthSquared()))
			up.Set(0.0f, 0.0f, 1.0f);
	}

	return up;
}

///////////////////////////////////////////////////////////////////////////////
// The render (don't touch this)
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::Render(
const CInstance *instance,
const Graphics4x4 *localToCamera,
float zCamDist)
{
	// update all the instances
	Update(*instance, *localToCamera, zCamDist);
	if (m_decals == 0)
		return;

	// Setup T factor for bump mapping
	if (m_hasBump)
		SetTFactor();

	// Weee
	TextureMgr::Cache().Unlock();
	m_set.GetTextureArray()->Lock();

	// render it
	m_stripBuffer.Render(*localToCamera, m_texMatrix, 0);

	// Done
	m_set.GetTextureArray()->Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Create and destory the geometry
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::CreateGeometry(
bool overbrighting)
{
	// Init the counts
	u32 vtxFlags = VtxBuffer::VERT_HAS_DIFFUSE | VtxBuffer::VERT_WRITEONLY;
	m_vertexBuffer.Init(m_maxDecals * 4, vtxFlags, m_texFlags);

	// init our strip buffer
	m_stripBuffer.Init();

#ifdef PS2
	// turn off the divide by 2 for PS2
	m_stripBuffer.Overbrighting(overbrighting);
#endif

	// Setup the strip buffer
	m_stripBuffer.SetVtxBuffer(&m_vertexBuffer);
	SetupStripTextures(m_stripBuffer, overbrighting);

	// Dummy the vertex buffer
	Lock();

	Vector3 origin(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < m_maxDecals; i++)
	{
		// Hide it
		Hide(i);

		// create the strip
		u16 strip[4];
		strip[0] = i * 4;
		strip[1] = strip[0] + 1;
		strip[2] = strip[0] + 2;
		strip[3] = strip[0] + 3;

		// Add the strip
		m_stripBuffer.AddStrip(strip, 4);
	}
	Unlock();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline int AutomaticDecalEmitter<PerDecal>::AllocateDecal()
{
	// If we are full, delete the oldest
	if (m_decals == m_maxDecals)
		FreeDecal();

	// Give them the instance
	int use = m_head;
	++m_decals;
	m_head = NextDecal(m_head);
	return use;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::FreeDecal()
{
	Lock();
	Hide(m_tail);
	m_tail = NextDecal(m_tail);
	--m_decals;
	Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Setup a vertex
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::SetupQuad(
int quad,
const Vector3Packed corner[4],
const Vector2 uv[4],
const ts_bRGBA color[4])
{
	const int startVert = quad * 4;

	m_vertexBuffer.Vertex(startVert).Set(corner[0]);
	m_vertexBuffer.Diffuse(startVert) = VtxBuffer::cvtRGBA(*((int *)&color[0]));

	m_vertexBuffer.Vertex(startVert + 1).Set(corner[1]);
	m_vertexBuffer.Diffuse(startVert + 1) = VtxBuffer::cvtRGBA(*((int *)&color[1]));

	m_vertexBuffer.Vertex(startVert + 2).Set(corner[2]);
	m_vertexBuffer.Diffuse(startVert + 2) = VtxBuffer::cvtRGBA(*((int *)&color[2]));

	m_vertexBuffer.Vertex(startVert + 3).Set(corner[3]);
	m_vertexBuffer.Diffuse(startVert + 3) = VtxBuffer::cvtRGBA(*((int *)&color[3]));

	// Do the texture coords

	for (int t = 0; t < m_textureLayers; t++)
	{
		m_vertexBuffer.TexCoordv2(t, startVert).Set(uv[0].X(), uv[0].Y());
		m_vertexBuffer.TexCoordv2(t, startVert + 1).Set(uv[1].X(), uv[1].Y());
		m_vertexBuffer.TexCoordv2(t, startVert + 2).Set(uv[2].X(), uv[2].Y());
		m_vertexBuffer.TexCoordv2(t, startVert + 3).Set(uv[3].X(), uv[3].Y());
	}
}

///////////////////////////////////////////////////////////////////////////////
// Hide an instance
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline void AutomaticDecalEmitter<PerDecal>::Hide(
int decal)
{
	int vertexBase = decal * 4;
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class PerDecal>
inline bool AutomaticDecalEmitter<PerDecal>::CheckCoplanar(
const Vector3Packed corner[4],
Vector3CRef normal,
float heightTweak,
DecalCoplanarCheck check)
{
	// Are we doing a check?
	if (check == DECAL_COPLANARCHECK_NONE)
		return true;
	else // do it
	{
		const float depth = 0.025f;
		return CheckCoplanar::Check(corner, -normal, heightTweak - depth, heightTweak + depth, false, check == DECAL_COPLANARCHECK_CHEAP);
	}
}
