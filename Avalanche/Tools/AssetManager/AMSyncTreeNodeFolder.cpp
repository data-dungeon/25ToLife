//-----------------------------------------------------------------------------
//	File:		AMSyncTreeNodeFolder.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#ifndef __AMSYNCTREENODEFOLDER__
#include "AMSyncTreeNodeFolder.h"
#endif



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::AMSyncTreeNodeFolder
// This method is the default constructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFolder::AMSyncTreeNodeFolder(void) : 
	SyncTreeNodeFolder(),
	m_hItemLeft(NULL),
	m_hItemRight(NULL),
	m_hItemIndicator(NULL)
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::AMSyncTreeNodeFolder
// This method is a copy constructor for the class.
//
// Input:
//  const AMSyncTreeNodeFolder& s -- The AMSyncTreeNodeFolder to use.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFolder::AMSyncTreeNodeFolder(const AMSyncTreeNodeFolder& s) : 
	SyncTreeNodeFolder(s), 
	m_hItemLeft(s.m_hItemLeft), 
	m_hItemRight(s.m_hItemRight),
	m_hItemIndicator(s.m_hItemIndicator)
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::AMSyncTreeNodeFolder
// This method is a copy constructor for the class.
//
// Input:
//  const TreeNodeFolder& s -- The TreeNodeFolder to use.
//  const TreeFactory& treeFactory -- The factory.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFolder::AMSyncTreeNodeFolder(const TreeNodeFolder& s, const TreeFactory& treeFactory) : 
	SyncTreeNodeFolder(s, treeFactory),
	m_hItemLeft(NULL),
	m_hItemRight(NULL),
	m_hItemIndicator(NULL)
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::~AMSyncTreeNodeFolder
// This is the destructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFolder::~AMSyncTreeNodeFolder()
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::Duplicate
// This method will duplicate this node folder.
//
// Input:
//  const TreeFactory& treeFactory -- The tree factory to use for creating the
//									  duplicated node folder.
//
// Output: TreeNodeBase* representing the new TreeNodeFolder.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFolder::Duplicate(const TreeFactory& treeFactory) const
{
	TreeNodeFolder* pNewNodeFolder = treeFactory.CreateTreeNodeFolder(*this);
	return pNewNodeFolder;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::Duplicate
// This method will duplicate this node folder.
//
// Input: None.
//
// Output: TreeNodeBase* representing the new AMSyncTreeNodeFolder.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFolder::Duplicate(void) const
{
	AMSyncTreeNodeFolder* pNewNodeFolder = new AMSyncTreeNodeFolder(*this);
	return pNewNodeFolder;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::Create
// This method will create a new node folder.
//
// Input:
//  const TreeFactory& treeFactory -- The tree factory to use for creating the
//									  node.
//
// Output: TreeNodeBase* representing the new TreeNodeFolder.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFolder::Create(const TreeFactory& treeFactory) const
{
	TreeNodeFolder* pNewNodeFolder = treeFactory.CreateTreeNodeFolder();
	return pNewNodeFolder;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::Create
// This method will create a new node folder.
//
// Input: None.
//
// Output: TreeNodeBase* representing the new AMSyncTreeNodeFolder.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFolder::Create(void) const
{
	AMSyncTreeNodeFolder* pNewNodeFolder = new AMSyncTreeNodeFolder;
	return pNewNodeFolder;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::SetHTreeItemLeft
// This mutator can be called to set the m_hItemLeft data member.
//
// Input:
//  HTREEITEM newHTreeItem -- The new HTREEITEM to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void AMSyncTreeNodeFolder::SetHTreeItemLeft(HTREEITEM newHTreeItem)
{
	m_hItemLeft = newHTreeItem;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::GetHTreeItemLeft
// This accessor can be called to get the m_hItemLeft data member.
//
// Input: None.
//
// Output: HTREEITEM representing the m_hItemLeft.
//-----------------------------------------------------------------------------
HTREEITEM AMSyncTreeNodeFolder::GetHTreeItemLeft()
{
	return m_hItemLeft;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::SetHTreeItemRight
// This mutator can be called to set the m_hItemRight data member.
//
// Input:
//  HTREEITEM newHTreeItem -- The new HTREEITEM to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void AMSyncTreeNodeFolder::SetHTreeItemRight(HTREEITEM newHTreeItem)
{
	m_hItemRight = newHTreeItem;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::GetHTreeItemRight
// This accessor can be called to get the m_hItemRight data member.
//
// Input: None.
//
// Output: HTREEITEM representing the m_hItemRight.
//-----------------------------------------------------------------------------
HTREEITEM AMSyncTreeNodeFolder::GetHTreeItemRight()
{
	return m_hItemRight;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::SetHTreeItemIndicator
// This mutator can be called to set the m_hItemIndicator data member.
//
// Input:
//  HTREEITEM newHTreeItem -- The new HTREEITEM to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void AMSyncTreeNodeFolder::SetHTreeItemIndicator(HTREEITEM newHTreeItem)
{
	m_hItemIndicator = newHTreeItem;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFolder::GetHTreeItemIndicator
// This accessor can be called to get the m_hItemIndicator data member.
//
// Input: None.
//
// Output: HTREEITEM representing the m_hItemIndicator.
//-----------------------------------------------------------------------------
HTREEITEM AMSyncTreeNodeFolder::GetHTreeItemIndicator()
{
	return m_hItemIndicator;
}


