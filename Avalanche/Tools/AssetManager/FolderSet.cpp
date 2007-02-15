////////////////////////////////////////////////////////////////////////////
//
// FolderSet
//
// Class for managing a folder set
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: FolderSet.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 6/30/03    Time: 1:38p
 * Updated in $/Avalanche/tools/AssetManager
 * added thread priorities - fix for resource files
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/AssetManager
 * CRC checks
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/19/03    Time: 12:14p
 * Updated in $/Avalanche/tools/AssetManager
 * default change
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

#include "stdafx.h"
#include "FolderSet.h"

FolderSet::FolderSet(void) : 
	m_bEnabled(false), 
	m_syncStyle(0),
	m_bUseDefaultAutoFreezeExtensions(true),
	m_bDoCRCCheck(false),
	m_bUseSyncDates(false),
	m_bVerifyCopiedFiles(false),
	m_bCloseWhenCompleted(false),
	m_bExitWhenClosed(false),
	m_scanThreadPriority(PRIORITY_NORMAL),
	m_syncThreadPriority(PRIORITY_NORMAL)
{
}

const char *FolderSet::GetName(void) const
{
	return m_name;
}

const char *FolderSet::GetDescription(void) const
{
	return m_description;
}

const char *FolderSet::GetLeftFolderPath(void) const
{
	return m_leftFolderPath;
}

const char *FolderSet::GetRightFolderPath(void) const
{
	return m_rightFolderPath;
}

bool FolderSet::IsEnabled(void) const
{
	return m_bEnabled;
}

int FolderSet::GetSyncStyle(void) const
{
	return m_syncStyle;
}

bool FolderSet::GetDoCRCCheck(void) const
{
	return m_bDoCRCCheck;
}

bool FolderSet::GetUseSyncDates(void) const
{
	return m_bUseSyncDates;
}

bool FolderSet::GetVerifyCopiedFiles(void) const
{
	return m_bVerifyCopiedFiles;
}

bool FolderSet::GetCloseWhenCompleted(void) const
{
	return m_bCloseWhenCompleted;
}

bool FolderSet::GetExitWhenClosed(void) const
{
	return m_bExitWhenClosed;
}

int FolderSet::GetScanThreadPriority(void) const
{
	return m_scanThreadPriority;
}

int FolderSet::GetSyncThreadPriority(void) const
{
	return m_syncThreadPriority;
}

bool FolderSet::IsUsingDefaultAutoFreezeExtensions(void) const
{
	return m_bUseDefaultAutoFreezeExtensions;
}

const char *FolderSet::GetAutoFreezeFileExtensions(void) const
{
	return m_autoFreezeFileExtensions;
}

const char *FolderSet::GetAutoFreezeFilePrefixes(void) const
{
	return m_autoFreezeFilePrefixes;
}

const char *FolderSet::GetAutoFreezeFolderPrefixes(void) const
{
	return m_autoFreezeFolderPrefixes;
}

CTime FolderSet::GetLastScanTime(void) const
{
	return m_lastScanTime;
}

void FolderSet::SetName(const char *pName)
{
	m_name = pName;
}

void FolderSet::SetDescription(const char *pDescription)
{
	m_description = pDescription;
}

void FolderSet::SetLeftFolderPath(const char *pLeftFolderPath)
{
	m_leftFolderPath = pLeftFolderPath;
}

void FolderSet::SetRightFolderPath(const char *pRightFolderPath)
{
	m_rightFolderPath = pRightFolderPath;
}

void FolderSet::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

void FolderSet::SetUsingDefaultAutoFreezeExtensions(bool value)
{
	m_bUseDefaultAutoFreezeExtensions = value;
}

void FolderSet::SetAutoFreezeFileExtensions(const char *pValue)
{
	m_autoFreezeFileExtensions = pValue;
}

void FolderSet::SetAutoFreezeFilePrefixes(const char *pValue)
{
	m_autoFreezeFilePrefixes = pValue;
}

void FolderSet::SetAutoFreezeFolderPrefixes(const char *pValue)
{
	m_autoFreezeFolderPrefixes = pValue;
}

void FolderSet::SetSyncStyle(int syncStyle)
{
	m_syncStyle = syncStyle;
}

void FolderSet::SetDoCRCCheck(bool bValue)
{
	m_bDoCRCCheck = bValue;
}

void FolderSet::SetUseSyncDates(bool bValue)
{
	m_bUseSyncDates = bValue;
}

void FolderSet::SetVerifyCopiedFiles(bool bValue)
{
	m_bVerifyCopiedFiles = bValue;
}

void FolderSet::SetCloseWhenCompleted(bool bValue)
{
	m_bCloseWhenCompleted = bValue;
}

void FolderSet::SetExitWhenClosed(bool bValue)
{
	m_bExitWhenClosed = bValue;
}

void FolderSet::SetScanThreadPriority(int priority)
{
	m_scanThreadPriority = priority;
}

void FolderSet::SetSyncThreadPriority(int priority)
{
	m_syncThreadPriority = priority;
}

void FolderSet::SetLastScanTime(CTime time)
{
	m_lastScanTime = time;
}

void FolderSet::Init(void)
{
	m_bEnabled = false;
	m_name.Empty();
	m_description.Empty();
	m_leftFolderPath.Empty();
	m_rightFolderPath.Empty();
	m_bUseDefaultAutoFreezeExtensions = true;
	m_autoFreezeFileExtensions.Empty();
	m_autoFreezeFilePrefixes.Empty();
	m_autoFreezeFolderPrefixes.Empty();
	m_syncStyle = 0;
	m_scanThreadPriority = PRIORITY_NORMAL;
	m_syncThreadPriority = PRIORITY_NORMAL;
	m_bDoCRCCheck = false;
	m_lastScanTime = CTime(1980,1,1,12,0,0);
}
