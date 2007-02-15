////////////////////////////////////////////////////////////////////////////
//
// LoadDBFeedback
//
// Class used dealing with the feedback while loading a database
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: LoadDBFeedback.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 10/14/02   Time: 4:00p
 * Updated in $/Avalanche/tools/SyncUtils
 * simple feedback classes
 */

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "LoadDBFeedback.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LoadDBFeedback::LoadDBFeedback(void)
{
}

LoadDBFeedback::~LoadDBFeedback()
{
}

void LoadDBFeedback::Update(const Stats &stats)
{
}

bool LoadDBFeedback::GetAbortFlag(void)
{
	return false; // return false if we don't need to abort
}

