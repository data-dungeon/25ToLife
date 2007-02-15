//-----------------------------------------------------------------------------
//	File:		AMSyncTreeFactory.cpp
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



//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#ifndef __AMSYNCTREEFACTORY__
#include "AMSyncTreeFactory.h"
#endif
#ifndef __AMSYNCTREENODEFOLDER__
#include "AMSyncTreeNodeFolder.h"
#endif
#ifndef __AMSYNCTREENODEFILE__
#include "AMSyncTreeNodeFile.h"
#endif



//-----------------------------------------------------------------------------
// Class Definition:


//-----------------------------------------------------------------------------
// AMSyncTreeFactory::AMSyncTreeFactory
// This method is the default constructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeFactory::AMSyncTreeFactory()
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeFactory::~AMSyncTreeFactory
// This is the destructor for the class.
//
// Input: None.
//
// Output: None.
//-----------------------------------------------------------------------------
AMSyncTreeFactory::~AMSyncTreeFactory()
{
}


//-----------------------------------------------------------------------------
// AMSyncTreeFactory::CreateTreeNodeFolder
// This method will create and pass back an AMSyncTreeNodeFolder as a
// TreeNodeFolder pointer.
//
// Input: None.
//
// Output: TreeNodeFolder* representing the new AMSyncTreeNodeFolder.
//-----------------------------------------------------------------------------
TreeNodeFolder* AMSyncTreeFactory::CreateTreeNodeFolder(void) const
{
	return new AMSyncTreeNodeFolder;
}


//-----------------------------------------------------------------------------
// AMSyncTreeFactory::CreateTreeNodeFolder
// This method will create and pass back an AMSyncTreeNodeFolder as a
// TreeNodeFolder pointer.
//
// Input:
//  const TreeNodeFolder& s -- The TreeNodeFolder to use when creating the new
//							   folder.
//
// Output: TreeNodeFolder* representing the new AMSyncTreeNodeFolder.
//-----------------------------------------------------------------------------
TreeNodeFolder* AMSyncTreeFactory::CreateTreeNodeFolder(const TreeNodeFolder& s) const
{
	return new AMSyncTreeNodeFolder(s, *this);
}


//-----------------------------------------------------------------------------
// AMSyncTreeFactory::CreateTreeNodeFile
// This method will create and pass back an AMSyncTreeNodeFile as a
// TreeNodeFile pointer.
//
// Input: None.
//
// Output: TreeNodeFile* representing the new AMSyncTreeNodeFile.
//-----------------------------------------------------------------------------
TreeNodeFile* AMSyncTreeFactory::CreateTreeNodeFile(void) const
{
	return new AMSyncTreeNodeFile;
}


//-----------------------------------------------------------------------------
// AMSyncTreeFactory::CreateTreeNodeFile
// This method will create and pass back an AMSyncTreeNodeFile as a
// TreeNodeFile pointer.
//
// Input:
//  const TreeNodeFile &s -- The TreeNodeFile to use when creating the new
//							 file.
//
// Output: TreeNodeFile* representing the new AMSyncTreeNodeFile.
//-----------------------------------------------------------------------------
TreeNodeFile* AMSyncTreeFactory::CreateTreeNodeFile(const TreeNodeFile &s) const
{
	return new AMSyncTreeNodeFile(s);
}
