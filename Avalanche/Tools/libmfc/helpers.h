#ifndef _MFC_HELPERS_H_
#define _MFC_HELPERS_H_

#include <afxwin.h>
#include <afxdlgs.h>
#include <afxcmn.h>

void CheckDlgItem( CWnd* pWnd, UINT ButtonId, BOOL Checked );
BOOL GetCheck( CWnd* pWnd, UINT ButtonId );

void ToolBarMFC_SetButtonStyle( CToolBar *pToolBar, UINT ButtonId, UINT Style );

#endif // _MFC_HELPERS_H_