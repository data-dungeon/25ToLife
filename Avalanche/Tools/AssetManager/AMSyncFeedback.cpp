//
//	AMSyncFeedback.h
//

//-----------------------------------------------------------------------------
// Includes:

#include "stdafx.h"
#include "AssetManager.h"
#include "AMSyncFeedback.h"
#include "SyncDialog.h"

AMSyncFeedback::AMSyncFeedback() : 
	SyncFeedback(),
	m_pDialog(NULL)
{
}

AMSyncFeedback::~AMSyncFeedback()
{
}

void AMSyncFeedback::SetDialog(SyncDialog *pDialog)
{
	m_pDialog = pDialog;
}

void AMSyncFeedback::UpdateStats(void)
{
	if (m_pDialog)
	{
		switch (m_syncMode)
		{
			case SyncFeedback::SYNC_MODE_NULL:
				{
					m_pDialog->SetSourceFileTitle("");
					m_pDialog->SetSourceFile("");
					m_pDialog->SetDestFileTitle("");
					m_pDialog->SetDestFile("");
				}
				break;
			case SyncFeedback::SYNC_MODE_COPY_LEFT_TO_RIGHT:
				{
					m_pDialog->SetSourceFileTitle("Copying ->");
					m_pDialog->SetDestFileTitle("To:");

					m_pDialog->SetSourceFile(m_sourceFile);
					m_pDialog->SetDestFile(m_destFile);
				}
				break;
			case SyncFeedback::SYNC_MODE_COPY_RIGHT_TO_LEFT:
				{
					m_pDialog->SetSourceFileTitle("Copying <-");
					m_pDialog->SetDestFileTitle("To:");

					m_pDialog->SetSourceFile(m_sourceFile);
					m_pDialog->SetDestFile(m_destFile);
				}
				break;
			case SyncFeedback::SYNC_MODE_DELETING_LEFT:
				{
					m_pDialog->SetSourceFileTitle("Deleting <-");
					m_pDialog->SetDestFileTitle("");

					m_pDialog->SetSourceFile(m_sourceFile);
					m_pDialog->SetDestFile("");
				}
				break;
			case SyncFeedback::SYNC_MODE_DELETING_RIGHT:
				{
					m_pDialog->SetSourceFileTitle("Deleting ->");
					m_pDialog->SetDestFileTitle("");

					m_pDialog->SetSourceFile(m_sourceFile);
					m_pDialog->SetDestFile("");

				}
				break;
		}
		int percent=0;
		if (m_totalFileBytes)
		{
			percent = (int)((100*m_totalFileBytesCopied)/m_totalFileBytes);
		}
		m_pDialog->SetCurrentFileProgress(percent);

		percent=0;
		if (m_totalBytes)
		{
			percent = (int)((100*m_totalBytesCopied)/m_totalBytes);
		}
		m_pDialog->SetTotalFileProgress(percent);

		__time64_t elapsedTime = CTime::GetCurrentTime().GetTime() - m_startTime.GetTime();
		if (elapsedTime==0)
			elapsedTime = 1;
		int transferRate = (int)(m_totalBytesCopied/elapsedTime);

		__time64_t estimatedTimeLeftTemp=0;
		if (m_totalBytesCopied)
		{
			estimatedTimeLeftTemp = (__time64_t)(((elapsedTime*m_totalBytes)/m_totalBytesCopied)-elapsedTime);
		}
		CTimeSpan estimatedTimeLeft(estimatedTimeLeftTemp);

		CString transferRateString;
		if (transferRate>=1048576)
			transferRateString.Format("Transfer Rate: %.2fMB/Sec",((float)transferRate)/1048576.0f);
		else
			transferRateString.Format("Transfer Rate: %.2fKB/Sec",((float)transferRate)/1024.0f);
		m_pDialog->SetTransferRate(transferRateString);


		CString estimatedTimePart;
		if (estimatedTimeLeft.GetDays())
		{
			estimatedTimePart = estimatedTimeLeft.Format("%D d %H h %M min %S sec");
		}
		else if (estimatedTimeLeft.GetTotalHours())
		{
			estimatedTimePart = estimatedTimeLeft.Format("%H h %M min %S sec");
		}
		else
		{
			estimatedTimePart = estimatedTimeLeft.Format("%M min %S sec");
		}
		CString estimatedTimeString;
		estimatedTimeString.Format("Estimated Time Left: %s (%.2fMB of %.2fMB Copied)",
			estimatedTimePart,
			((float)m_totalBytesCopied)/1048576.0f,
			((float)m_totalBytes)/1048576.0f);
		m_pDialog->SetEstimatedTime(estimatedTimeString);

		CString errorString;
		errorString.Format("Errors: %d",m_errorCount);
		m_pDialog->SetErrorText(errorString);
	}
}

void AMSyncFeedback::UpdateError(void)
{
	if (m_pDialog)
	{
		CString errorString;
		errorString.Format("Errors: %d",m_errorCount);
		m_pDialog->SetErrorText(errorString);
	}
}


bool AMSyncFeedback::GetAbortFlag(void) const
{
	if (m_pDialog && m_pDialog->GetCancelState())
		return true;
	else
		return false;
}

void AMSyncFeedback::ClearStats(void)
{
	if (m_pDialog)
	{
		m_pDialog->SetSourceFileTitle("");
		m_pDialog->SetSourceFile("");
		m_pDialog->SetDestFileTitle("");
		m_pDialog->SetDestFile("");
		m_pDialog->SetCurrentFileProgress(0);
		m_pDialog->SetTotalFileProgress(0);
		m_pDialog->SetTransferRate("");
		m_pDialog->SetEstimatedTime("");
		m_pDialog->SetErrorText("");
	}
}

void AMSyncFeedback::AddToLog(int flags,const char *pFormatString, ...)
{
	if (m_pDialog)
	{
		static char acMessage[800];
		va_list PrintArgs;

		/* create message */

		va_start(PrintArgs, pFormatString);
		vsprintf(acMessage, pFormatString, PrintArgs);
		va_end(PrintArgs);

		m_pDialog->WriteLog(flags,acMessage);
	}
}

void AMSyncFeedback::ClearLog(void)
{
	if (m_pDialog)
	{
		m_pDialog->ClearLog();
	}
}

