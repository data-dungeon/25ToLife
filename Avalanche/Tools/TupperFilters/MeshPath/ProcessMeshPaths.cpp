////////////////////////////////////////////////////////////////////////////
//
// ProcessMeshPaths.h
//
//
////////////////////////////////////////////////////////////////////////////

#include "MeshPathPch.h"
#include "ProcessMeshPaths.h"
#include "TreeFactory.h"

#include "tupperware/keeper.h"
#include <Navigate/NavMesh.h>
#include <Navigate/NavTree.h>
#include "Tupper3d/TupVertex.h"
#include "Tupper3d/TupMatrix.h"
#include <tupperware/TupString.h>

#include <new>

#include <Math/Vector.h>
#include <collide/aabb.h>
#include <collide/cube.h>

static TupString s_strOutputFilename;

ProcessMeshPathsManager::ProcessMeshPathsManager(const FilterOptions &options,FilterDataManager &dataManager) : m_options(options), m_dataManager(dataManager) 
{
	m_pSceneAgg = m_dataManager.GetSceneRoot();
	assert(m_pSceneAgg);
	m_pTupScene = new TupScene(m_pSceneAgg);

	m_pNodePoolAgg = m_pTupScene->GetNodePool();
	assert(m_pNodePoolAgg);
	m_pTupNodePool = new TupNodePool(m_pNodePoolAgg);

	m_pObjectPoolAgg = m_pTupScene->GetObjectPool();
	assert(m_pObjectPoolAgg);
	m_pTupObjectPool = new TupObjectPool( m_pObjectPoolAgg );

	m_pModifierPoolAgg = m_pTupScene->GetModifierPool();
	assert(m_pModifierPoolAgg);
	m_pTupModifierPool = new TupModifierPool( m_pModifierPoolAgg );

	m_pTupNodeConfigRoot = new TupConfigRoot;

	// Node Attribute Tree
	TupperwareAggregate *pSceneNodeAttributeTreeAgg = m_pTupScene->GetNodeAttributeTree();
	if ( !pSceneNodeAttributeTreeAgg )
	{
		throw( PluginException( ERROR_FATAL, "Node Attribute Tree does not exist!" ) );
	}
	m_pTupNodeConfigRoot->BuildConfigTree( pSceneNodeAttributeTreeAgg );

	// Face Attribute Tree
	TupperwareAggregate *faceAttribTreeAgg = m_pTupScene->GetFaceAttributeTree();
	if ( faceAttribTreeAgg != NULL )
	{
		m_pTupFaceConfigRoot = new TupConfigRoot;
		m_pTupFaceConfigRoot->BuildConfigTree( faceAttribTreeAgg );
	}
	else
	{
		m_pTupFaceConfigRoot = NULL;
	}

	s_strOutputFilename = m_dataManager.GetOutputFilename();
}

ProcessMeshPathsManager::~ProcessMeshPathsManager()
{
	delete m_pTupScene;
	delete m_pTupNodePool;
	delete m_pTupObjectPool;
	delete m_pTupModifierPool;
	delete m_pTupNodeConfigRoot;
	delete m_pTupFaceConfigRoot;
}

//static float AreaOfTriangle( const Vector3 &a, const Vector3 &b, const Vector3 &c )
//{
//	// split the triangle at a
//	TupVertex3 A( a.x(), a.y(), a.z() );
//	TupVertex3 B( b.x(), b.y(), b.z() );
//	TupVertex3 C( c.x(), c.y(), c.z() );
//
//	TupVertex3 p, q, r;
//	p = A;
//	q = B;
//	r = C;
//
//	float f = B.PointToLineIntersectionPercent( A, C );
//	if ( ( f > 0.0f ) && ( f < 1.0f ) )
//	{
//		B = p;
//		A = q;
//		C = r;
//	}
//	else
//	{
//		f = C.PointToLineIntersectionPercent( A, B );
//		if ( ( f > 0.0f ) && ( f < 1.0f ) )
//		{
//			C = p;
//			A = q;
//			B = r;
//		}
//	}
//
//	TupVertex3 V = A.PointToLineIntersection( B, C );
//
//	// now get the area of AVC
//	float avc = 0.5f * ( ( V - A ).Length() * ( C - V ).Length() );
//
//	// now get the area of AVB
//	float avb = 0.5f * ( ( V - A ).Length() * ( B - V ).Length() );
//
//	return( avc + avb );
//}

//static const unsigned int s_iOct[2][2][2] =
//{
//	{	// x == 0
//		{	// y == 0
//			3,	// z == 0
//			7,	// z == 1
//		},
//		{	// y == 1
//			2,	// z == 0
//			6,	// z == 1
//		}
//	},
//	{	// x == 1
//		{	// y == 0
//			0,	// z == 0
//			4,	// z == 1
//		},
//		{	// y == 1
//			1,	// z == 0
//			5,	// z == 1
//		}
//	},
//}
//
//static bool DetermineOct( Vector3 &minExtent, Vector3 &maxExtent, Vector3 &startPoint, Vector3 &endPoint, bool &iOct[] )
//{
//	bool bResult = false;
//
//	// make sure point is within extents
//	bResult = ( ( point >= minExtent ) && ( point <= maxExtent ) );
//
//	Vector3 Diagonal = maxExtent - minExtent;
//
//	unsigned int x, y, z;
//
//	x = ( point.x() >= minExtent.x() && point.x() < ( Diagonal.x() / 2.0f ) ) ? 0 : 1;
//	y = ( point.y() >= minExtent.y() && point.y() < ( Diagonal.y() / 2.0f ) ) ? 0 : 1;
//	z = ( point.z() >= minExtent.z() && point.z() < ( Diagonal.z() / 2.0f ) ) ? 0 : 1;
//
//	iOct = s_iOct[ x ][ y ][ z ];
//
//	return( bResult );
//}
//
//static void GetTreeLeafCounts( NavMesh::Mesh *mesh, unsigned int **iNodes, unsigned int iNodeCount, Vector3 minExtent, Vector3 maxExtent, unsigned int &iTreeCount, unsigned int &iLeafCount )
//{
//	// count the nodes in each child cube
//	unsigned int iChildNodeCount[ 8 ];
//	for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
//	{
//		iChildNodeCount[ iCurrChild ] = 0;
//	}
//
//	for( unsigned int iCurrNode = 0; iCurrNode < iNodeCount; iCurrNode++ )
//	{
//		bool iChildren[ 8 ];
//		for( unsigned int iCurrPoint = 0; iCurrPoint < 3; iCurrPoint++ )
//		{
//			Vector3 startPoint = iNodes[ iCurrNode ]->points[ iCurrPoint ];
//			Vector3 endPoint = iNodes[ iCurrNode ]->points[ ( iCurrPoint < 2 ? iCurrPoint + 1 : 0 ) ]
//			if ( DetermineOct( minExtent, maxExtent, startPoint, endPoint, iChildren ) )
//			{
//				for( iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
//				{
//					if ( iChildren[ iCurrChild ] )
//					{
//						iChildNodeCount[ iCurrChild ]++;
//					}
//				}
//			}
//		}
//	}
//
//	for( iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
//	{
//
//	}
//}

static void BuildFaceFlags( std::vector< unsigned int > &vNodeCoverFlags, std::vector< unsigned int > &vFaceFlags, int iObjectRef, TupConfigRoot *pTupFaceConfigRoot, TupModifierPool *pTupModifierPool, TupObjectPool *pTupObjectPool, TupNodePool *pTupNodePool )
{
	if ( pTupFaceConfigRoot == NULL )
	{
		return;
	}

	int totalNodes = pTupNodePool->GetNumNodes();
	int nodeIndex;
	for ( nodeIndex = 0; nodeIndex < totalNodes; ++nodeIndex )
	{
		TupperwareAggregate *pNodeAgg = pTupNodePool->GetNode(nodeIndex);
		if ( pNodeAgg == NULL )
		{
			continue;
		}
		TupNode tupNode(pNodeAgg);
		int objectRef;
		tupNode.GetObjectReference( objectRef );
		if ( objectRef != iObjectRef )
		{
			continue;
		}

		TupperwareAggregate *pFaceAttributesAgg = AnnotateHelper::FindFaceAttributes( *pTupModifierPool, *pTupObjectPool, objectRef );
		if ( pFaceAttributesAgg == NULL )
		{
			continue;
		}

		TupFaceAttributesModifierContext tupFaceAttributesModifierContext( pFaceAttributesAgg );
		int numAttributeFaces;
		int numAttributeVerts;
		char **pAttributeVerts = NULL;
		int *pAttributeIndices = NULL;
		tupFaceAttributesModifierContext.GetFaceAttributeVerts( &pAttributeVerts, numAttributeVerts );
		tupFaceAttributesModifierContext.GetFaceAttributeIndices( &pAttributeIndices, numAttributeFaces );
		assert( numAttributeVerts );

		// preallocate our cover vector
		vNodeCoverFlags.reserve( numAttributeFaces );
		vFaceFlags.reserve( numAttributeFaces );
		for ( int i = 0; i < numAttributeFaces; ++i )
		{	// the vast majority of faces will not have hard edges
			vNodeCoverFlags[ i ] = Navigate::TreeNode::NOCOVER_EDGE;
			vFaceFlags[ i ] = 0;
		}

		std::vector< unsigned int > vCoverAttrFlags;
		std::vector< unsigned int > vFaceAttrFlags;
		vCoverAttrFlags.reserve( numAttributeVerts );
		vFaceAttrFlags.reserve( numAttributeVerts );

		// go through each attribute and fill in each with proper data
		for ( int attributesIndex = 0; attributesIndex < numAttributeVerts; ++attributesIndex )
		{
			TupKeyValue tupAttributes( ( const char* )( pAttributeVerts[ attributesIndex ] ), NULL, NULL );

			TupKeyValueContext tupKeyValueContext;
			tupKeyValueContext.AddKey("aiCover");
			tupKeyValueContext.AddKey("coverType");
			tupKeyValueContext.AddKey("NavMeshFaceFlag0");
			tupKeyValueContext.AddKey("NavMeshFaceFlag1");
			tupKeyValueContext.AddKey("NavMeshFaceFlag2");
			tupKeyValueContext.AddKey("NavMeshFaceFlag3");
			tupKeyValueContext.AddKey("NavMeshFaceFlag4");
			tupKeyValueContext.AddKey("NavMeshFaceFlag5");
			tupKeyValueContext.AddKey("NavMeshFaceFlag6");
			tupKeyValueContext.AddKey("NavMeshFaceFlag7");
			pTupFaceConfigRoot->GetKeyValueContexts(tupAttributes,tupKeyValueContext,false);

			if ( !strcmp( tupKeyValueContext.GetValueAsString( 0 ), "Enabled" ) )
			{
				if ( !strcmp( tupKeyValueContext.GetValueAsString( 1 ), "fullCover" ) )
				{
					vCoverAttrFlags[ attributesIndex ] = Navigate::TreeNode::FULLCOVER_EDGE;
				}
				else if ( !strcmp( tupKeyValueContext.GetValueAsString( 1 ), "standingCover" ) )
				{
					vCoverAttrFlags[ attributesIndex ] = Navigate::TreeNode::STANDCOVER_EDGE;
				}
				else if ( !strcmp( tupKeyValueContext.GetValueAsString( 1 ), "crouchingCover" ) )
				{
					vCoverAttrFlags[ attributesIndex ] = Navigate::TreeNode::CROUCHCOVER_EDGE;
				}
				else if ( !strcmp( tupKeyValueContext.GetValueAsString( 1 ), "noCover" ) )
				{
					vCoverAttrFlags[ attributesIndex ] = Navigate::TreeNode::NOCOVER_EDGE;
				}
				else
				{
					ASSERT( 0 );
				}
			}
			else
			{
				vCoverAttrFlags[ attributesIndex ] = Navigate::TreeNode::NOCOVER_EDGE;
			}

			vFaceAttrFlags[ attributesIndex ] = 0;

			if ( !strcmp( tupKeyValueContext.GetValueAsString( 2 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x01;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 3 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x02;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 4 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x04;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 5 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x08;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 6 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x10;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 7 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x20;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 8 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x40;
			}
			if ( !strcmp( tupKeyValueContext.GetValueAsString( 9 ), "True" ) )
			{
				vFaceAttrFlags[ attributesIndex ] |= 0x80;
			}
		}

		// copy over the appropriate flags
		for( int iFaceIndex = 0; iFaceIndex < numAttributeFaces; ++iFaceIndex )
		{
			vNodeCoverFlags.push_back( vCoverAttrFlags[ pAttributeIndices[ iFaceIndex ] ] );
			vFaceFlags.push_back( vFaceAttrFlags[ pAttributeIndices[ iFaceIndex ] ] );
		}
	}
}

static void s_RemoveOldNAVFiles( const char *szFileNameBase )
{
	char drive[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];
	char fname[ _MAX_FNAME ];
	char ext[ _MAX_EXT ];
	char fileName[ _MAX_PATH ];

	_splitpath( szFileNameBase, drive, dir, fname, ext );
	_makepath( fileName, drive, dir, "*", ".NAV" );

	// loop through all *.nav files
	WIN32_FIND_DATA findData;
	HANDLE hFind = ::FindFirstFile( fileName, &findData );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			{	// avoid deleting directories
				_splitpath( findData.cFileName, NULL, NULL, fname, ext );
				_makepath( fileName, drive, dir, fname, ext );
				::DeleteFile( fileName );
			}
		} while ( ::FindNextFile( hFind, &findData ) );
	}

	::FindClose( hFind );
}

void ProcessMeshPathsManager::Process(void)
{
	int iCurrentMeshFileID = 0;
	int iErrorCount = 0;

	// remove any old data files
	s_RemoveOldNAVFiles( s_strOutputFilename );

	int totalNodes = m_pTupNodePool->GetNumNodes();
	int nodeIndex;
	for (nodeIndex=0;nodeIndex<totalNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = m_pTupNodePool->GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;
		TupNode tupNode(pNodeAgg);
		char *pNodeName = NULL;
		tupNode.GetName( &pNodeName );

		// check and set flags based on instance attributes
		// get my attributes from the node
		char *pNodeAttributesString;
		tupNode.GetUserProperties(&pNodeAttributesString);
		// count how many named points we have
		TupKeyValue tupKeyValue(pNodeAttributesString,BEGIN_BLOCK,END_BLOCK);
		TupKeyValueContext tupKeyValueContext;
		tupKeyValueContext.AddKey("instance_type");
		m_pTupNodeConfigRoot->GetKeyValueContexts(tupKeyValue,tupKeyValueContext,false);
		// if not set to a mesh path
		if (strcmp(tupKeyValueContext.GetValueAsString(0),"mesh_path")!=0)
		{
			continue;
		}

		// ok - we have a mesh_path to process

		int objectRef;	
		tupNode.GetObjectReference(objectRef);

		if (objectRef==-1)
			continue;

		TupperwareAggregate *pBaseMeshAgg = AnnotateHelper::FindBaseMesh( *m_pTupObjectPool,objectRef );
		// if no mesh at the end then forget it
		if (!pBaseMeshAgg)
			continue;

		Keeper<Vector3> vertexKeeper;

		TupVertex3f *pPositionVerts;
		TupVertex3f *pNormalVerts;
		int *pPositionIndices;
		int *pNormalIndices;
		int *pFaceFlags;
		int iPositionVertCount;
		int iNormalVertCount;
		int iPositionIndexCount;
		int iNormalIndexCount;
		int iFaceFlagCount;

		TupTriMesh tupTriMesh(pBaseMeshAgg);

		//////////////////////////////////////////////////////////////////////////

		// get matrix from node...
		TupMatrix4x4f localToWorldMatrix;
		TupperwareAggregate *pTransformAgg = tupNode.GetTransform( );
		if( pTransformAgg )
		{
			TupTransform tupTransform( pTransformAgg );
			if( tupTransform.GetTransformType( ) == TupTransform::STATIC_MATRIX )
			{
				TupStaticMatrix tupStaticMatrix( pTransformAgg );
				float *pMatrix;
				tupStaticMatrix.GetMatrix(&pMatrix);
				memcpy(&localToWorldMatrix,pMatrix,16*sizeof(float));
			}
		}

		// transform mesh into world coordinates...
		tupTriMesh.GetFaceFlags( &pFaceFlags, iFaceFlagCount );
		tupTriMesh.GetPositionVerts( (float **)&pPositionVerts, iPositionVertCount );
		tupTriMesh.GetPositionIndices( &pPositionIndices, iPositionIndexCount );
		tupTriMesh.GetNormalVerts( (float **)&pNormalVerts, iNormalVertCount );
		tupTriMesh.GetNormalIndices( &pNormalIndices, iNormalIndexCount );

		// transform position and normals into world space
		TupArray<TupVertex3f> worldPositionVerts;
		TupArray<TupVertex3f> worldNormalVerts;

		worldPositionVerts.SetSize(iPositionVertCount);
		worldNormalVerts.SetSize(iNormalVertCount);
		memcpy(worldPositionVerts.GetData(),pPositionVerts,iPositionVertCount*sizeof(TupVertex3f));
		memcpy(worldNormalVerts.GetData(),pNormalVerts,iNormalVertCount*sizeof(TupVertex3f));

		localToWorldMatrix.TransformVertices(worldPositionVerts);
		localToWorldMatrix.TransformNormals(worldNormalVerts);

		//////////////////////////////////////////////////////////////////////////

		// collect node cover flags
		std::vector< unsigned int > vNodeCoverFlags;
		std::vector< unsigned int > vNodeFaceFlags;
		BuildFaceFlags( vNodeCoverFlags, vNodeFaceFlags, objectRef, m_pTupFaceConfigRoot, m_pTupModifierPool, m_pTupObjectPool, m_pTupNodePool );

		//////////////////////////////////////////////////////////////////////////

		TupTriMeshConnectivity meshConnectivity;
		meshConnectivity.SetMesh(tupTriMesh,0);

		int iFaceCount = meshConnectivity.GetNumFaces();
		int iVertexCount = iPositionVertCount;	// room for the points of the triangle, and the unique face normals

		// make sure all normals are +y
		for( int iCurrFace = 0, iCurrFace3 = 0; iCurrFace < iFaceCount; ++iCurrFace, iCurrFace3 += 3 )
		{
			for( int iCurrPoint = 0; iCurrPoint < 3; ++iCurrPoint )
			{
				int iNormalIndex = pNormalIndices[ iCurrFace3 + iCurrPoint ];
				if ( worldNormalVerts[ iNormalIndex ].y < 0.0f )
				{
					int iPositionIndex = pPositionIndices[ iCurrFace3 + iCurrPoint ];
					TupVertex3f &vert = worldPositionVerts[ iPositionIndex ];
					PluginLog::Add( LogFlags::VERBOSE_LEVEL_NONE|LogFlags::FONT_COLOR_DK_ORANGE|LogFlags::FONT_STYLE_BOLD,
						"Error - Bad normal in instance %s at point (%f, %f, %f)", pNodeName, vert.x, vert.y, vert.z );

					++iErrorCount;
				}
			}
		}

		// now that we know how much memory we need, allocate the memory and build the data structures
		unsigned int meshSize = sizeof( Navigate::Mesh );
		if ( ( meshSize % 4 ) != 0 )
		{
			meshSize += ( 4 - ( meshSize % 4 ) );
		}
		unsigned int nodeSize = sizeof( Navigate::Node ) * iFaceCount;
		if ( ( nodeSize % 4 ) != 0 )
		{
			nodeSize += ( 4 - ( meshSize % 4 ) );
		}
		unsigned int dataSize =
			sizeof( Navigate::Mesh ) +
			nodeSize + //( sizeof( Navigate::Node ) * iFaceCount ) +
			( sizeof( Vector3Packed ) * iVertexCount );
		if ( ( dataSize % 4 ) != 0 )
		{	// make this part 4-byte aligned
			dataSize += ( 4 - ( dataSize % 4 ) );
		}

#pragma warning( disable: 4345 )
		// allocate room for the data
		unsigned char	*dataPtr = new unsigned char[ dataSize ];
		Navigate::Mesh	*mesh = new ( dataPtr ) Navigate::Mesh();
#pragma warning( default: 4345 )

		// initialize all the pointers
		mesh->nodeCount = iFaceCount;
		mesh->pointCount = iVertexCount;
		mesh->nodes = ( Navigate::Node* )( ( unsigned int )mesh + meshSize );
		mesh->points = ( Vector3Packed* )( ( unsigned int )( mesh->nodes ) + ( nodeSize /*sizeof( Navigate::Node ) * mesh->nodeCount*/ ) );

		// the octree will be built in a separate chunk first, then glom'd onto the end of this chunk when we write out the database
		mesh->root = NULL;

		// fill in the points we already know about (position and normals)
		int currOffset = 0;

		// record the position verts
		for( int iCurrPosition = 0; iCurrPosition < iPositionVertCount; ++iCurrPosition )
		{
			mesh->points[ iCurrPosition ].x( worldPositionVerts[ iCurrPosition ].x );
			mesh->points[ iCurrPosition ].y( worldPositionVerts[ iCurrPosition ].y );
			mesh->points[ iCurrPosition ].z( worldPositionVerts[ iCurrPosition ].z );
		}

		// fill in the nodes
		currOffset = 0;
		for ( unsigned int iNodeIndex = 0; iNodeIndex < mesh->nodeCount; iNodeIndex++ )
		{
			// record connectivity
			TupTriMeshConnectivity::TriFace face = meshConnectivity.GetFace( iNodeIndex );
			Navigate::Node *pNode = &( mesh->nodes[ iNodeIndex ] );

			for( unsigned int iEdgeIndex = 0; iEdgeIndex < 3; iEdgeIndex++ )
			{
				// record our neighbors
				TupTriMeshConnectivity::Edge edge = face.GetEdge( iEdgeIndex );

				pNode->neighbors[ iEdgeIndex ] = ( unsigned int )edge.GetFaceIndex();

				// record our point index for this edge
				pNode->points[ iEdgeIndex ] = pPositionIndices[ ( iNodeIndex * 3 ) + iEdgeIndex ];
			}

			// finish setting the face flags

			pNode->edgeFlags_0 = 0;		/* these get setup correctly during the BuildChildren process */
			pNode->edgeFlags_1 = 0;
			pNode->edgeFlags_2 = 0;

			pNode->collisionTest = 0;
			pNode->occlusionCount = 0;
			if ( ( vNodeFaceFlags.size() > 0 ) && ( iNodeIndex < vNodeFaceFlags.size() ) )
			{
				pNode->pathingAttributes = vNodeFaceFlags[ iNodeIndex ];
			}
			else
			{
				pNode->pathingAttributes = 0;
			}
			pNode->externalLinkIndex = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		// BEGIN BUILDING THE OCTREE
		//////////////////////////////////////////////////////////////////////////

		// get the extents
		Octant	extents;
		AABB	aabb;
		aabb.minV.x( Math::PositiveInfinity ); aabb.minV.y( Math::PositiveInfinity ); aabb.minV.z( Math::PositiveInfinity );
		aabb.maxV.x( Math::NegativeInfinity ); aabb.maxV.y( Math::NegativeInfinity ); aabb.maxV.z( Math::NegativeInfinity );
		for( iNodeIndex = 0; iNodeIndex < mesh->nodeCount; iNodeIndex++ )
		{
			Vector3Packed *point;
			for( unsigned int i = 0; i < 3; i++ )
			{
				point = &( mesh->points[ mesh->nodes[ iNodeIndex ].points[ i ] ] );

				// check min extents
				if ( point->x() < aabb.minV.x() )
				{
					aabb.minV.x( point->x() );
				}
				if ( point->y() < aabb.minV.y() )
				{
					aabb.minV.y( point->y() );
				}
				if ( point->z() < aabb.minV.z() )
				{
					aabb.minV.z( point->z() );
				}

				// check max extents
				if ( point->x() > aabb.maxV.x() )
				{
					aabb.maxV.x( point->x() );
				}
				if ( point->y() > aabb.maxV.y() )
				{
					aabb.maxV.y( point->y() );
				}
				if ( point->z() > aabb.maxV.z() )
				{
					aabb.maxV.z( point->z() );
				}
			}
		}
		extents = Octant::Root( aabb );
		mesh->extents = extents;

		// recursively build the octtree
		TreeNodeFactory::Reset();
		Navigate::TreeNode *root = TreeNodeFactory::BuildChildren( vNodeCoverFlags, mesh, extents, 0, 16, 16 );

		// begin searching for hard edge and cover edge data
		mesh->root = root;

		// ERROR OUT!!
		if ( iErrorCount > 0 )
		{
			throw( PluginException( ERROR_FATAL, "MeshPath - Export\r\n  Stopped after %d errors.\r\n", iErrorCount ) );
		}

		// turn the pointers into offsets
		mesh->nodes = ( Navigate::Node * )( ( unsigned int )( mesh->nodes ) - ( unsigned int )mesh );
		mesh->points = ( Vector3Packed * )( ( unsigned int )( mesh->points ) - ( unsigned int )mesh );

		// update the octtree data for export
		mesh->root = ( Navigate::TreeNode* )dataSize;

		// now write our data out somewhere
		char drive[ _MAX_DRIVE ];
		char dir[ _MAX_DIR ];
		char fname[ _MAX_FNAME ];
		char ext[ _MAX_EXT ];
		char fileName[ _MAX_PATH ];
		char pathName[ _MAX_PATH ];

		_splitpath( s_strOutputFilename, drive, dir, fname, ext );
		sprintf( fileName, "%s%.3d", fname, iCurrentMeshFileID++ );
		_makepath( pathName, drive, dir, fileName, ".NAV" );

		//FilePath::ChangeExtension( s_strOutputFilename, ".NAV" );
		//FILE *fp = fopen( s_strOutputFilename, "wb" );
		FILE *fp = fopen( pathName, "wb" );
		fwrite( ( void * )mesh, dataSize, 1, fp );

		unsigned int blockSize;
		root = TreeNodeFactory::MakeSingleFileBlock( root, dataSize, blockSize );	// this call will delete root's old data after copying it into one big block
		fwrite( ( void * )root, blockSize, 1, fp );

		fflush( fp );
		fclose( fp );
		fp = NULL;

		delete [] root;
		root = NULL;
		delete [] dataPtr;
		dataPtr = NULL;
		mesh = NULL;

		TreeNodeFactory::Reset();

		// read it back in to verify the data
		fp = fopen( pathName, "rb" );
		fseek( fp, 0, SEEK_END );
		int iOffset = ftell( fp );
		fseek( fp, 0, SEEK_SET );

		dataPtr = new unsigned char[ iOffset ];
		fread( dataPtr, iOffset, 1, fp );
		fflush( fp );
		fclose( fp );
		fp = NULL;

		mesh = Navigate::Mesh::DiskFixup( dataPtr );
		delete [] mesh;
	}
}

