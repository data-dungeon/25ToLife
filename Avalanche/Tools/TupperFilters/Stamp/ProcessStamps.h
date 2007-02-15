////////////////////////////////////////////////////////////////////////////
//
// ProcessStamps.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ProcessStamps.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:29p
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_STAMPS_H
#define PROCESS_STAMPS_H

class MaterialMapsManager;
class StampsManager;
class MeshLayersManager;

class ProcessStampsManager
{
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupConfigRoot *m_pTupNodeConfigRoot;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	TupperwareAggregate *m_pModifierPoolAgg;
	TupperwareAggregate *m_pMaterialPoolAgg;
	TupperwareAggregate *m_pMapPoolAgg;
	TupperwareAggregate *m_pImagePoolAgg;
	
	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupObjectPool *m_pTupObjectPool;
	TupModifierPool *m_pTupModifierPool;
	TupMaterialPool *m_pTupMaterialPool;
	TupMapPool *m_pTupMapPool;
	TupImagePool *m_pTupImagePool;

public:
	ProcessStampsManager(const FilterOptions &options,FilterDataManager &dataManager);
	~ProcessStampsManager();

	void Process(void);

};

#endif
