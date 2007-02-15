/***************************************************************************/
// Test ID's for oct trees
/***************************************************************************/

/***************************************************************************/
// Doh
/***************************************************************************/
inline OctTreeTestID::OctTreeTestID()
{
	m_testID = 0;
	m_triangle = NULL;
	m_triangles = 0;
}

/***************************************************************************/
// Create us!
/***************************************************************************/
inline void OctTreeTestID::Initialize(
TerrainTriangle *triList,
int triangles)
{
	m_testID = 0;
	m_triangle = triList;
	m_triangles = triangles;
}

/***************************************************************************/
// Get a new ID
/***************************************************************************/
inline u32 OctTreeTestID::GetNewID()
{
	// Increment it
	++m_testID;

	// Oh shit, wrap around?
	if (m_testID == 0)
	{
		// This could cause a burp in the frame rate!
		for (int t = 0; t < m_triangles; t++)
			m_triangle[t].ResetTestID();

		// Ready to hit it again!
		m_testID = 1;
	}

	return m_testID;
}

/***************************************************************************/
// Inspect the existing ID
/***************************************************************************/
inline u32 OctTreeTestID::CurrentID()
{
	return m_testID;
}

