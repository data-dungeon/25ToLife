////////////////////////////////////////////////////////////////////////////
//
// MeshPath
//
// Class to handle the Mesh Path Plugin
//
////////////////////////////////////////////////////////////////////////////

#include "MeshPathPch.h"
#include "MeshPath.h"
#include "ProcessMeshPaths.h"

MeshPathFilter::MeshPathFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int MeshPathFilter::GetTotalPasses(void) const
{
	return 1;
}

void MeshPathFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);

	ProcessMeshPathsManager processMeshPathsManager(options,dataManager);
	processMeshPathsManager.Process();
}


