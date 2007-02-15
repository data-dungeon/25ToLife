////////////////////////////////////////////////////////////////////////////
//
// CombineMeshes.h
//
// Class to handle the CombineMeshes Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: CombineMeshes.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 2:57p
 * Updated in $/Avalanche/tools/TupperFilters/CombineMeshes
 * add next stage face list for use in ps2 CombineMeshesture code
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/CombineMeshes
 * Duplicate Faces & CombineMeshes Start
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MULTI_TEX_H
#define MULTI_TEX_H

class TupperwareAggregate;

class CombineMeshesFilter : public FilterPlugin
{
	
public:
	CombineMeshesFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

private:

};

#endif
