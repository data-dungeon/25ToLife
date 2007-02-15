////////////////////////////////////////////////////////////////////////////
//
// SyncDBServerClient
//
// class to handle being a client of a server
//
////////////////////////////////////////////////////////////////////////////


#ifndef SYNC_UTILS_DLL_SYNC_DB_SERVER_CLIENT_H
#define SYNC_UTILS_DLL_SYNC_DB_SERVER_CLIENT_H

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

class AFX_EXT_CLASS SyncDBServerClient : public SyncDBBase
{
	class Feedback
	{
		LoadDBFeedback &m_loadDBFeedback;
		LoadDBFeedback::Stats m_stats;
		CTime m_startTime;

	public:
		Feedback(LoadDBFeedback &loadDBFeedback);
		bool Update(const char *pFileName);
	};

	SyncDBServerClient(void); // not defined

	TreeNodeFolder *m_pTreeRoot;
	TreeFactory m_treeFactory; // use default for now

	GetCRC *m_pGetCRC; // pointer to crc class

	LogFile &m_logFile;

public:
	SyncDBServerClient(const char *pFilePath,LoadDBFeedback &loadDBFeedback,LogFile &logFile);
	~SyncDBServerClient();

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

};

//-----------------------------------------------------------------------------

#endif

