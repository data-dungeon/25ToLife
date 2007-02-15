////////////////////////////////////////////////////////////////////////////
//
// BoundingFilter.h
//
// Class to handle the bounding Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BoundingFilter.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/Bounding
 * new bounding filter for use in annotate and break large meshes
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 3/31/03    Time: 3:20p
 * Created in $/Avalanche/tools/TupperFilters/Bounding
 */

////////////////////////////////////////////////////////////////////////////

#ifndef BOUNDING_FILTER_H
#define BOUNDING_FILTER_H

class TupperwareAggregate;

class BoundingFilter : public FilterPlugin
{
	enum WriteDependencies
	{
		OBJECT_BOUNDING,
		NODE_BOUNDING,
		NODE_OBB,
		NUM_WRITE_DEPENDENCIES // last entry
	};
	
	static char *m_writeDependencyNames[NUM_WRITE_DEPENDENCIES];
	static Revision m_writeDependencyRevisions[NUM_WRITE_DEPENDENCIES];

public:
	BoundingFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	int GetTotalWriteDependencies(int pass,const FilterOptions &options) const;
	const char *GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const;

};

#endif
