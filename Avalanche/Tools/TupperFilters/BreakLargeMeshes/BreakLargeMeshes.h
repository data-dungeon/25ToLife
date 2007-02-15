////////////////////////////////////////////////////////////////////////////
//
// BreakLargeMeshes
//
// Class to handle the BreakLargeMeshes Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BreakLargeMeshes.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/14/03    Time: 3:25p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * allow for turning off cutting
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/04/03    Time: 2:17p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * breakup meshes based on visibility regions as well as the grid
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 4:01p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Test Meshes and Copy objects
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/06/02    Time: 4:28p
 * Updated in $/Avalanche/tools/TupperFilters/BreakLargeMeshes
 * Start
 */

////////////////////////////////////////////////////////////////////////////

#ifndef BREAK_LARGE_MESHES_H
#define BREAK_LARGE_MESHES_H

class TupperwareAggregate;

class BreakUpSettings
{
public:
	float m_gridSize;
	float m_sizeThreshold;
	int m_cutOnTextureUsageLimit;
	bool m_bCutOnGrid;
	bool m_bCutOnVisRegion;
	bool m_bCutOnTextureUsage;
};

class BreakLargeMeshesFilter : public FilterPlugin
{
	enum ReadDependencies
	{
		NODE_BOUNDING,
		NODE_OBB,
		NUM_READ_DEPENDENCIES // last entry
	};

	static char *m_readDependencyNames[NUM_READ_DEPENDENCIES];
	static Revision m_readDependencyRevisions[NUM_READ_DEPENDENCIES];

	BreakUpSettings m_settings;

public:
	BreakLargeMeshesFilter(TupperChainLogBase &log);

	int GetTotalPasses(void) const;

	void Process(int pass,const FilterOptions &options,FilterDataManager &dataManager);

	int GetTotalReadDependencies(int pass,const FilterOptions &options) const;
	const char *GetReadDependencyKey(int pass,const FilterOptions &options,int index) const;
	Revision GetReadDependencyRevision(int pass,const FilterOptions &options,int index) const;

private:
	void CheckOptions(const FilterOptions &options);

};

#endif
