////////////////////////////////////////////////////////////////////////////
//
// UpdateDuplicates
//
// Class used for managing duplicate files
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: UpdateDuplicates.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 7/15/03    Time: 12:54p
 * Updated in $/Avalanche/tools/SyncUtils
 * fixed case check
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 6/30/03    Time: 2:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * case sensitive keeper fix
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/21/03    Time: 4:55p
 * Updated in $/Avalanche/tools/SyncUtils
 * hotkeys
 * rename right
 * auto freeze extensions
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

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "UpdateDuplicates.h"
#include "SyncTreeNodeFolder.h"
#include "SyncTreeNodeFile.h"
#include "TreeIterator.h"
#include "SyncDBBase.h"
#include "SyncFeedback.h"
#include "SyncTreeNodeFlags.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// path entry methods
bool UpdateDuplicatesManager::PathEntry::operator==(const UpdateDuplicatesManager::PathEntry& other) const
{
	int timeCompare = CompareTimes(m_fileTime,other.m_fileTime);
	return (timeCompare==0 && m_pathString.CompareNoCase(other.m_pathString)==0);
}

bool UpdateDuplicatesManager::PathEntry::operator<=(const UpdateDuplicatesManager::PathEntry& other) const
{
	int timeCompare = CompareTimes(m_fileTime,other.m_fileTime);
	if (timeCompare==1)
		return true;
	return (timeCompare==0 && m_pathString.CompareNoCase(other.m_pathString)<=0);
}

bool UpdateDuplicatesManager::PathEntry::operator>=(const UpdateDuplicatesManager::PathEntry& other) const
{
	int timeCompare = CompareTimes(m_fileTime,other.m_fileTime);
	if (timeCompare==-1)
		return true;
	return (timeCompare==0 && m_pathString.CompareNoCase(other.m_pathString)>=0);
}

bool UpdateDuplicatesManager::PathEntry::operator<(const UpdateDuplicatesManager::PathEntry& other) const
{
	int timeCompare = CompareTimes(m_fileTime,other.m_fileTime);
	if (timeCompare==1)
		return true;
	return (timeCompare==0 && m_pathString.CompareNoCase(other.m_pathString)<0);
}

bool UpdateDuplicatesManager::PathEntry::operator>(const UpdateDuplicatesManager::PathEntry& other) const
{
	int timeCompare = CompareTimes(m_fileTime,other.m_fileTime);
	if (timeCompare==-1)
		return true;
	return (timeCompare==0 && m_pathString.CompareNoCase(other.m_pathString)>0);
}

// duplicate set methods
bool UpdateDuplicatesManager::DuplicateSet::operator==(const UpdateDuplicatesManager::DuplicateSet& other) const
{
	return m_name.CompareNoCase(other.m_name)==0;
}

bool UpdateDuplicatesManager::DuplicateSet::operator<=(const UpdateDuplicatesManager::DuplicateSet& other) const
{
	return m_name.CompareNoCase(other.m_name)<=0;
}

bool UpdateDuplicatesManager::DuplicateSet::operator>=(const UpdateDuplicatesManager::DuplicateSet& other) const
{
	return m_name.CompareNoCase(other.m_name)>=0;
}

bool UpdateDuplicatesManager::DuplicateSet::operator<(const UpdateDuplicatesManager::DuplicateSet& other) const
{
	return m_name.CompareNoCase(other.m_name)<0;
}

bool UpdateDuplicatesManager::DuplicateSet::operator>(const UpdateDuplicatesManager::DuplicateSet& other) const
{
	return m_name.CompareNoCase(other.m_name)>0;
}

	
UpdateDuplicatesManager::UpdateDuplicatesManager(int flags) : m_flags(flags)
{
	m_pSetNames = new Keeper<TupString>;
}

UpdateDuplicatesManager::~UpdateDuplicatesManager()
{
	delete m_pSetNames;
}

void UpdateDuplicatesManager::AddSelectedFile(SyncTreeNodeFile *pSyncTreeNodeFile)
{
	int flags = pSyncTreeNodeFile->GetFlags();

	if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		return;

	if (!(flags & SyncTreeNodeFlags::FLAGS_VISIBLE))
		return;

	TreeNodeFile *pTreeNodeFile;
	if (m_flags & RIGHT_SYNC_NODES)
	{
		pTreeNodeFile = pSyncTreeNodeFile->GetRightFileNode();
	}
	else
	{
		pTreeNodeFile = pSyncTreeNodeFile->GetLeftFileNode();
	}
	if (!pTreeNodeFile)
		return;

	ASSERT(m_pSetNames);
	TupString name = pTreeNodeFile->GetFileName();
	// put the parent directory name in front
	if (m_flags & INCLUDE_PARENT_IN_NAME)
	{
		TreeNodeBase *pParent = pTreeNodeFile->GetParent();
		if (pParent)
		{
			TupString newName(pParent->GetFileName());
			newName+="\\";
			newName+=name;
			name = newName;
		}
		else
		{
			TupString newName("\\");
			newName+=name;
			name = newName;
		}
	}
	TupString lowerCaseName(name);
	lowerCaseName.MakeLower();

	int numOldNames = m_pSetNames->GetSize();
	int nameIndex = m_pSetNames->Add(lowerCaseName);
	// if we just added one
	if (nameIndex==numOldNames)
	{
		PathEntry newPathEntry;
		newPathEntry.m_flags = PATH_ENTRY_FIRST_FILE;
		newPathEntry.m_pTreeNodeFile = pTreeNodeFile;
		newPathEntry.m_fileTime = pTreeNodeFile->GetModifiedTime();
		pTreeNodeFile->GetPath(newPathEntry.m_pathString);

		DuplicateSet newDuplicateSet;
		newDuplicateSet.m_name = name;
		newDuplicateSet.m_flags = 0;
		newDuplicateSet.m_entries.Add(newPathEntry);
		m_duplicateSets.Add(newDuplicateSet);
	}
	// already one there
	else
	{
		DuplicateSet &duplicateSet = m_duplicateSets[nameIndex];

		int numEntries = duplicateSet.m_entries.GetSize();
		for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
		{
			PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
			if (pathEntry.m_pTreeNodeFile == pTreeNodeFile)
				return; // is already in the list then we are done
		}
		PathEntry newPathEntry;
		newPathEntry.m_flags = 0;
		newPathEntry.m_pTreeNodeFile = pTreeNodeFile;
		newPathEntry.m_fileTime = pTreeNodeFile->GetModifiedTime();
		pTreeNodeFile->GetPath(newPathEntry.m_pathString);
		duplicateSet.m_entries.Add(newPathEntry);
	}
}

void UpdateDuplicatesManager::AddSelectedFolder(SyncTreeNodeFolder *pSyncTreeNodeFolder)
{
	int flags = pSyncTreeNodeFolder->GetFlags();
	
	if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		return;
	
	if (!(flags & SyncTreeNodeFlags::FLAGS_VISIBLE))
		return;

	TreeNodeFolder *pTreeNodeFolder;
	if (m_flags & RIGHT_SYNC_NODES)
	{
		pTreeNodeFolder = pSyncTreeNodeFolder->GetRightFolderNode();
	}
	else
	{
		pTreeNodeFolder = pSyncTreeNodeFolder->GetLeftFolderNode();
	}
	if (!pTreeNodeFolder)
		return;

	ASSERT(m_pSetNames);
	TreeIterator first(pSyncTreeNodeFolder);
	TreeIterator last(pSyncTreeNodeFolder);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(*first);
		if (pSyncNodeFolder)
		{
			AddSelectedFolder(pSyncNodeFolder);
		}
		SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(*first);
		if (pSyncNodeFile)
		{
			AddSelectedFile(pSyncNodeFile);
		}
	}
}

void UpdateDuplicatesManager::AddDuplicateFiles(SyncTreeNodeFolder *pSyncTreeNodeFolder)
{
	// if we don't include frozen ones 
	if ((m_flags && INCLUDE_FROZEN_DUPLICATES)==0)
	{
		int flags = pSyncTreeNodeFolder->GetFlags();
		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
			return;
	}

	ASSERT(m_pSetNames);
	TreeIterator first(pSyncTreeNodeFolder);
	TreeIterator last(pSyncTreeNodeFolder);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(*first);
		if (pSyncNodeFolder)
		{
			AddDuplicateFiles(pSyncNodeFolder);
		}
		SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(*first);
		if (pSyncNodeFile)
		{
			AddDuplicateFile(pSyncNodeFile);
		}
	}
}

void UpdateDuplicatesManager::AddDuplicateFile(SyncTreeNodeFile *pSyncTreeNodeFile)
{
	// if we don't include frozen ones 
	if ((m_flags && INCLUDE_FROZEN_DUPLICATES)==0)
	{
		int flags = pSyncTreeNodeFile->GetFlags();
		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
			return;
	}

	TreeNodeFile *pTreeNodeFile;
	if (m_flags & RIGHT_SYNC_NODES)
	{
		pTreeNodeFile = pSyncTreeNodeFile->GetRightFileNode();
	}
	else
	{
		pTreeNodeFile = pSyncTreeNodeFile->GetLeftFileNode();
	}
	if (!pTreeNodeFile)
		return;

	ASSERT(m_pSetNames);
	TupString name = pTreeNodeFile->GetFileName();
	// put the parent directory name in front
	if (m_flags & INCLUDE_PARENT_IN_NAME)
	{
		TreeNodeBase *pParent = pTreeNodeFile->GetParent();
		if (pParent)
		{
			TupString newName(pParent->GetFileName());
			newName+="\\";
			newName+=name;
			name = newName;
		}
		else
		{
			TupString newName("\\");
			newName+=name;
			name = newName;
		}
	}

	TupString lowerCaseName(name);
	lowerCaseName.MakeLower();

	int nameIndex = m_pSetNames->GetIndex(lowerCaseName);
	// if this file can be in a set
	if (nameIndex!=-1)
	{
		DuplicateSet &duplicateSet = m_duplicateSets[nameIndex];
		
		int numEntries = duplicateSet.m_entries.GetSize();
		for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
		{
			PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
			if (pathEntry.m_pTreeNodeFile == pTreeNodeFile)
				return; // is already in the list then we are done
		}
		PathEntry newPathEntry;
		newPathEntry.m_flags = 0;
		newPathEntry.m_pTreeNodeFile = pTreeNodeFile;
		newPathEntry.m_fileTime = pTreeNodeFile->GetModifiedTime();
		pTreeNodeFile->GetPath(newPathEntry.m_pathString);
		duplicateSet.m_entries.Add(newPathEntry);
	}
}


void UpdateDuplicatesManager::InitAfterAdd(void)
{
	delete m_pSetNames;
	m_pSetNames = NULL;
	DeleteSingleEntrySets();
	SetDefaultActions();
	SetActionsFromDefault();
	Sort();
}

void UpdateDuplicatesManager::DeleteSingleEntrySets(void)
{
	for (int setIndex=m_duplicateSets.GetSize()-1;setIndex>=0;setIndex--)
	{
		if (m_duplicateSets[setIndex].m_entries.GetSize()<2)
		{
			m_duplicateSets.RemoveAt(setIndex);
		}
	}
}

void UpdateDuplicatesManager::SetDefaultActions(void)
{
	int numSets = m_duplicateSets.GetSize();
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		SetDefaultActions(setIndex);
	}
}

void UpdateDuplicatesManager::SetDefaultActions(int setIndex)
{
	DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
	int numEntries = duplicateSet.m_entries.GetSize();

	int sourceEntryIndex = -1;
	FILETIME newestFileTime;

	int entryIndex;
	// find out which entry should be the source entry 
	// the first entry is the one that was added due to selection
	// if the first entry is one that has the newest file time it will be the source
	for (entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
		if (sourceEntryIndex==-1 || CompareTimes(newestFileTime,pathEntry.m_pTreeNodeFile->GetModifiedTime())==-1)
		{
			sourceEntryIndex = entryIndex;
			newestFileTime = pathEntry.m_pTreeNodeFile->GetModifiedTime();
		}
	}
	int numOldEntries = 0;
	// see if we have any entries that are older then the source entry
	for (entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		if (entryIndex!=sourceEntryIndex)
		{
			PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
			if (CompareTimes(newestFileTime,pathEntry.m_pTreeNodeFile->GetModifiedTime())==1)
			{
				// set up to be a dest file
				pathEntry.m_flags = (pathEntry.m_flags&~PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK) | PATH_ENTRY_FLAGS_DEFAULT_ACTION_DEST_FILE;
				numOldEntries++;
			}
			else
			{
				// set up to be excluded
				pathEntry.m_flags = (pathEntry.m_flags&~PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK) | PATH_ENTRY_FLAGS_DEFAULT_ACTION_EXCLUDE_FILE;
			}
		}
	}
	PathEntry &sourceEntry = duplicateSet.m_entries[sourceEntryIndex];
	if (numOldEntries)
	{
		// set up to be the source file
		sourceEntry.m_flags = (sourceEntry.m_flags&~PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK) | PATH_ENTRY_FLAGS_DEFAULT_ACTION_SOURCE_FILE;
		// enable the set
		duplicateSet.m_flags = (duplicateSet.m_flags&~DUPLICATE_SET_FLAGS_DEFAULT_ACTION_MASK) | DUPLICATE_SET_FLAGS_DEFAULT_ACTION_ENABLE;
	}
	else
	{
		// set up to be excluded
		sourceEntry.m_flags = (sourceEntry.m_flags&~PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK) | PATH_ENTRY_FLAGS_DEFAULT_ACTION_EXCLUDE_FILE;
		// disable the set
		duplicateSet.m_flags = (duplicateSet.m_flags&~DUPLICATE_SET_FLAGS_DEFAULT_ACTION_MASK) | DUPLICATE_SET_FLAGS_DEFAULT_ACTION_DISABLE;
	}
}

void UpdateDuplicatesManager::SetActionsFromDefault(void)
{
	int numSets = m_duplicateSets.GetSize();
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		SetActionsFromDefault(setIndex,true);
	}
}

void UpdateDuplicatesManager::SetActionsFromDefault(int setIndex,bool bSetEntries)
{
	DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
	// replace the action with the default
	duplicateSet.m_flags = (duplicateSet.m_flags&~DUPLICATE_SET_FLAGS_ACTION_MASK) | 
								  ((duplicateSet.m_flags&DUPLICATE_SET_FLAGS_DEFAULT_ACTION_MASK)>>DUPLICATE_SET_FLAGS_DEFAULT_ACTION_SHIFT);
	if (bSetEntries)
	{
		int numEntries = duplicateSet.m_entries.GetSize();
		for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
		{
			SetActionsFromDefault(setIndex,entryIndex);
		}
	}
}

void UpdateDuplicatesManager::SetActionsFromDefault(int setIndex,int entryIndex)
{
	DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
	// replace the action with the default
	PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
	pathEntry.m_flags = (pathEntry.m_flags&~PATH_ENTRY_FLAGS_ACTION_MASK) | 
							  ((pathEntry.m_flags&PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK)>>PATH_ENTRY_FLAGS_DEFAULT_ACTION_SHIFT);
}

bool UpdateDuplicatesManager::IsSetUsingDefault(int setIndex)
{
	DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
	bool bSetUsingDefault = ((duplicateSet.m_flags&DUPLICATE_SET_FLAGS_ACTION_MASK) ==
			  ((duplicateSet.m_flags&DUPLICATE_SET_FLAGS_DEFAULT_ACTION_MASK)>>DUPLICATE_SET_FLAGS_DEFAULT_ACTION_SHIFT));
	// if the set is not using the default then we are done
	if (!bSetUsingDefault)
		return false;
	int numEntries = duplicateSet.m_entries.GetSize();
	for (int entryIndex=0;entryIndex<numEntries;entryIndex++)
	{
		bool bEntryUsingDefault = IsEntryUsingDefault(setIndex,entryIndex);
		// if any entries are not using the default then we are done
		if (!bEntryUsingDefault)
			return false;
	}
	// all are using the default
	return true;
}

bool UpdateDuplicatesManager::IsEntryUsingDefault(int setIndex,int entryIndex)
{
	DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
	PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
	return ((pathEntry.m_flags&PATH_ENTRY_FLAGS_ACTION_MASK) ==
			 ((pathEntry.m_flags&PATH_ENTRY_FLAGS_DEFAULT_ACTION_MASK)>>PATH_ENTRY_FLAGS_DEFAULT_ACTION_SHIFT));
}

void UpdateDuplicatesManager::Sort(void)
{
	// sort all the entries
	int numSets = m_duplicateSets.GetSize();
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
		TupSort<PathEntry>::SortAscending(duplicateSet.m_entries.GetData(),duplicateSet.m_entries.GetSize());
	}
	TupSort<DuplicateSet>::SortAscending(m_duplicateSets.GetData(),m_duplicateSets.GetSize());
}

int UpdateDuplicatesManager::GetNumDuplicateSets(void) const
{
	return m_duplicateSets.GetSize();
}

const char *UpdateDuplicatesManager::GetDuplicateSetName(int setIndex) const
{
	return m_duplicateSets[setIndex].m_name;
}

int UpdateDuplicatesManager::GetDuplicateSetFlags(int setIndex) const
{
	return m_duplicateSets[setIndex].m_flags;
}

void *UpdateDuplicatesManager::GetDuplicateSetUserData(int setIndex) const
{
	return m_duplicateSets[setIndex].m_userData;
}

int UpdateDuplicatesManager::GetNumPathEntries(int setIndex) const
{
	return m_duplicateSets[setIndex].m_entries.GetSize();
}

const char *UpdateDuplicatesManager::GetPathEntryString(int setIndex,int entryIndex) const
{
	return m_duplicateSets[setIndex].m_entries[entryIndex].m_pathString;
}

FILETIME UpdateDuplicatesManager::GetPathEntryModifiedTime(int setIndex,int entryIndex) const
{
	return m_duplicateSets[setIndex].m_entries[entryIndex].m_fileTime;
}

TreeNodeFile *UpdateDuplicatesManager::GetPathEntryNode(int setIndex,int entryIndex) const
{
	return m_duplicateSets[setIndex].m_entries[entryIndex].m_pTreeNodeFile;
}

int UpdateDuplicatesManager::GetPathEntryFlags(int setIndex,int entryIndex) const
{
	return m_duplicateSets[setIndex].m_entries[entryIndex].m_flags;
}

void *UpdateDuplicatesManager::GetPathEntryUserData(int setIndex,int entryIndex) const
{
	return m_duplicateSets[setIndex].m_entries[entryIndex].m_userData;
}

void UpdateDuplicatesManager::SetDuplicateSetFlags(int setIndex,int flags)
{
	m_duplicateSets[setIndex].m_flags = flags;
}

void UpdateDuplicatesManager::SetDuplicateSetUserData(int setIndex,void *userData)
{
	m_duplicateSets[setIndex].m_userData = userData;
}

void UpdateDuplicatesManager::SetPathEntryFlags(int setIndex,int entryIndex,int flags)
{
	m_duplicateSets[setIndex].m_entries[entryIndex].m_flags = flags;
}

void UpdateDuplicatesManager::SetPathEntryUserData(int setIndex,int entryIndex,void *userData)
{
	m_duplicateSets[setIndex].m_entries[entryIndex].m_userData = userData;
}

bool UpdateDuplicatesManager::UpdateDuplicates(SyncDBBase *pDatabase,SyncFeedback &feedback)
{
	int filesToCopy = 0;
	int filesToDelete = 0;
	int filesCopied = 0;
	int filesDeleted = 0;

	int numDuplicateSets = m_duplicateSets.GetSize();
	feedback.m_totalBytes = 0;
	int setIndex;
	int entryIndex;

	for (setIndex=0;setIndex<numDuplicateSets;setIndex++)
	{
		DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
		if ((duplicateSet.m_flags & DUPLICATE_SET_FLAGS_ACTION_MASK) == DUPLICATE_SET_FLAGS_ACTION_ENABLE)
		{
			int filesInSetToCopy = 0;
			__int64 totalBytesInSetToCopy = 0;
			bool bFoundSourceFile = false;
			int numPathEntries = duplicateSet.m_entries.GetSize();
			for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
			{
				PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
				switch (pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK)
				{
					case PATH_ENTRY_FLAGS_ACTION_DEST_FILE:
						filesInSetToCopy++;
						totalBytesInSetToCopy += pathEntry.m_pTreeNodeFile->GetFileLength();
						break;
					case PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE:
						bFoundSourceFile = true;
						break;
					case PATH_ENTRY_FLAGS_ACTION_DELETE_FILE:
						filesToDelete++;
						break;
				}
			}
			if (bFoundSourceFile)
			{
				filesToCopy += filesInSetToCopy;
				feedback.m_totalBytes += totalBytesInSetToCopy;
			}
		}
	}
	if (filesToCopy)
	{
		for (setIndex=0;setIndex<numDuplicateSets;setIndex++)
		{
			DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
			if ((duplicateSet.m_flags & DUPLICATE_SET_FLAGS_ACTION_MASK) == DUPLICATE_SET_FLAGS_ACTION_ENABLE)
			{
				TreeNodeFile *pSourceNodeFile = NULL;
				int numPathEntries = duplicateSet.m_entries.GetSize();
				for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
				{
					PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
					if ((pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK) == PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE)
					{
						pSourceNodeFile = duplicateSet.m_entries[entryIndex].m_pTreeNodeFile;
						break;
					}
				}
				// make sure we have a source file
				if (pSourceNodeFile)
				{
					for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
					{
						if (!feedback.GetAbortFlag())
						{
							PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
							if ((pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK) == PATH_ENTRY_FLAGS_ACTION_DEST_FILE)
							{
								TreeNodeFile *pDestNodeFile = duplicateSet.m_entries[entryIndex].m_pTreeNodeFile;
								TreeNodeFolder *pDestNodeParent = (TreeNodeFolder *)pDestNodeFile->GetParent();
								int flags = 0;
								TreeNodeFile *pResultFile = pDatabase->CopyFileToDB(pDatabase,pSourceNodeFile,pDestNodeParent,pDestNodeFile,feedback,flags,true);
								if (pResultFile)
								{
									filesCopied++;
								}
							}
						}
					}
				}
				for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
				{
					if (!feedback.GetAbortFlag())
					{
						PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
						if ((pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK) == PATH_ENTRY_FLAGS_ACTION_DELETE_FILE)
						{
							TreeNodeFile *pDeleteNodeFile = duplicateSet.m_entries[entryIndex].m_pTreeNodeFile;
							if (pDatabase->DeleteFileFromDB(pDeleteNodeFile,feedback,true))
								filesDeleted++;
						}
					}
				}
			}
		}
	}
	return (filesDeleted || filesCopied);
}

int UpdateDuplicatesManager::GetValidActions(const TupArray<int> &setIndices,const TupArray<int> &entryIndices)
{
	int validActions = 0;
	int numSelectedItems = setIndices.GetSize();
	for (int itemIndex=0;itemIndex<numSelectedItems;itemIndex++)
	{
		int setIndex = setIndices[itemIndex];
		DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
		int entryIndex = entryIndices[itemIndex];
		// if the set is selected itself
		if (entryIndex==-1)
		{
			if (!IsSetUsingDefault(setIndex))
			{
				validActions |= ACTIONS_SET_DEFAULT;
			}
			if ((duplicateSet.m_flags & DUPLICATE_SET_FLAGS_ACTION_MASK) == DUPLICATE_SET_FLAGS_ACTION_ENABLE)
			{
				validActions |= VALID_ACTIONS_EXCLUDE_FILE;
			}
		}
		// if an entry is selected
		else
		{
			if (!IsEntryUsingDefault(setIndex,entryIndex))
			{
				validActions |= VALID_ACTIONS_SET_DEFAULT;
			}
			PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];

			switch (pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK)
			{
				case PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE:
					validActions |= VALID_ACTIONS_DELETE_FILE | VALID_ACTIONS_DEST_FILE;
					if (numSelectedItems==1)
					{
						validActions |= VALID_ACTIONS_SOURCE_FILE;
					}
					break;
				case PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE:
					validActions |= VALID_ACTIONS_DELETE_FILE | VALID_ACTIONS_EXCLUDE_FILE | VALID_ACTIONS_DEST_FILE;
					break;
				case PATH_ENTRY_FLAGS_ACTION_DEST_FILE:
					validActions |= VALID_ACTIONS_DELETE_FILE | VALID_ACTIONS_EXCLUDE_FILE;
					if (numSelectedItems==1)
					{
						validActions |= VALID_ACTIONS_SOURCE_FILE;
					}
					break;
				case PATH_ENTRY_FLAGS_ACTION_DELETE_FILE:
					validActions |= VALID_ACTIONS_EXCLUDE_FILE | VALID_ACTIONS_DEST_FILE;
					if (numSelectedItems==1)
					{
						validActions |= VALID_ACTIONS_SOURCE_FILE;
					}
					break;
			}
		}
	}
	return validActions;
}

void UpdateDuplicatesManager::SetAction(const TupArray<int> &setIndices,const TupArray<int> &entryIndices,UpdateDuplicatesManager::Actions action)
{
	int numSelectedItems = setIndices.GetSize();
	for (int itemIndex=0;itemIndex<numSelectedItems;itemIndex++)
	{
		int setIndex = setIndices[itemIndex];
		DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
		int entryIndex = entryIndices[itemIndex];
		// if the set is selected itself
		if (entryIndex==-1)
		{
			switch (action)
			{
				case ACTIONS_SET_DEFAULT:
					SetActionsFromDefault(setIndex,true);
					break;
				case ACTIONS_EXCLUDE_FILE:
					{
						duplicateSet.m_flags = (duplicateSet.m_flags	&~DUPLICATE_SET_FLAGS_ACTION_MASK) | DUPLICATE_SET_FLAGS_ACTION_DISABLE;
						int numEntries = duplicateSet.m_entries.GetSize();
						for (entryIndex=0;entryIndex<numEntries;entryIndex++)
						{
							PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
							pathEntry.m_flags = (pathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE;
						}
					}
					break;
			}
		}
		// if an entry is selected
		else
		{
			PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
			
			switch (action)
			{
				case ACTIONS_SET_DEFAULT:
					SetActionsFromDefault(setIndex,entryIndex);
					break;
				case ACTIONS_EXCLUDE_FILE:
					pathEntry.m_flags = (pathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE;
					break;
				case ACTIONS_SOURCE_FILE:
					{
						// find any that are set to a source file and mark as the same as requested entry
						int numEntries = duplicateSet.m_entries.GetSize();
						for (int findEntryIndex=0;findEntryIndex<numEntries;findEntryIndex++)
						{
							PathEntry &findPathEntry = duplicateSet.m_entries[findEntryIndex];
							if ((findPathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK) == PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE)
							{
								findPathEntry.m_flags = (findPathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | (pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK);
							}
						}
						pathEntry.m_flags = (pathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE;
					}
					break;
				case ACTIONS_DEST_FILE:
					pathEntry.m_flags = (pathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | PATH_ENTRY_FLAGS_ACTION_DEST_FILE;
					break;
				case ACTIONS_DELETE_FILE:
					pathEntry.m_flags = (pathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | PATH_ENTRY_FLAGS_ACTION_DELETE_FILE;
					break;
			}
		}
	}
	FixSourceDest();
}

// turns off dest files if there is not a source file in the set
// also enables/disables the set if there is anything to do or not
void UpdateDuplicatesManager::FixSourceDest(void)
{
	int numDuplicateSets = m_duplicateSets.GetSize();
	for (int setIndex=0;setIndex<numDuplicateSets;setIndex++)
	{
		DuplicateSet &duplicateSet = m_duplicateSets[setIndex];

		bool bFoundSourceFile = false;
		int numDestFiles = 0;
		int numDeleteFiles = 0;
		
		int numPathEntries = duplicateSet.m_entries.GetSize();
		int entryIndex;
		for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
		{
			PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
			switch (pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK)
			{
			case PATH_ENTRY_FLAGS_ACTION_DEST_FILE:
				numDestFiles++;
				break;
			case PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE:
				bFoundSourceFile = true;
				break;
			case PATH_ENTRY_FLAGS_ACTION_DELETE_FILE:
				numDeleteFiles++;
				break;
			}
		}
		// if no source file then exclude all dest files
		if (!bFoundSourceFile)
		{
			if (numDestFiles)
			{
				for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
				{
					PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
					if ((pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK) == PATH_ENTRY_FLAGS_ACTION_DEST_FILE)
						pathEntry.m_flags = (pathEntry.m_flags &~PATH_ENTRY_FLAGS_ACTION_MASK) | PATH_ENTRY_FLAGS_ACTION_EXCLUDE_FILE;
				}
			}
			numDestFiles=0;
		}
		// if set enabled
		if ((duplicateSet.m_flags & DUPLICATE_SET_FLAGS_ACTION_MASK) == DUPLICATE_SET_FLAGS_ACTION_ENABLE)
		{
			// if nothing to do then disable the set
			if (!numDestFiles && !numDeleteFiles)
			{
				duplicateSet.m_flags = (duplicateSet.m_flags	&~DUPLICATE_SET_FLAGS_ACTION_MASK) | DUPLICATE_SET_FLAGS_ACTION_DISABLE;
			}
		}
		// if set disabled
		else
		{
			// if something to do then enable the set
			if (numDestFiles || numDeleteFiles)
			{
				duplicateSet.m_flags = (duplicateSet.m_flags	&~DUPLICATE_SET_FLAGS_ACTION_MASK) | DUPLICATE_SET_FLAGS_ACTION_ENABLE;
			}
		}
	}
}

void UpdateDuplicatesManager::GetStats(int &numFilesToCopy,__int64 &numBytesToCopy,int &numFilesToDelete) const
{
	numFilesToCopy = 0;
	numFilesToDelete = 0;
	numBytesToCopy = 0;

	int numDuplicateSets = m_duplicateSets.GetSize();
	int setIndex;
	int entryIndex;

	for (setIndex=0;setIndex<numDuplicateSets;setIndex++)
	{
		const DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
		if ((duplicateSet.m_flags & DUPLICATE_SET_FLAGS_ACTION_MASK) == DUPLICATE_SET_FLAGS_ACTION_ENABLE)
		{
			int filesInSetToCopy = 0;
			__int64 totalBytesInSetToCopy = 0;
			bool bFoundSourceFile = false;
			int numPathEntries = duplicateSet.m_entries.GetSize();
			for (entryIndex=0;entryIndex<numPathEntries;entryIndex++)
			{
				const PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
				switch (pathEntry.m_flags & PATH_ENTRY_FLAGS_ACTION_MASK)
				{
					case PATH_ENTRY_FLAGS_ACTION_DEST_FILE:
						filesInSetToCopy++;
						totalBytesInSetToCopy += pathEntry.m_pTreeNodeFile->GetFileLength();
						break;
					case PATH_ENTRY_FLAGS_ACTION_SOURCE_FILE:
						bFoundSourceFile = true;
						break;
					case PATH_ENTRY_FLAGS_ACTION_DELETE_FILE:
						numFilesToDelete++;
						break;
				}
			}
			if (bFoundSourceFile)
			{
				numFilesToCopy += filesInSetToCopy;
				numBytesToCopy += totalBytesInSetToCopy;
			}
		}
	}
}

void UpdateDuplicatesManager::GetSetStats(int setIndex,FILETIME &oldestTime,FILETIME &newestTime,int &smallestSize,int &largestSize) const
{
	bool bSetOldestTime = false;
	bool bSetNewestTime = false;
	bool bSetSmallestSize = false;
	bool bSetLargestSize = false;

	const DuplicateSet &duplicateSet = m_duplicateSets[setIndex];
	int numPathEntries = duplicateSet.m_entries.GetSize();
	for (int entryIndex=0;entryIndex<numPathEntries;entryIndex++)
	{
		const PathEntry &pathEntry = duplicateSet.m_entries[entryIndex];
		if (!bSetOldestTime || CompareTimes(oldestTime,pathEntry.m_fileTime)==-1)
		{
			oldestTime = pathEntry.m_fileTime;
			bSetOldestTime = true;
		}
		if (!bSetNewestTime || CompareTimes(newestTime,pathEntry.m_fileTime)==1)
		{
			newestTime = pathEntry.m_fileTime;
			bSetNewestTime = true;
		}
		if (!bSetSmallestSize || smallestSize>(int)pathEntry.m_pTreeNodeFile->GetFileLength())
		{
			smallestSize = pathEntry.m_pTreeNodeFile->GetFileLength();
			bSetSmallestSize = true;
		}
		if (!bSetLargestSize || largestSize<(int)pathEntry.m_pTreeNodeFile->GetFileLength())
		{
			largestSize = pathEntry.m_pTreeNodeFile->GetFileLength();
			bSetLargestSize = true;
		}
	}
}

int UpdateDuplicatesManager::CompareTimes(FILETIME leftTime,FILETIME rightTime)
{
	__int64 leftValue = *((__int64*)&leftTime);
	__int64 rightValue = *((__int64*)&rightTime);
	__int64 diff = leftValue-rightValue;
	__int64 threshold = 2*10000000; // 2 seconds * 10 million (100 nanoseconds periods)
	if (diff<-threshold)
	{
		return -1; // left is older than right
	}
	else if (diff>threshold)
	{
		return 1; // right is older then left
	}
	else
	{
		return 0; // left and right are within the threshold
	}
}
