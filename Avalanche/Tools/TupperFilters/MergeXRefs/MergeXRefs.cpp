////////////////////////////////////////////////////////////////////////////
//
// MergeXRefs
//
// Class to handle the MergeXRefs Plugin
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: MergeXRefs.cpp $
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * File Dependency checks
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 5/28/02    Time: 12:12p
 * Updated in $/Avalanche/tools/TupperFilters/MergeXRefs
 * Moved MergeXRefs from annotate filter to its own filter
 */

////////////////////////////////////////////////////////////////////////////

#include "MergeXRefsPch.h"
#include "MergeXRefs.h"
#include "ProcessXRefs.h"

MergeXRefsFilter::MergeXRefsFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

int MergeXRefsFilter::GetTotalPasses(void) const
{
	return 1;
}

void MergeXRefsFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	ProcessXRefs processXRefs(options,dataManager,true);
	try
	{
		processXRefs.Process();
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"MergeXRefs Filter - ProcessXRefs\r\n  %s",(const char *)(e)));
	}
}

void MergeXRefsFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	ProcessXRefs processXRefs(options,dataManager,false);
	try
	{
		processXRefs.GetFileDependencies(filePathArray);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"MergeXRefs Filter - GetFileDependencies\r\n  %s",(const char *)(e)));
	}
}
