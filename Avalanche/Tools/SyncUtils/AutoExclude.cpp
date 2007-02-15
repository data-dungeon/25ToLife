////////////////////////////////////////////////////////////////////////////
//
// AutoExclude
//
// Class for managing the prefixes and extensions for exclusion
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AutoExclude.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/14/03    Time: 4:35p
 * Updated in $/Avalanche/tools/SyncUtils
 * options for folder sets
 * ui for autofreeze list
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:24p
 * Updated in $/Avalanche/tools/SyncUtils
 * fixed button behavior, auto exclude stuff, trying to get tooltips to
 * work...
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 */

#include "stdafx.h"
#include "SyncUtils.h"
#include "AutoExclude.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

AutoExclude::AutoExclude(void) : m_bKeeperInSync(false)
{
}

void AutoExclude::GetFileExtensionsString(TupString &string) const
{
	GetString(m_fileExtensions,string);
}

void AutoExclude::GetFilePrefixString(TupString &string) const
{
	GetString(m_filePrefixes,string);
}

void AutoExclude::GetFolderPrefixString(TupString &string) const
{
	GetString(m_folderPrefixes,string);
}

int AutoExclude::GetNumFilePrefixes(void) const
{
	return m_filePrefixes.GetSize();
}

int AutoExclude::GetNumFolderPrefixes(void) const
{
	return m_folderPrefixes.GetSize();
}

void AutoExclude::SetFileExtensions(const char *pString)
{
	BuildStringArray(m_fileExtensions,pString);
	AddDots(m_fileExtensions);
}

void AutoExclude::SetFilePrefixes(const char *pString)
{
	BuildStringArray(m_filePrefixes,pString);
}

void AutoExclude::SetFolderPrefixes(const char *pString)
{
	BuildStringArray(m_folderPrefixes,pString);
}

int AutoExclude::CheckFile(const char *pFileName) const
{
	int fileNameLength = strlen(pFileName);
	int flags = 0;

	int numPrefixes = m_filePrefixes.GetSize();
	int index;
	for (index=0;index<numPrefixes;index++)
	{
		const TupString &string = m_filePrefixes[index];
		int prefixLength = string.GetLength();
		if (fileNameLength>=prefixLength)
		{
			if (!_strnicmp(pFileName,string,prefixLength))
			{
				flags |= FLAGS_PREFIX;
				break;
			}
		}
	}
	int numExtensions = m_fileExtensions.GetSize();
	for (index=0;index<numExtensions;index++)
	{
		const TupString &extensionString = m_fileExtensions[index];
		int extensionLength = extensionString.GetLength();
		if (fileNameLength>extensionLength)
		{
			// see if the last part of the filename matches the extension
			if (!_stricmp(&((const char *)pFileName)[fileNameLength-extensionLength],extensionString))
			{
				flags |= FLAGS_EXTENSION;
				break;
			}
		}
	}
	return flags;
}

int AutoExclude::CheckFolder(const char *pFolderName) const
{
	int folderNameLength = strlen(pFolderName);

	int flags = 0;
	int numPrefixes = m_folderPrefixes.GetSize();
	int index;
	for (index=0;index<numPrefixes;index++)
	{
		const TupString &string = m_folderPrefixes[index];
		int prefixLength = string.GetLength();
		if (folderNameLength>=prefixLength)
		{
			if (!_strnicmp(pFolderName,string,prefixLength))
			{
				flags |= FLAGS_PREFIX; 
				break;
			}
		}
	}
	return flags;
}

// return index into path array -1 if not found
int AutoExclude::CheckPath(const char *pPathName) const
{
	SyncKeeper();
	TupString stringToCheck(pPathName);
	stringToCheck.MakeLower();
	int keeperIndex = m_pathKeeper.GetIndex(stringToCheck);
	return (keeperIndex!=-1) ? m_keeperToPathIndexes[keeperIndex] : -1; // return index in the path array or -1
}

bool AutoExclude::RemoveFilePrefix(TupString &fileName) const
{
	bool bChangedString = false;
	int numPrefixes = m_filePrefixes.GetSize();
	int index;

	// try taking all the prefixes of the filename
	while (1)
	{
		bool bFoundPrefix = false;
		int fileNameLength = fileName.GetLength();
		for (index=0;index<numPrefixes;index++)
		{
			const TupString &string = m_filePrefixes[index];
			int prefixLength = string.GetLength();
			if (fileNameLength>=prefixLength)
			{
				if (!_strnicmp(fileName,string,prefixLength))
				{
					fileName.Delete(0,prefixLength);
					bFoundPrefix = true;
					bChangedString = true;
					break;
				}
			}
		}
		// continue looking until there is not prefix found
		if (!bFoundPrefix)
			break;
	}
	return bChangedString; // return true if we changed the string
}

bool AutoExclude::RemoveFolderPrefix(TupString &folderName) const
{
	bool bChangedString = false;
	int numPrefixes = m_folderPrefixes.GetSize();
	int index;
	
	// try taking all the prefixes of the filename
	while (1)
	{
		bool bFoundPrefix = false;
		int folderNameLength = folderName.GetLength();
		for (index=0;index<numPrefixes;index++)
		{
			const TupString &string = m_folderPrefixes[index];
			int prefixLength = string.GetLength();
			if (folderNameLength>=prefixLength)
			{
				if (!_strnicmp(folderName,string,prefixLength))
				{
					folderName.Delete(0,prefixLength);
					bFoundPrefix = true;
					bChangedString = true;
					break;
				}
			}
		}
		// continue looking until there is not prefix found
		if (!bFoundPrefix)
			break;
	}
	return bChangedString; // return true if we changed the string
}

bool AutoExclude::AddFilePrefix(TupString &fileName) const
{
	if (m_filePrefixes.GetSize())
	{
		fileName.Insert(0,m_filePrefixes[0]);
		return true;
	}
	return false;
}

bool AutoExclude::AddFolderPrefix(TupString &folderName) const
{
	if (m_folderPrefixes.GetSize())
	{
		folderName.Insert(0,m_folderPrefixes[0]);
		return true;
	}
	return false;
}



void AutoExclude::GetString(const TupArray<TupString> &strings,TupString &destString)
{
	destString = "";
	int numStrings = strings.GetSize();
	for (int index=0;index<numStrings;index++)
	{
		destString += strings[index];
		if (index+1<numStrings)
		{
			destString += ";";
		}
	}
}

void AutoExclude::BuildStringArray(TupArray<TupString> &strings,const char *pString)
{
	TupString sourceString(pString);

	strings.RemoveAll();

	int index = 0;
	int endIndex;
	endIndex = sourceString.Find(';',index);
	// get first except for last one
	while(endIndex!=-1)
	{
		TupString newString = sourceString.Mid(index,endIndex-index);
		strings.Add(newString);
		index = endIndex+1;
		endIndex = sourceString.Find(';',index);
	}
	// get last one
	if (index<sourceString.GetLength())
	{
		TupString newString = sourceString.Mid(index);
		strings.Add(newString);
	}
}

void AutoExclude::AddDots(TupArray<TupString> &strings)
{
	int numStrings = strings.GetSize();
	for (int index=0;index<numStrings;index++)
	{
		TupString &string = strings[index];
		if (string[0] != '.')
			string.Insert(0,".");
	}
}

void AutoExclude::AddPath(const TupString &path)
{
	m_paths.Add(path);
	m_bKeeperInSync = false;
}

bool AutoExclude::RemovePath(const TupString &path)
{
	bool bRemoved = false;
	for (int index=m_paths.GetSize()-1;index>=0;index--)
	{
		if (path.CompareNoCase(m_paths[index])==0)
		{
			m_paths.RemoveAt(index);
			bRemoved = true;
			m_bKeeperInSync = false;
		}
	}
	return bRemoved;
}

void AutoExclude::RemovePaths()
{
	m_bKeeperInSync = false;
	m_paths.RemoveAll();
}


int AutoExclude::GetNumPaths(void) const
{
	return m_paths.GetSize();
}

void AutoExclude::GetPath(TupString &path,int index) const
{
	path = m_paths[index];
}

void AutoExclude::SetPath(const char *pPath,int index)
{
	m_paths[index] = pPath;
	m_bKeeperInSync = false;
}

void AutoExclude::SyncKeeper(void) const
{
	if (!m_bKeeperInSync)
	{
		m_pathKeeper.RemoveAll();
		m_keeperToPathIndexes.RemoveAll();

		int numPaths = m_paths.GetSize();
		for (int index=0;index<numPaths;index++)
		{
			TupString stringToAdd(m_paths[index]);
			stringToAdd.MakeLower();
			int keeperOldSize = m_pathKeeper.GetSize();
			int keeperIndex = m_pathKeeper.Add(stringToAdd);
			// if I just added this one
			if (keeperOldSize==keeperIndex)
			{
				m_keeperToPathIndexes.Add(index); // reference to original array
			}
		}
		m_bKeeperInSync = true;
	}
}