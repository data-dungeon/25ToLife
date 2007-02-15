#include "pch.h"
#include "maintoolbar.h"

BEGIN_MESSAGE_MAP( MainToolBar, CToolBar )
	ON_CBN_SELCHANGE( 0x100, OnFontChange )
END_MESSAGE_MAP();

BOOL MainToolBar::CreateFontList( CFont *pGuiFont )
{
	RECT rc;
	int x, y;
	int Height;

	CToolBarCtrl &c = GetToolBarCtrl();

	int Count = c.GetButtonCount();

	for( int i = 0; i < Count; i++ )
	{
		c.GetItemRect(i, &rc);

		x = rc.right + 8;
		y = rc.top;

		Height = rc.bottom - rc.top;
	}

	rc = GdiMakeRect( x, y, 192, 64 );

	m_FontList.Create( WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST, rc, this, 0x100 );

	if( !m_FontList.m_hWnd )
		return FALSE;

	m_FontList.SetFont( pGuiFont );

	return TRUE;
}

TCHAR* MainToolBar::GetCurrentFont( void )
{
	m_FontList.GetLBText( m_FontList.GetCurSel(), m_szFontName );

	return m_szFontName;
}

void MainToolBar::OnFontChange( void )
{
}