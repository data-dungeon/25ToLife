//-----------------------------------------------------------------------------
//	File:		TreeIterator.hpp
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
 * $History: TreeIterator.hpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

inline void TreeIterator::Begin(void)
{
	m_nIndex = 0;
	m_pNode = m_pRoot->m_pFirstChild;
	m_bValid = true;
}

inline void TreeIterator::End(void)
{
	m_nIndex = m_nListSize;
	m_bValid = false;
}

inline bool TreeIterator::IsValid(void)
{
	return m_bValid;
}

inline TreeNodeBase* TreeIterator::operator*()
{
	ASSERT(m_bValid);
	return m_pNode;
}

inline TreeNodeBase* TreeIterator::operator[](int nIndex)
{
	if (m_nIndex<m_nListSize && m_nIndex >=0)
	{
		if (!m_bValid)
		{
			m_nIndex = 0; // start at beginning
			m_pNode = m_pRoot->m_pFirstChild;
		}
		while (m_nIndex>nIndex)
		{
			m_nIndex--;
			m_pNode = m_pNode->m_pPrev;
		}
		while (m_nIndex<nIndex)
		{
			m_nIndex++;
			m_pNode = m_pNode->m_pNext;
		}
		m_bValid = true;
		return m_pNode;
	}
	else
	{
		m_bValid = false;
		return NULL;
	}
}

inline bool TreeIterator::Increment(void)
{
	ASSERT(m_bValid);
	m_nIndex++;
	if (m_nIndex>=m_nListSize)
	{
		m_nIndex = m_nListSize;
		m_bValid = false;
	}
	else
	{
		m_pNode = m_pNode->m_pNext;
	}
	return m_bValid;
}

inline bool TreeIterator::Decrement(void)
{
	ASSERT(m_bValid);
	m_nIndex--;
	if (m_nIndex<0)
	{
		m_nIndex = 0;
		m_bValid = false;
	}
	else
	{
		m_pNode = m_pNode->m_pPrev;
	}
	return m_bValid;
}

inline bool operator>(TreeIterator &lhs, TreeIterator &rhs)
{
	return lhs.m_nIndex > rhs.m_nIndex;
}

inline bool operator<(TreeIterator &lhs, TreeIterator &rhs)
{
	return lhs.m_nIndex < rhs.m_nIndex;
}

inline bool operator>=(TreeIterator &lhs, TreeIterator &rhs)
{
	return lhs.m_nIndex >= rhs.m_nIndex;
}

inline bool operator<=(TreeIterator &lhs, TreeIterator &rhs)
{
	return lhs.m_nIndex <= rhs.m_nIndex;
}

inline bool operator==(TreeIterator &lhs, TreeIterator &rhs)
{
	return lhs.m_nIndex == rhs.m_nIndex;
}

inline bool operator!=(TreeIterator &lhs, TreeIterator &rhs)
{
	return lhs.m_nIndex != rhs.m_nIndex;
}

