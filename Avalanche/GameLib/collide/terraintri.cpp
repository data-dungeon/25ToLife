/***************************************************************************/
// Terrain triangles
/***************************************************************************/
#include "collide/CollidePCH.h"
#include "collide/terraintri.h"

// the vertex groups for lean triangles
Vector3Packed *TerrainTriangleLean::s_vertexGroup[256] = { 0 };

/***************************************************************************/
// What is the major plane for this normal?
/***************************************************************************/
TerrainTriangle::MAJORPLANE TerrainTriangleBase::MajorPlane(
Vector3CRef normal)
{
	Vector3 absN = normal.Abs();

	// Which plane is best to project into?
	if (absN.x() > absN.y())
	{
		if (absN.x() > absN.z())
   		return(YZ);
		return(XY);
	}
	else if (absN.y() > absN.z())
		return(XZ);

	return(XY);
}

/***************************************************************************/
// What is the major axis for this normal?
/***************************************************************************/
TerrainTriangle::MAJORAXIS TerrainTriangleBase::MajorAxis(
Vector3CRef normal)
{
	Vector3 absN = normal.Abs();

	// Which plane is best to project into?
	if (absN.x() > absN.y())
	{
		if (absN.x() > absN.z())
		{
			if (normal.x() >= 0.0f)
				return(POS_X);
			return(NEG_X);
		}
		else if (normal.z() >= 0.0f)
   		return(POS_Z);
		return(NEG_Z);
	}
	else if (absN.y() > absN.z())
	{
		if (normal.y() >= 0.0f)
   		return(POS_Y);
		return(NEG_Y);
	}
	else if (normal.z() >= 0.0f)
		return(POS_Z);
	return(NEG_Z);
}

/***************************************************************************/
/***************************************************************************/
bool TerrainTriangleFat::PointInTriangle(
Vector3CRef point) const
{
#ifndef USE_BARYCENTRIC
	switch ((MAJORPLANE)(m_plane & 0x3))
	{
	 case XY:
		return TriangleTest::PointInTriangleXY(point, m_a, m_b, m_c);
	 case XZ:
		return TriangleTest::PointInTriangleXZ(point, m_a, m_b, m_c);
	}
	return TriangleTest::PointInTriangleYZ(point, m_a, m_b, m_c);
#else
	// This assumes that the point is in the triangle plane.
	Vector3 localPoint = point - m_a;
	Vector3 rightEdge = m_b - m_a;
	float rightArea = m_normal.Dot(rightEdge.Cross(localPoint));
	if (rightArea < 0.0f)
		return false;

	Vector3 leftEdge = m_c - m_a;
	float leftArea = m_normal.Dot(localPoint.Cross(leftEdge));
	return ((leftArea >= 0.0f) && ((rightArea + leftArea) <= m_normal.Dot(rightEdge.Cross(leftEdge))));
#endif
}

/***************************************************************************/
/***************************************************************************/
bool TerrainTriangleLean::PointInTriangle(
Vector3CRef point) const
{
	ASSERT(s_vertexGroup[m_vertexGroup] != NULL);
	Vector3CRef a = s_vertexGroup[m_vertexGroup][m_a];
	Vector3CRef b = s_vertexGroup[m_vertexGroup][m_b];
	Vector3CRef c = s_vertexGroup[m_vertexGroup][m_c];

#ifndef USE_BARYCENTRIC
	switch ((MAJORPLANE)(m_plane & 0x3))
	{
	 case XY:
		return TriangleTest::PointInTriangleXY(point, a, b, c);
	 case XZ:
		return TriangleTest::PointInTriangleXZ(point, a, b, c);
	}
	return TriangleTest::PointInTriangleYZ(point, a, b, c);
#else
	// This assumes that the point is in the triangle plane.
	Vector3 localPoint = point - a;
	Vector3 rightEdge = b - a;
	float rightArea = m_normal.Dot(rightEdge.Cross(localPoint));
	if (rightArea < 0.0f)
		return false;

	Vector3 leftEdge = c - a;
	float leftArea = m_normal.Dot(localPoint.Cross(leftEdge));
	return ((leftArea >= 0.0f) && ((rightArea + leftArea) <= m_normal.Dot(rightEdge.Cross(leftEdge))));
#endif
}

/***************************************************************************/
// Add/remove vertex groups
/***************************************************************************/
u8 TerrainTriangleLean::AddVertexGroup(
Vector3Packed *vertexGroup)
{
	for (uint i = 0; i < 256; i++)
	{
		if (!s_vertexGroup[i])
		{
			s_vertexGroup[i] = vertexGroup;
			return (u8)i;
		}
	}

	// We are out of vertex groups!!!!!!! THIS IS REALLY BAD!
	ASSERT(false);
	return 0;
}

/***************************************************************************/
// Add/remove vertex groups
/***************************************************************************/
bool TerrainTriangleLean::RemoveVertexGroup(
Vector3Packed *vertexGroup)
{
	for (uint i = 0; i < 256; i++)
	{
		if (s_vertexGroup[i] == vertexGroup)
		{
			s_vertexGroup[i] = NULL;
			return true;
		}
	}

	// They deleted an unknown vertex group!
	ASSERT(false);
	return false;
}

/***************************************************************************/
// Add/remove vertex groups
/***************************************************************************/
bool TerrainTriangleLean::RemoveVertexGroup(
u8 vertexGroup)
{
	if (s_vertexGroup[vertexGroup] == NULL)
		return false;
	else
	{
		s_vertexGroup[vertexGroup] = NULL;
		return true;
	}
}
