#include "pch.h"
#include "toolbar.h"

CToolBar::CToolBar( void )
{
}

CToolBar::~CToolBar()
{
}

BOOL CToolBar::CreateToolBar( HWND hParent, UINT Resource, UINT ToolBarID )
{
	BOOL Result = CreateWindowEx( 0, TOOLBARCLASSNAME,
											0,
											WS_CHILD|WS_VISIBLE,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											CW_USEDEFAULT,
											hParent,
											NULL,
											NULL,
											NULL );

	if( !Result )
		return FALSE;

	if( !ToolBarLoadResource( WindowHandle(), RES(Resource) ) )
		return FALSE;

	SendMessage( TB_AUTOSIZE, 0, 0 );

	return TRUE;
}

//=============================================================================
// CToolBar::WhenCommand()
//=============================================================================

BOOL CToolBar::WhenCommand( HWND hWindow, int nCommand, int nNotifyCode )
{
	return CWindow::WhenCommand( hWindow, nCommand, nNotifyCode );
}

//=============================================================================
// ToolBarLoadResource
//=============================================================================

BOOL ToolBarLoadResource( HWND hToolBar, LPCTSTR lpResourceName )
{
	HINSTANCE hInstance = (HINSTANCE) GetWindowLong( hToolBar, GWL_HINSTANCE );

	HRESOURCE hResource = FindResource( hInstance, lpResourceName, RC_TOOLBAR );

	HGLOBAL hGlobal = LoadResource( hInstance, hResource );

	ToolBarResource *pResource = (ToolBarResource *) LockResource( hGlobal );

	if( pResource )
	{
		SendMessage( hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0 );

		ToolBarSetBitmapSize( hToolBar, pResource->Width, pResource->Height );
		ToolBarSetButtonSize( hToolBar, pResource->Width+7, pResource->Height+7 );
		
		ToolBarAddBitmapResource( hToolBar, pResource->ItemCount, lpResourceName );

		int Image = 0;

		for( int i = 0; i < pResource->ItemCount; i++ )
		{
			if( pResource->Items[i] == ID_SEPARATOR )
				ToolBarAddSeparator( hToolBar );
			else
			{
				ToolBarAddButton( hToolBar, TBSTYLE_TOOLTIPS|TBSTYLE_BUTTON, Image++, pResource->Items[i], 0 );
			}
		}
	}

	FreeResource( hGlobal );

	return TRUE;
}

//=============================================================================
// ToolBarSetButtonSize
//=============================================================================

void ToolBarSetButtonSize( HWND hToolBar, int Width, int Height )
{
	SendMessage( hToolBar, TB_SETBUTTONSIZE, 0, MAKELPARAM( Width, Height ) );
}

//=============================================================================
// ToolBarSetBitmapSize
//=============================================================================

void ToolBarSetBitmapSize( HWND hToolBar, int Width, int Height )
{
	SendMessage( hToolBar, TB_SETBITMAPSIZE, 0, MAKELPARAM( Width, Height ) );
}

//=============================================================================
// ToolBarAddBitmapResource
//=============================================================================

int ToolBarAddBitmapResource( HWND hToolBar, int ItemCount, LPCTSTR lpResourceName )
{
	TBADDBITMAP tba;

	tba.hInst = (HINSTANCE) GetWindowLong( hToolBar, GWL_HINSTANCE );
	tba.nID = (UINT) lpResourceName;

	return SendMessage( hToolBar, TB_ADDBITMAP, ItemCount, (LPARAM) &tba );
}

//=============================================================================
// ToolBarAddBitmapHandle
//=============================================================================

int ToolBarAddBitmapHandle( HWND hToolBar, int ItemCount, HANDLE hBitmap )
{
	TBADDBITMAP tba;
	
	tba.hInst = HINST_COMMCTRL;
	tba.nID = (UINT) hBitmap;

	return SendMessage( hToolBar, TB_ADDBITMAP, ItemCount, (LPARAM) &tba );
}

//=============================================================================
// ToolBarAddSeparator
//=============================================================================

int ToolBarAddSeparator( HWND hToolBar )
{
	return ToolBarAddButton( hToolBar, TBSTYLE_SEP, -1, ID_SEPARATOR, 0 );
}

//=============================================================================
// ToolBarAddButton
//=============================================================================

int ToolBarAddButton( HWND hToolBar, int Style, int Bitmap, int Command, int String )
{
	TBBUTTON tbb;

	tbb.iBitmap = Bitmap;
	tbb.idCommand = Command;
	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = (BYTE) Style;
	tbb.dwData = 0;
	tbb.iString = 0;

	return SendMessage( hToolBar, TB_ADDBUTTONS, 1, (LPARAM) &tbb );
}
