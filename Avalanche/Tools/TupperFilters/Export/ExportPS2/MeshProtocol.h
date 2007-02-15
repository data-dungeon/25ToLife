////////////////////////////////////////////////////////////////////////////
//
// MeshProtocol
//
// Protocol class for communicating with the PS2 Packetizer library
//
////////////////////////////////////////////////////////////////////////////

#ifndef MESH_PROTOCOL_H
#define MESH_PROTOCOL_H

#undef min
#undef max
#include "Packetizer/MeshInterface.h"
#include "Game/Database/SOM.h"
#include "MeshState.h"

#include <list>
class BoneBuilder;

class MeshProtocol : public MeshInterface
{
	FilterDataManager &m_dataManager;

	TupTriMesh &m_tupTriMesh;
	BoneBuilder &m_boneBuilder;
	TupObjectPool &m_tupObjectPool;
	TupMaterialPool &m_tupMaterialPool;
	TupMapPool &m_tupMapPool;
	TupImagePool &m_tupImagePool;

	int m_currentPartition;

	StageFaceManager m_stageFaceManager;

	class PassInfo
	{
	public:
		MeshState m_meshState;
		MeshPassInfo m_meshPassInfo;
		int m_layerIndex; // which layer (determined by the texture context #textures)
		int m_stageIndex; // which stage (multitexture) each face points to next stage
	};

	class UVStream
	{
	public:
		UVTransform m_uvTransform;
		int m_layerIndex; // so I have access to these in the uv access routine
		int m_stageIndex;
	};

	class BigUVStrip
	{
	public:
		int m_stageIndex;
		int m_layerIndex;
		int m_numPoints;
		Vector2 m_minUV;
		Vector2 m_maxUV;
		Vector2 m_uvOffset;
		int m_imageWidth;
		int m_imageHeight;
		int m_imageRef;
	};

	class StateInfo
	{
	public:
		TupArray<PassInfo> m_passArray;
		int m_startStripIndex; // what is the starting strip for this state
		int m_numStrips; // how many strips we have
		int m_numPoints; // how many points all the strips have total
		TupArray<UVStream> m_uvStreams;
	};

	class TriStripStage
	{
	public:
		int m_materialIndex;
		int m_faceFlags;
		TupArray<Vector2> m_stripUVOffsetArray; // value to add to the uv data
	};

	class TriStripHeader
	{
	public:
		int m_numPoints;
		int m_materialIndex;
		float m_environmentMapOpacity;
		int m_faceFlags;
		int *m_pFaceIndices; // pointing to tup data
		int *m_pFacePointIndices; // pointing to tup data
		TupArray<TriStripStage> m_stages;
	};

	class VertexColorMap
	{
	public:
		Position3<float> *m_pVertexColorVerts; // pointing to tup data
		int m_numVertexColorVerts;
		int *m_pVertexColorIndices;	// pointing to tup data
		int m_numVertexColorFaces;
	};

	TupArray<int> m_textureContextIDArray;
	TupArray<TupArray<UVTransform> > m_UVTransformArray;
	TupArray<int> m_materialFlagsArray;
	int m_nNumColorSets;
	TupArray<ts_FullTextureContext> m_textureContextArray;

	TupArray<TriStripHeader> m_triStripHeaderArray;
	TupArray<VertexColorMap> m_vertexColorSetsArray;
	TupArray<VertexColorMap> m_vertexAlphaSetsArray;

	int m_numVerts, m_numFaces;
	Position3<float> *m_pPositionVerts, *m_pNormalVerts; // pointing to tup data

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
	
	int *m_pPositionIndices, *m_pNormalIndices; // pointing to tup data
	int *m_pMaterialIndices,*m_pFaceFlags; // pointing to tup data
	float *m_pSubMaterialEnvironmentMapOpacityValues; // pointing to tup data
	int *m_pSubMaterialHasEnvironmentMapValues; // pointing to tup data

	bool m_bMeshVertexColors;
	bool m_bMeshNormals;
	bool m_bRealMeshBones;
	bool m_bFakeMeshBone;
	bool m_bHasEnvironmentMap;
	bool m_bUsesTextureTransformRot;

	TupTriangleStripPool *m_pTupTriangleStripPool;
	int m_nTotalTriStrips;

	TupArray<StateInfo> m_stateInfoArray;
	int m_currentColorSet;
							
	class ActiveState
	{
	public:
		int m_stateIndex; // which state we are in
		int m_passIndex; // which pass we are in
		int m_activeStatePoint; // which point within the state
		int m_stripIndex; // which strip we are in
		int m_pointIndex; // which point within the strip we are in
		int m_faceStage; // which face stage we are in
	};

	mutable ActiveState m_activeState;

	mutable BaseColor<unsigned short> m_color;
	mutable Position2<float> m_UV;
	mutable Position3<float> m_vertex;

	MeshProtocol(void); // don't use this one

public:
	MeshProtocol::MeshProtocol(TupTriMesh &tupMesh,BoneBuilder &boneBuilder,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,FilterDataManager &dataManager,int currentPartition);
	~MeshProtocol();

	void SetMesh(void);

	unsigned int states( void ) const;

	VUStateProtocol &		peekVUState( unsigned int uiState, unsigned int uiPass );
	MeshPassInfo &			peekPassInfo( unsigned int uiState, unsigned int uiPass );
	
	unsigned int numVertices( void ) const;
	unsigned int numStates( void ) const;
	unsigned short numPasses( unsigned int uiState ) const;
	const unsigned int numColorSets(void) const;
	void enableColorSet( unsigned int uiSet);
	const unsigned int numRequiredAlphaBits( void) const; 	 	// TJC - get the minimum # of alpha bits required (should currently always return 1 or 8, default is 8)

	VUStateProtocol &activateState( unsigned int uiState, unsigned int uiPass );

	unsigned int size( void ) const;
	unsigned short stages( void ) const;

	const	unsigned int idx( unsigned int uivtx) const;
	const Position3<float> &vtx( unsigned int uivtx) const;
	const Position3<float> &nrm( unsigned short usStreamIdx, unsigned int uivtx) const;
	const Position2<float> &uv( unsigned short usStreamIdx, unsigned int uivtx) const;
	const Position2<float> &st( unsigned short usStreamIdx, unsigned int uivtx) const;
	const BaseColor<unsigned short> &clr( unsigned short usStreamIdx, unsigned int uivtx) const;	

//	const unsigned int matrixidx( unsigned int uivtx) const;
//	const unsigned int matrixcount( void) const;
	// matrices
	const unsigned int numMatrices( void) const;
	const unsigned int idxMatrix( unsigned int uivtx) const;

#undef min
#undef max
	const Position3<float> min( void ) const;
	const Position3<float> max( void ) const;
	const Position3<float>	dim( void ) const;

private:

	void UpdateActiveState(unsigned int statePointIndex) const;
	void BuildTextureContextIDs(int materialRef);
	void BuildTextureContexts(int materialRef);
	void BuildUVTransformArray(int materialRef);
	void BuildFlagsArray(int materialRef);
	void BuildVertexColorInfo(void);
	void BuildMaterialInfo(void);
	void BuildTriStripInfo(void);
	void BuildMeshInfo(void);
	void SetupSortKeys(void);
	void BuildLightingInfo(void);
	int AddUVStream(TupArray<UVStream> &uvStreams,const UVTransform &uvTransform,int layerIndex,int stageIndex);
		
};

#endif
