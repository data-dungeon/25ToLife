////////////////////////////////////////////////////////////////////////////
//
// ProcessMeshPaths.h
//
//
////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_MESH_PATHS_H
#define PROCESS_MESH_PATHS_H

class ProcessMeshPathsManager
{
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupConfigRoot *m_pTupNodeConfigRoot;
	TupConfigRoot *m_pTupFaceConfigRoot;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pModifierPoolAgg;
	
	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;

public:
	ProcessMeshPathsManager(const FilterOptions &options,FilterDataManager &dataManager);
	~ProcessMeshPathsManager();

	void Process(void);

};

#endif
