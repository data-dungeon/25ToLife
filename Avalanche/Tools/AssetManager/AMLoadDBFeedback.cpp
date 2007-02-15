//
//	AMLoadDBFeedback.h
//

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AssetManager.h"
#include "AMLoadDBFeedback.h"
#include "LoadDBDialog.h"


static const char *LoadStatusStrings[] = 
{
	"",	// STATUS_NULL
	"Loading Left Database", // STATUS_LOADING_LEFT_DB
	"Loading Right Database",  // STATUS_LOADING_RIGHT_DB
	"Building Sync Info", // STATUS_BUILDING_SYNC_TREE
	"Building Sync Info with CRC", // STATUS_BUILDING_SYNC_TREE
	"Building Controls", // STATUS_BUILDING_TREE_CONTROLS
};

AMLoadDBFeedback::AMLoadDBFeedback() : 
	LoadDBFeedback(),
		m_loadStatus(STATUS_NULL),
	m_pDialog(NULL)
{
}

AMLoadDBFeedback::~AMLoadDBFeedback()
{
}

void AMLoadDBFeedback::SetDialog(LoadDBDialog *pDialog)
{
	m_pDialog = pDialog;
}

void AMLoadDBFeedback::SetStatus(LoadStatus loadStatus)
{
	m_loadStatus = loadStatus;
	if (m_pDialog)
	{
		m_pDialog->SetTextLine1(LoadStatusStrings[m_loadStatus]);
	}
}

AMLoadDBFeedback::LoadStatus AMLoadDBFeedback::GetStatus(void) const
{
	return m_loadStatus;
}

void AMLoadDBFeedback::Update(const LoadDBFeedback::Stats &stats)
{
	if (m_pDialog)
	{
		if (stats.m_errorMessage.GetLength()>0)
		{
			m_pDialog->SetTextLine1(stats.m_errorMessage);
			m_pDialog->SetTextLine2("");
			m_pDialog->SetTextLine3("");
		}
		else
		{
			CString numFilesString;
			CString fileNameString;
			if (stats.m_totalFilesScanned)
			{
				numFilesString.Format("%d Files Processed",stats.m_totalFilesScanned);
			}
			if (stats.m_fileName.GetLength()>0)
			{
				fileNameString.Format("Processing %s",(const char *)stats.m_fileName);
			}
			m_pDialog->SetTextLine2(numFilesString);
			m_pDialog->SetTextLine3(fileNameString);
		}
	}
}

bool AMLoadDBFeedback::GetAbortFlag(void)
{
	if (m_pDialog && m_pDialog->GetCancelState())
		return true;
	else
		return false;
}

