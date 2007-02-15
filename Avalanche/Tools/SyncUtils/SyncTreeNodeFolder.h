//
//	SyncTreeNodeFolder.h
//	

/*
 * $History: SyncTreeNodeFolder.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/01/02   Time: 4:36p
 * Updated in $/Avalanche/tools/SyncUtils
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

#ifndef SYNC_UTILS_DLL_SYNC_TREE_NODE_FOLDER_H
#define SYNC_UTILS_DLL_SYNC_TREE_NODE_FOLDER_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_FOLDER_H
#include "TreeNodeFolder.h"
#endif

//-----------------------------------------------------------------------------
// SyncTreeNodeFolder class declaration:

class AFX_EXT_CLASS SyncTreeNodeFolder : public TreeNodeFolder
{
	TreeNodeFolder *m_pLeftFolderNode;
	TreeNodeFolder *m_pRightFolderNode;
	unsigned long m_flags;

public:
	// Constructors and destructors:
	SyncTreeNodeFolder(void);
	SyncTreeNodeFolder(const SyncTreeNodeFolder &s);
	SyncTreeNodeFolder(const TreeNodeFolder &s,const TreeFactory &treeFactory);
	~SyncTreeNodeFolder();

	TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const;
	TreeNodeBase *Duplicate(void) const;
	TreeNodeBase *Create(const TreeFactory &treeFactory) const;
	TreeNodeBase *Create(void) const;

	TreeNodeFolder *GetLeftFolderNode(void) const;
	TreeNodeFolder *GetRightFolderNode(void) const;

	void SetLeftFolderNode(TreeNodeFolder *pLeftFolderNode);
	void SetRightFolderNode(TreeNodeFolder *pRightFolderNode);

	unsigned long GetFlags(void) const;
	void SetFlags(unsigned long flags);
};

//-----------------------------------------------------------------------------

#endif

