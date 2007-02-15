/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
inline ListOfTriangleLists::ListOfTriangleLists()
{
	m_onList = 0;
	m_onTriangle = 0;
}

/***************************************************************************/
// Reset the enumeration
/***************************************************************************/
inline void ListOfTriangleLists::ResetEnumeration()
{
	m_list.OpenArray();
	m_onList = 0;
	m_onTriangle = 0;
}

/***************************************************************************/
// Give me memory!
/***************************************************************************/
inline void ListOfTriangleLists::Initialize(
CCollisionStack *stack)
{
	m_list.InitArray(stack);
	m_onList = 0;
	m_onTriangle = 0;
}

/***************************************************************************/
/***************************************************************************/
inline void ListOfTriangleLists::Terminate()
{
	m_list.CloseArray();
}

/***************************************************************************/
// Add a triangle list
/***************************************************************************/
inline void ListOfTriangleLists::Add(
TerrainTriangle **triList,
int triangles)
{
	TerrainTriangleList* list = m_list.ExpandArray();
  	ASSERT(list != NULL);
	if (list != NULL)
	{
		list->tri = triList;
		list->count = triangles;
	}
}

/***************************************************************************/
// How many lists are there?
/***************************************************************************/
inline int ListOfTriangleLists::Lists()
{
	return m_list.NumEntries();
}

/***************************************************************************/
// Get the next triangle
/***************************************************************************/
inline TerrainTriangle *ListOfTriangleLists::GetNextTriangle(
u32 testID)
{
	// Are we done with all the lists?
	while (m_onList != m_list.NumEntries())
	{
		// Are we done with the current list
		if (m_onTriangle == m_list[m_onList].count)
		{
			++m_onList;
			if (m_onList == m_list.NumEntries())
			{
				m_list.CloseArray();
				return NULL;
			}
			m_onTriangle = 0;
		}

		// Get the next triangle
		TerrainTriangle *tri = m_list[m_onList].tri[m_onTriangle++];

		// Have we tried this one yet?
		if (!tri->MatchTestID(testID))
		{
			tri->SetTestID(testID);
			return tri;
		}
	}

	// We are done!
	m_list.CloseArray();
	return NULL;
}

/***************************************************************************/
/***************************************************************************/
inline AABB &OctTreeAABBIterator::Bounds()
{
	return m_bounds;
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle *OctTreeAABBIterator::GetNextTriangle(
u32 testID)
{
	return m_triList.GetNextTriangle(testID);
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeOBBIterator::Setup(
const AABB &aabb)
{
	m_aabb = aabb;
	m_obb = NULL;
	m_corner = NULL;
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeOBBIterator::Setup(
const AABB &aabb, 
const OBB &obb, 
const Vector3 *corners)
{
	m_aabb = aabb;
	m_obb = &obb;
	m_corner = corners;
}

/***************************************************************************/
/***************************************************************************/
inline TerrainTriangle *OctTreeOBBIterator::GetNextTriangle(
u32 testID)
{
	return m_triList.GetNextTriangle(testID);
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeLineIterator::Setup(
Vector3CRef start,
Vector3CRef end)
{
	m_start = start;
	m_end = end;
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeLineIterator::UpdateCenter(
Cube::Face exit)
{
	switch (exit)
	{
		case Cube::POSX:
			m_currentCenter.X(m_currentCenter.X() + m_leafSize);
			break;
		case Cube::NEGX:
			m_currentCenter.X(m_currentCenter.X() - m_leafSize);
			break;

		case Cube::POSY:
			m_currentCenter.Y(m_currentCenter.Y() + m_leafSize);
			break;
		case Cube::NEGY:
			m_currentCenter.Y(m_currentCenter.Y() - m_leafSize);
			break;

		case Cube::POSZ:
			m_currentCenter.Z(m_currentCenter.Z() + m_leafSize);
			break;
		case Cube::NEGZ:
			m_currentCenter.Z(m_currentCenter.Z() - m_leafSize);
			break;
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool OctTreeLineIterator::HaveValidEnd()
{
	return (m_haveEndPoint && m_leafStack.Top().m_octant.Inside(m_endPoint));
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeLineExactIterator::Setup(
Vector3CRef start,
Vector3CRef end)
{
	m_start = start;
	m_end = end;
}

/***************************************************************************/
/***************************************************************************/
inline void OctTreeLineExactIterator::UpdateCenter(
Cube::Face exit)
{
	switch (exit)
	{
		case Cube::POSX:
			m_currentCenter.X(m_currentCenter.X() + m_leafSize);
			break;
		case Cube::NEGX:
			m_currentCenter.X(m_currentCenter.X() - m_leafSize);
			break;

		case Cube::POSY:
			m_currentCenter.Y(m_currentCenter.Y() + m_leafSize);
			break;
		case Cube::NEGY:
			m_currentCenter.Y(m_currentCenter.Y() - m_leafSize);
			break;

		case Cube::POSZ:
			m_currentCenter.Z(m_currentCenter.Z() + m_leafSize);
			break;
		case Cube::NEGZ:
			m_currentCenter.Z(m_currentCenter.Z() - m_leafSize);
			break;
	}
}

/***************************************************************************/
/***************************************************************************/
inline bool OctTreeLineExactIterator::HaveValidEnd()
{
	return (m_haveEndPoint && m_leafStack.Top().m_octant.Inside(m_endPoint));
}


