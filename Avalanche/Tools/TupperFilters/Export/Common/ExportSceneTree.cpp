////////////////////////////////////////////////////////////////////////////
//
// ExportSceneTree
//
// Export the scene tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportSceneTree.cpp $
 * 
 * *****************  Version 22  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 5:45p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 21  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 5:44p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 20  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 4:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 19  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:58p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 4/23/03    Time: 1:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * handle no fog within scene tree
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:22p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * new bounding keys
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 1/30/03    Time: 12:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * visibility set stats in log 
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 9/25/02    Time: 3:24p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * render vs collision instances
 * 
 * *****************  Version 13  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 9:10a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Environmental instances are not added to scene tree
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:56p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Fixed to use Annotate filter
 * Added boundign and annotate helper references
 * Removed export pass
 * Added Dependencies
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/11/02    Time: 3:23p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/07/02    Time: 4:30p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * UV Offsets and Tiling now altering the uvs within the model
 * Pushed pools up to top of export dbl 
 * Renamed all inserted tupperware atoms to ExportPC - 
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instances that point to a Derived object working properly
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 4:17p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Made it ignore instances that don't point to a mesh (for now)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#include "ExportPch.h"
#include "ExportSceneTree.h"
#include "ExportVisibilityRegion.h"
#include "WalkSceneNode.h"

ExportSceneTree::ExportSceneTree(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot,int currentPartition) :
	m_pDBLFileName(pDBLFileName), m_options(options), m_dataManager(dataManager), m_tupConfigRoot(tupConfigRoot), m_currentPartition(currentPartition),
	m_ulXCount(10), m_ulZCount(10)
{
}

// A scene tree is a tree composed of CSceneNode nodes, each of which may point to data, 
// typically an instance at the leaves of the tree, but bounding box records are another 
// common type of data. the scene tree is rendered by the scene manager
u32 ExportSceneTree::WriteMainSceneTree(TupScene &tupScene,TupNodePool &tupNodePool)
{
	TupArray<int> visibilitySetNodeRefs;
	tupScene.GetBaseAggregate()->FindListByKey("Annotate_VisibilitySetNodeRefs")->GetAsInt(visibilitySetNodeRefs);

	int numVisibilitySets = visibilitySetNodeRefs.GetSize();

	TupArray<CSceneNode *> visibilitySetSceneNodePointers;
	visibilitySetSceneNodePointers.SetSize(numVisibilitySets);

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// set up roots. we do a double root so that when dynamic instances are added to the tree, they are 
	// siblings of TreeRoot2 instead of siblings of actual world nodes. this is questionable, but at least 
	// it lets you turn off the world (by disabling TreeRoot2) without turning off the dynamic instances 

	CSceneNode *pTreeRoot = new CSceneNode;
	pTreeRoot->Init();
	pTreeRoot->SetType(SCENE_NODE_ALWAYS_PASS);
	CSceneNode *pWorldRoot = new CSceneNode;
	pWorldRoot->Init();
	pWorldRoot->SetType(SCENE_NODE_ALWAYS_PASS);
	pTreeRoot->AddChild(pWorldRoot);

	// we have an x/z grid defined on top of the world (large models like the terrain are broken up to roughly match 
	// this grid). let's set up a quad tree based on grid size. what's the size of a grid section? 
	
	ts_BoundingBox WorldBoundingBox;
	float fXGridSize;
	float fZGridSize;
	CalculateGridSize(tupScene,WorldBoundingBox,fXGridSize,fZGridSize);

	/* what's the center of the world, rounded to nearest grid intersection? */

//	assert(Math::Zero(fXGridSize * m_ulXCount - (WorldBoundingBox.Max.x - WorldBoundingBox.Min.x), 0.00001f));	// world bounding box should be total grid size, or else center calc doesn't work
//	assert(Math::Zero(fZGridSize * m_ulZCount - (WorldBoundingBox.Max.z - WorldBoundingBox.Min.z), 0.00001f));
	float fWorldCenterX = WorldBoundingBox.Min.x( ) + ((m_ulXCount / 2) * fXGridSize);
	float fWorldCenterZ = WorldBoundingBox.Min.z( ) + ((m_ulZCount / 2) * fZGridSize);

	/* we want the whole-world bounding box to be a power of 2 grid sections in dimension so that as we divide it up, we */
	/* get grid-size bounding boxes at some level of the tree, so that the sub-models of large models fit well within */
	/* a bounding box */

	int nXCountPowerOf2 = 1;
	while (nXCountPowerOf2 < (signed) m_ulXCount)
		nXCountPowerOf2 *= 2;
	int nZCountPowerOf2 = 1;
	while (nZCountPowerOf2 < (signed) m_ulZCount)
		nZCountPowerOf2 *= 2;
	int nWorldGridCount = Math::Max(nZCountPowerOf2, nZCountPowerOf2);	// has to be same count in x and z so that sub-dividing will yield a grid-size bounding box at some point

	/* what's the size of the rounded-up bounding box? malloc bounding box so it can be free'd as part of WalkToFree */

	ts_BoundingBox *pWorldPowerOf2BoundingBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
	pWorldPowerOf2BoundingBox->Min.x( fWorldCenterX - ((nWorldGridCount / 2) * fXGridSize) );
	pWorldPowerOf2BoundingBox->Min.y( -MAXFLOAT32 );
	pWorldPowerOf2BoundingBox->Min.z( fWorldCenterZ - ((nWorldGridCount / 2) * fZGridSize) );
	pWorldPowerOf2BoundingBox->Max.x( pWorldPowerOf2BoundingBox->Min.x( ) + (nWorldGridCount * fXGridSize) );
	pWorldPowerOf2BoundingBox->Max.y( MAXFLOAT32 );
	pWorldPowerOf2BoundingBox->Max.z( pWorldPowerOf2BoundingBox->Min.z( ) + (nWorldGridCount * fZGridSize) );

	/* add whole-world bounding box to tree */

	CSceneNode *pWorldBoundingNode = new CSceneNode;
	pWorldBoundingNode->Init();
	pWorldBoundingNode->SetType(SCENE_NODE_BOUNDING_BOX);
	pWorldBoundingNode->SetData(pWorldPowerOf2BoundingBox);
	pWorldRoot->AddChild(pWorldBoundingNode);

	visibilitySetSceneNodePointers[0]=pWorldRoot; // keep track of this pointer for getting the index

	/* set up for visibility regions */
	SetupVisibilityRegions(tupNodePool,pTreeRoot,visibilitySetNodeRefs,visibilitySetSceneNodePointers);

	/* subdivide bounding boxes down to target size */
	WalkSceneNode::SetDivideTargetSize(10.0f);	// in  meters
	WalkSceneNode::WalkToDivideXZ(*pTreeRoot);

	/* remove the whole-world bounding node from tree-- it's not very useful */

	assert(pWorldBoundingNode->GetParent() == pWorldRoot);
	pWorldBoundingNode->Remove();
	CSceneNode *pNextChild = pWorldBoundingNode->GetFirstChild();
	while (pNextChild)
	{
		CSceneNode *pChild = pNextChild;
		pNextChild = pChild->GetNextSibling();
		pChild->Remove();
		pWorldRoot->AddChild(pChild);
	}
	free( pWorldBoundingNode->GetData() );
	pWorldBoundingNode->SetData( NULL );
	delete pWorldBoundingNode;

	/* for each static instance, find which bounding node it fits in best. bounding nodes can expand to include an */
	/* instance that mostly fits, but we have an expansion threshhold that we allow. if it doesn't fit in a bounding */
	/* node, put it in parent node. don't expand bounding node bounding boxes yet */

	AddNodesToSceneTree(tupNodePool,visibilitySetSceneNodePointers);

	// remove unneeded bounding nodes-- bounding nodes that have 0 or 1 children 
	WalkSceneNode::WalkToRemoveUnneededBoundingNodes(*pTreeRoot);

	/* resize bounding nodes to include children for greatest efficiency */
	WalkSceneNode::WalkToResizeBoundingNodes(*pTreeRoot);

	/* start chunk */
	DBLUtil::StartChunk(DB_SCENE_TREE, DB_SCENE_TREE_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	/* walk tree and write out data. this is a two stage process to fix up links between records, see comments */
	/* in CSceneNode::WalkToExportData() */

	assert(ftell(pDBLFile) == DBLUtil::GetChunkDataStart());
	WalkSceneNode::WalkToExportData(*pTreeRoot);
	fseek(pDBLFile, DBLUtil::GetChunkDataStart(), SEEK_SET);	// back to start of chunk data to re-write it
	WalkSceneNode::WalkToFixExportData(*pTreeRoot);

	/* let visibility regions know how to find themselves in the tree at runtime disk load fix-up */
	TupArray<int> visibilitySetSceneTreeIndices;
	visibilitySetSceneTreeIndices.SetSize(numVisibilitySets);
	for (int setIndex=0;setIndex<numVisibilitySets;setIndex++)
	{
		int sceneTreeIndex = -1;
		//-- look through the children of the root to find us.
		int index;
		CSceneNode *node;
		CSceneNode *visNode = visibilitySetSceneNodePointers[setIndex];
		for ( index = 0, node = pTreeRoot->GetFirstChild() ; node != NULL ; index++, node = node->GetNextSibling() )
		{
			if ( node == visNode )
			{
				sceneTreeIndex = index;
				break;
			}
		}
		visibilitySetSceneTreeIndices[setIndex] = sceneTreeIndex;
	}
	//  add these indexes so that the export visibility region code can use them
	tupScene.GetBaseAggregate()->AddListInt("ExportDBL_VisibilitySetSceneTreeIndices",NULL,visibilitySetSceneTreeIndices);

	/* clean up */
	CleanUpSceneTreeChunk(pTreeRoot);

	/* chunk is done */
	return DBLUtil::FinishChunk(FALSE);
}

// A scene tree is a tree composed of CSceneNode nodes, each of which may point to data, 
// typically an instance at the leaves of the tree, but bounding box records are another 
// common type of data. the scene tree is rendered by the scene manager
u32 ExportSceneTree::WritePartitionSceneTree(TupScene &tupScene,TupNodePool &tupNodePool)
{
	TupArray<int> visibilitySetNodeRefs;
	tupScene.GetBaseAggregate()->FindListByKey("Annotate_VisibilitySetNodeRefs")->GetAsInt(visibilitySetNodeRefs);

	int numVisibilitySets = visibilitySetNodeRefs.GetSize();

	TupArray<CSceneNode *> visibilitySetSceneNodePointers;
	visibilitySetSceneNodePointers.SetSize(numVisibilitySets);

	FILE *pDBLFile = DBLUtil::GetDBLFile();

	// set up one visibility set node as well as it's bounding for the current partition
	CSceneNode *pVisRoot = new CSceneNode;
	visibilitySetSceneNodePointers[m_currentPartition] = pVisRoot; // remember the pointer to get the index later

	pVisRoot->Init();
	pVisRoot->SetType(SCENE_NODE_ALWAYS_PASS);
	
	//-- visibility regions start out disabled-- doesn't really matter, they get re-checked every render
	pVisRoot->SetEnabled(false);
	
	int nodeRef = visibilitySetNodeRefs[m_currentPartition];
	TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeRef);
	
	//-- malloc and copy bounding box so it can be free'd as part of WalkToFree
	ts_BoundingBox *pVisBoundingBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
	pVisBoundingBox->Min = *(Vector3 *)pNodeAgg->FindListByKey("Annotate_VisiblitySetBoundingBoxMin")->GetAsFloat();
	pVisBoundingBox->Max = *(Vector3 *)pNodeAgg->FindListByKey("Annotate_VisiblitySetBoundingBoxMax")->GetAsFloat();
	
	//-- create a bounding box node as a child of this region
	CSceneNode *pVisBoundingNode = new CSceneNode;
	pVisBoundingNode->Init();
	pVisBoundingNode->SetType(SCENE_NODE_BOUNDING_BOX);
	pVisBoundingNode->SetData(pVisBoundingBox);
	pVisRoot->AddChild(pVisBoundingNode);

	/* subdivide bounding boxes down to target size */
	WalkSceneNode::SetDivideTargetSize(10.0f);	// in  meters
	WalkSceneNode::WalkToDivideXZ(*pVisRoot);

	/* for each static instance, find which bounding node it fits in best. bounding nodes can expand to include an */
	/* instance that mostly fits, but we have an expansion threshhold that we allow. if it doesn't fit in a bounding */
	/* node, put it in parent node. don't expand bounding node bounding boxes yet */

	AddNodesToSceneTree(tupNodePool,visibilitySetSceneNodePointers);

	// remove unneeded bounding nodes-- bounding nodes that have 0 or 1 children 
	WalkSceneNode::WalkToRemoveUnneededBoundingNodes(*pVisRoot);

	/* resize bounding nodes to include children for greatest efficiency */
	WalkSceneNode::WalkToResizeBoundingNodes(*pVisRoot);

	/* start chunk */
	DBLUtil::StartChunk(DB_SCENE_TREE, DB_SCENE_TREE_VERSION, DBL_CHUNK_NO_IGNORE, "1000");

	/* walk tree and write out data. this is a two stage process to fix up links between records, see comments */
	/* in CSceneNode::WalkToExportData() */

	assert(ftell(pDBLFile) == DBLUtil::GetChunkDataStart());
	WalkSceneNode::WalkToExportData(*pVisRoot);
	fseek(pDBLFile, DBLUtil::GetChunkDataStart(), SEEK_SET);	// back to start of chunk data to re-write it
	WalkSceneNode::WalkToFixExportData(*pVisRoot);

	/* clean up */
	CleanUpSceneTreeChunk(pVisRoot);

	/* chunk is done */
	return DBLUtil::FinishChunk(FALSE);
}


void ExportSceneTree::AddNodesToSceneTree(TupNodePool &tupNodePool,const TupArray<CSceneNode *> &visibilitySetSceneNodePointers)
{
	int numNodes = tupNodePool.GetNumNodes();
	for (int nodeIndex=0;nodeIndex<numNodes;nodeIndex++)
	{
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeIndex);
		if (!pNodeAgg)
			continue;

		TupperwareScalar *pRenderPartitionIndexScalar = pNodeAgg->FindScalarByKey("Annotate_RenderPartitionIndex");
		// skip ones that don't have this value
		if (!pRenderPartitionIndexScalar)
			continue;

		int renderPartitionIndex = pRenderPartitionIndexScalar->GetAsInt();
		// skip ones that are not in the current partition
		if (renderPartitionIndex!=m_currentPartition)
			continue;

		// if this is an env instance then skip
		if (pNodeAgg->FindScalarByKey("Annotate_EnvInstance")->GetAsInt())
			continue;
		
		int visibilitySetIndex = pNodeAgg->FindScalarByKey("Annotate_InVisibilitySetIndex")->GetAsInt(); // get the set we are in
		CSceneNode *pInstanceRoot = visibilitySetSceneNodePointers[visibilitySetIndex];

		ts_BoundingBox nodeBoundingBox;
		nodeBoundingBox.Min = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
		nodeBoundingBox.Max = *(Vector3 *)pNodeAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();
		
		// find its place, no box expansion 
		WalkSceneNode::InitBestBoundingParent(*pInstanceRoot, &nodeBoundingBox, FALSE);
		WalkSceneNode::WalkToFindBestBoundingParentXZ(*pInstanceRoot,pInstanceRoot->GetDepth());
		CSceneNode *pNoExpandParent = WalkSceneNode::GetBestBoundingParent();

		// do a second pass, allowing box expansion 
		WalkSceneNode::InitBestBoundingParent(*pInstanceRoot, &nodeBoundingBox, TRUE);
		WalkSceneNode::WalkToFindBestBoundingParentXZ(*pInstanceRoot,pInstanceRoot->GetDepth());
		CSceneNode *pExpandParent = WalkSceneNode::GetBestBoundingParent();

		// which is better? no expand wins a tie 
		CSceneNode *pBestParent = pNoExpandParent;
		if (pNoExpandParent != pExpandParent)
			pBestParent = (pNoExpandParent->GetDepth() >= pExpandParent->GetDepth()) ? pNoExpandParent : pExpandParent;
		assert((pBestParent->GetType() == SCENE_NODE_BOUNDING_BOX) || (pBestParent == pInstanceRoot));

		// put it in 
		CSceneNode *pInstanceNode = new CSceneNode;
		pInstanceNode->Init();
		pInstanceNode->SetType(SCENE_NODE_INSTANCE);
		pInstanceNode->SetData((void *) pNodeAgg); // the walk scene tree gets some stuff from the aggregate later
		pBestParent->AddChild(pInstanceNode);

		TupperwareScalar *pNoFarClipFlagScalar = pNodeAgg->FindScalarByKey("Annotate_NoFarClip");
		// if marked to be no far clipped set the flag and all of my parents
		if (pNoFarClipFlagScalar && pNoFarClipFlagScalar->GetAsInt())
		{
			CSceneNode *pParent = pInstanceNode;
			while (pParent)
			{
				pParent->SetFlags(pParent->GetFlags() | SCENE_NODE_NO_FAR_CLIP);
				pParent = pParent->GetParent();
			}
		}
	}
}


void ExportSceneTree::CleanUpSceneTreeChunk(CSceneNode *pTreeRoot)
{
	if (pTreeRoot)
	{
		WalkSceneNode::WalkToFree(*pTreeRoot);
	}
}

void ExportSceneTree::CalculateGridSize(TupScene &tupScene,ts_BoundingBox &worldBoundingBox,float &xGridSize,float &zGridSize)
{
	TupperwareAggregate *pSceneAgg = tupScene.GetBaseAggregate();
	worldBoundingBox.Min = *(Vector3 *)pSceneAgg->FindListByKey("Bounding_BoundingBoxMin")->GetAsFloat();
	worldBoundingBox.Max = *(Vector3 *)pSceneAgg->FindListByKey("Bounding_BoundingBoxMax")->GetAsFloat();

	//we now have the world's extents, compute the grid size
	xGridSize = (worldBoundingBox.Max.x( ) - worldBoundingBox.Min.x( ) ) / m_ulXCount;
	zGridSize = (worldBoundingBox.Max.z( ) - worldBoundingBox.Min.z( ) ) / m_ulZCount;
}

/* set for visibility region processing. there may not be any. for each visibility region, a new branch will be added to
the tree root, and instances that fall into the visibility region will go into its branch, not the whole-world branch. the
whole-world branch ends up being all instances that do not belong to any visibility region. instances can only be in the
scene tree once, so no instance can belong to more than one visibility region */

void ExportSceneTree::SetupVisibilityRegions(TupNodePool &tupNodePool,CSceneNode* pTreeRoot,const TupArray<int> &visibilitySetNodeRefs,TupArray<CSceneNode *> &visibilitySetSceneNodePointers)
{
	int numVisibilitySets = visibilitySetNodeRefs.GetSize();

	//-- add each vis region as brances to the tree, as a sibling of the whole-word node.
	//--             tree
	//--       +---+---+---+---+
	//--    world  v1  v2  v3  v4 ...

	for (int setIndex=1;setIndex<numVisibilitySets;setIndex++)
	{
		//-- create new node
		CSceneNode *pVisRoot = new CSceneNode;

		visibilitySetSceneNodePointers[setIndex] = pVisRoot; // remember the pointer to get the index later

		pVisRoot->Init();
		pVisRoot->SetType(SCENE_NODE_ALWAYS_PASS);
		pTreeRoot->AddChild(pVisRoot);

		//-- visibility regions start out disabled-- doesn't really matter, they get re-checked every render
		pVisRoot->SetEnabled(false);

		int nodeRef = visibilitySetNodeRefs[setIndex];
		
		TupperwareAggregate *pNodeAgg = tupNodePool.GetNode(nodeRef);
		
		//-- malloc and copy bounding box so it can be free'd as part of WalkToFree
		ts_BoundingBox *pVisBoundingBox = (ts_BoundingBox *) malloc(sizeof(ts_BoundingBox));
		pVisBoundingBox->Min = *(Vector3 *)pNodeAgg->FindListByKey("Annotate_VisiblitySetBoundingBoxMin")->GetAsFloat();
		pVisBoundingBox->Max = *(Vector3 *)pNodeAgg->FindListByKey("Annotate_VisiblitySetBoundingBoxMax")->GetAsFloat();

		//-- create a bounding box node as a child of this region
		CSceneNode *pVisBoundingNode = new CSceneNode;
		pVisBoundingNode->Init();
		pVisBoundingNode->SetType(SCENE_NODE_BOUNDING_BOX);
		pVisBoundingNode->SetData(pVisBoundingBox);
		pVisRoot->AddChild(pVisBoundingNode);
	}
}


