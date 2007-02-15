////////////////////////////////////////////////////////////////////////////
//
// Watchdog
//
// Class hold information about a watchdog
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: tray.cpp $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 12:37p
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * looks like we are here!
*/

#include "stdafx.h"
#include "ProcessWatchDog.h"
#include "tray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Tray

Tray::Tray ( CWnd* pWnd, UINT uCallbackMessage,
						  LPCTSTR szTip, HICON* pList )
{
	// this is only for Windows 95 (or higher)
	m_bEnabled = ( GetVersion() & 0xff ) >= 4 ;
	if (!m_bEnabled)
		return ;

	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize = sizeof(NOTIFYICONDATA) ;
	m_tnd.hWnd = pWnd->GetSafeHwnd() ;
	m_tnd.uID = 0 ;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP ;	
	m_tnd.uCallbackMessage = uCallbackMessage;  
	strcpy ( m_tnd.szTip, "ProcessWatchdog");
	//or you could use:
	//strcpy ( m_tnd.szTip, AfxGetApp()->m_pszAppName);
	//this will display the app name instead of the string you specify 

	// save the pointer to the icon list and set the initial
	// default icon.
	m_pIconList = pList ;
	m_tnd.hIcon = m_pIconList[0] ;
	Shell_NotifyIcon (NIM_ADD,&m_tnd);
}

Tray::~Tray()
{
	if (m_bEnabled)
	{
		m_tnd.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP ; // add this line
		Shell_NotifyIcon (NIM_DELETE, &m_tnd);
	}
}

void Tray::SetState(int id)
{
	if (!m_bEnabled)
		return;

	if (m_tnd.hIcon != m_pIconList[id])
	{
		m_tnd.hIcon = m_pIconList[id];
		Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
	}
}

void Tray::SetTip(LPCTSTR szTip)
{
	if (!m_bEnabled) 
		return;
	if (m_tnd.szTip != szTip)
	{
		strcpy ( m_tnd.szTip, szTip);
		Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
	}
}
