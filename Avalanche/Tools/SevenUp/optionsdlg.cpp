#include "pch.h"
#include "application.h"
#include "optionsdlg.h"
#include "resource.h"

BEGIN_MESSAGE_MAP( OptionsDialog, CDialog )
	ON_COMMAND( IDOK, OnOK )
	ON_COMMAND( IDCANCEL, OnCancel )
	ON_COMMAND( IDC_BROWSE1, OnBrowse1 )
	ON_COMMAND( IDC_BROWSE2, OnBrowse2 )
END_MESSAGE_MAP()

void OptionsDialog::SetExportUtility( LPCTSTR pFileName )
{
	m_ExportUtilityPath.set( pFileName );
}

void OptionsDialog::GetExportUtility( LPTSTR pFileName, int MaxName )
{
	if( pFileName )
		strncpy( pFileName, m_ExportUtilityPath.get(), MaxName );

	return;
}

void OptionsDialog::GetExportUtility( tstring &FileName )
{
	FileName.set( m_ExportUtilityPath.get() );
}

void OptionsDialog::SetGameDataPath( LPCTSTR pPath )
{
	m_GameDataPath.set( pPath );
}

void OptionsDialog::GetGameDataPath( LPTSTR pPath, int MaxPath )
{
	if( pPath )
		strncpy( pPath, m_GameDataPath.get(), MaxPath );

	return;
}

void OptionsDialog::GetGameDataPath( tstring &Path )
{
	Path.set( m_GameDataPath.get() );
}

BOOL OptionsDialog::OnInitDialog( void )
{
	SetDlgItemText( IDC_EXPORTUTILITYPATH, m_ExportUtilityPath.get() );
	SetDlgItemText( IDC_GAMEDATAPATH, m_GameDataPath.get() );

	return CDialog::OnInitDialog();
}

void OptionsDialog::OnOK( void )
{
	TCHAR szPathName[_MAX_PATH];

	GetDlgItemText( IDC_EXPORTUTILITYPATH, szPathName, _MAX_PATH );
	m_ExportUtilityPath.set( szPathName );

	GetDlgItemText( IDC_GAMEDATAPATH, szPathName, _MAX_PATH );
	m_GameDataPath.set( szPathName );

	App.OnOptionsOK();

	CDialog::OnOK();
}

void OptionsDialog::OnCancel( void )
{
	CDialog::OnCancel();
}

void OptionsDialog::OnBrowse1( void )
{
	LPCTSTR pFileName = AskFileOpen( m_hWnd, 0, "Executable Files (*.exe)\0*.exe\0\0" );

	if( pFileName )
		SetDlgItemText( IDC_EXPORTUTILITYPATH, pFileName );

	return;
}

void OptionsDialog::OnBrowse2( void )
{
	LPCTSTR pFileName = AskFileOpen( m_hWnd, 0, "" );

	if( pFileName )
		SetDlgItemText( IDC_GAMEDATAPATH, pFileName );

	return;
}