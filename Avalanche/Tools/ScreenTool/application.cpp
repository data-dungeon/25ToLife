#include "pch.h"
#include "application.h"
#include "direct3d.h"
#include "document.h"
#include "mainframe.h"
#include "view.h"
#include "resource.h"
#include "formattedfile.h"

//=============================================================================
// SevenupCommandLine::SevenupCommandLine
//=============================================================================

SevenupCommandLine::SevenupCommandLine( void )
{
	m_DoExport = FALSE;
}

//=============================================================================
// SevenupCommandLine
//=============================================================================

void SevenupCommandLine::ParseParam( const char* pParam, BOOL Flag, BOOL Last )
{
	if( Flag && lstrcmpi( pParam, "export" ) == 0 )
		m_DoExport = TRUE;
	else
	if( m_DoExport && m_strExportName.IsEmpty() )
		m_strExportName = pParam;
	else
		CCommandLineInfo::ParseParam( pParam, Flag, Last );

	ParseLast( Last );
}

SevenupApp App;

BEGIN_MESSAGE_MAP( SevenupApp, CWinApp )
	ON_COMMAND( ID_FILE_NEW, OnFileNew )
	ON_COMMAND( ID_FILE_OPEN, OnFileOpen )
	ON_COMMAND( ID_VIEW_OPTIONS, OnViewOptions )
END_MESSAGE_MAP()

//=============================================================================
// SevenupApp::SevenupApp
//
//		Calls SystemParametersInfo to retrieve the desktop work area (the area
//		of the desktop not obscured by task bars and office bars...)
//		This becomes the default startup rect for the application window.
//
//=============================================================================

SevenupApp::SevenupApp( void )
{
	SystemParametersInfo( SPI_GETWORKAREA, 0, (LPVOID) &m_StartupRect, 0 );
}

//=============================================================================
// SevenupApp::~SevenupApp
//=============================================================================

SevenupApp::~SevenupApp()
{
}

//=============================================================================
// SevenupApp::SetThumbnailDirectory
//
//		Sets the current directory for browsing thumbnail images.  This setting
//		is saved in the registry when the application exits and restored on
//		startup.
//=============================================================================

void SevenupApp::SetThumbnailDirectory( TCHAR *pPathName )
{
	m_ThumbnailPath.set( pPathName );
}

//=============================================================================
// SevenupApp::SetCurrentDirectory
//
//		Sets the current directory for screen project files.  This setting is
//		saved in the registry when the application exits and restored on startup.
//		The current directory is the path specified in the open/save dialog.
//=============================================================================

void SevenupApp::SetCurrentDirectory( TCHAR *pPathName )
{
	m_CurrentDirectory.set( pPathName );
}

//=============================================================================
// SevenupApp::GetCurrentDirectory
//
//		Returns the current directory as a string.
//=============================================================================

TCHAR* SevenupApp::GetCurrentDirectory( void )
{
	return m_CurrentDirectory.get();
}

//=============================================================================
// SevenupApp::GetStartupDirectory
//
//		Returns the startup directory as a string.
//=============================================================================

TCHAR* SevenupApp::GetStartupDirectory( void )
{
	return m_StartupDirectory.get();
}

//=============================================================================
// SevenupApp::DefaultGuiFont
//
//		Returns a pointer to the default gui font.  The default gui font is the
//		font used by Windows to display messages in message boxes and button
//		faces.  It is initialized during application startup.
//=============================================================================

CFont *SevenupApp::DefaultGuiFont( BOOL Bold )
{
	if( Bold )
		return &m_DefaultBoldFont;

	return &m_DefaultGuiFont;
}

//=============================================================================
// SevenupApp::DefaultTextureHandle
//
//		Returns a handle to the default texture.  The default texture is usually
//		an opaque 8x8 white image.
//
//=============================================================================

t_Handle SevenupApp::DefaultTextureHandle( void )
{
	return m_DefaultTextureHandle;
}

//=============================================================================
// SevenupApp::GetGameFont
//
//		Returns the currently selected game font.
//
//=============================================================================

Font* SevenupApp::GetGameFont( void )
{
	MainFrame* pMainFrame = (MainFrame*) AfxGetMainWnd();
	Font* pGameFont;

	if( pMainFrame )
		pGameFont = Font::FindFont( pMainFrame->m_ToolBar.GetCurrentFont() );
	else
		pGameFont = Font::GetFirstFont();

	return pGameFont;
}

//=============================================================================
// SevenupApp::GetSystemFont
//
//		Returns the currently selected game font.
//
//=============================================================================

Font* SevenupApp::GetSystemFont( void )
{
	Font* pSystemFont = m_pSystemFont;

	if( !pSystemFont )
		pSystemFont = Font::GetFirstFont();

	return pSystemFont;
}

//=============================================================================
// SevenupApp::OnOptionsOK
//
//		Called by the options dialog when the OK buttion is pressed.
//
//=============================================================================

void SevenupApp::OnOptionsOK( void )
{
	m_OptionsDialog.GetExportUtility( m_ExportUtility );
}

//=============================================================================
// SevenupApp::AskDocOpen
//=============================================================================

BOOL SevenupApp::AskDocOpen( CString &FileName )
{
	if( !m_pDocManager )
		return FALSE;

	return m_pDocManager->DoPromptFileName(FileName, AFX_IDS_OPENFILE, OFN_FILEMUSTEXIST, TRUE, NULL );
}

//=============================================================================
// SevenupApp::AskDocSave
//=============================================================================

BOOL SevenupApp::AskDocSave( CString &FileName )
{
	if( !m_pDocManager )
		return FALSE;

	return m_pDocManager->DoPromptFileName(FileName, AFX_IDS_SAVEFILE, OFN_HIDEREADONLY, FALSE, NULL );
}

//=============================================================================
// SevenupApp::InitInstance
//
//		Called by the framework when the application starts up, this function
//		initializes everything and parses the command line.
//
//=============================================================================

BOOL SevenupApp::InitInstance( void )
{
	if( !CWinApp::InitInstance() )
		return FALSE;

	SevenupCommandLine cli;
	ParseCommandLine( cli );

	InitStartupDirectory();

	CoInitialize(0);

	InitRegistry();

	InitImageList();

	InitDocTemplates();

	InitGuiFonts();

	if( !InitMainFrame() )
		return FALSE;

	InitDirect3D();

	InitGameEngine();

	if( !InitProject() )
		return FALSE;

	InitCurrentDirectory();

	if( !ProcessCommandLine(cli) )
		return FALSE;

	return TRUE;
}

//=============================================================================
// SevenupApp::InitStartupDirectory
//=============================================================================

void SevenupApp::InitStartupDirectory( void )
{
	TCHAR szStartupDirectory[_MAX_PATH];
	::GetCurrentDirectory( _MAX_PATH, szStartupDirectory );

	m_StartupDirectory.set( szStartupDirectory );
}

//=============================================================================
// SevenupApp::InitRegistery
//
//		Called when the application is initialized, this function retrieves
//		settings from the registry.
//
//=============================================================================

void SevenupApp::InitRegistry( void )
{
	SetRegistryKey( _T("Avalanche") );
	LoadStdProfileSettings();

	ASSERT(m_pRecentFileList);

	m_ExportUtility.set( GetProfileString( "Settings", "ExportUtility" ) );
	m_ThumbnailPath.set( GetProfileString( "Settings", "ThumbnailPath" ) );
	m_CurrentDirectory.set( GetProfileString( "Settings", "CurrentDirectory" ) );
}

//=============================================================================
// SevenupApp::InitImageList
//
//		Called when the application is initialized, this function initializes
//		the image list used by the application.  Currently it appears not be
//		working...
//
//=============================================================================

void SevenupApp::InitImageList( void )
{
	static const UINT Images[] =
	{
		IDB_TVSET16B,
		IDB_TVSET16,
		IDB_APPICON16B,
		IDB_APPICON16,
		IDB_LETTER16B,
		IDB_LETTER16,
	};

	BOOL bResult = m_ImageList.Create( 16, 16, ILC_COLOR32, 16, 16 );

	if( bResult )
	{
		HINSTANCE hInstance = AfxGetInstanceHandle();
		HBITMAP hbmImage;

		int c = sizeof(Images)/sizeof(Images[c]);

		for( int i = 0; i < c; i++ )
		{
			hbmImage = GdiLoadBitmap( hInstance, Images[i], LR_CREATEDIBSECTION );

			if( hbmImage )
				ImageList_Add( m_ImageList.m_hImageList, hbmImage, 0 );
		}
	}

	return;
}

//=============================================================================
// SevenupApp::InitDocTemplates
//
//		Called when the application is initialized, this function initializes
//		the document template.  The document template is required by the MFC
//		framework for document/view applications such as this.
//
//=============================================================================

void SevenupApp::InitDocTemplates( void )
{
	CSingleDocTemplate *pDocTemplate;

	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS( SevenupDoc ),
		RUNTIME_CLASS( MainFrame ),
		RUNTIME_CLASS( SevenupView ) );

	AddDocTemplate( pDocTemplate );
}

//=============================================================================
// SevenupApp::InitGameEngine
//
//		Called when the application is initialized, this function initializes
//		the game engine.  The run-time class objects are initialized and fonts
//		and loaded from the font directory.
//
//=============================================================================

void SevenupApp::InitGameEngine( void )
{
	HudObject::RegisterClass( RUNTIME_CLASSOBJECT(Sprite) );
	HudObject::RegisterClass( RUNTIME_CLASSOBJECT(Text) );
	HudObject::RegisterClass( RUNTIME_CLASSOBJECT(Window) );

	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT(Screen) );
	SpriteInterface::RegisterClass( RUNTIME_CLASSOBJECT(Button) );

	InitGameFonts();

	m_DefaultTextureHandle = dblLoad( "Textures\\DefaultTexture.bmp" );
}

//=============================================================================
// SevenupApp::InitGameFonts
//
//		Called when the application is initialized, this function initializes
//		fonts used by the game engine.  The fonts are stored in the application's
//		data\fonts directory with either a .dbl or .tga image extention and .fnt
//		for the font definition file.  When a font is successfully loaded, it is
//		placed into a drop-down box on the tool bar.
//
//=============================================================================

void SevenupApp::InitGameFonts( void )
{
	m_pSystemFont = Font::LoadSystemFont( "impact4" );

	FILENAME FileName;
	BOOL FindResult;
	WIN32_FIND_DATA fd;
	HANDLE hFindFile;

	hFindFile = FindFirstFile( "data\\fonts\\*.fnt", &fd );

	while( hFindFile != INVALID_HANDLE_VALUE )
	{
		FileName.delext( fd.cFileName );

		Font::LoadFont( FileName.get() );

		FindResult = FindNextFile( hFindFile, &fd );

		if( FindResult == FALSE )
		{
			FindClose( hFindFile );
			hFindFile = INVALID_HANDLE_VALUE;
		}
	}

	MainFrame *pMainFrame = (MainFrame *) AfxGetMainWnd(); 

	if( !pMainFrame )
		return;

	Font *pGameFont = Font::GetFirstFont();

	while( pGameFont )
	{
		pMainFrame->AddFont( pGameFont->m_FontName.get() );

		pGameFont = pGameFont->Next();
	}

	return;
}

//=============================================================================
// SevenupApp::InitGuiFonts
//
//		Called when the application is initialized, this function initializes
//		the gui fonts used by the application.  The default gui font is font is
//		the same font used by windows to display messages in the dialog box and
//		text on button faces.
//
//=============================================================================

void SevenupApp::InitGuiFonts( void )
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
// SevenupApp::InitMainFrame
//=============================================================================

BOOL SevenupApp::InitMainFrame( void )
{
	OnFileNew();

	if( !m_pMainWnd )
		return FALSE;

	return TRUE;
}

//=============================================================================
// SevenupApp::InitDirect3D
//
//		Called when application is initialized, this function initializes the
//		Direct3D renderer.
//
//=============================================================================

BOOL SevenupApp::InitDirect3D( void )
{
	if( !m_pMainWnd )
		return FALSE;

	BOOL bResult = d3d.Initialize( m_pMainWnd->m_hWnd, 0, 0, D3DFMT_UNKNOWN, TRUE );

	if( bResult )
		return TRUE;

	m_pMainWnd->MessageBox( "Direct3D could not be initialized", 0, MB_OK|MB_ICONEXCLAMATION );

	return FALSE;
}

//=============================================================================
// SevenupApp::InitProject
//=============================================================================

BOOL SevenupApp::InitProject( void )
{
	return Project.Initialize();
}

//=============================================================================
// SevenupApp::InitCurrentDirectory
//=============================================================================

void SevenupApp::InitCurrentDirectory( void )
{
	::SetCurrentDirectory( m_CurrentDirectory.get() );
}

//=============================================================================
// SevenupApp::ExitInstance
//
//		Called by the frame work when the application is terminated, this
//		function shuts down necessary systems and saves settings to the registry.
//
//=============================================================================

int SevenupApp::ExitInstance( void )
{
//	SaveStdProfileSettings();

	SaveSettings();

	ShutdownGameEngine();

	ShutdownDirect3D();

	ShutdownProject();

	return CWinApp::ExitInstance();
}

//=============================================================================
// SevenupApp::SaveSettings
//
//		Called when the application is terminated, this function saves settings
//		to the registry.
//
//=============================================================================

void SevenupApp::SaveSettings( void )
{
	TCHAR szCurrentDirectory[_MAX_PATH];

	::GetCurrentDirectory( _MAX_PATH, szCurrentDirectory );
	SetCurrentDirectory( szCurrentDirectory );

	WriteProfileString( "Settings", "ExportUtility", m_ExportUtility.get() );
	WriteProfileString( "Settings", "ThumbnailPath", m_ThumbnailPath.get() );
	WriteProfileString( "Settings", "CurrentDirectory", m_CurrentDirectory.get() );
}

//=============================================================================
// SevenupApp::ShutdownGameEngine
//
//		Called when the application terminates, this function removes fonts
//		loaded by the game engine.
//
//=============================================================================

void SevenupApp::ShutdownGameEngine( void )
{
	ProjectItem::ClearItemNames();

	SpriteInterface::UnregisterAllClasses();
	HudObject::UnregisterAllClasses();
	Font::FlushAllFonts();
}

//=============================================================================
// SevenupApp::ShutdownDirect3D
//
//		Called when the application is terminated, this function cleanly shuts
//		down the Direct3D renderer.
//
//=============================================================================

void SevenupApp::ShutdownDirect3D( void )
{
	d3d.Terminate();
}

//=============================================================================
// SevenupApp::ShutdownProject
//
//		Called when the application is terminated, this function frees up memory
//		and resources created by the project.
//
//=============================================================================

void SevenupApp::ShutdownProject( void )
{
	ProjectItem::ClearItemNames();
}

//=============================================================================
// SevenupApp::ParseCommandLine
//
//		Called when the application is initialized, this function Parsees
//		the command line.
//
//=============================================================================

void SevenupApp::ParseCommandLine( SevenupCommandLine &cli )
{
	CWinApp::ParseCommandLine(cli);
}

//=============================================================================
// SevenupApp::ProcessCommandLine
//
//		Called when the application is initialized, this function processes
//		the command line.
//
//=============================================================================

BOOL SevenupApp::ProcessCommandLine( SevenupCommandLine &cli )
{
	if( !ProcessShellCommand( cli ) )
		return FALSE;

	if( cli.m_DoExport )
	{
		CDocument* pDocument;
		CDocTemplate* pDocTemplate;
		POSITION pos;
		POSITION pos2;
		
		pos = GetFirstDocTemplatePosition();

		while( pos )
		{
			pDocTemplate = (CDocTemplate*) GetNextDocTemplate(pos);
			pos2 = pDocTemplate->GetFirstDocPosition();

			while( pos2 )
			{
				pDocument = pDocTemplate->GetNextDoc(pos2);

				if( pDocument && pDocument->IsKindOf(RUNTIME_CLASS(SevenupDoc)) )
				{
					SevenupDoc* pSevenupDoc = (SevenupDoc*) pDocument;

					pSevenupDoc->ExportGameFiles( cli.m_strExportName );
				}
			}
		}

		if( m_pMainWnd )
			m_pMainWnd->DestroyWindow();
	}

	return TRUE;
}

//=============================================================================
// SavenupApp::OnFileNew
//
//		Called by the framework when the user clicks File/New.
//
//=============================================================================

void SevenupApp::OnFileNew( void )
{
	CWinApp::OnFileNew();
}

//=============================================================================
// SavenupApp::OnFileOpen
//
//		Called by the framework when the user clicks File/Open.
//
//=============================================================================

void SevenupApp::OnFileOpen( void )
{
	CWinApp::OnFileOpen();
}

//=============================================================================
// SavenupApp::OnViewOptions
//
//		Called by the framework when the user clicks View/Options.
//
//=============================================================================

void SevenupApp::OnViewOptions( void )
{
	CWnd *pParent = AfxGetMainWnd();

	m_OptionsDialog.SetExportUtility( m_ExportUtility.get() );

	if( !m_OptionsDialog.m_hWnd )
		m_OptionsDialog.Create( IDD_OPTIONS, pParent );

	if( m_OptionsDialog.m_hWnd )
	{
		RECT rc;

		if( pParent )
			pParent->GetWindowRect( &rc );
		else
		{
			SystemParametersInfo( SPI_GETWORKAREA, 0, &rc, 0 );
		}

		Win32_CenterWindow( m_OptionsDialog.m_hWnd, rc );

		m_OptionsDialog.ShowWindow( SW_SHOW );
		m_OptionsDialog.UpdateWindow();
	}

	return;
}


//=============================================================================
// SevenupApp::OnIdle
//
//		Called by the framework when the application is idle.
//=============================================================================

BOOL SevenupApp::OnIdle( LONG Count )
{
	return CWinApp::OnIdle( Count );
}