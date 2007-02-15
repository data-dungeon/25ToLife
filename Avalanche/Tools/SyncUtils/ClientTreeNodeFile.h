//
//	ClientTreeNodeFile.h
//	

/*
 * $History: ClientTreeNodeFile.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 1/10/03    Time: 1:05p
 * Updated in $/Avalanche/tools/SyncUtils
 * client data manager foundataion
 * exclude / frozen lock stuff
 */

#ifndef SYNC_UTILS_DLL_CLIENT_TREE_NODE_FILE_H
#define SYNC_UTILS_DLL_CLIENT_TREE_NODE_FILE_H

//-----------------------------------------------------------------------------
// Includes:

#ifndef SYNC_UTILS_DLL_TREE_NODE_FILE_H
#include "TreeNodeFile.h"
#endif

//-----------------------------------------------------------------------------
// CTreeBase class declaration:

class AFX_EXT_CLASS ClientTreeNodeFile : public TreeNodeFile
{
	unsigned long m_flags;
	FILETIME m_leftSyncDate;
	FILETIME m_rightSyncDate;

public:
	// Constructors and destructors:
	ClientTreeNodeFile(void);
	ClientTreeNodeFile(const TreeNodeFile &s);
	ClientTreeNodeFile(const ClientTreeNodeFile &s);
	~ClientTreeNodeFile();

	TreeNodeBase *Duplicate(const TreeFactory &treeFactory) const;
	TreeNodeBase *Duplicate(void) const;
	TreeNodeBase *Create(const TreeFactory &treeFactory) const;
	TreeNodeBase *Create(void) const;

	unsigned long GetFlags(void) const;
	void SetFlags(unsigned long flags);

	FILETIME GetLeftSyncDate(void) const;
	void SetLeftSyncDate(FILETIME fileTime);

	FILETIME GetRightSyncDate(void) const;
	void SetRightSyncDate(FILETIME fileTime);

};

//-----------------------------------------------------------------------------

#endif

