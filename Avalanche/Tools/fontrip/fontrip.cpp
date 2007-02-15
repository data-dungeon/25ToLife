#include <afxres.h>
#include <math.h>
#include <assert.h>

#include "fontrip.h"
#include "resource.h"

static char *szQuality[5] = {"None", "2x", "3x", "4x", "16x" };

static wchar_t sCurrentCharacterMap[ 256 ] =
{
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

static int sCurrentCharacterMapMaxIndex = 255;

//=============================================================================
// Printf
//=============================================================================

char *Printf( const char *fmt, ... )
{
	static int i = 0;
	static char szBuffer[8][512];

	va_list args;
	va_start( args, fmt );
	vsprintf( szBuffer[i&7], fmt, args );
	va_end( args );

	return szBuffer[i++&7];
}

//=============================================================================
// ColorBox::WhenPaint
//=============================================================================

void ColorBox::WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct )
{
	HBRUSH hbrush = CreateSolidBrush( m_bkColor );

	RECT rc;
	GetClientRect( &rc );

	FillRect( hdcPaint, &rc, hbrush );

	DeleteObject( hbrush );

	return;
}

//=============================================================================
// PictureBox::PictureBox()
//=============================================================================

PictureBox::PictureBox()
{
	m_hbmBitmap = 0;
}

//=============================================================================
// PictureBox::~PictureBox()
//=============================================================================

PictureBox::~PictureBox()
{
	Release();
}

//=============================================================================
// PictureBox::Release
//=============================================================================

void PictureBox::Release()
{
	if( m_hbmBitmap )
		ReleaseDIB( m_hbmBitmap );

	m_hbmBitmap = 0;
}

//=============================================================================
// PictureBox::WhenDestroy
//=============================================================================

void PictureBox::WhenDestroy()
{
	CWindow::WhenDestroy();

	Release();
}

//=============================================================================
// PictureBox::WhenLButtonUp
//=============================================================================

void PictureBox::WhenLButtonUp( DWORD dwFlags, int x, int y )
{
	LPCTSTR lpszFileName = AskFileOpen( ParentHandle(), "Open...", "Bitmap files (*.bmp)\0*.bmp\0\0" );

	if( lpszFileName )
		Release(), m_hbmBitmap = LoadDIB( lpszFileName );

	return;
}

//=============================================================================
// PictureBox::WhenPaint
//=============================================================================

void PictureBox::WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct )
{
	if( !m_hbmBitmap )
		CWindow::WhenPaint( hdcPaint, pstruct );
	else
	{
		RECT rc;
		GetClientRect( &rc );

		HighQualityStretchBlt( hdcPaint, 0, 0, rc.right, rc.bottom, m_hbmBitmap, 0, 0, GetBitmapWidth( m_hbmBitmap ), GetBitmapHeight( m_hbmBitmap ) );
	}

	return;
}

//=============================================================================
// PreviewWindow::WhenCreate
//=============================================================================

BOOL PreviewWindow::WhenCreate( LPCREATESTRUCT lpCS )
{
	if( !CWindow::WhenCreate( lpCS ) )
		return FALSE;

	LoadMenu( IDR_MENU );

	m_bAlpha = TRUE;

	return TRUE;
}

//=============================================================================
// PreviewWindow::WhenDestroy
//=============================================================================

void PreviewWindow::WhenDestroy( void )
{
	HMENU hMenu = GetMenu();

	if( hMenu )
		DestroyMenu();

	CWindow::WhenDestroy();
}

//=============================================================================
// PreviewWindow::WhenCommand
//=============================================================================

BOOL PreviewWindow::WhenCommand( HWND hWindow, int nCommand, int nNotifyCode )
{
	switch( nCommand )
	{
	case ID_FILE_SAVE_AS:
		{
			LPCTSTR lpszFileName = AskFileSave( WindowHandle(), NULL, "Bitmap Files (*.bmp)\0*.bmp\0Targa Files (*.tga)\0*.tga\0\0" );

			if( lpszFileName )
			{
				if( !SaveDIB( lpszFileName, m_hbmPreview, NULL ) )
					MessageBox( "Unable to save the file!", MB_OK|MB_ICONEXCLAMATION );

				TCHAR *extention = (TCHAR *) &lpszFileName[strlen(lpszFileName)-4];
				
				extention[0] = '.';
				extention[1] = 'f';
				extention[2] = 'n';
				extention[3] = 't';

				HANDLE hFile = CreateFile( lpszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

				if( hFile != INVALID_HANDLE_VALUE )
				{
					FileWrite( hFile, m_pMetrics, sizeof( FontMetrics ) );

					CloseHandle( hFile );
				}
			}
		}
		break;
	case ID_VIEW_ALPHACHANNEL:
		m_bAlpha = !m_bAlpha;
		Invalidate();
		break;
	}

	return CWindow::WhenCommand( hWindow, nCommand, nNotifyCode );
}

//=============================================================================
// PreviewWindow::WhenPaint
//=============================================================================

void PreviewWindow::WhenPaint( HDC hdcPaint, PAINTSTRUCT *pstruct )
{
	int cxwindow = ClientWidth();
	int cywindow = ClientHeight();
	int cxbitmap = GetBitmapWidth( m_hbmPreview );
	int cybitmap = GetBitmapHeight( m_hbmPreview );

	if( !m_bAlpha )
		BltBitmap( hdcPaint, (cxwindow - cxbitmap) / 2, (cywindow - cybitmap) / 2, cxbitmap, cybitmap, m_hbmPreview, 0, 0, cxbitmap, cybitmap );
	else
	{
		int cxbitmap = GetBitmapWidth( m_hbmPreview );
		int cybitmap = GetBitmapHeight( m_hbmPreview );

		HBITMAP hbmTemp = CreateDIB( GetBitmapWidth( m_hbmPreview ), GetBitmapHeight( m_hbmPreview ), 32 );

		if( hbmTemp )
		{
			BeginDIB( 0, hbmTemp );
			BeginDIB( 1, m_hbmPreview);

			COLORREF c;

			for( int y = 0; y < cybitmap; y++ )
			{
				for( int x = 0; x < cxbitmap; x++ )
				{
					c = GetPixelU( 1, x, y );

					c = ColorBlend( c, 0xaa66aa, 255 - (c>>24) );

					SetPixelU( 0, x, y, c );
				}
			}
		}

		BltBitmap( hdcPaint, (cxwindow - cxbitmap) / 2, (cywindow - cybitmap) / 2, cxbitmap, cybitmap, hbmTemp, 0, 0, cxbitmap, cybitmap );
	}

	return;
}

//=============================================================================
// PreviewWindow::WhenEraseBkgnd
//=============================================================================

void PreviewWindow::WhenEraseBkgnd( HDC hdcErase )
{
	RECT rc;
	GetClientRect( &rc );

	FillRect( hdcErase, &rc, (HBRUSH) GetStockObject( BLACK_BRUSH ) );
}

//=============================================================================
// FontWindow::EnumFontsCallback
//=============================================================================

int CALLBACK FontWindow::EnumFontsCallback( LPLOGFONT pLogFont, LPNEWTEXTMETRIC *pTextMetric, DWORD dwType, FontWindow *pFontWindow )
{
	if( !pFontWindow )
		return FALSE;

	LRESULT i = pFontWindow->SendDlgItemMessage( IDC_LIST_TYPEFACES, LB_ADDSTRING, 0, (LPARAM) pLogFont->lfFaceName );
	pFontWindow->SendDlgItemMessage( IDC_LIST_TYPEFACES, LB_SETITEMDATA, (WPARAM) i, (LPARAM) pLogFont->lfCharSet );

	return TRUE;
}	

//=============================================================================
// FontWindow::FontWindow
//=============================================================================

FontWindow::FontWindow( void )
{
	m_hbmOutput = NULL;
	m_hFont = NULL;
	m_crFont = 0xffffff;
}

//=============================================================================
// FontWindow::ReadCharacterMap()
//=============================================================================

void FontWindow::ProcessCharacterMap( LPCTSTR lpszFileName )
{
	if ( lpszFileName == NULL )
	{
		sCurrentCharacterMapMaxIndex = 255;
		for ( int i = 0; i <= sCurrentCharacterMapMaxIndex; ++i )
		{
			sCurrentCharacterMap[ i ] = i;
		}
	}
	else
	{
		// open the file
		HANDLE hFile = CreateFile(
			lpszFileName,
			GENERIC_READ,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

		assert( hFile != INVALID_HANDLE_VALUE );

		// read in the mapping
		DWORD dwFileSize = GetFileSize( hFile, NULL );
		DWORD dwBytesRead = 0;
		char *pBuff = new char[ dwFileSize + 1 ];
		BOOL BSuccess = ReadFile( hFile, pBuff, dwFileSize, &dwBytesRead, NULL );
		assert( BSuccess == TRUE );
		assert( dwBytesRead == dwFileSize );
		pBuff[ dwFileSize ] = '\0';

		// close the file
		CloseHandle( hFile );

		// process the loaded map
		int iMaxIndex = 0;
		int iIndex = -1;
		int iValue = -1;
		char *pLineStart = pBuff;
		char *pCurr = pBuff;
		while ( *pCurr )
		{
			// process the next line
			while ( *pCurr != '\n' )
			{
				++pCurr;
			}
			sscanf( pLineStart, "%2x %4x", &iIndex, &iValue );

			assert( ( iIndex >= 0 ) && ( iIndex <= 255 ) );
			sCurrentCharacterMap[ iIndex ] = iValue;
			++iMaxIndex;

			if ( *pCurr == '\n' )
			{
				pLineStart = ++pCurr;
			}
		}
		sCurrentCharacterMapMaxIndex = ( iMaxIndex - 1 );
	}
}

//=============================================================================
// FontWindow::EnumFonts()
//=============================================================================

void FontWindow::EnumFonts( void )
{
	HDC hdc = GetDC( NULL );

	LOGFONT lf;

	lf.lfCharSet			= ANSI_CHARSET;
	lf.lfFaceName[0]		= '\0';
	lf.lfPitchAndFamily	= 0;
	lf.lfQuality			= NONANTIALIASED_QUALITY;

	EnumFontFamiliesEx( hdc, &lf, (FONTENUMPROC) EnumFontsCallback, (LPARAM) this, 0 );

	lf.lfCharSet			= SHIFTJIS_CHARSET; /* Japanese */
	//lf.lfCharSet			= EASTEUROPE_CHARSET; /* Polish */
	//lf.lfCharSet			= RUSSIAN_CHARSET; /* Russian */
	lf.lfFaceName[0]		= '\0';
	lf.lfPitchAndFamily	= 0;
	lf.lfQuality			= NONANTIALIASED_QUALITY;

	EnumFontFamiliesEx( hdc, &lf, (FONTENUMPROC) EnumFontsCallback, (LPARAM) this, 0 );
 
	ReleaseDC( NULL, hdc );

	SendDlgItemMessage( IDC_LIST_STYLE, LB_ADDSTRING, 0, (LPARAM) "Regular" );
	SendDlgItemMessage( IDC_LIST_STYLE, LB_ADDSTRING, 0, (LPARAM) "Italic" );
	SendDlgItemMessage( IDC_LIST_STYLE, LB_ADDSTRING, 0, (LPARAM) "Bold" );
	SendDlgItemMessage( IDC_LIST_STYLE, LB_ADDSTRING, 0, (LPARAM) "Bold Italic" );

	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "8" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "9" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "10" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "11" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "12" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "14" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "16" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "18" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "20" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "22" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "24" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "26" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "28" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "36" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "48" );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_ADDSTRING, 0, (LPARAM) "72" );

	SetDlgItemInt( IDC_EDIT_SIZE, 8 );

	SetDlgItemText( IDC_EDIT_PREVIEW, "AaBbCcDdEe 0123456789" );

	SendDlgItemMessage( IDC_LIST_TYPEFACES, LB_SETCURSEL, 0, 0 );
	SendDlgItemMessage( IDC_LIST_STYLE, LB_SETCURSEL, 0, 0 );
	SendDlgItemMessage( IDC_LIST_SIZE, LB_SETCURSEL, 0, 0 );

	UpdateFont();	
}

//=============================================================================
// FontWindow::SelectFont
//=============================================================================

BOOL FontWindow::SelectFont( LPCTSTR lpszFontName, int nBold, BOOL bItalic, int nSize, int nCharSet )
{
	if( m_hFont )
		DeleteObject( m_hFont );

	strcpy( m_szFontName, lpszFontName );

	m_nFontSize = nSize;
	m_nCharSet = nCharSet;
	m_bItalic = bItalic;
	m_nBold = nBold;

	m_hFont = CreateFont(-m_nFontSize,
								 0,
								 0,
								 0,
								 m_nBold,
								 bItalic,
								 FALSE,
								 FALSE,
								 nCharSet,
								 OUT_DEFAULT_PRECIS,
								 CLIP_DEFAULT_PRECIS,
								 DEFAULT_QUALITY,
								 DEFAULT_PITCH|FF_DONTCARE,
								 lpszFontName );

	if( !m_hFont )
		return FALSE;

	SendDlgItemMessage( IDC_EDIT_PREVIEW, WM_SETFONT, (WPARAM) m_hFont, TRUE );

	return TRUE;
}

void FontWindow::UpdateFont( void )
{
	int nSelection = SendDlgItemMessage( IDC_LIST_TYPEFACES, LB_GETCURSEL, 0, 0 );
	
	TCHAR szFontName[256] = "";
	int nCharSet = DEFAULT_CHARSET;

	SendDlgItemMessage( IDC_LIST_TYPEFACES, LB_GETTEXT, nSelection, (LPARAM) szFontName );
	nCharSet = SendDlgItemMessage( IDC_LIST_TYPEFACES, LB_GETITEMDATA, nSelection, 0 );
	nSelection = SendDlgItemMessage( IDC_LIST_STYLE, LB_GETCURSEL, 0, 0 );

	int nBold = FW_DONTCARE;
	BOOL bItalic = FALSE;

	if( nSelection == 1 || nSelection == 3 )
		bItalic = TRUE;

	if( nSelection == 2 || nSelection == 3 )
		nBold = FW_BOLD;

	SelectFont( szFontName, nBold, bItalic, GetDlgItemInt( IDC_EDIT_SIZE ) * 7 / 4, nCharSet );
}

//=============================================================================
// FontWindow::WhenOptimize
//=============================================================================

void FontWindow::WhenOptimize( void )
{
	int x, y, i;

	BOOL bUseCharacterMap = FALSE;
	if( GetDlgItemCheck( IDC_CHECK_USECHARACTERMAP ) )
		bUseCharacterMap = TRUE;

	TCHAR szCharacterMapFileName[ _MAX_PATH ];
	int iFileNameLength = GetDlgItemText( IDC_EDIT_CHARACTERMAPFILENAME, szCharacterMapFileName, _MAX_PATH );
	if ( ( bUseCharacterMap ) && ( iFileNameLength > 0 ) )
	{	// use the provided character map file
		ProcessCharacterMap( szCharacterMapFileName );
	}
	else
	{	// reset to the default (ASCII) character map.
		ProcessCharacterMap();
	}

	if( m_hbmOutput )
		DeleteObject( m_hbmOutput );

	m_hbmOutput = CreateDIB( GetDlgItemInt( IDC_EDIT_IMAGEWIDTH ),
									 GetDlgItemInt( IDC_EDIT_IMAGEHEIGHT ),
									 32 );

	int bmWidth = GetBitmapWidth( m_hbmOutput );
	int bmHeight = GetBitmapHeight( m_hbmOutput );

	int nScale = 1;
	BOOL bTmRC = FALSE;

	if( GetDlgItemCheck( IDC_CHECK_ENABLED ) )
		nScale = GetDlgItemInt( IDC_SLIDER_QUALITY );

	if( GetDlgItemCheck( IDC_CHECK_TMREGISTERED ) )
		bTmRC = TRUE;

	if( nScale == 5 )
		nScale = 16; // Insane crazy anti-aliasing...

	for( i = 0; i <= sCurrentCharacterMapMaxIndex; i++ )
	{
		m_Metrics.glyph[i].x1 = 0;
		m_Metrics.glyph[i].y1 = 0;
		m_Metrics.glyph[i].x2 = 0;
		m_Metrics.glyph[i].y2 = 0;
		m_Metrics.glyph[i].ch = 0;
		m_Metrics.glyph[i].cy = 0;
		m_Metrics.glyph[i].left = 0;
		m_Metrics.glyph[i].right = 0;
		m_Metrics.glyph[i].width = 0;
	}

	HFONT hFont = CreateFont(-m_nFontSize * nScale,
									  0,
									  0,
									  0,
									  m_nBold,
									  m_bItalic,
									  FALSE,
									  FALSE,
									  m_nCharSet,
									  OUT_DEFAULT_PRECIS,
									  CLIP_DEFAULT_PRECIS,
									  NONANTIALIASED_QUALITY,
									  DEFAULT_PITCH|FF_DONTCARE,
									  m_szFontName );
	HFONT hTMFont = 0;
	if ( bTmRC )
	{
		hTMFont = CreateFont(-m_nFontSize * nScale,
									  0,
									  0,
									  0,
									  m_nBold,
									  m_bItalic,
									  FALSE,
									  FALSE,
									  ANSI_CHARSET,
									  OUT_DEFAULT_PRECIS,
									  CLIP_DEFAULT_PRECIS,
									  NONANTIALIASED_QUALITY,
									  DEFAULT_PITCH|FF_DONTCARE,
									  "Arrial" );
	}

	HDC hdc = CreateCompatibleDC(NULL);
	hFont = (HFONT) SelectObject( hdc, hFont );

	int start = GetDlgItemInt( IDC_EDIT_FIRSTCHAR );
	int stop = GetDlgItemInt( IDC_EDIT_LASTCHAR );
	int nRowHeight = 0;
	int nBorderSize = 0;
	int nAdvance = 0;

	if( start < 0 ) start = 0;
	if( stop < 0 ) stop = 0;

	if( GetDlgItemCheck( IDC_CHECK_OUTLINE ) )
		nBorderSize = nScale * GetDlgItemInt( IDC_SLIDER_OUTLINE ) / 4;

	ABCFLOAT size;

	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	m_Metrics.height  = m_nFontSize << 16;
	m_Metrics.ascent  = (int) ((float) (tm.tmAscent          << 16) / (float) nScale);
	m_Metrics.descent = (int) ((float) (tm.tmDescent         << 16) / (float) nScale);
	m_Metrics.leadin  = (int) ((float) (tm.tmInternalLeading << 16) / (float) nScale);
	m_Metrics.leadout = (int) ((float) (tm.tmExternalLeading << 16) / (float) nScale);

	float nMaxCellSize = 0.0f;

	for( i = start; i <= stop; i++ )
	{
		GetCharABCWidthsFloat( hdc, i, i, &size );

		if( nMaxCellSize < size.abcfB )
			nMaxCellSize = size.abcfB;

		if( nAdvance < (int) size.abcfA )
			nAdvance = (int) size.abcfA;
	}

	int nMaxWidth = (int) (nMaxCellSize + 0.9999999f);// tm.tmMaxCharWidth + tm.tmOverhang + nAdvance;
	int nMaxHeight = tm.tmHeight;

	nMaxWidth  = nMaxWidth  + nBorderSize * 2;
	nMaxHeight = nMaxHeight + nBorderSize * 2;

	HBITMAP hbmTemp = CreateDIB( nMaxWidth, nMaxHeight, 32 );
	HBITMAP hbmOld;

	int Sizes[256];
	int Order[256];
	int Printed[256];
	int dx = 0;
	int dy = 0;
	int left;
	int right;
	int top;
	int bottom;
	int width;
	int height;

	SendDlgItemMessage( IDC_PROGRESS2, PBM_SETRANGE32, 0, 256 );
	SendDlgItemMessage( IDC_PROGRESS2, PBM_SETPOS, start, 0 );

	COLORREF color;

	SetTextColor( hdc, m_crFont );
	SetBkColor( hdc, 0x000000 );

	char szExclude[128];
	WORD nIndex;

	GetDlgItemText( IDC_EDIT_EXCLUDE, szExclude, sizeof(szExclude)-1 );

	COLORREF BkColor = 0x00000000;
	int Count = 0;
	int nCharWidth = 0;
	int nOffset = 0;
	int ch;
	wchar_t c;

	for( ch = 0; ch <= sCurrentCharacterMapMaxIndex; ch++ )
	{
		c = sCurrentCharacterMap[ ch ];

		GetGlyphIndicesW( hdc, &c, 1, &nIndex, GGI_MARK_NONEXISTING_GLYPHS );

		top = -1;
		bottom = -1;

		if( nIndex != 0xffff )
		{
			Count++;
			
			ClearDIB( hbmTemp, BkColor );

			hbmOld = (HBITMAP) SelectObject( hdc, hbmTemp );
			TextOutW( hdc, 0, 0, &c, 1 );
			SelectObject( hdc, hbmOld );

			BeginDIB( 0, hbmTemp );

			for( y = 0; y < nMaxHeight; y++ )
			{
				for( x = 0; x < nMaxWidth; x++ )
				{
					color = GetPixelU( 0, x, y );

					if( color != BkColor )
					{
						top = y;
						break;
					}
				}

				if( top == y )
					break;
			}

			for( y = nMaxHeight-1; y > top; y-- )
			{
				for( x = 0; x < nMaxWidth; x++ )
				{
					color = GetPixelU( 0, x, y );

					if( color != BkColor )
					{
						bottom = y;
						break;
					}
				}

				if( bottom == y )
					break;
			}

			FinishBitmap(0);
		}

		Order[ch] = ch;
		Sizes[ch] = bottom - top;
		Printed[ch] = 0;

		SendDlgItemMessage( IDC_PROGRESS2, PBM_SETPOS, ch, 0 );
	}

	BOOL ReduceWaste = TRUE;
	BOOL DrawBoxes = FALSE;

	int Size1;
	int Size2;
	int ch1, ch2;

	if( ReduceWaste )
	{
		for( ch = 0; ch < sCurrentCharacterMapMaxIndex; )
		{
			Size1 = Sizes[ch+0];
			Size2 = Sizes[ch+1];

			if( Size2 <= Size1 )
				ch++;
			else
			{
				Sizes[ch+0] = Size2;
				Sizes[ch+1] = Size1;

				ch1 = Order[ch+0];
				ch2 = Order[ch+1];

				Order[ch+0] = ch2;
				Order[ch+1] = ch1;

				if( ch > 0 )
					ch--;
			}
		}
	}

	SendDlgItemMessage( IDC_PROGRESS, PBM_SETRANGE32, 0, Count );
	SendDlgItemMessage( IDC_PROGRESS, PBM_SETPOS, 0, 0 );

	BOOL UpperToLower = FALSE;
	BOOL LowerToUpper = FALSE;
	BOOL AlphaOnly = FALSE;

	if( GetDlgItemCheck( IDC_CHECK_UPPERTOLOWER ) )
		UpperToLower = TRUE;
	
	if( GetDlgItemCheck( IDC_CHECK_LOWERTOUPPER ) )
		LowerToUpper = TRUE;

	if( GetDlgItemCheck( IDC_CHECK_ALPHA_ONLY ) )
		AlphaOnly = TRUE;

	Count = 0;

	for( ch1 = 0; ch1 <= sCurrentCharacterMapMaxIndex; ch1++ )
	{
		ch = Order[ch1];

		if ( ch == c )	/* SCL: only attempt to change the case if the index matches the value (index mapped to ASCII value). */
		{
			if( UpperToLower )
				ch = tolower(ch);
			else if( LowerToUpper )
				ch = toupper(ch);
		}

		c = sCurrentCharacterMap[ ch ];
		BOOL bTMFont = (bTmRC && (c == 0x99 || c == 0xa9 || c == 0xae) );
		HFONT hOrigFont = 0;

		if( (ch >= start && ch <= stop) || bTMFont )
		{
			if ( ( bTMFont ) && ( hTMFont != 0 ) )
			{	// switch to ANSI font for the special characters
				hOrigFont = (HFONT) SelectObject( hdc, hTMFont );
			}

			GetGlyphIndicesW( hdc, &c, 1, &nIndex, GGI_MARK_NONEXISTING_GLYPHS );

			if( nIndex != 0xffff && Printed[ch] == 0 )
			{
				Count++;

				GetCharABCWidthsFloatW( hdc, c, c, &size );

				nOffset = (int) -ceil( size.abcfA );

				if( !strchr( szExclude, ch ) )
				{
					ClearDIB( hbmTemp, 0x000000 );

					hbmOld = (HBITMAP) SelectObject( hdc, hbmTemp );
					TextOutW( hdc, nOffset + nBorderSize, nBorderSize, &c, 1 );
					SelectObject( hdc, hbmOld );
				}

				BeginDIB( 0, hbmTemp );

				// Paint the alpha channel because TextOut doesn't do it for us

				if( ReduceWaste )
				{
					left = nMaxWidth;
					top = nMaxHeight;
					right = -1;
					bottom = -1;
				}
				else
				{
					left = 0;
					top  = 0;
					right = nMaxWidth;
					bottom = nMaxHeight;
				}

				for( y = 0; y < nMaxHeight; y++ )
				{
					for( x = 0; x < nMaxWidth; x++ )
					{
						color = GetPixelU( 0, x, y );

						if( color )
						{
							SetPixelU( 0, x, y, color | 0xff000000 );

							if( left   > x ) left   = x;
							if( top    > y ) top    = y;
							if( right  < x ) right  = x;
							if( bottom < y ) bottom = y;
						}
					}
				}

				left   -= nBorderSize;
				top    -= nBorderSize;
				right  += nBorderSize;
				bottom += nBorderSize;

				for( i = 0; i < nBorderSize; i++ )
				{
					HBITMAP hbmClone = CloneDIB( hbmTemp, FALSE );

					BeginDIB( 1, hbmClone );

					for( y = 0; y < nMaxHeight; y++ )
					{
						for( x = 0; x < nMaxWidth; x++ )
						{
							color = GetPixelU( 1, x, y );

							if( color )
							{
								SetPixelF( 0, x - 1, y, 0xff000000 | GetPixelF( 1, x - 1, y ) );
								SetPixelF( 0, x + 1, y, 0xff000000 | GetPixelF( 1, x + 1, y ) );
								SetPixelF( 0, x, y - 1, 0xff000000 | GetPixelF( 1, x, y - 1 ) );
								SetPixelF( 0, x, y + 1, 0xff000000 | GetPixelF( 1, x, y + 1 ) );
							}
						}
					}

					FinishBitmap( 1 );
					ReleaseDIB( hbmClone );
				}

				if( left <= right && top <= bottom )
				{
					width = right - left + 1;
					height = bottom - top + 1;

					int nCellWidth = (width + nScale - 1) / nScale;
					int nCellHeight = (height + nScale -1) / nScale;

					width = nCellWidth * nScale;
					height = nCellHeight * nScale;

					if( dx + nCellWidth > bmWidth )
					{
						dx = 0;
						dy += nRowHeight + 1;
						nRowHeight = 0;
					}

					HighQualityCopyBitmap( m_hbmOutput, dx, dy, nCellWidth, nCellHeight, hbmTemp, left, top, width, height );

					if( DrawBoxes )
					{
						BeginDIB( 2, m_hbmOutput );
						FastRect( 2, dx, dy, dx + nCellWidth, dy + nCellHeight, 0x0000ff00 );
						FinishBitmap( 2 );
					}

					m_Metrics.glyph[ch].x1 = dx;
					m_Metrics.glyph[ch].y1 = dy;
					m_Metrics.glyph[ch].x2 = dx + nCellWidth;
					m_Metrics.glyph[ch].y2 = dy + nCellHeight;

					dx += nCellWidth + 1;
			
					if( nRowHeight < nCellHeight )
						nRowHeight = nCellHeight;

					Printed[ch]++;
				}

				m_Metrics.glyph[ch].cy = (int) ((float) (top << 16) / (float) nScale );
				m_Metrics.glyph[ch].ch = m_nFontSize * 65536;

				m_Metrics.glyph[ch].left = (int) (size.abcfA * 65536.0f / (float) nScale);
				m_Metrics.glyph[ch].width = (int) (((float) size.abcfB) * 65356.0f / (float) nScale);
				m_Metrics.glyph[ch].right = (int) (size.abcfC * 65536.0f / (float) nScale);

				ClearDIB( hbmTemp, 0x000000 );
			}

			SendDlgItemMessage( IDC_PROGRESS, PBM_SETPOS, Count, 0 );

			if ( ( bTMFont ) && ( hOrigFont ) )
			{	// replace the original font
				SelectObject( hdc, hOrigFont );
			}
		}
	}

	if( AlphaOnly )
	{
		BeginDIB( 0, m_hbmOutput   );

		for( y = 0; y < bmHeight; y++ )
		{
			for( x = 0; x < bmWidth; x++ )
			{
				color = GetPixelU( 0, x, y ) & 0xff000000;
				SetPixelU( 0, x, y, m_crFont|color );
			}
		}

		FinishBitmap(0);
	}

	if ( c == ch )	/* SCL: only attempt to change the case if the index matches the value (index mapped to ASCII value). */
	{
		if( UpperToLower )
		{
			for( ch = 0; ch < sCurrentCharacterMapMaxIndex; ch++ )
			{
				ch2 = tolower(ch);

				if( ch != ch2 )
					m_Metrics.glyph[ch] = m_Metrics.glyph[ch2];
			}
		}
		else if( LowerToUpper )
		{
			for( ch = 0; ch < sCurrentCharacterMapMaxIndex; ch++ )
			{
				ch2 = toupper(ch);

				if( ch != ch2 )
					m_Metrics.glyph[ch] = m_Metrics.glyph[ch2];
			}
		}
	}

	SelectObject( hdc, hFont );

	DeleteDC( hdc );

	ReleaseDIB( hbmTemp );
	
	HBITMAP hbmComposite = 0;

	if( m_wndComposite.m_hbmBitmap )
		hbmComposite = ResampleDIB( m_wndComposite.m_hbmBitmap, 0, 0, 0, 0, GetBitmapWidth( m_hbmOutput), GetBitmapHeight( m_hbmOutput ) );

	if( m_wndComposite.m_hbmBitmap )
	{
		BeginDIB( 0, m_hbmOutput );
		BeginDIB( 1, hbmComposite );

		for( int y = 0; y < bmHeight; y++ )
			for( int x = 0; x < bmWidth; x++ )
				SetPixelU( 0, x, y, ColorMultiply( GetPixelU( 0, x, y), GetPixelF( 1, x, y) | 0xff000000 ) );

		FinishBitmap( 0 );
		FinishBitmap( 1 );
	}

	if( hbmComposite )
		ReleaseDIB( hbmComposite );

	RECT rcParent;
	GetWindowRect( &rcParent );

	m_wndPreview.m_pMetrics = &m_Metrics;

	m_wndPreview.CreateWindowEx( 0,
										  NULL,
										  "Preview",
										  WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX,
										  rcParent.right,
										  rcParent.top,
										  CW_USEDEFAULT,
										  CW_USEDEFAULT,
										  WindowHandle(),
										  NULL,
										  InstanceHandle(),
										  NULL );

	m_wndPreview.m_bAlpha = AlphaOnly;

	if( DrawBoxes )
		m_wndPreview.m_bAlpha = FALSE;

	RECT rcClient;
	RECT rcWindow;
	m_wndPreview.GetClientRect( &rcClient );
	m_wndPreview.GetWindowRect( &rcWindow );
	
	rcWindow.right -= rcClient.right - bmWidth;
	rcWindow.bottom -= rcClient.bottom - bmHeight;

	m_wndPreview.MoveWindow( rcWindow.left, rcWindow.top, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, TRUE );
	m_wndPreview.m_hbmPreview = m_hbmOutput;
	m_wndPreview.ShowWindow( SW_SHOW );
	m_wndPreview.UpdateWindow();
}

//=============================================================================
// FontWindow::WhenCommand
//=============================================================================

BOOL FontWindow::WhenCommand( HWND hWindow, int nCommand, int nNotifyCode )
{
	switch( nCommand )
	{
	case IDC_BUTTON_OPTIMIZE:
		if( nNotifyCode == BN_CLICKED )
			WhenOptimize();
		break;
	case IDC_BUTTON_EXPORT:
		SendMessage( WM_COMMAND, MAKEWPARAM( IDC_BUTTON_OPTIMIZE, 0 ), (LPARAM) GetDlgItem( IDC_BUTTON_OPTIMIZE ) );
		m_wndPreview.SendMessage( WM_COMMAND, MAKEWPARAM( ID_FILE_SAVE_AS, 0 ), 0 );
		break;
	case IDC_LIST_SIZE:
		if( nNotifyCode == LBN_SELCHANGE )
		{
			TCHAR szText[256];

			SendDlgItemMessage( IDC_LIST_SIZE, LB_GETTEXT, SendDlgItemMessage( IDC_LIST_SIZE, LB_GETCURSEL, 0, 0 ), (LPARAM) szText );

			SetDlgItemText( IDC_EDIT_SIZE, szText );
		}
	case IDC_EDIT_SIZE:
	case IDC_LIST_TYPEFACES:
	case IDC_LIST_STYLE:
		if( nNotifyCode == LBN_SELCHANGE || nNotifyCode == EN_UPDATE )
			UpdateFont();
		break;
	case IDC_BUTTON_CHOOSECHARACTERMAP:
		{
			LPCSTR pFileName = AskFileOpen( m_hWindow, "Choose Character Map File", "Text File (*.txt)\0*.txt\0\0" );
			if ( pFileName != NULL )
			{
				CheckDlgItem( IDC_CHECK_USECHARACTERMAP );
				SetDlgItemText( IDC_EDIT_CHARACTERMAPFILENAME, pFileName );
			}
			else
			{
				UncheckDlgItem( IDC_CHECK_USECHARACTERMAP );
				SetDlgItemText( IDC_EDIT_CHARACTERMAPFILENAME, "" );
			}
		}
		break;
	case IDC_CHECK_USECHARACTERMAP:
		{
			int iChecked = GetDlgItemCheck( IDC_CHECK_USECHARACTERMAP );
			if ( iChecked == BST_CHECKED )
			{
				EnableDlgItem( IDC_EDIT_CHARACTERMAPFILENAME );
				EnableDlgItem( IDC_BUTTON_CHOOSECHARACTERMAP );
				SetDlgItemText( IDC_EDIT_CHARACTERMAPFILENAME, "" );
			}
			else
			{
				DisableDlgItem( IDC_EDIT_CHARACTERMAPFILENAME );
				DisableDlgItem( IDC_BUTTON_CHOOSECHARACTERMAP );
				SetDlgItemText( IDC_EDIT_CHARACTERMAPFILENAME, "" );
			}
		}
		break;
	}

	return CDialog::WhenCommand( hWindow, nCommand, nNotifyCode );
}

//=============================================================================
// FontWindow::WhenClose
//=============================================================================

void FontWindow::WhenClose()
{
	DestroyWindow();
}

//=============================================================================
// FontWindow::WhenDestroy
//=============================================================================

void FontWindow::WhenDestroy()
{
	if( m_hbmOutput )
		DeleteObject( m_hbmOutput );

	PostQuitMessage(0);
}

//=============================================================================
// FontWindow::WhenInitDialog
//=============================================================================

BOOL FontWindow::WhenInitDialog( void )
{
	m_wndRedWindow.AttachDialogItem( WindowHandle(), IDC_STATIC_RED );
	m_wndGreenWindow.AttachDialogItem( WindowHandle(), IDC_STATIC_GREEN );
	m_wndBlueWindow.AttachDialogItem( WindowHandle(), IDC_STATIC_BLUE );
	m_wndColorWindow.AttachDialogItem( WindowHandle(), IDC_STATIC_COLOR );

	m_wndRedWindow.m_bkColor = 0xff0000;
	m_wndGreenWindow.m_bkColor = 0x00ff00;
	m_wndBlueWindow.m_bkColor = 0x0000ff;
	m_wndColorWindow.m_bkColor = m_crFont;

	SendDlgItemMessage( IDC_SLIDER_RED, TBM_SETRANGE, TRUE, MAKELPARAM( 0, 255 ));
	SendDlgItemMessage( IDC_SLIDER_GREEN, TBM_SETRANGE, TRUE, MAKELPARAM( 0, 255 ));
	SendDlgItemMessage( IDC_SLIDER_BLUE, TBM_SETRANGE, TRUE, MAKELPARAM( 0, 255 ));

	SendDlgItemMessage( IDC_SLIDER_RED, TBM_SETPOS, TRUE, m_crFont >> 16 & 0xff );
	SendDlgItemMessage( IDC_SLIDER_GREEN, TBM_SETPOS, TRUE, m_crFont >> 8 & 0xff );
	SendDlgItemMessage( IDC_SLIDER_BLUE, TBM_SETPOS, TRUE, m_crFont & 0xff );

	SendDlgItemMessage( IDC_PROGRESS, PBM_SETRANGE32, 32, 128 );
	SendDlgItemMessage( IDC_PROGRESS, PBM_SETPOS, 32, 0 );

	SendDlgItemMessage( IDC_SLIDER_QUALITY, TBM_SETRANGE, TRUE, MAKELPARAM( 2, 5 ) );
	SendDlgItemMessage( IDC_SLIDER_QUALITY, TBM_SETPOS, TRUE, 4 );

	SetDlgItemText( IDC_STATIC_QUALITY, szQuality[GetDlgItemInt( IDC_SLIDER_QUALITY )-1] );

	SetDlgItemInt( IDC_EDIT_IMAGEWIDTH, 256 );
	SetDlgItemInt( IDC_EDIT_IMAGEHEIGHT, 128 );

	SetDlgItemInt( IDC_EDIT_FIRSTCHAR, 32 );
	SetDlgItemInt( IDC_EDIT_LASTCHAR, 255 );

	SetDlgItemText( IDC_EDIT_EXCLUDE, "" );

	SendDlgItemMessage( IDC_SLIDER_OUTLINE, TBM_SETRANGE, TRUE, MAKELPARAM( 1, 16 ) );
	SendDlgItemMessage( IDC_SLIDER_OUTLINE, TBM_SETPOS, TRUE, 1 );
	
	SetDlgItemText( IDC_STATIC_OUTLINE, Printf( "%1.2f", (float) GetDlgItemInt( IDC_SLIDER_OUTLINE ) / 4.0f ) );

	CheckDlgItem( IDC_CHECK_TMREGISTERED );

	UncheckDlgItem( IDC_CHECK_USECHARACTERMAP );
	SetDlgItemText( IDC_EDIT_CHARACTERMAPFILENAME, "" );
	DisableDlgItem( IDC_EDIT_CHARACTERMAPFILENAME );
	DisableDlgItem( IDC_BUTTON_CHOOSECHARACTERMAP );

	EnumFonts();

	m_wndComposite.AttachDialogItem( WindowHandle(), IDC_BUTTON_COMPOSITE );

	return CDialog::WhenInitDialog();
}

//=============================================================================
// FontWindow::WhenHScroll
//=============================================================================

void FontWindow::WhenHScroll( int nCode, int nPos, HWND hWindow )
{
	int nRed = SendDlgItemMessage( IDC_SLIDER_RED, TBM_GETPOS, 0, 0 );
	int nGreen = SendDlgItemMessage( IDC_SLIDER_GREEN, TBM_GETPOS, 0, 0 );
	int nBlue = SendDlgItemMessage( IDC_SLIDER_BLUE, TBM_GETPOS, 0, 0 );

	m_crFont = nRed << 16 | nGreen << 8 | nBlue;

	m_wndColorWindow.m_bkColor = m_crFont;
	m_wndColorWindow.Invalidate();

	SetDlgItemText( IDC_STATIC_QUALITY, szQuality[GetDlgItemInt( IDC_SLIDER_QUALITY )-1] );
	SetDlgItemText( IDC_STATIC_OUTLINE, Printf( "%1.2f", (float) GetDlgItemInt( IDC_SLIDER_OUTLINE ) / 4.0f ) );

	CDialog::WhenHScroll( nCode, nPos, hWindow );
}