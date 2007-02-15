////////////////////////////////////////////////////////////////////////////
//
// ImageReader
//
// Base class for all image plugin filters
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ImageReaderPlugin.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 7/29/02    Time: 3:12p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Image Reader Options available to Image Reader Plugins
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/20/02    Time: 2:50p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * Exceptions working properly
 * Log working with all plugins
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

#ifndef TUPPERFILTERS_LIB_IMAGE_PLUGINS_IMAGE_READER_PLUGIN_H
#define TUPPERFILTERS_LIB_IMAGE_PLUGINS_IMAGE_READER_PLUGIN_H

#ifndef TUPPERFILTERS_LIB_PLUGINS_REVISION_H
#include "TupperFilters/Libraries/Plugins/revision.h"
#endif

class TupperwareAggregate;
class TupperChainLogBase;
class FilterOptions;
class FilePathArray;
class TupConfigRoot;

class ImageReaderPlugin
{
	ImageReaderPlugin(void); // not defined

protected:
	TupperChainLogBase &m_log;
public:
	ImageReaderPlugin(TupperChainLogBase &log) : m_log(log) {}
	virtual int GetTotalSupportedExtensions(void) = 0;
	virtual const char *GetExtension(int extensionIndex) = 0;
	virtual TupperwareAggregate *GetImageData(const char *filename,const FilterOptions &options,const char *pAttributeString,TupConfigRoot *pConfigRoot) = 0;
	virtual TupperwareAggregate *GetDummyImageData(const char *filename,const FilterOptions &options,const char *pAttributeString,TupConfigRoot *pConfigRoot) = 0;
	virtual void GetImageFileDependencies(const char *filename,const FilterOptions &options,FilePathArray &filePathArray) = 0;
};

#endif
