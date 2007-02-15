//
//	ClientTreeNodeFolder.h
//	

/*
 * $History: ClientTreeNodeFolder.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 */

#ifndef SYNC_UTILS_DLL_CLIENT_TREE_NODE_FOLDER_H
#define SYNC_UTILS_DLL_CLIENT_TREE_NODE_FOLDER_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_FOLDER_H
#include "TreeNodeFolder.h"
#endif

//-----------------------------------------------------------------------------
// ClientTreeNodeFolder class declaration:

class AFX_EXT_CLASS ClientTreeNodeFolder : public TreeNodeFolder
{
	unsigned long m_flags;

public:
	// Constructors and destructors:
	ClientTreeNodeFolder(void);
	ClientTreeNodeFolder(const ClientTreeNodeFolder &s);
	ClientTreeNodeFolder(const TreeNodeFolder &s,const TreeFactory &treeFactory);
	~ClientTreeNodeFolder();

	TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const;
	TreeNodeBase *Duplicate(void) const;
	TreeNodeBase *Create(const TreeFactory &treeFactory) const;
	TreeNodeBase *Create(void) const;

	unsigned long GetFlags(void) const;
	void SetFlags(unsigned long flags);
};

//-----------------------------------------------------------------------------

#endif

