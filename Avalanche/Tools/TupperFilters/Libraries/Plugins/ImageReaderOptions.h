////////////////////////////////////////////////////////////////////////////
//
// ImageReaderOptions
//
// Class to handle options for the image reader plugins
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageReaderOptions.h $
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

#ifndef TUPPERFILTERS_LIB_PLUGINS_IMAGE_READER_OPTIONS_H
#define TUPPERFILTERS_LIB_PLUGINS_IMAGE_READER_OPTIONS_H

#ifndef TUPPERFILTERS_LIB_PLUGINS_OPTIONS_H
#include "options.h"
#endif

class ImageReaderOptions
{
	TupString m_imageReaderName;
	FilterOptions m_options;

public:
	ImageReaderOptions(void);
	ImageReaderOptions(const char *imageReaderName,const FilterOptions &options);
	const char *GetImageReaderName(void) const;
	const FilterOptions &GetOptions(void) const;
};

class ImageReaderOptionsManager
{
	const FilterOptions &m_globalOptions;
	TupArray<ImageReaderOptions> m_imageReaderOptionsArray;

	ImageReaderOptionsManager(void); // not defined

public:
	ImageReaderOptionsManager(const FilterOptions &globalOptions);

	void AddOptions(const char *imageReaderName,const FilterOptions &options);
	const FilterOptions &GetImageReaderOptions(const char *imageReaderName) const;
	const FilterOptions &GetGlobalOptions(void) const;
};
 
#endif