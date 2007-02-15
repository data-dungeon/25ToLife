#include "pch.h"
#include "application.h"
#include "document.h"
#include "mainframe.h"
#include "view.h"
#include "resource.h"

UndibbleApp App;

BEGIN_MESSAGE_MAP( UndibbleApp, CWinApp )
	ON_COMMAND( ID_FILE_NEW, OnFileNew )
	ON_COMMAND( ID_FILE_OPEN, OnFileOpen )
END_MESSAGE_MAP()

//=============================================================================
// UndibbleApp::UndibbleApp
//=============================================================================

UndibbleApp::UndibbleApp( void )
{
	RECT rc;

	SystemParametersInfo( SPI_GETWORKAREA, 0, (LPVOID) &rc, 0 );

	int cx = rc.left + (rc.right - rc.left) / 2;
	int cy = rc.top + (rc.bottom - rc.top) / 2;

	int Width = 640;
	int Height = 480;

	m_StartupRect = GdiMakeRect( cx - Width/2, cy - Height/2, Width, Height );
}

//=============================================================================
// UndibbleApp::~UndibbleApp
//=============================================================================

UndibbleApp::~UndibbleApp()
{
}

//=============================================================================
// UndibbleApp::DefaultGuiFont
//=============================================================================

CFont *UndibbleApp::DefaultGuiFont( BOOL Bold )
{
	if( Bold )
		return &m_DefaultBoldFont;

	return &m_DefaultGuiFont;
}

//=============================================================================
// UndibbleApp::InitInstance
//=============================================================================

BOOL UndibbleApp::InitInstance( void )
{
	if( !CWinApp::InitInstance() )
		return FALSE;

	InitRegistry();

	InitImageList();

	InitDocTemplates();

	InitGuiFonts();

	if( !ProcessCommandLine() )
		return FALSE;

	return TRUE;
}

//=============================================================================
// UndibbleApp::InitRegistery
//=============================================================================

void UndibbleApp::InitRegistry( void )
{
	SetRegistryKey( _T("Avalanche") );
	LoadStdProfileSettings();
}

//=============================================================================
// UndibbleApp::InitImageList
//=============================================================================

void UndibbleApp::InitImageList( void )
{
}

//=============================================================================
// UndibbleApp::InitDocTemplates
//=============================================================================

void UndibbleApp::InitDocTemplates( void )
{
	CSingleDocTemplate *pDocTemplate;

	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS( UndibbleDoc ),
		RUNTIME_CLASS( MainFrame ),
		RUNTIME_CLASS( UndibbleView ) );

	AddDocTemplate( pDocTemplate );
}

//=============================================================================
// UndibbleApp::InitGuiFonts
//=============================================================================

void UndibbleApp::InitGuiFonts( void )
{
	HFONT hFont = (HFONT) GetStockObject( DEFAULT_GUI_FONT );

	if( hFont )
	{
		HFONT hGuiFont = GdiCreateSimilarFont( hFont, 0, FW_DONTCARE, 0, 0 );

		if( hGuiFont )
			m_DefaultGuiFont.Attach( hFont );

		HFONT hBoldFont = GdiCreateSimilarFont( hGuiFont, 0, FW_BOLD, 0, 0 );

		if( hBoldFont )
			m_DefaultBoldFont.Attach( hFont );
	}

	return;
}

//=============================================================================
// UndibbleApp::ExitInstance
//=============================================================================

int UndibbleApp::ExitInstance( void )
{
	SaveStdProfileSettings();

	SaveSettings();

	return CWinApp::ExitInstance();
}

//=============================================================================
// UndibbleApp::SaveSettings
//=============================================================================

void UndibbleApp::SaveSettings( void )
{
}

//=============================================================================
// UndibbleApp::ProcessCommandLine
//=============================================================================

BOOL UndibbleApp::ProcessCommandLine( void )
{
	CCommandLineInfo cli;
	ParseCommandLine( cli );

	if( !ProcessShellCommand( cli ) )
		return FALSE;

	return TRUE;
}

//=============================================================================
// SavenupApp::OnFileNew
//=============================================================================

void UndibbleApp::OnFileNew( void )
{
	CWinApp::OnFileNew();
}

void UndibbleApp::OnFileOpen( void )
{
	CWinApp::OnFileOpen();
}

void UndibbleApp::OnFileUndibble( void )
{
}