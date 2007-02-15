//
// SyncTreeNodeFolder.cpp
//

/*
 * $History: SyncTreeNodeFolder.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/01/02   Time: 4:36p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/31/02   Time: 4:31p
 * Updated in $/Avalanche/tools/SyncUtils
 * context menu
 * exclusions / including / expanding / collapsing
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
 * User: Adam Clayton Date: 10/10/02   Time: 12:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * fixed contructors to fall base constructors
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "SyncTreeNodeFolder.h"
#include "TreeFactory.h"
#include "SyncTreeNodeFlags.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// SyncTreeNodeFolder methods
SyncTreeNodeFolder::SyncTreeNodeFolder(void) : 
	TreeNodeFolder(),
	m_pLeftFolderNode(NULL),
	m_pRightFolderNode(NULL),
	m_flags(0)
{
}

SyncTreeNodeFolder::SyncTreeNodeFolder(const SyncTreeNodeFolder &s) : 
	TreeNodeFolder(s), 
	m_pLeftFolderNode(s.m_pLeftFolderNode), 
	m_pRightFolderNode(s.m_pRightFolderNode),
	m_flags(s.m_flags)
{
}

SyncTreeNodeFolder::SyncTreeNodeFolder(const TreeNodeFolder &s,const TreeFactory &treeFactory) : 
	TreeNodeFolder(s,treeFactory),
	m_pLeftFolderNode(NULL),
	m_pRightFolderNode(NULL),
	m_flags(0)
{
}

SyncTreeNodeFolder::~SyncTreeNodeFolder()
{
}

TreeNodeBase *SyncTreeNodeFolder::Duplicate(const TreeFactory &treeFactory) const
{
	TreeNodeFolder *pNewNodeFolder = treeFactory.CreateTreeNodeFolder(*this);
	return pNewNodeFolder;
}

TreeNodeBase *SyncTreeNodeFolder::Duplicate(void) const
{
	SyncTreeNodeFolder *pNewNodeFolder = new SyncTreeNodeFolder(*this);
	return pNewNodeFolder;
}

TreeNodeBase *SyncTreeNodeFolder::Create(const TreeFactory &treeFactory) const
{
	TreeNodeFolder *pNewNodeFolder = treeFactory.CreateTreeNodeFolder();
	return pNewNodeFolder;
}

TreeNodeBase *SyncTreeNodeFolder::Create(void) const
{
	SyncTreeNodeFolder *pNewNodeFolder = new SyncTreeNodeFolder;
	return pNewNodeFolder;
}

TreeNodeFolder *SyncTreeNodeFolder::GetLeftFolderNode(void) const
{
	return m_pLeftFolderNode;
}

TreeNodeFolder *SyncTreeNodeFolder::GetRightFolderNode(void) const
{
	return m_pRightFolderNode;
}

void SyncTreeNodeFolder::SetLeftFolderNode(TreeNodeFolder *pLeftFolderNode)
{
	m_pLeftFolderNode = pLeftFolderNode;
}

void SyncTreeNodeFolder::SetRightFolderNode(TreeNodeFolder *pRightFolderNode)
{
	m_pRightFolderNode = pRightFolderNode;
}

unsigned long SyncTreeNodeFolder::GetFlags(void) const
{
	return m_flags;
}

void SyncTreeNodeFolder::SetFlags(unsigned long flags)
{
	m_flags = flags;
}

