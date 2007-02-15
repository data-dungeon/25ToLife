////////////////////////////////////////////////////////////////////////////
//
// SyncTreeFactory
//
// Class to creating tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncTreeFactory.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
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
#include "SyncTreeFactory.h"
#include "SyncTreeNodeFolder.h"
#include "SyncTreeNodeFile.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TreeNodeFolder* SyncTreeFactory::CreateTreeNodeFolder(void) const
{
	return new SyncTreeNodeFolder;
}

TreeNodeFolder* SyncTreeFactory::CreateTreeNodeFolder(const TreeNodeFolder &s) const
{
	return new SyncTreeNodeFolder(s,*this);
}

TreeNodeFile* SyncTreeFactory::CreateTreeNodeFile(void) const
{
	return new SyncTreeNodeFile;
}

TreeNodeFile* SyncTreeFactory::CreateTreeNodeFile(const TreeNodeFile &s) const
{
	return new SyncTreeNodeFile(s);
}
