//-----------------------------------------------------------------------------
//	File:		TreeNodeFolder.cpp
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
 * $History: TreeNodeFolder.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 2/04/03    Time: 2:15p
 * Updated in $/Avalanche/tools/SyncUtils
 * Tooltip for tree entries
 * Fix for folder set path not existing with an existing folder set
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/10/02   Time: 12:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * fixed contructors to fall base constructors
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


//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "TreeNodeFolder.h"
#include "TreeNodeFile.h"
#include "TreeFactory.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// TreeFolder methods
TreeNodeFolder::TreeNodeFolder(void) :
	TreeNodeBase(),
	m_pFirstChild(NULL),
	m_pLastChild(NULL),
	m_nNumChildren(0)
{
}

TreeNodeFolder::TreeNodeFolder(const TreeNodeFolder &s) : 
	TreeNodeBase(s),
	m_pFirstChild(NULL),
	m_pLastChild(NULL),
	m_nNumChildren(0)
{
	TreeNodeBase *pNode = s.m_pFirstChild;
	while(pNode)
	{
		TreeNodeBase *pNewNode =  pNode->Duplicate();
		ASSERT(pNewNode);
		AddChild(pNewNode,true);
		pNode=pNode->m_pNext;
	}
}

TreeNodeFolder::TreeNodeFolder(const TreeNodeFolder &s,const TreeFactory &treeFactory) : 
	TreeNodeBase(s),
	m_pFirstChild(NULL),
	m_pLastChild(NULL),
	m_nNumChildren(0)
{
	TreeNodeBase *pNode = s.m_pFirstChild;
	while(pNode)
	{
		TreeNodeBase *pNewNode =  pNode->Duplicate(treeFactory);
		ASSERT(pNewNode);
		AddChild(pNewNode,true);
		pNode=pNode->m_pNext;
	}
}

TreeNodeFolder::~TreeNodeFolder()
{
	while ( m_pFirstChild )
	{
		TreeNodeBase *pChild = m_pFirstChild;
		pChild->Detatch();
		delete pChild;
	}
}

TreeNodeBase *TreeNodeFolder::Duplicate(const TreeFactory &treeFactory) const
{
	TreeNodeFolder *pNewNodeFolder = treeFactory.CreateTreeNodeFolder(*this);
	return pNewNodeFolder;
}

TreeNodeBase *TreeNodeFolder::Duplicate(void) const
{
	TreeNodeFolder *pNewNodeFolder = new TreeNodeFolder(*this);
	return pNewNodeFolder;
}

TreeNodeBase *TreeNodeFolder::Create(const TreeFactory &treeFactory) const
{
	TreeNodeFolder *pNewNodeFolder = treeFactory.CreateTreeNodeFolder();
	return pNewNodeFolder;
}

TreeNodeBase *TreeNodeFolder::Create(void) const
{
	TreeNodeFolder *pNewNodeFolder = new TreeNodeFolder;
	return pNewNodeFolder;
}

void TreeNodeFolder::CopyInfo(const TreeNodeFolder &s)
{
	TreeNodeBase::CopyInfo(s);
}

void TreeNodeFolder::SetRootFolderInfo(void)
{
	m_fileName = "$"; // root filename for now
}


void TreeNodeFolder::AddChild( TreeNodeBase *pChild, bool bAddToEnd)
{
	ASSERT(pChild);
	pChild->Detatch();
	pChild->m_pParent = this;
	m_nNumChildren++;
	if (bAddToEnd) 
	{
		if (m_pLastChild) // if there is a child at the end
		{ 
			pChild->m_pPrev = m_pLastChild;
			m_pLastChild->m_pNext = pChild;
			m_pLastChild = pChild; // I am now at the end
		}
		else // if there are no children
		{
			m_pFirstChild = pChild;
			m_pLastChild = pChild;
		}
	}
	else
	{
		pChild->m_pNext = m_pFirstChild;
		if (m_pFirstChild)
		{
			m_pFirstChild->m_pPrev = pChild;
		}
		m_pFirstChild = pChild;
		if (!m_pLastChild)
			m_pLastChild = pChild;
	}
}

void TreeNodeFolder::InsertChild( TreeNodeBase *pChild, TreeNodeBase *pSibling, bool bInsertAfter)
{
	ASSERT(pChild);
	if (!pSibling) // if sibling is null (just add child like normal)
	{
		AddChild(pChild);
	}
	else
	{
		ASSERT(pSibling->m_pParent == this); // this better point to me
		pChild->Detatch();
		pChild->m_pParent = this;
		m_nNumChildren++;
		if (bInsertAfter)
		{
			pChild->m_pNext = pSibling->m_pNext;
			pSibling->m_pNext = pChild;
			pChild->m_pPrev = pSibling;
			if (pChild->m_pNext)
				pChild->m_pNext->m_pPrev = pChild;
			else
			{
				ASSERT(m_pLastChild == pSibling);
				m_pLastChild = pChild;
			}
		}
		else
		{
			pChild->m_pPrev = pSibling->m_pPrev;
			pSibling->m_pPrev = pChild;
			pChild->m_pNext = pSibling;
			if (pChild->m_pPrev)
				pChild->m_pPrev->m_pNext = pChild;
			else
			{
				ASSERT(m_pFirstChild == pSibling);
				m_pFirstChild = pChild;
			}
		}
	}
}

void TreeNodeFolder::DeleteNode( TreeNodeBase *pNode)
{
	ASSERT(pNode->m_pParent == this); // we should be the parent
	pNode->Detatch();
	delete pNode;
}

TreeNodeBase *TreeNodeFolder::GetFirstChildNode(void) const
{
	return m_pFirstChild;
}

TreeNodeBase *TreeNodeFolder::GetLastChildNode(void) const
{
	return m_pLastChild;
}

int TreeNodeFolder::GetNumChildren(void) const
{
	return m_nNumChildren;
}

__int64 TreeNodeFolder::GetTotalSize(void) const
{
	__int64 size=0;

	TreeNodeBase *pChild = m_pFirstChild;
	while(pChild)
	{
		TreeNodeFolder *pChildFolder = dynamic_cast<TreeNodeFolder *>(pChild);
		if (pChildFolder)
		{
			size+=pChildFolder->GetTotalSize();
		}
		TreeNodeFile *pChildFile = dynamic_cast<TreeNodeFile *>(pChild);
		if (pChildFile)
		{
			size+=pChildFile->GetFileLength();
		}
		pChild=pChild->GetNext();
	}
	return size;
}
