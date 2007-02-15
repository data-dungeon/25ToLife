////////////////////////////////////////////////////////////////////////////
//
// FilePathArray
//
// class to handle an array of file paths
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: filepatharray.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 */

#ifndef TUPPERFILTERS_LIB_PLUGINS_FILEPATHARRAY__H
#define TUPPERFILTERS_LIB_PLUGINS_FILEPATHARRAY__H

class FilePathArray
{
	Keeper<TupString> m_pathKeeper;
	TupArray<int> m_flagsArray;
public:
	enum Flags
	{
		FILE_INPUT			= 0x0001,
		FILE_OUTPUT			= 0x0002,
		FILE_ACCESS_WARNING	= 0x0004,
		FILE_ACCESS_FATAL	= 0x0008,
	};

	void AddFilePath(const char *pFilePath,int flags=FILE_INPUT|FILE_ACCESS_FATAL);
	int GetNumPaths(void) const;
	const char *GetFilePathName(int nIndex) const;
	int GetFilePathFlags(int nIndex) const;
};

#endif