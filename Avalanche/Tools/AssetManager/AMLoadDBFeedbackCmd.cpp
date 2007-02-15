//
//	AMLoadDBFeedbackCmdCmd
//

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AMLoadDBFeedbackCmd.h"


AMLoadDBFeedbackCmd::AMLoadDBFeedbackCmd() : LoadDBFeedback()
{
}

AMLoadDBFeedbackCmd::~AMLoadDBFeedbackCmd()
{
}


void AMLoadDBFeedbackCmd::Update(const LoadDBFeedback::Stats &stats)
{
	if (stats.m_errorMessage.GetLength()>0)
	{
		printf((const char *)stats.m_errorMessage);
		printf("\n");
	}
}

bool AMLoadDBFeedbackCmd::GetAbortFlag(void)
{
	return false;
}

