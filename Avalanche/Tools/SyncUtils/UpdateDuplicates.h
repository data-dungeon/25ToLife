////////////////////////////////////////////////////////////////////////////
//
// UpdateDuplicates
//
// Class used for managing duplicate files
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: UpdateDuplicates.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/04/03    Time: 3:11p
 * Updated in $/Avalanche/tools/SyncUtils
 * update duplicates:
 * new icons
 * include parent in name
 * options
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 2/18/03    Time: 9:42a
 * Updated in $/Avalanche/tools/SyncUtils
 * update duplicates complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/17/03    Time: 4:26p
 * Updated in $/Avalanche/tools/SyncUtils
 * Update Duplicates UI first draft (a few things left to do)
 */

#ifndef SYNC_UTILS_DLL_UPDATE_DUPLICATES_H
#define SYNC_UTILS_DLL_UPDATE_DUPLICATES_H

//-----------------------------------------------------------------------------
// SyncTreeFactory class declaration:

class TreeNodeFile;
class TreeNodeFolder;
class SyncTreeNodeFile;
class SyncTreeNodeFolder;
class SyncFeedback;
class SyncDBBase;

class AFX_EXT_CLASS UpdateDuplicatesManager
{
public:
	enum PathEntryFlags
	{
		PATH_ENTRY_FLAGS_ACTION_MASK						= 7<<0,

		PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE			= 0<<0,
		PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE				= 1<<0,
		PATH_ENTRY_FLAGS_ACTION_DEST_FILE				= 2<<0,
		PATH_ENTRY_FLAGS_ACTION_DELETE_FILE				= 3<<0,

		PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK			= 7<<3,

		PATH_ENTRY_FLAGS_DEFAULT_ACTION_EXCLUDE_FILE	= 0<<3,
		PATH_ENTRY_FLAGS_DEFAULT_ACTION_SOURCE_FILE	= 1<<3,
		PATH_ENTRY_FLAGS_DEFAULT_ACTION_DEST_FILE		= 2<<3,

		PATH_ENTRY_FLAGS_DEFAULT_ACTION_SHIFT			= 3,

		PATH_ENTRY_FIRST_FILE								= 1<<6,

	};

	enum DuplicateSetFlags
	{
		DUPLICATE_SET_FLAGS_ACTION_MASK					= 7<<0,

		DUPLICATE_SET_FLAGS_ACTION_ENABLE				= 0<<0,
		DUPLICATE_SET_FLAGS_ACTION_DISABLE				= 1<<0,

		DUPLICATE_SET_FLAGS_DEFAULT_ACTION_MASK		= 7<<3,
		
		DUPLICATE_SET_FLAGS_DEFAULT_ACTION_ENABLE		= 0<<3,
		DUPLICATE_SET_FLAGS_DEFAULT_ACTION_DISABLE	= 1<<3,

		DUPLICATE_SET_FLAGS_DEFAULT_ACTION_SHIFT		= 3,

	};

	enum ValidActionFlags
	{
		VALID_ACTIONS_SET_DEFAULT							= 1<<0,
		VALID_ACTIONS_EXCLUDE_FILE							= 1<<1,
		VALID_ACTIONS_SOURCE_FILE							= 1<<2,
		VALID_ACTIONS_DEST_FILE								= 1<<3,
		VALID_ACTIONS_DELETE_FILE							= 1<<4,
	};

	enum Actions
	{
		ACTIONS_SET_DEFAULT,
		ACTIONS_EXCLUDE_FILE,
		ACTIONS_SOURCE_FILE,
		ACTIONS_DEST_FILE,
		ACTIONS_DELETE_FILE,
	};

	class PathEntry
	{
	public:
		int m_flags;
		TupString m_pathString;
		FILETIME m_fileTime;
		TreeNodeFile *m_pTreeNodeFile;
		void *m_userData;

		bool operator==(const PathEntry& other) const;
		bool operator<=(const PathEntry& other) const;
		bool operator>=(const PathEntry& other) const;
		bool operator<(const PathEntry& other) const;
		bool operator>(const PathEntry& other) const;
	};

	class DuplicateSet
	{
	public:
		TupString m_name;
		int m_flags;
		TupArray<PathEntry> m_entries;
		void *m_userData;

		bool operator==(const DuplicateSet& other) const;
		bool operator<=(const DuplicateSet& other) const;
		bool operator>=(const DuplicateSet& other) const;
		bool operator<(const DuplicateSet& other) const;
		bool operator>(const DuplicateSet& other) const;
	};

	enum Flags
	{
		RIGHT_SYNC_NODES				= 0x0001,
		INCLUDE_FROZEN_DUPLICATES  = 0x0002,
		INCLUDE_PARENT_IN_NAME		= 0x0004,
	};

private:
	Keeper<TupString> *m_pSetNames;
	TupArray<DuplicateSet> m_duplicateSets;
	int m_flags;

public:
	UpdateDuplicatesManager(int flags);
	~UpdateDuplicatesManager();

	void AddSelectedFile(SyncTreeNodeFile *pSyncTreeNodeFile);
	void AddSelectedFolder(SyncTreeNodeFolder *pSyncTreeNodeFolder);
	void AddDuplicateFiles(SyncTreeNodeFolder *pSyncTreeNodeFolder);
	void InitAfterAdd(void);

	int GetNumDuplicateSets(void) const;
	const char *GetDuplicateSetName(int setIndex) const;
	int GetDuplicateSetFlags(int setIndex) const;
	void *GetDuplicateSetUserData(int setIndex) const;
	void GetStats(int &numFilesToCopy,__int64 &numBytesToCopy,int &numFilesToDelete) const;
	void GetSetStats(int setIndex,FILETIME &oldestTime,FILETIME &newestTime,int &smallestSize,int &largestSize) const;

	int GetNumPathEntries(int setIndex) const;
	const char *GetPathEntryString(int setIndex,int entryIndex) const;
	FILETIME GetPathEntryModifiedTime(int setIndex,int entryIndex) const;
	TreeNodeFile *GetPathEntryNode(int setIndex,int entryIndex) const;
	int GetPathEntryFlags(int setIndex,int entryIndex) const;
	void *GetPathEntryUserData(int setIndex,int entryIndex) const;

	void SetDuplicateSetFlags(int setIndex,int flags);
	void SetPathEntryFlags(int setIndex,int entryIndex,int flags);
	void SetDuplicateSetUserData(int setIndex,void *userData);
	void SetPathEntryUserData(int setIndex,int entryIndex,void *userData);

	bool UpdateDuplicates(SyncDBBase *pDatabase,SyncFeedback &feedback);
	int GetValidActions(const TupArray<int> &setIndices,const TupArray<int> &entryIndices);
	void SetAction(const TupArray<int> &setIndices,const TupArray<int> &entryIndices,Actions action);

	static int CompareTimes(FILETIME leftTime,FILETIME rightTime);

private:
	void AddDuplicateFile(SyncTreeNodeFile *pSyncTreeNodeFile);
	void DeleteSingleEntrySets(void);
	void SetDefaultActions(void);
	void SetDefaultActions(int setIndex);
	void SetActionsFromDefault(void);
	void SetActionsFromDefault(int setIndex,bool bSetEntries);
	void SetActionsFromDefault(int setIndex,int entryIndex);
	bool IsSetUsingDefault(int setIndex);
	bool IsEntryUsingDefault(int setIndex,int entryIndex);
	void Sort(void);
	void FixSourceDest(void);
};

//-----------------------------------------------------------------------------



#endif

