////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "SyncUtils.h"
#include "SyncFeedback.h"

//-----------------------------------------------------------------------------
// Defines, etc.:

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// SyncFeedback methods
SyncFeedback::SyncFeedback(void) :
	m_syncMode(SyncFeedback::SYNC_MODE_NULL),
	m_totalFileBytes(0),
	m_totalFileBytesCopied(0),
	m_totalBytes(0),
	m_totalBytesCopied(0),
	m_errorCount(0),
	m_bPreviewMode(false),
	m_bIncludePath(false)
{
	m_startTime = CTime::GetCurrentTime();
	m_sourceFile = "";
	m_destFile = "";
}

SyncFeedback::~SyncFeedback()
{
}

