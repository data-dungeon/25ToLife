////////////////////////////////////////////////////////////////////////////
//
// SyncDBSimple
//
// Simple class for sync databases
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncDBSimple.h $
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 3/27/03    Time: 4:34p
 * Updated in $/Avalanche/tools/SyncUtils
 * log file output
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 3/11/03    Time: 12:21p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete both bug
 * escape rename on right side bug
 * touch when copying over newer file
 * no copy when copying over a file that is out of sync
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 1/31/03    Time: 4:39p
 * Updated in $/Avalanche/tools/SyncUtils
 * file rename
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 11/05/02   Time: 10:02a
 * Updated in $/Avalanche/tools/SyncUtils
 * removed handles and added attributes
 * read only fixes
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 3:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete folders and files
 * child flags for actions and status
 * SetAction function in sync tree manager
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 10:44a
 * Updated in $/Avalanche/tools/SyncUtils
 * Error Handling
 * Feedback on directories
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 4:04p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync feedback fixes (ready to test)
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 2:44p
 * Updated in $/Avalanche/tools/SyncUtils
 * SyncDialog foundation
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 12:51p
 * Updated in $/Avalanche/tools/SyncUtils
 * copy copying complete
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

#ifndef SYNC_UTILS_DLL_SYNC_DB_SIMPLE_H
#define SYNC_UTILS_DLL_SYNC_DB_SIMPLE_H

#ifndef SYNC_UTILS_DLL_SYNC_DB_BASE_H
#include "SyncDBBase.h"
#endif

#ifndef SYNC_UTILS_DLL_TREE_NODE_BASE_H
#include "TreeNodeBase.h"
#endif

#ifndef SYNC_UTILS_DLL_TREE_FACTORY_H
#include "TreeFactory.h"
#endif

#ifndef SYNC_UTILS_DLL_LOAD_DB_FEEDBACK_H
#include "LoadDBFeedback.h"
#endif

class TreeNodeBase;
class GetCRC;
class LogFile;

//-----------------------------------------------------------------------------
// SyncDBSimple class declaration:

class AFX_EXT_CLASS SyncDBSimple : public SyncDBBase
{
	class Feedback
	{
		LoadDBFeedback &m_loadDBFeedback;
		LoadDBFeedback::Stats m_stats;
		CTime m_startTime;
		DWORD m_errorCode;

	public:
		Feedback(LoadDBFeedback &loadDBFeedback);
		bool Update(const char *pFileName,const char *pErrorMessage = NULL);
		bool GetAbortFlag(void) const;
		DWORD GetErrorCode(void) const;
		void SetErrorCode(DWORD errorCode);
		const char *GetLastFileName(void) const;
	};

	SyncDBSimple(void); // not defined

	TreeNodeFolder *m_pTreeRoot;
	TreeFactory m_treeFactory; // use default for now

	GetCRC *m_pGetCRC; // pointer to crc class

	LogFile &m_logFile;

	bool m_bRemoteDB;

public:
	SyncDBSimple(const char *pFilePath,LoadDBFeedback &loadDBFeedback,LogFile &logFile);
	~SyncDBSimple();

	bool IsRemoteDB(void) const;
	TreeNodeFolder *GetTreeRoot(void) const;
	TreeNodeFolder *CreateDirectory(SyncDBBase *pOtherDB,TreeNodeFolder *pOtherFolder,TreeNodeFolder *pParentFolder,SyncFeedback &feedback,bool bRight);
	TreeNodeFile *CopyFileToDB(SyncDBBase *pOtherDB,TreeNodeFile *pOtherFile,TreeNodeFolder *pParentFolder,TreeNodeFile *pFile,SyncFeedback &feedback,int flags,bool bRight);
	bool DeleteFileFromDB(TreeNodeFile *pFile,SyncFeedback &feedback,bool bRight);
	bool DeleteDirectory(TreeNodeFolder *pFolder,SyncFeedback &feedback,bool bRight);
	void GetPathToFile(const TreeNodeFile *pFile,TupString &filePath) const;
	int RenameFolder(TreeNodeFolder *pFolder,const char *pNewName);
	int RenameFile(TreeNodeFile *pFile,const char *pNewName);
	bool GetCRCFromFile(TreeNodeFile *pFile,DWORD &crcValue) const;

private:
	void SetupDatabase(LoadDBFeedback &loadDBFeedback);
	bool GetNodeChildren(TreeNodeFolder *pTreeNodeFolder,const char *pFilePathString,Feedback &feedback);
	void SortChildren(TreeNodeFolder *pTreeNodeFolder,bool bSortChildFolders);
	int CompareNodes(TreeNodeBase *pNode1,TreeNodeBase *pNode2);
	void AddChildNodeSorted(TreeNodeFolder *pParentFolder,TreeNodeBase *pNewChildNode);
	bool CheckFileInSync(TreeNodeFile *pFile);
	bool CheckAndTouch(SyncDBBase *pOtherDB,TreeNodeFile *pOtherFile,TreeNodeFile *pFile);
	bool TouchFile(TreeNodeFile *pFile);
	void CheckAndRemoveLog(const char *pPathString);
	static bool RemotePath(const char *pPathString);
};

//-----------------------------------------------------------------------------

#endif

