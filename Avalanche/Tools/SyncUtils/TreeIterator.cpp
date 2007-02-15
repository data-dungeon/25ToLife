//-----------------------------------------------------------------------------
//	File:		TreeIterator.cpp
//	Created:	Sep 26, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TreeIterator class allow for iterating through a tree
//			hierarchy.
//
//	History:	10/04/2002: Initial creation - Adam Clayton
//-----------------------------------------------------------------------------

/*
 * $History: TreeIterator.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

#include "stdafx.h"
#include "SyncUtils.h"
#include "TreeIterator.h"
#include "TreeNodeBase.h"
#include "TreeNodeFolder.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// TreeIterator methods
TreeIterator::TreeIterator(TreeNodeFolder *pRoot) :
	m_pRoot(pRoot)
{
	ASSERT(pRoot);
	m_pNode = m_pRoot->GetFirstChildNode();
	m_nIndex = 0;
	m_bValid = true;
	m_nListSize = m_pRoot->GetNumChildren();
}

