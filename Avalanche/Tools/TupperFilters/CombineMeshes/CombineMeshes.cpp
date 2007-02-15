////////////////////////////////////////////////////////////////////////////
//
// CombineMeshes
//
// Class to handle the CombineMeshes Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "CombineMeshesPch.h"
#include "CombineMeshes.h"
#include "ProcessTup.h"

CombineMeshesFilter::CombineMeshesFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int CombineMeshesFilter::GetTotalPasses(void) const
{
	return 1;
}

void CombineMeshesFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	ProcessTup processTup(options,dataManager);
	processTup.Process();
}

