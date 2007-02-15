////////////////////////////////////////////////////////////////////////////
//
// MakeYUp
//
// Class to handle the Make YUp Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MakeYUp.h $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/11/03    Time: 4:05p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * convert over to tupper3d coordinate system classes
 * keep old code just in case
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 5/29/02    Time: 2:59p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Transform skin modifier
 * Changed to use node pools
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/12/02    Time: 12:57p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Changed plugin class
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:45a
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Added Node Pool and Node Referencing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * const functions
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * skeleton filter chain complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/08/02    Time: 4:35p
 * Updated in $/Avalanche/tools/TupperFilters/MakeYUp
 * Filter chain files created
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MAKE_Y_UP_H
#define MAKE_Y_UP_H

#define USE_TUPPER3D

class TupperwareAggregate;
class TupperChainLogBase;
class TupNodePool;

class MakeYUpFilter : public FilterPlugin
{
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
	MakeYUpFilter(TupperChainLogBase &log) : FilterPlugin(log) {}

	int GetTotalPasses(void) const;
	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

private:
#ifndef USE_TUPPER3D
	void TransformScene(void);
	void TransformObject(TupperwareAggregate *pObjectAgg);
	void TransformModifier(TupperwareAggregate *pDerivedModifierAgg);
	void TransformNode(TupperwareAggregate *pNodeAgg);
#endif
};

#endif
