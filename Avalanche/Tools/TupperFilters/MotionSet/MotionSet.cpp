////////////////////////////////////////////////////////////////////////////
//
// MotionSet
//
// Class to handle the MotionSet Plugin
//
////////////////////////////////////////////////////////////////////////////

#include "MotionSetPch.h"
#include "MotionSet.h"
#include "MotionManager.h"
#include "Options.h"

MotionSetFilter::MotionSetFilter(TupperChainLogBase &log) : FilterPlugin(log)
{
}

MotionSetFilter::~MotionSetFilter()
{
}

int MotionSetFilter::GetTotalPasses(void) const
{
	return 1;
}

void MotionSetFilter::Process(int pass,const FilterOptions &parentOptions,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"MotionSet - KFM to motion database converter");
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Version %s - Build %s %s\n",g_pMotionSetVersion,g_pMotionSetDate,g_pMotionSetTime);

	Options options(parentOptions);

	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Using Default Translation ErrorTolerance = %f", options.m_transErrorTolerance);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Using Default Rotation ErrorTolerance = %f", options.m_rotErrorTolerance);
	PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Using Default Scale ErrorTolerance = %f", options.m_scaleErrorTolerance);

	if(options.m_bExcludeModelBone)
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Excluding the Model Bone");
	else
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Including the Model Bone");

	if(options.m_bWriteNamesAsChunk)
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Writing motion names as a separate chunk in the database file");
	else
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_MED,"Writing motion names as a text header file");

	int nStartTickCount = GetTickCount();

	int result = 0;

	MotionManager motionManager(options,dataManager);

	try
	{
		motionManager.ReadBoneGroupTableFile();
		motionManager.ReadInputData();
		motionManager.ReadMotionAndAnalyze();
		motionManager.PrintAnalyzeStats();
		motionManager.WriteDatabaseFile();
		if (options.m_bWriteHeaderFile)
		{
			motionManager.WriteHeaderFile();
		}
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"Error %s",(const char *)(e)));
	}
}



