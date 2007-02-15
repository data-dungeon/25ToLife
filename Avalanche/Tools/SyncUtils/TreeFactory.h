////////////////////////////////////////////////////////////////////////////
//
// TreeFactory
//
// Class to creating tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: TreeFactory.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

#ifndef SYNC_UTILS_DLL_TREE_FACTORY_H
#define SYNC_UTILS_DLL_TREE_FACTORY_H

//-----------------------------------------------------------------------------
// TreeFactory class declaration:

class TreeNodeFile;
class TreeNodeFolder;

class AFX_EXT_CLASS TreeFactory  
{
public:
	virtual TreeNodeFolder* CreateTreeNodeFolder(void) const;
	virtual TreeNodeFolder* CreateTreeNodeFolder(const TreeNodeFolder &s) const;
	virtual TreeNodeFile* CreateTreeNodeFile(void) const;
	virtual TreeNodeFile* CreateTreeNodeFile(const TreeNodeFile &s) const;
};

//-----------------------------------------------------------------------------

#endif

