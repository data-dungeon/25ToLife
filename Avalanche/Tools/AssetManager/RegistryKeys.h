////////////////////////////////////////////////////////////////////////////
//
// RegistryKeys
//
// File that maintains the registry keys used
//
////////////////////////////////////////////////////////////////////////////

#ifndef ASSET_MANAGER_REGISTRY_KEYS_H
#define ASSET_MANAGER_REGISTRY_KEYS_H

#define FOLDER_SETS_APP_DATA_FOLDER		(TEXT("Avalanche Software\\AssetManager\\FolderSets"))


// Keys
#define ASSET_MANAGER_KEY				(TEXT("Software\\Avalanche Software\\AssetManager"))
#define LEFT_FOLDERS_KEY				(TEXT("Software\\Avalanche Software\\AssetManager\\LeftFolders"))
#define RIGHT_FOLDERS_KEY				(TEXT("Software\\Avalanche Software\\AssetManager\\RightFolders"))
#define FOLDER_SETS_KEY					(TEXT("Software\\Avalanche Software\\AssetManager\\FolderSets"))

// Fields
#define EXCLUDE_LOCK_FLAG_FIELD					(TEXT("ExcludeLockFlag"))
#define VIEW_EXCLUDE_FLAG_FIELD					(TEXT("ViewExcludeFlag"))
#define FROZEN_LOCK_FLAG_FIELD					(TEXT("FrozenLockFlag"))
#define VIEW_FROZEN_FLAG_FIELD					(TEXT("ViewFrozenFlag"))

#define SORT_FOLDER_SETS_FIELD					(TEXT("SortFolderSets"))

#define DIALOG_RECT_TOP_FIELD						(TEXT("DialogRectTop"))
#define DIALOG_RECT_BOTTOM_FIELD					(TEXT("DialogRectBottom"))
#define DIALOG_RECT_LEFT_FIELD					(TEXT("DialogRectLeft"))
#define DIALOG_RECT_RIGHT_FIELD					(TEXT("DialogRectRight"))

#define UPDATE_DUPLICATES_DIALOG_RECT_TOP_FIELD						(TEXT("UpdateDuplicatesDialogRectTop"))
#define UPDATE_DUPLICATES_DIALOG_RECT_BOTTOM_FIELD					(TEXT("UpdateDuplicatesDialogRectBottom"))
#define UPDATE_DUPLICATES_DIALOG_RECT_LEFT_FIELD					(TEXT("UpdateDuplicatesDialogRectLeft"))
#define UPDATE_DUPLICATES_DIALOG_RECT_RIGHT_FIELD					(TEXT("UpdateDuplicatesDialogRectRight"))

#define AUTO_EXCLUDE_FILE_EXTENSIONS_FIELD		(TEXT("AutoExcludeFileExtensions"))
#define AUTO_EXCLUDE_FILE_PREFIXES_FIELD			(TEXT("AutoExcludeFilePrefixes"))
#define AUTO_EXCLUDE_FOLDER_PREFIXES_FIELD		(TEXT("AutoExcludeFolderPrefixes"))

#define UPDATE_DUPLICATES_INCLUDE_FROZEN_DUPLICATES	(TEXT("UpdateDuplicatesIncludeFrozenDuplicates"))
#define UPDATE_DUPLICATES_INCLUDE_PARENT_NAMES			(TEXT("UpdateDuplicatesIncludeParentNames"))

#define TOTAL_FOLDER_SETS_FIELD						(TEXT("TotalSets"))
#define ACTIVE_FOLDER_SET_FIELD						(TEXT("ActiveSet"))
#define FOLDER_SET_ENABLED_FIELD						(TEXT("Enabled"))
#define FOLDER_SET_NAME_FIELD							(TEXT("Name"))
#define FOLDER_SET_DESCRIPTION_FIELD				(TEXT("Description"))
#define FOLDER_SET_LEFT_FOLDER_PATH_FIELD			(TEXT("LeftPath"))
#define FOLDER_SET_RIGHT_FOLDER_PATH_FIELD		(TEXT("RightPath"))
#define FOLDER_SET_SYNC_STYLE							(TEXT("SyncStyle"))
#define FOLDER_SET_DO_CRC_CHECK						(TEXT("DoCRCCheck"))
#define FOLDER_SET_USE_SYNC_DATES					(TEXT("UseSyncDates"))
#define FOLDER_SET_VERIFY_COPIED_FILES				(TEXT("VerifyCopiedFiles"))
#define FOLDER_SET_CLOSE_WHEN_COMPLETED			(TEXT("CloseWhenCompleted"))
#define FOLDER_SET_EXIT_WHEN_CLOSED					(TEXT("ExitWhenClosed"))
#define FOLDER_SET_SCAN_THREAD_PRIORITY			(TEXT("ScanThreadPriority"))
#define FOLDER_SET_SYNC_THREAD_PRIORITY			(TEXT("SyncThreadPriority"))
#define FOLDER_SET_LAST_SCAN_TIME					(TEXT("LastScanTime"))

#define FOLDER_SET_USING_DEFAULT_AUTO_FREEZE_EXTENSIONS	(TEXT("UsingDefaultAutoFreezeExtensions"))
#define FOLDER_SET_AUTO_FREEZE_FILE_EXTENSIONS				(TEXT("AutoFreezeFileExtensions"))
#define FOLDER_SET_AUTO_FREEZE_FILE_PREFIXES					(TEXT("AutoFreezeFilePrefixes"))
#define FOLDER_SET_AUTO_FREEZE_FOLDER_PREFIXES				(TEXT("AutoFreezeFolderPrefixes"))

#define FOLDER_SET_SYNC_STYLE							(TEXT("SyncStyle"))

#endif
