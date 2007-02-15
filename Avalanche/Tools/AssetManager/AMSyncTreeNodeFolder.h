//-----------------------------------------------------------------------------
//	File:		AMSyncTreeNodeFolder.h
//	Created:	Oct. 9, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The AMSyncTreeNodeFolder class derives from the SyncTreeNodeFolder
//			class and provides a folder structure containing data specific to
//			the Asset Manager.
//
//	History:	10/9/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __AMSYNCTREENODEFOLDER__
#define __AMSYNCTREENODEFOLDER__



//-----------------------------------------------------------------------------
// Class Declaration:

class AMSyncTreeNodeFolder : public SyncTreeNodeFolder
{
public:
	// Constructors & Destructors:
	AMSyncTreeNodeFolder(void);
	AMSyncTreeNodeFolder(const AMSyncTreeNodeFolder& s);
	AMSyncTreeNodeFolder(const TreeNodeFolder& s, const TreeFactory& treeFactory);
	~AMSyncTreeNodeFolder();

	// Accessors & mutators:
	void SetHTreeItemLeft(HTREEITEM newHTreeItem);
	HTREEITEM GetHTreeItemLeft();
	void SetHTreeItemRight(HTREEITEM newHTreeItem);
	HTREEITEM GetHTreeItemRight();
	void SetHTreeItemIndicator(HTREEITEM newHTreeItem);
	HTREEITEM GetHTreeItemIndicator();

	void ClearHtreeItems(bool doChildren);
	void ClearHtreeItems(TreeNodeBase *pNode,bool doChildren);

	// Public methods:
	TreeNodeBase* Duplicate(const TreeFactory& treeFactory) const;
	TreeNodeBase* Duplicate(void) const;
	TreeNodeBase* Create(const TreeFactory& treeFactory) const;
	TreeNodeBase* Create(void) const;

protected:
	// Protected methods:

	// Protected data members:
	HTREEITEM m_hItemLeft;
	HTREEITEM m_hItemRight;
	HTREEITEM m_hItemIndicator;
};

#endif
