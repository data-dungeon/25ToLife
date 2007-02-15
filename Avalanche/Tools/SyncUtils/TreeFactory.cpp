////////////////////////////////////////////////////////////////////////////
//
// TreeFactory
//
// Class to creating tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TreeFactory.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "TreeFactory.h"
#include "TreeNodeFolder.h"
#include "TreeNodeFile.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TreeNodeFolder* TreeFactory::CreateTreeNodeFolder(void) const
{
	return new TreeNodeFolder;
}

TreeNodeFolder* TreeFactory::CreateTreeNodeFolder(const TreeNodeFolder &s) const
{
	return new TreeNodeFolder(s,*this);
}

TreeNodeFile* TreeFactory::CreateTreeNodeFile(void) const
{
	return new TreeNodeFile;

}

TreeNodeFile* TreeFactory::CreateTreeNodeFile(const TreeNodeFile &s) const
{
	return new TreeNodeFile(s);
}
