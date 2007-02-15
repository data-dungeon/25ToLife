//-----------------------------------------------------------------------------
//	File:		TreeIterator.h
//	Created:	Sep 26, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TreeIterator class allow for iterating through a tree
//			hierarchy.
//
//	History:	10/04/2002: Initial creation - Adam Clayton
//-----------------------------------------------------------------------------

/*
 * $History: TreeIterator.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/11/02   Time: 8:24a
 * Updated in $/Avalanche/tools/SyncUtils
 * made AFX_EXT_CLASS
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

#ifndef SYNC_UTILS_DLL_TREE_ITERATOR_H
#define SYNC_UTILS_DLL_TREE_ITERATOR_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_FOLDER_H
#include "TreeNodeFolder.h"
#endif

//-----------------------------------------------------------------------------
// TreeIterator class declaration:

class TreeNodeBase;
class TreeNodeFolder;

////////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS TreeIterator 
{
	TreeNodeFolder *m_pRoot;
	int m_nIndex;
	TreeNodeBase *m_pNode;
	bool m_bValid;
	int m_nListSize;

public:

	TreeIterator(TreeNodeFolder *pRoot);
	void Begin(void);
	void End(void);
	bool IsValid(void);
	TreeNodeBase *operator[](int nIndex);
	TreeNodeBase *operator*();
	bool Increment(void);
	bool Decrement(void);

private:
	friend bool operator>(TreeIterator &lhs, TreeIterator &rhs);
	friend bool operator<(TreeIterator &lhs, TreeIterator &rhs);
	friend bool operator>=(TreeIterator &lhs, TreeIterator &rhs);
	friend bool operator<=(TreeIterator &lhs, TreeIterator &rhs);
	friend bool operator==(TreeIterator &lhs, TreeIterator &rhs);
	friend bool operator!=(TreeIterator &lhs, TreeIterator &rhs);
};

#include "TreeIterator.hpp"

#endif

