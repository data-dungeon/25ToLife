////////////////////////////////////////////////////////////////////////////
//
// SyncDBBase
//
// Base class for sync databases
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncDBBase.h $
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/27/03    Time: 4:34p
 * Updated in $/Avalanche/tools/SyncUtils
 * log file output
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 1/31/03    Time: 4:39p
 * Updated in $/Avalanche/tools/SyncUtils
 * file rename
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 3:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete folders and files
 * child flags for actions and status
 * SetAction function in sync tree manager
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 10:44a
 * Updated in $/Avalanche/tools/SyncUtils
 * Error Handling
 * Feedback on directories
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 4:04p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync feedback fixes (ready to test)
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/11/02   Time: 4:38p
 * Updated in $/Avalanche/tools/SyncUtils
 * first draft of folder and file copies
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

#ifndef SYNC_UTILS_DLL_SYNC_DB_BASE_H
#define SYNC_UTILS_DLL_SYNC_DB_BASE_H

//-----------------------------------------------------------------------------
// SyncDBBase class declaration:

class TreeNodeFolder;
class TreeNodeFile;
class TreeFactory;
class SyncFeedback;
class LogFile;

class AFX_EXT_CLASS SyncDBBase  
{
public:
	enum Flags
	{
		FLAGS_UPDATE_DEST_TO_CURRENT_TIME		= 0x0001,
		FLAGS_VERIFY_COPIED_FILE					= 0x0002,
	};

private:
	SyncDBBase(void); // not defined

protected:
	TupString m_filePath;

public:
	SyncDBBase(const char *pFilePath);
	virtual ~SyncDBBase();
	virtual bool IsRemoteDB(void) const = 0;

	virtual TreeNodeFolder *GetTreeRoot(void) const = 0;
	virtual TreeNodeFolder *CreateDirectory(SyncDBBase *pOtherDB,TreeNodeFolder *pOtherFolder,TreeNodeFolder *pParentFolder,SyncFeedback &feedback,bool bRight) = 0;
	virtual TreeNodeFile *CopyFileToDB(SyncDBBase *pOtherDB,TreeNodeFile *pOtherFile,TreeNodeFolder *pParentFolder,TreeNodeFile *pFile,SyncFeedback &feedback,int flags,bool bRight) = 0;
	virtual bool DeleteFileFromDB(TreeNodeFile *pFile,SyncFeedback &feedback,bool bRight) = 0;
	virtual bool DeleteDirectory(TreeNodeFolder *pFolder,SyncFeedback &feedback,bool bRight) = 0;
	virtual void GetPathToFile(const TreeNodeFile *pFile,TupString &filePath) const = 0;
	virtual int RenameFolder(TreeNodeFolder *pFolder,const char *pNewName) = 0;
	virtual int RenameFile(TreeNodeFile *pFile,const char *pNewName) = 0;
	virtual bool GetCRCFromFile(TreeNodeFile *pFile,DWORD &crcValue) const = 0;
};

//-----------------------------------------------------------------------------

#endif

