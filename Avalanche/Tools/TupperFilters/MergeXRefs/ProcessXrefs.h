////////////////////////////////////////////////////////////////////////////
//
// ProcessXRefs
//
// Class to handle processing xrefs and merging them into the scene
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ProcessXrefs.h $
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * File Dependency checks
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_XREFS_H
#define PROCESS_XREFS_H

class XRefManager;

class ProcessXRefs
{
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupConfigRoot *m_pTupConfigRoot;

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

	ProcessXRefs(void); // this is not defined

public:
	ProcessXRefs(const FilterOptions &options,FilterDataManager &dataManager,bool bProcessStep);
	~ProcessXRefs();
	void Process(void);
	void GetFileDependencies(FilePathArray &filePathArray);

private:
// process xrefs
	void MergeXRefs(void);
	void ApplyXRefAttributes(int xrefObjectRef,const char *pXRefNodeAttributes);
	void RemapObjectRefs(int xrefObjectRef,int newObjectRef);

// AddFileDependencies
	void AddXRefFileDependencies(XRefManager &xRefManager,const char *pFilename,const char *pNodeName,FilePathArray &filePathArray);
};

#endif
