////////////////////////////////////////////////////////////////////////////
//
// Tray
//
// Class to manage the system tray icon
//
////////////////////////////////////////////////////////////////////////////

/*
* $History: tray.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 10/02/03   Time: 12:37p
 * Created in $/Avalanche/tools/Plugins/ANIMATION MASTER V10.5/ProcessWatchdog/ProcessWatchDog
 * looks like we are here!
*/

#ifndef PROCESS_WATCHDOG_TRAY_H
#define PROCESS_WATCHDOG_TRAY_H

class Tray : public CObject
{
	// Construction
public:
	Tray ( CWnd* pWnd, UINT uCallbackMessage,
		LPCTSTR szTip, HICON* pList ) ;

	// Attributes
public:
	BOOL m_bEnabled ;	
	NOTIFYICONDATA	m_tnd ;
	HICON* m_pIconList ;

public:
	void SetState ( int id = 0 ) ;
	void SetTip(LPCTSTR szTip);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Tray)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~Tray();

};

/////////////////////////////////////////////////////////////////////////////

#endif