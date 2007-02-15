///////////////////////////////////////////////////////////////////////////////
// Decal implementations
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Construct it!
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline StaticDecalEmitter<ExtraData>::StaticDecalEmitter(
DecalSet &set,
const DecalTextureLayer *layer,
int layers) :
DecalEmitter(set, layer, layers)
{
	// Vertex buffer locking
	m_locked = 0;

	// No instances yet
	m_decalList = NULL;

	// Setup the defaults
	const ts_bRGBA white = { 255, 255, 255, 255 };
	SetDefaultColor(white);
	UseDefaultUV();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline StaticDecalEmitter<ExtraData>::~StaticDecalEmitter()
{
	delete [] m_decalList;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::Setup(
int maxDecals,
int renderBucket,
bool overbrighting)
{
	ASSERT(maxDecals > 0);

	// Call the parent
	if (!DecalEmitter::Setup(maxDecals, renderBucket, overbrighting))
		return false;

	// Nothing is in use yet
	m_decalList = new PerDecal[maxDecals];
	ASSERT(m_decalList);
	for (int i = 0; i < maxDecals; i++)
		m_decalList[i].flags = 0;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Clear all instances
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::Clear()
{
	// do it!
	for (int i = 0; i < m_maxDecals; i++)
		Free(i);
}

///////////////////////////////////////////////////////////////////////////////
// Create a new instance
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline StaticDecalHandle StaticDecalEmitter<ExtraData>::Create(
const Vector3 corner[4],
const ts_bRGBA *color,
const Vector2 *uv)
{
	// Allocate an instance
	int decalIndex = Allocate();
	if (decalIndex < 0)
		return NULL;

	// Wee!
	EnsureSceneBullshit();

	// Setup our struct!
	PerDecal &decal = m_decalList[decalIndex];
	const ts_bRGBA white = { 255, 255, 255, 255 };
	for (int i = 0; i < 4; i++)
	{
		decal.corner[i] = corner[i];
		decal.color[i] = color ? color[i] : white;
		decal.uv[i] = uv ? uv[i] : m_defaultUV[i];
	}

	// Commit!
	Commit(decalIndex);

	// Give them the handle
	return IndexToHandle(decalIndex);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::SetDefaultColor(
const ts_bRGBA &c)
{
	for (int i = 0; i < 4; i++)
		m_defaultColor[i] = c;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::SetDefaultColors(
const ts_bRGBA c[4])
{
	for (int i = 0; i < 4; i++)
		m_defaultColor[i] = c[i];
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::UseDefaultUV()
{
	DefaultUV(m_defaultUV);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::SetDefaultUV(
const Vector2 uv[4])
{
	for (int i = 0; i < 4; i++)
		m_defaultUV[i] = uv[i];
}

///////////////////////////////////////////////////////////////////////////////
// Create a decal using all defaults
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline StaticDecalHandle StaticDecalEmitter<ExtraData>::Create(
const Vector3 corner[4])
{
	return Create(corner, m_defaultColor, m_defaultUV);
}

///////////////////////////////////////////////////////////////////////////////
// Free a decal!
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::Destroy(
StaticDecalHandle handle)
{
	int index = HandleToIndex(handle);
	if (index < 0)
		return false;
	return Free(index);
}

///////////////////////////////////////////////////////////////////////////////
// Update stuff about the decal (pass in four of each, or NULL for no change)
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::Update(
StaticDecalHandle handle,
const Vector3 *corner,
const Vector2 *uv,
const ts_bRGBA *color)
{
	int index = HandleToIndex(handle);
	if (index < 0)
		return false;

	PerDecal &decal = m_decalList[index];
	for (int i = 0; i < 4; i++)
	{
		if (corner)
			decal.corner[i] = corner[i];
		if (uv)
			decal.uv[i] = uv[i];
		if (color)
			decal.color[i] = color[i];
	}

	// Wee!
	Commit(index);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Hide/show the decal
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::Show(
StaticDecalHandle handle,
bool show)
{
	int index = HandleToIndex(handle);
	if (index < 0)
		return false;

	if (show)
	{
		if ((m_decalList[index].flags & DECAL_HIDDEN) != 0)
		{
			m_decalList[index].flags &= ~DECAL_HIDDEN;
			Commit(index);
		}
	}
	else
	{
		if ((m_decalList[index].flags & DECAL_HIDDEN) == 0)
		{
			m_decalList[index].flags |= DECAL_HIDDEN;
			Hide(index);
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::Hidden(
StaticDecalHandle handle)
{
	int index = HandleToIndex(handle);
	if (index < 0)
		return false;

	return (m_decalList[index].flags & DECAL_HIDDEN) != 0;
}

///////////////////////////////////////////////////////////////////////////////
// get an up if you don't know one
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline Vector3 StaticDecalEmitter<ExtraData>::GenerateUp(
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
// Create the corners from minimal info
// Returns false if collide is set and the corners are not coplanar
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::CreateCorners(
Vector3 corner[4],
Vector3CRef point,
Vector3CRef normal,
Vector3CRef up,
float width,
float height,
float heightTweak,
DecalCoplanarCheck coplanarCheck)
{
	// Make the right
	Vector3 right = Vector3::Cross(normal, up);
	right.Normalize();
	Vector3 realUp = Vector3::Cross(normal, right);

	// What is the point we are basing this on?
	Vector3 p = point + (normal * heightTweak);

	// Make the vertices
	Vector3 w = right * width * 0.5f;
	Vector3 h = realUp * height * 0.5f;

	// Here is the real work
	corner[0] = p - w + h;
	corner[1] = p + w + h;
	corner[2] = p - w - h;
	corner[3] = p + w - h;

	return CheckCoplanar(corner, normal, heightTweak, coplanarCheck);
}

///////////////////////////////////////////////////////////////////////////////
// Manually check if you are coplanar
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::CheckCoplanar(
const Vector3 corner[4],
Vector3CRef normal,
float scanDepth,
DecalCoplanarCheck coplanarCheck)
{
	// Are we doing a check?
	if (coplanarCheck == DECAL_COPLANARCHECK_NONE)
		return true;
	else // Do the real work
	{
		const float depth = 0.025f;
		return CheckCoplanar::Check(corner, -normal, heightTweak - depth, heightTweak + depth, false, check == DECAL_COPLANARCHECK_CHEAP);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Build uv's from a rotation angle
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::BuildRotatedUV(
float radians,
Vector2 uv[4])
{
	const Vector2 baseUV[4] =
	{
		Vector2(0.0f, 0.0f),
		Vector2(1.0f, 0.0f),
		Vector2(1.0f, 1.0f),
		Vector2(0.0f, 1.0f)
	};

	float cosTheta, sinTheta;
	Math::SinCos(radians, sinTheta, cosTheta);
	float a = (sinTheta * 0.5f - cosTheta * 0.5f);
	float b = -1.0f * (sinTheta * 0.5f + cosTheta * 0.5f);
	for (int i = 0; i < 4; i++)
	{
		uv[i].Set(cosTheta * baseUV[i].x() - sinTheta * baseUV[i].y() + a + 0.5f,
					 sinTheta * baseUV[i].x() + cosTheta * baseUV[i].y() + b + 0.5f);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Handy utility for turning a single color into 4 (only good for one call, as a static is returned)
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline ts_bRGBA *StaticDecalEmitter<ExtraData>::ExpandColor(
const ts_bRGBA color)
{
	static ts_bRGBA c[4];

	c[0] = c[1] = c[2] = c[3] = color;
	return c;
}

///////////////////////////////////////////////////////////////////////////////
// The render (don't touch this)
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::Render(
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
	if (m_set.GetTextureArray())
		m_set.GetTextureArray()->Lock();

	// render it
	m_stripBuffer.Render(*localToCamera, m_texMatrix, 0);

	// Done
	if (m_set.GetTextureArray())
		m_set.GetTextureArray()->Unlock();
}

///////////////////////////////////////////////////////////////////////////////
// Create and destory the geometry
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::CreateGeometry(
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
template <class ExtraData>
inline int StaticDecalEmitter<ExtraData>::Allocate()
{
	// Are we free
	if (m_decals == m_maxDecals)
		return -1;

	// Find a free one
	for (int i = 0; i < m_maxDecals; i++)
	{
		if ((m_decalList[i].flags & DECAL_ALLOCATED) == 0)
		{
			m_decalList[i].flags = DECAL_ALLOCATED;
			++m_decals;
			return i;
		}
	}

	// We shouldn't get here
	ASSERT(false);
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline bool StaticDecalEmitter<ExtraData>::Free(
int index)
{
	ASSERT(index >= 0 && index < m_maxDecals);
	if (index < 0 || index >= m_maxDecals)
		return false;

	// Hide it
	Hide(index);

	// Free it
	m_decalList[index].flags = 0;
	--m_decals;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Hide an instance
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::Hide(
int index)
{
	ASSERT(index >= 0 && index < m_maxDecals);

	int vertexBase = index * 4;
	Lock();
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	m_vertexBuffer.Vertex(vertexBase++).Set(c_origin);
	Unlock();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ExtraData>
inline void StaticDecalEmitter<ExtraData>::Commit(
int index)
{
	ASSERT(index >= 0 && index < m_maxDecals);

	// Wee!
	PerDecal &decal = m_decalList[index];

	// Anything to do?
	if (decal.flags & DECAL_HIDDEN)
		return;

	// Update the vertices
	const int startVertex = index * 4;
	Lock();

	m_vertexBuffer.Vertex(startVertex).Set(decal.corner[0]);
	m_vertexBuffer.Diffuse(startVertex) = VtxBuffer::cvtRGBA(*((int *)&decal.color[0]));

	m_vertexBuffer.Vertex(startVertex + 1).Set(decal.corner[1]);
	m_vertexBuffer.Diffuse(startVertex + 1) = VtxBuffer::cvtRGBA(*((int *)&decal.color[1]));

	m_vertexBuffer.Vertex(startVertex + 2).Set(decal.corner[2]);
	m_vertexBuffer.Diffuse(startVertex + 2) = VtxBuffer::cvtRGBA(*((int *)&decal.color[2]));

	m_vertexBuffer.Vertex(startVertex + 3).Set(decal.corner[3]);
	m_vertexBuffer.Diffuse(startVertex + 3) = VtxBuffer::cvtRGBA(*((int *)&decal.color[3]));

	// Do the texture coords
	for (int t = 0; t < m_textureLayers; t++)
	{
		m_vertexBuffer.TexCoordv2(t, startVertex).Set(decal.uv[0].X(), decal.uv[0].Y());
		m_vertexBuffer.TexCoordv2(t, startVertex + 1).Set(decal.uv[1].X(), decal.uv[1].Y());
		m_vertexBuffer.TexCoordv2(t, startVertex + 2).Set(decal.uv[2].X(), decal.uv[2].Y());
		m_vertexBuffer.TexCoordv2(t, startVertex + 3).Set(decal.uv[3].X(), decal.uv[3].Y());
	}

	Unlock();
}


