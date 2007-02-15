////////////////////////////////////////////////////////////////////////////
//
// ImageManager
//
// Class handle the image manager
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageManager.cpp $
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 5/28/03    Time: 9:25a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * replace path fix
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 5/27/03    Time: 3:29p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * access to actual file path
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 11/25/02   Time: 1:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * path search fixes
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 11/25/02   Time: 12:39p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * make sure back slashes and forward slashes are not a problem with each
 * other
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * image path updates
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 7/16/02    Time: 12:46p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Improved path code - plugin path is specified
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 4/15/02    Time: 4:38p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * made image array a pointer for problems in release mode
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 4/12/02    Time: 4:33p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/01/02    Time: 12:05p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Rich Edit Control for the Log.  Added additional flags for font styles
 * and colors.
 * 
 * *****************  Version 7  *****************
 * User: Tony Jones   Date: 2/28/02    Time: 3:11p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 2:04p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Relative Path Support for Images
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/19/02    Time: 5:08p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * First stage of gui app complete
 * Log window working
 * Exceptions set up
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 4:25p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added Collage Image Reader
 * Uses Paintlib (16 extensions)
 * Can read file and add width and height to tup
 * More to come...
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/13/02    Time: 2:20p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Added Image plugin support, Image manager, and renamed the filer
 * classes to not confuse anyone
 */

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "ImageManager.h"
#include "ImageReaderPluginManager.h"
#include "ImageReaderOptions.h"
#include "PluginError.h"
#include "PluginLog.h"
#include "FilePath.h"
#include "TokenizeString.h"

// TupperChainImage methods
TupperChainImage::TupperChainImage(void) : m_pData(0)
{
}

TupperChainImage::TupperChainImage(const char *filename,TupperwareAggregate *data) : m_pData(data), m_filename(filename)
{
}

TupperChainImage::~TupperChainImage()
{
	// Do Not destroy the m_pData here! - Image manager does it when it goes away
}

TupperwareAggregate *TupperChainImage::GetData(void)
{
	return m_pData;
}

const char *TupperChainImage::GetFilename(void) const
{
	return m_filename;
}

void TupperChainImage::SetFilename(const char *filename)
{
	m_filename = filename;
}

void TupperChainImage::SetData(TupperwareAggregate *data)
{
	if (m_pData)
		delete m_pData;
	m_pData = data;
}

// ImageManager methods
ImageManager::ImageManager(TupperChainLogBase &log,const char *basePathname,const char *pluginPathname) : 
	m_log(log), m_basePathName(basePathname)
{
	m_pImageReaderPluginManager = new ImageReaderPluginManager(log,pluginPathname);
	m_pImageArray = new TupArray<TupperChainImage >;
	m_pImageReaderOptionsManager = NULL;
}

ImageManager::~ImageManager()
{
	int numImages=m_pImageArray->GetSize();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		delete (*m_pImageArray)[imageIndex].GetData(); // delete the tupperware aggregate here
	}
	delete m_pImageArray; // delete this before the ImageReaderPluginManager
	delete m_pImageReaderPluginManager;
}

TupperwareAggregate *ImageManager::GetImageData(const char *filename,const char *pAttributeString,TupConfigRoot *pConfigRoot)
{
	TupString fullPath;
	GetActualFilePath(filename,fullPath);

	int numImages=m_pImageArray->GetSize();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperChainImage &image = (*m_pImageArray)[imageIndex];
		const char *imageFileName = image.GetFilename();
		if (!stricmp(fullPath,imageFileName))
		{
			return image.GetData();
		}
	}
	TupperwareAggregate *imageData = m_pImageReaderPluginManager->GetImageData(fullPath,m_pImageReaderOptionsManager,pAttributeString,pConfigRoot);
	assert(imageData);
	TupperChainImage newImage(fullPath,imageData);
	m_pImageArray->Add(newImage);
	return imageData;
}

int ImageManager::GetImageIndex(const char *filename,const char *pAttributeString,TupConfigRoot *pConfigRoot)
{
	TupString fullPath;
	GetActualFilePath(filename,fullPath);

	int numImages=m_pImageArray->GetSize();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		const TupperChainImage &image = (*m_pImageArray)[imageIndex];
		const char *imageFileName = image.GetFilename();
		if (!stricmp(fullPath,imageFileName))
		{
			return imageIndex;
		}
	}
	TupperwareAggregate *imageData = m_pImageReaderPluginManager->GetImageData(fullPath,m_pImageReaderOptionsManager,pAttributeString,pConfigRoot);
	assert(imageData);
	TupperChainImage newImage(fullPath,imageData);
	m_pImageArray->Add(newImage);

	return numImages;
}

bool ImageManager::IsImageLoaded(const char *filename) const
{
	TupString fullPath;
	GetActualFilePath(filename,fullPath);
	
	int numImages=m_pImageArray->GetSize();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		const TupperChainImage &image = (*m_pImageArray)[imageIndex];
		const char *imageFileName = image.GetFilename();
		if (!stricmp(fullPath,imageFileName))
		{
			return true;
		}
	}
	return false;
}

bool ImageManager::PurgeImage(const char *filename)
{
	int numImages=m_pImageArray->GetSize();
	for (int imageIndex=0;imageIndex<numImages;imageIndex++)
	{
		TupperChainImage &image = (*m_pImageArray)[imageIndex];
		const char *imageFileName = image.GetFilename();
		if (!stricmp(filename,imageFileName))
		{
			delete image.GetData(); // kill the tupperware aggregate
			m_pImageArray->RemoveAt(imageIndex);
			return true;
		}
	}
	return false;
}

int ImageManager::GetTotalImages(void) const
{
	return m_pImageArray->GetSize();
}

TupperwareAggregate *ImageManager::GetImageData(int imageIndex)
{
	int numImages=m_pImageArray->GetSize();
	assert(numImages>imageIndex);
	return (*m_pImageArray)[imageIndex].GetData();
}

const char *ImageManager::GetImageFilename(int imageIndex) const
{
	int numImages=m_pImageArray->GetSize();
	assert(numImages>imageIndex);
	return (*m_pImageArray)[imageIndex].GetFilename();

}

void ImageManager::SetImageReaderOptionsManager(const ImageReaderOptionsManager &imageReaderOptionsManager)
{
	m_pImageReaderOptionsManager = &imageReaderOptionsManager;
}

void ImageManager::GetImageFileDependencies(const char *filename,FilePathArray &filePathArray)
{
	TupString fullPath;
	GetActualFilePath(filename,fullPath);
	m_pImageReaderPluginManager->GetImageFileDependencies(fullPath,m_pImageReaderOptionsManager,filePathArray);
}

// figure out an actual file path for the filename
void ImageManager::GetActualFilePath(const char *filename,TupString &destPath) const
{
	const char *pImagePathSearchHigh = m_pImageReaderOptionsManager->GetGlobalOptions().GetValue("ImagePathSearchHigh");
	const char *pImagePathSearch = m_pImageReaderOptionsManager->GetGlobalOptions().GetValue("ImagePathSearch");
	const char *pReplaceImagePathSubString = m_pImageReaderOptionsManager->GetGlobalOptions().GetValue("ReplaceImagePathSubString");

	// do high priority search paths
	if (pImagePathSearchHigh)
	{
		TupString pathSearchString(pImagePathSearchHigh);
		FixFilePathString(pathSearchString);
		if (FilePath::SearchPathsForFile(filename,pathSearchString,m_basePathName,destPath))
		{
			// found it
			return;
		}
	}

	// turn a relative path to a full path
	TupString relativePath,fullPath,name,ext;
	// get full path of filename
	FilePath::GetPathsToFile(relativePath,fullPath,name,ext,filename,m_basePathName);
	// always lowercase and no backslashes (forward slashes)
	FixFilePathString(fullPath);

	// try each replace in order from left to right
	if (pReplaceImagePathSubString)
	{
		// handle replace path substring
		TupString replacePathSubString(pReplaceImagePathSubString);
		FixFilePathString(replacePathSubString);
		TupArray<TupString> replacePathSubStrings;
		TokenizeString::BuildArray(replacePathSubString,";",replacePathSubStrings);
		int numSubStrings = replacePathSubStrings.GetSize();
		for (int subStringIndex=0;subStringIndex<numSubStrings;subStringIndex++)
		{
			TupString replacePath(fullPath);
			if (ReplacePathSubString(replacePath,replacePathSubStrings[subStringIndex]))
			{
				if (_access(replacePath,0)==0)
				{
					destPath = replacePath;
					return;
				}
			}
		}
	}

	// see if I can access the file that was originally specified
	if (_access(fullPath,0)==0)
	{
		// found it
		destPath = fullPath;
		return;
	}

	// see if I can find the file using the regular path search
	if (pImagePathSearch)
	{
		TupString pathSearchString(pImagePathSearch);
		FixFilePathString(pathSearchString);
		if (FilePath::SearchPathsForFile(filename,pathSearchString,m_basePathName,destPath))
		{
			return;
		}
	}
	// if I cannot find the file then return the original full path
	destPath = fullPath;
}

bool ImageManager::ReplacePathSubString(TupString &fileString,const TupString &replacePathSubString)
{
	TupString oldString(fileString);
	TupArray<TupString> subStringParts;
	TokenizeString::BuildArray(replacePathSubString,"|",subStringParts);
	// if it has a source and replacement strings
	if (subStringParts.GetSize()==2)
	{
		fileString.Replace(subStringParts[0],subStringParts[1]); // replace part(s) with new part(s)
	}
	return (fileString!=oldString); // return true if changed
}

void ImageManager::FixFilePathString(TupString &fileString)
{
	fileString.MakeLower();
	fileString.Replace("/","\\"); // forward slashes to back slashes
}


