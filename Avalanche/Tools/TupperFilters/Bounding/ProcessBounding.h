////////////////////////////////////////////////////////////////////////////
//
// ProcessBounding.h
//
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ProcessBounding.h $
 * 
 * *****************  Version 3  *****************
 * User: Dwain Skinner Date: 6/12/03    Time: 3:54p
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * new bounding filter for use in annotate and break large meshes
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/31/03    Time: 3:14p
 * Created in $/Avalanche/tools/TupperFilters/Bounding
 */

////////////////////////////////////////////////////////////////////////////

#ifndef PROCESS_BOUNDING_H
#define PROCESS_BOUNDING_H

class FilterOptions;
class FilterDataManager;
class TupConfigRoot;
class TupperwareAggregate;
class TupScene;
class TupNodePool;
class TupObjectPool;
//class Vector3;

class ProcessBoundingManager
{
	const FilterOptions &m_options;
	FilterDataManager &m_dataManager;

	TupConfigRoot *m_pTupNodeConfigRoot;

	// these point into the tree
	TupperwareAggregate *m_pSceneAgg;
	TupperwareAggregate *m_pNodePoolAgg;
	TupperwareAggregate *m_pObjectPoolAgg;
	
	// I own these
	TupScene *m_pTupScene;
	TupNodePool *m_pTupNodePool;
	TupObjectPool *m_pTupObjectPool;

	Vector3 *m_pBoundingBoxMin;
	Vector3 *m_pBoundingBoxMax;

public:
	ProcessBoundingManager(const FilterOptions &options,FilterDataManager &dataManager);
	~ProcessBoundingManager();

	void Process(void);

private:
	void AddObjectBounding(void);
	void AddNodeBounding(void);

};

#endif
