////////////////////////////////////////////////////////////////////////////
//
// SyncTreeManager
//
// Class used for creating and maintaining the sync tree
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncTreeManager.cpp $
 * 
 * *****************  Version 39  *****************
 * User: Adam Clayton Date: 7/02/03    Time: 3:19p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 38  *****************
 * User: Adam Clayton Date: 5/01/03    Time: 4:10p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 37  *****************
 * User: Adam Clayton Date: 3/28/03    Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * view status flags
 * 
 * *****************  Version 36  *****************
 * User: Adam Clayton Date: 3/27/03    Time: 4:34p
 * Updated in $/Avalanche/tools/SyncUtils
 * log file output
 * 
 * *****************  Version 35  *****************
 * User: Adam Clayton Date: 3/26/03    Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * CRC checks
 * 
 * *****************  Version 34  *****************
 * User: Adam Clayton Date: 3/20/03    Time: 2:20p
 * Updated in $/Avalanche/tools/SyncUtils
 * dialog renaming
 * auto freeze paths name changes
 * remove from list (including children)
 * fix bug with not clearing the auto freeze path flag
 * 
 * *****************  Version 33  *****************
 * User: Adam Clayton Date: 3/14/03    Time: 4:35p
 * Updated in $/Avalanche/tools/SyncUtils
 * options for folder sets
 * ui for autofreeze list
 * 
 * *****************  Version 32  *****************
 * User: Adam Clayton Date: 3/11/03    Time: 12:21p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete both bug
 * escape rename on right side bug
 * touch when copying over newer file
 * no copy when copying over a file that is out of sync
 * 
 * *****************  Version 31  *****************
 * User: Adam Clayton Date: 3/10/03    Time: 4:43p
 * Updated in $/Avalanche/tools/SyncUtils
 * auto freeze list
 * sync styles
 * 
 * *****************  Version 30  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 29  *****************
 * User: Adam Clayton Date: 1/29/03    Time: 9:17a
 * Updated in $/Avalanche/tools/SyncUtils
 * validatation bug fixes
 * 
 * *****************  Version 28  *****************
 * User: Adam Clayton Date: 1/28/03    Time: 12:38p
 * Updated in $/Avalanche/tools/SyncUtils
 * file time difference threshold from .2 sec to 2 sec
 * 
 * *****************  Version 27  *****************
 * User: Adam Clayton Date: 1/20/03    Time: 2:48p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data sync bugs
 * 
 * *****************  Version 26  *****************
 * User: Adam Clayton Date: 1/20/03    Time: 1:12p
 * Updated in $/Avalanche/tools/SyncUtils
 * Phase 2 complete
 * 
 * *****************  Version 25  *****************
 * User: Adam Clayton Date: 1/15/03    Time: 4:12p
 * Updated in $/Avalanche/tools/SyncUtils
 * part one of client database stuff
 * 
 * *****************  Version 24  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 * 
 * *****************  Version 23  *****************
 * User: Adam Clayton Date: 1/06/03    Time: 9:23a
 * Updated in $/Avalanche/tools/SyncUtils
 * unfreeze all except fix for default actions for folders
 * 
 * *****************  Version 22  *****************
 * User: Adam Clayton Date: 12/06/02   Time: 2:12p
 * Updated in $/Avalanche/tools/SyncUtils
 * cancel button check for sync.  refresh bug (clear databases)
 * 
 * *****************  Version 21  *****************
 * User: Adam Clayton Date: 12/05/02   Time: 9:06a
 * Updated in $/Avalanche/tools/SyncUtils
 * total bytes to copy looks at enable flags
 * 
 * *****************  Version 20  *****************
 * User: Adam Clayton Date: 12/02/02   Time: 12:51p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete both bug - default action context fixed
 * 
 * *****************  Version 19  *****************
 * User: Adam Clayton Date: 11/26/02   Time: 4:24p
 * Updated in $/Avalanche/tools/SyncUtils
 * fixed button behavior, auto exclude stuff, trying to get tooltips to
 * work...
 * 
 * *****************  Version 18  *****************
 * User: Adam Clayton Date: 11/21/02   Time: 4:24p
 * Updated in $/Avalanche/tools/SyncUtils
 * bug with visiblity
 * 
 * *****************  Version 17  *****************
 * User: Adam Clayton Date: 11/18/02   Time: 1:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * freeze implemented
 * 
 * *****************  Version 16  *****************
 * User: Adam Clayton Date: 11/05/02   Time: 10:03a
 * Updated in $/Avalanche/tools/SyncUtils
 * attributes
 * default for hidden and system is excluded
 * 
 * *****************  Version 15  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 14  *****************
 * User: Adam Clayton Date: 11/01/02   Time: 4:36p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 13  *****************
 * User: Adam Clayton Date: 10/31/02   Time: 4:31p
 * Updated in $/Avalanche/tools/SyncUtils
 * context menu
 * exclusions / including / expanding / collapsing
 * 
 * *****************  Version 12  *****************
 * User: Adam Clayton Date: 10/30/02   Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stuff
 * 
 * *****************  Version 11  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 3:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete folders and files
 * child flags for actions and status
 * SetAction function in sync tree manager
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 10:44a
 * Updated in $/Avalanche/tools/SyncUtils
 * Error Handling
 * Feedback on directories
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 10/17/02   Time: 4:37p
 * Updated in $/Avalanche/tools/SyncUtils
 * updated stats
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 4:04p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync feedback fixes (ready to test)
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 10/16/02   Time: 2:44p
 * Updated in $/Avalanche/tools/SyncUtils
 * SyncDialog foundation
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 1:21p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stats
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 12:51p
 * Updated in $/Avalanche/tools/SyncUtils
 * copy copying complete
 * 
 * *****************  Version 4  *****************
 * User: Brad Worthen Date: 10/14/02   Time: 12:17p
 * Updated in $/Avalanche/tools/SyncUtils
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

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "SyncTreeManager.h"
#include "TreeFactory.h"
#include "SyncTreeFactory.h"
#include "SyncDBBase.h"
#include "TreeNodeFolder.h"
#include "TreeNodeFile.h"
#include "TreeIterator.h"
#include "SyncTreeNodeFolder.h"
#include "SyncTreeNodeFile.h"
#include "SyncFeedback.h"
#include "SyncTreeNodeFlags.h"
#include "AutoExclude.h"
#include "ClientDataManager.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SyncTreeManager::SyncTreeManager(SyncTreeFactory &syncTreeFactory) : 
	m_syncTreeFactory(syncTreeFactory),
	m_pLeftDB(NULL),
	m_pRightDB(NULL),
	m_pAutoExclude(NULL),
	m_pLeftDBTreeRoot(NULL),
	m_pRightDBTreeRoot(NULL),
	m_pSyncTreeRoot(NULL),
	m_bCheckingCRC(false),
	m_bUseSyncDates(false),
	m_bVerifyCopiedFiles(false),
	m_syncStyle(SYNC_STYLE_2_WAY),
	m_pLoadFeedback(NULL)
{
}

SyncTreeManager::~SyncTreeManager()
{
	delete m_pSyncTreeRoot;
}

SyncDBBase *SyncTreeManager::GetLeftDB(void) const
{
	return m_pLeftDB;
}

SyncDBBase *SyncTreeManager::GetRightDB(void) const
{
	return m_pRightDB;
}

void SyncTreeManager::SetDatabases(SyncDBBase *pLeftDB,SyncDBBase *pRightDB,ClientDataManager *pClientDataManager,LoadDBFeedback &feedback)
{
	ASSERT(!m_pLoadFeedback);
	m_pLoadFeedback = new Feedback(feedback);

	bool bWorkToDo = false;
	if (m_pLeftDB != pLeftDB)
	{
		bWorkToDo = true;
		// set this to the database to use (we don't own it)
		m_pLeftDB = pLeftDB;
		m_pLeftDBTreeRoot = NULL;
		// if we have a db then create a new tree
		if (m_pLeftDB)
		{
			m_pLeftDBTreeRoot = m_pLeftDB->GetTreeRoot();
		}
	}
	if (m_pRightDB != pRightDB)
	{
		bWorkToDo = true;
		// set this to the database to use (we don't own it)
		m_pRightDB = pRightDB;
		m_pRightDBTreeRoot = NULL;
		// if we have a db then create a new tree
		if (m_pRightDB)
		{
			m_pRightDBTreeRoot = m_pRightDB->GetTreeRoot();
		}
	}
	if (bWorkToDo)
	{
		BuildSyncTree(pClientDataManager);
	}
	delete m_pLoadFeedback;
	m_pLoadFeedback = NULL;
}

void SyncTreeManager::ClearDatabases(void)
{
	m_pLeftDB = NULL;
	m_pRightDB = NULL;
	m_pLeftDBTreeRoot = NULL;
	m_pRightDBTreeRoot = NULL;
	delete m_pSyncTreeRoot;
	m_pSyncTreeRoot = NULL;
}


void SyncTreeManager::SetAutoExclude(AutoExclude *pAutoExclude)
{
	m_pAutoExclude = pAutoExclude;
}

SyncTreeNodeFolder *SyncTreeManager::GetSyncTree(void) const
{
	return m_pSyncTreeRoot;
}

void SyncTreeManager::BuildSyncTree(ClientDataManager *pClientDataManager)
{
	delete m_pSyncTreeRoot;
	m_pSyncTreeRoot = NULL;
	// if both databases are not set then return
	if (!m_pLeftDBTreeRoot && !m_pRightDBTreeRoot)
		return;
	m_pSyncTreeRoot = dynamic_cast<SyncTreeNodeFolder *>(m_syncTreeFactory.CreateTreeNodeFolder());
	ASSERT(m_pSyncTreeRoot);
	m_pSyncTreeRoot->SetRootFolderInfo();
	m_pSyncTreeRoot->SetLeftFolderNode(m_pLeftDBTreeRoot);
	m_pSyncTreeRoot->SetRightFolderNode(m_pRightDBTreeRoot);
	BuildSyncNodeChildren(m_pSyncTreeRoot);
	// update some flags in the sync tree using the client data manager
	if (pClientDataManager)
	{
		pClientDataManager->UpdateSyncTree(m_pSyncTreeRoot);
	}
	SetStatusFlags();
	SetDefaultActions(false);
}


void SyncTreeManager::BuildSyncNodeChildren(SyncTreeNodeFolder *pParent)
{
	ASSERT(pParent);
	TreeNodeFolder *pLeft = pParent->GetLeftFolderNode();
	TreeNodeFolder *pRight = pParent->GetRightFolderNode();
	// if we have both sides..
	if (pLeft && pRight)
	{
		TreeNodeBase *pLeftNode = pLeft->GetFirstChildNode();
		TreeNodeBase *pRightNode = pRight->GetFirstChildNode();
		while(pLeftNode || pRightNode)
		{
			if (pLeftNode && pRightNode)
			{
				int stringCompare = CompareNodes(pLeftNode,pRightNode);
				// if they are equal
				if (stringCompare==0)
				{
					if (SameNodeTypes(pLeftNode,pRightNode))
					{
						CreateLeftAndRightNode(pParent,pLeftNode,pRightNode);
					}
					else // if they are not the same type of node (file & folder)
					{
						CreateLeftOrRightNode(pParent,pLeftNode,false);
						CreateLeftOrRightNode(pParent,pRightNode,true);
					}
					pLeftNode=pLeftNode->GetNext();
					pRightNode=pRightNode->GetNext();
				}
				// if left < right
				else if (stringCompare<0)
				{
					CreateLeftOrRightNode(pParent,pLeftNode,false);
					pLeftNode=pLeftNode->GetNext();
				}
				// if left > right
				else
				{
					CreateLeftOrRightNode(pParent,pRightNode,true);
					pRightNode=pRightNode->GetNext();
				}
			}
			else if (pLeftNode)
			{
				CreateLeftOrRightNode(pParent,pLeftNode,false);
				pLeftNode=pLeftNode->GetNext();
			}
			else // if (pRightNode)
			{
				CreateLeftOrRightNode(pParent,pRightNode,true);
				pRightNode=pRightNode->GetNext();
			}
		}

	}
	// just the left one
	else if (pLeft)
	{
		TreeNodeBase *pNode = pLeft->GetFirstChildNode();
		while(pNode)
		{
			CreateLeftOrRightNode(pParent,pNode,false);
			pNode=pNode->GetNext();
		}
	}
	// just the right one
	else if (pRight)
	{
		TreeNodeBase *pNode = pRight->GetFirstChildNode();
		while(pNode)
		{
			CreateLeftOrRightNode(pParent,pNode,true);
			pNode=pNode->GetNext();
		}
	}
	else
	{
		ASSERT(false); // this should not be
	}
}

void SyncTreeManager::CreateLeftOrRightNode(SyncTreeNodeFolder *pParent,TreeNodeBase *pNode,bool bRight)
{
	TreeNodeFolder *pNodeFolder = dynamic_cast<TreeNodeFolder *>(pNode);
	// if this child is a folder
	if (pNodeFolder)
	{
		SyncTreeNodeFolder *pNewChildFolder = dynamic_cast<SyncTreeNodeFolder *>(m_syncTreeFactory.CreateTreeNodeFolder());
		ASSERT(pNewChildFolder);
		pNewChildFolder->CopyInfo(*pNodeFolder);
		if (bRight)
		{
			pNewChildFolder->SetRightFolderNode(pNodeFolder);
			pNewChildFolder->SetFlags(SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT);
		}
		else
		{
			pNewChildFolder->SetLeftFolderNode(pNodeFolder);
			pNewChildFolder->SetFlags(SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT);
		}
		pParent->AddChild(pNewChildFolder);
		BuildSyncNodeChildren(pNewChildFolder);
	}
	else // must be a file
	{
		TreeNodeFile *pNodeFile = dynamic_cast<TreeNodeFile *>(pNode);
		ASSERT(pNodeFile);
		SyncTreeNodeFile *pNewChildFile = dynamic_cast<SyncTreeNodeFile *>(m_syncTreeFactory.CreateTreeNodeFile());
		ASSERT(pNewChildFile);
		pNewChildFile->CopyInfo(*pNodeFile);
		if (bRight)
		{
			pNewChildFile->SetRightFileNode(pNodeFile);
			pNewChildFile->SetFlags(SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT);
		}
		else
		{
			pNewChildFile->SetLeftFileNode(pNodeFile);
			pNewChildFile->SetFlags(SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT);
		}
		pParent->AddChild(pNewChildFile);
	}
}

void SyncTreeManager::CreateLeftAndRightNode(SyncTreeNodeFolder *pParent,TreeNodeBase *pLeftNode,TreeNodeBase *pRightNode)
{
	TreeNodeFolder *pLeftNodeFolder = dynamic_cast<TreeNodeFolder *>(pLeftNode);
	TreeNodeFolder *pRightNodeFolder = dynamic_cast<TreeNodeFolder *>(pRightNode);
	// if this child is a folder
	if (pLeftNodeFolder)
	{
		ASSERT(pRightNodeFolder);
		SyncTreeNodeFolder *pNewChildFolder = dynamic_cast<SyncTreeNodeFolder *>(m_syncTreeFactory.CreateTreeNodeFolder());
		ASSERT(pNewChildFolder);
		pNewChildFolder->CopyInfo(*pLeftNodeFolder);

		unsigned long attributes = pLeftNodeFolder->GetAttributes();
		attributes |= pRightNodeFolder->GetAttributes();
		pNewChildFolder->SetAttributes(attributes);

		pNewChildFolder->SetLeftFolderNode(pLeftNodeFolder);
		pNewChildFolder->SetRightFolderNode(pRightNodeFolder);
		pNewChildFolder->SetFlags(SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT|SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT);
		pParent->AddChild(pNewChildFolder);
		BuildSyncNodeChildren(pNewChildFolder);
	}
	else // must be a file
	{
		TreeNodeFile *pLeftNodeFile = dynamic_cast<TreeNodeFile *>(pLeftNode);
		ASSERT(pLeftNodeFile);
		TreeNodeFile *pRightNodeFile = dynamic_cast<TreeNodeFile *>(pRightNode);
		ASSERT(pRightNodeFile);
		SyncTreeNodeFile *pNewChildFile = dynamic_cast<SyncTreeNodeFile *>(m_syncTreeFactory.CreateTreeNodeFile());
		ASSERT(pNewChildFile);
		pNewChildFile->CopyInfo(*pLeftNodeFile);

		unsigned long attributes = pLeftNodeFile->GetAttributes();
		attributes |= pRightNodeFile->GetAttributes();
		pNewChildFile->SetAttributes(attributes);

		pNewChildFile->SetLeftFileNode(pLeftNodeFile);
		pNewChildFile->SetRightFileNode(pRightNodeFile);
		pNewChildFile->SetFlags(SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT|SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT);
		pParent->AddChild(pNewChildFile);
	}
}

bool SyncTreeManager::SameNodeTypes(TreeNodeBase *pLeftNode,TreeNodeBase *pRightNode)
{
	TreeNodeFolder *pLeftNodeFolder = dynamic_cast<TreeNodeFolder *>(pLeftNode);
	TreeNodeFolder *pRightNodeFolder = dynamic_cast<TreeNodeFolder *>(pRightNode);
	// return true if they are both folders or both not folders
	return ((pLeftNodeFolder && pRightNodeFolder) || (!pLeftNodeFolder && !pRightNodeFolder));
}

int SyncTreeManager::CompareTimes(FILETIME leftTime,FILETIME rightTime)
{
	__int64 leftValue = *((__int64*)&leftTime);
	__int64 rightValue = *((__int64*)&rightTime);
	__int64 diff = leftValue-rightValue;
	__int64 threshold = 2*10000000; // 2 seconds * 10 million (100 nanoseconds periods)
	if (diff<-threshold)
	{
		return -1; // left is older than right
	}
	else if (diff>threshold)
	{
		return 1; // right is older then left
	}
	else
	{
		return 0; // left and right are within the threshold
	}
}

void SyncTreeManager::Synchronize(SyncFeedback &feedback,SyncEnable &syncEnable,unsigned long viewFlags)
{
	// there better be two databases
	if (!m_pLeftDB || !m_pRightDB)
		return;
	ASSERT(m_pSyncTreeRoot);

	TreeStats treeStats;
	GetTreeStats(treeStats,viewFlags);
	feedback.m_totalBytes = 0;
	if (syncEnable.m_bLRCopyEnable) feedback.m_totalBytes += treeStats.m_totalBytesCopiedLeftRight;
	if (syncEnable.m_bRLCopyEnable) feedback.m_totalBytes += treeStats.m_totalBytesCopiedRightLeft;
	feedback.m_totalBytesCopied = 0;

	TreeIterator first(m_pSyncTreeRoot);
	TreeIterator last(m_pSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SynchronizeNode(*first,feedback,syncEnable);
	}
	RemoveDeadNodes(m_pSyncTreeRoot);
}

void SyncTreeManager::SynchronizeNode(TreeNodeBase *pNode,SyncFeedback &feedback,SyncEnable &syncEnable)
{
	if (feedback.GetAbortFlag())
		return;

	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		int flags = pSyncNodeFolder->GetFlags();
		if (!(flags&SyncTreeNodeFlags::FLAGS_FROZEN) && (flags&SyncTreeNodeFlags::FLAGS_VISIBLE))
		{
			switch (flags&SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
					if (syncEnable.m_bLDeleteEnable)
					{
						if (!DeleteDirectory(pSyncNodeFolder,feedback,false))
							return; // we are in trouble due to an error
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
					if (syncEnable.m_bRDeleteEnable)
					{
						if (!DeleteDirectory(pSyncNodeFolder,feedback,true))
							return; // we are in trouble due to an error
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
					if (syncEnable.m_bLDeleteEnable)
					{
						if (!DeleteDirectory(pSyncNodeFolder,feedback,false))
							return; // we are in trouble due to an error
					}
					if (syncEnable.m_bRDeleteEnable)
					{
						if (!DeleteDirectory(pSyncNodeFolder,feedback,true))
							return; // we are in trouble due to an error
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
					{
						if (syncEnable.m_bLRCopyEnable)
						{
							if (!CreateDirectory(pSyncNodeFolder,feedback,true))
								return; // we are in trouble due to an error
						}
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
					{
						if (syncEnable.m_bRLCopyEnable)
						{
							if (!CreateDirectory(pSyncNodeFolder,feedback,false))
								return; // we are in trouble due to an error
						}
					}
					break;
			}
			// if at least one side is still there
			if (pSyncNodeFolder->GetLeftFolderNode() || pSyncNodeFolder->GetRightFolderNode())
			{
				TreeIterator first(pSyncNodeFolder);
				TreeIterator last(pSyncNodeFolder);
				last.End();
				// step through child containers
				for (;first<last;first.Increment())
				{
					SynchronizeNode(*first,feedback,syncEnable);
				}
			}
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();
		if (!(flags&SyncTreeNodeFlags::FLAGS_FROZEN) && (flags&SyncTreeNodeFlags::FLAGS_VISIBLE))
		{
			switch (flags&SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
					if (syncEnable.m_bLDeleteEnable)
					{
						bool bResult = m_pLeftDB->DeleteFileFromDB(pSyncNodeFile->GetLeftFileNode(),feedback,false);
						if (bResult)
						{
							pSyncNodeFile->SetLeftFileNode(NULL);
							RefreshSyncDates(pSyncNodeFile);
							SetStatusFlags(pSyncNodeFile,false);
							SetAction(pSyncNodeFile,ACTION_EXCLUDE,false,false);
						}
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
					if (syncEnable.m_bRDeleteEnable)
					{
						bool bResult = m_pRightDB->DeleteFileFromDB(pSyncNodeFile->GetRightFileNode(),feedback,true);
						if (bResult)
						{
							pSyncNodeFile->SetRightFileNode(NULL);
							RefreshSyncDates(pSyncNodeFile);
							SetStatusFlags(pSyncNodeFile,false);
							SetAction(pSyncNodeFile,ACTION_EXCLUDE,false,false);
						}
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
					if (syncEnable.m_bLDeleteEnable)
					{
						bool bResult = m_pLeftDB->DeleteFileFromDB(pSyncNodeFile->GetLeftFileNode(),feedback,false);
						if (bResult)
						{
							pSyncNodeFile->SetLeftFileNode(NULL);
							RefreshSyncDates(pSyncNodeFile);
							SetStatusFlags(pSyncNodeFile,false);
							SetAction(pSyncNodeFile,ACTION_EXCLUDE,false,false);
						}
					}
					if (syncEnable.m_bRDeleteEnable)
					{
						bool bResult = m_pRightDB->DeleteFileFromDB(pSyncNodeFile->GetRightFileNode(),feedback,true);
						if (bResult)
						{
							pSyncNodeFile->SetRightFileNode(NULL);
							RefreshSyncDates(pSyncNodeFile);
							SetStatusFlags(pSyncNodeFile,false);
							SetAction(pSyncNodeFile,ACTION_EXCLUDE,false,false);
						}
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
					{
						if (syncEnable.m_bLRCopyEnable)
						{
							SyncTreeNodeFolder *pDestFileParent = dynamic_cast<SyncTreeNodeFolder *>(pSyncNodeFile->GetParent());
							ASSERT(pDestFileParent);
							// if the right folder does not exist
							if (!pDestFileParent->GetRightFolderNode())
							{
								if (!CreateDirectory(pDestFileParent,feedback,true))
									return; // we are in trouble due to an error
							}
							int copyFlags = 0;
							if (m_bUseSyncDates)
								copyFlags |= SyncDBBase::FLAGS_UPDATE_DEST_TO_CURRENT_TIME;
							if (m_bVerifyCopiedFiles)
								copyFlags |= SyncDBBase::FLAGS_VERIFY_COPIED_FILE;

							TreeNodeFile *pNewFile = m_pRightDB->CopyFileToDB(m_pLeftDB,pSyncNodeFile->GetLeftFileNode(),pDestFileParent->GetRightFolderNode(),pSyncNodeFile->GetRightFileNode(),feedback,copyFlags,true);
							if (pNewFile)
							{
								pSyncNodeFile->SetRightFileNode(pNewFile);
								RefreshSyncDates(pSyncNodeFile);
								SetStatusFlags(pSyncNodeFile,false);
								SetPreviousFlag(pSyncNodeFile,true,false);
								SetAction(pSyncNodeFile,ACTION_EXCLUDE,false,false);
							}
						}
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
					{
						if (syncEnable.m_bRLCopyEnable)
						{
							SyncTreeNodeFolder *pDestFileParent = dynamic_cast<SyncTreeNodeFolder *>(pSyncNodeFile->GetParent());
							ASSERT(pDestFileParent);
							// if the right folder does not exist
							if (!pDestFileParent->GetLeftFolderNode())
							{
								CreateDirectory(pDestFileParent,feedback,false);
							}
							int copyFlags = 0;
							if (m_bUseSyncDates)
								copyFlags |= SyncDBBase::FLAGS_UPDATE_DEST_TO_CURRENT_TIME;
							if (m_bVerifyCopiedFiles)
								copyFlags |= SyncDBBase::FLAGS_VERIFY_COPIED_FILE;

							TreeNodeFile *pNewFile = m_pLeftDB->CopyFileToDB(m_pRightDB,pSyncNodeFile->GetRightFileNode(),pDestFileParent->GetLeftFolderNode(),pSyncNodeFile->GetLeftFileNode(),feedback,copyFlags,false);
							if (pNewFile)
							{
								pSyncNodeFile->SetLeftFileNode(pNewFile);
								RefreshSyncDates(pSyncNodeFile);
								SetStatusFlags(pSyncNodeFile,false);
								SetPreviousFlag(pSyncNodeFile,false,false);
								SetAction(pSyncNodeFile,ACTION_EXCLUDE,false,false);
							}
						}
					}
					break;
			}
		}
	}
}

bool SyncTreeManager::CreateDirectory(SyncTreeNodeFolder *pSyncNodeFolder,SyncFeedback &feedback,bool bRight)
{
	SyncTreeNodeFolder *pSyncNodeFolderParent = dynamic_cast<SyncTreeNodeFolder *>(pSyncNodeFolder->GetParent());
	ASSERT(pSyncNodeFolderParent);

	if (bRight)
	{
		TreeNodeFolder *pNodeFolder = pSyncNodeFolder->GetRightFolderNode();
		if (!pNodeFolder)
		{
			TreeNodeFolder *pNodeFolderParent = pSyncNodeFolderParent->GetRightFolderNode();
			if (!pNodeFolderParent)
			{
				// If I can't create my parent then we are in trouble
				if (!CreateDirectory(pSyncNodeFolderParent,feedback,bRight))
					return false;
			}
			pNodeFolderParent = pSyncNodeFolderParent->GetRightFolderNode();
			ASSERT(pNodeFolderParent);
			TreeNodeFolder *pNewNodeFolder = m_pRightDB->CreateDirectory(m_pLeftDB,pSyncNodeFolder->GetLeftFolderNode(),pNodeFolderParent,feedback,bRight);
			if (pNewNodeFolder)
			{
				pSyncNodeFolder->SetRightFolderNode(pNewNodeFolder);
				SetStatusFlags(pSyncNodeFolder,false);
				SetPreviousFlag(pSyncNodeFolder,bRight,false);
				SetAction(pSyncNodeFolder,ACTION_EXCLUDE,false,false);
			}
			else
			{
				return false; // we are in trouble
			}
		}
	}
	else
	{
		TreeNodeFolder *pNodeFolder = pSyncNodeFolder->GetLeftFolderNode();
		if (!pNodeFolder)
		{
			TreeNodeFolder *pNodeFolderParent = pSyncNodeFolderParent->GetLeftFolderNode();
			if (!pNodeFolderParent)
			{
				// If I can't create my parent then we are in trouble
				if (!CreateDirectory(pSyncNodeFolderParent,feedback,bRight))
					return false;
			}
			pNodeFolderParent = pSyncNodeFolderParent->GetLeftFolderNode();
			ASSERT(pNodeFolderParent);
			TreeNodeFolder *pNewNodeFolder = m_pLeftDB->CreateDirectory(m_pRightDB,pSyncNodeFolder->GetRightFolderNode(),pNodeFolderParent,feedback,bRight);
			if (pNewNodeFolder)
			{
				pSyncNodeFolder->SetLeftFolderNode(pNewNodeFolder);
				SetStatusFlags(pSyncNodeFolder,false);
				SetPreviousFlag(pSyncNodeFolder,bRight,false);
				SetAction(pSyncNodeFolder,ACTION_EXCLUDE,false,false);
			}
			else
			{
				return false; // we are in trouble
			}
		}
	}
	return true; // all is well
}

bool SyncTreeManager::DeleteDirectory(SyncTreeNodeFolder *pSyncNodeFolder,SyncFeedback &feedback,bool bRight)
{
	SyncTreeNodeFolder *pSyncNodeFolderParent = dynamic_cast<SyncTreeNodeFolder *>(pSyncNodeFolder->GetParent());
	ASSERT(pSyncNodeFolderParent);

	if (bRight)
	{
		TreeNodeFolder *pNodeFolder = pSyncNodeFolder->GetRightFolderNode();
		if (pNodeFolder)
		{
			bool bResult = m_pRightDB->DeleteDirectory(pNodeFolder,feedback,bRight);
			if (bResult)
			{
				DetatchFromDB(pSyncNodeFolder,bRight);
				SetStatusFlags(pSyncNodeFolder,true);
				SetAction(pSyncNodeFolder,ACTION_EXCLUDE,false,true);
			}
			else
			{
				return false; // we are in trouble
			}
		}
	}
	else
	{
		TreeNodeFolder *pNodeFolder = pSyncNodeFolder->GetLeftFolderNode();
		if (pNodeFolder)
		{
			bool bResult = m_pLeftDB->DeleteDirectory(pNodeFolder,feedback,bRight);
			if (bResult)
			{
				DetatchFromDB(pSyncNodeFolder,bRight);
				SetStatusFlags(pSyncNodeFolder,true);
				SetAction(pSyncNodeFolder,ACTION_EXCLUDE,false,true);
			}
			else
			{
				return false; // we are in trouble
			}
		}
	}
	return true; // all is well
}

// folders are less than files
// returns -1 if node1<node2
// returns 0 if node1==node2
// return 1 if node1>node2
int SyncTreeManager::CompareNodes(TreeNodeBase *pNode1,TreeNodeBase *pNode2)
{
	TreeNodeFolder *pNodeFolder1 = dynamic_cast<TreeNodeFolder *>(pNode1);
	TreeNodeFolder *pNodeFolder2 = dynamic_cast<TreeNodeFolder *>(pNode2);

	// if both files or folders
	if ((pNodeFolder1 && pNodeFolder2)||(!pNodeFolder1 && !pNodeFolder2))
	{
		return stricmp(pNode1->GetFileName(),pNode2->GetFileName());
	}
	// if node1 is a folder, node2 is a file
	else if (pNodeFolder1)
	{
		return -1;
	}
	// if node2 is a folder, node1 is a file
	else 
	{
		return 1;
	}
}

void SyncTreeManager::GetTreeStats(TreeStats &treeStats,unsigned long viewFlags)
{
	memset(&treeStats,0,sizeof(TreeStats));

	// there better be two databases
	if (!m_pLeftDB || !m_pRightDB)
		return;
	ASSERT(m_pSyncTreeRoot);
	ValidateActionsAndVisiblity(viewFlags);

	TreeIterator first(m_pSyncTreeRoot);
	TreeIterator last(m_pSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		GetTreeStats(*first,treeStats);
	}
}

void SyncTreeManager::GetTreeStats(TreeNodeBase *pNode,TreeStats &treeStats)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		int flags = pSyncNodeFolder->GetFlags();
		if (!(flags&SyncTreeNodeFlags::FLAGS_FROZEN) && (flags & SyncTreeNodeFlags::FLAGS_VISIBLE))
		{
			TreeNodeFolder *pLeftFolderNode = pSyncNodeFolder->GetLeftFolderNode();
			TreeNodeFolder *pRightFolderNode = pSyncNodeFolder->GetRightFolderNode();
			if (pLeftFolderNode)
			{
				treeStats.m_foldersLeft++;
			}
			if (pRightFolderNode)
			{
				treeStats.m_foldersRight++;
			}

			switch (flags&SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
					treeStats.m_filesFoldersDeletedLeft++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
					treeStats.m_filesFoldersDeletedRight++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
					treeStats.m_filesFoldersDeletedLeft++;
					treeStats.m_filesFoldersDeletedRight++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
					treeStats.m_filesFoldersCopiedLeftRight++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
					treeStats.m_filesFoldersCopiedRightLeft++;
					break;
			}
		}
		TreeIterator first(pSyncNodeFolder);
		TreeIterator last(pSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			GetTreeStats(*first,treeStats);
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();
		if (!(flags&SyncTreeNodeFlags::FLAGS_FROZEN) && (flags & SyncTreeNodeFlags::FLAGS_VISIBLE))
		{
			TreeNodeFile *pLeftFileNode = pSyncNodeFile->GetLeftFileNode();
			TreeNodeFile *pRightFileNode = pSyncNodeFile->GetRightFileNode();
			if (pLeftFileNode)
			{
				treeStats.m_filesLeft++;
				treeStats.m_totalBytesLeft+=pLeftFileNode->GetFileLength();
			}
			if (pRightFileNode)
			{
				treeStats.m_filesRight++;
				treeStats.m_totalBytesRight+=pRightFileNode->GetFileLength();
			}

			switch (flags&SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
					treeStats.m_filesFoldersDeletedLeft++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
					treeStats.m_filesFoldersDeletedRight++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
					treeStats.m_filesFoldersDeletedLeft++;
					treeStats.m_filesFoldersDeletedRight++;
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
					treeStats.m_filesFoldersCopiedLeftRight++;
					treeStats.m_totalBytesCopiedLeftRight+=pLeftFileNode->GetFileLength();
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
					treeStats.m_filesFoldersCopiedRightLeft++;
					treeStats.m_totalBytesCopiedRightLeft+=pRightFileNode->GetFileLength();
					break;
			}
		}
	}
}

void SyncTreeManager::SetDefaultActions(bool bResetManualFlags)
{
	// there better be two databases
	if (!m_pLeftDB || !m_pRightDB)
		return;

	ASSERT(m_pSyncTreeRoot);

	TreeIterator first(m_pSyncTreeRoot);
	TreeIterator last(m_pSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SetDefaultActions(*first,bResetManualFlags,true,false);
	}
}

// returns true if frozen
bool SyncTreeManager::SetDefaultActions(TreeNodeBase *pNode,bool bResetManualFlags,bool bDoChildren,bool bFrozen)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		if (m_pLoadFeedback)
		{
			if (!m_pLoadFeedback->Update(pSyncNodeFolder->GetFileName()))
				return false;
		}

		int flags = pSyncNodeFolder->GetFlags();
		// set bit if reseting manual flags
		if (bResetManualFlags)
			flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET;

		// if not manual set
		if (!(flags & SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET))
		{
			// mask off the action
			flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MASK;

			flags |= GetDefaultAction(pSyncNodeFolder);
			if (bFrozen)
			{
				flags |= SyncTreeNodeFlags::FLAGS_FROZEN;
			}
			else if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
			{
				bFrozen = true; // my children are now frozen
			}
		}

		pSyncNodeFolder->SetFlags(flags);
		if (bDoChildren)
		{
			TreeIterator first(pSyncNodeFolder);
			TreeIterator last(pSyncNodeFolder);
			last.End();
			// step through child containers
			for (;first<last;first.Increment())
			{
				SetDefaultActions(*first,bResetManualFlags,bDoChildren,bFrozen);
			}
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		if (m_pLoadFeedback)
		{
			if (!m_pLoadFeedback->Update(pSyncNodeFile->GetFileName()))
				return false;
		}

		int flags = pSyncNodeFile->GetFlags();
		// set bit if reseting manual flags
		if (bResetManualFlags)
			flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET;

		// if not manual set
		if (!(flags & SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET))
		{
			// mask off the action
			flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MASK;

			flags |= GetDefaultAction(pSyncNodeFile);
			if (bFrozen)
			{
				flags |= SyncTreeNodeFlags::FLAGS_FROZEN;
			}
			else if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
			{
				bFrozen = true; // my children are now frozen
			}
		}
		pSyncNodeFile->SetFlags(flags);
	}
	return bFrozen;
}

static int DefaultActionFlagsLeftNewer[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_2_WAY
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_REFRESH_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_UPDATE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_FORCE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_FORCE_RIGHT
};

static int DefaultActionFlagsRightNewer[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_2_WAY
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_REFRESH_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_UPDATE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_FORCE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_FORCE_RIGHT
};

static int DefaultActionFlagsLeftOnlyNew[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_2_WAY
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_AUGMENT_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_UPDATE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT,				// SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT,				// SYNC_STYLE_FORCE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_FORCE_RIGHT
};

static int DefaultActionFlagsLeftOnlyOld[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT,				// SYNC_STYLE_2_WAY
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_AUGMENT_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_UPDATE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT,				// SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT,				// SYNC_STYLE_FORCE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT,	// SYNC_STYLE_FORCE_RIGHT
};

static int DefaultActionFlagsRightOnlyNew[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_2_WAY
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_AUGMENT_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_UPDATE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT,			// SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_FORCE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT,			// SYNC_STYLE_FORCE_RIGHT
};

static int DefaultActionFlagsRightOnlyOld[SyncTreeManager::SYNC_STYLE_NUM_STYLES] = 
{
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT,				// SYNC_STYLE_2_WAY
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_AUGMENT_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_AUGMENT_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_REFRESH_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_UPDATE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED,					// SYNC_STYLE_UPDATE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_UPDATE_AND_PRUNE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT,			// SYNC_STYLE_UPDATE_AND_PRUNE_RIGHT
	SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT,	// SYNC_STYLE_FORCE_LEFT
	SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT,			// SYNC_STYLE_FORCE_RIGHT
};


int SyncTreeManager::GetDefaultAction(TreeNodeBase *pNode) const
{
	int flags = 0;
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		flags = pSyncNodeFolder->GetFlags();

		// mask off the action
		flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MASK;

		// set the action based on the status
		switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
				flags |= SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				// if last time it was in both panes then try to delete
				if ((flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT) &&
					(flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT))
				{
					flags |= DefaultActionFlagsLeftOnlyOld[m_syncStyle];
				}
				else
				{
					flags |= DefaultActionFlagsLeftOnlyNew[m_syncStyle];
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				// if last time it was in both panes then try to delete
				if ((flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT) &&
					(flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT))
				{
					flags |= DefaultActionFlagsRightOnlyOld[m_syncStyle];
				}
				else
				{
					flags |= DefaultActionFlagsRightOnlyNew[m_syncStyle];
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				flags |= SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
				break;
		}

		// clear auto freeze flags
		flags &= ~(SyncTreeNodeFlags::FLAGS_NAME_PREFIX	| SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH);
		if (m_pAutoExclude)
		{
			if (m_pAutoExclude->CheckFolder(pSyncNodeFolder->GetFileName()) & AutoExclude::FLAGS_PREFIX)
				flags |= SyncTreeNodeFlags::FLAGS_NAME_PREFIX; // name has a prefix

			TupString path;
			pSyncNodeFolder->GetPath(path);
			if (m_pAutoExclude->CheckPath(path)!=-1)
			{
				flags |= SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH; // name is in the list
			}
		}

		if ((pSyncNodeFolder->GetAttributes() & (TreeNodeBase::ATTRIBUTE_HIDDEN|TreeNodeBase::ATTRIBUTE_SYSTEM)) ||
			 (flags & SyncTreeNodeFlags::FLAGS_NAME_PREFIX) || 
			 (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH))
		{
			flags |= SyncTreeNodeFlags::FLAGS_FROZEN;
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		flags = pSyncNodeFile->GetFlags();

		// mask off the action
		flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MASK;

		// set the action based on the status
		switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
				flags |= SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				// if last time it was in both panes then try to delete
				if ((flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT) &&
					(flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT))
				{
					flags |= DefaultActionFlagsLeftOnlyOld[m_syncStyle];
				}
				else
				{
					flags |= DefaultActionFlagsLeftOnlyNew[m_syncStyle];
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				// if last time it was in both panes then try to delete
				if ((flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT) &&
					(flags & SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT))
				{
					flags |= DefaultActionFlagsRightOnlyOld[m_syncStyle];
				}
				else
				{
					flags |= DefaultActionFlagsRightOnlyNew[m_syncStyle];
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				flags |= SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
				{
					if (m_bCheckingCRC && CheckFileCRC(pSyncNodeFile))
					{
						flags |= SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
					}
					else
					{
						flags |= DefaultActionFlagsLeftNewer[m_syncStyle];
					}
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
				{
					if (m_bCheckingCRC && CheckFileCRC(pSyncNodeFile))
					{
						flags |= SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
					}
					else
					{
						flags |= DefaultActionFlagsRightNewer[m_syncStyle];
					}
				}
				break;
		}

		flags &= ~(SyncTreeNodeFlags::FLAGS_NAME_PREFIX	| SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH);
		int autoExcludeFlags = 0;
		if (m_pAutoExclude)
		{
			autoExcludeFlags = m_pAutoExclude->CheckFile(pSyncNodeFile->GetFileName());
			if (autoExcludeFlags & AutoExclude::FLAGS_PREFIX)
				flags |= SyncTreeNodeFlags::FLAGS_NAME_PREFIX; // name has a prefix

			TupString path;
			pSyncNodeFile->GetPath(path);
			if (m_pAutoExclude->CheckPath(path)!=-1)
			{
				flags |= SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH; // name is in the list
			}
		}

		if ((pSyncNodeFile->GetAttributes() & (TreeNodeBase::ATTRIBUTE_HIDDEN|TreeNodeBase::ATTRIBUTE_SYSTEM)) ||
			autoExcludeFlags || 
			(flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH))
		{
			flags |= SyncTreeNodeFlags::FLAGS_FROZEN;
		}
	}
	return flags;
}


void SyncTreeManager::SetStatusFlags(void)
{
	// there better be two databases
	if (!m_pLeftDB || !m_pRightDB)
		return;

	ASSERT(m_pSyncTreeRoot);

	TreeIterator first(m_pSyncTreeRoot);
	TreeIterator last(m_pSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		SetStatusFlags(*first,true);
	}
}

void SyncTreeManager::SetStatusFlags(TreeNodeBase *pNode,bool bDoChildren)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		int flags = pSyncNodeFolder->GetFlags();
		flags &= ~SyncTreeNodeFlags::FLAGS_STATUS_MASK;

		TreeNodeFolder *pLeftNodeFolder = pSyncNodeFolder->GetLeftFolderNode();
		TreeNodeFolder *pRightNodeFolder = pSyncNodeFolder->GetRightFolderNode();
		if (pLeftNodeFolder && pRightNodeFolder)
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL;
		}
		else if (pLeftNodeFolder)
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY;
		}
		else if (pRightNodeFolder)
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY;
		}
		else
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE;
		}
		pSyncNodeFolder->SetFlags(flags);
		if (bDoChildren)
		{
			TreeIterator first(pSyncNodeFolder);
			TreeIterator last(pSyncNodeFolder);
			last.End();
			// step through child containers
			for (;first<last;first.Increment())
			{
				SetStatusFlags(*first,bDoChildren);
			}
		}
	}
	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();

		flags &= ~SyncTreeNodeFlags::FLAGS_STATUS_MASK;
		TreeNodeFile *pLeftNodeFile = pSyncNodeFile->GetLeftFileNode();
		TreeNodeFile *pRightNodeFile = pSyncNodeFile->GetRightFileNode();
		if (pLeftNodeFile && pRightNodeFile)
		{
 			int compareCurrentFileTimes = CompareTimes(pLeftNodeFile->GetModifiedTime(),pRightNodeFile->GetModifiedTime());

			// if we care about sync dates and the node has them
			if (m_bUseSyncDates && (pSyncNodeFile->GetFlags() & SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE))
			{
				int compareLeftTimeToSync = CompareTimes(pLeftNodeFile->GetModifiedTime(),pSyncNodeFile->GetLeftSyncDate());
				int compareRightTimeToSync = CompareTimes(pRightNodeFile->GetModifiedTime(),pSyncNodeFile->GetRightSyncDate());
				// if left file has not changed
				if (compareLeftTimeToSync == 0)
				{
					// if right file has not changed
					if (compareRightTimeToSync == 0)
					{
						compareCurrentFileTimes = 0; // make them equal even though the modified times may be different
					}
					else
					{
						compareCurrentFileTimes = -1; // make the right newer
					}
				}
				else
				{
					// if right file has not changed
					if (compareRightTimeToSync == 0)
					{
						compareCurrentFileTimes = 1; // make the left newer
					}
					// if both are changed we may have a problem...
					// for now we use the latest modified time
					// (leave compareCurrentFileTimes alone)
				}
			}

			// if their times are equal
			if (compareCurrentFileTimes==0)
			{
				flags |= SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL;
			}
			else if (compareCurrentFileTimes<0)
			{
				flags |= SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER;
			}
			else // if (compareCurrentFileTimes>0)
			{
				flags |= SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER;
			}
		}
		else if (pLeftNodeFile)
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY;
		}
		else if (pRightNodeFile)
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY;
		}
		else
		{
			flags |= SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE;
		}
		pSyncNodeFile->SetFlags(flags);
	}
}

void SyncTreeManager::ClearPreviousFlag(TreeNodeBase *pNode,bool bRight,bool bDoChildren)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		int flags = pSyncNodeFolder->GetFlags();
		if (bRight)
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT;
		}
		else
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT;
		}
		pSyncNodeFolder->SetFlags(flags);
		if (bDoChildren)
		{
			TreeIterator first(pSyncNodeFolder);
			TreeIterator last(pSyncNodeFolder);
			last.End();
			// step through child containers
			for (;first<last;first.Increment())
			{
				ClearPreviousFlag(*first,bRight,bDoChildren);
			}
		}
	}
	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();
		if (bRight)
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT;
		}
		else
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT;
		}
		pSyncNodeFile->SetFlags(flags);
	}
}

void SyncTreeManager::SetPreviousFlag(TreeNodeBase *pNode,bool bRight,bool bDoChildren)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		int flags = pSyncNodeFolder->GetFlags();
		if (bRight)
		{
			flags |= SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT;
		}
		else
		{
			flags |= SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT;
		}
		pSyncNodeFolder->SetFlags(flags);
		if (bDoChildren)
		{
			TreeIterator first(pSyncNodeFolder);
			TreeIterator last(pSyncNodeFolder);
			last.End();
			// step through child containers
			for (;first<last;first.Increment())
			{
				SetPreviousFlag(*first,bRight,bDoChildren);
			}
		}
	}
	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();
		if (bRight)
		{
			flags |= SyncTreeNodeFlags::FLAGS_PREVIOUS_RIGHT;
		}
		else
		{
			flags |= SyncTreeNodeFlags::FLAGS_PREVIOUS_LEFT;
		}
		pSyncNodeFile->SetFlags(flags);
	}
}

void SyncTreeManager::RefreshSyncDates(SyncTreeNodeFile *pFile)
{
	TreeNodeFile *pLeftFileNode = pFile->GetLeftFileNode();
	TreeNodeFile *pRightFileNode = pFile->GetRightFileNode();
	if (pLeftFileNode && pRightFileNode)
	{
		pFile->SetLeftSyncDate(pLeftFileNode->GetModifiedTime());
		pFile->SetRightSyncDate(pRightFileNode->GetModifiedTime());
		pFile->SetFlags(pFile->GetFlags() | SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE);
	}
	else
	{
		pFile->SetFlags(pFile->GetFlags() & ~SyncTreeNodeFlags::FLAGS_HAS_SYNC_DATE);
	}
}

void SyncTreeManager::SetAction(TreeNodeBase *pNode,Actions action,bool bManualSet,bool bDoChildren,bool bFrozen,int nodeDepth)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		int flags = pSyncNodeFolder->GetFlags();

		bool bValidFrozenAction = (action==ACTION_UNFREEZE ||	
											action==ACTION_AUTO_FREEZE_ADD ||
											action==ACTION_AUTO_FREEZE_REMOVE ||
											action==ACTION_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN);


		// do only when visible and not frozen or if we are trying to unfreeze it
		if ((flags & SyncTreeNodeFlags::FLAGS_VISIBLE) && (!(flags & SyncTreeNodeFlags::FLAGS_FROZEN) || bValidFrozenAction ))
		{
			if (bManualSet)
			{
				flags |= SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET;
			}
			else
			{
				flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET;
			}

			switch (action)
			{
				case ACTION_EXCLUDE:
					flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
							 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
					break;
				case ACTION_DELETE_LEFT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT;
							break;
					}
					break;
				case ACTION_DELETE_RIGHT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT;
							break;
					}
					break;
				case ACTION_DELETE_BOTH:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH;
							break;
					}
					break;
				case ACTION_COPY_LEFT_TO_RIGHT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
							break;
					}
					break;
				case ACTION_COPY_RIGHT_TO_LEFT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
							break;
					}
					break;
				case ACTION_FREEZE:
					flags |= SyncTreeNodeFlags::FLAGS_FROZEN;
					break;
				case ACTION_UNFREEZE:
					flags &= ~SyncTreeNodeFlags::FLAGS_FROZEN;
					break;
				case ACTION_DEFAULT_ACTION:
					bFrozen = SetDefaultActions(pSyncNodeFolder,false,false,bFrozen);
					flags = pSyncNodeFolder->GetFlags();
					break;
				case ACTION_AUTO_FREEZE_ADD:
					if (nodeDepth==0 && m_pAutoExclude)
					{
						TupString path;
						pSyncNodeFolder->GetPath(path);
						if (m_pAutoExclude->CheckPath(path)==-1)
						{
							m_pAutoExclude->AddPath(path);
						}
						flags |= SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH;
					}
					break;
				case ACTION_AUTO_FREEZE_REMOVE:
					if (nodeDepth==0 && m_pAutoExclude)
					{
						TupString path;
						pSyncNodeFolder->GetPath(path);
						if (m_pAutoExclude->CheckPath(path)!=-1)
						{
							m_pAutoExclude->RemovePath(path);
						}
						flags &= ~SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH;
					}
					break;
				case ACTION_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN:
					if (m_pAutoExclude)
					{
						TupString path;
						pSyncNodeFolder->GetPath(path);
						if (m_pAutoExclude->CheckPath(path)!=-1)
						{
							m_pAutoExclude->RemovePath(path);
						}
						flags &= ~SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH;
					}
					break;
			}
			pSyncNodeFolder->SetFlags(flags);
			if (bDoChildren)
			{
				TreeIterator first(pSyncNodeFolder);
				TreeIterator last(pSyncNodeFolder);
				last.End();
				// step through child containers
				for (;first<last;first.Increment())
				{
					SetAction(*first,action,bManualSet,bDoChildren,bFrozen,nodeDepth+1);
				}
			}
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();

		bool bValidFrozenAction = (action==ACTION_UNFREEZE ||	
											action==ACTION_AUTO_FREEZE_ADD ||
											action==ACTION_AUTO_FREEZE_REMOVE);
		
		// do only when visible and included or if we are trying to include it
		if ((flags & SyncTreeNodeFlags::FLAGS_VISIBLE) && (!(flags & SyncTreeNodeFlags::FLAGS_FROZEN) || bValidFrozenAction))
		{
			if (bManualSet)
			{
				flags |= SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET;
			}
			else
			{
				flags &= ~SyncTreeNodeFlags::FLAGS_ACTION_MANUAL_SET;
			}
			switch (action)
			{
				case ACTION_EXCLUDE:
					flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
							 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
					break;
				case ACTION_DELETE_LEFT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT;
							break;
					}
					break;
				case ACTION_DELETE_RIGHT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT;
							break;
					}
					break;
				case ACTION_DELETE_BOTH:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT;
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH;
							break;
					}
					break;
				case ACTION_COPY_LEFT_TO_RIGHT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT;
							break;
					}
					break;
				case ACTION_COPY_RIGHT_TO_LEFT:
					// set the action based on the status
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK)|
									 SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT;
							break;
					}
					break;
				case ACTION_FREEZE:
					flags |= SyncTreeNodeFlags::FLAGS_FROZEN;
					break;
				case ACTION_UNFREEZE:
					flags &= ~SyncTreeNodeFlags::FLAGS_FROZEN;
					SetAction(pSyncNodeFile->GetParent(),action,bManualSet,false);
					break;
				case ACTION_DEFAULT_ACTION:
					SetDefaultActions(pSyncNodeFile,false,false,bFrozen);
					flags = pSyncNodeFile->GetFlags();
					break;
				case ACTION_AUTO_FREEZE_ADD:
					if (nodeDepth==0 && m_pAutoExclude)
					{
						TupString path;
						pSyncNodeFile->GetPath(path);
						if (m_pAutoExclude->CheckPath(path)==-1)
						{
							m_pAutoExclude->AddPath(path);
						}
						flags |= SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH;
					}
					break;
				case ACTION_AUTO_FREEZE_REMOVE:
					if (nodeDepth==0 && m_pAutoExclude)
					{
						TupString path;
						pSyncNodeFile->GetPath(path);
						if (m_pAutoExclude->CheckPath(path)!=-1)
						{
							m_pAutoExclude->RemovePath(path);
						}
						flags &= ~SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH;
					}
					break;
				case ACTION_AUTO_FREEZE_REMOVE_INCLUDE_CHILDREN:
					if (m_pAutoExclude)
					{
						TupString path;
						pSyncNodeFile->GetPath(path);
						if (m_pAutoExclude->CheckPath(path)!=-1)
						{
							m_pAutoExclude->RemovePath(path);
						}
						flags &= ~SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH;
					}
					break;
			}
			pSyncNodeFile->SetFlags(flags);
		}
	}
}

void SyncTreeManager::DetatchFromDB(TreeNodeBase *pNode,bool bRight)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		if (bRight)
		{
			pSyncNodeFolder->SetRightFolderNode(NULL);
		}
		else
		{
			pSyncNodeFolder->SetLeftFolderNode(NULL);
		}
		TreeIterator first(pSyncNodeFolder);
		TreeIterator last(pSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			DetatchFromDB(*first,bRight);
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		if (bRight)
		{
			pSyncNodeFile->SetRightFileNode(NULL);
		}
		else
		{
			pSyncNodeFile->SetLeftFileNode(NULL);
		}
	}
}

void SyncTreeManager::RemoveDeadNodes(TreeNodeBase *pNode)
{
	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		// if both nodes are null then we are gone
		if (!pSyncNodeFolder->GetLeftFolderNode() && !pSyncNodeFolder->GetRightFolderNode())
		{
			delete pNode;
			return;
		}
		TreeNodeBase *pChild = pSyncNodeFolder->GetFirstChildNode();
		// don't use an iterator here since the child may be deleted
		while (pChild)
		{
			TreeNodeBase *pNextChild = pChild->GetNext();
			RemoveDeadNodes(pChild);
			pChild = pNextChild;
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		// if both nodes are null then we are gone
		if (!pSyncNodeFile->GetLeftFileNode() && !pSyncNodeFile->GetRightFileNode())
		{
			delete pNode;
			return;
		}
	}
}

void SyncTreeManager::ValidateActionsAndVisiblity(unsigned long viewFlags)
{
	// there better be two databases
	if (!m_pLeftDB || !m_pRightDB)
		return;
	ASSERT(m_pSyncTreeRoot);

	TreeIterator first(m_pSyncTreeRoot);
	TreeIterator last(m_pSyncTreeRoot);
	last.End();
	// step through child containers
	for (;first<last;first.Increment())
	{
		ValidateActionsAndVisiblity(*first,viewFlags);
	}
}

unsigned long SyncTreeManager::ValidateActionsAndVisiblity(TreeNodeBase *pNode,unsigned long viewFlags)
{
	// this is true if all the status view flags are down
	bool bViewFlagsAllStatus = ((viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_STATUS) == SyncTreeNodeFlags::VIEW_FLAGS_STATUS);

	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		unsigned long childActionStatusFlags = 0;
		TreeIterator first(pSyncNodeFolder);
		TreeIterator last(pSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			childActionStatusFlags |= ValidateActionsAndVisiblity(*first,viewFlags);
		}
		unsigned long flags = pSyncNodeFolder->GetFlags();
		// update child has actions 
		if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__LEFT_ONLY |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__RIGHT_ONLY |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY |
											  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH))
			flags |= SyncTreeNodeFlags::FLAGS_CHILD_HAS_ACTION;
		else
			flags &= ~SyncTreeNodeFlags::FLAGS_CHILD_HAS_ACTION;

		// validate actions
		switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
		{
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
				switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
				{
					case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
						// if a child wants to be copied to the right then we need to as well
						if (childActionStatusFlags & SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY)
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT;
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
						// if all children are not going to be deleted then we cannot delete
						if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH))
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
						// this is ok
						break;
					default:
						ASSERT(FALSE);			
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
				switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
				{
					case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
						// if a child wants to be copied to the left then we need to as well
						if (childActionStatusFlags & SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY)
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT;
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
						// if all children are not going to be deleted then we cannot delete
						if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH))
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
						// this is ok
						break;
					default:
						ASSERT(FALSE);			
				}
				break;
			case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
				{
					case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
						// this is ok
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
						// if all children are not going to be deleted then we cannot delete
						if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH))
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
						// if all children are not going to be deleted then we cannot delete
						if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH))
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
						break;
					case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
						// if all children are not going to be deleted then we cannot delete
						if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__RIGHT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__LEFT_ONLY |
															  SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH))
							flags = (flags & ~SyncTreeNodeFlags::FLAGS_ACTION_MASK) | SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED;
						break;
					default:
						ASSERT(FALSE);			
				}
				break;
			default:
				ASSERT(FALSE);			
		}
		// If I am trying to be frozen
		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		{
			// if any of my children are doing any action (including excluded) then I can't be frozen
			if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__LEFT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__RIGHT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH))
				flags &= ~SyncTreeNodeFlags::FLAGS_FROZEN;
		}

		// start with what the kids are doing
		unsigned long myActionStatusFlags = childActionStatusFlags;
		bool bVisible = false;

		// set my action status flags as well as if I am visible

		// if I am still frozen
		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		{
			if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN)
			{
				switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
				{
					case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
						myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__LEFT_ONLY;
						break;
					case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
						myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__RIGHT_ONLY;
						break;
					case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
						myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH;
						break;
				}

				bVisible = true;
			}
		}
		// if I am not frozen
		else
		{
			// if any of my children are visible then I must be visible
			if (childActionStatusFlags & (SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__LEFT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__RIGHT_ONLY |
													SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__BOTH))
				bVisible = true;

			switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__LEFT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__RIGHT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_BOTH |
												 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;

						default:
							ASSERT(FALSE);			
					}
					break;
				default:
					ASSERT(FALSE);			
			}
		}
		if (bVisible)
		{
			flags |= SyncTreeNodeFlags::FLAGS_VISIBLE;

			switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
					myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__LEFT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
					myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__RIGHT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
					myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__BOTH;
					break;
			}
		}
		else
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_VISIBLE;

			switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
					myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__LEFT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
					myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__RIGHT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
					myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH;
					break;
			}
		}
		pSyncNodeFolder->SetFlags(flags);
		return myActionStatusFlags;
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		unsigned long myActionStatusFlags = 0;
		bool bVisible = false;
		unsigned long flags = pSyncNodeFile->GetFlags();

		// if I am excluded
		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		{
			if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_FROZEN)
			{
				switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
				{
					case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
						myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__LEFT_ONLY;
						break;
					case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
						myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__RIGHT_ONLY;
						break;
					case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
					case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
					case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
						myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_FROZEN__BOTH;
						break;
				}
				bVisible = true;
			}
		}
		// if I am not frozen
		else
		{
			// set my action status flags as well as if I am visible
			switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__LEFT_ONLY;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__LEFT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__LEFT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_ONLY))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__RIGHT_ONLY;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__RIGHT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__RIGHT_ONLY;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_ONLY))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_BOTH |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_RIGHT_EQUAL))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_BOTH |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_LEFT_NEWER))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
					switch (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_EXCLUDED__BOTH;
							if (bViewFlagsAllStatus)
							{
								if (viewFlags & SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE)
									bVisible = true;
							}
							else
							{
								if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER |
													 SyncTreeNodeFlags::VIEW_FLAGS_ACTION_EXCLUDE))
									bVisible = true;
							}
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_DELETE_BOTH__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_DELETE_BOTH |
												SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_LEFT_TO_RIGHT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_LEFT_TO_RIGHT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER))
								bVisible = true;
							break;
						case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
							myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_COPY_RIGHT_TO_LEFT__BOTH;
							if (viewFlags & (SyncTreeNodeFlags::VIEW_FLAGS_ACTION_COPY_RIGHT_TO_LEFT |
											 SyncTreeNodeFlags::VIEW_FLAGS_STATUS_RIGHT_NEWER))
								bVisible = true;
							break;
						default:
							ASSERT(FALSE);			
					}
					break;
				default:
					ASSERT(FALSE);			
			}
		}
		if (bVisible)
		{
			flags |= SyncTreeNodeFlags::FLAGS_VISIBLE;
			switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
					myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__LEFT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
					myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__RIGHT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
					myActionStatusFlags |= SyncTreeNodeFlags::FLAGS_ACTION_STATUS_VISIBLE__BOTH;
					break;
			}
		}
		else
		{
			flags &= ~SyncTreeNodeFlags::FLAGS_VISIBLE;
			switch (flags & SyncTreeNodeFlags::FLAGS_STATUS_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
					myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__LEFT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
					myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__RIGHT_ONLY;
					break;
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
				case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
				case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
					myActionStatusFlags = SyncTreeNodeFlags::FLAGS_ACTION_STATUS_HIDDEN__BOTH;
					break;
			}
		}
		pSyncNodeFile->SetFlags(flags);
		return myActionStatusFlags;
	}
	ASSERT(FALSE);
	return 0;
}


unsigned long SyncTreeManager::GetValidActionFlags(TreeNodeBase *pNode,bool bLeftCtrl,bool bDoChildren,int nodeDepth)
{
	unsigned long validActionFlags = 0;

	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		TreeNodeFolder *pCtrlFolder = bLeftCtrl ? pSyncNodeFolder->GetLeftFolderNode() : pSyncNodeFolder->GetRightFolderNode();
		validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPAND_ALL|
							 SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COLLAPSE_ALL);

		int flags = pSyncNodeFolder->GetFlags();

		// only if we are the ones selected
		if (nodeDepth==0)
		{
			if (pCtrlFolder)
			{
				if (flags & SyncTreeNodeFlags::FLAGS_NAME_PREFIX)
				{
					validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_PREFIX;
				}
				else if (m_pAutoExclude->GetNumFolderPrefixes()>0)
				{
					validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_PREFIX;
				}
			}
			if (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE;
			}
			else
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_AUTO_FREEZE;
			}
		}
		else
		{
			if (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE_INCLUDE_CHILDREN;
			}
		}

		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		{
			validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE;
			if (bDoChildren)
			{
				TreeIterator first(pSyncNodeFolder);
				TreeIterator last(pSyncNodeFolder);
				last.End();
				// step through child containers
				for (;first<last;first.Increment())
				{
					validActionFlags |= GetValidActionFlagsFrozenParent(*first,bLeftCtrl,true,nodeDepth+1);
				}
			}
		}
		else
		{
			// if not already doing the default then we are ok to allow the set default action
			int defaultActionFlags = GetDefaultAction(pSyncNodeFolder);
			if (((defaultActionFlags & SyncTreeNodeFlags::FLAGS_ACTION_MASK) != (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK))||
				(defaultActionFlags & SyncTreeNodeFlags::FLAGS_FROZEN))
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SET_DEFAULT;
			}

			switch (flags&SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
					}
					break;
			}
			if (bDoChildren)
			{
				TreeIterator first(pSyncNodeFolder);
				TreeIterator last(pSyncNodeFolder);
				last.End();
				// step through child containers
				for (;first<last;first.Increment())
				{
					validActionFlags |= GetValidActionFlags(*first,bLeftCtrl,true,nodeDepth+1);
				}
			}
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		TreeNodeFile *pCtrlFile = bLeftCtrl ? pSyncNodeFile->GetLeftFileNode() : pSyncNodeFile->GetRightFileNode();

		int flags = pSyncNodeFile->GetFlags();

		if (nodeDepth==0)
		{
			if (pCtrlFile)
			{
				if (flags & SyncTreeNodeFlags::FLAGS_NAME_PREFIX)
				{
					validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_PREFIX;
				}
				else if (m_pAutoExclude->GetNumFolderPrefixes()>0)
				{
					validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_PREFIX;
				}
			}
			if (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE;
			}
			else
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_ADD_AUTO_FREEZE;
			}
		}
		else
		{
			if (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE_INCLUDE_CHILDREN;
			}
		}

		if (flags & SyncTreeNodeFlags::FLAGS_FROZEN)
		{
			validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_UNFREEZE;
		}
		else
		{
			validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_FREEZE;

			// if not already doing the default then we are ok to allow the set default action
			int defaultActionFlags = GetDefaultAction(pSyncNodeFile);
			if (((defaultActionFlags & SyncTreeNodeFlags::FLAGS_ACTION_MASK) != (flags & SyncTreeNodeFlags::FLAGS_ACTION_MASK))||
				(defaultActionFlags & SyncTreeNodeFlags::FLAGS_FROZEN))
			{
				validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_SET_DEFAULT;
			}

			switch (flags&SyncTreeNodeFlags::FLAGS_ACTION_MASK)
			{
				case SyncTreeNodeFlags::FLAGS_ACTION_EXCLUDED:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_LEFT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_RIGHT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_DELETE_BOTH:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_LEFT_TO_RIGHT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
					}
					break;
				case SyncTreeNodeFlags::FLAGS_ACTION_COPY_RIGHT_TO_LEFT:
					switch (flags&SyncTreeNodeFlags::FLAGS_STATUS_MASK)
					{
						case SyncTreeNodeFlags::FLAGS_STATUS_INACTIVE:
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_RIGHT_TO_LEFT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_ONLY:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_RIGHT_EQUAL:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_LEFT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
						case SyncTreeNodeFlags::FLAGS_STATUS_RIGHT_NEWER:
							validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXCLUDE|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_LEFT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_RIGHT|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_DELETE_BOTH|
												SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COPY_LEFT_TO_RIGHT);
							break;
					}
			}
		}
	}
	return validActionFlags;
}

unsigned long SyncTreeManager::GetValidActionFlagsFrozenParent(TreeNodeBase *pNode,bool bLeftCtrl,bool bDoChildren,int nodeDepth)
{
	unsigned long validActionFlags = 0;

	// check and see if this node is a sync folder
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		validActionFlags |= (SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_EXPAND_ALL|
							 SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_COLLAPSE_ALL);

		int flags = pSyncNodeFolder->GetFlags();

		if (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
		{
			validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE_INCLUDE_CHILDREN;
		}
		if (bDoChildren)
		{
			TreeIterator first(pSyncNodeFolder);
			TreeIterator last(pSyncNodeFolder);
			last.End();
			// step through child containers
			for (;first<last;first.Increment())
			{
				validActionFlags |= GetValidActionFlagsFrozenParent(*first,bLeftCtrl,true,nodeDepth+1);
			}
		}
	}

	// check and see if this node is a sync file
	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		int flags = pSyncNodeFile->GetFlags();

		if (flags & SyncTreeNodeFlags::FLAGS_PATH_AUTO_FROZEN_PATH)
		{
			validActionFlags |= SyncTreeNodeFlags::VALID_ACTION_FLAGS_ACTION_REMOVE_AUTO_FREEZE_INCLUDE_CHILDREN;
		}
	}
	return validActionFlags;
}


SyncTreeManager::SyncStyles SyncTreeManager::GetSyncStyle(void) const
{
	return m_syncStyle;
}

void SyncTreeManager::SetSyncStyle(SyncTreeManager::SyncStyles syncStyle)
{
	m_syncStyle = syncStyle;
}

bool SyncTreeManager::IsCheckingCRC(void) const
{
	return m_bCheckingCRC;
}

void SyncTreeManager::SetCheckingCRC(bool bCheckingCRC)
{
	m_bCheckingCRC = bCheckingCRC;
}

bool SyncTreeManager::IsUsingSyncDates(void) const
{
	return m_bUseSyncDates;
}

void SyncTreeManager::SetUsingSyncDates(bool bUseSyncDates)
{
	m_bUseSyncDates = bUseSyncDates;
}

bool SyncTreeManager::IsVerifyingCopiedFiles(void) const
{
	return m_bVerifyCopiedFiles;
}

void SyncTreeManager::SetVerifyCopiedFiles(bool bVerifyCopiedFiles)
{
	m_bVerifyCopiedFiles = bVerifyCopiedFiles;
}

void SyncTreeManager::GetUsedAutoFreezePaths(TupArray<bool> &usedEntries)
{
	usedEntries.RemoveAll();
	int numPaths = m_pAutoExclude->GetNumPaths();
	usedEntries.SetSize(numPaths);
	for (int index=0;index<numPaths;index++)
	{
		usedEntries[index] = false;
	}

	GetUsedAutoFreezePaths(m_pSyncTreeRoot,usedEntries);
}

void SyncTreeManager::GetUsedAutoFreezePaths(TreeNodeBase *pNode,TupArray<bool> &usedEntries)
{
	SyncTreeNodeFolder *pSyncNodeFolder = dynamic_cast<SyncTreeNodeFolder *>(pNode);
	// if this is not null then it is a sync folder
	if (pSyncNodeFolder)
	{
		TupString path;
		pSyncNodeFolder->GetPath(path);
		int pathIndex = m_pAutoExclude->CheckPath(path);
		if (pathIndex!=-1)
		{
			usedEntries[pathIndex] = true;
		}
		TreeIterator first(pSyncNodeFolder);
		TreeIterator last(pSyncNodeFolder);
		last.End();
		// step through child containers
		for (;first<last;first.Increment())
		{
			GetUsedAutoFreezePaths(*first,usedEntries);
		}
	}

	SyncTreeNodeFile *pSyncNodeFile = dynamic_cast<SyncTreeNodeFile *>(pNode);
	// if this is not null then it is a sync file
	if (pSyncNodeFile)
	{
		TupString path;
		pSyncNodeFile->GetPath(path);
		int pathIndex = m_pAutoExclude->CheckPath(path);
		if (pathIndex!=-1)
		{
			usedEntries[pathIndex] = true;
		}
	}
}

// returns true if both have the same crc
bool SyncTreeManager::CheckFileCRC(SyncTreeNodeFile *pSyncTreeNodeFile) const
{
	TreeNodeFile *pLeftFileNode = pSyncTreeNodeFile->GetLeftFileNode();
	TreeNodeFile *pRightFileNode = pSyncTreeNodeFile->GetRightFileNode();

	// if the lengths are not the same then bail
	if (pLeftFileNode->GetFileLength() != pRightFileNode->GetFileLength())
	{
		return false;
	}

	DWORD leftCRC,rightCRC;
	if (!m_pLeftDB->GetCRCFromFile(pSyncTreeNodeFile->GetLeftFileNode(),leftCRC))
		return false;
	if (!m_pRightDB->GetCRCFromFile(pSyncTreeNodeFile->GetRightFileNode(),rightCRC))
		return false;

	return (leftCRC==rightCRC);
}

SyncTreeManager::Feedback::Feedback(LoadDBFeedback &loadDBFeedback) : m_loadDBFeedback(loadDBFeedback)
{
	m_startTime = CTime::GetCurrentTime();
	m_stats.m_totalFilesScanned = 0;
	m_stats.m_fileName.Empty();
	m_stats.m_errorMessage.Empty();
}

bool SyncTreeManager::Feedback::Update(const char *pFileName)
{
	m_stats.m_fileName = pFileName;
	
	__time64_t elapsedTime = CTime::GetCurrentTime().GetTime() - m_startTime.GetTime();
	if (!m_stats.m_totalFilesScanned || elapsedTime>0)
	{
		m_startTime = CTime::GetCurrentTime();
		m_loadDBFeedback.Update(m_stats);
	}
	m_stats.m_totalFilesScanned++;
	return !m_loadDBFeedback.GetAbortFlag();
}


