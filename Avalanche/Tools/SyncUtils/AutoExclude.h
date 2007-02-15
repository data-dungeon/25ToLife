////////////////////////////////////////////////////////////////////////////
//
// AutoExclude
//
// Class for managing the prefixes and extensions for exclusion
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AutoExclude.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/14/03    Time: 4:35p
 * Updated in $/Avalanche/tools/SyncUtils
 * options for folder sets
 * ui for autofreeze list
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 */

#ifndef SYNC_UTILS_DLL_AUTO_EXCLUDE_H
#define SYNC_UTILS_DLL_AUTO_EXCLUDE_H

//-----------------------------------------------------------------------------
// SyncDBFactory class declaration:

class AFX_EXT_CLASS AutoExclude
{
	TupArray<TupString> m_fileExtensions;
	TupArray<TupString> m_filePrefixes;
	TupArray<TupString> m_folderPrefixes;
	TupArray<TupString> m_paths; // can have duplicates
	mutable Keeper<TupString> m_pathKeeper; // all the unique paths (for faster checking)
	mutable bool m_bKeeperInSync;
	mutable TupArray<int> m_keeperToPathIndexes;

public:
	enum CheckFlags
	{
		FLAGS_PREFIX			= 1<<0,
		FLAGS_EXTENSION		= 1<<1,
	};

	AutoExclude(void);

	void GetFileExtensionsString(TupString &string) const;
	void GetFilePrefixString(TupString &string) const;
	void GetFolderPrefixString(TupString &string) const;
	int GetNumFilePrefixes(void) const;
	int GetNumFolderPrefixes(void) const;

	void SetFileExtensions(const char *pString);
	void SetFilePrefixes(const char *pString);
	void SetFolderPrefixes(const char *pString);

	int CheckFile(const char *pFileName) const;
	int CheckFolder(const char *pFolderName) const;
	int CheckPath(const char *pPath) const;

	bool RemoveFilePrefix(TupString &fileName) const;
	bool RemoveFolderPrefix(TupString &folderName) const;

	bool AddFilePrefix(TupString &fileName) const;
	bool AddFolderPrefix(TupString &folderName) const;

	void AddPath(const TupString &path);
	bool RemovePath(const TupString &path);
	void RemovePaths(void);

	int GetNumPaths(void) const;
	void GetPath(TupString &path,int index) const;
	void SetPath(const char *pPath,int index);

private:
	static void GetString(const TupArray<TupString> &strings,TupString &destString);
	static void BuildStringArray(TupArray<TupString> &strings,const char *pString);
	static void AddDots(TupArray<TupString> &strings);
	void SyncKeeper(void) const;

};

//-----------------------------------------------------------------------------

#endif

