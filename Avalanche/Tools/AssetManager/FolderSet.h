////////////////////////////////////////////////////////////////////////////
//
// FolderSet
//
// Class for managing a folder set
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FolderSet.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 6/30/03    Time: 1:38p
 * Updated in $/Avalanche/tools/AssetManager
 * added thread priorities - fix for resource files
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/AssetManager
 * CRC check
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/14/03    Time: 4:35p
 * Updated in $/Avalanche/tools/AssetManager
 * options for folder sets
 * ui for autofreeze list
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/AssetManager
 * auto freeze list
 * sync styles
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

#ifndef ASSET_MANAGER_FOLDER_SET_H
#define ASSET_MANAGER_FOLDER_SET_H

class FolderSet
{
	bool m_bEnabled;
	TupString m_name;
	TupString m_description;
	TupString m_leftFolderPath;
	TupString m_rightFolderPath;

	bool m_bUseDefaultAutoFreezeExtensions;
	TupString m_autoFreezeFileExtensions;
	TupString m_autoFreezeFilePrefixes;
	TupString m_autoFreezeFolderPrefixes;

	int m_syncStyle;
	bool m_bDoCRCCheck;
	bool m_bUseSyncDates;
	bool m_bVerifyCopiedFiles;
	bool m_bCloseWhenCompleted;
	bool m_bExitWhenClosed;
	int m_scanThreadPriority;
	int m_syncThreadPriority;

	CTime m_lastScanTime;

public:
	enum ThreadPriority
	{
		PRIORITY_REALTIME,
		PRIORITY_HIGH,
		PRIORITY_ABOVE_NORMAL,
		PRIORITY_NORMAL,
		PRIORITY_BELOW_NORMAL,
		PRIORITY_LOW,
	};

	FolderSet(void);

	const char *GetName(void) const;
	const char *GetDescription(void) const;
	const char *GetLeftFolderPath(void) const;
	const char *GetRightFolderPath(void) const;
	bool IsEnabled(void) const;
	int GetSyncStyle(void) const;
	bool GetDoCRCCheck(void) const;
	bool GetUseSyncDates(void) const;
	bool GetVerifyCopiedFiles(void) const;
	bool GetCloseWhenCompleted(void) const;
	bool GetExitWhenClosed(void) const;
	int GetScanThreadPriority(void) const;
	int GetSyncThreadPriority(void) const;
	bool IsUsingDefaultAutoFreezeExtensions(void) const;
	const char *GetAutoFreezeFileExtensions(void) const;
	const char *GetAutoFreezeFilePrefixes(void) const;
	const char *GetAutoFreezeFolderPrefixes(void) const;
	CTime GetLastScanTime(void) const;

	void SetName(const char *pName);
	void SetDescription(const char *pDescription);
	void SetLeftFolderPath(const char *pLeftFolderPath);
	void SetRightFolderPath(const char *pRightFolderPath);
	void SetEnabled(bool bEnabled);
	void SetSyncStyle(int syncStyle);
	void SetDoCRCCheck(bool bDoCRCCheck);
	void SetUseSyncDates(bool bUseSyncDates);
	void SetVerifyCopiedFiles(bool bVerifyCopiedFiles);
	void SetCloseWhenCompleted(bool bCloseWhenCompleted);
	void SetExitWhenClosed(bool bExitWhenClosed);
	void SetScanThreadPriority(int priority);
	void SetSyncThreadPriority(int priority);
	void SetUsingDefaultAutoFreezeExtensions(bool value);
	void SetAutoFreezeFileExtensions(const char *pValue);
	void SetAutoFreezeFilePrefixes(const char *pValue);
	void SetAutoFreezeFolderPrefixes(const char *pValue);
	void SetLastScanTime(CTime time);

	void Init(void);

};

#endif
