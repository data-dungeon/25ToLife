#include "pch.h"
#include "projectpaths.h"

BEGIN_MESSAGE_MAP( ProjectPaths, CPropertyPage )
	ON_COMMAND( IDC_BROWSE_GAMEDIR, OnBrowseGameDir )
	ON_COMMAND( IDC_BROWSE_DATADIR, OnBrowseDataDir )
	ON_COMMAND( IDC_BROWSE_DATAPC, OnBrowseDataPC )
	ON_COMMAND( IDC_BROWSE_DATADX, OnBrowseDataDX )
	ON_COMMAND( IDC_BROWSE_DATAPS2, OnBrowseDataPS2 )
	ON_COMMAND( IDC_BROWSE_DATAGCN, OnBrowseDataGCN )
	ON_COMMAND( IDC_BROWSE_DATAXBOX, OnBrowseDataXBOX )
	ON_COMMAND( IDC_BROWSE_DATAPATH, OnBrowseDataPath )
	ON_COMMAND( IDC_DEFAULTDIR, OnDefaultDir )
	ON_UPDATE_COMMAND_UI_RANGE( 0, 0x7fffffff, OnUpdateCommandUI )
END_MESSAGE_MAP();

ProjectPaths::ProjectPaths( void ) : CPropertyPage( IDD_PATHS )
{
}

BOOL ProjectPaths::OnInitDialog( void )
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

	SetDlgItemText( IDC_GAMEDIR, m_GameDir.get() );
	SetDlgItemText( IDC_DATADIR, m_DataDir.get() );
	SetDlgItemText( IDC_DATAPC, m_DataPC.get() );
	SetDlgItemText( IDC_DATADX, m_DataDX.get() );
	SetDlgItemText( IDC_DATAPS2, m_DataPS2.get() );
	SetDlgItemText( IDC_DATAGCN, m_DataGCN.get() );
	SetDlgItemText( IDC_DATAXBOX, m_DataXBOX.get() );
	SetDlgItemText( IDC_DATAPATH, m_DataPath.get() );
	
	return CPropertyPage::OnInitDialog();
}

void ProjectPaths::OnOK( void )
{
	TCHAR szPath[_MAX_PATH];

	GetDlgItemText( IDC_GAMEDIR, szPath, _MAX_PATH );
	m_GameDir.set( szPath );

	GetDlgItemText( IDC_DATADIR, szPath, _MAX_PATH );
	m_DataDir.set( szPath );

	GetDlgItemText( IDC_DATAPC, szPath, _MAX_PATH );
	m_DataPC.set( szPath );

	GetDlgItemText( IDC_DATADX, szPath, _MAX_PATH );
	m_DataDX.set( szPath );

	GetDlgItemText( IDC_DATAPS2, szPath, _MAX_PATH );
	m_DataPS2.set( szPath );

	GetDlgItemText( IDC_DATAGCN, szPath, _MAX_PATH );
	m_DataGCN.set( szPath );

	GetDlgItemText( IDC_DATAXBOX, szPath, _MAX_PATH );
	m_DataXBOX.set( szPath );

	GetDlgItemText( IDC_DATAPATH, szPath, _MAX_PATH );
	m_DataPath.set( szPath );

	CPropertyPage::OnOK();
}

void ProjectPaths::OnCancel( void )
{
	CPropertyPage::OnCancel();
}

void ProjectPaths::OnBrowse( UINT ParentId, UINT Id )
{
	TCHAR szGameDir[_MAX_PATH];

	GetDlgItemText( ParentId, szGameDir, _MAX_PATH );

	filename Path;
	Path.set( AskDirectoryOpen( szGameDir, m_hWnd ) );
	Path.relativepath( szGameDir );

	SetDlgItemText( Id, Path.get() );
}

void ProjectPaths::OnBrowseGameDir( void )
{
	SetDlgItemText( IDC_GAMEDIR, AskDirectoryOpen( 0, m_hWnd ) );
}

void ProjectPaths::OnBrowseDataDir( void )
{
	OnBrowse( IDC_GAMEDIR, IDC_DATADIR );
}

void ProjectPaths::OnBrowseDataPC( void )
{
	OnBrowse( IDC_GAMEDIR, IDC_DATAPC );
}

void ProjectPaths::OnBrowseDataDX( void )
{
	OnBrowse( IDC_GAMEDIR, IDC_DATADX );
}

void ProjectPaths::OnBrowseDataPS2( void )
{
	OnBrowse( IDC_GAMEDIR, IDC_DATAPS2 );
}

void ProjectPaths::OnBrowseDataGCN( void )
{
	OnBrowse( IDC_GAMEDIR, IDC_DATAGCN );
}

void ProjectPaths::OnBrowseDataXBOX( void )
{
	OnBrowse( IDC_GAMEDIR, IDC_DATAXBOX );
}

void ProjectPaths::OnBrowseDataPath( void )
{
	TCHAR szGameDir[_MAX_PATH];
	TCHAR szDataDir[_MAX_PATH];

	GetDlgItemText( IDC_GAMEDIR, szGameDir, _MAX_PATH );
	GetDlgItemText( IDC_DATADIR, szDataDir, _MAX_PATH );

	filename GameDataDir;
	GameDataDir.set( Printf( "%s\\%s", szGameDir, szDataDir ) );

	filename Path;
	Path.set( AskDirectoryOpen( GameDataDir.get(), m_hWnd ) );

	Path.relativepath( GameDataDir.get() );

	SetDlgItemText( IDC_DATAPATH, Path.get() );
}

void ProjectPaths::OnDefaultDir( void )
{
	SetDlgItemText( IDC_DATADIR, "Data" );
	SetDlgItemText( IDC_DATAPC, "Data_PC" );
	SetDlgItemText( IDC_DATAPS2, "Data_PS2" );
	SetDlgItemText( IDC_DATAGCN, "Data_GCN" );
	SetDlgItemText( IDC_DATAXBOX, "Data_XBOX" );
	SetDlgItemText( IDC_DATAPATH, "Screens" );
}

void ProjectPaths::OnUpdateCommandUI( CCmdUI *pCmdUI )
{
	switch( pCmdUI->m_nID )
	{
	}

	return;
}