////////////////////////////////////////////////////////////////////////////
//
// MeshProtocol
//
// Protocol class for communicating with the PS2 Packetizer library
//
////////////////////////////////////////////////////////////////////////////

#include "ExportPch.h"
#pragma warning(disable : 4786)
#include <map>
#include <math.h>
#include <float.h>
#include "Game\Animation\bone.h"
#include "Packetizer/VUStateProtocol.h"
#include "MeshProtocol.h"
#include "ExportBones.h"

MeshProtocol::MeshProtocol(TupTriMesh &tupTriMesh,BoneBuilder &boneBuilder,TupObjectPool &tupObjectPool,TupMaterialPool &tupMaterialPool,TupMapPool &tupMapPool,TupImagePool &tupImagePool,FilterDataManager &dataManager,int currentPartition) : 
	m_tupTriMesh(tupTriMesh), 
	m_boneBuilder(boneBuilder),
	m_tupObjectPool(tupObjectPool),
	m_tupMaterialPool(tupMaterialPool),
	m_tupMapPool(tupMapPool),
	m_tupImagePool(tupImagePool),
	m_dataManager(dataManager),
	m_currentPartition(currentPartition),
	m_stageFaceManager(tupTriMesh)
{
	SetMesh();
}

MeshProtocol::~MeshProtocol( )
{
	delete m_pTupTriangleStripPool;
}

unsigned int MeshProtocol::states( void ) const
{
	return m_stateInfoArray.GetSize(); 
}

VUStateProtocol &MeshProtocol::peekVUState( unsigned int uiState, unsigned int uiPass )
{
	return m_stateInfoArray[uiState].m_passArray[uiPass].m_meshState;
}

unsigned int MeshProtocol::numVertices( void ) const
{
	return m_stateInfoArray[m_activeState.m_stateIndex].m_numPoints;
}

MeshPassInfo &MeshProtocol::peekPassInfo( unsigned int uiState, unsigned int uiPass )
{
	return m_stateInfoArray[uiState].m_passArray[uiPass].m_meshPassInfo;
}

unsigned int MeshProtocol::numStates( void ) const
{
	return(m_stateInfoArray.GetSize());
}

unsigned short MeshProtocol::numPasses( unsigned int uiState ) const
{
	return(m_stateInfoArray[uiState].m_passArray.GetSize());
}


void MeshProtocol::enableColorSet( unsigned int uiSet )
{
	m_currentColorSet = uiSet;
}

const unsigned int MeshProtocol::numRequiredAlphaBits( void ) const
{
	return( 8 );
}
// -------------------------------------------------------------------------


VUStateProtocol &MeshProtocol::activateState( unsigned int uiState, unsigned int uiPass )
{
	m_activeState.m_stateIndex = uiState;
	m_activeState.m_passIndex = uiPass;
	m_activeState.m_activeStatePoint = 0;
	m_activeState.m_pointIndex = 0;
	m_activeState.m_stripIndex = m_stateInfoArray[uiState].m_startStripIndex;
	m_activeState.m_faceStage = 0;
	return peekVUState(uiState,uiPass);
}

void MeshProtocol::SetMesh(void)
{
	m_bRealMeshBones = false;
	if (m_boneBuilder.GetNumBones())
		m_bRealMeshBones = true;

	BuildMaterialInfo();
	BuildMeshInfo();
	BuildLightingInfo();
	BuildVertexColorInfo();
	BuildTriStripInfo();

	// compute the number of states and passes we will need to do this mesh
	// the tristrips are already sorted properly for display
	// a new state is determined based on the material index
	// states attempt to combine strips that use the same info

	assert(m_nTotalTriStrips>0);
	int stripIndex;
	for (stripIndex = 0;stripIndex<m_nTotalTriStrips;stripIndex++)
	{
		TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
		int numStages = triStripHeader.m_stages.GetSize();
		int faceFlags = triStripHeader.m_stages[0].m_faceFlags & (TupTriMesh::FLAGS_FACE_NO_Z_WRITE);

		// start with this strip
		int numStateStrips = 1;
		int numStateStripPoints = triStripHeader.m_numPoints;
		// count other strips that will share the same state
		int nextStateStrip;
		for (nextStateStrip=stripIndex+1;nextStateStrip<m_nTotalTriStrips;nextStateStrip++)
		{
			TriStripHeader &checkTriStripHeader = m_triStripHeaderArray[nextStateStrip];
			int checkNumStages = checkTriStripHeader.m_stages.GetSize();

			bool bStripsSame = true;
			if (numStages==checkNumStages)
			{
				// make sure all the stages are the same
				for (int stageIndex=0;stageIndex<numStages;stageIndex++)
				{
					int materialIndex = triStripHeader.m_stages[stageIndex].m_materialIndex;
					int faceFlags = triStripHeader.m_stages[stageIndex].m_faceFlags & (TupTriMesh::FLAGS_FACE_NO_Z_WRITE);
					int checkMaterialIndex = checkTriStripHeader.m_stages[stageIndex].m_materialIndex;
					int checkFaceFlags = checkTriStripHeader.m_stages[stageIndex].m_faceFlags & (TupTriMesh::FLAGS_FACE_NO_Z_WRITE);
					if (checkMaterialIndex!=materialIndex ||
						checkFaceFlags!=faceFlags)
					{
						bStripsSame = false;
						break;
					}
				}
			}
			else
			{
				bStripsSame = false;
			}

			if (bStripsSame)
			{
				numStateStrips++;
				numStateStripPoints+= checkTriStripHeader.m_numPoints;
			}
			else
			{
				break; // stop when the end is found
			}
		}
		StateInfo stateInfo;
		stateInfo.m_numPoints = numStateStripPoints; // total number of points in all the strips in this state
		stateInfo.m_numStrips = numStateStrips; // total number of strips that were combined in this state
		stateInfo.m_startStripIndex = stripIndex; // index into m_triStripHeaderArray for first strip of state

		// generate pass information
		// loop through the number of stages
		bool	bEVMapped = false;
		int lastPassIndex = -1;

		for (int stageIndex=0;stageIndex<numStages;stageIndex++)
		{
			TriStripStage &stage = triStripHeader.m_stages[stageIndex];
			int numLayers = m_textureContextArray[stage.m_materialIndex].TextureCount;
			for (int layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				ts_FullTextureContext &textureContext = m_textureContextArray[stage.m_materialIndex];

				if( textureContext.TextureLayer[layerIndex].u8Flags & TEXTURE_IS_ENVIRONMENT_MAP)
					bEVMapped = true;
				else
				{
					PassInfo passInfo;
					passInfo.m_stageIndex = stageIndex;
					passInfo.m_layerIndex = layerIndex;

					// set up the meshPassInfo data
					// set up the uv stream index
					TupArray<UVTransform> &uvTransforms = m_UVTransformArray[stage.m_materialIndex];
					passInfo.m_meshPassInfo.idxTexCoordStream = AddUVStream(stateInfo.m_uvStreams,uvTransforms[layerIndex],layerIndex,stageIndex);
					// set the color stream to the stage # or -1 if we are outputting colors or not
					passInfo.m_meshPassInfo.idxRGBAStream = m_bMeshVertexColors ? stageIndex : -1;
					// set normal stream to the stage # or -1 depending on if we are outputting normals or not
					passInfo.m_meshPassInfo.idxNormalStream = m_bMeshNormals ? stageIndex : -1; 

					passInfo.m_meshPassInfo.mPassType = MeshPassInfo::PASS_TYPE_NORMAL; // for now
					passInfo.m_meshPassInfo.mPassFlags = MeshPassInfo::PASS_FLAG_DYNAMIC_TEXTURE; // for now

					// index into texture pool (used for minimizing texture switches)
					passInfo.m_meshPassInfo.m_textureID = (int)textureContext.TextureLayer[layerIndex].s16TextureIndex;

					// set passInfo with no z write flag
					if (faceFlags & TupTriMesh::FLAGS_FACE_NO_Z_WRITE)
					{
						passInfo.m_meshPassInfo.mPassFlags |= MeshPassInfo::PASS_FLAG_NO_Z_WRITE; 
					}

					// set up the meshstate data
					// get texture context data
					u32 textureContextID = m_textureContextIDArray[stage.m_materialIndex];
				
					passInfo.m_meshState.setShadingMode( VUStateProtocol::MODE_SHADE_GOURAUD );
					passInfo.m_meshState.setTextureContext((ts_TextureContext *) &textureContext, textureContextID );
					passInfo.m_meshState.setTextureMode( VUStateProtocol::MODE_DECAL_TEXTURED );
				
					passInfo.m_meshState.setConstantColor( *(unsigned int *)&textureContext.BaseColor );
					passInfo.m_meshState.setTextureLayer( passInfo.m_layerIndex );

					// add the pass to the state
					lastPassIndex = stateInfo.m_passArray.Add(passInfo);
				}
			}
		}

		if( bEVMapped)
		{
			assert( lastPassIndex != -1);

			PassInfo &lastPassInfo = stateInfo.m_passArray[lastPassIndex];

			lastPassInfo.m_meshPassInfo.mPassFlags |= MeshPassInfo::PASS_FLAG_ENVIRONMENT_MAPPED; 
			lastPassInfo.m_meshPassInfo.idxNormalStream = 0;
		}

		m_stateInfoArray.Add(stateInfo);
		stripIndex = nextStateStrip-1; // so we will be nextStateStrip next run
	}
	// setup sort keys (try to group other passes down based on texture)
	SetupSortKeys();

	// cleanup any old allocations
	MeshState::clearFirstPass();
}

void MeshProtocol::SetupSortKeys(void)
{
	int numStates = m_stateInfoArray.GetSize();
	int stateIndex;
	int passIndex;
	int sortKeyValue = 0;

	// sort pass 0 to be first
	for (stateIndex=0;stateIndex<numStates;stateIndex++)
	{
		m_stateInfoArray[stateIndex].m_passArray[0].m_meshPassInfo.m_sortKey = sortKeyValue++;
	}
	// place others after it in pass order
	for (stateIndex=0;stateIndex<numStates;stateIndex++)
	{
		StateInfo &stateInfo = m_stateInfoArray[stateIndex];
		int numPasses = stateInfo.m_passArray.GetSize();
		for (passIndex=1;passIndex<numPasses;passIndex++)
		{
			stateInfo.m_passArray[passIndex].m_meshPassInfo.m_sortKey = sortKeyValue++;
		}
	}
}

const Position3<float> MeshProtocol::min( void ) const
{
	Position3<float> 	ret( FLT_MAX, FLT_MAX, FLT_MAX);

	int activeState = m_activeState.m_stateIndex;

	int startStrip = m_stateInfoArray[activeState].m_startStripIndex;
	int endStrip = startStrip + m_stateInfoArray[activeState].m_numStrips;

	int stripIndex;
	for (stripIndex=startStrip;stripIndex<endStrip;stripIndex++)
	{
		const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
		int numPoints = triStripHeader.m_numPoints;
		int pointIndex;
		for (pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			int faceIndex = triStripHeader.m_pFaceIndices[pointIndex];
			int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];
			// get the position (lookup by face and point on face)
			Position3<float> &pos = m_pPositionVerts[m_pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex]];

			if( pos.x() < ret.x() )
				ret.x() = pos.x();

			if( pos.y() < ret.y() )
				ret.y() = pos.y();

			if( pos.z() < ret.z() )
				ret.z() = pos.z();
		}
	}
	return Position3<float>(ret);
}

const Position3<float> MeshProtocol::max( void ) const
{
	Position3<float> 	ret( -FLT_MAX, -FLT_MAX, -FLT_MAX);

	int activeState = m_activeState.m_stateIndex;

	int startStrip = m_stateInfoArray[activeState].m_startStripIndex;
	int endStrip = startStrip + m_stateInfoArray[activeState].m_numStrips;

	int stripIndex;
	for (stripIndex=startStrip;stripIndex<endStrip;stripIndex++)
	{
		const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
		int numPoints = triStripHeader.m_numPoints;
		int pointIndex;
		for (pointIndex=0;pointIndex<numPoints;pointIndex++)
		{
			int faceIndex = triStripHeader.m_pFaceIndices[pointIndex];
			int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];
			// get the position (lookup by face and point on face)
			Position3<float> &pos = m_pPositionVerts[m_pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex]];

			if( pos.x() > ret.x() )
				ret.x() = pos.x();

			if( pos.y() > ret.y() )
				ret.y() = pos.y();

			if( pos.z() > ret.z() )
				ret.z() = pos.z();
		}
	}
	return Position3<float>(ret);
}

const Position3<float> MeshProtocol::dim( void ) const
{

	Position3<float> minimum, maximum;

	minimum = min( );
	maximum = max( );

	return Position3<float>( Math::Abs( maximum.x( ) - minimum.x( )), 
							 Math::Abs( maximum.y( ) - minimum.y( )), 
							 Math::Abs( maximum.z( ) - minimum.z( )));
}

 // update the strip index and point index based on index within the state
void MeshProtocol::UpdateActiveState(unsigned int statePointIndex) const
{
	// if this is the same then we are done
	if (statePointIndex==m_activeState.m_activeStatePoint)
	{
		return;
	}
	// if the request is > than the last one
	else if (statePointIndex>(unsigned int)m_activeState.m_activeStatePoint)
	{
		int activeState = m_activeState.m_stateIndex;

		int startStrip = m_stateInfoArray[activeState].m_startStripIndex;
		int endStrip = startStrip + m_stateInfoArray[activeState].m_numStrips;

		int difference = statePointIndex - m_activeState.m_activeStatePoint;
		int numPointsInActiveStrip = m_triStripHeaderArray[m_activeState.m_stripIndex].m_numPoints;
		// if going into another strip
		while(m_activeState.m_pointIndex+difference>=numPointsInActiveStrip)
		{
			difference-=numPointsInActiveStrip-m_activeState.m_pointIndex; // take off points till the end of the strip

			m_activeState.m_stripIndex++; // go to the next strip
			assert(m_activeState.m_stripIndex<endStrip); // better not be outside of our strips
			m_activeState.m_pointIndex = 0; // start at the beginning of the next strip
			numPointsInActiveStrip = m_triStripHeaderArray[m_activeState.m_stripIndex].m_numPoints; // get total for next strip
		}
		// adjust to the correct point within the strip
		m_activeState.m_pointIndex += difference;
		m_activeState.m_activeStatePoint = statePointIndex;
	}
	// if the request is < than the last one
	else
	{
		int activeState = m_activeState.m_stateIndex;

		int startStrip = m_stateInfoArray[activeState].m_startStripIndex;

		int difference = m_activeState.m_activeStatePoint - statePointIndex;
		// if going into another strip
		while(m_activeState.m_pointIndex<difference)
		{
			difference-=(m_activeState.m_pointIndex+1); // take off points at the beginning of the strip and get us to the end of previous

			m_activeState.m_stripIndex--; // go to the next strip
			assert(m_activeState.m_stripIndex>=startStrip); // better not be outside of our strips
			m_activeState.m_pointIndex = m_triStripHeaderArray[m_activeState.m_stripIndex].m_numPoints-1; // start at the end of the previous strip
		}
		// adjust to the correct point within the strip
		m_activeState.m_pointIndex -= difference;
		m_activeState.m_activeStatePoint = statePointIndex;
	}
}

const unsigned int MeshProtocol::idxMatrix( unsigned int uivtx) const
{
	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state

	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;

	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	int faceIndex = triStripHeader.m_pFaceIndices[pointIndex];
	int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];
	// get the bone index for the point
	if (m_bRealMeshBones)
	{
		int vertexIndex = m_pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex];
		const TupArray<int> &boneVerts = m_boneBuilder.GetBoneVerts();
		return boneVerts[vertexIndex];
	}
	return 0; // all use first one for now
}

const unsigned int MeshProtocol::numMatrices( void) const
{
	if (m_bRealMeshBones)
	{
		return m_boneBuilder.GetNumBones();
	}
	else if (m_bFakeMeshBone)
	{
		return 1; // if we have fake bones then return 1 matrix
	}
	else
	{
		return 0;
	}
}

const unsigned int MeshProtocol::idx( unsigned int uivtx) const
{
	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state

	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;

	// get object that holds the pointers for the strip face and face point
	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	// if last point of this strip then set the flag
	if (pointIndex==triStripHeader.m_numPoints-1)
		uivtx|=IDX_END_OF_STRIP;
	return uivtx; // return index with end of strip flag set
}


const Position3<float> & MeshProtocol::vtx( unsigned int uivtx) const
{
	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state

	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;

	// get object that holds the pointers for the strip face and face point
	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	int faceIndex = triStripHeader.m_pFaceIndices[pointIndex];
	int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];
	// get the position (lookup by face and point on face)
	m_vertex = m_pPositionVerts[m_pPositionIndices[(faceIndex<<1)+faceIndex+facePointIndex]];

	return m_vertex;
}

const Position3<float> & MeshProtocol::nrm( unsigned short usStreamIdx, unsigned int uivtx ) const
{
	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state

	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;

	// get object that holds the pointers for the strip face and face point
	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	int faceIndex = m_stageFaceManager.GetStageFace(triStripHeader.m_pFaceIndices[pointIndex],usStreamIdx);
	assert(faceIndex!=-1);
	int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];

	// if whole mesh has normals or need them for environment mapping
	if (m_bMeshNormals || m_pSubMaterialHasEnvironmentMapValues[triStripHeader.m_materialIndex])
	{
		m_vertex = m_pNormalVerts[m_pNormalIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
		// added code to make sure the normal is normalized (if this is not needed it will be removed)
		
	#if 0
		float length = Math::Sqrt(m_vertex.x()*m_vertex.x()+m_vertex.y()*m_vertex.y()+m_vertex.z()*m_vertex.z());
		// if length is 0
		if (Math::DivByZero(1.0f, length))
		{
			m_vertex.set(0.0f,1.0f,0.0f); // if no normals then set to y up
		}
		else
		{
			float invLength = 1.0f / length;
			m_vertex *= invLength;
		}
	#endif
		
	}
	else
	{
		m_vertex.set(0.0f,0.0f,0.0f);
	}
	return m_vertex;
}

const BaseColor<unsigned short> & MeshProtocol::clr(unsigned short usStreamIdx, unsigned int uivtx) const
{

	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state

	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;

	// start with texture context color
	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	const ts_FullTextureContext &textureContext = m_textureContextArray[triStripHeader.m_stages[usStreamIdx].m_materialIndex];
	ts_bRGBA color = textureContext.BaseColor; // start with the base color

	// the stream index is set up to be the stage of the face
	int faceIndex = m_stageFaceManager.GetStageFace(triStripHeader.m_pFaceIndices[pointIndex],usStreamIdx);
	assert(faceIndex!=-1);
	int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];

	// if there are color sets to do
	if (m_currentColorSet<m_nNumColorSets)
	{
		if (m_vertexColorSetsArray.GetSize())
		{
			const VertexColorMap &vertexColorMap = m_vertexColorSetsArray[m_currentColorSet];
			if (vertexColorMap.m_pVertexColorVerts && vertexColorMap.m_pVertexColorIndices)
			{
				// get object that holds the pointers for the strip face and face point
				assert(faceIndex<vertexColorMap.m_numVertexColorFaces);
				assert(vertexColorMap.m_pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex]<vertexColorMap.m_numVertexColorVerts);
				Position3<float> &colorRef = vertexColorMap.m_pVertexColorVerts[vertexColorMap.m_pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
				color.R = (u8) (colorRef.x()*255.0f);
				color.G = (u8) (colorRef.y()*255.0f);
				color.B = (u8) (colorRef.z()*255.0f);
			}
		}
		if (m_vertexAlphaSetsArray.GetSize())
		{
			const VertexColorMap &vertexColorMap = m_vertexAlphaSetsArray[m_currentColorSet];
			if (vertexColorMap.m_pVertexColorVerts && vertexColorMap.m_pVertexColorIndices)
			{
				// get object that holds the pointers for the strip face and face point
				assert(faceIndex<vertexColorMap.m_numVertexColorFaces);
				assert(vertexColorMap.m_pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex]<vertexColorMap.m_numVertexColorVerts);
				Position3<float> &colorRef = vertexColorMap.m_pVertexColorVerts[vertexColorMap.m_pVertexColorIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
				color.A = (u8) (colorRef.x()*255.0f);
			}
		}
	}

	m_color.Set(PS2_HALF_COLOR(color.R),
				PS2_HALF_COLOR(color.G),
				PS2_HALF_COLOR(color.B),
				PS2_HALF_COLOR(color.A));

	return m_color;
}

const Position2<float> & MeshProtocol::uv( unsigned short usStreamIdx, unsigned int uivtx) const
{
	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state

	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;

	const StateInfo &stateInfo = m_stateInfoArray[activeState];
	const UVStream &uvStream = stateInfo.m_uvStreams[usStreamIdx];
	int layerIndex = uvStream.m_layerIndex;
	int stageIndex = uvStream.m_stageIndex;

	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	int faceIndex = m_stageFaceManager.GetStageFace(triStripHeader.m_pFaceIndices[pointIndex],stageIndex);
	assert(faceIndex!=-1);
	int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];

	// get the uv offset from the original tri strip data
	const Vector2 &uvOffset = triStripHeader.m_stages[stageIndex].m_stripUVOffsetArray[layerIndex];

	const UVTransform &uvTransform = uvStream.m_uvTransform;
	int mapChannel = uvTransform.m_mapChannel;
	int uvMapIndex = m_uvChannelToIndexKeeper.GetIndex(mapChannel);
	// if this channel exists in the mesh
	if (uvMapIndex!=-1)
	{
		// get the position (lookup by face and point on face)
		Vector3 tempVert = m_uvMaps[uvMapIndex].m_pUVVerts[m_uvMaps[uvMapIndex].m_pUVIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
		ApplyUVTransform::Apply(uvTransform,tempVert);

		m_UV.x() = (tempVert.x( ) - uvOffset.x( ) ) * 0.0f; // this is what is was originally doing
		m_UV.y() = (tempVert.y( ) - uvOffset.y( ) ) * 0.0f; // this is what is was originally doing
	}
	// if there is no vert channel then set to 0
	else
	{
		m_UV.x() = 0.0f;
		m_UV.y() = 0.0f; 
	}
	return m_UV;
}

const Position2<float> & MeshProtocol::st( unsigned short usStreamIdx, unsigned int uivtx) const
{
	UpdateActiveState(uivtx); // update the strip index and point index based on index within the state
	
	int activeState = m_activeState.m_stateIndex;
	int stripIndex = m_activeState.m_stripIndex;
	int pointIndex = m_activeState.m_pointIndex;
	
	const StateInfo &stateInfo = m_stateInfoArray[activeState];
	const UVStream &uvStream = stateInfo.m_uvStreams[usStreamIdx];
	int layerIndex = uvStream.m_layerIndex;
	int stageIndex = uvStream.m_stageIndex;
	
	const TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
	int faceIndex = m_stageFaceManager.GetStageFace(triStripHeader.m_pFaceIndices[pointIndex],stageIndex);
	assert(faceIndex!=-1);
	int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];
	
	// get the uv offset from the original tri strip data
	const Vector2 &uvOffset = triStripHeader.m_stages[stageIndex].m_stripUVOffsetArray[layerIndex];
	
	const UVTransform &uvTransform = uvStream.m_uvTransform;
	int mapChannel = uvTransform.m_mapChannel;
	int uvMapIndex = m_uvChannelToIndexKeeper.GetIndex(mapChannel);
	// if this channel exists in the mesh
	if (uvMapIndex!=-1)
	{
		// get the position (lookup by face and point on face)
		Vector3 tempVert = m_uvMaps[uvMapIndex].m_pUVVerts[m_uvMaps[uvMapIndex].m_pUVIndices[(faceIndex<<1)+faceIndex+facePointIndex]];
		ApplyUVTransform::Apply(uvTransform,tempVert);

		m_UV.x() = (tempVert.x( ) - uvOffset.x( ) );
		m_UV.y() = (tempVert.y( ) - uvOffset.y( ) );

#if 0
		float texelU = uvStream.m_uvTransform.m_imageWidth*(tempVert.x() - uvOffset.x());
		float texelV = uvStream.m_uvTransform.m_imageHeight*(tempVert.y() - uvOffset.y());
		assert(texelU>=-2048.0f && texelU<=2048.0f);
		assert(texelV>=-2048.0f && texelV<=2048.0f);
#endif
	}
	// if there is no vert channel then set to 0
	else
	{
		m_UV.x() = 0.0f;
		m_UV.y() = 0.0f; 
	}
	return m_UV;
}

const unsigned int MeshProtocol::numColorSets(void) const
{ 
	return(m_nNumColorSets); 
} 

// this creates an array of ints and fills it with the proper texture context ids for the mesh
// the id's are respective to the entire material pool and not with the ones used just for this mesh
void MeshProtocol::BuildTextureContextIDs(int materialReference)
{
	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialReference!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialReference);

	if (pMaterialAgg)
	{
		TupMaterial tupMaterial(pMaterialAgg);

		if ( tupMaterial.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMaterial);
			int totalTextureContextIDs = tupMultiSubObjectMaterial.GetNumSubMaterials();
			m_textureContextIDArray.SetSize(totalTextureContextIDs);
			if (totalTextureContextIDs)
			{
				for (int materialIndex = 0;materialIndex<totalTextureContextIDs;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
						{
							// get id from material (this better be a standard material for now)
							TupperwareList *pTextureContextIDList = pMaterialAgg->FindListByKey("Annotate_TextureContextIDs");
							if (pTextureContextIDList)
							{
								int *pTextureContextIDs = pTextureContextIDList->GetAsInt();
								int textureID = pTextureContextIDs[m_currentPartition];
								assert(textureID!=-1);
								m_textureContextIDArray[materialIndex] = textureID;
							}
							else
							{
								if (m_currentPartition==0)
									m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
								else
									m_textureContextIDArray[materialIndex] = 0xff000000; // use the blank one from world partition
							}
						}
						else
						{
							if (m_currentPartition==0)
								m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
							else
								m_textureContextIDArray[materialIndex] = 0xff000000; // use the blank one from world partition
							char *pMultiName;
							tupMultiSubObjectMaterial.GetName(&pMultiName);
							TupperwareScalar *pSubMatNameScalar = tupSubMat.GetBaseAggregate()->FindScalarByKey("Name");
							if (pSubMatNameScalar)
							{
								PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
									"Warning - Multi-Sub Object Material %s referencing non standard material %s in slot %d",
									pMultiName,pSubMatNameScalar->GetAsString(),materialIndex);
							}
						}
					}
					else
					{
						if (m_currentPartition==0)
							m_textureContextIDArray[materialIndex] = 0; // use the blank one if not set (this slot should not be used)
						else
							m_textureContextIDArray[materialIndex] = 0xff000000; // use the blank one from world partition
					}
				}
			}
			else // if there are no submaterials on the multi-sub material
			{
				m_textureContextIDArray.SetSize(1);
				m_textureContextIDArray[0] = 0; // use the first one (no texture)
			}
		}
		else if ( tupMaterial.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
		{
			// just deal with 1 material
			m_textureContextIDArray.SetSize(1);
			// get id from material (this better be a standard material for now)
			TupperwareList *pTextureContextIDList = pMaterialAgg->FindListByKey("Annotate_TextureContextIDs");
			if (pTextureContextIDList)
			{
				int *pTextureContextIDs = pTextureContextIDList->GetAsInt();
				int textureID = pTextureContextIDs[m_currentPartition];
				assert(textureID!=-1);
				m_textureContextIDArray[0] = textureID;
			}
			else
			{
				if (m_currentPartition==0)
					m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
				else
					m_textureContextIDArray[0] = 0xff000000; // use the blank one from world partition
			}
		}
		else // any other materials
		{
			// just deal with 1 material
			m_textureContextIDArray.SetSize(1);
			if (m_currentPartition==0)
				m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
			else
				m_textureContextIDArray[0] = 0xff000000; // use the blank one from world partition
			TupperwareScalar *pMatNameScalar = pMaterialAgg->FindScalarByKey("Name");
			if (pMatNameScalar)
			{
				PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Unknown material %s",pMatNameScalar->GetAsString());
			}
		}
	}
	else // not using a material (-1)
	{
		// just deal with 1 material
		m_textureContextIDArray.SetSize(1);
		if (m_currentPartition==0)
			m_textureContextIDArray[0] = 0; // use the blank one if not set (this slot should not be used)
		else
			m_textureContextIDArray[0] = 0xff000000; // use the blank one from world partition
//		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,"Warning - Using Material Reference of -1");
	}
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void MeshProtocol::BuildTextureContexts(int materialRef)
{
	BuildTextureContextManager buildTextureContextManager(m_dataManager,m_tupMaterialPool,m_tupMapPool,m_tupImagePool,m_currentPartition,true);
	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);

	if (pMaterialAgg)
	{
		TupMaterial tupMat(pMaterialAgg);
		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalTextureContexts = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalTextureContexts)
			{
				m_textureContextArray.SetSize(totalTextureContexts);
				for (int materialIndex = 0;materialIndex<totalTextureContexts;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						buildTextureContextManager.BuildTextureContext(tupSubMat,m_textureContextArray[materialIndex]);
					}
					else
					{
						buildTextureContextManager.BuildDummyTextureContext(m_textureContextArray[materialIndex]);
					}
				}
			}
			else
			{
				// just deal with 1 material
				m_textureContextArray.SetSize(1);
				buildTextureContextManager.BuildDummyTextureContext(m_textureContextArray[0]);
			}
		}
		else
		{
			// just deal with 1 material
			m_textureContextArray.SetSize(1);
			buildTextureContextManager.BuildTextureContext(tupMat,m_textureContextArray[0]);
		}
	}
	else
	{
		m_textureContextArray.SetSize(1);
		buildTextureContextManager.BuildDummyTextureContext(m_textureContextArray[0]);
	}
}

// this creates an array of ints and fills it with the proper texture context ids for the mesh
void MeshProtocol::BuildUVTransformArray(int materialRef)
{
	BuildUVTransformManager buildUVTransformManager(m_tupMaterialPool,m_tupMapPool,m_tupImagePool,true);

	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);

	if (pMaterialAgg)
	{
		TupMaterial tupMat(pMaterialAgg);

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalUVTransforms = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalUVTransforms)
			{
				m_UVTransformArray.SetSize(totalUVTransforms);
				for (int materialIndex = 0;materialIndex<totalUVTransforms;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int subMaterialRef;
					tupSubMaterial.GetMaterialReference(subMaterialRef);
					buildUVTransformManager.BuildUVTransforms(subMaterialRef,m_UVTransformArray[materialIndex]);
				}
			}
			else
			{
				m_UVTransformArray.SetSize(1);
				m_UVTransformArray[0].SetSize(1);
				m_UVTransformArray[0][0].Init();
			}
		}
		else
		{
			m_UVTransformArray.SetSize(1);
			buildUVTransformManager.BuildUVTransforms(materialRef,m_UVTransformArray[0]);
		}
	}
	else
	{
		m_UVTransformArray.SetSize(1);
		m_UVTransformArray[0].SetSize(1);
		m_UVTransformArray[0][0].Init();
	}
}

void MeshProtocol::BuildFlagsArray(int materialRef)
{
	TupperwareAggregate *pMaterialAgg = NULL;
	if (materialRef!=-1)
		pMaterialAgg = m_tupMaterialPool.GetMaterial(materialRef);

	if (pMaterialAgg)
	{
		TupMaterial tupMat(pMaterialAgg);

		if ( tupMat.GetMaterialType( ) == TupMaterial::MULTI_SUB_OBJECT_MATERIAL )
		{
			// we need to deal with multiple materials
			TupMultiSubObjectMaterial tupMultiSubObjectMaterial(tupMat);
			int totalMaterialFlags = tupMultiSubObjectMaterial.GetNumSubMaterials();
			if (totalMaterialFlags)
			{
				m_materialFlagsArray.SetSize(totalMaterialFlags);
				for (int materialIndex = 0;materialIndex<totalMaterialFlags;materialIndex++)
				{
					TupperwareAggregate *pSubMaterialAgg = tupMultiSubObjectMaterial.GetSubMaterial(materialIndex);
					TupMultiSubObjectMaterialSubMaterial tupSubMaterial(pSubMaterialAgg);
					int materialPoolIndex;
					tupSubMaterial.GetMaterialReference(materialPoolIndex);
					m_materialFlagsArray[materialIndex] = 0; // default
					if (materialPoolIndex!=-1)
					{
						TupperwareAggregate *pMaterialAgg = m_tupMaterialPool.GetMaterial( materialPoolIndex );
						TupMaterial tupSubMat( pMaterialAgg );
						if (tupSubMat.GetMaterialType()==TupMaterial::STANDARD_MATERIAL)
						{
							TupStandardMaterial tupStandardMaterial(pMaterialAgg);
							int flags;
							tupStandardMaterial.GetFlags(flags);
							m_materialFlagsArray[materialIndex] = flags;
						}
					}
				}
			}
			else
			{
				m_materialFlagsArray.SetSize(1);
				m_materialFlagsArray[0] = 0; 
			}
		}
		else if ( tupMat.GetMaterialType( ) == TupMaterial::STANDARD_MATERIAL )
		{
			// just deal with 1 material
			m_materialFlagsArray.SetSize(1);
			TupStandardMaterial tupStandardMaterial(tupMat);
			int flags;
			tupStandardMaterial.GetFlags(flags);
			m_materialFlagsArray[0] = flags;
		}
		else // any other materials
		{
			m_materialFlagsArray.SetSize(1);
			m_materialFlagsArray[0] = 0; 
		}
	}
	else
	{
		m_materialFlagsArray.SetSize(1);
		m_materialFlagsArray[0] = 0; 
	}
}

void MeshProtocol::BuildVertexColorInfo(void)
{
	TupperwareAggregate *pMeshAgg = m_tupTriMesh.GetBaseAggregate();
	// search for the vertex color reference list
	// set up vertex color info so we can later determine if we need to increment the vertex color offsets
	int *pVertexColorRefData = NULL;
	int *pVertexAlphaRefData = NULL;
	m_nNumColorSets = 0;
	if (m_bMeshVertexColors)
	{
		TupperwareList *pVertexColorRefList = pMeshAgg->FindListByKey("Annotate_VertexColorRefList");
		TupperwareList *pVertexAlphaRefList = pMeshAgg->FindListByKey("Annotate_VertexAlphaRefList");
		if (pVertexColorRefList && pVertexAlphaRefList)
		{
			pVertexColorRefData = pVertexColorRefList->GetAsInt();
			pVertexAlphaRefData = pVertexAlphaRefList->GetAsInt();
			m_nNumColorSets = pVertexColorRefList->GetLength();
			// check if all references are pointing to a -1 reference (standard white)
			if (m_nNumColorSets==1 && pVertexColorRefData[0] == -1 && pVertexAlphaRefData[0] == -1)
			{
				// lets say there are no vertex color sets
				m_nNumColorSets = 0;
			}
		}
	}

	// set up the vertex color sets
	if (m_nNumColorSets)
	{
		m_vertexColorSetsArray.SetSize(m_nNumColorSets);
		m_vertexAlphaSetsArray.SetSize(m_nNumColorSets);

		for (int colorSetIndex = 0;colorSetIndex<m_nNumColorSets;colorSetIndex++)
		{
			VertexColorMap &vertexColorMap = m_vertexColorSetsArray[colorSetIndex];
			VertexColorMap &vertexAlphaMap = m_vertexAlphaSetsArray[colorSetIndex];

			int colorObjectRef = pVertexColorRefData[colorSetIndex];
			int alphaObjectRef = pVertexAlphaRefData[colorSetIndex];

			if (colorObjectRef!=-1) // if valid object reference
			{
				TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexColorMeshMap(m_tupObjectPool,colorObjectRef);
				assert(pChildMeshMapAgg);

				TupTriMeshMap tupTriMeshMap(pChildMeshMapAgg);

				tupTriMeshMap.GetMapVerts((float **) &vertexColorMap.m_pVertexColorVerts, vertexColorMap.m_numVertexColorVerts );
				tupTriMeshMap.GetMapIndices( &vertexColorMap.m_pVertexColorIndices, vertexColorMap.m_numVertexColorFaces );
				assert(vertexColorMap.m_numVertexColorFaces == m_numFaces);
			}
			else
			{
				// make this entry not point to any data
				vertexColorMap.m_numVertexColorFaces=0;
				vertexColorMap.m_numVertexColorVerts=0;
				vertexColorMap.m_pVertexColorVerts=NULL;
				vertexColorMap.m_pVertexColorIndices=NULL;
			}
			if (alphaObjectRef!=-1) // if valid object reference
			{
				TupperwareAggregate *pChildMeshMapAgg = AnnotateHelper::FindChildVertexAlphaMeshMap(m_tupObjectPool,alphaObjectRef);
				assert(pChildMeshMapAgg);

				TupTriMeshMap tupTriMeshMap(pChildMeshMapAgg);

				tupTriMeshMap.GetMapVerts((float **) &vertexAlphaMap.m_pVertexColorVerts, vertexAlphaMap.m_numVertexColorVerts );
				tupTriMeshMap.GetMapIndices( &vertexAlphaMap.m_pVertexColorIndices, vertexAlphaMap.m_numVertexColorFaces );
				assert(vertexAlphaMap.m_numVertexColorFaces == m_numFaces);
			}
			else
			{
				// make this entry not point to any data
				vertexAlphaMap.m_numVertexColorFaces=0;
				vertexAlphaMap.m_numVertexColorVerts=0;
				vertexAlphaMap.m_pVertexColorVerts=NULL;
				vertexAlphaMap.m_pVertexColorIndices=NULL;
			}
		}
	}
	else
	{
		m_vertexColorSetsArray.RemoveAll(); // no sets
		m_vertexAlphaSetsArray.RemoveAll();
	}
}

void MeshProtocol::BuildMaterialInfo(void)
{
	TupperwareAggregate *pMeshAgg = m_tupTriMesh.GetBaseAggregate();
	// search for the material list
	int materialRef = pMeshAgg->FindScalarByKey("Annotate_MaterialRef")->GetAsInt();

	// get the texture context ids for the materials used in the mesh
	m_textureContextIDArray.RemoveAll();
	m_UVTransformArray.RemoveAll();
	m_materialFlagsArray.RemoveAll();
	m_textureContextArray.RemoveAll();

	BuildTextureContextIDs(materialRef);
	BuildTextureContexts(materialRef);
	BuildUVTransformArray(materialRef);
	BuildFlagsArray(materialRef);
}

void MeshProtocol::BuildTriStripInfo(void)
{
	TupperwareAggregate *pTriangleStripsAgg = m_tupTriMesh.GetTriangleStrips();
	if (!pTriangleStripsAgg) throw(PluginException(ERROR_FATAL,"Triangle Strips not found"));

	m_pTupTriangleStripPool = new TupTriangleStripPool(pTriangleStripsAgg);

	m_nTotalTriStrips = m_pTupTriangleStripPool->GetNumTriangleStrips();

	if (!m_nTotalTriStrips) throw(PluginException(ERROR_FATAL,"Triangle Strips not found"));

	// allocate data for the tristrip headers
	m_triStripHeaderArray.SetSize(m_nTotalTriStrips);

	TupArray<BigUVStrip> bigStripData;
	// build the strip headers and add to the map based on the strip material index
	for (int stripIndex = 0;stripIndex<m_nTotalTriStrips;stripIndex++)
	{
		TriStripHeader &triStripHeader = m_triStripHeaderArray[stripIndex];
		TupperwareAggregate *pTriangleStripAgg = m_pTupTriangleStripPool->GetTriangleStrip(stripIndex);
		TupTriangleStrip tupTriangleStrip(pTriangleStripAgg);
		int numFaceIndices;
		int numFacePointIndices;
		tupTriangleStrip.GetFaceIndices(&triStripHeader.m_pFaceIndices,numFaceIndices);
		assert(numFaceIndices);
		tupTriangleStrip.GetFacePointIndices(&triStripHeader.m_pFacePointIndices,numFacePointIndices);
		assert(numFacePointIndices);
		assert(numFaceIndices==numFacePointIndices);
		triStripHeader.m_numPoints = numFaceIndices;
		int firstFaceIndex = triStripHeader.m_pFaceIndices[0];
		// get face flags
		triStripHeader.m_faceFlags = m_pFaceFlags[firstFaceIndex];
		// the materialIndex is the offset for the multi-subobject material for each face
		triStripHeader.m_materialIndex = m_pMaterialIndices[firstFaceIndex];
		// get the environment map opacity value
		triStripHeader.m_environmentMapOpacity = m_pSubMaterialEnvironmentMapOpacityValues[triStripHeader.m_materialIndex];;

		// put data into stage array
		int numStages = m_stageFaceManager.GetNumStages(firstFaceIndex);
		for (int stageIndex=0;stageIndex<numStages;stageIndex++)
		{
			TriStripStage triStripStage;
			int stageFaceIndex = m_stageFaceManager.GetStageFace(firstFaceIndex,stageIndex);
			// get face flags
			triStripStage.m_faceFlags = m_pFaceFlags[stageFaceIndex];
			// the materialIndex is the offset for the multi-subobject material for each face
			triStripStage.m_materialIndex = m_pMaterialIndices[stageFaceIndex];
			// make sure the the index fits within the total
			assert(triStripStage.m_materialIndex<m_textureContextIDArray.GetSize());
			TupArray<UVTransform> &uvTransforms = m_UVTransformArray[triStripStage.m_materialIndex];

			// set up the uv offsets for each layer
			int numLayers = uvTransforms.GetSize();
			triStripStage.m_stripUVOffsetArray.SetSize(numLayers);
			int layerIndex;
			for (layerIndex=0;layerIndex<numLayers;layerIndex++)
			{
				UVTransform &uvTransform = uvTransforms[layerIndex];
				int mapChannel = uvTransform.m_mapChannel;
				int uvMapIndex = m_uvChannelToIndexKeeper.GetIndex(mapChannel);
				Vector3 tempVert;
				// if this channel exists in the mesh
				if (uvMapIndex!=-1)
				{
					// get the position (lookup by face and point on face)
					ApplyUVTransform::Apply(uvTransform,tempVert);

					Vector2 newOffset;
					// set up the offsets for the tristrip uvs
					Vector2 vMinUV;
					Vector2 vMaxUV;
					vMinUV.Set(100000.0f, 100000.0f);
					vMaxUV.Set(-100000.0f, -100000.0f);
					// compute the min/max values for the strip uvs
					for (int pointIndex=0;pointIndex<numFacePointIndices;pointIndex++)
					{
						int faceIndex = triStripHeader.m_pFaceIndices[pointIndex];
						int stageFaceIndex = m_stageFaceManager.GetStageFace(faceIndex,stageIndex);
						int facePointIndex = triStripHeader.m_pFacePointIndices[pointIndex];
						Vector3 tempVert = m_uvMaps[uvMapIndex].m_pUVVerts[m_uvMaps[uvMapIndex].m_pUVIndices[(stageFaceIndex<<1)+stageFaceIndex+facePointIndex]];
						ApplyUVTransform::Apply(uvTransform,tempVert);

						// add to min/max
						if (tempVert.x( ) < vMinUV.x( ) )
							vMinUV.x( tempVert.x( ) );
						if (tempVert.y( ) < vMinUV.y( ) )
							vMinUV.y( tempVert.y( ) );
						if (tempVert.x( ) > vMaxUV.x( ) )
							vMaxUV.x( tempVert.x( ) );
						if (tempVert.y( ) > vMaxUV.y( ) )
							vMaxUV.y( tempVert.y( ) );
					}

					// if the mesh uses texture transform rotation then don't center the uvs
					if (!m_bUsesTextureTransformRot)
					{
						// calculate the uv offset (center uv range -+)
						newOffset = 0.5f * (vMinUV + vMaxUV);
						if (newOffset.x( ) > 0.0f)
							newOffset.x( (float) ((int) (newOffset.x( ) + 0.5f)));
						else
							newOffset.x( (float) ((int) (newOffset.x( ) - 0.5f)));
						if (newOffset.y( ) > 0.0f)
							newOffset.y( (float) ((int) (newOffset.y( ) + 0.5f)));
						else
							newOffset.y( (float) ((int) (newOffset.y( ) - 0.5f)));

						/* never normalize if range is 0 to 1-- screws up clamped textures. expand the bounds to test, since it */
						/* might be reasonable to have something a clamped texture run from -0.1 to 1.0 and be clamped from -0.1 */
						/* to 0.0. no normalization is needed in that range anyway */

						if ((vMinUV.x( ) >= -1.0f) && (vMaxUV.x( ) <= 2.0f))
							newOffset.x( 0.0f );
						if ((vMinUV.y( ) >= -1.0f) && (vMaxUV.y( ) <= 2.0f))
							newOffset.y( 0.0f );
					}
					else
					{
						newOffset.x( 0.0f );
						newOffset.y( 0.0f );
					}

					// set in the array
					triStripStage.m_stripUVOffsetArray[layerIndex] = newOffset;

					// test uv range and if too big then make sure there is a warning
					if (uvTransform.m_imageRef!=-1)
					{
						assert(uvTransform.m_imageHeight && uvTransform.m_imageWidth);
						float maxURange = 2048.0f/((float)uvTransform.m_imageWidth);
						float maxVRange = 2048.0f/((float)uvTransform.m_imageHeight);
						if (vMaxUV.x( ) - vMinUV.x( ) > maxURange ||
							vMaxUV.y( ) - vMinUV.y( ) > maxVRange)
						{
							BigUVStrip newBigUVStrip;
							newBigUVStrip.m_stageIndex = stageIndex;
							newBigUVStrip.m_layerIndex = layerIndex;
							newBigUVStrip.m_numPoints = numFacePointIndices;
							newBigUVStrip.m_imageHeight = uvTransform.m_imageHeight;
							newBigUVStrip.m_imageWidth = uvTransform.m_imageWidth;
							newBigUVStrip.m_minUV = vMinUV;
							newBigUVStrip.m_maxUV = vMaxUV;
							newBigUVStrip.m_uvOffset = newOffset;
							newBigUVStrip.m_imageRef = uvTransform.m_imageRef;

							bigStripData.Add(newBigUVStrip);
						}
					}

				}
				// if there is no vert channel then set to 0
				else
				{
					triStripStage.m_stripUVOffsetArray[layerIndex].Set(0.0f,0.0f);
				}
			}
			triStripHeader.m_stages.Add(triStripStage);
		}
	}
	// if any of the strips are a problem with respect to the UV...
	int numBigStrips = bigStripData.GetSize();
	if (numBigStrips)
	{
		const char *pNumber = m_tupTriMesh.GetBaseAggregate()->GetName();
		TupString name = "None";
		TupperwareScalar *pMayaObjectNameScalar = m_tupTriMesh.GetBaseAggregate()->FindScalarByKey("MayaObjectName");
		if (pMayaObjectNameScalar)
		{
			name = pMayaObjectNameScalar->GetAsString();
		}
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"Warning - Mesh #%s %s has %d strips that have a uv range of greater than 2048 pixels",pNumber,(const char *)name,numBigStrips);
#if 1
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"    Stage  Layer #Points   minU     maxU     minV   maxV  ImageWidth  ImageHeight  ImageName");
		for (int bigStripIndex=0;bigStripIndex<numBigStrips;bigStripIndex++)
		{
			BigUVStrip &bigUVStrip = bigStripData[bigStripIndex];

			TupperwareAggregate *imageAgg = m_tupImagePool.GetImage(bigUVStrip.m_imageRef);
			TupImage tupImage(imageAgg);
			char *pImagePathName;
			tupImage.GetFilePathName(&pImagePathName);

			PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
			"   %4d     %2d    %4d  % 5.4f  % 5.4f  % 5.4f   % 5.4f  %6d       %6d  %s",
			bigUVStrip.m_stageIndex,
			bigUVStrip.m_layerIndex,
			bigUVStrip.m_numPoints,
			bigUVStrip.m_minUV.x( ) - bigUVStrip.m_uvOffset.x( ),
			bigUVStrip.m_maxUV.x( ) - bigUVStrip.m_uvOffset.x( ),
			bigUVStrip.m_minUV.y( ) - bigUVStrip.m_uvOffset.y( ),
			bigUVStrip.m_maxUV.y( ) - bigUVStrip.m_uvOffset.y( ),
			bigUVStrip.m_imageWidth,
			bigUVStrip.m_imageHeight,
			pImagePathName);
		}
#endif
	}
}

void MeshProtocol::BuildMeshInfo(void)
{
	int tempValue;

	m_uvMaps.RemoveAll();
	m_uvChannelToIndexKeeper.RemoveAll();

	m_tupTriMesh.GetFaceFlags( &m_pFaceFlags, m_numFaces );
	m_tupTriMesh.GetPositionVerts((float **) &m_pPositionVerts, m_numVerts );
	m_tupTriMesh.GetNormalVerts((float **) &m_pNormalVerts, tempValue );
	m_tupTriMesh.GetPositionIndices( &m_pPositionIndices, tempValue );
	m_tupTriMesh.GetNormalIndices( &m_pNormalIndices, tempValue );
	m_tupTriMesh.GetFaceMaterialIndices(&m_pMaterialIndices,tempValue);

	m_pSubMaterialEnvironmentMapOpacityValues = m_tupTriMesh.GetBaseAggregate()->FindListByKey("Annotate_SubMaterialEnvironmentMapOpacityList")->GetAsFloat();
	TupperwareList *pSubMaterialHasEnvironmentMapValues = m_tupTriMesh.GetBaseAggregate()->FindListByKey("Annotate_SubMaterialHasEnvironmentMapList");
	m_pSubMaterialHasEnvironmentMapValues = pSubMaterialHasEnvironmentMapValues->GetAsInt();
	int numSubMaterialEntries = pSubMaterialHasEnvironmentMapValues->GetLength();
	m_bHasEnvironmentMap = false;
	for (int subMaterialIndex=0;subMaterialIndex<numSubMaterialEntries;subMaterialIndex++)
	{
		if (m_pSubMaterialEnvironmentMapOpacityValues[subMaterialIndex])
			m_bHasEnvironmentMap = true;
	}

	m_bUsesTextureTransformRot = false;
	TupperwareScalar *pUseTextureTransformRotationScalar = m_tupTriMesh.GetBaseAggregate()->FindScalarByKey("Annotate_Use Texture Transform Rotation");
	if (pUseTextureTransformRotationScalar)
	{
		m_bUsesTextureTransformRot = true;
	}

	for (int meshMapIndex = 0; meshMapIndex < m_tupTriMesh.GetNumMeshMaps(); meshMapIndex++ )
	{
		TupTriMeshMap  tupTriMeshMap( m_tupTriMesh.GetMeshMap( meshMapIndex ) );
		int mapChannel = tupTriMeshMap.GetMapChannel();
		if (mapChannel>=TupTriMeshMap::MAP_CHANNEL_TEXTURE_UV_START)
		{
			UVMap newUVMap;
			newUVMap.m_mapChannel = mapChannel;
			int oldSize = m_uvChannelToIndexKeeper.GetSize();
			int newIndex = m_uvChannelToIndexKeeper.Add(mapChannel); // keep track of mapChannel to array index
			if (oldSize==newIndex)
			{
				int numMapFaces;
				tupTriMeshMap.GetMapVerts((float **) &newUVMap.m_pUVVerts, newUVMap.m_uvVerts );
				tupTriMeshMap.GetMapIndices( &newUVMap.m_pUVIndices, numMapFaces);
				m_uvMaps.Add(newUVMap);
			}
		}
	}
}

void MeshProtocol::BuildLightingInfo(void)
{
	m_bFakeMeshBone = false;

	TupperwareAggregate *pMeshAgg = m_tupTriMesh.GetBaseAggregate();

	bool bALit = pMeshAgg->FindScalarByKey("RealTimeLighting_Ambient")->GetAsInt() ? true : false;
	bool bPLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Point")->GetAsInt() ? true : false;
	bool bDLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Directional")->GetAsInt() ? true : false;
	bool bVLit = pMeshAgg->FindScalarByKey("RealTimeLighting_Vertex")->GetAsInt() ? true : false;

	// save normals if realtime lit or has bones
	if (bALit || bPLit || bDLit || m_bRealMeshBones)
	{
		m_bMeshVertexColors = bVLit;
		m_bMeshNormals = true;
	}
	else
	{
		m_bMeshVertexColors = true; // if not realtime lit always do vertex lighting
		m_bMeshNormals = false;
	}
}

int MeshProtocol::AddUVStream(TupArray<UVStream> &uvStreams,const UVTransform &uvTransform,int layerIndex,int stageIndex)
{
	int numStreams = uvStreams.GetSize();
	for (int streamIndex=0;streamIndex<numStreams;streamIndex++)
	{
		UVStream &compareStream = uvStreams[streamIndex];
		if (uvTransform.m_rotationMatrix == compareStream.m_uvTransform.m_rotationMatrix &&
			uvTransform.m_UOffset == compareStream.m_uvTransform.m_UOffset &&
			uvTransform.m_VOffset == compareStream.m_uvTransform.m_VOffset &&
			uvTransform.m_UTiling == compareStream.m_uvTransform.m_UTiling &&
			uvTransform.m_VTiling == compareStream.m_uvTransform.m_VTiling &&
			uvTransform.m_mapChannel == compareStream.m_uvTransform.m_mapChannel &&
			layerIndex == compareStream.m_layerIndex &&
			stageIndex == compareStream.m_stageIndex)
			return streamIndex;
	}
	// make a new one if no match found
	UVStream uvStream;
	uvStream.m_uvTransform = uvTransform;
	uvStream.m_layerIndex = layerIndex;
	uvStream.m_stageIndex = stageIndex;
	uvStreams.Add(uvStream);
	return uvStreams.GetSize()-1;
}
