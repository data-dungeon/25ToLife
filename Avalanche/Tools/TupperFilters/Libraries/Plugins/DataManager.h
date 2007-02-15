////////////////////////////////////////////////////////////////////////////
//
// DataManager
//
// Class to handle the data that is passed from filter to filter
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DataManager.h $
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 6/03/02    Time: 3:30p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added work path
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 5/23/02    Time: 4:18p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added more support for other scenes
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/07/02    Time: 5:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * added output filenames
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/10/02    Time: 2:52p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Configuration file working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/09/02    Time: 4:36p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * skeleton filter chain complete
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_DATA_MANAGER_H
#define TUPPERFILTERS_LIB_PLUGINS_DATA_MANAGER_H

class TupperwareAggregate;
class ImageManager;
class TupperChainLogBase;

class TupTreeEntry
{
	TupperwareAggregate *m_aggregate;
	TupString m_filename;

public:
	TupTreeEntry(void);
	TupTreeEntry(const char *filename);
	TupTreeEntry(TupperwareAggregate *pInitialDataAgg, const char *filename);
	TupTreeEntry(const TupTreeEntry &s);
	TupTreeEntry &operator=(const TupTreeEntry &s);

	const char *GetFilename(void) const;
	TupperwareAggregate *GetAggregate(void);

};

class FilterDataManager
{
	// these are not defined
	FilterDataManager &operator=(const FilterDataManager &s); 
	FilterDataManager(const FilterDataManager &s);
	FilterDataManager(void);

	TupArray<TupTreeEntry> m_tupTreeArray;
	TupString m_inputFilename;
	TupString m_outputFilename;
	TupString m_workDirectory;
	TupString m_pluginDirectory;
	TupString m_configDirectory;
	ImageManager &m_imageManager;
public:
	FilterDataManager(const char *inputFilename,const char *outputFilename,const char *workDirectory,const char *pluginDirectory,const char *configDirectory,ImageManager &imageManager);
	FilterDataManager(TupperwareAggregate *pInitialDataAgg, const char *inputFilename,const char *outputFilename,const char *workDirectory,const char *pluginDirectory,const char *configDirectory,ImageManager &imageManager);
	~FilterDataManager();

	// Get Methods
	TupperwareAggregate *GetSceneRoot(void);
	const char *GetSceneFilename(void) const;

	TupperwareAggregate *GetOtherSceneRoot(const char *filename);
	int GetOtherSceneIndex(const char *filename);
	int GetNumOtherScenes(void) const;
	TupperwareAggregate *GetOtherSceneRoot(int index);
	const char *GetOtherSceneFilename(int index) const;

	const char *GetInputFilename(void) const;
	const char *GetOutputFilename(void) const;
	const char *GetWorkDirectory(void) const;
	const char *GetPluginDirectory(void) const;
	const char *GetConfigDirectory(void) const;
	ImageManager &GetImageManager(void);
};

#endif
