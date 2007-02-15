//-----------------------------------------------------------------------------
//	File:		TreeNodeFolder.h
//	Created:	Sep 26, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TreeNodeFolder class handles all the functionality for a folder
//			item in the folder tree hierarchy.
//
//	History:	9/26/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: TreeNodeFolder.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 9:03a
 * Updated in $/Avalanche/tools/SyncUtils
 * added treenode code
 * added tupperware & keeper to project and workspace
 */

#ifndef SYNC_UTILS_DLL_TREE_NODE_FOLDER_H
#define SYNC_UTILS_DLL_TREE_NODE_FOLDER_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_BASE_H
#include "TreeNodeBase.h"
#endif

//-----------------------------------------------------------------------------
// CTreeFolder class declaration:

class AFX_EXT_CLASS TreeNodeFolder : public TreeNodeBase  
{
	friend class TreeNodeBase;
	friend class TreeIterator;

	TreeNodeBase* m_pFirstChild;
	TreeNodeBase* m_pLastChild;
	int m_nNumChildren;

public:
	// Constructors and destructors:
	TreeNodeFolder(void);
	TreeNodeFolder(const TreeNodeFolder &s);
	TreeNodeFolder(const TreeNodeFolder &s,const TreeFactory &treeFactory);

	~TreeNodeFolder();

	// Public methods:
	const char *GetClassName(void) const;
	TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const;
	TreeNodeBase *Duplicate(void) const;
	TreeNodeBase *Create(const TreeFactory &treeFactory) const;
	TreeNodeBase *Create(void) const;

	void CopyInfo(const TreeNodeFolder &s);
	void SetRootFolderInfo(void);

	void AddChild( TreeNodeBase *pChild, bool bAddToEnd = true );
	void InsertChild( TreeNodeBase *pChild, TreeNodeBase *pSibling, bool bInsertAfter = true );

	void DeleteNode( TreeNodeBase *pNode);

	TreeNodeBase *GetFirstChildNode(void) const;
	TreeNodeBase *GetLastChildNode(void) const;

	int GetNumChildren(void) const;

	__int64 GetTotalSize(void) const;


};

//-----------------------------------------------------------------------------



#endif // !defined(AFX_TREEFOLDER_H__2A5826E8_1913_41B3_94C4_92089AB95ACA__INCLUDED_)
