//
// ClientTreeNodeFolder.cpp
//

/*
 * $History: ClientTreeNodeFolder.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "ClientTreeNodeFolder.h"
#include "TreeFactory.h"
#include "SyncTreeNodeFlags.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ClientTreeNodeFolder methods
ClientTreeNodeFolder::ClientTreeNodeFolder(void) : 
	TreeNodeFolder(),
	m_flags(0)
{
}

ClientTreeNodeFolder::ClientTreeNodeFolder(const ClientTreeNodeFolder &s) : 
	TreeNodeFolder(s), 
	m_flags(s.m_flags)
{
}

ClientTreeNodeFolder::ClientTreeNodeFolder(const TreeNodeFolder &s,const TreeFactory &treeFactory) : 
	TreeNodeFolder(s,treeFactory),
	m_flags(0)
{
}

ClientTreeNodeFolder::~ClientTreeNodeFolder()
{
}

TreeNodeBase *ClientTreeNodeFolder::Duplicate(const TreeFactory &treeFactory) const
{
	TreeNodeFolder *pNewNodeFolder = treeFactory.CreateTreeNodeFolder(*this);
	return pNewNodeFolder;
}

TreeNodeBase *ClientTreeNodeFolder::Duplicate(void) const
{
	ClientTreeNodeFolder *pNewNodeFolder = new ClientTreeNodeFolder(*this);
	return pNewNodeFolder;
}

TreeNodeBase *ClientTreeNodeFolder::Create(const TreeFactory &treeFactory) const
{
	TreeNodeFolder *pNewNodeFolder = treeFactory.CreateTreeNodeFolder();
	return pNewNodeFolder;
}

TreeNodeBase *ClientTreeNodeFolder::Create(void) const
{
	ClientTreeNodeFolder *pNewNodeFolder = new ClientTreeNodeFolder;
	return pNewNodeFolder;
}

unsigned long ClientTreeNodeFolder::GetFlags(void) const
{
	return m_flags;
}

void ClientTreeNodeFolder::SetFlags(unsigned long flags)
{
	m_flags = flags;
}

