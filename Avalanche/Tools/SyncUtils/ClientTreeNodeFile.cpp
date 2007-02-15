//
// ClientTreeNodeFile.cpp
//

/*
 * $History: ClientTreeNodeFile.cpp $
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
#include "ClientTreeNodeFile.h"
#include "TreeFactory.h"
#include "SyncTreeNodeFlags.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ClientTreeNodeFile methods
ClientTreeNodeFile::ClientTreeNodeFile(void) : 
	TreeNodeFile(),
	m_flags(0)
{
	m_leftSyncDate.dwLowDateTime = 0;
	m_leftSyncDate.dwHighDateTime = 0;
	m_rightSyncDate.dwLowDateTime = 0;
	m_rightSyncDate.dwHighDateTime = 0;
}

ClientTreeNodeFile::ClientTreeNodeFile(const TreeNodeFile &s) : 
	TreeNodeFile(s), 
	m_flags(0)
{
	m_leftSyncDate.dwLowDateTime = 0;
	m_leftSyncDate.dwHighDateTime = 0;
	m_rightSyncDate.dwLowDateTime = 0;
	m_rightSyncDate.dwHighDateTime = 0;
}

ClientTreeNodeFile::ClientTreeNodeFile(const ClientTreeNodeFile &s) : TreeNodeFile(s), 
	m_flags(s.m_flags),
	m_leftSyncDate(s.m_leftSyncDate),
	m_rightSyncDate(s.m_rightSyncDate)

{
}

ClientTreeNodeFile::~ClientTreeNodeFile()
{
}

TreeNodeBase *ClientTreeNodeFile::Duplicate(const TreeFactory &treeFactory) const
{
	TreeNodeFile *pNewNodeFile = treeFactory.CreateTreeNodeFile(*this);
	return pNewNodeFile;
}

TreeNodeBase *ClientTreeNodeFile::Duplicate(void) const
{
	ClientTreeNodeFile *pNewNodeFile = new ClientTreeNodeFile(*this);
	return pNewNodeFile;
}

TreeNodeBase *ClientTreeNodeFile::Create(const TreeFactory &treeFactory) const
{
	TreeNodeFile *pNewNodeFile = treeFactory.CreateTreeNodeFile();
	return pNewNodeFile;
}

TreeNodeBase *ClientTreeNodeFile::Create(void) const
{
	ClientTreeNodeFile *pNewNodeFile = new ClientTreeNodeFile;
	return pNewNodeFile;
}

unsigned long ClientTreeNodeFile::GetFlags(void) const
{
	return m_flags;
}

void ClientTreeNodeFile::SetFlags(unsigned long flags)
{
	m_flags = flags;
}

FILETIME ClientTreeNodeFile::GetLeftSyncDate(void) const
{
	return m_leftSyncDate;
}

void ClientTreeNodeFile::SetLeftSyncDate(FILETIME fileTime)
{
	m_leftSyncDate = fileTime;
}

FILETIME ClientTreeNodeFile::GetRightSyncDate(void) const
{
	return m_rightSyncDate;
}

void ClientTreeNodeFile::SetRightSyncDate(FILETIME fileTime)
{
	m_rightSyncDate = fileTime;
}
