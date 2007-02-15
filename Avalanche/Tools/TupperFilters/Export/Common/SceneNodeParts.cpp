////////////////////////////////////////////////////////////////////////////
//
// SceneNodeParts
//
// contains needed functions from the SceneNode.cpp file (without having to include any more files)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SceneNodeParts.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/29/02    Time: 1:00p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Use common name for precompiled header
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/25/02    Time: 12:41p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * hybrid configuration
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/12/02    Time: 5:11p
 * Updated in $/Avalanche/tools/TupperFilters/ExportPC
 * Instance, Scene Tree, etc
 */

#include "ExportPch.h"

void CSceneNode::Init()
{
	memset(this, 0, sizeof(CSceneNode));
}

void CSceneNode::AddChild(CSceneNode *pChildNode)
{
	/* add new child to head of list */
	pChildNode->m_pNextSibling = m_pFirstChild;
	m_pFirstChild = pChildNode;
	assert(pChildNode->m_pParent == NULL);	// if this hits, we're not cleaning up somewhere
	pChildNode->SetParent(this);
}

/* remove node from tree. this use to be done with a call to SLinkList__RemoveLink, but i want save space in the CSceneNode
record (by using a pointer instead of a ts_SLinkList record), so do it by hand */

void CSceneNode::Remove()
{
	assert(m_pParent);
#ifdef _DEBUG
	CSceneNode *pCheckNode = m_pParent->m_pFirstChild;
	while (pCheckNode)
	{
		if (pCheckNode == this)
			break;
		pCheckNode = pCheckNode->m_pNextSibling;
	}
	assert(pCheckNode);	// if this hits, node isn't in parent's list of children!
#endif //_DEBUG

	/* remove link */

	if (this == m_pParent->m_pFirstChild)
		m_pParent->m_pFirstChild = m_pNextSibling;	// head of list, link around it
	else
	{
		/* not head of list, have to find it. the link MUST be in the parent's list or this will crash */

		CSceneNode *pSearchNode = m_pParent->m_pFirstChild;
		while (pSearchNode->m_pNextSibling != this)
			pSearchNode = pSearchNode->m_pNextSibling;

		/* pSearchNode is the link before the current node-- link around the current node */

		pSearchNode->m_pNextSibling = m_pNextSibling;
	}

	/* not strictly necessary, but good for making sure we're running clean */

	SetParent(NULL);
}

/* set the view frustum to use when checking bounding box nodes while walking a tree */

/* count node's children */

int CSceneNode::ChildCount()
{
	int nCount = 0;
	CSceneNode *pChild = m_pFirstChild;
	while (pChild)
	{
		nCount++;
		pChild = pChild->m_pNextSibling;
	}
	return(nCount);
}

/* how deep in tree is this node? */

int CSceneNode::GetDepth()
{
	int nDepth = 0;
	CSceneNode *pParent = m_pParent;
	while (pParent)
	{
		nDepth++;
		pParent = pParent->m_pParent;
	}
	return(nDepth);
}
