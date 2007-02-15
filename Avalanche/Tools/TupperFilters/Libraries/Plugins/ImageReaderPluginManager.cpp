////////////////////////////////////////////////////////////////////////////
//
// ImagePluginManager
//
// Class handle the image plugins
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageReaderPluginManager.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/11/02    Time: 11:48a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Memory Errors
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

#include "PluginsPch.h"
#include "ImageReaderPluginManager.h"
#include "ImageReaderOptions.h"
#include "ImageReaderPluginDesc.h"
#include "ImageReaderPlugin.h"
#include "filepath.h"


// ImageReaderPlugin Methods
ImageReaderPluginManagerPlugin::ImageReaderPluginManagerPlugin(void) :
	m_pImageReaderPlugin(NULL), m_pLog(NULL)
{
}

ImageReaderPluginManagerPlugin::ImageReaderPluginManagerPlugin(const char *filename,TupperChainLogBase &log) :
	m_pImageReaderPlugin(NULL), m_filename(filename), m_pLog(&log)
{
	// load dll into memory
	m_hInstance = LoadLibraryEx(filename, NULL, 0);
	if (m_hInstance)
	{
		const char*	(*vDescription)();
		ImageReaderPluginDesc*	(*vPluginDesc)();
		ULONG (*vVersion)();

		vDescription = (const char *(*)())GetProcAddress(m_hInstance, "LibDescription");
		vPluginDesc = (ImageReaderPluginDesc *(*)())GetProcAddress(m_hInstance, "LibPluginDesc");
		vVersion = (ULONG(*)())GetProcAddress(m_hInstance, "LibVersion");

		if (vDescription && vPluginDesc && vVersion)
		{
			ULONG version = vVersion();
			if (version==VERSION_FILTER_CHAIN)
			{
				ImageReaderPluginDesc *pluginDesc = vPluginDesc();
				if (pluginDesc)
				{
					m_pImageReaderPlugin = (ImageReaderPlugin *)pluginDesc->Create(log);
				}
			}
		}
	}
}

ImageReaderPluginManagerPlugin::~ImageReaderPluginManagerPlugin()
{
}

ImageReaderPlugin *ImageReaderPluginManagerPlugin::GetImageReaderPlugin(void)
{
	return m_pImageReaderPlugin;
}

HINSTANCE ImageReaderPluginManagerPlugin::GetInstance(void)
{
	return m_hInstance;
}

const char *ImageReaderPluginManagerPlugin::GetFilename(void)
{
	return m_filename;
}

// PluginManager Methods
ImageReaderPluginManager::ImageReaderPluginManager(TupperChainLogBase &log,const char *pluginPathname) :
	m_log(log)
{
	HANDLE findhandle;
	WIN32_FIND_DATA file;

	// scan for all twi files
	TupString scanName(pluginPathname);
	scanName += "*.twi";

	if((findhandle=FindFirstFile(scanName,&file)) != INVALID_HANDLE_VALUE) 
	{
		do 
		{
         TupString imagePluginFilename = pluginPathname;
         imagePluginFilename += file.cFileName;
			ImageReaderPluginManagerPlugin imageReaderPlugin(imagePluginFilename,log);
			if (imageReaderPlugin.GetInstance())
			{
				// add if this was able to load
				m_imageReaderPluginArray.Add(imageReaderPlugin);
			}
		} 
		while(FindNextFile(findhandle,&file)==TRUE);
        FindClose(findhandle);
	}
}

ImageReaderPluginManager::~ImageReaderPluginManager(void)
{
	// free plugins
	int total = m_imageReaderPluginArray.GetSize();
	for (int i=0;i<total;i++)
	{
		ImageReaderPluginManagerPlugin &plugin = m_imageReaderPluginArray[i];
		HINSTANCE hInstance = plugin.GetInstance();
		if (hInstance)
		{
			FreeLibrary(hInstance);
		}
		delete plugin.GetImageReaderPlugin();
	}
}

TupperwareAggregate *ImageReaderPluginManager::GetImageData(const char *filename,
																				const ImageReaderOptionsManager *pImageReaderOptionsManager,
																				const char *pAttributeString,
																				TupConfigRoot *pConfigRoot)
{
	char extString[_MAX_EXT];
	FilePath::GetExt(filename,extString);
	int totalPlugins = m_imageReaderPluginArray.GetSize();
	for (int pluginIndex=0;pluginIndex<totalPlugins;pluginIndex++)
	{
		ImageReaderPluginManagerPlugin &plugin = m_imageReaderPluginArray[pluginIndex];
		ImageReaderPlugin *pImageReader = plugin.GetImageReaderPlugin();
		if (pImageReader)
		{
			int totalSupportedExtensions = pImageReader->GetTotalSupportedExtensions();
			for (int extensionIndex=0;extensionIndex<totalSupportedExtensions;extensionIndex++)
			{
				const char *pluginExtString = pImageReader->GetExtension(extensionIndex);
				if (!stricmp(extString,pluginExtString))
				{
					TupperwareAggregate *pImageData;
					if (pImageReaderOptionsManager)
					{
						TupString imageReaderFilename = plugin.GetFilename(); // what is this plugin called?
						char fdrive[_MAX_DRIVE];
						char fdir[_MAX_DIR];
						char fname[_MAX_FNAME];
						char fext[_MAX_EXT];
						_splitpath(imageReaderFilename,fdrive,fdir,fname,fext);
						strcat(fname,fext);
						pImageData = pImageReader->GetImageData(filename,pImageReaderOptionsManager->GetImageReaderOptions(fname),pAttributeString,pConfigRoot);
					}
					else
					{
						FilterOptions emptyOptions;
						pImageData = pImageReader->GetImageData(filename,emptyOptions,pAttributeString,pConfigRoot);
					}
					if (pImageData)
						return pImageData; // only return if it was able to load the image
				}
			}
		}
	}
	// if we are still here we must have a file we don't understand
	for (int pluginIndex=0;pluginIndex<totalPlugins;pluginIndex++)
	{
		ImageReaderPluginManagerPlugin &plugin = m_imageReaderPluginArray[pluginIndex];
		ImageReaderPlugin *pImageReader = plugin.GetImageReaderPlugin();
		if (pImageReader)
		{
			TupperwareAggregate *pImageData;
			if (pImageReaderOptionsManager)
			{
				TupString imageReaderFilename = plugin.GetFilename(); // what is this plugin called?
				char fdrive[_MAX_DRIVE];
				char fdir[_MAX_DIR];
				char fname[_MAX_FNAME];
				char fext[_MAX_EXT];
				_splitpath(imageReaderFilename,fdrive,fdir,fname,fext);
				strcat(fname,fext);
				pImageData = pImageReader->GetDummyImageData(filename,pImageReaderOptionsManager->GetImageReaderOptions(fname),pAttributeString,pConfigRoot);
			}
			else
			{
				FilterOptions emptyOptions;
				pImageData = pImageReader->GetDummyImageData(filename,emptyOptions,pAttributeString,pConfigRoot);
			}
			if (pImageData)
				return pImageData; // only return if it was able to load the image
		}
	}
	return NULL;
}

void ImageReaderPluginManager::GetImageFileDependencies(const char *filename,const ImageReaderOptionsManager *pImageReaderOptionsManager,FilePathArray &filePathArray)
{
	char extString[_MAX_EXT];
	FilePath::GetExt(filename,extString);
	int totalPlugins = m_imageReaderPluginArray.GetSize();
	for (int pluginIndex=0;pluginIndex<totalPlugins;pluginIndex++)
	{
		ImageReaderPluginManagerPlugin &plugin = m_imageReaderPluginArray[pluginIndex];
		ImageReaderPlugin *pImageReader = plugin.GetImageReaderPlugin();
		if (pImageReader)
		{
			int totalSupportedExtensions = pImageReader->GetTotalSupportedExtensions();
			for (int extensionIndex=0;extensionIndex<totalSupportedExtensions;extensionIndex++)
			{
				const char *pluginExtString = pImageReader->GetExtension(extensionIndex);
				if (!stricmp(extString,pluginExtString))
				{
					if (pImageReaderOptionsManager)
					{
						TupString imageReaderFilename = plugin.GetFilename(); // what is this plugin called?
						char fdrive[_MAX_DRIVE];
						char fdir[_MAX_DIR];
						char fname[_MAX_FNAME];
						char fext[_MAX_EXT];
						_splitpath(imageReaderFilename,fdrive,fdir,fname,fext);
						strcat(fname,fext);
						pImageReader->GetImageFileDependencies(filename,pImageReaderOptionsManager->GetImageReaderOptions(fname),filePathArray);
					}
					else
					{
						FilterOptions emptyOptions;
						pImageReader->GetImageFileDependencies(filename,emptyOptions,filePathArray);
					}
				}
			}
		}
	}
}
