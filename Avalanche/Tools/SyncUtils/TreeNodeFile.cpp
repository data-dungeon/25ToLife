//-----------------------------------------------------------------------------
//	File:		TreeNodeFile.cpp
//	Created:	Sep 26, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The CTreeFile class handles all the functionality for a file
//			item in the folder tree hierarchy.
//
//	History:	9/26/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

/*
 * $History: TreeNodeFile.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 10/10/02   Time: 12:09p
 * Updated in $/Avalanche/tools/SyncUtils
 * fixed contructors to fall base constructors
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 10/08/02   Time: 9:39a
 * Updated in $/Avalanche/tools/SyncUtils
 * pulled back to simple tree structure
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
TreeNodeFile::TreeNodeFile(void) : 
	TreeNodeBase(), 
	m_fileLength(0)
{
}

TreeNodeFile::TreeNodeFile(const TreeNodeFile &s) : 
	TreeNodeBase(s),
	m_fileLength(s.m_fileLength)
{
}

TreeNodeFile::~TreeNodeFile()
{
}

TreeNodeBase *TreeNodeFile::Duplicate(const TreeFactory &treeFactory) const
{
	TreeNodeFile *pNewNodeFile = treeFactory.CreateTreeNodeFile(*this);
	return pNewNodeFile;
}

TreeNodeBase *TreeNodeFile::Duplicate(void) const
{
	TreeNodeFile *pNewNodeFile = new TreeNodeFile(*this);
	return pNewNodeFile;
}

TreeNodeBase *TreeNodeFile::Create(const TreeFactory &treeFactory) const
{
	TreeNodeFile *pNewNodeFile = treeFactory.CreateTreeNodeFile();
	return pNewNodeFile;
}

TreeNodeBase *TreeNodeFile::Create(void) const
{
	TreeNodeFile *pNewNodeFile = new TreeNodeFile;
	return pNewNodeFile;
}

void TreeNodeFile::CopyInfo(const TreeNodeFile &s)
{
	TreeNodeBase::CopyInfo(s);
	m_fileLength = s.m_fileLength;
}

DWORD TreeNodeFile::GetFileLength(void) const
{
	return m_fileLength;
}

void TreeNodeFile::SetFileLength(DWORD newFileLength)
{
	m_fileLength = newFileLength;
}

