////////////////////////////////////////////////////////////////////////////
//
// ImageManager
//
// Class handle the image manager
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageManager.h $
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 5/27/03    Time: 3:29p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * access to actual file path
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 11/25/02   Time: 12:39p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * make sure back slashes and forward slashes are not a problem with each
 * other
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * image path updates
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 4/15/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * made image array a pointer for problems in release mode
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Relative Path Support for Images
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

#ifndef TUPPERFILTERS_LIB_PLUGINS_IMAGE_MANAGER_H
#define TUPPERFILTERS_LIB_PLUGINS_IMAGE_MANAGER_H

class TupperwareAggregate;
class ImageReaderPluginManager;
class TupperChainLogBase;
class ImageReaderOptionsManager;
class FilePathArray;

class TupperChainImage
{
	TupString m_filename;
	TupperwareAggregate *m_pData;
public:
	TupperChainImage(void);
	TupperChainImage(const char *filename,TupperwareAggregate *data);
	~TupperChainImage();
	TupperwareAggregate *GetData(void);
	const char *GetFilename(void) const;

	void SetFilename(const char *filename);
	void SetData(TupperwareAggregate *data);
};

class TupConfigRoot;

class ImageManager
{
	// these are not defined
	ImageManager &operator=(const ImageManager &s); 
	ImageManager(const ImageManager &s);

	ImageReaderPluginManager *m_pImageReaderPluginManager;
	const ImageReaderOptionsManager *m_pImageReaderOptionsManager;

	TupArray<TupperChainImage > *m_pImageArray;
	TupperChainLogBase &m_log;
	TupString m_basePathName;

public:
	ImageManager(TupperChainLogBase &log,const char *basePathname,const char *pluginPathname);
	~ImageManager(void);

	TupperwareAggregate *GetImageData(const char *filename,const char *pAttributeString=NULL,TupConfigRoot *pConfigRoot=NULL);
	int GetImageIndex(const char *filename,const char *pAttributeString=NULL,TupConfigRoot *pConfigRoot=NULL);
	bool IsImageLoaded(const char *filename) const;
	bool PurgeImage(const char *filename);
	int GetTotalImages(void) const;
	TupperwareAggregate *GetImageData(int imageIndex);
	const char *GetImageFilename(int imageIndex) const;
	void SetImageReaderOptionsManager(const ImageReaderOptionsManager &imageReaderOptionsManager);
	void GetImageFileDependencies(const char *filename,FilePathArray &filePathArray);

private:
	void GetActualFilePath(const char *filename,TupString &fullPath) const;
	static bool ReplacePathSubString(TupString &fileString,const TupString &replacePathSubString);
	static void FixFilePathString(TupString &fileString);

};

#endif
