////////////////////////////////////////////////////////////////////////////
//
// ExportSceneTree
//
// Export the scene tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ExportSceneTree.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/12/03    Time: 10:50a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * streaming system updates
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/30/03    Time: 12:10p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPS2
 * visibility set stats in log 
 * 
 * *****************  Version 7  *****************
 * User: Rob Nelson   Date: 9/16/02    Time: 3:49p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/25/02    Time: 9:10a
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Environmental instances are not added to scene tree
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 2:48p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instances that point to a Derived object working properly
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

#ifndef EXPORT_SCENE_TREE_H
#define EXPORT_SCENE_TREE_H

class FilterOptions;
class FilterDataManager;
class TupScene;
class TupNodePool;
class ExportVisibilityRegion;

class ExportSceneTree
{
	const char *m_pDBLFileName;
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;
	TupConfigRoot &m_tupConfigRoot;

	ExportSceneTree(void); // this is not defined

	unsigned long m_ulXCount;
	unsigned long m_ulZCount;

	int m_currentPartition;

public:
	ExportSceneTree(const char *pDBLFileName,const FilterOptions &options,FilterDataManager &dataManager,TupConfigRoot &tupConfigRoot,int currentPartition);
	u32 WriteMainSceneTree(TupScene &tupScene,TupNodePool &tupNodePool);
	u32 WritePartitionSceneTree(TupScene &tupScene,TupNodePool &tupNodePool);

private:
	void AddNodesToSceneTree(TupNodePool &tupNodePool,const TupArray<CSceneNode *> &visibilitySetSceneNodePointers);
	void CleanUpSceneTreeChunk(CSceneNode *pTreeRoot);
	void CalculateGridSize(TupScene &tupScene,ts_BoundingBox &worldBoundingBox,float &xGridSize,float &zGridSize);
	void SetupVisibilityRegions(TupNodePool &tupNodePool,CSceneNode* pTreeRoot,const TupArray<int> &visibilitySetNodeRefs,TupArray<CSceneNode *> &visibilitySetSceneNodePointers);
};


#endif