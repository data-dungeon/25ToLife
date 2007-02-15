////////////////////////////////////////////////////////////////////////////
//
// SyncDBBase
//
// Base class for sync databases
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: SyncDBBase.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/04/02   Time: 4:01p
 * Updated in $/Avalanche/tools/SyncUtils
 * first pass of tree, factories, and syncdb files
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "SyncDBBase.h"
#include "TreeFactory.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SyncDBBase::SyncDBBase(const char *pFilePath) :
	m_filePath(pFilePath)
{
}

SyncDBBase::~SyncDBBase()
{
}

