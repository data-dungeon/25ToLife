//-----------------------------------------------------------------------------
//	File:		TreeNodeFile.h
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
 * $History: TreeNodeFile.h $
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

#ifndef SYNC_UTILS_DLL_TREE_NODE_FILE_H
#define SYNC_UTILS_DLL_TREE_NODE_FILE_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_BASE_H
#include "TreeNodeBase.h"
#endif

//-----------------------------------------------------------------------------
// CTreeBase class declaration:

class AFX_EXT_CLASS TreeNodeFile : public TreeNodeBase
{
	DWORD m_fileLength;

public:
	// Constructors and destructors:
	TreeNodeFile(void);
	TreeNodeFile(const TreeNodeFile &s);
	~TreeNodeFile();

	TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const;
	TreeNodeBase *Duplicate(void) const;
	TreeNodeBase *Create(const TreeFactory &treeFactory) const;
	TreeNodeBase *Create(void) const;

	void CopyInfo(const TreeNodeFile &s);

	DWORD GetFileLength(void) const;
	void SetFileLength(DWORD newFileLength);

};

//-----------------------------------------------------------------------------

#endif

