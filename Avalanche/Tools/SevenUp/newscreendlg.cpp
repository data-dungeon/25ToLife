#include "pch.h"
#include "newscreendlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP( NewScreenDialog, CDialog )
END_MESSAGE_MAP();

NewScreenDialog::NewScreenDialog( CWnd *pParent ) : CDialog( IDD_NEWSCREEN, pParent )
{
}

BOOL NewScreenDialog::OnInitDialog( void )
{
	RECT rc;
	CWnd *pParent = GetParent();

	if( pParent )
		pParent->GetWindowRect( &rc );
	else
	{
		SystemParametersInfo( SPI_GETWORKAREA, 0, &rc, 0 );
	}

	Win32_CenterWindow( m_hWnd, rc );

	TCHAR *pScreenName = m_ScreenName.get();

	if( tstrlen(pScreenName) < 1 )
		pScreenName = "NewScreen";

	SetDlgItemText( IDC_SCREENNAME, pScreenName );

	return CDialog::OnInitDialog();
}

void NewScreenDialog::OnOK( void )
{
	TCHAR szText[256];

	GetDlgItemText( IDC_SCREENNAME, szText, sizeof(szText)/sizeof(szText[0]) );
	m_ScreenName.set( szText );

	CDialog::OnOK();
}

void NewScreenDialog::OnCancel( void )
{
	CDialog::OnCancel();
}