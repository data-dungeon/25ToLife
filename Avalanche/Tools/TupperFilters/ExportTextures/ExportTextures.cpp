////////////////////////////////////////////////////////////////////////////
//
// ExportTextures
//
// Class to handle the ExportTextures Plugin
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#include "ExportTexturesPch.h"
#include "ExportTextures.h"
#include "ProcessTup.h"

int ExportTexturesFilter::GetTotalPasses(void)	const
{
	return 1;
}

void ExportTexturesFilter::Process(int pass,const FilterOptions &options,FilterDataManager &dataManager)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	ProcessTup processTup(options,dataManager);
	try
	{
		processTup.SetPlatform(m_platform);
		processTup.SetLoadSCFiles(m_bLoadSCFiles);
		processTup.SetExportXBoxRDFFile(m_bExportXBoxRDFFile);
		processTup.SetExportTextureData(m_bExportTextureData);
		processTup.SetScaleTextureWidth(m_scaleTextureWidth);
		processTup.SetScaleTextureHeight(m_scaleTextureHeight);
		processTup.Process();
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ExportTextures Filter - Process\r\n  %s",(const char *)(e)));
	}
}

void ExportTexturesFilter::GetFileDependencies(int pass,const FilterOptions &options,FilterDataManager &dataManager,FilePathArray &filePathArray)
{
	PluginLog::SetLog(&m_log);
	CheckOptions(options);
	ProcessTup processTup(options,dataManager);
	try
	{
		processTup.SetPlatform(m_platform);
		processTup.SetLoadSCFiles(m_bLoadSCFiles);
		processTup.SetExportXBoxRDFFile(m_bExportXBoxRDFFile);
		processTup.SetExportTextureData(m_bExportTextureData);
		processTup.SetScaleTextureWidth(m_scaleTextureWidth);
		processTup.SetScaleTextureHeight(m_scaleTextureHeight);
		processTup.GetFileDependencies(filePathArray);
	}
	catch (PluginException e)
	{
		throw(PluginException(ERROR_FATAL,"ExportTextures Filter - GetFileDependencies\r\n  %s",(const char *)(e)));
	}
}

void ExportTexturesFilter::CheckOptions(const FilterOptions &options)
{
	const char *pValue;
	pValue = options.GetValue("Platform");
	if (pValue)
	{
		m_platform = pValue;
	}
	else
	{
		throw(PluginException(ERROR_FATAL,"ExportTextures Filter - Platform not set"));
	}
	m_bLoadSCFiles = false;
	pValue = options.GetValue("LoadSCFiles");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bLoadSCFiles = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_NONE,"LoadSCFiles enabled");

	}

	m_bExportXBoxRDFFile = false;
	pValue = options.GetValue("ExportXBoxRDFFile");
	if (pValue && !stricmp(pValue,"true"))
	{
		m_bExportXBoxRDFFile = true;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Going to save an XBoxRDF file");
	}

	m_bExportTextureData = true;
	pValue = options.GetValue("ExportTextureData");
	if (pValue && !stricmp(pValue,"false"))
	{
		m_bExportTextureData = false;
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Not going to export texture data in .dbl file");
	}

	m_scaleTextureWidth = 100;
	pValue = options.GetValue("ScaleTextureWidth");
	if (pValue)
	{
		m_scaleTextureWidth = atoi(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Scale Texture Width %d",m_scaleTextureWidth);
	}

	m_scaleTextureHeight = 100;
	pValue = options.GetValue("ScaleTextureHeight");
	if (pValue)
	{
		m_scaleTextureHeight = atoi(pValue);
		PluginLog::Add(LogFlags::VERBOSE_LEVEL_LOW,"Scale Texture Height %d",m_scaleTextureHeight);
	}

}

