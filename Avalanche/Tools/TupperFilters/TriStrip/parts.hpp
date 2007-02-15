////////////////////////////////////////////////////////////////////////////
//
// Parts
//
// Class to handle the parts of the StripBuilder class
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

// Vertex
inline bool StripBuilder::Vertex::Compare(Vertex &vertex,float threshold)
{
	Vertex delta;
	delta.x = x-vertex.x;
	delta.y = y-vertex.y;
	delta.z = z-vertex.z;
	float diff = delta.x*delta.x+delta.y*delta.y+delta.z*delta.z;
	return (diff<(threshold*threshold));
}

// Edge
inline StripBuilder::Edge::Edge(void) : m_faceIndex(-1), m_pointIndex(-1) { }
inline StripBuilder::Edge::Edge(int faceIndex,int pointIndex) : m_faceIndex(faceIndex), m_pointIndex(pointIndex) { }
inline int StripBuilder::Edge::GetFaceIndex(void) const { return m_faceIndex; }
inline int StripBuilder::Edge::GetPointIndex(void) const { return m_pointIndex; }
inline void StripBuilder::Edge::SetFaceIndex(int faceIndex) { m_faceIndex=faceIndex; }
inline void StripBuilder::Edge::SetPointIndex(int pointIndex) { m_pointIndex = pointIndex; }

// TriFace
inline StripBuilder::TriFace::TriFace(void) : m_flags(0), m_pathNumSplits(0), m_pathSplitCredit(0), m_pathParentFace(-1) { }
inline StripBuilder::Edge &StripBuilder::TriFace::GetEdge(int edgeIndex) { assert(edgeIndex>=0 && edgeIndex<3); return m_edges[edgeIndex]; }
inline int StripBuilder::TriFace::GetFlags(void) const { return m_flags; }
inline bool StripBuilder::TriFace::PathEnabled(int edgeIndex) const 
{
	switch (edgeIndex)
	{
		case 0:
			return ((m_flags&PATH_0)!=0);
		case 1:
			return ((m_flags&PATH_1)!=0);
		case 2:
			return ((m_flags&PATH_2)!=0);
	}
	return false;
}

inline int StripBuilder::TriFace::GetPathNumSplits(void) const { return m_pathNumSplits; }
inline int StripBuilder::TriFace::GetPathSplitCredit(void) const { return m_pathSplitCredit; }
inline int StripBuilder::TriFace::GetPathParentFace(void) const { return m_pathParentFace; }
inline int StripBuilder::TriFace::GetStripIndex(void) const { return m_stripIndex; }
inline int StripBuilder::TriFace::GetPathChildFace(void) const
{
	switch (m_flags&PATHS)
	{
		// no children
		case 0:
			return -1;
		case PATH_0:
			return m_edges[0].GetFaceIndex();
		case PATH_1:
			return m_edges[1].GetFaceIndex();
		case PATH_2:
			return m_edges[2].GetFaceIndex();
		default:
			assert(false);
			return -1;
	}
}
inline int StripBuilder::TriFace::GetPathChildEdge(void) const
{
	switch (m_flags&PATHS)
	{
		// no children
		case 0:
			return -1;
		case PATH_0:
			return 0;
		case PATH_1:
			return 1;
		case PATH_2:
			return 2;
		default:
			assert(false);
			return -1;
	}
}

inline void StripBuilder::TriFace::SetEdge(StripBuilder::Edge &edge,int edgeIndex) { assert(edgeIndex>=0 && edgeIndex<3); m_edges[edgeIndex]=edge; }
inline void StripBuilder::TriFace::SetFlags(int flags) { m_flags = flags; }
inline void StripBuilder::TriFace::EnablePath(int edgeIndex)
{
	switch (edgeIndex)
	{
		case 0:
			m_flags|=PATH_0;
			break;
		case 1:
			m_flags|=PATH_1;
			break;
		case 2:
			m_flags|=PATH_2;
			break;
	}
}
inline void StripBuilder::TriFace::DisablePath(int edgeIndex)
{
	switch (edgeIndex)
	{
		case 0:
			m_flags&=~PATH_0;
			break;
		case 1:
			m_flags&=~PATH_1;
			break;
		case 2:
			m_flags&=~PATH_2;
			break;
	}
}
inline void StripBuilder::TriFace::ClearPaths(void) { m_flags&=~PATHS; }
inline void StripBuilder::TriFace::SetPathNumSplits(int pathNumSplits) { m_pathNumSplits = pathNumSplits; }
inline void StripBuilder::TriFace::SetPathSplitCredit(int pathSplitCredit) { m_pathSplitCredit = pathSplitCredit; }
inline void StripBuilder::TriFace::SetPathParentFace(int pathParentFace) { m_pathParentFace = pathParentFace; }
inline void StripBuilder::TriFace::SetStripIndex(int stripIndex) { m_stripIndex = stripIndex; }

// MeshMap
inline StripBuilder::MeshMap::MeshMap(void) : m_pMapVerts(NULL),m_pMapIndices(NULL),m_numMapVerts(0) { }
inline StripBuilder::MeshMap::~MeshMap() {}

inline void StripBuilder::MeshMap::AddMapVerts(float *pMapVerts,int numMapVerts)
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

inline void StripBuilder::MeshMap::AddMapIndices(int *pMapIndices)
{
	// make sure we have not done this before
	assert(!m_pMapIndices);
	// make sure there are some verts in the list
	assert(pMapIndices);
	m_pMapIndices = pMapIndices; // just point to the data (faster)
}

inline int *StripBuilder::MeshMap::GetMapIndices(void)
{
	return m_pMapIndices;
}

inline StripBuilder::Vertex *StripBuilder::MeshMap::GetMapVerts(void)
{
	return m_pMapVerts;
}

inline int StripBuilder::MeshMap::GetNumMapVerts(void)
{
	return m_numMapVerts;
}

// VertexColors
inline StripBuilder::VertexColorMap::VertexColorMap(void) : m_pColorVerts(NULL),m_pColorIndices(NULL),m_numColorVerts(0) { }
inline StripBuilder::VertexColorMap::~VertexColorMap() {}

inline void StripBuilder::VertexColorMap::AddColorVerts(float *pColorVerts,int numColorVerts)
{
	// make sure we have not done this before
	assert(!m_pColorVerts);
	// make sure there are some verts in the list
	assert(pColorVerts);
	// make sure there are some verts in the list
	assert(numColorVerts>0);
	m_numColorVerts = numColorVerts;
	m_pColorVerts = (Vertex *)pColorVerts; // just point to the data (faster)
}

inline void StripBuilder::VertexColorMap::AddColorIndices(int *pColorIndices)
{
	// make sure we have not done this before
	assert(!m_pColorIndices);
	// make sure there are some verts in the list
	assert(pColorIndices);
	m_pColorIndices = pColorIndices; // just point to the data (faster)
}

inline int *StripBuilder::VertexColorMap::GetColorIndices(void)
{
	return m_pColorIndices;
}

inline StripBuilder::Vertex *StripBuilder::VertexColorMap::GetColorVerts(void)
{
	return m_pColorVerts;
}

inline int StripBuilder::VertexColorMap::GetNumColorVerts(void)
{
	return m_numColorVerts;
}


// FaceEdgeLink
inline StripBuilder::FaceEdgeLink::FaceEdgeLink(void) : m_pNextLink(NULL) { }
inline StripBuilder::FaceEdgeLink::FaceEdgeLink(const StripBuilder::Edge &edge,StripBuilder::FaceEdgeLink *nextLink,int endPointIndex) : m_edge(edge), m_pNextLink(nextLink), m_endPointIndex(endPointIndex) { }
inline StripBuilder::FaceEdgeLink::~FaceEdgeLink() { }
inline StripBuilder::Edge &StripBuilder::FaceEdgeLink::GetEdge(void) { return m_edge; }
inline StripBuilder::FaceEdgeLink *StripBuilder::FaceEdgeLink::GetNextLink(void) { return m_pNextLink; }
inline int StripBuilder::FaceEdgeLink::GetEndPointIndex(void) { return m_endPointIndex; }
inline void StripBuilder::FaceEdgeLink::SetEdge(StripBuilder::Edge &edge) { m_edge = edge; }
inline void StripBuilder::FaceEdgeLink::SetNextLink(StripBuilder::FaceEdgeLink *pNextLink) { m_pNextLink=pNextLink; } 
inline void StripBuilder::FaceEdgeLink::SetEndPointIndex(int endPointIndex) { m_endPointIndex = endPointIndex; }

// BuildPathStackEntry
inline StripBuilder::BuildPathStackEntry::BuildPathStackEntry(void) : m_faceIndex(-1), m_nextPointIndex(-1), m_prevPointIndex(-1), m_parity(false), m_stage(0) { }
inline StripBuilder::BuildPathStackEntry::BuildPathStackEntry(int faceIndex,int nextPointIndex,int prevPointIndex,bool parity) : 
	m_faceIndex(faceIndex), m_nextPointIndex(nextPointIndex), m_prevPointIndex(prevPointIndex), m_parity(parity), m_stage(0) { }
inline int StripBuilder::BuildPathStackEntry::GetFaceIndex(void) const { return m_faceIndex; }
inline int StripBuilder::BuildPathStackEntry::GetNextPointIndex(void) const { return m_nextPointIndex; }
inline int StripBuilder::BuildPathStackEntry::GetPrevPointIndex(void) const { return m_prevPointIndex; }
inline bool StripBuilder::BuildPathStackEntry::GetParity(void) const { return m_parity; }
inline int StripBuilder::BuildPathStackEntry::GetStage(void) const { return m_stage; }

inline void StripBuilder::BuildPathStackEntry::SetFaceIndex(int faceIndex) { m_faceIndex = faceIndex; }
inline void StripBuilder::BuildPathStackEntry::SetNextPointIndex(int nextPointIndex) { m_nextPointIndex = nextPointIndex; }
inline void StripBuilder::BuildPathStackEntry::SetPrevPointIndex(int prevPointIndex) { m_prevPointIndex = prevPointIndex; }
inline void StripBuilder::BuildPathStackEntry::SetParity(bool parity) { m_parity = parity; }
inline void StripBuilder::BuildPathStackEntry::SetStage(int stage) { m_stage = stage; }

// PeelPathStackEntry
inline StripBuilder::PeelPathStackEntry::PeelPathStackEntry(void) : m_faceIndex(-1), m_stage(0) { }
inline StripBuilder::PeelPathStackEntry::PeelPathStackEntry(int faceIndex) : m_faceIndex(faceIndex), m_stage(0) { }
inline int StripBuilder::PeelPathStackEntry::GetFaceIndex(void) const { return m_faceIndex; }
inline int StripBuilder::PeelPathStackEntry::GetStage(void) const { return m_stage; }

inline void StripBuilder::PeelPathStackEntry::SetFaceIndex(int faceIndex) { m_faceIndex = faceIndex; }
inline void StripBuilder::PeelPathStackEntry::SetStage(int stage) { m_stage = stage; }

// Strip
inline StripBuilder::Strip::Strip(void): m_startFace(-1), m_endFace(-1), m_numFaces(0), m_flags(0), m_numPoints(0) { }
inline StripBuilder::Strip::Strip(int startFace,int endFace,int numFaces) : m_startFace(startFace), m_endFace(endFace), m_numFaces(numFaces), m_flags(0), m_numPoints(0) { }

inline int StripBuilder::Strip::GetStartFace(void) const { return m_startFace; }
inline int StripBuilder::Strip::GetEndFace(void) const { return m_endFace; }
inline int StripBuilder::Strip::GetNumFaces(void) const { return m_numFaces; }
inline int StripBuilder::Strip::GetFlags(void) const { return m_flags; }
inline int StripBuilder::Strip::GetNumPoints(void) const { return m_numPoints; }

inline void StripBuilder::Strip::SetStartFace(int startFace) { m_startFace = startFace; }
inline void StripBuilder::Strip::SetEndFace(int endFace) { m_endFace = endFace; }
inline void StripBuilder::Strip::SetNumFaces(int numFaces) { m_numFaces = numFaces; }
inline void StripBuilder::Strip::SetFlags(int flags) { m_flags = flags; }
inline void StripBuilder::Strip::SetNumPoints(int numPoints) { m_numPoints = numPoints; }

// PointStrip
inline StripBuilder::PointStrip::PointStrip(void) : m_numTriangles(0) {}

inline int StripBuilder::PointStrip::GetNumPoints(void) const { return m_faceIndices.GetSize(); }
inline int StripBuilder::PointStrip::GetFaceIndex(int index) const { return m_faceIndices[index]; }
inline int StripBuilder::PointStrip::GetPointIndex(int index) const { return m_pointIndices[index]; }
inline const int *StripBuilder::PointStrip::GetFaceIndices(void) const { return &m_faceIndices[0]; }
inline const int *StripBuilder::PointStrip::GetPointIndices(void) const { return &m_pointIndices[0]; }
inline const int StripBuilder::PointStrip::GetNumTriangles(void) const { return m_numTriangles; }

inline void StripBuilder::PointStrip::SetNumPoints(int numPoints) { m_faceIndices.SetSize(numPoints); m_pointIndices.SetSize(numPoints); }
inline void StripBuilder::PointStrip::SetFaceIndex(int index,int faceIndex) { m_faceIndices[index] = faceIndex; }
inline void StripBuilder::PointStrip::SetPointIndex(int index,int pointIndex) { m_pointIndices[index] = pointIndex; }
inline int StripBuilder::PointStrip::AddFaceIndex(int faceIndex) { m_faceIndices.Add(faceIndex); return m_faceIndices.GetSize()-1; }
inline int StripBuilder::PointStrip::AddPointIndex(int pointIndex) { m_pointIndices.Add(pointIndex); return m_pointIndices.GetSize()-1; }
inline void StripBuilder::PointStrip::SetNumTriangles(int numTriangles) { m_numTriangles = numTriangles; }
inline void StripBuilder::PointStrip::Init(void) { m_numTriangles = 0; m_faceIndices.RemoveAll(); m_pointIndices.RemoveAll(); }

// StripSortData
inline StripBuilder::StripSortData::StripSortData(void) : m_stripIndex(0)
{
}

inline StripBuilder::StripSortData::StripSortData(const StripSortData &src) : m_stripIndex(src.m_stripIndex), m_compareData(src.m_compareData)
{
}

inline const StripBuilder::StripSortData& StripBuilder::StripSortData::operator=(const StripBuilder::StripSortData& src) 
{
	m_stripIndex = src.m_stripIndex;
	m_compareData = src.m_compareData;
	return *this;
}

inline bool StripBuilder::StripSortData::operator==(const StripBuilder::StripSortData& other) const
{
	return m_compareData==other.m_compareData;
}

inline bool StripBuilder::StripSortData::operator<=(const StripBuilder::StripSortData& other) const
{
	return m_compareData<=other.m_compareData;
}

inline bool StripBuilder::StripSortData::operator>=(const StripBuilder::StripSortData& other) const
{
	return m_compareData>=other.m_compareData;
}

inline bool StripBuilder::StripSortData::operator<(const StripBuilder::StripSortData& other) const
{
	return m_compareData<other.m_compareData;
}

inline bool StripBuilder::StripSortData::operator>(const StripBuilder::StripSortData& other) const
{
	return m_compareData>other.m_compareData;
}

// MinMaxUV
inline StripBuilder::MinMaxUV::MinMaxUV(void) : m_minU(MAXFLOAT32),m_minV(MAXFLOAT32),m_maxU(-MAXFLOAT32),m_maxV(-MAXFLOAT32)
{
}

inline bool StripBuilder::MinMaxUV::Test(const Vector3 &limitUV) const
{
	// is the min/max set up?
	if (m_minU<=m_maxU && m_minV<=m_maxV)
	{
		if (m_maxU-m_minU > limitUV.x( ) )
			return false;
		if (m_maxV-m_minV > limitUV.y( ) )
			return false;
	}
	return true;
}

inline void StripBuilder::MinMaxUV::Init(void)
{
	m_minU = m_minV = MAXFLOAT32;
	m_maxU = m_maxV = -MAXFLOAT32;
}

inline bool StripBuilder::MinMaxUV::TestCoor(const Vector3 &coor,const Vector3 &limitUV) const
{
	// is the min/max set up?
	if (m_minU<=m_maxU && m_minV<=m_maxV)
	{
		float newMin,newMax;
		newMin = (coor.x( ) < m_minU) ? coor.x( ) : m_minU;
		newMax = (coor.x( ) >m_maxU) ? coor.x( ) : m_maxU;
		if (newMax-newMin > limitUV.x( ) )
			return false;
		newMin = (coor.y( ) < m_minV) ? coor.y( ) : m_minV;
		newMax = (coor.y( ) > m_maxV) ? coor.y( ) : m_maxV;
		if (newMax-newMin > limitUV.y( ) )
			return false;
	}
	return true;
}

inline void StripBuilder::MinMaxUV::AddCoor(const Vector3 &coor)
{
	m_minU = (coor.x( ) < m_minU) ? coor.x( ) : m_minU;
	m_maxU = (coor.x( ) > m_maxU) ? coor.x( ) : m_maxU;
	m_minV = (coor.y( ) < m_minV) ? coor.y( ) : m_minV;
	m_maxV = (coor.y( ) > m_maxV) ? coor.y( ) : m_maxV;
}

inline void StripBuilder::MinMaxUV::Combine(const MinMaxUV &minMaxUV)
{
	m_minU = (minMaxUV.m_minU<m_minU) ? minMaxUV.m_minU : m_minU;
	m_minU = (minMaxUV.m_maxU<m_minU) ? minMaxUV.m_maxU : m_minU;
	m_maxU = (minMaxUV.m_minU>m_maxU) ? minMaxUV.m_minU : m_maxU;
	m_maxU = (minMaxUV.m_maxU>m_maxU) ? minMaxUV.m_maxU : m_maxU;
	m_minV = (minMaxUV.m_minV<m_minV) ? minMaxUV.m_minV : m_minV;
	m_minV = (minMaxUV.m_maxV<m_minV) ? minMaxUV.m_maxV : m_minV;
	m_maxV = (minMaxUV.m_minV>m_maxV) ? minMaxUV.m_minV : m_maxV;
	m_maxV = (minMaxUV.m_maxV>m_maxV) ? minMaxUV.m_maxV : m_maxV;
}

inline bool StripBuilder::MinMaxUV::TestCombine(const MinMaxUV &minMaxUV,const Vector3 &limitUV) const
{
	MinMaxUV tempMinMax(*this);
	tempMinMax.Combine(minMaxUV);
	return tempMinMax.Test(limitUV);
}

// SavedStrip
inline StripBuilder::SavedStrip::SavedStrip(void) 
{
}

inline void StripBuilder::SavedStrip::SetStrip(const StripBuilder::Strip &strip) 
{
   m_savedStrip = strip; 
}

inline void StripBuilder::SavedStrip::AddFace(int faceIndex,const StripBuilder::TriFace &triFace) 
{ 
   m_faceIndices.Add(faceIndex); 
   m_savedFaces.Add(triFace); 
}

inline const StripBuilder::Strip &StripBuilder::SavedStrip::GetStrip(void) const 
{
   return m_savedStrip; 
}

inline int StripBuilder::SavedStrip::GetNumFaces(void) const 
{
   return m_faceIndices.GetSize(); 
}

inline const StripBuilder::TriFace &StripBuilder::SavedStrip::GetFace(int index) const 
{
   return m_savedFaces[index]; 
}

inline int StripBuilder::SavedStrip::GetFaceIndex(int index) const 
{
   return m_faceIndices[index]; 
}

// stats
inline StripBuilder::Stats::Stats(void)
{
	Init();
}

inline void StripBuilder::Stats::Init(void)
{
	m_totalConnections = 0;
	m_totalFaceConnectionGroups = 0;
 	m_connectionBreakMaterialIndex = 0;
	m_connectionBreakFaceFlags = 0;
	m_connectionBreakVertexNormals = 0;
	m_connectionBreakMeshMaps = 0;
	m_connectionBreakVertexColors = 0;
	m_connectionBreakVertexAlphas = 0;
	m_connectionBreakStages = 0;
	m_sortedStripsBreakMaterialIndex = 0;
	m_sortedStripsBreakFaceFlags = 0;
	m_buildPathBreaksBones = 0;
	m_buildPathBreaksUVLimit = 0;
	m_buildStripFromPathOneFace = 0;
	m_buildStripFromPathTwoFace = 0;
	m_buildStripFromPathBadEdge = 0;
	m_buildStripFromPathBadEvenStrip = 0;
	m_buildStripFromPathBadEvenStripEdge = 0;
   m_vertexCacheMisses = 0;
   m_vertexCacheHits = 0;
	m_combineStrips = 0;
}

// VertexCache
inline StripBuilder::VertexCache::VertexCache(int maxEntries) : 
   m_nextPos(0), 
   m_count(0)
{
   assert(maxEntries);
   m_verts.SetSize(maxEntries);
}

inline StripBuilder::VertexCache::VertexCache(const StripBuilder::VertexCache &src) : 
   m_verts(src.m_verts),
   m_nextPos(src.m_nextPos),
   m_count(src.m_count)
{
}

inline const StripBuilder::VertexCache& StripBuilder::VertexCache::operator=(const StripBuilder::VertexCache &src)
{
   m_verts = src.m_verts;
   m_nextPos = src.m_nextPos;
   m_count = src.m_count;
   return *this;
}

bool inline StripBuilder::VertexCache::InCache(int value) const
{
   int numToCheck = m_count;
   int checkIndex = m_nextPos>0 ? m_nextPos-1 : m_verts.GetSize()-1;
   while(numToCheck)
   {
      if (m_verts[checkIndex]==value)
         return true;
      checkIndex = checkIndex>0 ? checkIndex-1 : m_verts.GetSize()-1;
      numToCheck--;
   }
   return false;
}

bool inline StripBuilder::VertexCache::AddValue(int value)
{
   if (InCache(value))
      return false;
   else
   {
      m_verts[m_nextPos] = value;
      if (m_count<m_verts.GetSize())
         m_count++;
      m_nextPos = m_nextPos<m_verts.GetSize()-1 ? m_nextPos+1 : 0;
      return true;
   }
}

int inline StripBuilder::VertexCache::GetMaxEntries(void) const
{
   return m_verts.GetSize();
}

int inline StripBuilder::VertexCache::GetNumValues(void) const
{
   return m_count;
}

