/***************************************************************************/
// Terrain triangles
/***************************************************************************/

/***************************************************************************/
// Access the test ID
/***************************************************************************/
inline void TerrainTriangleBase::ResetTestID()
{
	m_testID = 0;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleBase::SetTestID(
u32 newTestID)
{
	m_testID = newTestID;
}

/***************************************************************************/
/***************************************************************************/
inline bool TerrainTriangleBase::MatchTestID(
u32 test) const
{
	return (m_testID == test);
}

/***************************************************************************/
/***************************************************************************/
inline u32 TerrainTriangleBase::TestID() const
{
	return m_testID;
}

/***************************************************************************/
// Return the color
/***************************************************************************/
inline u16 TerrainTriangleBase::Color() const
{
	return m_color;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleBase::SetColor(
u16 color)
{
	m_color = color;
}

/***************************************************************************/
// Return the material
/***************************************************************************/
inline u8 TerrainTriangleBase::Material() const
{
	return(m_material);
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleBase::SetMaterial(
u8 material)
{
	m_material = material;
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangleBase::MAJORPLANE TerrainTriangleBase::MajorPlane() const
{
	return((MAJORPLANE)(m_plane & 0x3));
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleBase::ComputeMajorPlane(
Vector3CRef normal)
{
	m_plane = MajorPlane(normal);
}

/***************************************************************************/
// What is the "major" axis of projection for this triangle
/***************************************************************************/
inline TerrainTriangleBase::MAJORAXIS TerrainTriangleBase::MajorAxis() const
{
	return (MAJORAXIS)m_plane;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleBase::ComputeMajorAxis(
Vector3CRef normal)
{
	m_plane = MajorAxis(normal);
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleBase::DiskFixup()
{
#ifdef GCN
	ByteSwap(m_color);
#endif
}

/***************************************************************************/
// Return the vertex data
/***************************************************************************/
inline Vector3CRef TerrainTriangleFat::A() const
{
	return m_a;
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef TerrainTriangleFat::B() const
{
	return m_b;
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef TerrainTriangleFat::C() const
{
	return m_c;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleFat::SetVertices(
Vector3CRef a,
Vector3CRef b,
Vector3CRef c)
{
	m_a = a;
	m_b = b;
	m_c = c;
}

/***************************************************************************/
// Return the normal
/***************************************************************************/
inline Vector3CRef TerrainTriangleFat::FaceNormal() const
{
	return m_normal;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleFat::SetFaceNormal(
Vector3CRef normal)
{
	m_normal = normal;
}


/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleFat::DiskFixup()
{
#ifdef GCN
	m_normal.EndianSwap();
	m_a.EndianSwap();
	m_b.EndianSwap();
	m_c.EndianSwap();
#endif
	TerrainTriangleBase::DiskFixup();
}

#ifdef USE_BARYCENTRIC
/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleFat::SetupPointInTriangle(
Vector3 &rightEdge,
Vector3 &leftEdge,
float &area) const
{
	rightEdge = m_b - m_a;
	leftEdge = m_c - m_a;
	area = m_normal.Dot(rightEdge.Cross(leftEdge));
}

/***************************************************************************/
/***************************************************************************/
inline bool TerrainTriangleFat::PointInTriangle(
Vector3CRef point,
Vector3 &rightEdge,
Vector3 &leftEdge,
float &area) const
{
	// This assumes that the point is in the triangle plane.
	Vector3 localPoint = point - m_a;
	float rightArea = m_normal.Dot(rightEdge.Cross(localPoint));
	if (rightArea < 0.0f)
		return false;

	float leftArea = m_normal.Dot(localPoint.Cross(leftEdge));
	return ((leftArea >= 0.0f) && ((rightArea + leftArea) <= area));
}
#endif

/***************************************************************************/
// Return the vertex data
/***************************************************************************/
inline Vector3CRef TerrainTriangleLean::A() const
{
	ASSERT(s_vertexGroup[m_vertexGroup] != NULL);
	return s_vertexGroup[m_vertexGroup][m_a];
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef TerrainTriangleLean::B() const
{
	ASSERT(s_vertexGroup[m_vertexGroup] != NULL);
	return s_vertexGroup[m_vertexGroup][m_b];
}

/***************************************************************************/
/***************************************************************************/
inline Vector3CRef TerrainTriangleLean::C() const
{
	ASSERT(s_vertexGroup[m_vertexGroup] != NULL);
	return s_vertexGroup[m_vertexGroup][m_c];
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleLean::SetVertices(
uint a,
uint b,
uint c)
{
	m_a = a;
	m_b = b;
	m_c = c;
}

/***************************************************************************/
// Return the normal
/***************************************************************************/
inline Vector3CRef TerrainTriangleLean::FaceNormal() const
{
	return m_normal;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleLean::SetFaceNormal(
Vector3CRef normal)
{
	m_normal = normal;
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleLean::DiskFixup(
u8 vertexGroup)
{
#ifdef GCN
	ByteSwap(m_a);
	ByteSwap(m_b);
	m_normal.EndianSwap();
#endif
	TerrainTriangleBase::DiskFixup();
	SetVertexGroup(vertexGroup);
}

/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleLean::SetVertexGroup(
u8 vertexGroup)
{
	m_vertexGroup = vertexGroup;
}

#ifdef USE_BARYCENTRIC
/***************************************************************************/
/***************************************************************************/
inline void TerrainTriangleLean::SetupPointInTriangle(
Vector3 &rightEdge,
Vector3 &leftEdge,
float &area) const
{
	ASSERT(s_vertexGroup[m_vertexGroup] != NULL);

	rightEdge = s_vertexGroup[m_vertexGroup][m_b] - s_vertexGroup[m_vertexGroup][m_a];
	leftEdge = s_vertexGroup[m_vertexGroup][m_c] - s_vertexGroup[m_vertexGroup][m_a];
	area = FaceNormal().Dot(rightEdge.Cross(leftEdge));
}

/***************************************************************************/
/***************************************************************************/
inline bool TerrainTriangleLean::PointInTriangle(
Vector3CRef point,
Vector3 &rightEdge,
Vector3 &leftEdge,
float &area) const
{
	ASSERT(s_vertexGroup[m_vertexGroup] != NULL);

	// This assumes that the point is in the triangle plane.
	Vector3 localPoint = point - s_vertexGroup[m_vertexGroup][m_a];
	float rightArea = m_normal.Dot(rightEdge.Cross(localPoint));
	if (rightArea < 0.0f)
		return false;

	float leftArea = m_normal.Dot(localPoint.Cross(leftEdge));
	return ((leftArea >= 0.0f) && ((rightArea + leftArea) <= area));
}
#endif

