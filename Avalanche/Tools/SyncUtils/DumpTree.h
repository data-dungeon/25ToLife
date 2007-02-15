////////////////////////////////////////////////////////////////////////////
//
// DumpTree
//
// Class for printing out the tree nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: DumpTree.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:45p
 * Updated in $/Avalanche/tools/SyncUtils
 * trace dump
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

#ifndef SYNC_UTILS_DLL_DUMP_TREE_H
#define SYNC_UTILS_DLL_DUMP_TREE_H

//-----------------------------------------------------------------------------
// DumpTree class declaration:

class TreeNodeBase;

class AFX_EXT_CLASS DumpTreeDumperBase
{
public:
	virtual void Dump(const char *pString,int indent) = 0;
};

class AFX_EXT_CLASS DumpTreeDumperTrace : public DumpTreeDumperBase
{
public:
	virtual void Dump(const char *pString,int indent);
};

class AFX_EXT_CLASS DumpTree
{
public:
	static void DoDump(TreeNodeBase *pRoot,DumpTreeDumperBase &dumper,int indent=0);
};


//-----------------------------------------------------------------------------

#endif

