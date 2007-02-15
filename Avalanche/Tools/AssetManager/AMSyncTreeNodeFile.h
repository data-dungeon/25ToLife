//-----------------------------------------------------------------------------
//	File:		AMSyncTreeNodeFile.h
//	Created:	Oct. 9, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The AMSyncTreeNodeFile class derives from the SyncTreeNodeFile
//			class and provides a file structure containing data specific to
//			the Asset Manager.
//
//	History:	10/9/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __AMSYNCTREENODEFILE__
#define __AMSYNCTREENODEFILE__



//-----------------------------------------------------------------------------
// Class Declaration:

class AMSyncTreeNodeFile : public SyncTreeNodeFile
{
public:
	// Constructors & Destructors:
	AMSyncTreeNodeFile(void);
	AMSyncTreeNodeFile(const TreeNodeFile& s);
	AMSyncTreeNodeFile(const AMSyncTreeNodeFile& s);
	~AMSyncTreeNodeFile();

	// Accessors & mutators:
	void SetHTreeItemLeft(HTREEITEM newHTreeItem);
	HTREEITEM GetHTreeItemLeft();
	void SetHTreeItemRight(HTREEITEM newHTreeItem);
	HTREEITEM GetHTreeItemRight();
	void SetHTreeItemIndicator(HTREEITEM newHTreeItem);
	HTREEITEM GetHTreeItemIndicator();

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
