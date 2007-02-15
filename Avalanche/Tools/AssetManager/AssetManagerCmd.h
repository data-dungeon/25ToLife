//-----------------------------------------------------------------------------
//	File:		AssetManager.h
//	Created:	Sep 13, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CAssetManagerApp class defines behavior for the application as
//			a whole.
//
//	History:	9/13/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

#ifndef __ASSETMANAGERCMD__
#define __ASSETMANAGERCMD__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//-----------------------------------------------------------------------------
// Includes:

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

class AMSyncTreeFactory;
class FolderSets;
class AMSyncTreeNodeFolder;
class AMSyncTreeNodeFile;

//-----------------------------------------------------------------------------
// CAssetManagerApp class declaration:

class CAssetManagerCmdApp
{
	class CommandLineOptions
	{
	public:
		enum Flags
		{
			FLAGS_SET_FOLDER_SET					= 0x0001,
			FLAGS_SET_SYNC_STYLE					= 0x0002,
			FLAGS_SET_COPY_TO_RIGHT				= 0x0004,
			FLAGS_SET_COPY_TO_LEFT				= 0x0008,
			FLAGS_SET_DELETE_FROM_LEFT			= 0x0010,
			FLAGS_SET_DELETE_FROM_RIGHT		= 0x0020,
			FLAGS_PREVIEW_MODE					= 0x0040,
		};
		int m_flags;
		TupString m_folderSetName;
		int m_syncStyle;
	};
	CommandLineOptions m_commandLineOptions;
	HANDLE m_hSyncClientDatabasesMutex;
public:
	// Constructors and destructors:
	CAssetManagerCmdApp();
	~CAssetManagerCmdApp();

public:
	BOOL InitInstance(int argc, char** argv);

private:
	void CheckCommandLine(int argc, char** argv);
	void GetRegistrySettings(void);
	bool SetUpFolderSets(void);
	void LoadDatabases(void);
	const char *GetClientDataFilePath(int setIndex);
	bool DirectoryExists(const char *pPath);
	bool CreateDirs (const char *pPath, bool bIsFilePath = false);
	void SaveClientData(void);
	bool SyncDatabases(void);
	void UpdateSyncStats(void);
	bool DoSync(void);

	SyncDBFactory* m_pDBFactory;								// The database factory
	AMSyncTreeFactory* m_pAMSyncTreeFactory;				// The sync tree factory
	SyncTreeManager* m_pSyncTreeManager;					// The sync tree manager
	SyncDBBase* m_pLeftDataBase;								// The database for the left tree
	SyncDBBase* m_pRightDataBase;								// The database for the right tree

	AutoExclude m_autoExclude;

	BOOL m_updateDuplicatesIncludeFrozenDuplicates;
	BOOL m_updateDuplicatesIncludeParentNames;

	SyncTreeManager::TreeStats m_syncStats;				// information concerning sync
	SyncTreeManager::SyncEnable m_syncEnable;				// what is enabled with the sync

	TupString m_folderSetToLoad;

	FolderSets *m_pFolderSets;

	TupString m_defaultAutoFreezeFileExtensions;
	TupString m_defaultAutoFreezeFilePrefixes;
	TupString m_defaultAutoFreezeFolderPrefixes;

	TupString m_userName;
	TupString m_computerName;

	LogFile *m_pLogFile;

	unsigned long m_viewFlags;
};

extern CAssetManagerCmdApp theApp;

//-----------------------------------------------------------------------------

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif
