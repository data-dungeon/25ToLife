////////////////////////////////////////////////////////////////////////////
//
// MultiTexMesh
//
// Class to handle setting up the meshes for multi texturing
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiTexMesh.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/28/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Material Remap Skeleton & MultiTexMesh done
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MULTI_TEX_MESH
#define MULTI_TEX_MESH

#ifndef MULTI_TEX_PARTS
#include "MultiTexParts.h"
#endif

class MultiTexMeshManager
{
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	int m_rootNodeReference;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pRootNodeAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pModifierPoolAgg;
	TupperwareAggregate *m_pMaterialPoolAgg;
	TupperwareAggregate *m_pMapPoolAgg;
	TupperwareAggregate *m_pImagePoolAgg;

	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupNode *m_pTupRootNode;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;
	TupMaterialPool *m_pTupMaterialPool;
	TupMapPool *m_pTupMapPool;
	TupImagePool *m_pTupImagePool;

public:

	MultiTexMeshManager(const FilterOptions &options,FilterDataManager &dataManager);
	~MultiTexMeshManager();

	void Process(void);

private:
	void CalculateUsedMaterials(TupArray<int> &usedMaterials);
	int CreateMultiSubMaterial(int oldMaterialRef,TupArray<int> &subMaterialRefs);
	void RemapNodeMaterialRefs(int objectRef,int oldMaterialRef,int newMaterialRef);
};

#endif
