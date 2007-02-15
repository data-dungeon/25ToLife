////////////////////////////////////////////////////////////////////////////
//
// Stamp.h
//
// Class to handle the Stamp Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Stamp.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:41a
 * Updated in $/Avalanche/tools/TupperFilters/Stamp
 * Added Stamp Filter
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/24/03    Time: 9:29a
 * Created in $/Avalanche/tools/TupperFilters/Stamp
 */

////////////////////////////////////////////////////////////////////////////

#ifndef STAMP_H
#define STAMP_H

class TupperwareAggregate;

class StampFilter : public FilterPlugin
{
public:
	StampFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

};

#endif
