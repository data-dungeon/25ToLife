////////////////////////////////////////////////////////////////////////////
//
// MultiTex.h
//
// Class to handle the MultiTex Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiTex.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 2:57p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * add next stage face list for use in ps2 multitexture code
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Duplicate Faces & Multitex Start
 */

////////////////////////////////////////////////////////////////////////////

#ifndef MULTI_TEX_H
#define MULTI_TEX_H

class TupperwareAggregate;

class MultiTexFilter : public FilterPlugin
{
	enum WriteDependencies
	{
		NEXT_STAGE_FACE_INDICES,
		NUM_WRITE_DEPENDENCIES // last entry
	};
	
	static char *m_writeDependencyNames[NUM_WRITE_DEPENDENCIES];
	static Revision m_writeDependencyRevisions[NUM_WRITE_DEPENDENCIES];
	
public:
	MultiTexFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	int GetTotalWriteDependencies(int pass,const FilterOptions &options) const;
	const char *GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const;
	
private:
	void CheckOptions(const FilterOptions &options);

};

#endif
