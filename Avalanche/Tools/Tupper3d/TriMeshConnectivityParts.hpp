////////////////////////////////////////////////////////////////////////////
//
// TriMeshConnectivityParts
//
// Class to handle the parts of the TriMeshConnectivity class
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TriMeshConnectivityParts.hpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 8/08/03    Time: 4:59p
 * Updated in $/Avalanche/tools/Tupper3d
 * added connection changes
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/01/03    Time: 10:42a
 * Updated in $/Avalanche/tools/Tupper3d
 * moved tri mesh connectivity from plugins library
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 6/02/03    Time: 4:09p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * first pass of optimize uvs
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/29/03    Time: 4:11p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * additional access for optimize uvs
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/05/02    Time: 4:14p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added finding all connected faces
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/03/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * mesh connectivity added
 */

////////////////////////////////////////////////////////////////////////////

// Vertex
inline bool TupTriMeshConnectivity::Vertex::Compare(Vertex &vertex,float threshold)
{
	Vertex delta;
	delta.x = x-vertex.x;
	delta.y = y-vertex.y;
	delta.z = z-vertex.z;
	float diff = delta.x*delta.x+delta.y*delta.y+delta.z*delta.z;
	return (diff<(threshold*threshold));
}

// Edge
inline TupTriMeshConnectivity::Edge::Edge(void) : m_faceIndex(-1), m_pointIndex(-1) { }
inline TupTriMeshConnectivity::Edge::Edge(int faceIndex,int pointIndex) : m_faceIndex(faceIndex), m_pointIndex(pointIndex) { }
inline int TupTriMeshConnectivity::Edge::GetFaceIndex(void) const { return m_faceIndex; }
inline int TupTriMeshConnectivity::Edge::GetPointIndex(void) const { return m_pointIndex; }
inline void TupTriMeshConnectivity::Edge::SetFaceIndex(int faceIndex) { m_faceIndex=faceIndex; }
inline void TupTriMeshConnectivity::Edge::SetPointIndex(int pointIndex) { m_pointIndex = pointIndex; }

// TriFace
inline TupTriMeshConnectivity::TriFace::TriFace(void) { }
inline const TupTriMeshConnectivity::Edge &TupTriMeshConnectivity::TriFace::GetEdge(int edgeIndex) const { assert(edgeIndex>=0 && edgeIndex<3); return m_edges[edgeIndex]; }
inline TupTriMeshConnectivity::Edge &TupTriMeshConnectivity::TriFace::GetEdge(int edgeIndex) { assert(edgeIndex>=0 && edgeIndex<3); return m_edges[edgeIndex]; }
inline void TupTriMeshConnectivity::TriFace::SetEdge(TupTriMeshConnectivity::Edge &edge,int edgeIndex) { assert(edgeIndex>=0 && edgeIndex<3); m_edges[edgeIndex]=edge; }

inline const TupTriMeshConnectivity::FaceEdgeLink &TupTriMeshConnectivity::TriFace::GetFaceEdgeLink(int edgeIndex) const { assert(edgeIndex>=0 && edgeIndex<3); return m_edgeLinks[edgeIndex]; }
inline TupTriMeshConnectivity::FaceEdgeLink &TupTriMeshConnectivity::TriFace::GetFaceEdgeLink(int edgeIndex) { assert(edgeIndex>=0 && edgeIndex<3); return m_edgeLinks[edgeIndex]; }
inline void TupTriMeshConnectivity::TriFace::SetFaceEdgeLink(TupTriMeshConnectivity::FaceEdgeLink &faceEdgeLink,int edgeIndex) { assert(edgeIndex>=0 && edgeIndex<3); m_edgeLinks[edgeIndex]=faceEdgeLink; }

// MeshMap
inline TupTriMeshConnectivity::MeshMap::MeshMap(void) : m_pMapVerts(NULL),m_pMapIndices(NULL),m_numMapVerts(0),m_mapChannel(0),m_mapIndex(0) { }
inline TupTriMeshConnectivity::MeshMap::~MeshMap() {}

inline void TupTriMeshConnectivity::MeshMap::AddMapVerts(float *pMapVerts,int numMapVerts)
{
	// make sure we have not done this before
	assert(!m_pMapVerts);
	// make sure there are some verts in the list
	assert(pMapVerts);
	// make sure there are some verts in the list
	assert(numMapVerts>0);
	m_numMapVerts = numMapVerts;
	m_pMapVerts = (Vertex *)pMapVerts; // just point to the data (faster)
}

inline void TupTriMeshConnectivity::MeshMap::AddMapIndices(int *pMapIndices)
{
	// make sure we have not done this before
	assert(!m_pMapIndices);
	// make sure there are some verts in the list
	assert(pMapIndices);
	m_pMapIndices = pMapIndices; // just point to the data (faster)
}

inline void TupTriMeshConnectivity::MeshMap::SetMapChannel(int mapChannel)
{
	m_mapChannel = mapChannel;
}

inline void TupTriMeshConnectivity::MeshMap::SetMapIndex(int mapIndex)
{
	m_mapIndex = mapIndex;
}

inline int *TupTriMeshConnectivity::MeshMap::GetMapIndices(void) const
{
	return m_pMapIndices;
}

inline TupTriMeshConnectivity::Vertex *TupTriMeshConnectivity::MeshMap::GetVerts(void) const
{
	return m_pMapVerts;
}

inline int TupTriMeshConnectivity::MeshMap::GetNumVerts(void) const 
{
	return m_numMapVerts;
}

inline int TupTriMeshConnectivity::MeshMap::GetMapChannel(void) const 
{
	return m_mapChannel;
}

inline int TupTriMeshConnectivity::MeshMap::GetMapIndex(void) const 
{
	return m_mapIndex;
}

// FaceEdgeLink
inline TupTriMeshConnectivity::FaceEdgeLink::FaceEdgeLink(void) : m_nextLink(-1,-1) { }
inline TupTriMeshConnectivity::FaceEdgeLink::FaceEdgeLink(const TupTriMeshConnectivity::FaceEdgeLink &s) : m_nextLink(s.m_nextLink) { }
inline TupTriMeshConnectivity::FaceEdgeLink::FaceEdgeLink(const TupTriMeshConnectivity::Edge &nextLink) : m_nextLink(nextLink) { }
inline TupTriMeshConnectivity::FaceEdgeLink::~FaceEdgeLink() { }
inline TupTriMeshConnectivity::Edge &TupTriMeshConnectivity::FaceEdgeLink::GetNextLink(void) { return m_nextLink; }
inline const TupTriMeshConnectivity::Edge &TupTriMeshConnectivity::FaceEdgeLink::GetNextLink(void) const { return m_nextLink; }
inline void TupTriMeshConnectivity::FaceEdgeLink::SetNextLink(TupTriMeshConnectivity::Edge &nextLink) { m_nextLink = nextLink; }
inline bool TupTriMeshConnectivity::FaceEdgeLink::Valid(void) const { return m_nextLink.GetFaceIndex()!=-1; }
inline void TupTriMeshConnectivity::FaceEdgeLink::SetInvalid(void) { m_nextLink.SetFaceIndex(-1); }

// GetConnectedFacesStackEntry
inline TupTriMeshConnectivity::GetConnectedFacesStackEntry::GetConnectedFacesStackEntry(void) : m_faceIndex(-1), m_stage(0) { }
inline TupTriMeshConnectivity::GetConnectedFacesStackEntry::GetConnectedFacesStackEntry(int faceIndex) : m_faceIndex(faceIndex), m_stage(0) { }
inline int TupTriMeshConnectivity::GetConnectedFacesStackEntry::GetFaceIndex(void) const { return m_faceIndex; }
inline int TupTriMeshConnectivity::GetConnectedFacesStackEntry::GetStage(void) const { return m_stage; }

inline void TupTriMeshConnectivity::GetConnectedFacesStackEntry::SetFaceIndex(int faceIndex) { m_faceIndex = faceIndex; }
inline void TupTriMeshConnectivity::GetConnectedFacesStackEntry::SetStage(int stage) { m_stage = stage; }

// GetConnectedPointsStackEntry
inline TupTriMeshConnectivity::GetConnectedPointsStackEntry::GetConnectedPointsStackEntry(void) : m_faceIndex(-1), m_pointIndex(-1), m_distance(-1) { }
inline TupTriMeshConnectivity::GetConnectedPointsStackEntry::GetConnectedPointsStackEntry(int faceIndex,int pointIndex,int distance) : m_faceIndex(faceIndex), m_pointIndex(pointIndex), m_distance(distance) { }
inline int TupTriMeshConnectivity::GetConnectedPointsStackEntry::GetFaceIndex(void) const { return m_faceIndex; }
inline int TupTriMeshConnectivity::GetConnectedPointsStackEntry::GetPointIndex(void) const { return m_pointIndex; }
inline int TupTriMeshConnectivity::GetConnectedPointsStackEntry::GetDistance(void) const { return m_distance; }

inline void TupTriMeshConnectivity::GetConnectedPointsStackEntry::SetFaceIndex(int faceIndex) { m_faceIndex = faceIndex; }
inline void TupTriMeshConnectivity::GetConnectedPointsStackEntry::SetPointIndex(int pointIndex) { m_pointIndex = pointIndex; }
inline void TupTriMeshConnectivity::GetConnectedPointsStackEntry::SetDistance(int distance) { m_distance = distance; }
