////////////////////////////////////////////////////////////////////////////
//
// SyncTreeManager
//
// Class used for creating and maintaining the sync tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncTreeManager.h $
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 7/02/03    Time: 3:19p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 3/27/03    Time: 4:34p
 * Updated in $/Avalanche/tools/SyncUtils
 * log file output
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 3/20/03    Time: 2:20p
 * Updated in $/Avalanche/tools/SyncUtils
 * dialog renaming
 * auto freeze paths name changes
 * remove from list (including children)
 * fix bug with not clearing the auto freeze path flag
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 3/14/03    Time: 4:35p
 * Updated in $/Avalanche/tools/SyncUtils
 * options for folder sets
 * ui for autofreeze list
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 1/15/03    Time: 4:12p
 * Updated in $/Avalanche/tools/SyncUtils
 * part one of client database stuff
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 1/06/03    Time: 9:23a
 * Updated in $/Avalanche/tools/SyncUtils
 * unfreeze all except fix for default actions for folders
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 12/06/02   Time: 2:12p
 * Updated in $/Avalanche/tools/SyncUtils
 * cancel button check for sync.  refresh bug (clear databases)
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 11/18/02   Time: 1:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * freeze implemented
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 11/01/02   Time: 4:36p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 10/31/02   Time: 4:31p
 * Updated in $/Avalanche/tools/SyncUtils
 * context menu
 * exclusions / including / expanding / collapsing
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 10/30/02   Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stuff
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 3:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete folders and files
 * child flags for actions and status
 * SetAction function in sync tree manager
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 10:44a
 * Updated in $/Avalanche/tools/SyncUtils
 * Error Handling
 * Feedback on directories
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 10/17/02   Time: 4:37p
 * Updated in $/Avalanche/tools/SyncUtils
 * updated stats
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
 * User: Adam Clayton Date: 10/14/02   Time: 1:21p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stats
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 12:51p
 * Updated in $/Avalanche/tools/SyncUtils
 * copy copying complete
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/11/02   Time: 4:38p
 * Updated in $/Avalanche/tools/SyncUtils
 * first draft of folder and file copies
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 */

#ifndef SYNC_UTILS_DLL_SYNC_TREE_MANAGER_H
#define SYNC_UTILS_DLL_SYNC_TREE_MANAGER_H

#ifndef SYNC_UTILS_DLL_LOAD_DB_FEEDBACK_H
#include "LoadDBFeedback.h"
#endif

//-----------------------------------------------------------------------------
// SyncTreeFactory class declaration:

class SyncDBBase;
class TreeNodeBase;
class TreeNodeFolder;
class SyncTreeNodeFolder;
class SyncTreeNodeFile;
class SyncTreeFactory;
class SyncFeedback;
class AutoExclude;
class ClientDataManager;

class AFX_EXT_CLASS SyncTreeManager
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

	Feedback *m_pLoadFeedback;

public:
	class TreeStats
	{
	public:
		unsigned long m_rootChildFlags; // all the childflags or'ed together from the tree

		int m_filesLeft;
		int m_foldersLeft;
		int m_filesRight;
		int m_foldersRight;

		__int64 m_totalBytesLeft;
		__int64 m_totalBytesRight;
		
		int m_filesFoldersCopiedLeftRight;
		int m_filesFoldersCopiedRightLeft;

		int m_filesFoldersDeletedLeft;
		int m_filesFoldersDeletedRight;

		__int64 m_totalBytesCopiedLeftRight;
		__int64 m_totalBytesCopiedRightLeft;
	};

	class SyncEnable
	{
	public:
		bool m_bLRCopyEnable;
		bool m_bRLCopyEnable;
		bool m_bLDeleteEnable;
		bool m_bRDeleteEnable;
	};

	enum Actions
	{
		ACTION_EXCLUDE,
		ACTION_DELETE_LEFT,
		ACTION_DELETE_RIGHT,
		ACTION_DELETE_BOTH,
		ACTION_COPY_LEFT_TO_RIGHT,
		ACTION_COPY_RIGHT_TO_LEFT,
		ACTION_FREEZE,
		ACTION_UNFREEZE,
		ACTION_DEFAULT_ACTION,
		ACTION_AUTO_FREEZE_ADD,
		ACTION_AUTO_FREEZE_REMOVE,
		ACTION_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN,
	};

	enum SyncStyles
	{
		SYNC_STYLE_2_WAY,
		SYNC_STYLE_AUGMENT_LEFT,
		SYNC_STYLE_AUGMENT_RIGHT,
		SYNC_STYLE_REFRESH_LEFT,
		SYNC_STYLE_REFRESH_RIGHT,
		SYNC_STYLE_UPDATE_LEFT,
		SYNC_STYLE_UPDATE_RIGHT,
		SYNC_STYLE_UPDATE_AND_PRUNE_LEFT,
		SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT,
		SYNC_STYLE_FORCE_LEFT,
		SYNC_STYLE_FORCE_RIGHT,
		SYNC_STYLE_NUM_STYLES, // must be last
	};

private:
	// data members

	// I don't own these...
	SyncTreeFactory &m_syncTreeFactory; // factory for creating the sync db tree nodes
	SyncDBBase *m_pLeftDB;
	SyncDBBase *m_pRightDB;
	TreeNodeFolder *m_pLeftDBTreeRoot;
	TreeNodeFolder *m_pRightDBTreeRoot;
	AutoExclude *m_pAutoExclude;

	SyncStyles m_syncStyle;

	bool m_bCheckingCRC;
	bool m_bUseSyncDates;
	bool m_bVerifyCopiedFiles;

	// I own these
	SyncTreeNodeFolder *m_pSyncTreeRoot;

	// private methods not defined
	SyncTreeManager(void);

public:
	SyncTreeManager(SyncTreeFactory &syncTreeFactory);
	virtual ~SyncTreeManager();

	SyncDBBase *GetLeftDB(void) const;
	SyncDBBase *GetRightDB(void) const;
	SyncTreeNodeFolder *GetSyncTree(void) const;
	void Synchronize(SyncFeedback &feedback,SyncEnable &syncEnable,unsigned long viewFlags);

	void SetDatabases(SyncDBBase *pLeftDB,SyncDBBase *pRightDB,ClientDataManager *pClientDataManager,LoadDBFeedback &feedback);
	void ClearDatabases(void);
	void SetAutoExclude(AutoExclude *autoExclude);

	void GetTreeStats(TreeStats &treeStats,unsigned long viewFlags);
	void GetTreeStats(TreeNodeBase *pNode,TreeStats &treeStats);

	void SetDefaultActions(bool bResetManualFlags);
	int GetDefaultAction(TreeNodeBase *pNode) const;
	void SetStatusFlags(void);

	void SetAction(TreeNodeBase *pNode,Actions action,bool bManualSet,bool bDoChildren,bool bFrozen=false,int nodeDepth=0);
	unsigned long GetValidActionFlags(TreeNodeBase *pNode,bool bLeftControl,bool bDoChildren,int nodeDepth = 0);
	unsigned long GetValidActionFlagsFrozenParent(TreeNodeBase *pNode,bool bLeftControl,bool bDoChildren,int nodeDepth = 0);

	SyncStyles GetSyncStyle(void) const;
	void SetSyncStyle(SyncStyles syncStyle);

	bool IsCheckingCRC(void) const;
	void SetCheckingCRC(bool bCheckingCRC);

	bool IsUsingSyncDates(void) const;
	void SetUsingSyncDates(bool bUseSyncDates);

	bool IsVerifyingCopiedFiles(void) const;
	void SetVerifyCopiedFiles(bool bVerifyCopiedFiles);

	void GetUsedAutoFreezePaths(TupArray<bool> &usedEntries);

private:
	void BuildSyncTree(ClientDataManager *pClientDataManager);
	void BuildSyncNodeChildren(SyncTreeNodeFolder *pSyncTreeNodeParent);
	void CreateLeftOrRightNode(SyncTreeNodeFolder *pParent,TreeNodeBase *pNode,bool bRight);
	void CreateLeftAndRightNode(SyncTreeNodeFolder *pParent,TreeNodeBase *pLeftNode,TreeNodeBase *pRightNode);
	bool SameNodeTypes(TreeNodeBase *pLeftNode,TreeNodeBase *pRightNode);
	int CompareTimes(FILETIME leftTime,FILETIME rightTime);
	void SynchronizeNode(TreeNodeBase *pNode,SyncFeedback &feedback,SyncEnable &syncEnable);
	bool CreateDirectory(SyncTreeNodeFolder *pNodeFolder,SyncFeedback &feedback,bool bRight);
	bool DeleteDirectory(SyncTreeNodeFolder *pSyncNodeFolder,SyncFeedback &feedback,bool bRight);
	int CompareNodes(TreeNodeBase *pNode1,TreeNodeBase *pNode2);
	bool SetDefaultActions(TreeNodeBase *pNode,bool bResetManualFlags,bool bDoChildren,bool bFrozen);
	void SetStatusFlags(TreeNodeBase *pNode,bool bDoChildren);
	void ClearPreviousFlag(TreeNodeBase *pNode,bool bRight,bool bDoChildren);
	void SetPreviousFlag(TreeNodeBase *pNode,bool bRight,bool bDoChildren);
	void RefreshSyncDates(SyncTreeNodeFile *pFile);
	void DetatchFromDB(TreeNodeBase *pNode,bool bRight);
	void RemoveDeadNodes(TreeNodeBase *pNode);
	void ValidateActionsAndVisiblity(unsigned long viewFlags);
	unsigned long ValidateActionsAndVisiblity(TreeNodeBase *pNode,unsigned long viewFlags);
	void SetChildFlags(unsigned long &rootChildFlags);
	void SetChildFlags(TreeNodeBase *pNode,unsigned long &parentChildFlags);
	void SetValidActions(TreeNodeBase *pNode,bool bDoChildren);
	void GetUsedAutoFreezePaths(TreeNodeBase *pNode,TupArray<bool> &usedEntries);
	bool CheckFileCRC(SyncTreeNodeFile *pSyncTreeNodeFile) const;
};

//-----------------------------------------------------------------------------

#endif

