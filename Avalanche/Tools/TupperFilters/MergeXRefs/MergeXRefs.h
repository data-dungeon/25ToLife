////////////////////////////////////////////////////////////////////////////
//
// MergeXRefs
//
// Class to handle the MergeXRefs Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MergeXRefs.h $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * File Dependency checks
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MERGE_XREFS_H
#define MERGE_XREFS_H

class TupperwareAggregate;

class MergeXRefsFilter : public FilterPlugin
{

public:
	MergeXRefsFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	void GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray);

};

#endif
