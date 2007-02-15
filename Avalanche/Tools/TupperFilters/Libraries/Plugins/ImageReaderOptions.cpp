////////////////////////////////////////////////////////////////////////////
//
// ImageReaderOptions
//
// Class to handle options the image reader plugins
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageReaderOptions.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/22/02   Time: 11:38a
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * image path updates
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Image Reader Options available to Image Reader Plugins
 */

////////////////////////////////////////////////////////////////////////////

#include "PluginsPch.h"
#include "ImageReaderOptions.h"

// ImageReaderOptions methods
ImageReaderOptions::ImageReaderOptions(void)
{
}

ImageReaderOptions::ImageReaderOptions(const char *imageReaderName,const FilterOptions &options) :
	m_imageReaderName(imageReaderName),m_options(options)
{
}

const char *ImageReaderOptions::GetImageReaderName(void) const
{
	return m_imageReaderName;
}

const FilterOptions &ImageReaderOptions::GetOptions(void) const
{
	return m_options;
}

// ImageReaderOptionsManager methods
ImageReaderOptionsManager::ImageReaderOptionsManager(const FilterOptions &globalOptions) : m_globalOptions(globalOptions)
{
}

void ImageReaderOptionsManager::AddOptions(const char *imageReaderName,const FilterOptions &options)
{
	m_imageReaderOptionsArray.Add(ImageReaderOptions(imageReaderName,options));
}

const FilterOptions &ImageReaderOptionsManager::GetImageReaderOptions(const char *imageReaderName) const
{
	int numImageReaderOptions = m_imageReaderOptionsArray.GetSize();
	for (int imageReaderIndex = 0;imageReaderIndex<numImageReaderOptions;imageReaderIndex++)
	{
		if (!stricmp(m_imageReaderOptionsArray[imageReaderIndex].GetImageReaderName(),imageReaderName))
		{
			return m_imageReaderOptionsArray[imageReaderIndex].GetOptions();
		}
	}
	return m_globalOptions; // if one is not found then return the global options
}

const FilterOptions &ImageReaderOptionsManager::GetGlobalOptions(void) const
{
	return m_globalOptions;
}
