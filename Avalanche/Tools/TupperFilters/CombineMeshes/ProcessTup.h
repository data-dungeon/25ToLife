////////////////////////////////////////////////////////////////////////////
//
// ProcessTup
//
// Class to handle processing the tupperware data and adding any special information that is needed for export
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_TUP_H
#define PROCESS_TUP_H

class FilterOptions;
class FilterDataManager;
class TupScene;
class TupNodePool;
class TupObjectPool;
class TupNode;
class TupMaterialPool;
class TupModifierPool;

class ProcessTup
{
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupConfigRoot *m_pTupNodeConfigRoot;
	TupConfigRoot *m_pTupFaceConfigRoot;

	int m_rootNodeReference;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pRootNodeAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pModifierPoolAgg;
	TupperwareAggregate *m_pMaterialPoolAgg;

	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupNode *m_pTupRootNode;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;
	TupMaterialPool *m_pTupMaterialPool;

	ProcessTup(void); // this is not defined

public:
	ProcessTup(const FilterOptions &options,FilterDataManager &dataManager);
	~ProcessTup();
	void Process(void);

private:

};

#endif
