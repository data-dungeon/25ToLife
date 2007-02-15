////////////////////////////////////////////////////////////////////////////
//
// StripBuilder
//
// Class to handle the Building of Triangle Strips
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef STRIP_BUILDER_H
#define STRIP_BUILDER_H

class StripBuilder
{
public:

class Vertex;
class MeshMap;
class VertexColorMap;
class Edge;
class TriFace;
class FaceEdgeLink;
class BuildPathStackEntry;
class PeelPathStackEntry;
class Strip;
class PointStrip;
class StripSortData;
class LimitUV;
class MinMaxUV;
class SavedStrip;
class Stats;
class VertexCache;

private:

// just points to this data (does not own it)
	Vertex *m_pPositionVerts;
	Vertex *m_pNormalVerts;
	int *m_pPositionIndices;
	int *m_pNormalIndices;
	int *m_pFaceMaterialIndices;
	int *m_pFaceFlags;
	Vertex *m_pFaceNormalVerts;
	int *m_pFaceNormalIndices;
	int *m_pBoneVerts;
	int m_nBonesPerStrip;
	bool m_bAllowSwaps;
   bool m_bOptimizeVertexCache;
	float m_normalThreshold;
	float m_mapThreshold;
	float m_vertexColorThreshold;
	float m_vertexAlphaThreshold;
   int m_nVertexCacheSize;
   int m_nVertexCacheStart;

	bool m_bStripStageFaces;
	int *m_pNextStagedFaceIndices;

	TupArray<TupArray<Vector3> > *m_pUVLimitsArray;
	TupArray<TupArray<Vector3> > *m_pFaceUVArray;

	int m_numPositionVerts;
	int m_numNormalVerts;
	int m_numFaceNormalVerts;
	int m_numFaces;

	TupString m_name;

// owns this data
	TupArray<MeshMap> m_meshMaps;
	TupArray<VertexColorMap> m_vertexColorMaps;
	TupArray<VertexColorMap> m_vertexAlphaMaps;

	TriFace *m_pFaces;

	TupArray<int> m_buildPathRoots;
	TupArray<int> m_peeledRoots;
	TupArray<Strip> m_strips;
	TupArray<PointStrip> m_pointStrips;
	TupArray<Keeper<int> > m_stripBones;
	TupArray<TupArray<TupArray<MinMaxUV> > >m_stripUVMinMaxArray; // [strip][stage][layer]
	TupArray<StripSortData> m_stripSortArray;

	// what face group each face belongs to
	TupArray<int> m_connectedFaceGroups;

	// a list of faces in each group
	TupArray<int> m_connectedFaces;
	// offsets for the start of each connected face group
	TupArray<int> m_connectedFaceGroupOffsets;

	Stats *m_pStats;

	int m_faceFlagMask;

	static int m_numNonRequestedSwaps; // temp for keeping track of swaps that exist when bAllowSwaps is false
	
public:
	StripBuilder(void);
	~StripBuilder();

	void SetNumFaces(int numFaces);
	void AddPositionVerts(float *pPositionVerts,int numPositionVerts);
	void AddNormalVerts(float *pNormalVerts,int numNormalVerts);
	void AddPositionIndices(int *pPositionIndices);
	void AddNormalIndices(int *pNormalIndices);
	void AddFaceMaterialIndices(int *pFaceMaterialIndices);
	void AddFaceNormalVerts(float *pFaceNormalVerts,int numFaceNormalVerts);
	void AddFaceNormalIndices(int *pFaceNormalIndices);
	void AddFaceFlags(int *pFaceFlags);
	void SetFaceFlagMask(int faceFlagMask);
	void AddMeshMap(float *pMapVerts,int numMapVerts,int *pMapIndices);
	void AddVertexColorMap(float *pColorVerts,int numColorVerts,int *pColorIndices);
	void AddVertexAlphaMap(float *pColorVerts,int numColorVerts,int *pColorIndices);
	void AddName(const char *pName);
	void AddNextStagedFaceIndices(int *pNextStageFaceIndices);

	void AddBoneVerts(int *pBoneVerts,int numPositionVerts);
	void SetBonesPerStrip(int nBonesPerStrip);
	void SetAllowSwaps(bool bAllowSwaps);
   void SetOptimizeVertexCache(bool bOptimizeVertexCache);
	void SetNormalThreshold(float normalThreshold);
	void SetMapThreshold(float mapThreshold);
	void SetVertexColorThreshold(float vertexColorThreshold);
	void SetVertexAlphaThreshold(float vertexAlphaThreshold);
	void SetStripStageFaces(bool bStripStageFaces);
   void SetVertexCacheSize(int vertexCacheSize);
   void SetVertexCacheStart(int vertexCacheStart);
	
	// max strip pixels stuff
	void AddUVLimitsArray(TupArray<TupArray<Vector3> > &uvLimitsArray);
	void AddFaceUVArray(TupArray<TupArray<Vector3> > &faceUVArray);

	const Stats &GetStats(void) const;

	// temp for checking output
	TriFace *GetFaceData(void) { return m_pFaces; }
	TupArray<Strip> &GetStripData(void) { return m_strips; }
	TupArray<PointStrip> &GetPointStripData(void) { return m_pointStrips; }
	TupArray<StripSortData> &GetStripSortData(void) { return m_stripSortArray; }

	int GetNumConnectedFaceGroups(void) { return m_connectedFaceGroupOffsets.GetSize(); }
	TupArray<int> &GetConnectedFaceGroups(void) { return m_connectedFaceGroups; }
	
	void ComputeStrips(void);

	enum CombineStripsType
	{
		START_TO_END, // start of strip2 to end of strip1
		END_TO_START, // end of strip2 to start of strip1
		END_TO_END, // end of strip2 to end of strip1
		START_TO_START, // start of strip2 to start of strip1
	};

private:
	void ConnectFaces(void);
	void BuildConnectedFaceGroups(void);
	void CheckFaces(void);
	void BuildPaths(void);
	int BuildPathSearch(int faceIndex,int nextPointIndex,int prevPointIndex,Keeper<int> &usedBones,VertexCache &usedVerts,TupArray<TupArray<MinMaxUV> >&minMaxUVArray);
   int SearchAheadForCacheHit(const VertexCache &usedVerts,int faceIndex,int pointIndex,int maxDepth);
   int SearchAheadForCacheHit(const VertexCache &usedVerts,int faceIndex,int pointIndex,const Keeper<int> &usedFaces,int currentDepth,int maxDepth);
	void PeelPaths(void);
	void PeelPathSearch(int faceIndex);
	void FixPeeledRoots(void); // fixes problem of root having 2 children in a path
	void BuildStripsFromPaths(void);
	void ComputeStripNumPoints(void);
	void ComputeStripNumPoints(Strip &strip);
	void SaveStrip(const Strip &strip,SavedStrip &saveStrip) const;
	void RestoreStrip(Strip &strip,const SavedStrip &saveStrip);
	bool MergeStrips(Strip &strip1,Strip &strip2,CombineStripsType combineStripsType,int threshold); // 4 types of hookups
	void MergeStripsSimple(Strip &strip1,Strip &strip2); // start of strip2 to end of strip1
	void ReverseStrip(Strip &strip);
	int GetOptimalFaceInPattern(int face1,int face2,int face3);
	int GetAlternateFaceInPattern(int face1,int face2,int face3);
	int GetOneFaceInPattern(int face1,int face2);
	int GetAnotherFaceInPattern(int face1,int face2);
	void CombineStrips(void);
	bool CombineStrips(int stripIndex,int threshold);
	bool CombineStrips(int firstFace,int secondFace,int thirdFace,int tryFirstFace,int stripIndex,int threshold,bool stripEnd);
	bool CombineStrips(int firstFace,int secondFace,int thirdFace,int stripIndex,int threshold,bool stripEnd);
	bool CombineStrips(int firstFace,int secondFace,int stripIndex,int threshold,bool stripEnd);
	bool CombineStrips(int firstFace,int stripIndex,int threshold);
	void CheckStrips(void);
	void CheckStrip(int stripIndex);
	void BuildPointStrips(void);
	void BuildPointStrip(Strip &strip, PointStrip &pointStrip);
	void BuildStripBones(void);
	void BuildStripBones(int stripIndex);
	void BuildStripUVMinMax(void);
	void BuildStripUVMinMax(int stripIndex);
	bool StripCombineCheck(int strip1Index,int strip2Index);
	void SortStrips(void);
	void BuildSortedStripStats(void);
	void CheckPointStrips(void);
	void AddStagedFaceStrips(void);
	int GetNumStages(int faceIndex);
	void GetStagedFaces(int faceIndex,TupArray<int> &stagedFaces);



};

#endif
