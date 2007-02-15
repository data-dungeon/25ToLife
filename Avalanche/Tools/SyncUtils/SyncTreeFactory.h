////////////////////////////////////////////////////////////////////////////
//
// SyncTreeFactory
//
// Class to creating tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncTreeFactory.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/09/02   Time: 1:57p
 * Updated in $/Avalanche/tools/SyncUtils
 * sync tree first pass complete
 */

#ifndef SYNC_UTILS_DLL_SYNC_TREE_FACTORY_H
#define SYNC_UTILS_DLL_SYNC_TREE_FACTORY_H

#ifndef SYNC_UTILS_DLL_TREE_FACTORY_H
#include "TreeFactory.h"
#endif

//-----------------------------------------------------------------------------
// SyncTreeFactory class declaration:

class TreeNodeFile;
class TreeNodeFolder;

class AFX_EXT_CLASS SyncTreeFactory : public TreeFactory  
{
public:
	TreeNodeFolder* CreateTreeNodeFolder(void) const;
	TreeNodeFolder* CreateTreeNodeFolder(const TreeNodeFolder &s) const;
	TreeNodeFile* CreateTreeNodeFile(void) const;
	TreeNodeFile* CreateTreeNodeFile(const TreeNodeFile &s) const;
};

//-----------------------------------------------------------------------------

#endif

