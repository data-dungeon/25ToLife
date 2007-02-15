////////////////////////////////////////////////////////////////////////////
//
// ImagePluginManager
//
// Class handle the image plugins
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageReaderPluginManager.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERFILTERS_LIB_PLUGINS_IMAGE_PLUGIN_MANAGER_H
#define TUPPERFILTERS_LIB_PLUGINS_IMAGE_PLUGIN_MANAGER_H

class TupperwareAggregate;
class ImageReaderPlugin;
class TupperChainLogBase;
class ImageReaderOptionsManager;
class FilePathArray;

class ImageReaderPluginManagerPlugin
{
	TupString m_filename;
	HINSTANCE m_hInstance;
	ImageReaderPlugin *m_pImageReaderPlugin;
	TupperChainLogBase *m_pLog;
public:
	ImageReaderPluginManagerPlugin(void);
	ImageReaderPluginManagerPlugin(const char *filename,TupperChainLogBase &log);
	~ImageReaderPluginManagerPlugin();
	ImageReaderPlugin *GetImageReaderPlugin(void);
	HINSTANCE GetInstance(void);
	const char *GetFilename(void);
};

class ImageReaderPluginManager
{
	// these are not defined
	ImageReaderPluginManager &operator=(const ImageReaderPluginManager &s); 
	ImageReaderPluginManager(const ImageReaderPluginManager &s);

	TupArray<ImageReaderPluginManagerPlugin> m_imageReaderPluginArray;
	TupperChainLogBase &m_log;

public:
	ImageReaderPluginManager(TupperChainLogBase &log,const char *pluginPathname);
	~ImageReaderPluginManager(void);

	TupperwareAggregate *GetImageData(const char *filename,const ImageReaderOptionsManager *pImageReaderOptionsManager,const char *pAttributeString,TupConfigRoot *pConfigRoot);
	void GetImageFileDependencies(const char *filename,const ImageReaderOptionsManager *pImageReaderOptionsManager,FilePathArray &filePathArray);
};

#endif
