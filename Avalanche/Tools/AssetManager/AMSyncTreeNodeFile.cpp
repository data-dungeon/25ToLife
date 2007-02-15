//-----------------------------------------------------------------------------
//	File:		AMSyncTreeNodeFile.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#ifndef __AMSYNCTREENODEFILE__
#include "AMSyncTreeNodeFile.h"
#endif



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::AMSyncTreeNodeFile
// This method is the default constructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFile::AMSyncTreeNodeFile(void) : 
	SyncTreeNodeFile(),
	m_hItemLeft(NULL),
	m_hItemRight(NULL),
	m_hItemIndicator(NULL)
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::AMSyncTreeNodeFile
// This method is a copy constructor for the class.
//
// Input:
//  const TreeNodeFile& s -- The TreeNodeFile to use.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFile::AMSyncTreeNodeFile(const TreeNodeFile& s) : 
	SyncTreeNodeFile(s),
	m_hItemLeft(NULL),
	m_hItemRight(NULL),
	m_hItemIndicator(NULL)
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::AMSyncTreeNodeFile
// This method is a copy constructor for the class.
//
// Input:
//  const AMSyncTreeNodeFile &s -- The AMSyncTreeNodeFile to use.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFile::AMSyncTreeNodeFile(const AMSyncTreeNodeFile &s) :
	SyncTreeNodeFile(s),
	m_hItemLeft(s.m_hItemLeft),
	m_hItemRight(s.m_hItemRight),
	m_hItemIndicator(s.m_hItemIndicator)
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::~AMSyncTreeNodeFile
// This is the destructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeNodeFile::~AMSyncTreeNodeFile()
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::Duplicate
// This method will duplicate this node file.
//
// Input:
//  const TreeFactory& treeFactory -- The tree factory to use for creating the
//									  duplicated node.
//
// Output: TreeNodeBase* representing the new TreeNodeFile.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFile::Duplicate(const TreeFactory& treeFactory) const
{
	TreeNodeFile* pNewNodeFile = treeFactory.CreateTreeNodeFile(*this);
	return pNewNodeFile;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::Duplicate
// This method will duplicate this node file.
//
// Input: None.
//
// Output: TreeNodeBase* representing the new AMSyncTreeNodeFile.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFile::Duplicate(void) const
{
	AMSyncTreeNodeFile* pNewNodeFile = new AMSyncTreeNodeFile(*this);
	return pNewNodeFile;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::Create
// This method will create a new node file.
//
// Input:
//  const TreeFactory& treeFactory -- The tree factory to use for creating the
//									  node.
//
// Output: TreeNodeBase* representing the new TreeNodeFile.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFile::Create(const TreeFactory& treeFactory) const
{
	TreeNodeFile* pNewNodeFile = treeFactory.CreateTreeNodeFile();
	return pNewNodeFile;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::Create
// This method will create a new node file.
//
// Input: None.
//
// Output: TreeNodeBase* representing the new AMSyncTreeNodeFile.
//-----------------------------------------------------------------------------
TreeNodeBase* AMSyncTreeNodeFile::Create(void) const
{
	AMSyncTreeNodeFile* pNewNodeFile = new AMSyncTreeNodeFile;
	return pNewNodeFile;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::SetHTreeItemLeft
// This mutator can be called to set the m_hItemLeft data member.
//
// Input:
//  HTREEITEM newHTreeItem -- The new HTREEITEM to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void AMSyncTreeNodeFile::SetHTreeItemLeft(HTREEITEM newHTreeItem)
{
	m_hItemLeft = newHTreeItem;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::GetHTreeItemLeft
// This accessor can be called to get the m_hItemLeft data member.
//
// Input: None.
//
// Output: HTREEITEM representing the m_hItemLeft.
//-----------------------------------------------------------------------------
HTREEITEM AMSyncTreeNodeFile::GetHTreeItemLeft()
{
	return m_hItemLeft;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::SetHTreeItemRight
// This mutator can be called to set the m_hItemRight data member.
//
// Input:
//  HTREEITEM newHTreeItem -- The new HTREEITEM to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void AMSyncTreeNodeFile::SetHTreeItemRight(HTREEITEM newHTreeItem)
{
	m_hItemRight = newHTreeItem;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::GetHTreeItemRight
// This accessor can be called to get the m_hItemRight data member.
//
// Input: None.
//
// Output: HTREEITEM representing the m_hItemRight.
//-----------------------------------------------------------------------------
HTREEITEM AMSyncTreeNodeFile::GetHTreeItemRight()
{
	return m_hItemRight;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::SetHTreeItemIndicator
// This mutator can be called to set the m_hItemIndicator data member.
//
// Input:
//  HTREEITEM newHTreeItem -- The new HTREEITEM to set.
//
// Output: None.
//-----------------------------------------------------------------------------
void AMSyncTreeNodeFile::SetHTreeItemIndicator(HTREEITEM newHTreeItem)
{
	m_hItemIndicator = newHTreeItem;
}


//-----------------------------------------------------------------------------
// AMSyncTreeNodeFile::GetHTreeItemIndicator
// This accessor can be called to get the m_hItemIndicator data member.
//
// Input: None.
//
// Output: HTREEITEM representing the m_hItemIndicator.
//-----------------------------------------------------------------------------
HTREEITEM AMSyncTreeNodeFile::GetHTreeItemIndicator()
{
	return m_hItemIndicator;
}
