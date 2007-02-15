#include "pch.h"
#include "notifyedit.h"

LRESULT NotifyEdit::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	NOTIFYEDITSTRUCT notify;

	notify.nmhdr.hwndFrom = GetSafeHwnd();
	notify.nmhdr.idFrom = GetDlgCtrlID();
	notify.nmhdr.code = EN_WINDOWMESSAGE;

	notify.message = message;
	notify.wParam = wParam;
	notify.lParam = lParam;

	CWnd *pParent = GetParent();

	if( pParent )
		pParent->SendMessage( WM_NOTIFY, (WPARAM) notify.nmhdr.idFrom, (LPARAM) &notify );

	return CEdit::WindowProc( notify.message, notify.wParam, notify.lParam );
}