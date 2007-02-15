////////////////////////////////////////////////////////////////////////////
//
// FilePathArray
//
// class to handle an array of file paths
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: filepatharray.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 8/05/02    Time: 12:34p
 * Updated in $/Avalanche/tools/TupperFilters/Libraries/Plugins
 * File Dependency checks
 */

#include "PluginsPch.h"
#include "filepatharray.h"

void FilePathArray::AddFilePath(const char *pFilePath,int flags)
{
	TupString newFilePath(pFilePath);
	newFilePath.MakeLower();
	int index = m_pathKeeper.Add(newFilePath);
	if (index<m_flagsArray.GetSize())
		m_flagsArray[index] |= flags; // if already in keeper then combine the flags
	else
		m_flagsArray.Add(flags); // if not in the keeper then add the flags (keeper and flags array are in sync)
	assert(m_flagsArray.GetSize()==m_pathKeeper.GetSize());
}

int FilePathArray::GetNumPaths(void) const
{
	return m_pathKeeper.GetSize();
}

const char *FilePathArray::GetFilePathName(int nIndex) const
{
	return m_pathKeeper[nIndex];
}

int FilePathArray::GetFilePathFlags(int nIndex) const
{
	return m_flagsArray[nIndex];
}

