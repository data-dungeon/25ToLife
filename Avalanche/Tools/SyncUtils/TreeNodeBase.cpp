//-----------------------------------------------------------------------------
//	File:		TreeNodeBase.cpp
//	Created:	Sep 26, 2002
//				Copyright (c) 2002, Avalanche Software Inc.
//				All Rights Reserved.
//	Author:		Brad Worthen
//
//	Desc:	The TreeNodeBase class acts as a base class for the folder tree
//			hierarchy.
//
//	History:	9/26/2002: Initial creation - Brad Worthen
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "TreeNodeBase.h"
#include "TreeNodeFolder.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// TreeNodeBase methods

TreeNodeBase::TreeNodeBase(void) : 
	m_pParent(NULL),m_pNext(NULL),m_pPrev(NULL),m_attributes(NULL),m_CRCValue(0),m_bValidCRC(false)
{
	m_modifiedTime.dwLowDateTime=0;
	m_modifiedTime.dwHighDateTime=0;
}

TreeNodeBase::TreeNodeBase(const TreeNodeBase &s) : 
	m_pParent(NULL),m_pNext(NULL),m_pPrev(NULL),m_attributes(s.m_attributes),m_fileName(s.m_fileName),m_modifiedTime(s.m_modifiedTime)
{
}

TreeNodeBase::~TreeNodeBase()
{
	Detatch();
}

TreeNodeBase* TreeNodeBase::GetParent(void) const
{
	return m_pParent;
}

TreeNodeBase* TreeNodeBase::GetNext(void) const
{
	return m_pNext;
}

TreeNodeBase* TreeNodeBase::GetPrev(void) const
{
	return m_pPrev;
}

const char *TreeNodeBase::GetFileName(void) const
{
	return m_fileName;
}

void TreeNodeBase::SetFileName(const char *pNewName)
{
	m_fileName = pNewName;
}

const FILETIME TreeNodeBase::GetModifiedTime(void) const
{
	return m_modifiedTime;
}

void TreeNodeBase::SetModifiedTime(const FILETIME &newModifiedTime)
{
	m_modifiedTime = newModifiedTime;
}

void TreeNodeBase::SetAttributes(unsigned long attributes)
{
	m_attributes = attributes;
}

unsigned long TreeNodeBase::GetAttributes(void) const
{
	return m_attributes;
}

DWORD TreeNodeBase::GetCRC(void) const
{
	return m_CRCValue;
}

bool TreeNodeBase::ValidCRC(void) const
{
	return m_bValidCRC;
}

void TreeNodeBase::SetCRC(DWORD newCRC) const
{
	m_CRCValue = newCRC;
}

void TreeNodeBase::SetValidCRC(bool bValidCRC) const
{
	m_bValidCRC = bValidCRC;
}

void TreeNodeBase::Detatch(void)
{
	// if parent is looking at me then set to look at the next atom
	if (m_pParent)
	{
		if (m_pParent->m_pFirstChild == this)
		{
			m_pParent->m_pFirstChild = m_pNext;
		}
		if (m_pParent->m_pLastChild == this)
		{
			m_pParent->m_pLastChild = m_pPrev;
		}
		m_pParent->m_nNumChildren--;
	}
	// remove from sibling list
	if (m_pPrev)
		m_pPrev->m_pNext = m_pNext;
	if (m_pNext)
		m_pNext->m_pPrev = m_pPrev;
	m_pParent = NULL;
	m_pNext = m_pPrev = NULL;
}

void TreeNodeBase::CopyInfo(const TreeNodeBase &s)
{
	m_modifiedTime = s.m_modifiedTime;
	m_fileName = s.m_fileName;
	m_attributes = s.m_attributes;
}

void TreeNodeBase::GetPath(TupString &filePath) const
{
	// if I have a parent
	if (m_pParent)
	{
		TupString parentFilePath;
		m_pParent->GetPath(parentFilePath);
		filePath = parentFilePath;
		if (filePath[filePath.GetLength()-1]!='\\')
			filePath+="\\";
		filePath+=m_fileName;
	}
	// if I am the root
	else
	{
		filePath = m_fileName;
	}
}

