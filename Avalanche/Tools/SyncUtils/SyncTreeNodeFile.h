//
//	SyncTreeNodeFile.h
//	

/*
 * $History: SyncTreeNodeFile.h $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/30/02   Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stuff
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/18/02   Time: 3:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * delete folders and files
 * child flags for actions and status
 * SetAction function in sync tree manager
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

#ifndef SYNC_UTILS_DLL_SYNC_TREE_NODE_FILE_H
#define SYNC_UTILS_DLL_SYNC_TREE_NODE_FILE_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_FILE_H
#include "TreeNodeFile.h"
#endif

//-----------------------------------------------------------------------------
// CTreeBase class declaration:

class AFX_EXT_CLASS SyncTreeNodeFile : public TreeNodeFile
{
	TreeNodeFile *m_pLeftFileNode;
	TreeNodeFile *m_pRightFileNode;
	unsigned long m_flags;
	FILETIME m_leftSyncDate;
	FILETIME m_rightSyncDate;

public:
	// Constructors and destructors:
	SyncTreeNodeFile(void);
	SyncTreeNodeFile(const TreeNodeFile &s);
	SyncTreeNodeFile(const SyncTreeNodeFile &s);
	~SyncTreeNodeFile();

	TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const;
	TreeNodeBase *Duplicate(void) const;
	TreeNodeBase *Create(const TreeFactory &treeFactory) const;
	TreeNodeBase *Create(void) const;

	TreeNodeFile *GetLeftFileNode(void) const;
	TreeNodeFile *GetRightFileNode(void) const;

	void SetLeftFileNode(TreeNodeFile *pLeftFileNode);
	void SetRightFileNode(TreeNodeFile *pRightFileNode);

	unsigned long GetFlags(void) const;
	void SetFlags(unsigned long flags);

	FILETIME GetLeftSyncDate(void) const;
	void SetLeftSyncDate(FILETIME fileTime);

	FILETIME GetRightSyncDate(void) const;
	void SetRightSyncDate(FILETIME fileTime);

};

//-----------------------------------------------------------------------------

#endif

