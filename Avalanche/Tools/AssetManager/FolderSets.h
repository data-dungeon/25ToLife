////////////////////////////////////////////////////////////////////////////
//
// FolderSets
//
// Class for managing the folder sets
//
////////////////////////////////////////////////////////////////////////////

#ifndef ASSET_MANAGER_FOLDER_SETS_H
#define ASSET_MANAGER_FOLDER_SETS_H

#ifndef ASSET_MANAGER_FOLDER_SET_H
#include "FolderSet.h"
#endif

class FolderSets
{
	TupArray<FolderSet> m_folderSetsArray;
	TupArray<TupString> m_previousLeftPaths;
	TupArray<TupString> m_previousRightPaths;
	int m_activeSet;

public:
	FolderSets(void);

	void LoadFromRegistry(const char *pFolderSetToLoad);
	void SaveToRegistry(void) const;

	int GetNumFolderSets(void) const;
	int GetActiveSet(void) const;

	const char *GetName(int index) const;
	const char *GetDescription(int index) const;
	const char *GetLeftFolderPath(int index) const;
	const char *GetRightFolderPath(int index) const;
	bool IsEnabled(int index) const;
	int FindSetByName(const char *pName) const;
	int GetSyncStyle(int index) const;
	bool GetDoCRCCheck(int index) const;
	bool GetUseSyncDates(int index) const;
	bool GetVerifyCopiedFiles(int index) const;
	bool GetCloseWhenCompleted(int index) const;
	bool GetExitWhenClosed(int index) const;
	int GetScanThreadPriority(int index) const;
	int GetSyncThreadPriority(int index) const;
	bool IsUsingDefaultAutoFreezeExtensions(int index) const;
	const char *GetAutoFreezeFileExtensions(int index) const;
	const char *GetAutoFreezeFilePrefixes(int index) const;
	const char *GetAutoFreezeFolderPrefixes(int index) const;
	CTime GetLastScanTime(int index) const;


	int GetNumLeftPreviousPaths(void) const;
	const char *GetLeftPreviousPath(int index) const;
	int GetNumRightPreviousPaths(void) const;
	const char *GetRightPreviousPath(int index) const;

	void SetName(int index,const char *pName);
	void SetDescription(int index,const char *pDescription);
	void SetLeftFolderPath(int index,const char *pLeftFolderPath);
	void SetRightFolderPath(int index,const char *pRightFolderPath);
	void SetEnabled(int index,bool bEnabled);
	void SetSyncStyle(int index,int syncStyle);
	void SetDoCRCCheck(int index,bool bDoCRCCheck);
	void SetUseSyncDates(int index,bool bUseSyncDates);
	void SetVerifyCopiedFiles(int index,bool bVerifyCopiedFiles);
	void SetCloseWhenCompleted(int index,bool bCloseWhenCompleted);
	void SetExitWhenClosed(int index,bool bExitWhenClosed);
	void SetScanThreadPriority(int index,int priority);
	void SetSyncThreadPriority(int index,int priority);
	void SetUsingDefaultAutoFreezeExtensions(int index,bool value);
	void SetAutoFreezeFileExtensions(int index,const char *pValue);
	void SetAutoFreezeFilePrefixes(int index,const char *pValue);
	void SetAutoFreezeFolderPrefixes(int index,const char *pValue);
	void SetLastScanTime(int index,CTime time);

	void SetActiveSet(int index);
	int CreateNew(void);

	void AddLeftPreviousPath(const char *pPath,int maxEntries);
	void AddRightPreviousPath(const char *pPath,int maxEntries);
	

};

#endif
