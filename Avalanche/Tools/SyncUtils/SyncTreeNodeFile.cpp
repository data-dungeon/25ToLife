//
// SyncTreeNodeFile.cpp
//

/*
 * $History: SyncTreeNodeFile.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 11/04/02   Time: 4:30p
 * Updated in $/Avalanche/tools/SyncUtils
 * new validate action code
 * auto exclude based on extension and prefix
 * refresh button
 * view excluded
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 11/01/02   Time: 4:36p
 * Updated in $/Avalanche/tools/SyncUtils
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/31/02   Time: 4:31p
 * Updated in $/Avalanche/tools/SyncUtils
 * context menu
 * exclusions / including / expanding / collapsing
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/30/02   Time: 4:23p
 * Updated in $/Avalanche/tools/SyncUtils
 * tree stuff
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
#include "SyncTreeNodeFile.h"
#include "TreeFactory.h"
#include "SyncTreeNodeFlags.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// SyncTreeNodeFile methods
SyncTreeNodeFile::SyncTreeNodeFile(void) : 
	TreeNodeFile(),
	m_pLeftFileNode(NULL),
	m_pRightFileNode(NULL),
	m_flags(0)
{
	m_leftSyncDate.dwLowDateTime = 0;
	m_leftSyncDate.dwHighDateTime = 0;
	m_rightSyncDate.dwLowDateTime = 0;
	m_rightSyncDate.dwHighDateTime = 0;
}

SyncTreeNodeFile::SyncTreeNodeFile(const TreeNodeFile &s) : 
	TreeNodeFile(s), 
	m_pLeftFileNode(NULL),
	m_pRightFileNode(NULL),
	m_flags(0)
{
	m_leftSyncDate.dwLowDateTime = 0;
	m_leftSyncDate.dwHighDateTime = 0;
	m_rightSyncDate.dwLowDateTime = 0;
	m_rightSyncDate.dwHighDateTime = 0;
}

SyncTreeNodeFile::SyncTreeNodeFile(const SyncTreeNodeFile &s) : TreeNodeFile(s), 
	m_pLeftFileNode(s.m_pLeftFileNode), 
	m_pRightFileNode(s.m_pRightFileNode),
	m_flags(s.m_flags),
	m_leftSyncDate(s.m_leftSyncDate),
	m_rightSyncDate(s.m_rightSyncDate)
{
}

SyncTreeNodeFile::~SyncTreeNodeFile()
{
}

TreeNodeBase *SyncTreeNodeFile::Duplicate(const TreeFactory &treeFactory) const
{
	TreeNodeFile *pNewNodeFile = treeFactory.CreateTreeNodeFile(*this);
	return pNewNodeFile;
}

TreeNodeBase *SyncTreeNodeFile::Duplicate(void) const
{
	SyncTreeNodeFile *pNewNodeFile = new SyncTreeNodeFile(*this);
	return pNewNodeFile;
}

TreeNodeBase *SyncTreeNodeFile::Create(const TreeFactory &treeFactory) const
{
	TreeNodeFile *pNewNodeFile = treeFactory.CreateTreeNodeFile();
	return pNewNodeFile;
}

TreeNodeBase *SyncTreeNodeFile::Create(void) const
{
	SyncTreeNodeFile *pNewNodeFile = new SyncTreeNodeFile;
	return pNewNodeFile;
}

TreeNodeFile *SyncTreeNodeFile::GetLeftFileNode(void) const
{
	return m_pLeftFileNode;
}

TreeNodeFile *SyncTreeNodeFile::GetRightFileNode(void) const
{
	return m_pRightFileNode;
}

void SyncTreeNodeFile::SetLeftFileNode(TreeNodeFile *pLeftFileNode)
{
	m_pLeftFileNode = pLeftFileNode;
}

void SyncTreeNodeFile::SetRightFileNode(TreeNodeFile *pRightFileNode)
{
	m_pRightFileNode = pRightFileNode;
}

unsigned long SyncTreeNodeFile::GetFlags(void) const
{
	return m_flags;
}

void SyncTreeNodeFile::SetFlags(unsigned long flags)
{
	m_flags = flags;
}

FILETIME SyncTreeNodeFile::GetLeftSyncDate(void) const
{
	return m_leftSyncDate;
}

void SyncTreeNodeFile::SetLeftSyncDate(FILETIME fileTime)
{
	m_leftSyncDate = fileTime;
}

FILETIME SyncTreeNodeFile::GetRightSyncDate(void) const
{
	return m_rightSyncDate;
}

void SyncTreeNodeFile::SetRightSyncDate(FILETIME fileTime)
{
	m_rightSyncDate = fileTime;
}
