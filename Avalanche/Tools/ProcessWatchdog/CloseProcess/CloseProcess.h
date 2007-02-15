////////////////////////////////////////////////////////////////////////////
//
// CloseProcess
//
// Class close a process
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: CloseProcess.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/10/03   Time: 8:47a
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/CloseProcess
 * added new console application to close a process
*/

#ifndef CLOSE_PROCESS_CLOSE_PROCESS_H
#define CLOSE_PROCESS_CLOSE_PROCESS_H

class CloseProcess
{
	class SendMessageData
	{
	public:
		DWORD m_processID;
		UINT m_message;
		WPARAM m_wparam;
		LPARAM m_lparam;
		int m_postedCount;
	};
public:
	static bool KillProcess(const char *pProcessName,int waitForResponse);

private:
	static BOOL CALLBACK SendMessageTimeoutEnum( HWND hwnd, LPARAM lParam );
	static bool KillProcess(DWORD processID,const char *pName,int waitForResponse);
	static TupString StripExtensionFromString(const TupString &string);
};

#endif

