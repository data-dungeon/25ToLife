//-----------------------------------------------------------------------------
//	File:		AssetManagerCmd.cpp
//	Created:	Sep 13, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CAssetManagerCmdApp class defines behavior for the application as
//			a whole.
//
//	History:	9/13/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AssetManagerCmd.h"
#include "Registry.h"
#include "AMSyncTreeFactory.h"
#include "AMSyncTreeNodeFolder.h"
#include "AMSyncTreeNodeFile.h"
#include "RegistryKeys.h"
#include "FolderSets.h"
#include "AMLoadDBFeedbackCmd.h"
#include "AMSyncFeedbackCmd.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int main( int argc, char** argv )
{
	CAssetManagerCmdApp *pApp = new CAssetManagerCmdApp;
	int result = pApp->InitInstance(argc,argv);
	delete pApp;
	return result;
}


//-----------------------------------------------------------------------------
// CAssetManagerCmdApp::CAssetManagerCmdApp
// This method is the default constructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
CAssetManagerCmdApp::CAssetManagerCmdApp() 
{
	// Create the factory for creating the databases:
	m_pDBFactory = new SyncDBFactory;

	// Create a sync tree factory and a sync tree manager to tie the databases together:
	m_pAMSyncTreeFactory = new AMSyncTreeFactory;
	m_pSyncTreeManager = new SyncTreeManager(*m_pAMSyncTreeFactory);
	m_pSyncTreeManager->SetAutoExclude(&m_autoExclude);

	// Set the databases to null here:
	m_pLeftDataBase = NULL;			
	m_pRightDataBase = NULL;			

	m_pFolderSets = NULL;
	m_pLogFile = NULL;

}

CAssetManagerCmdApp::~CAssetManagerCmdApp() 
{
	if ((m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_PREVIEW_MODE)==0)
	{
		SaveClientData();
	}

	if (m_pDBFactory != NULL)
	{
		delete m_pDBFactory;
		m_pDBFactory = NULL;
	}
	if (m_pAMSyncTreeFactory != NULL)
	{
		delete m_pAMSyncTreeFactory;
		m_pAMSyncTreeFactory = NULL;
	}
	if (m_pSyncTreeManager != NULL)
	{
		delete m_pSyncTreeManager;
		m_pSyncTreeManager = NULL;
	}
	if (m_pLeftDataBase != NULL)
	{
		delete m_pLeftDataBase;
		m_pLeftDataBase = NULL;
	}
	if (m_pRightDataBase != NULL)
	{
		delete m_pRightDataBase;
		m_pRightDataBase = NULL;
	}
	if (m_pFolderSets != NULL)
	{
		delete m_pFolderSets;
		m_pFolderSets = NULL;
	}
	if (m_pLogFile != NULL)
	{
		delete m_pLogFile;
		m_pLogFile = NULL;
	}

}


//-----------------------------------------------------------------------------
// CAssetManagerCmdApp::InitInstance
// This method will handle initialization of any instances of the app.
//
// Input: None.
//
// Output: TRUE if initialization was successful, FALSE otherwise.
//-----------------------------------------------------------------------------
BOOL CAssetManagerCmdApp::InitInstance( int argc, char** argv )
{
	printf("AssetManagerCmd %s %s %s\n",g_pAssetManagerVersion,g_pAssetManagerDate,g_pAssetManagerTime);

	CheckCommandLine(argc,argv);
	if ((m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_FOLDER_SET)==0)
	{
		printf("Usage: AssetManagerCmd foldersetname [options]\n");
		printf("  Options:\n");
		printf("   /CR - Enable Copy to Right\n");
		printf("   /CL - Enable Copy to Left\n");
		printf("   /DR - Enable Delete from Right\n");
		printf("   /DL - Enable Delete from Left\n");
		printf("   /P  - Enable Preview Mode\n");
		printf("   /S \"Sync Style\" - Override sync style\n");
		printf("	     Sync Styles:\n");
		printf("	       \"Normal Sync\"\n");
		printf("	       \"Augment Left\"\n");
		printf("	       \"Augment Right\"\n");
		printf("	       \"Refresh Left\"\n");
		printf("	       \"Refresh Right\"\n");
		printf("	       \"Update Left\"\n");
		printf("	       \"Update Right\"\n");
		printf("	       \"Update and Prune Left\"\n");
		printf("	       \"Update and Prune Right\"\n");
		printf("	       \"Force Left\"\n");
		printf("	       \"Force Right\"\n");
		return TRUE;
	}

	if (DoSync())
		return FALSE;
	else
		return TRUE;
}


void CAssetManagerCmdApp::CheckCommandLine(int argc, char** argv)
{
	m_commandLineOptions.m_flags = 0;

	if (argc>1)
	{
		m_commandLineOptions.m_folderSetName = argv[1];
		m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_FOLDER_SET;
	}
	for (int i = 2; i < argc; i++)
	{
		TupString optionString(argv[i]);
		if (!stricmp(optionString,"/S"))
		{
			i++;
			if (i<__argc)
			{
				if (!stricmp(__argv[i],"Normal Sync"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_2_WAY;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Augment Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_AUGMENT_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Augment Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_AUGMENT_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Refresh Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_REFRESH_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Refresh Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_REFRESH_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update and Prune Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_AND_PRUNE_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Update and Prune Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Force Left"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_FORCE_LEFT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else if (!stricmp(__argv[i],"Force Right"))
				{
					m_commandLineOptions.m_syncStyle = SyncTreeManager::SYNC_STYLE_FORCE_RIGHT;
					m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_SYNC_STYLE;
				}
				else
				{
					printf("Sync Style Override %s not known sync style\n",__argv[i]);
				}
			}
		}
		else if (!stricmp(optionString,"/CR"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_COPY_TO_RIGHT;
		}
		else if (!stricmp(optionString,"/CL"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_COPY_TO_LEFT;
		}
		else if (!stricmp(optionString,"/DR"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_DELETE_FROM_RIGHT;
		}
		else if (!stricmp(optionString,"/DL"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_SET_DELETE_FROM_LEFT;
		}
		else if (!stricmp(optionString,"/P"))
		{
			m_commandLineOptions.m_flags |= CommandLineOptions::FLAGS_PREVIEW_MODE;
		}
	}
}

void CAssetManagerCmdApp::GetRegistrySettings(void)
{
	Registry registry(ASSET_MANAGER_KEY);
	DWORD regInt;

	char excludeBuffer[512];
	TupString autoExcludeString;

	if (registry.Get(AUTO_EXCLUDE_FILE_EXTENSIONS_FIELD,&excludeBuffer,512))
	{
		m_defaultAutoFreezeFileExtensions = excludeBuffer;
	}
	else
	{
		m_defaultAutoFreezeFileExtensions = ".tmp;.bak;.pix;.ava;.attr;.iff;.log;.mel";
	}
	if (registry.Get(AUTO_EXCLUDE_FILE_PREFIXES_FIELD,&excludeBuffer,512))
	{
		m_defaultAutoFreezeFilePrefixes = excludeBuffer;
	}
	else
	{
		m_defaultAutoFreezeFilePrefixes = "_;buttmap;devicemap";
	}
	if (registry.Get(AUTO_EXCLUDE_FOLDER_PREFIXES_FIELD,&excludeBuffer,512))
	{
		m_defaultAutoFreezeFolderPrefixes = excludeBuffer;
	}
	else
	{
		m_defaultAutoFreezeFolderPrefixes = "_;.mayaSwatches;cache;Latest";
	}

	if (registry.Get(UPDATE_DUPLICATES_INCLUDE_FROZEN_DUPLICATES,&regInt))
	{
		m_updateDuplicatesIncludeFrozenDuplicates = regInt;
	}
	else
	{
		m_updateDuplicatesIncludeFrozenDuplicates = TRUE;
	}

	if (registry.Get(UPDATE_DUPLICATES_INCLUDE_PARENT_NAMES,&regInt))
	{
		m_updateDuplicatesIncludeParentNames = regInt;
	}
	else
	{
		m_updateDuplicatesIncludeParentNames = TRUE;
	}

	m_viewFlags = SyncTreeNodeFlags::VIEW_FLAGS_ALL|
		SyncTreeNodeFlags::VIEW_FLAGS_STATUS_ACTION|
		SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN|
		SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE;

	if (registry.Get(EXCLUDE_LOCK_FLAG_FIELD,&regInt) && (regInt==1))
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_EXCLUDE_LOCKED;
		if (registry.Get(VIEW_EXCLUDE_FLAG_FIELD,&regInt) && (regInt==0))
		{
			m_viewFlags &= ~SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE; // clear if set to 0
		}
	}
	if (registry.Get(FROZEN_LOCK_FLAG_FIELD,&regInt) && (regInt==1))
	{
		m_viewFlags |= SyncTreeNodeFlags::VIEW_FLAGS_FROZEN_LOCKED;
		if (registry.Get(VIEW_FROZEN_FLAG_FIELD,&regInt) && (regInt==0))
		{
			m_viewFlags &= ~SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN; // clear if set to 0
		}
	}

}

//-----------------------------------------------------------------------------
// CAssetManagerCmdApp::SetUpFolderSets
// This method is called to set up and load the left and right combo boxes.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
bool CAssetManagerCmdApp::SetUpFolderSets(void)
{
	m_pFolderSets = new FolderSets;
	m_pFolderSets->LoadFromRegistry(m_commandLineOptions.m_folderSetName);

	int activeSet = m_pFolderSets->GetActiveSet();
	int numSets = m_pFolderSets->GetNumFolderSets();

	bool bFolderSetOk = true;

	// if we have a good set
	if (activeSet!=-1)
	{
		// make sure the folder set we requested is found
		const char *pActiveFolderSetName = m_pFolderSets->GetName(activeSet);
		if (stricmp(pActiveFolderSetName,m_commandLineOptions.m_folderSetName))
		{
			printf("Error Folder Set %s not defined\n",(const char *)m_commandLineOptions.m_folderSetName);
			bFolderSetOk = false;
		}

		const char *pLeftFolderPath = m_pFolderSets->GetLeftFolderPath(activeSet);
		if (!strlen(pLeftFolderPath) || !::PathFileExists((LPCTSTR) pLeftFolderPath))
		{
			printf("Error Left Path %s does not exist\n",pLeftFolderPath);
			bFolderSetOk = false;
		}
		const char *pRightFolderPath = m_pFolderSets->GetRightFolderPath(activeSet);
		if (!strlen(pRightFolderPath) || !::PathFileExists((LPCTSTR) pRightFolderPath))
		{
			printf("Error Right Path %s does not exist\n",pRightFolderPath);
			bFolderSetOk = false;
		}
	}
	else
	{
		printf("Error Folder Set %s not defined\n",(const char *)m_commandLineOptions.m_folderSetName);
		bFolderSetOk = false;
	}

	return bFolderSetOk;
}

//-----------------------------------------------------------------------------
// CAssetManagerCmdApp::LoadDatabases
// This method can be called to load the left and right databases as well as set up the tree
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
void CAssetManagerCmdApp::LoadDatabases(void)
{
	// the feedback object is used to pass information to the sync dialog
	// the other thread is sitting in the domodal loop ready to handle any ui messages for the load db dialog
	AMLoadDBFeedbackCmd loadFeedback;

	int activeSet = m_pFolderSets->GetActiveSet();
	printf("Using Folder Set %s\n",m_pFolderSets->GetName(activeSet));

	if (!m_pLeftDataBase)
	{
		const char *pLeftFolderPath = m_pFolderSets->GetLeftFolderPath(activeSet);
		if (strlen(pLeftFolderPath))
		{
			printf("Loading Left Database\n");
			m_pLeftDataBase = m_pDBFactory->CreateDB(pLeftFolderPath,loadFeedback,*m_pLogFile);
			if (m_pLeftDataBase->IsRemoteDB())
			{
				printf("Left Database is Remote\n");
			}
			if (!m_pLeftDataBase->GetTreeRoot())
			{
				delete m_pLeftDataBase;
				m_pLeftDataBase = NULL;
			}
		}
	}
	if (!m_pRightDataBase)
	{
		const char *pRightFolderPath = m_pFolderSets->GetRightFolderPath(activeSet);
		if (strlen(pRightFolderPath))
		{
			printf("Loading Right Database\n");
			m_pRightDataBase = m_pDBFactory->CreateDB(pRightFolderPath,loadFeedback,*m_pLogFile);
			if (m_pRightDataBase->IsRemoteDB())
			{
				printf("Right Database is Remote\n");
			}
			if (!m_pRightDataBase->GetTreeRoot())
			{
				delete m_pRightDataBase;
				m_pRightDataBase = NULL;
			}
		}
	}
	bool bUseDefaultAutoExclude = m_pFolderSets->IsUsingDefaultAutoFreezeExtensions(activeSet);
	if (bUseDefaultAutoExclude)
	{
		m_autoExclude.SetFileExtensions(m_defaultAutoFreezeFileExtensions);
		m_autoExclude.SetFilePrefixes(m_defaultAutoFreezeFilePrefixes);
		m_autoExclude.SetFolderPrefixes(m_defaultAutoFreezeFolderPrefixes);
	}
	else
	{
		m_autoExclude.SetFileExtensions(m_pFolderSets->GetAutoFreezeFileExtensions(activeSet));
		m_autoExclude.SetFilePrefixes(m_pFolderSets->GetAutoFreezeFilePrefixes(activeSet));
		m_autoExclude.SetFolderPrefixes(m_pFolderSets->GetAutoFreezeFolderPrefixes(activeSet));
	}

	int syncStyle = m_pFolderSets->GetSyncStyle(activeSet);
	m_pSyncTreeManager->SetSyncStyle((SyncTreeManager::SyncStyles) syncStyle);

	bool bDoCRCCheck = m_pFolderSets->GetDoCRCCheck(activeSet);
	m_pSyncTreeManager->SetCheckingCRC(bDoCRCCheck);

	bool bUseSyncDates = m_pFolderSets->GetUseSyncDates(activeSet);
	m_pSyncTreeManager->SetUsingSyncDates(bUseSyncDates);

	bool bVerifyFileCopies = m_pFolderSets->GetVerifyCopiedFiles(activeSet);
	m_pSyncTreeManager->SetVerifyCopiedFiles(bVerifyFileCopies);

	if (bDoCRCCheck)
		printf("Building Sync Tree With CRC\n");
	else
		printf("Building Sync Tree\n");

	if (bUseSyncDates)
		printf("Update Copied File Time to Time of Sync\n");

	if (bVerifyFileCopies)
		printf("Verifying Copied Files\n");

	// get the filename for the client database
	TupString clientDataFilePath = GetClientDataFilePath(activeSet);

	ClientDataManager *pClientDataManager = NULL;
	// if it is a valid path
	if (clientDataFilePath.GetLength())
	{
		pClientDataManager = new ClientDataManager;
		pClientDataManager->LoadData(clientDataFilePath);

		// pull over the autofrozen path list regardless of the folder paths
		int numPaths = pClientDataManager->GetNumAutoFrozenPaths();
		m_autoExclude.RemovePaths();
		for (int pathIndex=0;pathIndex<numPaths;pathIndex++)
		{
			TupString path;
			pClientDataManager->GetAutoFrozenPath(path,pathIndex);
			m_autoExclude.AddPath(path);
		}

		// if this database is not pointing to the same folders then we kill it
		if (stricmp(pClientDataManager->GetLeftFolderPath(),m_pFolderSets->GetLeftFolderPath(activeSet))!=0 ||
			stricmp(pClientDataManager->GetRightFolderPath(),m_pFolderSets->GetRightFolderPath(activeSet))!=0)
		{
			delete pClientDataManager;
			pClientDataManager = NULL;
		}
	}

	m_pSyncTreeManager->SetDatabases(m_pLeftDataBase,m_pRightDataBase,pClientDataManager,loadFeedback);
	// done with this
	if (pClientDataManager)
	{
		delete pClientDataManager;
		pClientDataManager = NULL;
	}
	UpdateSyncStats();
}

const char *CAssetManagerCmdApp::GetClientDataFilePath(int setIndex)
{
	static TupString pathString = "";

	TCHAR path[MAX_PATH];
	HRESULT hResult = SHGetFolderPath(NULL,                //hWnd	
		CSIDL_APPDATA | CSIDL_FLAG_CREATE,       //csidl
		NULL,                //hToken
		SHGFP_TYPE_CURRENT,  //dwFlags
		path);             //pszPath
	if (SUCCEEDED(hResult))
	{
		PathAppend(path, FOLDER_SETS_APP_DATA_FOLDER);
	}
	else
	{
		strcpy(path,"c:\\"); // just use c:
		PathAppend(path, FOLDER_SETS_APP_DATA_FOLDER);
	}

	PathAppend(path, TEXT("FolderSet"));
	char numString[10];
	itoa(setIndex,numString,10);
	pathString = path;
	pathString += numString;
	pathString += ".tup";

	CreateDirs(pathString,TRUE);

	return pathString;
}

bool CAssetManagerCmdApp::DirectoryExists(const char *pPath)
{
	DWORD fileAttributes = ::GetFileAttributes(pPath);
	return (fileAttributes != INVALID_FILE_ATTRIBUTES);
}

//
//  CreateDirs()
//  DESC: Creates each directory in a given path. 
//  ARGS: sPath - The fully qualified path whose directories we will create.
//           As each portion of the the path is parsed out, if it doesn't
//           exists, it will be created by CreateDirectory().  
//        bIsFilePath - If TRUE, CreateDirs() assumes the last token in the
//           path is a filename, and skips it.
bool CAssetManagerCmdApp::CreateDirs (const char *pPath, bool bIsFilePath /*= false*/)
{
	char outpath[MAX_PATH] = "";      // we build this during the parsing of the source path
	char inpath[MAX_PATH];           // create local copy of source path (we destructively parse it)  
	char *p = strcpy (inpath, pPath); 
	char *slash = NULL;

	// store the drive portion of path and skip past it
	if (!((slash = strchr (p, '\\')) || (slash = strchr (p, '/'))))
		return false;
	*slash = 0;           // terminate p at first slash so we can...
	strcpy (outpath, p);  // copy drive letter and : to outpath
	p = slash + 1;        // advance p to next token in path

	ASSERT(outpath[1] == ':'); // so far, so good

	// parse out slash delimited tokens, 
	// creating each part of the directory path as we go
	do 
	{
		if ((slash = strchr (p, '\\')) || (slash = strchr (p, '/')))     
			*slash = 0;                         // terminate so we can call strcat() with p
		else if (bIsFilePath)                   // skip last token if the path is to a file
			return true;
		strcat (outpath, "\\");                 // add token to the output path path...
		strcat (outpath, p);                    // ...
		if (!DirectoryExists (outpath) &&       // if path doesn't already exists
			!CreateDirectory (outpath, NULL))   // try to create it
			return false;                       // something went wrong
		p = slash + 1;                          // advance p to next token in path
	}
	while (slash);

	return true;
}


void CAssetManagerCmdApp::SaveClientData(void)
{
	HANDLE hSyncClientDatabasesMutex = CreateMutex(NULL,FALSE,"AssetManagerClientDatabaseMutex");

	DWORD waitResult = WaitForSingleObject(hSyncClientDatabasesMutex,INFINITE);
	if (waitResult != WAIT_OBJECT_0)
	{
		printf("Error saving client data\n");
		return;
	}

	if (m_pFolderSets)
	{
		int activeSet = m_pFolderSets->GetActiveSet();
		if (activeSet!=-1 && m_pSyncTreeManager)
		{
			// get the filename for the client database
			TupString clientDataFilePath = GetClientDataFilePath(activeSet);

			// if it is a valid path
			if (clientDataFilePath.GetLength())
			{
				ClientDataManager pClientDataManager;
				pClientDataManager.BuildTree(m_pSyncTreeManager->GetSyncTree());
				pClientDataManager.SetLeftFolderPath(m_pFolderSets->GetLeftFolderPath(activeSet));
				pClientDataManager.SetRightFolderPath(m_pFolderSets->GetRightFolderPath(activeSet));
				int numAutoFrozenPaths = m_autoExclude.GetNumPaths();
				for (int pathIndex=0;pathIndex<numAutoFrozenPaths;pathIndex++)
				{
					TupString path;
					m_autoExclude.GetPath(path,pathIndex);
					pClientDataManager.AddAutoFrozenPath(path);
				}
				pClientDataManager.SaveData(clientDataFilePath);
			}
		}
	}
	ReleaseMutex(hSyncClientDatabasesMutex);
	CloseHandle(hSyncClientDatabasesMutex);
}


//-----------------------------------------------------------------------------
// CAssetManagerCmdApp::SyncDatabases
// This method can be called sync the databases
//
//-----------------------------------------------------------------------------
bool CAssetManagerCmdApp::SyncDatabases(void)
{
	ASSERT(m_pLeftDataBase && m_pRightDataBase);

	// make sure the the sync tree is present
	ASSERT(m_pSyncTreeManager->GetSyncTree());

	AMSyncFeedbackCmd syncFeedback;

	if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_PREVIEW_MODE)
	{
		syncFeedback.m_bPreviewMode = true;
		printf("Preview Mode Enabled\n");
	}
	syncFeedback.m_bIncludePath = true;

	// to the sync - pass the feedback object so the dialog can be updated
	m_pSyncTreeManager->Synchronize(syncFeedback,m_syncEnable,m_viewFlags);

	return (syncFeedback.m_errorCount==0); // return true if no errors
}

void CAssetManagerCmdApp::UpdateSyncStats(void)
{
	// gets information from the tree as well as sets the visible flags
	m_pSyncTreeManager->GetTreeStats(m_syncStats,m_viewFlags);

	printf("Left Database Files: %d  Folders: %d  Total Bytes: %I64d\n",m_syncStats.m_filesLeft,m_syncStats.m_foldersLeft,m_syncStats.m_totalBytesLeft);
	printf("Right Database Files: %d  Folders: %d  Total Bytes: %I64d\n",m_syncStats.m_filesRight,m_syncStats.m_foldersRight,m_syncStats.m_totalBytesRight);

	printf("Files/Folders To Copy Left->Right: %d (%I64d Bytes)\n",m_syncStats.m_filesFoldersCopiedLeftRight,m_syncStats.m_totalBytesCopiedLeftRight);
	printf("Files/Folders To Copy Right->Left: %d (%I64d Bytes)\n",m_syncStats.m_filesFoldersCopiedRightLeft,m_syncStats.m_totalBytesCopiedRightLeft);
	printf("Files/Folders To Delete from Left: %d\n",m_syncStats.m_filesFoldersDeletedLeft);
	printf("Files/Folders To Delete from Right: %d\n",m_syncStats.m_filesFoldersDeletedRight);
}



bool CAssetManagerCmdApp::DoSync(void)
{
	GetRegistrySettings();

	char nameBuffer[512];
	DWORD maxSize = 512;
	GetUserName(nameBuffer,&maxSize);
	m_userName = nameBuffer;
	GetComputerName(nameBuffer,&maxSize);
	m_computerName = nameBuffer;
	m_pLogFile = new LogFile("AssetManager.log",m_userName,m_computerName);

	// Set up the folder sets
	if (SetUpFolderSets())
	{
		int activeSet = m_pFolderSets->GetActiveSet();
		if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_SYNC_STYLE)
		{
			m_pFolderSets->SetSyncStyle(activeSet,m_commandLineOptions.m_syncStyle);
		}

		switch (m_pFolderSets->GetSyncStyle(activeSet))
		{
			case SyncTreeManager::SYNC_STYLE_2_WAY:
				printf("Using Sync Style: Normal Sync\n");
				break;
			case SyncTreeManager::SYNC_STYLE_AUGMENT_LEFT:
				printf("Using Sync Style: Augment Left\n");
				break;
			case SyncTreeManager::SYNC_STYLE_AUGMENT_RIGHT:
				printf("Using Sync Style: Augment Right\n");
				break;
			case SyncTreeManager::SYNC_STYLE_REFRESH_LEFT:
				printf("Using Sync Style: Refresh Left\n");
				break;
			case SyncTreeManager::SYNC_STYLE_REFRESH_RIGHT:
				printf("Using Sync Style: Refresh Right\n");
				break;
			case SyncTreeManager::SYNC_STYLE_UPDATE_LEFT:
				printf("Using Sync Style: Update Left\n");
				break;
			case SyncTreeManager::SYNC_STYLE_UPDATE_RIGHT:
				printf("Using Sync Style: Update Right\n");
				break;
			case SyncTreeManager::SYNC_STYLE_UPDATE_AND_PRUNE_LEFT:
				printf("Using Sync Style: Update and Prune Left\n");
				break;
			case SyncTreeManager::SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT:
				printf("Using Sync Style: Update and Prune Right\n");
				break;
			case SyncTreeManager::SYNC_STYLE_FORCE_LEFT:
				printf("Using Sync Style: Force Left\n");
				break;
			case SyncTreeManager::SYNC_STYLE_FORCE_RIGHT:
				printf("Using Sync Style: Force Right\n");
				break;
		}

		// Load the databases:
		LoadDatabases();
		if (!m_pLeftDataBase || !m_pRightDataBase)
		{
			printf("Error scanning folders\n");
			return false;
		}

		if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_COPY_TO_LEFT)
		{
			printf("Copy Right->Left Enabled\n");
			m_syncEnable.m_bRLCopyEnable = true;
		}
		if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_COPY_TO_RIGHT)
		{
			printf("Copy Left->Right Enabled\n");
			m_syncEnable.m_bLRCopyEnable = true;
		}
		if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_DELETE_FROM_LEFT)
		{
			printf("Delete from Left Enabled\n");
			m_syncEnable.m_bLDeleteEnable = true;
		}
		if (m_commandLineOptions.m_flags & CommandLineOptions::FLAGS_SET_DELETE_FROM_RIGHT)
		{
			printf("Delete from Right Enabled\n");
			m_syncEnable.m_bRDeleteEnable = true;
		}

		if ((m_syncStats.m_filesFoldersCopiedLeftRight && m_syncEnable.m_bLRCopyEnable) ||
			(m_syncStats.m_filesFoldersCopiedRightLeft && m_syncEnable.m_bRLCopyEnable) ||
			(m_syncStats.m_filesFoldersDeletedLeft && m_syncEnable.m_bLDeleteEnable) ||
			(m_syncStats.m_filesFoldersDeletedRight && m_syncEnable.m_bRDeleteEnable))
		{
			return SyncDatabases();
		}
		return true; // nothing to sync
	}
	
	return false;
}