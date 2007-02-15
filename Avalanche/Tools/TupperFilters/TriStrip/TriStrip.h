////////////////////////////////////////////////////////////////////////////
//
// TriStrip
//
// Class to handle the Triangle Stripper Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TRI_STRIP_H
#define TRI_STRIP_H

#ifndef STRIP_BUILDER_H
#include "stripBuilder.h"
#endif


class TupperwareAggregate;

class TriStripFilter : public FilterPlugin
{
	class MeshStripStats
	{
	public:
		int m_numStrips;
		int m_numFaces;
		int m_numPoints;
		int m_numSwaps;
		int m_totalConnections;
		int m_totalFaceConnectionGroups;
		int m_connectionBreakMaterialIndex;
		int m_connectionBreakFaceFlags;
		int m_connectionBreakVertexNormals;
		int m_connectionBreakMeshMaps;
		int m_connectionBreakVertexColors;
		int m_connectionBreakVertexAlphas;
		int m_connectionBreakStages;
		int m_buildPathBreaksBones;
		int m_buildPathBreaksUVLimit;
		int m_buildStripFromPathOneFace;
		int m_buildStripFromPathTwoFace;
		int m_buildStripFromPathBadEdge;
		int m_buildStripFromPathBadEvenStrip;
		int m_buildStripFromPathBadEvenStripEdge;
		int m_combineStrips;
		int m_sortedStripsBreakMaterialIndex;
		int m_sortedStripsBreakFaceFlags;
      int m_vertexCacheMisses;
      int m_vertexCacheHits;
		float m_pointsPerFace;
		float m_trianglesPerStrip;
		TupString m_name;
	};

	bool m_bAllowSwaps;
   int m_nBonesPerStrip;
   bool m_bOptimizeVertexCache;
	int m_nMaxStripPixels;
	bool m_bStripStageFaces;
	float m_normalThreshold;
	float m_mapThreshold;
	float m_vertexColorThreshold;
	float m_vertexAlphaThreshold;
   int m_nVertexCacheSize;
   int m_nVertexCacheStart;
	
	int m_totalStrips;
	int m_totalFaces;
	int m_totalPoints;
	int m_totalSwaps;

	TupArray<MeshStripStats> m_meshStripStats;

	enum WriteDependencies
	{
		TRI_STRIPS,
		NUM_WRITE_DEPENDENCIES // last entry
	};

	enum WriteDependenciesOptional
	{
		BONES_PER_STRIP,
		NO_SWAPS,
		MAX_STRIP_PIXELS,
		STRIP_STAGE_FACES,
      OPTIMIZE_VERTEX_CACHE,
		NUM_OPTIONAL_WRITE_DEPENDENCIES // last entry
	};

	enum ReadDependencies
	{
		BONES_TO_MESH,
		OBJECT_SOM_STARTS,
		ADD_VERTEX_COLOR_REFS_TO_MESH,
		NUM_READ_DEPENDENCIES // last entry
	};

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pMaterialPoolAgg;
	TupperwareAggregate *m_pMapPoolAgg;
	TupperwareAggregate *m_pImagePoolAgg;

	// I own these
	TupScene *m_pTupScene;
	TupObjectPool *m_pTupObjectPool;
	TupMaterialPool *m_pTupMaterialPool;
	TupMapPool *m_pTupMapPool;
	TupImagePool *m_pTupImagePool;

	class UVMap
	{
	public:
		int m_mapChannel;
		int m_uvVerts;
		Vector3 *m_pUVVerts;
		int *m_pUVIndices;
	};

	TupArray<UVMap> m_uvMaps;
	Keeper<int> m_uvChannelToIndexKeeper;

	class VertexColorMap
	{
	public:
		int m_numColorVerts;
		Vector3 *m_pColorVerts;
		int *m_pColorIndices;
	};

	TupArray<VertexColorMap> m_vertexColorMaps;
	TupArray<VertexColorMap> m_vertexAlphaMaps;

	FilterDataManager *m_pTupDataManager;

	static char *m_writeDependencyNames[NUM_WRITE_DEPENDENCIES];
	static Revision m_writeDependencyRevisions[NUM_WRITE_DEPENDENCIES];

	static char *m_writeDependencyNamesOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES];
	static char *m_writeDependencyOptionNames[NUM_OPTIONAL_WRITE_DEPENDENCIES];
	static Revision m_writeDependencyRevisionsOptions[NUM_OPTIONAL_WRITE_DEPENDENCIES];

	static char *m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision m_readDependencyRevisions[NUM_READ_DEPENDENCIES];



public:
	TriStripFilter(TupperChainLogBase &log) : FilterPlugin(log) {}

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	int GetTotalWriteDependencies(int pass,const FilterOptions &options) const;
	const char *GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const;

	int GetTotalReadDependencies(int pass,const FilterOptions &options) const;
	const char *GetReadDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const;

private:
	int GetWriteDependencyOption(const FilterOptions &options,int offset) const;
	void CheckOptions(const FilterOptions &options);

	void TriStripTriMesh(TupTriMesh &tupTriMesh);
	void BuildUVTransformsAndLimits(int materialRef,TupArray<TupArray<UVTransform> > &uvTransformArray,TupArray<TupArray<Vector3> > &
		uvLimitsArray);
	void BuildUVFaceData(TupArray<TupArray<UVTransform> > &uvTransformArray,TupTriMesh &tupTriMesh,TupArray<TupArray<Vector3> >&faceUVArray);
	void AddSortedStripsToLog(void);
};

#endif


