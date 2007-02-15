//-----------------------------------------------------------------------------
//	File:		AMSyncTreeFactory.h
//	Created:	Oct. 9, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The AMSyncTreeFactory class derives from the SyncTreeFactory
//			class and provides a factory that can build AMSyncTreeNodeFolders
//			and AMSyncTreeNodeFiles.
//
//	History:	10/9/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------


#ifndef __AMSYNCTREEFACTORY__
#define __AMSYNCTREEFACTORY__



//-----------------------------------------------------------------------------
// Class Declaration:

class AMSyncTreeFactory : public SyncTreeFactory
{
public:
	// Constructors & Destructors:
	AMSyncTreeFactory();
	~AMSyncTreeFactory();

	// Public methods:
	TreeNodeFolder* CreateTreeNodeFolder(void) const;
	TreeNodeFolder* CreateTreeNodeFolder(const TreeNodeFolder& s) const;
	TreeNodeFile* CreateTreeNodeFile(void) const;
	TreeNodeFile* CreateTreeNodeFile(const TreeNodeFile& s) const;

	// Public data members:

protected:
	// Protected methods:

	// Protected data members:
};

#endif
