////////////////////////////////////////////////////////////////////////////
//
// BoundingFilter
//
// Class to handle the Bounding Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: BoundingFilter.cpp $
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

#include "BoundingPch.h"
#include "BoundingFilter.h"
#include "ProcessBounding.h"

char *BoundingFilter::m_writeDependencyNames[NUM_WRITE_DEPENDENCIES] = 
{
	"Bounding_ObjectBounding", // OBJECT_BOUNDING
	"Bounding_NodeBounding", // NODE_BOUNDING
	"Bounding_NodeOBB", // NODE_OBB
};

BoundingFilter::BoundingFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int BoundingFilter::GetTotalPasses(void) const
{
	return 1;
}

void BoundingFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	ProcessBoundingManager boundingManager(options,dataManager);
	boundingManager.Process();
}


Revision BoundingFilter::m_writeDependencyRevisions[NUM_WRITE_DEPENDENCIES] = 
{
	Revision(1,0), // OBJECT_BOUNDING
	Revision(1,0), // NODE_BOUNDING
	Revision(1,0), // NODE_OBB
};		

int BoundingFilter::GetTotalWriteDependencies(int pass,const FilterOptions &options) const
{
	return NUM_WRITE_DEPENDENCIES;
}

const char *BoundingFilter::GetWriteDependencyKey(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	return m_writeDependencyNames[index];
}

Revision BoundingFilter::GetWriteDependencyRevision(int pass,const FilterOptions &options,int index) const
{
	int numDependencies = GetTotalWriteDependencies(pass,options);
	assert(index>=0 && index<numDependencies);
	return m_writeDependencyRevisions[index];
}


