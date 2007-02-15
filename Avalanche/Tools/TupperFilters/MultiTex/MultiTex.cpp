////////////////////////////////////////////////////////////////////////////
//
// MultiTex
//
// Class to handle the MultiTex Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MultiTex.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 1/21/03    Time: 2:57p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * add next stage face list for use in ps2 multitexture code
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/17/02    Time: 10:01a
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Multi-texturing first pass complete
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/28/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Material Remap Skeleton & MultiTexMesh done
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/20/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/MultiTex
 * Duplicate Faces & Multitex Start
 */

////////////////////////////////////////////////////////////////////////////

#include "MultiTexPch.h"
#include "MultiTex.h"
#include "MultiTexMesh.h"

char *MultiTexFilter::m_writeDependencyNames[NUM_WRITE_DEPENDENCIES] = 
{
	"MultiTex_NextStageFaceIndices", // NEXT_STAGE_FACE_INDICES
};

Revision MultiTexFilter::m_writeDependencyRevisions[NUM_WRITE_DEPENDENCIES] = 
{
	Revision(1,0), // NEXT_STAGE_FACE_INDICES
};		

MultiTexFilter::MultiTexFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int MultiTexFilter::GetTotalPasses(void) const
{
	return 1;
}

void MultiTexFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	MultiTexMeshManager multiTexMeshManager(options,dataManager);
	multiTexMeshManager.Process();
}

void MultiTexFilter::CheckOptions(const FilterOptions &options)
{
}

int MultiTexFilter::GetTotalWriteDependencies(int pass,const FilterOptions &options) const
{
	return NUM_WRITE_DEPENDENCIES;
}

const char *MultiTexFilter::GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	return m_writeDependencyNames[index];
}

Revision MultiTexFilter::GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	return m_writeDependencyRevisions[index];
}
