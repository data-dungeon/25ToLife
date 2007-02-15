#include "pch.h"
#include "win32.h"

void Win32_TreeView_DeleteChildren( HWND hWnd, HTREEITEM hItem )
{
	if( hItem && Win32_TreeView_ItemHasChildren( hWnd, hItem ) )
	{
		HTREEITEM hNextItem;
		HTREEITEM hChildItem = TreeView_GetChild( hWnd, hItem );

		while( hChildItem )
		{
			hNextItem = TreeView_GetNextItem( hWnd, hChildItem, TVGN_NEXT );

			Win32_TreeView_DeleteChildren( hWnd, hChildItem );

			TreeView_DeleteItem( hWnd, hChildItem );

			hChildItem = hNextItem;
		}
	}

	return;
}

BOOL Win32_TreeView_ItemHasChildren( HWND hWnd, HTREEITEM hItem )
{
	if( TreeView_GetChild( hWnd, hItem ) )
		return TRUE;

	return FALSE;
}

void Win32_ScrollBar_OnUpdate( UINT Code, int LineSize, SCROLLINFO &si )
{
	switch( Code )
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;

	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

	case SB_LINEUP:
		si.nPos -= LineSize;
		break;

	case SB_LINEDOWN:
		si.nPos += LineSize;
		break;

	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;

	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;

	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	}

	return;
}

RECT Win32_GetClientRect( HWND hWnd )
{
	RECT rc;
	GetClientRect( hWnd, &rc );

	return rc;
}

void Win32_CenterWindow( HWND hWnd, RECT &rc )
{
	RECT rcWnd;
	GetWindowRect( hWnd, &rcWnd );

	int rcWidth = rc.right - rc.left;
	int rcHeight = rc.bottom - rc.top;
	int cx = rc.left + rcWidth / 2;
	int cy = rc.top + rcHeight / 2;

	int Width = rcWnd.right - rcWnd.left;
	int Height = rcWnd.bottom - rcWnd.top;

	int x = cx - Width / 2;
	int y = cy - Height / 2;

	MoveWindow( hWnd, x, y, Width, Height, FALSE );
}

void Win32_RunProgram( LPCTSTR pApplication, LPCTSTR pCommandLine )
{
	SHELLEXECUTEINFO sh;

	sh.cbSize = sizeof(sh);
	sh.fMask = 0;
	sh.hwnd = 0;
	sh.lpVerb = 0;
	sh.lpFile = pApplication;
	sh.lpParameters = pCommandLine;
	sh.lpDirectory = 0;
	sh.nShow = SW_NORMAL;
	sh.hInstApp = NULL;

	ShellExecuteEx( &sh );
}