//
//	AMSyncFeedbackCmd.h
//

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AMSyncFeedbackCmd.h"

AMSyncFeedbackCmd::AMSyncFeedbackCmd() : 
SyncFeedback()
{
}

AMSyncFeedbackCmd::~AMSyncFeedbackCmd()
{
}

void AMSyncFeedbackCmd::UpdateStats(void)
{
}

void AMSyncFeedbackCmd::UpdateError(void)
{
}


bool AMSyncFeedbackCmd::GetAbortFlag(void) const
{
	return false;
}

void AMSyncFeedbackCmd::ClearStats(void)
{
}

void AMSyncFeedbackCmd::AddToLog(int flags,const char *pFormatString, ...)
{
	static char acMessage[800];
	va_list PrintArgs;

	/* create message */
	va_start(PrintArgs, pFormatString);
	if (PrintArgs)
	vsprintf(acMessage, pFormatString, PrintArgs);
	va_end(PrintArgs);

	printf("%s",acMessage);
}

void AMSyncFeedbackCmd::ClearLog(void)
{
}

