////////////////////////////////////////////////////////////////////////////
//
// FolderSets
//
// Class for managing the folder sets
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FolderSets.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 6/30/03    Time: 1:38p
 * Updated in $/Avalanche/tools/AssetManager
 * added thread priorities - fix for resource files
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/AssetManager
 * CRC check
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/20/03    Time: 2:20p
 * Updated in $/Avalanche/tools/AssetManager
 * dialog renaming
 * auto freeze paths name changes
 * remove from list (including children)
 * fix bug with not clearing the auto freeze path flag
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/14/03    Time: 4:35p
 * Updated in $/Avalanche/tools/AssetManager
 * options for folder sets
 * ui for autofreeze list
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/AssetManager
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/20/03    Time: 1:12p
 * Updated in $/Avalanche/tools/AssetManager
 * Phase 2 complete
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/16/03    Time: 4:29p
 * Updated in $/Avalanche/tools/AssetManager
 * More work on Phase 2 - Getting the folder sets dialog working
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/15/03    Time: 4:12p
 * Updated in $/Avalanche/tools/AssetManager
 * part one of client database stuff
 */

#include "stdafx.h"
#include "FolderSets.h"
#include "Registry.h"
#include "RegistryKeys.h"


TupArray<FolderSet> m_folderSetsArray;
int m_activeSet;
	
FolderSets::FolderSets(void) : m_activeSet(-1)
{
}
	
void FolderSets::LoadFromRegistry(const char *pFolderSetToLoad)
{
	// start with empty
	m_folderSetsArray.RemoveAll();
	m_activeSet = -1;
	m_previousLeftPaths.RemoveAll();
	m_previousRightPaths.RemoveAll();
	
	Registry registry(FOLDER_SETS_KEY);

	int numFolderSets = 0;
	char numString[10];
	DWORD tempInt;
#define STRING_SIZE 512
	char tempString[STRING_SIZE];

	if (registry.Get(TOTAL_FOLDER_SETS_FIELD,&tempInt))
		numFolderSets = tempInt;

	bool bSetActive = false;
	for (int folderSetIndex=0;folderSetIndex<numFolderSets;folderSetIndex++)
	{
		itoa(folderSetIndex,numString,10);

		FolderSet newFolderSet;
		TupString activeField(FOLDER_SET_ENABLED_FIELD);
		activeField+= numString; // append the set number
		// if this slot is active
		if (registry.Get(activeField,&tempInt) && tempInt)
		{
			newFolderSet.SetEnabled(true);
			// get the name
			TupString nameField(FOLDER_SET_NAME_FIELD);
			nameField+= numString;
			if (registry.Get(nameField,&tempString,STRING_SIZE))
			{
				newFolderSet.SetName(tempString);
				if (!bSetActive && pFolderSetToLoad)
				{
					if (!stricmp(tempString,pFolderSetToLoad))
					{
						bSetActive = true;
						m_activeSet = folderSetIndex;
					}
				}
			}
			// get the description
			TupString descriptionField(FOLDER_SET_DESCRIPTION_FIELD);
			descriptionField+= numString;
			if (registry.Get(descriptionField,&tempString,STRING_SIZE))
			{
				newFolderSet.SetDescription(tempString);
			}
			// get the left folder path
			TupString leftFolderPathField(FOLDER_SET_LEFT_FOLDER_PATH_FIELD);
			leftFolderPathField+= numString;
			if (registry.Get(leftFolderPathField,&tempString,STRING_SIZE))
			{
				newFolderSet.SetLeftFolderPath(tempString);
			}
			// get the right folder path
			TupString rightFolderPathField(FOLDER_SET_RIGHT_FOLDER_PATH_FIELD);
			rightFolderPathField+= numString;
			if (registry.Get(rightFolderPathField,&tempString,STRING_SIZE))
			{
				newFolderSet.SetRightFolderPath(tempString);
			}
			// get the sync style
			TupString syncStyleField(FOLDER_SET_SYNC_STYLE);
			syncStyleField+= numString;
			if (registry.Get(syncStyleField,&tempInt))
			{
				newFolderSet.SetSyncStyle(tempInt);
			}
			// get the flag doing crc checks
			TupString doCRCCheckField(FOLDER_SET_DO_CRC_CHECK);
			doCRCCheckField+= numString;
			if (registry.Get(doCRCCheckField,&tempInt))
			{
				newFolderSet.SetDoCRCCheck(tempInt ? true : false);
			}

			// get the flag using sync dates
			TupString useSyncDates(FOLDER_SET_USE_SYNC_DATES);
			useSyncDates+= numString;
			if (registry.Get(useSyncDates,&tempInt))
			{
				newFolderSet.SetUseSyncDates(tempInt ? true : false);
			}

			// get the flag verify copied files
			TupString verifyCopiedFiles(FOLDER_SET_VERIFY_COPIED_FILES);
			verifyCopiedFiles+= numString;
			if (registry.Get(verifyCopiedFiles,&tempInt))
			{
				newFolderSet.SetVerifyCopiedFiles(tempInt ? true : false);
			}

			// get the flag closed when completed
			TupString closedWhenCompleted(FOLDER_SET_CLOSE_WHEN_COMPLETED);
			closedWhenCompleted+= numString;
			if (registry.Get(closedWhenCompleted,&tempInt))
			{
				newFolderSet.SetCloseWhenCompleted(tempInt ? true : false);
			}

			// get the flag exit when closed
			TupString exitWhenClosed(FOLDER_SET_EXIT_WHEN_CLOSED);
			exitWhenClosed+= numString;
			if (registry.Get(exitWhenClosed,&tempInt))
			{
				newFolderSet.SetExitWhenClosed(tempInt ? true : false);
			}

			// get the thread priorities
			TupString scanThreadPriorityField(FOLDER_SET_SCAN_THREAD_PRIORITY);
			scanThreadPriorityField+= numString;
			if (registry.Get(scanThreadPriorityField,&tempInt))
			{
				newFolderSet.SetScanThreadPriority(tempInt);
			}
			TupString syncThreadPriorityField(FOLDER_SET_SYNC_THREAD_PRIORITY);
			syncThreadPriorityField+= numString;
			if (registry.Get(syncThreadPriorityField,&tempInt))
			{
				newFolderSet.SetSyncThreadPriority(tempInt);
			}

			// get if we are using the default auto freeze extensions
			TupString usingDefaultAutoFreezeExtensions(FOLDER_SET_USING_DEFAULT_AUTO_FREEZE_EXTENSIONS);
			usingDefaultAutoFreezeExtensions+= numString;
			if (registry.Get(usingDefaultAutoFreezeExtensions,&tempInt))
			{
				newFolderSet.SetUsingDefaultAutoFreezeExtensions(tempInt ? true : false);
			}
			// get auto freeze file extensions
			TupString autoFreezeFileExtentions(FOLDER_SET_AUTO_FREEZE_FILE_EXTENSIONS);
			autoFreezeFileExtentions+= numString;
			if (registry.Get(autoFreezeFileExtentions,&tempString,STRING_SIZE))
			{
				newFolderSet.SetAutoFreezeFileExtensions(tempString);
			}
			// get auto freeze file prefixes
			TupString autoFreezeFilePrefixes(FOLDER_SET_AUTO_FREEZE_FILE_PREFIXES);
			autoFreezeFilePrefixes+= numString;
			if (registry.Get(autoFreezeFilePrefixes,&tempString,STRING_SIZE))
			{
				newFolderSet.SetAutoFreezeFilePrefixes(tempString);
			}
			// get auto freeze folder prefixes
			TupString autoFreezeFolderPrefixes(FOLDER_SET_AUTO_FREEZE_FOLDER_PREFIXES);
			autoFreezeFolderPrefixes+= numString;
			if (registry.Get(autoFreezeFolderPrefixes,&tempString,STRING_SIZE))
			{
				newFolderSet.SetAutoFreezeFolderPrefixes(tempString);
			}
			// get last scan time
			TupString lastScanTime(FOLDER_SET_LAST_SCAN_TIME);
			lastScanTime+= numString;
			if (registry.Get(lastScanTime,&tempString,STRING_SIZE))
			{
				CTime time(_atoi64(tempString));
				newFolderSet.SetLastScanTime(time);
			}
		}
		m_folderSetsArray.Add(newFolderSet);
	}
	if (!bSetActive && registry.Get(ACTIVE_FOLDER_SET_FIELD,&tempInt) )
	{
		// make sure a valid and active set
		m_activeSet = (int)tempInt;
		if (m_activeSet<0 || m_activeSet>=m_folderSetsArray.GetSize() ||
			!m_folderSetsArray[m_activeSet].IsEnabled())
		{
			m_activeSet = -1; // set to -1 if not valid
		}
	}

	// load in the previous folders
	Registry regLeftFolders(LEFT_FOLDERS_KEY);
	int nNumVals = regLeftFolders.GetNumValues();
	for (int nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		if (regLeftFolders.Get(itoa(nIndex, numString, 10), tempString, STRING_SIZE))
		{
			if (strlen(tempString) && ::PathFileExists((LPCTSTR) tempString) == TRUE)
			{
				TupString strToAdd(tempString);
				m_previousLeftPaths.Add(strToAdd);
			}
		}
	}

	Registry regRightFolders(RIGHT_FOLDERS_KEY);
	nNumVals = regRightFolders.GetNumValues();
	for (nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		if (regRightFolders.Get(itoa(nIndex, numString, 10), tempString, STRING_SIZE))
		{
			if (strlen(tempString) && ::PathFileExists((LPCTSTR) tempString) == TRUE)
			{
				TupString strToAdd(tempString);
				m_previousRightPaths.Add(strToAdd);
			}
		}
	}
}

void FolderSets::SaveToRegistry(void) const
{
	Registry registry(FOLDER_SETS_KEY);
	
	int numFolderSets = m_folderSetsArray.GetSize();
	
	registry.Set(TOTAL_FOLDER_SETS_FIELD,(DWORD)numFolderSets);
	registry.Set(ACTIVE_FOLDER_SET_FIELD,(DWORD)m_activeSet);

	char numString[10];
	
	for (int folderSetIndex=0;folderSetIndex<numFolderSets;folderSetIndex++)
	{
		itoa(folderSetIndex,numString,10);
		
		const FolderSet &folderSet = m_folderSetsArray[folderSetIndex];

		TupString activeField(FOLDER_SET_ENABLED_FIELD);
		activeField+= numString; // append the set number
		if (m_folderSetsArray[folderSetIndex].IsEnabled())
		{
			// set as active
			registry.Set(activeField,TRUE);

			// set the name
			TupString nameField(FOLDER_SET_NAME_FIELD);
			nameField+= numString;
			registry.Set(nameField,folderSet.GetName());

			// set the description
			TupString descriptionField(FOLDER_SET_DESCRIPTION_FIELD);
			descriptionField+= numString;
			registry.Set(descriptionField,folderSet.GetDescription());

			// set the left folder path
			TupString leftFolderPathField(FOLDER_SET_LEFT_FOLDER_PATH_FIELD);
			leftFolderPathField+= numString;
			registry.Set(leftFolderPathField,folderSet.GetLeftFolderPath());

			// set the right folder path
			TupString rightFolderPathField(FOLDER_SET_RIGHT_FOLDER_PATH_FIELD);
			rightFolderPathField+= numString;
			registry.Set(rightFolderPathField,folderSet.GetRightFolderPath());

			// set the sync styles
			TupString syncStylesField(FOLDER_SET_SYNC_STYLE);
			syncStylesField+= numString;
			registry.Set(syncStylesField,folderSet.GetSyncStyle());

			// set the flag doing crc checks
			TupString doCRCCheckField(FOLDER_SET_DO_CRC_CHECK);
			doCRCCheckField+= numString;
			registry.Set(doCRCCheckField,folderSet.GetDoCRCCheck() ? TRUE : FALSE);

			// set the flag use sync dates 
			TupString useSyncDates(FOLDER_SET_USE_SYNC_DATES);
			useSyncDates+= numString;
			registry.Set(useSyncDates,folderSet.GetUseSyncDates() ? TRUE : FALSE);

			// set the flags verify copied files
			TupString verifyCopiedFiles(FOLDER_SET_VERIFY_COPIED_FILES);
			verifyCopiedFiles+= numString;
			registry.Set(verifyCopiedFiles,folderSet.GetVerifyCopiedFiles() ? TRUE : FALSE);

			// set the flags closed when completed
			TupString closedWhenCompleted(FOLDER_SET_CLOSE_WHEN_COMPLETED);
			closedWhenCompleted+= numString;
			registry.Set(closedWhenCompleted,folderSet.GetCloseWhenCompleted() ? TRUE : FALSE);

			// set the flags exit when closed
			TupString exitWhenClosed(FOLDER_SET_EXIT_WHEN_CLOSED);
			exitWhenClosed+= numString;
			registry.Set(exitWhenClosed,folderSet.GetExitWhenClosed() ? TRUE : FALSE);

			// set the thread priorities
			TupString scanThreadPriorityField(FOLDER_SET_SCAN_THREAD_PRIORITY);
			scanThreadPriorityField+= numString;
			registry.Set(scanThreadPriorityField,folderSet.GetScanThreadPriority());
			TupString syncThreadPriorityField(FOLDER_SET_SYNC_THREAD_PRIORITY);
			syncThreadPriorityField+= numString;
			registry.Set(syncThreadPriorityField,folderSet.GetSyncThreadPriority());

			// Set if we are using the default auto freeze extensions
			TupString usingDefaultAutoFreezeExtensions(FOLDER_SET_USING_DEFAULT_AUTO_FREEZE_EXTENSIONS);
			usingDefaultAutoFreezeExtensions+= numString;
			registry.Set(usingDefaultAutoFreezeExtensions,folderSet.IsUsingDefaultAutoFreezeExtensions() ? TRUE : FALSE);

			// Set auto freeze file extensions
			TupString autoFreezeFileExtentions(FOLDER_SET_AUTO_FREEZE_FILE_EXTENSIONS);
			autoFreezeFileExtentions+= numString;
			registry.Set(autoFreezeFileExtentions,folderSet.GetAutoFreezeFileExtensions());

			// Set auto freeze file prefixes
			TupString autoFreezeFilePrefixes(FOLDER_SET_AUTO_FREEZE_FILE_PREFIXES);
			autoFreezeFilePrefixes+= numString;
			registry.Set(autoFreezeFilePrefixes,folderSet.GetAutoFreezeFilePrefixes());

			// Set auto freeze folder prefixes
			TupString autoFreezeFolderPrefixes(FOLDER_SET_AUTO_FREEZE_FOLDER_PREFIXES);
			autoFreezeFolderPrefixes+= numString;
			registry.Set(autoFreezeFolderPrefixes,folderSet.GetAutoFreezeFolderPrefixes());

			// Set last scan time
			TupString lastScanTime(FOLDER_SET_LAST_SCAN_TIME);
			lastScanTime+= numString;
			char timeString[20];
			_i64toa(folderSet.GetLastScanTime().GetTime(),timeString,10);
			registry.Set(lastScanTime,timeString);
		}
		else
		{
			registry.Set(activeField,(DWORD)FALSE);
		}
	}
	// load in the previous folders
	Registry regLeftFolders(LEFT_FOLDERS_KEY);
	int nNumVals = regLeftFolders.GetNumValues();
	for (int nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		regLeftFolders.Delete(itoa(nIndex, numString, 10));
	}
	nNumVals = m_previousLeftPaths.GetSize();
	for (nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		regLeftFolders.Set(itoa(nIndex, numString, 10),m_previousLeftPaths[nIndex]);
	}
	
	Registry regRightFolders(RIGHT_FOLDERS_KEY);
	nNumVals = regRightFolders.GetNumValues();
	for (nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		regRightFolders.Delete(itoa(nIndex, numString, 10));
	}
	nNumVals = m_previousRightPaths.GetSize();
	for (nIndex = 0; nIndex < nNumVals; nIndex++)
	{
		regRightFolders.Set(itoa(nIndex, numString, 10),m_previousRightPaths[nIndex]);
	}
}

int FolderSets::GetNumFolderSets(void) const
{
	return m_folderSetsArray.GetSize();
}

int FolderSets::GetActiveSet(void) const
{
	return m_activeSet;
}

const char *FolderSets::GetName(int index) const
{
	return m_folderSetsArray[index].GetName();
}

const char *FolderSets::GetDescription(int index) const
{
	return m_folderSetsArray[index].GetDescription();
}

const char *FolderSets::GetLeftFolderPath(int index) const
{
	return m_folderSetsArray[index].GetLeftFolderPath();
}

const char *FolderSets::GetRightFolderPath(int index) const
{
	return m_folderSetsArray[index].GetRightFolderPath();
}

bool FolderSets::IsEnabled(int index) const
{
	return m_folderSetsArray[index].IsEnabled();
}

int FolderSets::GetSyncStyle(int index) const
{
	return m_folderSetsArray[index].GetSyncStyle();
}

bool FolderSets::GetDoCRCCheck(int index) const
{
	return m_folderSetsArray[index].GetDoCRCCheck();
}

bool FolderSets::GetUseSyncDates(int index) const
{
	return m_folderSetsArray[index].GetUseSyncDates();
}

bool FolderSets::GetVerifyCopiedFiles(int index) const
{
	return m_folderSetsArray[index].GetVerifyCopiedFiles();
}

bool FolderSets::GetCloseWhenCompleted(int index) const
{
	return m_folderSetsArray[index].GetCloseWhenCompleted();
}

bool FolderSets::GetExitWhenClosed(int index) const
{
	return m_folderSetsArray[index].GetExitWhenClosed();
}

int FolderSets::GetScanThreadPriority(int index) const
{
	return m_folderSetsArray[index].GetScanThreadPriority();
}

int FolderSets::GetSyncThreadPriority(int index) const
{
	return m_folderSetsArray[index].GetSyncThreadPriority();
}

bool FolderSets::IsUsingDefaultAutoFreezeExtensions(int index) const
{
	return m_folderSetsArray[index].IsUsingDefaultAutoFreezeExtensions();
}

const char *FolderSets::GetAutoFreezeFileExtensions(int index) const
{
	return m_folderSetsArray[index].GetAutoFreezeFileExtensions();
}

const char *FolderSets::GetAutoFreezeFilePrefixes(int index) const
{
	return m_folderSetsArray[index].GetAutoFreezeFilePrefixes();
}

const char *FolderSets::GetAutoFreezeFolderPrefixes(int index) const
{
	return m_folderSetsArray[index].GetAutoFreezeFolderPrefixes();
}

// search through the sets and return the index of the first set that matches the name
// if not found returns -1
int FolderSets::FindSetByName(const char *pName) const
{
	int numSets = m_folderSetsArray.GetSize();
	for (int setIndex=0;setIndex<numSets;setIndex++)
	{
		const FolderSet &folderSet = m_folderSetsArray[setIndex];
		if (folderSet.IsEnabled())
		{
			if (!stricmp(pName,folderSet.GetName()))
			{
				return setIndex;
			}
		}
	}
	return -1;
}

int FolderSets::GetNumLeftPreviousPaths(void) const
{
	return m_previousLeftPaths.GetSize();
}

const char *FolderSets::GetLeftPreviousPath(int index) const
{
	return m_previousLeftPaths[index];
}

int FolderSets::GetNumRightPreviousPaths(void) const
{
	return m_previousRightPaths.GetSize();
}

const char *FolderSets::GetRightPreviousPath(int index) const
{
	return m_previousRightPaths[index];
}

CTime FolderSets::GetLastScanTime(int index) const
{
	return m_folderSetsArray[index].GetLastScanTime();
}


void FolderSets::SetName(int index,const char *pName)
{
	m_folderSetsArray[index].SetName(pName);
}

void FolderSets::SetDescription(int index,const char *pDescription)
{
	m_folderSetsArray[index].SetDescription(pDescription);
}

void FolderSets::SetLeftFolderPath(int index,const char *pLeftFolderPath)
{
	m_folderSetsArray[index].SetLeftFolderPath(pLeftFolderPath);
}

void FolderSets::SetRightFolderPath(int index,const char *pRightFolderPath)
{
	m_folderSetsArray[index].SetRightFolderPath(pRightFolderPath);
}

void FolderSets::SetEnabled(int index,bool bActive)
{
	m_folderSetsArray[index].SetEnabled(bActive);
	// change the active set if not enabled any more
	if (!bActive && m_activeSet==index)
	{
		SetActiveSet(index);
	}
}

void FolderSets::SetSyncStyle(int index,int syncStyle)
{
	m_folderSetsArray[index].SetSyncStyle(syncStyle);
}

void FolderSets::SetDoCRCCheck(int index,bool bDoCRCCheck)
{
	m_folderSetsArray[index].SetDoCRCCheck(bDoCRCCheck);
}

void FolderSets::SetUseSyncDates(int index,bool bUseSyncDates)
{
	m_folderSetsArray[index].SetUseSyncDates(bUseSyncDates);
}

void FolderSets::SetVerifyCopiedFiles(int index,bool bVerifyCopiedFiles)
{
	m_folderSetsArray[index].SetVerifyCopiedFiles(bVerifyCopiedFiles);
}

void FolderSets::SetCloseWhenCompleted(int index,bool bCloseWhenCompleted)
{
	m_folderSetsArray[index].SetCloseWhenCompleted(bCloseWhenCompleted);
}

void FolderSets::SetExitWhenClosed(int index,bool bExitWhenClosed)
{
	m_folderSetsArray[index].SetExitWhenClosed(bExitWhenClosed);
}

void FolderSets::SetScanThreadPriority(int index,int priority)
{
	return m_folderSetsArray[index].SetScanThreadPriority(priority);
}

void FolderSets::SetSyncThreadPriority(int index,int priority)
{
	return m_folderSetsArray[index].SetSyncThreadPriority(priority);
}

void FolderSets::SetUsingDefaultAutoFreezeExtensions(int index,bool value)
{
	m_folderSetsArray[index].SetUsingDefaultAutoFreezeExtensions(value);
}

void FolderSets::SetAutoFreezeFileExtensions(int index,const char *pValue)
{
	m_folderSetsArray[index].SetAutoFreezeFileExtensions(pValue);
}

void FolderSets::SetAutoFreezeFilePrefixes(int index,const char *pValue)
{
	m_folderSetsArray[index].SetAutoFreezeFilePrefixes(pValue);
}

void FolderSets::SetAutoFreezeFolderPrefixes(int index,const char *pValue)
{
	m_folderSetsArray[index].SetAutoFreezeFolderPrefixes(pValue);
}

void FolderSets::SetLastScanTime(int index, CTime time)
{
	m_folderSetsArray[index].SetLastScanTime(time);
}

void FolderSets::SetActiveSet(int index)
{
	if (index<0 || index>=m_folderSetsArray.GetSize())
	{
		index = 0;
	}
	if (m_folderSetsArray[index].IsEnabled())
	{
		m_activeSet = index; // the one we want is enabled
	}
	else
	{
		// find one that is enabled starting with the next one
		int findIndex;
		for (findIndex=index+1;findIndex<m_folderSetsArray.GetSize();findIndex++)
		{
			if (m_folderSetsArray[findIndex].IsEnabled())
			{
				m_activeSet = findIndex;
				return;
			}
		}
		// find one that is enabled starting with the previous one
		for (findIndex=index-1;findIndex>=0;findIndex--)
		{
			if (m_folderSetsArray[findIndex].IsEnabled())
			{
				m_activeSet = findIndex;
				return;
			}
		}
		m_activeSet = -1; // none are enabled 
	}
}

int FolderSets::CreateNew(void)
{
	int totalSets = m_folderSetsArray.GetSize();
	// if we find one that is not active return it
	for (int folderSetIndex = 0;folderSetIndex<totalSets;folderSetIndex++)
	{
		FolderSet &folderSet = m_folderSetsArray[folderSetIndex];
		if (!folderSet.IsEnabled())
		{
			folderSet.Init();
			return folderSetIndex;
		}
	}
	FolderSet newFolderSet;
	m_folderSetsArray.Add(newFolderSet);
	return m_folderSetsArray.GetSize()-1;
}

void FolderSets::AddLeftPreviousPath(const char *pPath,int maxEntries)
{
	int numPaths = m_previousLeftPaths.GetSize();
	// remove all paths that match this one
	for (int pathIndex=numPaths-1;pathIndex>=0;pathIndex--)
	{
		if (!stricmp(pPath,m_previousLeftPaths[pathIndex]))
		{
			m_previousLeftPaths.RemoveAt(pathIndex);
		}
	}
	TupString pathToAdd(pPath);
	m_previousLeftPaths.InsertAt(0,pathToAdd);

	// make sure there are not more than maxEntries
	while (m_previousLeftPaths.GetSize()>maxEntries)
	{
		m_previousLeftPaths.RemoveAt(m_previousLeftPaths.GetSize()-1);
	}
}

void FolderSets::AddRightPreviousPath(const char *pPath,int maxEntries)
{
	int numPaths = m_previousRightPaths.GetSize();
	// remove all paths that match this one
	for (int pathIndex=numPaths-1;pathIndex>=0;pathIndex--)
	{
		if (!stricmp(pPath,m_previousRightPaths[pathIndex]))
		{
			m_previousRightPaths.RemoveAt(pathIndex);
		}
	}
	TupString pathToAdd(pPath);
	m_previousRightPaths.InsertAt(0,pathToAdd);

	// make sure there are not more than maxEntries
	while (m_previousRightPaths.GetSize()>maxEntries)
	{
		m_previousRightPaths.RemoveAt(m_previousRightPaths.GetSize()-1);
	}
}

