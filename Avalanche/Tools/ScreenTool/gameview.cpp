#include "pch.h"
#include "application.h"
#include "document.h"
#include "gameview.h"
#include "helpers.h"
#include "mainframe.h"
#include "sprite.h"
#include "texture.h"
#include "resource.h"
#include "newscreendlg.h"

IMPLEMENT_DYNCREATE( GameView, CView );

BEGIN_MESSAGE_MAP( GameView, CView )
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_COMMAND( ID_EDIT_UNDO, OnEditUndo )
	ON_COMMAND( ID_EDIT_REDO, OnEditRedo )
	ON_COMMAND( ID_EDIT_CUT, OnEditCut )
	ON_COMMAND( ID_EDIT_COPY, OnEditCopy )
	ON_COMMAND( ID_EDIT_PASTE, OnEditPaste )
	ON_COMMAND( ID_EDIT_DELETE, OnEditDelete )
	ON_COMMAND( ID_EDIT_SELECTALL, OnEditSelectAll )
	ON_COMMAND( ID_EDIT_FIND, OnEditFind )
	ON_COMMAND( ID_EDIT_FINDREPLACE, OnEditFindReplace )
	ON_COMMAND( ID_NEW_SCREEN, OnNewScreen )
	ON_COMMAND( ID_NEW_SPRITE, OnNewSprite )
	ON_COMMAND( ID_SPRITE_LOCK, OnSpriteLock )
	ON_COMMAND( ID_SPRITE_DELETE, OnSpriteDelete )
	ON_COMMAND( ID_SPRITE_PROPERTIES, OnSpriteProperties )
	ON_COMMAND( ID_SPRITE_SAVEAS, OnSpriteSaveAs )
	ON_COMMAND( ID_SPRITE_ADDFILES, OnSpriteAddFiles )
	ON_COMMAND( ID_TRANSFORM_POSITION, OnTransformPosition )
	ON_COMMAND( ID_TRANSFORM_SIZE, OnTransformSize )
	ON_COMMAND( ID_TRANSFORM_SCALE, OnTransformScale )
	ON_COMMAND( ID_TRANSFORM_FLIPHORIZONTAL, OnTransformFlipHorizontal )
	ON_COMMAND( ID_TRANSFORM_FLIPVERTICAL, OnTransformFlipVertical )
	ON_COMMAND( ID_TRANSFORM_ROTATECW, OnTransformRotateCW )
	ON_COMMAND( ID_TRANSFORM_ROTATECCW, OnTransformRotateCCW )
	ON_COMMAND( ID_TRANSFORM_ROTATE, OnTransformRotate )
	ON_COMMAND( ID_TEXTURE_APPLY, OnTextureApply )
	ON_COMMAND( ID_TEXTURE_REMOVE, OnTextureRemove )
	ON_COMMAND( ID_TEXTURE_SELECT, OnTextureSelect )
	ON_COMMAND( ID_TEXTURE_CUSTOMIZE, OnTextureCustomize )
	ON_COMMAND( ID_TEXTURE_FLIPHORIZONTAL, OnTextureFlipHorizontal )
	ON_COMMAND( ID_TEXTURE_FLIPVERTICAL, OnTextureFlipVertical )
	ON_COMMAND( ID_TEXTURE_ROTATECW, OnTextureRotateCW )
	ON_COMMAND( ID_TEXTURE_ROTATECCW, OnTextureRotateCCW )
	ON_COMMAND( ID_TEXTURE_ROTATE, OnTextureRotate )
	ON_COMMAND( ID_TEXTURE_AUTOFIT, OnTextureAutoFit )
	ON_COMMAND( ID_TEXTURE_DEFAULT, OnTextureDefault )
	ON_COMMAND( ID_GROUP_REMOVE, OnGroupRemove )
	ON_COMMAND( ID_GROUP_SELECTPARENT, OnGroupSelectParent )
	ON_COMMAND( ID_GROUP_SELECTFAMILY, OnGroupSelectFamily )
	ON_COMMAND( ID_INTERFACES_SCREEN, OnInterfaceScreen )
	ON_COMMAND( ID_INTERFACES_BUTTON, OnInterfaceButton )
	ON_COMMAND( ID_TEXT_APPLYFONT, OnTextApplyFont )
	ON_COMMAND( ID_BUTTON_ZOOM, OnButtonZoom )
	ON_COMMAND( ID_BUTTON_CENTERVIEW, OnButtonCenterView )
	ON_COMMAND( ID_BUTTON_ARROW, OnButtonArrow )
	ON_COMMAND( ID_BUTTON_ARROWSELECT, OnButtonArrowSelect )
	ON_COMMAND( ID_BUTTON_NEWSPRITE, OnButtonNewSprite )
	ON_COMMAND( ID_BUTTON_NEWTEXT, OnButtonNewText )
	ON_COMMAND( ID_BUTTON_STAMPER, OnButtonStamper )
	ON_COMMAND( ID_BUTTON_NEWFAMILY, OnButtonNewFamily )
	ON_COMMAND( ID_BUTTON_BUTTONORDER, OnButtonButtonOrder )
	ON_COMMAND( ID_BUTTON_GRIDLOCK, OnButtonGridLock )
	ON_COMMAND( ID_BUTTON_SELECTFAMILY, OnButtonSelectFamily )
	ON_COMMAND( ID_BUTTON_SELECTPARENT, OnButtonSelectParent )
	ON_COMMAND( ID_BUTTON_SELECTCHILDREN, OnButtonSelectChildren )
	ON_COMMAND( ID_BUTTON_SELECTNEXTCHILD, OnButtonSelectNextChild )
	ON_COMMAND( ID_BUTTON_CHILDORDER, OnButtonChildOrder )
	ON_COMMAND( ID_BUTTON_GROUP, OnButtonGroup )
	ON_COMMAND( ID_BUTTON_CREATEFAMILY, OnButtonCreateFamily )
	ON_COMMAND( ID_BUTTON_SEPARATEFAMILY, OnButtonSeparateFamily )
	ON_COMMAND( ID_BUTTON_SAMESIZE, OnButtonSameSize )
	ON_COMMAND( ID_BUTTON_SAMEWIDTH, OnButtonSameWidth )
	ON_COMMAND( ID_BUTTON_SAMEHEIGHT, OnButtonSameHeight )
	ON_COMMAND( ID_BUTTON_SPACEVERTICAL, OnButtonSpaceVertical )
	ON_COMMAND( ID_BUTTON_SPACEHORIZONTAL, OnButtonSpaceHorizontal )
	ON_COMMAND( ID_BUTTON_ALIGNLEFT, OnButtonAlignLeft )
	ON_COMMAND( ID_BUTTON_ALIGNRIGHT, OnButtonAlignRight )
	ON_COMMAND( ID_BUTTON_ALIGNTOP, OnButtonAlignTop )
	ON_COMMAND( ID_BUTTON_ALIGNBOTTOM, OnButtonAlignBottom )
	ON_COMMAND( ID_KEY_ESCAPE, OnKeyEscape )
	ON_NOTIFY_RANGE( PROPNOTIFY_ITEMCHANGED, 0, 0x7fffffff, OnPropertyChange )
	ON_NOTIFY_RANGE( TEXNOTIFY_UPDATE, 0, 0x7fffffff, OnCustomTextureUpdate )
	ON_UPDATE_COMMAND_UI( ID_SPRITE_LOCK, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_TEXTURE_APPLY, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_TEXTURE_REMOVE, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_INTERFACES_SCREEN, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_INTERFACES_BUTTON, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_TEXT_APPLYFONT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ARROW, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_EDIT_CUT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_EDIT_COPY, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_EDIT_DELETE, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_EDIT_PASTE, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ARROWSELECT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_NEWSPRITE, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_NEWTEXT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_STAMPER, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_GRIDLOCK, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_NEWFAMILY, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_PROPERTIES, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ZOOM, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SELECTFAMILY, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SELECTCHILDREN, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SELECTPARENT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SELECTNEXTCHILD, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_GROUP, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_CREATEFAMILY, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SEPARATEFAMILY, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SAMESIZE, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SAMEWIDTH, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SAMEHEIGHT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ALIGNLEFT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ALIGNRIGHT, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ALIGNTOP, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_ALIGNBOTTOM, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SPACEVERTICAL, OnUpdateCommandUI )
	ON_UPDATE_COMMAND_UI( ID_BUTTON_SPACEHORIZONTAL, OnUpdateCommandUI )
	ON_COMMAND( ID_BUTTON_FIND, OnFindDialogFind )
	ON_COMMAND( ID_BUTTON_REPLACE, OnFindDialogReplace )
	ON_COMMAND( ID_BUTTON_SELECT, OnFindDialogSelect )

END_MESSAGE_MAP()

#define TIMEREVENT_SPRITECHANGES		(0x100)

//=============================================================================
// GameView::GameView
//=============================================================================

GameView::GameView( void )
{
	m_pSwapChain = 0;
	m_pNewSprite = 0;
	m_pTopSprite = 0;
	m_pSceneList = 0;

	m_MouseMode = MOUSEMODE_HOVER;
	m_MouseTool = MOUSETOOL_DRAG;
	m_ObjectType = HUDOBJECT_SPRITE;

	m_SizeHandles = 0;
	m_SizeX1 = 0.0f;
	m_SizeY1 = 0.0f;
	m_SizeX2 = 0.0f;
	m_SizeY2 = 0.0f;

	m_ViewPortX = 0.0f;
	m_ViewPortY = 0.0f;
	m_Zoom = 1.0f;

	m_ViewMode = VIEWMODE_SHOWNAMES|VIEWMODE_SHOWBOXES;

	m_AllowSizing = TRUE;
	m_AltKey = FALSE;
	m_ControlKey = FALSE;
	m_ShiftKey = FALSE;
	m_EscapeKeyHandled = FALSE;
	m_GridLock = FALSE;

	m_PositionLock = 0;

	m_OemCursor = OCR_NORMAL;
	m_OemCursor2 = ~m_OemCursor;

	m_HoverColor = 0xcc88ff88;
	m_SelColor = 0xffff7777;
	m_MultiSelColor = 0xff7777ff;
	m_GroupSelColor = 0xffffff77;
	m_LockedColor = 0xffff0000;

	m_GameTime = Timer();
}

//=============================================================================
// GameView::~GameView
//=============================================================================

GameView::~GameView()
{
	Cleanup();
}

//=============================================================================
// GameView::Init
//
//		Called then the view is created, this function performs a one-time
//		initialization.
//=============================================================================

void GameView::Init( void )
{
	return;
}

//=============================================================================
// GameView::Shutdown
//
//		Called when the view is destroyed, this function cleans up memory and
//		frees any resources it allocated.
//
//=============================================================================

void GameView::Cleanup( void )
{
	DeselectAll();

	DeleteSprites( m_CopyList );

	ClearSpriteList( m_PickList );
	ClearSpriteList( m_HoverList );
	ClearSpriteList( m_CopyList );

	DestroySwapChain();

	if( m_pNewSprite )
		m_pNewSprite = Project.DeleteSprite( m_pNewSprite );

	return;
}

//=============================================================================
// GameView::CreateSwapChain
//=============================================================================

BOOL GameView::CreateSwapChain( void )
{
	D3DDISPLAYMODE DisplayMode;
	HRESULT hresult;
	RECT rc;
	GetClientRect( &rc );

	hresult = D3DRESULT( d3d.GetDisplayMode( 0, &DisplayMode ) );

	if( FAILED(hresult) )
		return FALSE;

	int Width = rc.right - rc.left;
	int Height = rc.bottom - rc.top;

	if( m_pSwapChain )
	{
		if( m_PresentParameters.BackBufferWidth != Width ||
			m_PresentParameters.BackBufferHeight != Height ||
			m_PresentParameters.BackBufferFormat != DisplayMode.Format )
		{
			DestroySwapChain();
		}
	}

	if( !m_pSwapChain )
	{
		m_PresentParameters.BackBufferWidth = Width;
		m_PresentParameters.BackBufferHeight = Height;
		m_PresentParameters.BackBufferFormat = DisplayMode.Format;
		m_PresentParameters.BackBufferCount = 1;
		m_PresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
		m_PresentParameters.MultiSampleQuality = 0;
		m_PresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_PresentParameters.hDeviceWindow = m_hWnd;
		m_PresentParameters.Windowed = TRUE;
		m_PresentParameters.EnableAutoDepthStencil = TRUE;
		m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_PresentParameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		m_PresentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		hresult = D3DRESULT( d3d.CreateAdditionalSwapChain( &m_PresentParameters, &m_pSwapChain ) );

		if( FAILED(hresult) )
			return FALSE;
	}

	return TRUE;
}

//=============================================================================
// GameView::DestroySwapChain
//=============================================================================

void GameView::DestroySwapChain( void )
{
	if( m_pSwapChain )
	{
		m_pSwapChain->Release();
		m_pSwapChain = 0;
	}

	return;
}

//=============================================================================
// GameView::SetupRenderTarget
//=============================================================================

BOOL GameView::SetupRenderTarget( void )
{
	if( !CreateSwapChain() )
		return FALSE;

	LPDIRECT3DSURFACE9 pBackBuffer;
	HRESULT hresult;

	hresult = D3DRESULT( m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) );

	if( FAILED(hresult) )
		return FALSE;

	pBackBuffer->Release();

	hresult = D3DRESULT( d3d.SetRenderTarget( 0, pBackBuffer ) );

	if( FAILED(hresult) )
		return FALSE;

	return TRUE;
}

//=============================================================================
// GameView::GetDocument
//=============================================================================

SevenupDoc * GameView::GetDocument( void )
{
	return (SevenupDoc *) CView::GetDocument();
}

//=============================================================================
// GameView::GetProjectParams
//=============================================================================

void GameView::GetProjectParams( PROJECTPARAMS& ProjectParams )
{
	memset( &ProjectParams, 0, sizeof(PROJECTPARAMS) );

	SevenupDoc* pDocument = GetDocument();

	if( pDocument )
		ProjectParams = pDocument->m_Params;

	return;
}


//=============================================================================
// GameView::Invalidate
//=============================================================================

void GameView::Invalidate( void )
{
	CView::Invalidate( FALSE );
}

//=============================================================================
// GameView::DocumentGetScreenSize
//=============================================================================

void GameView::DocumentGetScreenSize( int &Width, int &Height )
{
	Width = DocumentGetScreenWidth();
	Height = DocumentGetScreenHeight();
}

//=============================================================================
// GameView::DocumentGetScreenWidth
//=============================================================================

int GameView::DocumentGetScreenWidth( void )
{
	SevenupDoc* pDocument = GetDocument();

	if( !pDocument )
		return 0;

	return pDocument->m_Params.m_ScreenWidth;
}

//=============================================================================
// GameView::DocumentGetScreenHeight
//=============================================================================

int GameView::DocumentGetScreenHeight( void )
{
	SevenupDoc* pDocument = GetDocument();

	if( !pDocument )
		return 0;

	return pDocument->m_Params.m_ScreenHeight;
}

//=============================================================================
// GameView::DocumentGetTextureHandle
//=============================================================================

t_Handle GameView::DocumentGetTextureHandle( void )
{
	t_Handle Handle = INVALID_HANDLE;

	if( GetDocument() )
		Handle = GetDocument()->GetTextureHandle();

	return Handle;
}

//=============================================================================
// GameView::DocumentGetTextureIndex
//=============================================================================

int GameView::DocumentGetTextureIndex( void )
{
	int Texture = 0;

	if( GetDocument() )
		Texture = GetDocument()->GetTextureIndex();

	return Texture;
}

//=============================================================================
// GameView::DocumentGetSceneList
//=============================================================================

Sprite* GameView::DocumentGetSceneList( void )
{
	SevenupDoc* pDocument = GetDocument();

	if( !pDocument )
		return 0;

	return pDocument->GetSceneList();

}
//=============================================================================
// GameView::DocumentGetSpriteAtPosition
//=============================================================================

Sprite * GameView::DocumentGetSpriteAtPosition( float x, float y )
{
	SevenupDoc *pDocument = GetDocument();

	if( !pDocument )
		return 0;

	return pDocument->GetSpriteAtPosition( x, y );
}

//=============================================================================
// GameView::DocumentGetSpritesAtPosition
//=============================================================================

SpriteItem* GameView::DocumentGetSpritesAtPosition( float x, float y )
{
	SevenupDoc* pDocument = GetDocument();

	if( !pDocument )
		return 0;

	return pDocument->GetSpritesAtPosition( x, y, TopSprite() );
}

//=============================================================================
// GameView::DocumentSyncProjectViews
//=============================================================================

void GameView::DocumentSyncProjectViews( void )
{
	SevenupDoc *pDocument = GetDocument();

	if( pDocument )
		pDocument->SyncProjectViews();

	return;
}

//=============================================================================
// GameView::DocumentSelectSprite
//=============================================================================

void GameView::DocumentSelectSprite( Sprite* pSprite )
{
	SevenupDoc* pDocument = GetDocument();

	if( pDocument )
		pDocument->SelectSprite( pSprite );

	return;
}

//=============================================================================
// GameView::GetGameFont
//=============================================================================

Font* GameView::GetGameFont( void )
{
	MainFrame *pMainFrame = (MainFrame*) AfxGetMainWnd();
	Font *pGameFont;

	if( pMainFrame )
		pGameFont = Font::FindFont( pMainFrame->m_ToolBar.GetCurrentFont() );
	else
		pGameFont = Font::GetFirstFont();

	return pGameFont;
}

//=============================================================================
// GameView::CreateSprite
//=============================================================================

Sprite* GameView::CreateSprite( HUDOBJECTTYPE Type )
{
	Sprite* pSprite = 0;

	SevenupDoc* pDocument = GetDocument();

	if( pDocument )
		pSprite = pDocument->CreateObject( Type );

	return pSprite;
}

//=============================================================================
// GameView::DeleteSprite
//=============================================================================

Sprite* GameView::DeleteSprite( Sprite* pSprite )
{
	if( pSprite )
	{
		DeselectSprite( pSprite );
		RemoveFromPickList( pSprite );
		RemoveFromHoverList( pSprite );

		if( m_pTopSprite == pSprite )
			m_pTopSprite = 0;

		if( m_pSceneList == pSprite )
			m_pSceneList = 0;

		SevenupDoc* pDocument = GetDocument();

		if( pDocument )
			pSprite = pDocument->DeleteSprite( pSprite );
	}

	return pSprite;
}

//=============================================================================
// GameView::DeleteSpriteFromProject
//=============================================================================

void GameView::DeleteSpriteFromProject( Sprite* pSpriteToDelete )
{
	if( pSpriteToDelete )
	{
		Sprite* pParent = pSpriteToDelete->Parent();
		Sprite* pChild = pSpriteToDelete->Child();
		float x, y;

		if( !pParent )
			DeleteFamilyFromProject( pSpriteToDelete );
		else
		{
			while( pChild )
			{
				pChild->GetScreenPosition( x, y );
				pChild->SetParent( pParent );
				pChild->SetScreenPosition( x, y );

				pChild = pSpriteToDelete->Child();
			}

			DeleteSprite( pSpriteToDelete );
		}
	}

	return;
}

//=============================================================================
// GameView::DeleteSpriteFromProject
//=============================================================================

void GameView::DeleteFamilyFromProject( Sprite *pSpriteToDelete )
{
	if( pSpriteToDelete )
	{		
		Sprite *pChild = pSpriteToDelete->Child();
		Sprite *pNext;

		while( pChild )
		{
			pNext = pChild->Next();

			DeleteFamilyFromProject( pChild );

			pChild = pNext;
		}

		DeleteSprite( pSpriteToDelete );

		DocumentSyncProjectViews();
	}
}

//=============================================================================
// GameView::GuiGetContextMenu
//=============================================================================

HMENU GameView::GuiGetContextMenu( void )
{
	CWnd *pMainWnd = AfxGetMainWnd();

	if( !pMainWnd )
		return 0;

	HMENU hTopMenu = ::GetMenu( pMainWnd->m_hWnd );
	HMENU hSubMenu = ::GetSubMenu( hTopMenu, 3 );

	return hSubMenu;
}

//=============================================================================
// GameView::GuiShowProperties
//=============================================================================

BOOL GameView::GuiShowProperties( Sprite *pSprite )
{
	ProjectItem *pProjectItem = Project.GetProjectItem( pSprite );

	if( !pProjectItem )
		return FALSE;

	if( m_PropertyWnd.m_hWnd == 0 )
	{
		RECT rc = m_PropertyWnd.m_WindowRect;

		if( rc.right - rc.left == 0 && rc.bottom - rc.top == 0 )
		{
			CWnd *pFrameWnd = GetParentFrame();

			if( !pFrameWnd )
				pFrameWnd = this;

			RECT rcFrame;

			pFrameWnd->GetWindowRect( &rcFrame );

			int CxWindow = 256;
			int CyWindow = 385;

			int CxFrame = rcFrame.right - rcFrame.left;
			int CyFrame = rcFrame.bottom - rcFrame.top;

			int x = rcFrame.left + CxFrame/2 - CxWindow/2;
			int y = rcFrame.top + CyFrame/2 - CyWindow/2;

			rc = GdiMakeRect( x, y, CxWindow, CyWindow );
		}

		DWORD dwStyle =
			WS_CAPTION|WS_CHILD|WS_CLIPCHILDREN|WS_POPUP|WS_SYSMENU|WS_THICKFRAME;

		if( !m_PropertyWnd.CreateEx( 0, 0, "Properties", dwStyle, rc, this, 0 ) )
			return FALSE;
	}

//	Project.GetProperties( m_PropertyWnd, pProjectItem );

	pProjectItem->GetProperties( m_PropertyWnd );

	m_PropertyWnd.ShowWindow( SW_SHOW );
	m_PropertyWnd.UpdateWindow();
	m_PropertyWnd.SetActiveWindow();

	return TRUE;
}

//=============================================================================
// GameView::GuiHideProperties
//=============================================================================

void GameView::GuiHideProperties( void )
{
	m_PropertyWnd.DestroyWindow();
}

//=============================================================================
// GameView::SetOemCursor
//=============================================================================

void GameView::SetOemCursor( int CursorType )
{
	m_OemCursor = CursorType;
}

//=============================================================================
// GameView::UpdateOemCursor
//=============================================================================

void GameView::UpdateOemCursor( void )
{
	HCURSOR hCursor = LoadCursor( AfxGetInstanceHandle(), MAKEINTRESOURCE(m_OemCursor) );

	if( !hCursor )
		hCursor = (HCURSOR) LoadImage( 0, MAKEINTRESOURCE(m_OemCursor), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_SHARED );

	if( hCursor )
		SetCursor( hCursor );

	m_OemCursor2 = m_OemCursor;

	return;
}

//=============================================================================
// GameView::SetMouseTool
//=============================================================================

void GameView::SetMouseTool( MOUSETOOL MouseTool )
{
	if( m_MouseTool != MouseTool )
	{
		OnMouseToolEnd( m_MouseTool );
		OnMouseToolBegin( MouseTool );
		m_MouseTool = MouseTool;
	}

	return;
}

//=============================================================================
// GameView::SetMouseMode
//=============================================================================

void GameView::SetMouseMode( MOUSEMODE MouseMode )
{
	if( MouseMode == MOUSEMODE_SIZE )
	{
		Sprite *pSprite = SelectedSprite();

		if( pSprite )
			pSprite->GetSpriteRect( m_SizeX1, m_SizeY1, m_SizeX2, m_SizeY2 );
	}

	if( m_MouseMode != MouseMode )
	{
		OnMouseModeEnd( m_MouseMode );
		OnMouseModeBegin( MouseMode );
		m_MouseMode = MouseMode;
	}

	return;
}

//=============================================================================
// GameView::SetMouseHover - Check the position of the mouse and highlight
//									  whatever sprite is at that location.
//=============================================================================

void GameView::SetMouseHover( int x, int y )
{
	CPoint p;
	float fx, fy;

	ClearHoverList();
	ClearLockedList();

	MouseToWorld( x, y, fx, fy );

	SpriteItem* pSpriteList = DocumentGetSpritesAtPosition( fx, fy );
	Sprite* pPickSprite = 0;

	if( pSpriteList )
	{
		SpriteItem* pItem;
		Sprite* pSprite;
		float x1, y1;
		float x2, y2;
		float cx, cy;
		float rect = 1024.0f*1024.0f*1024.0f;
		int priority = 0;

		pItem = pSpriteList->Child();

		while( pItem )
		{
			pSprite = pItem->m_pSprite;

			if( pSprite && IsLocked( pSprite ) )
				AddToLockedList( pSprite );
			else
			if( pSprite && !IsDragged( pSprite ) )
			{
				AddToHoverList( pSprite );

				pSprite->GetSpriteRect( x1, y1, x2, y2 );

				cx = x2 - x1;
				cy = y2 - y1;

				if( rect == cx * cy )
				{
					int p1 = pSprite->GetSortedPriority();
					int p2 = 0;

					if( pPickSprite )
						p2 = pPickSprite->GetSortedPriority();

					if( p1 > p2 )
						pPickSprite = pSprite;
				}
				else
				if( rect > cx * cy )
				{
					rect = cx * cy;
					pPickSprite = pSprite;
				}
			}

			pItem = pItem->Next();
		}
	}

	if( pPickSprite )
	{
		RemoveFromHoverList( pPickSprite );
		AddToHoverList( pPickSprite );
	}

	return;
}

//=============================================================================
// GameView::GetSizeHandles
//=============================================================================

typedef struct
{
	int x;
	int y;
	int SizeHandles;
}
Zone;

int GameView::GetSizeHandles( int x, int y )
{
	int SizeHandles = 0;

	Sprite* pSelSprite = SelectedSprite();

	if( pSelSprite )
	{
		Zone Zones[8];

		float sprite_x1;
		float sprite_y1;
		float sprite_x2;
		float sprite_y2;

		pSelSprite->GetSpriteRect( sprite_x1, sprite_y1, sprite_x2, sprite_y2 );

		int x1, y1;
		int x2, y2;

		PointToScreen( sprite_x1, sprite_y1, x1, y1 );
		PointToScreen( sprite_x2, sprite_y2, x2, y2 );

		int cx = x1 + (x2 - x1) / 2;
		int cy = y1 + (y2 - y1) / 2;

		if( x >= x1 && x <= x2 && y >= y1 && y <= y2 )
		{
			//////////////////////////////////////////////////////////////////////////
					
			Zones[0].x = x1;
			Zones[0].y = y1;
			Zones[0].SizeHandles = SIZEHANDLE_LEFT|SIZEHANDLE_TOP;

			Zones[1].x = x2;
			Zones[1].y = y1;
			Zones[1].SizeHandles = SIZEHANDLE_RIGHT|SIZEHANDLE_TOP;
				
			Zones[2].x = x2;
			Zones[2].y = y2;
			Zones[2].SizeHandles = SIZEHANDLE_RIGHT|SIZEHANDLE_BOTTOM;

			Zones[3].x = x1;
			Zones[3].y = y2;
			Zones[3].SizeHandles = SIZEHANDLE_LEFT|SIZEHANDLE_BOTTOM;

			Zones[4].x = x;
			Zones[4].y = y1;
			Zones[4].SizeHandles = SIZEHANDLE_TOP;

			Zones[5].x = x2;
			Zones[5].y = y;
			Zones[5].SizeHandles = SIZEHANDLE_RIGHT;

			Zones[6].x = x;
			Zones[6].y = y2;
			Zones[6].SizeHandles = SIZEHANDLE_BOTTOM;

			Zones[7].x = x1;
			Zones[7].y = y;
			Zones[7].SizeHandles = SIZEHANDLE_LEFT;

			//////////////////////////////////////////////////////////////////////////

			int MinDistanceSquared = 0x7fffffff;
			int DistanceSquared;
			int Dx, Dy;
			int i;

			int Cx = x1 + (x2 - x1) / 2;
			int Cy = y1 + (y2 - y1) / 2;

			for( i = 0; i < 8; i++ )
			{
				Dx = Cx - Zones[i].x;
				Dy = Cy - Zones[i].y;

				DistanceSquared = Dx*Dx + Dy*Dy;

				if( MinDistanceSquared > DistanceSquared )
					MinDistanceSquared = DistanceSquared;
			}

			float DistanceOver2 = Math::Sqrt( (float) MinDistanceSquared ) * 0.5f;

			if( DistanceOver2 > 8.0f )
				DistanceOver2 = 8.0f;

			MinDistanceSquared = (int) (DistanceOver2 * DistanceOver2);

			for( i = 0; i < 8; i++ )
			{
				Dx = x - Zones[i].x;
				Dy = y - Zones[i].y;

				DistanceSquared = Dx*Dx + Dy*Dy;

				if( MinDistanceSquared > DistanceSquared )
				{
					SizeHandles = Zones[i].SizeHandles;
					MinDistanceSquared = DistanceSquared;
				}

				if( SizeHandles != 0 && i == 3 )
					break;
			}
		}
	}
	
	return SizeHandles;
}

//=============================================================================
// GameView::PointToWorld
//=============================================================================

void GameView::PointToWorld( int mouse_x, int mouse_y, float &world_x, float &world_y )
{
	RECT rc;
	GetClientRect( &rc );

	float cx = (float) (rc.right - rc.left);
	float cy = (float) (rc.bottom - rc.top);

	world_x = mouse_x / cx * Sprite::SimulatedScreenWidth() + Sprite::ViewPortX();
	world_y = mouse_y / cy * Sprite::SimulatedScreenHeight() + Sprite::ViewPortY();
}

//=============================================================================
// GameView::PointToScreen
//=============================================================================

void GameView::PointToScreen( float world_x, float world_y, int &screen_x, int &screen_y )
{
	RECT rc;
	GetClientRect( &rc );

	int ClientWidth = rc.right - rc.left;
	int ClientHeight = rc.bottom - rc.top;

	float x = (world_x - Sprite::ViewPortX()) / Sprite::SimulatedScreenWidth();
	float y = (world_y - Sprite::ViewPortY()) / Sprite::SimulatedScreenHeight();

	screen_x = (int) (x * ClientWidth);
	screen_y = (int) (y * ClientHeight);
}

//=============================================================================
// GameView::MouseToWorld
//=============================================================================

void GameView::MouseToWorld( int mouse_x, int mouse_y, float &world_x, float &world_y )
{
	PointToWorld( mouse_x, mouse_y, world_x, world_y );

	int x = (int) (world_x + 0.5f);
	int y = (int) (world_y + 0.5f);

	if( m_GridLock )
	{
		x = x / 4 * 4;
		y = y / 4 * 4;
	}

	world_x = (float) x;
	world_y = (float) y;
}

//=============================================================================
// GameView::MouseToWorld
//=============================================================================

void GameView::MouseToWorld( CPoint mouse, float &world_x, float &world_y )
{
	MouseToWorld( mouse.x, mouse.y, world_x, world_y );
}

//=============================================================================
// GameView::SelectSprite
//=============================================================================

void GameView::SelectSprite( Sprite* pSprite )
{
	if( pSprite )
	{
		SpriteItem* pSelection = m_SelectionList.Child();

		while( pSelection )
		{
			if( pSelection->m_pSprite == pSprite )
				return;

			pSelection = pSelection->Next();
		}

		pSelection = new SpriteItem;

		if( pSelection )
		{
			pSelection->m_pSprite = pSprite;
			pSelection->AddRecord( SelectRecord::ClassnameStatic() );

			m_SelectionList.SetChild( pSelection );

			Sprite* pParent = pSprite;
			Screen* pScreen;

			while( pParent )
			{
				pScreen = (Screen*) pParent->GetInterface( Screen::ClassnameStatic() );

				if( !pScreen )
					pParent = pParent->Parent();
				else
				{
					SetTopSprite( pParent );
					break;
				}
			}

			DocumentSelectSprite( pSprite );

			KillTimer( TIMEREVENT_SPRITECHANGES );
			SetTimer( TIMEREVENT_SPRITECHANGES, 100, 0 );

			Invalidate();
		}
	}
	
	return;
}

//=============================================================================
// GameView::ReselectSprite
//=============================================================================

void GameView::ReselectSprite( Sprite* pSprite )
{
	if( pSprite )
	{
		SpriteItem* pSelection = m_SelectionList.Child();

		while( pSelection )
		{
			if( pSelection->m_pSprite == pSprite )
			{
				m_SelectionList.SetChild( pSelection );
				break;
			}

			pSelection = pSelection->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::SelectChildren
//=============================================================================

void GameView::SelectChildren( Sprite* pSprite )
{
	if( pSprite )
	{
		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			SelectSprite( pChild );
			SelectChildren( pChild );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::DeselectSprite
//=============================================================================

void GameView::DeselectSprite( Sprite* pSprite )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	SpriteItem* pNext;

	while( pSelection )
	{
		pNext = pSelection->Next();

		if( pSelection->m_pSprite == pSprite )
			delete pSelection;

		pSelection = pNext;
	}

	return;
}

//=============================================================================
// GameView::DeselectChildren
//=============================================================================

void GameView::DeselectChildren( Sprite* pSprite )
{
	if( pSprite )
	{
		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			DeselectSprite( pChild );
			DeselectChildren( pChild );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::DeselectAllChildren
//=============================================================================

void GameView::DeselectAllChildren( void )
{
	SpriteItem SelectionList;
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite && !IsAnyParentSelected( pSprite ) )
			AddToSpriteList( SelectionList, pSprite );

		pSelection = pSelection->Next();
	}

	pSelection = SelectionList.Child();

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
			DeselectChildren( pSprite );

		pSelection = pSelection->Next();
	}

	ClearSpriteList( SelectionList );

	return;
}

//=============================================================================
// GameView::DeselectAll
//=============================================================================

void GameView::DeselectAll( void )
{
	while( m_SelectionList.Child() )
		delete m_SelectionList.Child();

	return;
}

//=============================================================================
// GameView::SelectRect
//=============================================================================

void GameView::SelectRect( float x1, float y1, float x2, float y2 )
{
	DeselectAll();

	// find all the sprites inside this rect and select them

	SevenupDoc* pDocument = GetDocument();
	Sprite* pSceneList = TopSprite();
	Sprite* pSprite;

	if( !pSceneList )
		pSceneList = DocumentGetSceneList();

	if( pSceneList )
	{
		pSprite = pSceneList->Child();

		while( pSprite )
		{
			SelectRect( pSprite, x1, y1, x2, y2 );

			pSprite = pSprite->Next();
		}
	}

	return;
}

void GameView::SelectRect( Sprite* pSprite, float x1, float y1, float x2, float y2 )
{
	if( pSprite )
	{
		float ax, ay;
		float bx, by;

		pSprite->GetSpriteRect( ax, ay, bx, by );

		if( !IsLocked( pSprite) && RectOnRectf( ax, ay, bx, by, x1, y1, x2, y2 ) )
			SelectSprite( pSprite );

		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			SelectRect( pChild, x1, y1, x2, y2 );

			pChild = pChild->Next();
		}
	}
	
	return;
}

//=============================================================================
// GameView::SelectedSprite
//=============================================================================

Sprite* GameView::SelectedSprite( void )
{
	SpriteItem* pNode = m_SelectionList.Child();

	if( !pNode )
		return 0;

	return pNode->m_pSprite;
}

//=============================================================================
// GameView::HoveredSprite
//=============================================================================

Sprite* GameView::HoveredSprite( void )
{
	Sprite* pSprite = 0;
	SpriteItem* pHoverItem = m_HoverList.Child();

	if( pHoverItem )
		pSprite = pHoverItem->m_pSprite;

	return pSprite;
}

//=============================================================================
// GameView::IsAnyParentSelected
//=============================================================================

BOOL GameView::IsAnyParentSelected( Sprite* pSprite )
{
	if( !pSprite )
		return false;

	Sprite* pParent = pSprite->Parent();

	while( pParent )
	{
		if( IsSelected( pParent ) )
			return TRUE;

		pParent = pParent->Parent();
	}

	return FALSE;
}

//=============================================================================
// GameView::IsSelected
//=============================================================================

BOOL GameView::IsSelected( Sprite* pSprite )
{
	SpriteItem* pSelection = m_SelectionList.Child();

	while( pSelection )
	{
		if( pSelection->m_pSprite == pSprite )
			return TRUE;

		pSelection = pSelection->Next();
	}

	return FALSE;
}

//=============================================================================
// GameView::IsLocked
//=============================================================================

BOOL GameView::IsLocked( Sprite* pSprite )
{
	return Project.Locked( pSprite );
}

//=============================================================================
// GameView::IsDragged
//=============================================================================

BOOL GameView::IsDragged( Sprite* pSprite )
{
	if( m_MouseMode != MOUSEMODE_POSITION )
		return FALSE;

	return IsSelected( pSprite );
}

//=============================================================================
// GameView::GetSelectionRect
//=============================================================================

void GameView::GetSelectionRect( float &x1, float &y1, float &x2, float &y2 )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	float ax, ay;
	float bx, by;

	if( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
			pSprite->GetSpriteRect( x1, y1, x2, y2 );

		pSelection = pSelection->Next();

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
			{
				pSprite->GetSpriteRect( ax, ay, bx, by );

				if( x1 > ax ) x1 = ax;
				if( y1 > ay ) y1 = ay;
				if( x2 < bx ) x2 = bx;
				if( y2 < by ) y2 = by;
			}


			pSelection = pSelection->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::AddToSpriteList
//=============================================================================

SpriteItem* GameView::AddToSpriteList( SpriteItem& SpriteList, Sprite* pSprite )
{
	SpriteItem* pItem = SpriteList.Child();

	while( pItem )
	{
		if( pItem->m_pSprite == pSprite )
			break;

		pItem = pItem->Next();
	}

	if( !pItem )
	{
		pItem = new SpriteItem;

		if( pItem )
		{
			pItem->m_pSprite = pSprite;
			SpriteList.SetChild( pItem );
		}
	}

	return pItem;
}


//=============================================================================
// GameView::AddFamilyToSpriteList
//=============================================================================

void GameView::AddFamilyToSpriteList( SpriteItem& SpriteList, Sprite* pSprite )
{
	if( pSprite )
	{
		AddToSpriteList( SpriteList, pSprite );

		Sprite* pChild = pSprite->Child();

		while( pChild )
		{
			AddFamilyToSpriteList( SpriteList, pChild );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::RemoveFromSpriteList
//=============================================================================

void GameView::RemoveFromSpriteList( SpriteItem& SpriteList, Sprite* pSprite )
{
	SpriteItem* pItem = SpriteList.Child();
	SpriteItem* pNext;

	while( pItem )
	{
		pNext = pItem->Next();

		if( pItem->m_pSprite == pSprite )
			delete pItem;

		pItem = pNext;
	}

	return;
}

//=============================================================================
// GameView::ClearSpriteList
//=============================================================================

void GameView::ClearSpriteList( SpriteItem& SpriteList )
{
	while( SpriteList.Child() )
		delete SpriteList.Child();

	return;
}

//=============================================================================
// GameView::DeleteSprites
//=============================================================================

void GameView::DeleteSprites( SpriteItem& SpriteList)
{
	SpriteItem* pObject = SpriteList.Child();

	while( pObject )
	{
		DeleteSprite( pObject->m_pSprite );

		delete pObject;

		pObject = SpriteList.Child();
	}

	return;
}

//=============================================================================
// GameView::FindSpriteInList
//=============================================================================

SpriteItem* GameView::FindSpriteInList( SpriteItem& SpriteList, Sprite* pSprite )
{
	SpriteItem* pItem = SpriteList.Child();

	while( pItem )
	{
		if( pItem->m_pSprite == pSprite )
			return pItem;

		pItem = pItem->Next();
	}

	return 0;
}

//=============================================================================
// GameView::AddToHoverList
//=============================================================================

void GameView::AddToHoverList( Sprite* pSprite )
{
	AddToSpriteList( m_HoverList, pSprite );
}

//=============================================================================
// GameView::RemoveFromHoverList
//=============================================================================

void GameView::RemoveFromHoverList( Sprite* pSprite )
{
	RemoveFromSpriteList( m_HoverList, pSprite );
}

//=============================================================================
// GameView::ClearHoverList
//=============================================================================

void GameView::ClearHoverList( void )
{
	ClearSpriteList( m_HoverList );
}

//=============================================================================
// GameView::IsHoverSelected
//=============================================================================

BOOL GameView::IsHoverSelected( void )
{
	if( IsSelected( HoveredSprite() ) )
		return TRUE;

	SpriteItem* pHover = m_HoverList.Child();

	while( pHover )
	{
		if( IsSelected( pHover->m_pSprite ) )
			return TRUE;

		pHover = pHover->Next();
	}

	return FALSE;
}

//=============================================================================
// GameView::AddToPickList
//=============================================================================

void GameView::AddToPickList( Sprite* pSprite )
{
	AddToSpriteList( m_PickList, pSprite );
}

//=============================================================================
// GameView::RemoveFromPickList
//=============================================================================

void GameView::RemoveFromPickList( Sprite* pSprite )
{
	RemoveFromSpriteList( m_PickList, pSprite );
}

//=============================================================================
// GameView::ClearPickList
//=============================================================================

void GameView::ClearPickList( void )
{
	ClearSpriteList( m_PickList );
}

//=============================================================================
// GameView::FindPick
//=============================================================================

SpriteItem* GameView::FindPick( Sprite* pSprite )
{
	return FindSpriteInList( m_PickList, pSprite );
}

//=============================================================================
// GameView::PickNextSprite
//=============================================================================

void GameView::PickNextSprite( void )
{
	Sprite* pSprite = SelectedSprite();

	SpriteItem* pPick = FindPick( pSprite );
	SpriteItem* pNextPick;

	if( pPick )
	{
		pNextPick = pPick->Next();

		if( !pNextPick )
			pNextPick = m_PickList.Child();

		if( pNextPick != pPick )
		{
			DeselectSprite( pSprite );
			SelectSprite( pNextPick->m_pSprite );
		}
	}

	return;
}

//=============================================================================
// GameView::PickPrevSprite
//=============================================================================

void GameView::PickPrevSprite( void )
{
	Sprite* pSprite = SelectedSprite();

	SpriteItem* pPick = FindPick( pSprite );
	SpriteItem* pPrevPick;

	if( pPick )
	{
		pPrevPick = pPick->Prev();

		if( !pPrevPick )
			pPrevPick = (SpriteItem*) m_PickList.GetLastChild();

		if( pPrevPick != pPick )
		{
			DeselectSprite( pSprite );
			SelectSprite( pPrevPick->m_pSprite );
		}
	}

	return;
}

//=============================================================================
// GameView::AddToLockedListf
//=============================================================================

void GameView::AddToLockedList( Sprite* pSprite )
{
	AddToSpriteList( m_LockedList, pSprite );
}

//=============================================================================
// GameView::RemoveFromLockedList
//=============================================================================

void GameView::RemoveFromLockedList( Sprite* pSprite )
{
	RemoveFromSpriteList( m_LockedList, pSprite );
}

//=============================================================================
// GameView::ClearLockedList
//=============================================================================

void GameView::ClearLockedList( void )
{
	ClearSpriteList( m_LockedList );
}

//=============================================================================
// GameView::HoverListToPickList
//=============================================================================

void GameView::HoverListToPickList( void )
{
	ClearPickList();

	SpriteItem* pHover = m_HoverList.Child();
	Sprite* pSprite;

	while( pHover )
	{
		pSprite = pHover->m_pSprite;

		if( !IsLocked( pSprite ) && !IsSelected( pSprite ) )
			AddToPickList( pHover->m_pSprite );

		pHover = pHover->Next();
	}

	return;
}

//=============================================================================
// GameView::SetTopSprite
//=============================================================================

void GameView::SetTopSprite( Sprite* pSprite )
{
	m_pTopSprite = pSprite;
}

//=============================================================================
// GameView::TopSprite
//=============================================================================

Sprite* GameView::TopSprite( void )
{
	return m_pTopSprite;
}

//=============================================================================
// GameView::TopSprite
//=============================================================================

Screen* GameView::TopScreen( void )
{
	Screen* pScreen = 0;

	if( m_pTopSprite )
		pScreen = (Screen*) m_pTopSprite->GetInterface( Screen::ClassnameStatic() );

	if( pScreen == 0 )
	{
		SpriteItem* pSelection = m_SelectionList.Child();
		Sprite* pSprite;

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
				pScreen = (Screen*) pSprite->GetInterface( Screen::ClassnameStatic() );

			if( pScreen )
				break;

			pSelection = pSelection->Next();
		}
	}

	return pScreen;
}

//=============================================================================
// GameView::SetViewPortOffset
//=============================================================================

void GameView::SetViewPortOffset( float x, float y )
{
	m_ViewPortX = x;
	m_ViewPortY = y;
	
	Invalidate();
}

//=============================================================================
// GameView::ZoomIn
//=============================================================================

void GameView::ZoomIn( void )
{
	float cx = m_x;
	float cy = m_y;

	if( m_MouseTool != MOUSETOOL_ZOOM )
	{
		Sprite* pSprite = SelectedSprite();

		if( pSprite )
		{
			float x1, y1;
			float x2, y2;

			pSprite->GetSpriteRect( x1, y1, x2, y2 );

			cx = x1 + (x2 - x1) * 0.5f;
			cy = y1 + (y2 - y1) * 0.5f;
		}
		else
		{
		}
	}

	m_Zoom = m_Zoom / 1.25f;

	int Width, Height;
	DocumentGetScreenSize( Width, Height );

	float ScreenWidth = Width * m_Zoom;
	float ScreenHeight = Height * m_Zoom;

	float x = cx - ScreenWidth * 0.5f;
	float y = cy - ScreenHeight * 0.5f;

	SetViewPortOffset( x, y );
}

//=============================================================================
// GameView::ZoomOut
//=============================================================================

void GameView::ZoomOut( void )
{
	float cx = m_x;
	float cy = m_y;

	if( m_MouseTool != MOUSETOOL_ZOOM )
	{
		Sprite* pSprite = SelectedSprite();

		if( pSprite )
		{
			float x1, y1;
			float x2, y2;

			pSprite->GetSpriteRect( x1, y1, x2, y2 );

			cx = x1 + (x2 - x1) * 0.5f;
			cy = y1 + (y2 - y1) * 0.5f;
		}
		else
		{
		}
	}

	m_Zoom = m_Zoom * 1.25f;

	int Width, Height;
	DocumentGetScreenSize( Width, Height );

	float ScreenWidth = Width * m_Zoom;
	float ScreenHeight = Height * m_Zoom;

	float x = cx - ScreenWidth * 0.5f;
	float y = cy - ScreenHeight * 0.5f;

	SetViewPortOffset( x, y );
}

//=============================================================================
// GameView::UpdateScrollBars
//=============================================================================

void GameView::UpdateScrollBars( void )
{
}

//=============================================================================
// GameView::BeginMoveSelection
//=============================================================================

void GameView::BeginMoveSelection( void )
{
	float x, y;

	SpriteItem* pSelection = m_SelectionList.Child();
	SelectRecord* pRecord;
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;
		pRecord = (SelectRecord*) pSelection->GetRecord( SelectRecord::ClassnameStatic() );

		if( pSprite && pRecord )
		{
			pSprite->GetScreenPosition( x, y );
			//pRecord->m_ScreenPosX = x;
			//pRecord->m_ScreenPosY = y;

			pRecord->m_MousePosX = m_x - x;
			pRecord->m_MousePosY = m_y - y;
			pRecord->m_SpriteState = pSprite->m_state;
		}

		pSelection = pSelection->Next();
	}

	return;
}

//=============================================================================
// GameView::EndMoveSelection
//=============================================================================

void GameView::EndMoveSelection( void )
{
	return;
}

//=============================================================================
// GameView::MoveSelection
//=============================================================================

void GameView::MoveSelection( float x, float y )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite && !IsAnyParentSelected( pSprite ) )
			pSprite->Move( x, y );

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::SpaceSelection
//=============================================================================

void GameView::SpaceSelection( BOOL Horz, BOOL Vert )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	Sprite* pSprite1;
	Sprite* pSprite2;
	float top;
	float bottom;
	float left;
	float right;
	float horz, h;
	float vert, v;
	float x, y;
	float x1, y1;
	float x2, y2;
	float ax, ay;
	float bx, by;
	int i, c;

	Sprite* pSpriteTop;
	Sprite* pSpriteBottom;
	Sprite* pSpriteLeft;
	Sprite* pSpriteRight;

	bool Condition;

	c = m_SelectionList.GetNumChildren();

	if( c > 2 && pSelection )
	{
		Sprite** ppSprites = new Sprite*[c];

		if( !ppSprites )
			return;

		pSelection = m_SelectionList.Child();

		for( i = 0; pSelection; )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite && !IsSelected( pSprite->Parent() ) )
				ppSprites[i++] = pSelection->m_pSprite;

			pSelection = pSelection->Next();
		}

		pSelection = m_SelectionList.Child();

		c = i; // how many sprites there REALLY are.

		x = 0.0f;
		y = 0.0f;

		pSprite = pSelection->m_pSprite;

		if( pSprite )
			pSprite->GetScreenPosition( x, y );

		left = x;
		right = x;
		top = y;
		bottom = y;

		pSpriteTop = pSprite;
		pSpriteBottom = pSprite;
		pSpriteLeft = pSprite;
		pSpriteRight = pSprite;

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSprites[i];

			if( pSprite )
			{
				pSprite->GetScreenPosition( x, y );

				if( left > x )
				{
					left = x;
					pSpriteLeft = pSprite;
				}

				if( right < x )
				{
					right = x;
					pSpriteRight = pSprite;
				}

				if( top > y )
				{
					top = y;
					pSpriteTop = pSprite;
				}

				if( bottom < y )
				{
					bottom = y;
					pSpriteBottom = pSprite;
				}
			}
		}

		for( i = 0; i < (c-1); i++ )
		{
			if( i < 0 )
				i = 0;

			pSprite1 = ppSprites[i+0];
			pSprite2 = ppSprites[i+1];

			pSprite1->GetSpriteRect( x1, y1, x2, y2 );

			ax = x1 + (x2 - x1) * 0.5f;
			ay = y1 + (y2 - y1) * 0.5f;

			pSprite2->GetSpriteRect( x1, y1, x2, y2 );

			bx = x1 + (x2 - x1) * 0.5f;
			by = y1 + (y2 - y1) * 0.5f;

			if( Vert && !Horz )
				Condition = ay > by;
			else
			if( Horz && !Vert )
				Condition = ax > bx;
			else
				Condition = ay > by;

			if( Condition )
			{
				ppSprites[i+0] = pSprite2;
				ppSprites[i+1] = pSprite1;
				
				i -= 2;
			}
		}

		horz = bottom - top;
		vert = right - left;
		h = horz / (c-1);
		v = vert / (c-1);

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSprites[i];

			if( pSprite )
			{
				pSprite->GetScreenPosition( x, y );

				if( Horz )
					x = left + v * i;

				if( Vert )
					y = top + h * i;

				pSprite->SetScreenPosition( x, y );
			}
		}

		delete [] ppSprites;
	}

	Invalidate();
}

//=============================================================================
// GameView::AlignSelection
//=============================================================================

void GameView::AlignSelection( int Side )
{
	SpriteItem *pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	float x1, y1;
	float x2, y2;
	float x3, y3;
	float x4, y4;
	float dx, dy;

	if( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			pSprite->GetSpriteRect( x1, y1, x2, y2 );

			pSelection = pSelection->Next();

			while( pSelection )
			{
				pSprite = pSelection->m_pSprite;

				if( pSprite )
				{
					pSprite->GetSpriteRect( x3, y3, x4, y4 );

					dx = 0.0f;
					dy = 0.0f;

					switch( Side )
					{
					case 0:
						dy = y1 - y3;
						break;
					case 1:
						dy = y2 - y4;
						break;
					case 2:
						dx = x1 - x3;
						break;
					case 3:
						dx = x2 - x4;
					}

					pSprite->Move( dx, dy );
				}

				pSelection = pSelection->Next();
			}
		}
	}

	Invalidate();
}

//=============================================================================
// GameView::CutSelection
//=============================================================================

void GameView::CutSelection( void )
{
	CopySelection();
	DeleteSelection();
}


Sprite* GameView::AddSpriteToCopyList( Sprite* pSprite, Sprite* pParent )
{
	Sprite* pNewSprite = Project.DuplicateSprite( pSprite );
	SpriteItem* pSpriteItem;
	SelectRecord* pRecord;
	float x, y;

	if( pNewSprite )
	{
		if( pParent )
			pParent->AddChild( pNewSprite );
		else
		{
			pSpriteItem = AddToSpriteList( m_CopyList, pNewSprite );

			if( pSpriteItem )
			{
				pRecord = (SelectRecord*) pSpriteItem->AddRecord( SelectRecord::ClassnameStatic() );

				if( pRecord )
				{
					pRecord->m_SpriteState.position.x = pSprite->m_state.position.x;
					pRecord->m_SpriteState.position.y = pSprite->m_state.position.y;
				}
			}
		}

		pSprite->GetScreenPosition( x, y );
		pNewSprite->SetScreenPosition( x, y );
	}

	return pNewSprite;
}

//=============================================================================
// GameView::CopySelection
//=============================================================================

void GameView::CopySelection( void )
{
	DeleteSprites( m_CopyList );

	SpriteItem* pSelection;
	Sprite* pSprite;

	pSelection = m_SelectionList.Child();

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite && !IsAnyParentSelected( pSprite ) )
			CopySelectedChildren( pSprite, 0 );

		pSelection = pSelection->Next();
	}

	return;
}

//=============================================================================
// GameView::CopySelectedChildren
//=============================================================================

void GameView::CopySelectedChildren( Sprite* pSprite, Sprite* pParent )
{
	Sprite* pChild;

	if( pSprite )
	{
		if( IsSelected( pSprite ) )
			pParent = AddSpriteToCopyList( pSprite, pParent );

		pChild = pSprite->Child();

		while( pChild )
		{
			CopySelectedChildren( pChild, pParent );

			pChild = pChild->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::PasteSelection
//=============================================================================

void GameView::PasteSelection( BOOL Select )
{
	SevenupDoc* pDocument;
	SpriteItem* pSelection;
	Sprite* pNewSprite;
	Sprite* pSprite;

	pDocument = GetDocument();

	if( Select )
		DeselectAll();

	if( pDocument )
	{
		pSelection = m_CopyList.Child();

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
			{
				pNewSprite = pDocument->DuplicateSpriteFamily( pSprite );

				if( Select && pNewSprite )
				{
					Sprite* pParent = TopSprite();

					if( pParent )
						pParent->AddChild( pNewSprite );

					SelectSprite( pNewSprite );
					SelectChildren( pNewSprite );
				}
			}

			pSelection = pSelection->Next();
		}
	}

	DocumentSyncProjectViews();

	return;
}

//=============================================================================
// GameView::DeleteSelection
//=============================================================================

void GameView::DeleteSelection( void )
{
	SpriteItem* pSelection;
	Sprite* pSprite;
	Sprite* pChild;
	int Result = IDOK;

	int Result1 = IDOK;
	int Result2 = IDNO;
	BOOL Confirmed = FALSE;

	if( m_SelectionList.GetNumChildren() > 1 )
	{
		Result1 = MessageBox( "Are you sure you want to delete the selected objects?", 0, MB_OKCANCEL|MB_ICONQUESTION );
	}

	if( Result1 == IDOK )
	{
		pSelection = m_SelectionList.Child();

		while( pSelection && !Confirmed )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
			{
				pChild = pSprite->Child();

				while( pChild && !Confirmed)
				{
					if( !IsSelected( pChild ) )
					{
						Result2 = MessageBox( "Some of the objects you are deleting contain children that are not selected.\n"
													 "Do you want to delete those objects, too?", 0, MB_YESNOCANCEL|MB_ICONEXCLAMATION );

						Confirmed = TRUE;
					}

					pChild = pChild->Next();
				}
			}

			pSelection = pSelection->Next();
		}

		if( Result2 != IDCANCEL )
		{
			pSelection = m_SelectionList.Child();

			while( pSelection )
			{
				pSprite = pSelection->m_pSprite;

				if( Result2 == IDYES )
					DeleteFamilyFromProject( pSprite );
				else
					DeleteSpriteFromProject( pSprite );

				pSelection = m_SelectionList.Child();
			}

			Invalidate();
		}

		DocumentSyncProjectViews();
	}

	return;
}

//=============================================================================
// GameView::SyncMouseSprite
//=============================================================================

void GameView::SyncMouseSprite( void )
{
	BOOL ShowSprite = FALSE;

	if( m_MouseTool == MOUSETOOL_NEW && m_MouseMode == MOUSEMODE_HOVER )
		ShowSprite = TRUE;

	if( ShowSprite == FALSE )
	{
		m_pNewSprite = Project.DeleteSprite( m_pNewSprite );
	}
	else
	{
		t_Handle Handle = DocumentGetTextureHandle();
		int Texture = DocumentGetTextureIndex();

		if( m_pNewSprite )
		{
			if( Project.GetObjectType( m_pNewSprite ) != m_ObjectType )
				m_pNewSprite = Project.DeleteSprite( m_pNewSprite );
		}

		if( !m_pNewSprite )
			m_pNewSprite = CreateSprite( m_ObjectType );

		if( m_pNewSprite )
		{
			if( m_pNewSprite->GetTextureSet() != INVALID_HANDLE )
			{
				if( m_pNewSprite->GetTextureSet() != Handle || m_pNewSprite->GetTexture() != Texture )
					m_pNewSprite->SetTextureSet( Handle, Texture );

				m_pNewSprite->UseTextureSize();
			}

			m_pNewSprite->SetScreenPosition( m_x, m_y );
			m_pNewSprite->SetPriority( 0x7fff );
			m_pNewSprite->Show();
			m_pNewSprite->Update();
		}
	}

	return;
}

//=============================================================================
// GameView::OnDraw
//=============================================================================

void GameView::OnDraw( CDC *pdc )
{
	if( m_SelectRect.GetTextureSet() != App.DefaultTextureHandle() )
	{
		m_SelectRect.Create( App.DefaultTextureHandle(), 0 );
		m_SelectRect.SetBaseColor( 0.0f, 0.0f, 1.0f, 0.4f );
		m_SelectRect.SetPriority( 0x7fff );
		m_SelectRect.Hide();
	}

	SevenupDoc *pDocument = GetDocument();

	RECT rc;
	GetClientRect( &rc );

	m_FrameTime = Timer() - m_GameTime;
	m_GameTime = m_GameTime + m_FrameTime;

	Animator::UpdateAll( (float) m_FrameTime );

	if( SetupRenderTarget() )
	{
		int DocWidth, DocHeight;
		DocumentGetScreenSize( DocWidth, DocHeight );

		float ScreenWidth = DocWidth * m_Zoom;
		float ScreenHeight = DocHeight * m_Zoom;

		Sprite::SimulateScreenResolution( ScreenWidth, ScreenHeight );
		Sprite::SetViewPortOrigin( m_ViewPortX, m_ViewPortY );

		HRESULT hresult = D3DRESULT( d3d.Clear( 0, 0, D3DCLEAR_TARGET, 0x000000, 1.0f, 0 ) );

		if( m_pSceneList && m_pTopSprite != m_pSceneList )
			m_pSceneList->ReleaseHandlesRecursively();

		if( m_pTopSprite )
			m_pSceneList = m_pTopSprite;
		else
			m_pSceneList = DocumentGetSceneList();

		if( m_pSceneList )
			m_pSceneList->Update();

		ProjectItem *pProjectItem;

		SpriteItem* pSelection = m_SelectionList.Child();
		Sprite* pSprite;
		float Width;
		float Height;
		float x1, y1;
		float x2, y2;
		Graphics4x4 Transform;

		COLOR32 Color;
		COLOR32 Color1 = m_SelColor;
		COLOR32 Color2 = m_MultiSelColor;

		if( m_MouseTool == MOUSETOOL_GROUP )
			Color2 = m_GroupSelColor;

		int MaxNames = sizeof(m_DisplayName)/sizeof(m_DisplayName[0]);

		for( int i = 0; i < MaxNames; i++ )
		{
			m_DisplayName[i].Hide();

			if( pSelection && (m_ViewMode & VIEWMODE_SHOWNAMES) )
			{
				pSprite = pSelection->m_pSprite;

				if( pSelection == m_SelectionList.Child() )
					Color = Color1;
				else
					Color = Color2;

				if( pSprite )
				{
					pSprite->GetSpriteRect( x1, y1, x2, y2 );
					pProjectItem = Project.GetProjectItem( pSprite );

					if( pProjectItem )
					{
						Transform = pSprite->GetFamilyTransform();
						Width = pSprite->GetWidth();
						Height = pSprite->GetHeight();

						m_DisplayName[i].Printf( "%s", pProjectItem->m_Name.get() );
						m_DisplayName[i].SetFont( App.GetSystemFont() );
						m_DisplayName[i].SetScreenPosition( x1, y2 );
						m_DisplayName[i].SetOrigin( 0.0f, 0.0f );
						m_DisplayName[i].SetTextHeight( 12.0f );
						m_DisplayName[i].ShadowSettings( false, 128, 2, 2 );
						m_DisplayName[i].SetPriority( 0x7fff );
						m_DisplayName[i].SetBaseColor( Color );
						m_DisplayName[i].Show();
					}
				}

				pSelection = pSelection->Next();
			}

			m_DisplayName[i].Update();
		}

		pSprite = HoveredSprite();

		if( pSprite )
		{
			pProjectItem = (ProjectItem *) pSprite->m_pToolData;

			if( pProjectItem )
			{
				float width = pSprite->GetWidth();
				float height = pSprite->GetHeight();

				Graphics4x4 transform = pSprite->GetFamilyTransform();
				transform.CatTranslation( 0.0f, 0.0f, 0.0f );

				char *pName = pProjectItem->m_Name.get();
				char *pDesc = pProjectItem->m_Description.get();

				if( !pName )
					pName = "< error >";

				if( !pDesc )
					pDesc = "";

				char szDisplayText[4096];

				sprintf( szDisplayText, "%s\n%s", pName, pDesc );

				m_NameHover.SetFont( App.GetSystemFont() );
				m_NameHover.SetText( szDisplayText );
				m_NameHover.SetPosition( m_x + 8.0f, m_y );
				m_NameHover.SetTextHeight( 12.0f );
				m_NameHover.SetPriority( 0x7fff );
				m_NameHover.SetColor( m_HoverColor );
				m_NameHover.Show();
			}
		}

		if( HoveredSprite() == 0 || HoveredSprite() == SelectedSprite() )
			m_NameHover.Hide();

		m_NameHover.Update();

		SyncMouseSprite();

		if( m_MouseTool == MOUSETOOL_STAMP )
		{
			SpriteItem* pCopyItem = m_CopyList.Child();
			
			while( pCopyItem )
			{
				pSprite = pCopyItem->m_pSprite;

				if( pSprite )
					pSprite->Update();

				pCopyItem = pCopyItem->Next();
			}
		}

	//	DrawButtonOrder();

		Sprite::DrawAll();

		sm.RenderAllSprites( d3d );

		// Draw somes lines

		LPDIRECT3DSURFACE9 pRenderTarget;
		hresult = D3DRESULT( d3d.GetRenderTarget( 0, &pRenderTarget ) );

		if( !FAILED(hresult) )
		{
			D3DSURFACE_DESC SurfaceDesc;

			hresult = D3DRESULT( pRenderTarget->GetDesc( &SurfaceDesc ) );

			if( !FAILED(hresult) )
			{
				D3DLOCKED_RECT Lock;
				hresult = D3DRESULT( pRenderTarget->LockRect( &Lock, NULL, 0 ) );

				if( !FAILED(hresult) )
				{
					int x, y;
					
					PointToScreen( m_x, m_y, x, y );

					BITMAP2 bm2;

					bm2.Width = SurfaceDesc.Width;
					bm2.Height = SurfaceDesc.Height;
					bm2.Format = PixelFormatFromDirectX( SurfaceDesc.Format );
					bm2.Stride = Lock.Pitch;
					bm2.pBits = (BYTE *) Lock.pBits;

					OnCustomDraw( &bm2 );

					pRenderTarget->UnlockRect();
				}
			}
			
			pRenderTarget->Release();
		}

		hresult = D3DRESULT( m_pSwapChain->Present( 0, 0, m_hWnd, 0, 0 ) );

		if( FAILED(hresult) )
		{
			GdiFillRect( pdc->m_hDC, 0, 0, rc.right, rc.bottom, 0xffffff );

			GdiCenterText( pdc->m_hDC, rc.left, rc.top, rc.right, rc.bottom, "An error occured." );
		}
		else
		{
		}
	}

	UpdateOemCursor();

	return;
}

//=============================================================================
// GameView::OnCustomDraw
//=============================================================================

void GameView::OnCustomDraw( BITMAP2 *pBitmap )
{
	BeginBitmap( 0, pBitmap );

	float ScreenWidth = Sprite::SimulatedScreenWidth();
	float ScreenHeight = Sprite::SimulatedScreenHeight();
	float SafeZoneX = 0.05f;
	float SafeZoneY = 0.05f;
	BOOL DrawBorder = TRUE;
	BOOL DrawSafeZone = TRUE;

	SevenupDoc* pDocument = GetDocument();

	if( pDocument )
	{
		ScreenWidth = (float) pDocument->m_Params.m_ScreenWidth;
		ScreenHeight = (float) pDocument->m_Params.m_ScreenHeight;
		DrawBorder = pDocument->m_Params.m_DrawBorder;
		DrawSafeZone = pDocument->m_Params.m_DrawSafeZone;
		SafeZoneX = pDocument->m_Params.m_SafeZoneX * 0.01f;
		SafeZoneY = pDocument->m_Params.m_SafeZoneY * 0.01f;
	}

	int x, y;
	int x1, y1;
	int x2, y2;

	if( DrawBorder )
	{
		PointToScreen( 0.0f, 0.0f, x1, y1 );
		PointToScreen( ScreenWidth, ScreenHeight, x2, y2 );

		FastRect( 0, x1, y1, x2, y2, 0x77777777 );
	}

	if( DrawSafeZone )
	{
		PointToScreen( ScreenWidth * (0.0f + SafeZoneX), ScreenHeight * (0.0f + SafeZoneY), x1, y1 );
		PointToScreen( ScreenWidth * (1.0f - SafeZoneX), ScreenHeight * (1.0f - SafeZoneY), x2, y2 );

		FastRect( 0, x1, y1, x2, y2, 0x77777777 );
	}

	if( m_ViewMode & VIEWMODE_SHOWBOXES )
		DrawBoxes( pBitmap );

	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSelSprite = SelectedSprite();

	if( pSelSprite )
	{
		Sprite *pSprite = pSelSprite;

		int Width = pBitmap->Width;
		int Height = pBitmap->Height;
		float x, y;

		if( m_MouseMode == MOUSEMODE_ROTATE )
		{
			pSprite->GetScreenPosition( x, y );

			PointToScreen( x, y, x1, y1 );

			FastLine( 0, 0, y1, Width, y1, 0xff6666 );
			FastLine( 0, x1, 0, x1, Height, 0xff6666 );
			
			DrawEllipse( 0, x1-16, y1-16, x1+16, y1+16, 0xff6666 );

			PointToScreen( m_x2, m_y2, x2, y2 );

			FastLine( 0, x1, y1, x2, y2, 0xaaffaa );
		}

		if( m_MouseMode == MOUSEMODE_ROTATETEXTURE )
		{
			float OldX = pSprite->m_state.origin.x;
			float OldY = pSprite->m_state.origin.y;

			pSprite->SetOrigin( 0.5f, 0.5f );
			pSprite->GetScreenPosition( x, y );
			pSprite->SetOrigin( OldX, OldY );

			PointToScreen( x, y, x1, y1 );

			FastLine( 0, 0, y1, Width, y1, 0xff0000 );
			FastLine( 0, x1, 0, x1, Height, 0xff0000 );

			DrawEllipse( 0, x1-32, y1-32, x1+32, y1+32, 0xffffff );

			PointToScreen( m_x2, m_y2, x2, y2 );

			FastLine( 0, x1, y1, x2, y2, 0x00ff00 );
		}
	}

	PointToScreen( m_x, m_y, x, y );
	PointToScreen( m_x1, m_y1, x1, y1 );
	PointToScreen( m_x2, m_y2, x2, y2 );

	if( m_MouseTool == MOUSETOOL_SELECT )
	{
		FastLine( 0, 0, y, pBitmap->Width, y, 0xffffff );
		FastLine( 0, x, 0, x, pBitmap->Height, 0xffffff );
		FastRect( 0, x1, y1, x2, y2, 0xffffff );
	}
	
	FinishBitmap(0);
}

void GameView::DrawBoxes( BITMAP2* pBitmap )
{
	SpriteItem* pSelection;
	Sprite* pSelSprite;
	COLOR32 FrameColor;
	COLOR32 Color1;
	COLOR32 Color2;

	Color1 = m_SelColor;
	Color2 = m_MultiSelColor;

	if( m_MouseTool == MOUSETOOL_GROUP )
		Color2 = m_GroupSelColor;

	pSelection = m_SelectionList.Child();

	while( pSelection )
	{
		pSelSprite = pSelection->m_pSprite;

		if( pSelSprite )
		{
			if( pSelection == m_SelectionList.Child() )
				FrameColor = Color1;
			else
				FrameColor = Color2;

			DrawFrameAroundSprite( pBitmap, pSelSprite, FrameColor );
		}

		pSelection = pSelection->Next();
	}

	if( m_ShiftKey || m_ControlKey || m_AltKey )
	{
		SpriteItem* pSpriteList = m_LockedList.Child();
		Sprite* pSprite;

		while( pSpriteList )
		{
			pSprite = pSpriteList->m_pSprite;

			if( pSprite )
				DrawFrameAroundSprite( pBitmap, pSprite, m_LockedColor );

			pSpriteList = pSpriteList->Next();
		}

		pSpriteList = m_HoverList.Child();

		while( pSpriteList )
		{
			pSprite = pSpriteList->m_pSprite;

			if( pSprite )
			{
				if( pSprite == HoveredSprite() )
					FrameColor = m_HoverColor;
				else
					FrameColor = ColorBlend( m_HoverColor, 0xff000000, 128 );

				if( IsSelected( pSprite ) )
					FrameColor = ColorBlend( FrameColor, m_SelColor, 128 );

				DrawFrameAroundSprite( pBitmap, pSprite, FrameColor );
			}

			pSpriteList = pSpriteList->Next();
		}

		DrawFrameAroundSprite( pBitmap, m_pNewSprite, 0x0000ff );
	}

	pSelection = m_SelectionList.Child();

	if( m_MouseTool == MOUSETOOL_GROUP )
	{
		pSelection = m_SelectionList.Child();

		Sprite* pParent;
		Sprite* pChild;

		while( pSelection )
		{
			pParent = pSelection->m_pSprite;

			if( pParent )
			{
				pChild = pParent->Child();

				while( pChild )
				{
					DrawFrameAroundSprite( pBitmap, pChild, 0x00ff00 );
					pChild = pChild->Next();
				}
			}

			pSelection = pSelection->Next();
		}
	}

	return;
}

void GameView::DrawButtonOrder( void )
{
	int c = sizeof(m_ButtonOrderBox) / sizeof(m_ButtonOrderBox[0]);

	float BoxWidth;

	for( int i = 0; i < c; i++ )
	{
		m_ButtonOrderText[i].SetTextHeight( 18.0f );
		m_ButtonOrderText[i].Printf( "%d", i );

		BoxWidth = m_ButtonOrderText[i].ComputeTextLength() + 8.0f;

		m_ButtonOrderBox[i].AddChild( &m_ButtonOrderText[i] );
		m_ButtonOrderBox[i].Create( App.DefaultTextureHandle(), 0 );
		m_ButtonOrderBox[i].SetSize( BoxWidth, 18.0f );
		m_ButtonOrderBox[i].SetOrigin( 0.5f, 0.5f );
		m_ButtonOrderBox[i].SetPriority( 0x7eff );
		m_ButtonOrderBox[i].SetColor( 0, 0, 255, 100 );

		m_ButtonOrderText[i].SetPosition( BoxWidth * 0.5f, 9.0f );
		m_ButtonOrderText[i].SetOrigin( 0.5f, 0.5f );
	}

	Screen* pScreen = TopScreen();

	if( pScreen )
	{
		//Sprite* pSprite = pScreen->GetSprite( pScreen->m_Param.InitialButton );
		Sprite* pSprite = 0;
		Button* pButton = 0;
		float x1, y1;
		float x2, y2;
		
		for( int i = 0; pSprite; i++ )
		{
			if( i < c )
			{
				pSprite->GetSpriteRect( x1, y1, x2, y2 );

				m_ButtonOrderBox[i].SetScreenPosition( x1, y1 );
				m_ButtonOrderBox[i].Update();
			}
			else
				break;

			pButton = (Button*) pSprite->GetInterface( Button::ClassnameStatic() );

	//		if( pButton && pButton->m_Param.NextButton != pScreen->m_Param.InitialButton )
	//			pSprite = pScreen->GetSprite( pButton->m_Param.NextButton );
	//		else
				pSprite = 0;
		}
	}

	return;
}

//=============================================================================
// GameView::OnNewDocument
//=============================================================================

void GameView::OnNewDocument( void )
{
	DeselectAll();

	m_pNewSprite = Project.DeleteSprite( m_pNewSprite );

	m_pSceneList = 0;
	m_pTopSprite = 0;
}

//=============================================================================
// GameView::DrawFrameAroundSprite
//=============================================================================

void GameView::DrawFrameAroundSprite( BITMAP2 *pBitmap, Sprite *pSprite, COLOR32 Color )
{
	if( pBitmap && pSprite )
	{
		BeginBitmap( 0, pBitmap );

		Vector3 v[4];

		pSprite->GetSpriteVertices(v);

		int x1, y1;
		int x2, y2;
		int x3, y3;
		int x4, y4;

		PointToScreen( v[0].x(), v[0].y(), x1, y1 );
		PointToScreen( v[1].x(), v[1].y(), x2, y2 );
		PointToScreen( v[2].x(), v[2].y(), x3, y3 );
		PointToScreen( v[3].x(), v[3].y(), x4, y4 );

		FastLine( 0, x1, y1, x2, y2, Color );
		FastLine( 0, x2, y2, x3, y3, Color );
		FastLine( 0, x3, y3, x4, y4, Color );
		FastLine( 0, x4, y4, x1, y1, Color );

		FinishBitmap(0);
	}

	return;
}

//=============================================================================
// GameView::SizeHandlesToOemCursor
//=============================================================================

void GameView::SizeHandlesToOemCursor( int SizeHandles )
{
	if( SizeHandles == 0 )
		SetOemCursor( OCR_HAND );
	else
	{
		switch( SizeHandles )
		{
		case SIZEHANDLE_LEFT:
		case SIZEHANDLE_RIGHT:
			SetOemCursor( OCR_SIZEWE );
			break;

		case SIZEHANDLE_TOP:
		case SIZEHANDLE_BOTTOM:
			SetOemCursor( OCR_SIZENS );
			break;

		case SIZEHANDLE_LEFT|SIZEHANDLE_TOP:
		case SIZEHANDLE_RIGHT|SIZEHANDLE_BOTTOM:
			SetOemCursor( OCR_SIZENWSE );
			break;

		case SIZEHANDLE_RIGHT|SIZEHANDLE_TOP:
		case SIZEHANDLE_LEFT|SIZEHANDLE_BOTTOM:
			SetOemCursor( OCR_SIZENESW );
			break;
		}
	}

	return;
}

//=============================================================================
// GameView::OnMouseHover
//=============================================================================

void GameView::OnMouseHover( UINT Flags, CPoint p )
{
	SetMouseHover( p.x, p.y );

	if( m_MouseTool == MOUSETOOL_NEW )
	{
		SetOemCursor( OCR_NORMAL );
	}
	else
	if( m_MouseTool == MOUSETOOL_DRAG )
	{
		BOOL SizeHandles = FALSE;

		if( m_AllowSizing && IsHoverSelected() && m_SelectionList.GetNumChildren() == 1 )
			SizeHandles = TRUE;

		if( SizeHandles )
			SizeHandlesToOemCursor( GetSizeHandles( p.x, p.y ) );
		else
		if( HoveredSprite() )
			SetOemCursor( OCR_HAND );
		else
		{
			SetOemCursor( OCR_NORMAL );
		}
	}
	else
	if( m_MouseTool == MOUSETOOL_SELECT )
	{
		SetOemCursor( OCR_NORMAL );

		if( Flags & MK_LBUTTON )
		{
			MouseToWorld( p.x, p.y, m_x2, m_y2 );

			float RectWidth = m_x2 - m_x1;
			float RectHeight = m_y2 - m_y1;

			if( RectWidth == 0.0f )
				RectWidth = 0.00001f;

			if( RectHeight == 0.0f )
				RectHeight = 0.00001f;

			m_SelectRect.SetPosition( m_x1, m_y1 );
			m_SelectRect.SetSize( RectWidth, RectHeight );
			m_SelectRect.SetOrigin( 0.0f, 0.0f );
			m_SelectRect.Show();
			m_SelectRect.Update();
			m_SelectRect.Draw();

			SelectRect( m_x1, m_y1, m_x2, m_y2 );
		}
		else
		{
			m_x1 = m_x2 = m_x;
			m_y1 = m_y2 = m_y;

			m_SelectRect.Hide();
			m_SelectRect.Update();
		}
	}
	else
	if( m_MouseTool == MOUSETOOL_ZOOM )
	{
	}
	else
	if( m_MouseTool == MOUSETOOL_STAMP )
	{
		float x1, y1;
		float x2, y2;
		float ax, ay;
		float bx, by;
		float cx, cy;
		float dx, dy;

		SpriteItem* pCopyItem = m_CopyList.Child();
		Sprite* pSprite;

		if( pCopyItem )
		{
			pSprite = pCopyItem->m_pSprite;

			if( pSprite )
				pSprite->GetFamilyRect( ax, ay, bx, by );

			pCopyItem = pCopyItem->Next();

			while( pCopyItem )
			{
				pSprite = pCopyItem->m_pSprite;

				if( pSprite )
				{
					pSprite->GetFamilyRect( x1, y1, x2, y2 );

					if( ax > x1 ) ax = x1;
					if( ay > y1 ) ay = y1;
					if( bx < x2 ) bx = x2;
					if( by < y2 ) by = y2;
				}

				pCopyItem = pCopyItem->Next();
			}

			cx = ax + (bx - ax) * 0.5f;
			cy = ay + (by - ay) * 0.5f;

			dx = m_x - cx;
			dy = m_y - cy;

			pCopyItem = m_CopyList.Child();

			while( pCopyItem )
			{
				pSprite = pCopyItem->m_pSprite;

				if( pSprite )
					pSprite->Move( dx, dy );

				pCopyItem = pCopyItem->Next();
			}
		}
	}

	Invalidate();
}

//=============================================================================
// GameView::OnMousePositionBegin
//=============================================================================

void GameView::OnMousePositionBegin( UINT Flags, CPoint p )
{
	BeginMoveSelection();

	SetMouseMode( MOUSEMODE_POSITION );

	m_x1 = m_x;
	m_y1 = m_y;

	OnMouseMove( Flags, p );
}

//=============================================================================
// GameView::OnMousePosition
//=============================================================================

typedef struct
{
	float x1, y1;
	float x2, y2;
}
Line2f;

void GameView::OnMousePosition( UINT Flags, CPoint p )
{
	SetMouseHover( p.x, p.y );

	MouseToWorld( p.x, p.y, m_x2, m_y2 );

	SpriteItem* pSelection = m_SelectionList.Child();
	SelectRecord* pRecord;
	Sprite* pSprite;
	float mx, my;
	float ox, oy;
	float sx, sy;

	mx = m_x2;
	my = m_y2;

	if( m_PositionLock == 1 )
	{
		float dx = m_x2 - m_x1;
		float dy = m_y2 - m_y1;

		dx = Math::Abs(dx);
		dy = Math::Abs(dy);

		if( dx*dx + dy*dy > 8.0f )
		{
			if( dx < dy )
				m_PositionLock = 2;
			else
				m_PositionLock = 3;
		}
	}
	
	if( m_PositionLock == 2 )
	{
		mx = m_x1;
	}
	else
	if( m_PositionLock == 3 )
	{
		my = m_y1;
	}

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite && !IsAnyParentSelected( pSprite ) )
		{
			pRecord = (SelectRecord*) pSelection->GetRecord( SelectRecord::ClassnameStatic() );

			if( pRecord )
			{
				ox = pRecord->m_MousePosX;
				oy = pRecord->m_MousePosY;
			//	ox = pSelection->m_mouse_x;
			//	oy = pSelection->m_mouse_y;

				sx = mx - ox;
				sy = my - oy;

				if( !(Flags & (MK_LBUTTON|MK_RBUTTON) ) )
				{
					if( m_MouseTool == MOUSETOOL_DRAG )
						SetMouseMode( MOUSEMODE_HOVER );

					OnMouseMove( Flags, p );
				}
				else
				{
					pSprite->SetScreenPosition( sx, sy );

					SetOemCursor( OCR_HAND );
				}
			}
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnMouseSizeBegin
//=============================================================================

void GameView::OnMouseSizeBegin( int SizeHandles, CPoint p )
{
	Sprite* pSelSprite = SelectedSprite();

	if( pSelSprite )
	{
		SetMouseMode( MOUSEMODE_SIZE );

		MouseToWorld( p.x, p.y, m_x1, m_y1 );

		pSelSprite->GetSpriteRect( m_SizeX1, m_SizeY1, m_SizeX2, m_SizeY2 );

		if( SizeHandles & SIZEHANDLE_LEFT )
			m_ox = m_x - m_SizeX1;

		if( SizeHandles & SIZEHANDLE_RIGHT )
			m_ox = m_x - m_SizeX2;

		if( SizeHandles & SIZEHANDLE_TOP )
			m_oy = m_y - m_SizeY1;

		if( SizeHandles & SIZEHANDLE_BOTTOM )
			m_oy = m_y - m_SizeY2;

		m_SizeHandles = SizeHandles;
	}

	return;
}

//=============================================================================
// GameView::OnMouseSize
//=============================================================================

void GameView::OnMouseSize( UINT Flags, CPoint p )
{
	Sprite* pSelSprite = SelectedSprite();

	MouseToWorld( p.x, p.y, m_x2, m_y2 );

	if( !(Flags & MK_LBUTTON) )
	{
		SetMouseMode( MOUSEMODE_HOVER );

		m_SizeHandles = 0;
	}
	else
	if( pSelSprite )
	{
		if( m_SizeHandles == 0 )
		{
			float Width = m_x2 - m_x1;
			float Height = m_y2 - m_y1;

			if( Width == 0.0f )
				Width = 0.001f;

			if( Height == 0.0f )
				Height = 0.001f;

			pSelSprite->SetSize( Width, Height );

			SetOemCursor( OCR_CROSS );
		}
		else
		{
			float SizeX1 = m_SizeX1;
			float SizeY1 = m_SizeY1;
			float SizeX2 = m_SizeX2;
			float SizeY2 = m_SizeY2;

			float w, h;

			if( m_SizeHandles & SIZEHANDLE_LEFT )
				SizeX1 = m_x2;

			if( m_SizeHandles & SIZEHANDLE_RIGHT )
				SizeX2 = m_x2;

			if( m_SizeHandles & SIZEHANDLE_TOP )
				SizeY1 = m_y2;
			
			if( m_SizeHandles & SIZEHANDLE_BOTTOM )
				SizeY2 = m_y2;

			SizeHandlesToOemCursor( m_SizeHandles );

			Graphics4x4 transform;
			Sprite *pParent = pSelSprite->Parent();

			if( pParent )
				transform = pParent->GetFamilyTransformInverse();
			else
				transform.Identity();

			float OldWidth = m_SizeX2 - m_SizeX1;
			float OldHeight = m_SizeY2 - m_SizeY1;
			float NewWidth = SizeX2 - SizeX1;
			float NewHeight = SizeY2 - SizeY1;

			Sprite* pChild = pSelSprite->Child();

			float childx = 0.0f;
			float childy = 0.0f;

			if( pChild )
				pChild->GetScreenPosition( childx, childy );

			w = pSelSprite->GetWidth();
			h = pSelSprite->GetHeight();

			pSelSprite->SetSize( NewWidth, NewHeight );

			float x1, y1;
			float x2, y2;

			pSelSprite->GetSpriteRect( x1, y1, x2, y2 );

			Vector3 vec1 = transform.Transform( SizeX1, SizeY1, 0.0f );
			Vector3 vec2 = transform.Transform( x1, y1, 0.0f );
			Vector3 vec3 = vec1 - vec2;

			float dx = vec3.x();
			float dy = vec3.y();

			pSelSprite->Move( dx, dy );

			if( !m_ShiftKey )
			{
				if( pChild )
				{
					pChild->GetPosition( dx, dy );
					pChild->SetScreenPosition( childx, childy );
					pChild->GetPosition( childx, childy );
					
					dx = childx - dx;
					dy = childy - dy;

					pChild = pChild->Next();

					while( pChild )
					{
						pChild->Move( dx, dy );
						pChild = pChild->Next();
					}
				}
			}
			else
			{
				// Resize everything

				while( pChild )
				{
					childx = pChild->GetWidth() / w;
					childy = pChild->GetHeight() / h;

					dx = childx * NewWidth;
					dy = childy * NewHeight;

					pChild->SetSize( dx, dy );

					pChild->GetPosition( childx, childy );

					dx = childx / w * NewWidth;
					dy = childy / h * NewHeight;

					pChild->SetPosition( dx, dy );

					pChild = pChild->Next();
				}
			}
		}
	}

	Invalidate();
}

//=============================================================================
// GameView::OnMouseRotateBegin
//=============================================================================

void GameView::OnMouseRotateBegin( void )
{
}

//=============================================================================
// GameView::OnMouseRotate
//=============================================================================

void GameView::OnMouseRotate( UINT Flags, CPoint p )
{
}

//=============================================================================
// GameView::OnMouseDuplicateBegin
//=============================================================================

void GameView::OnMouseDuplicateBegin( UINT Flags, CPoint p )
{
	BeginMoveSelection();

	SetMouseMode( MOUSEMODE_DUPLICATE );

	m_x1 = m_x;
	m_y1 = m_y;

	OnMouseMove( Flags, p );
}

//=============================================================================
// GameView::OnMouseDuplicate
//=============================================================================

void GameView::OnMouseDuplicate( UINT Flags, CPoint p )
{
	SetMouseHover( p.x, p.y );

	MouseToWorld( p.x, p.y, m_x2, m_y2 );

	Sprite* pParent = TopSprite();
	Sprite* pSprite = SelectedSprite();
	SevenupDoc* pDocument = GetDocument();

	if( pSprite && pDocument )
	{
		float dx = m_x2 - m_x1;
		float dy = m_y2 - m_y1;

		float distance = Math::Sqrt( (dx*dx)+(dy*dy) );

		if( distance >= 8.0f )
		{
			int c = m_SelectionList.GetNumChildren();
			SpriteItem* pSelection = m_SelectionList.Child();
			SpriteItem* pSelectionList;
			SelectRecord* pRecord1;
			SelectRecord* pRecord2;
			Sprite* pNewSprite;
			int i;

			pSelectionList = new SpriteItem[c];

			if( pSelectionList )
			{
				for( i = 0; i < c; i++ )
				{
					pSelectionList[i].m_pSprite = pSelection->m_pSprite;					
					pSelection = pSelection->Next();
				}

				for( i = 0; i < c; i++ )
					DeselectChildren( pSelectionList[i].m_pSprite );

				pSelection = m_SelectionList.Child();
				c = m_SelectionList.GetNumChildren();

				for( i = 0; i < c; i++ )
				{
					pRecord1 = (SelectRecord*) pSelectionList[i].AddRecord( SelectRecord::ClassnameStatic() );
					pRecord2 = (SelectRecord*) pSelection->GetRecord( SelectRecord::ClassnameStatic() );

					pSelectionList[i].m_pSprite = pSelection->m_pSprite;

					if( pRecord1 && pRecord2 )
					{
						pRecord1->m_MousePosX = pRecord2->m_MousePosX;
						pRecord1->m_MousePosY = pRecord2->m_MousePosY;
						pRecord1->m_SpriteState = pRecord2->m_SpriteState;
					}

					pSelection = pSelection->Next();
				}

				DeselectAll();

				for( i = 0; i < c; i++ )
				{
					pSprite = pSelectionList[i].m_pSprite;

					pNewSprite = pDocument->DuplicateSpriteFamily( pSprite );

					if( pNewSprite )
					{
						if( pParent )
							pParent->AddChild( pNewSprite );

						SelectSprite( pNewSprite );

						pSelection = m_SelectionList.Child();

						if( pSelection )
						{
							pRecord1 = (SelectRecord*) pSelection->GetRecord( SelectRecord::ClassnameStatic() );
							pRecord2 = (SelectRecord*) pSelectionList[i].GetRecord( SelectRecord::ClassnameStatic() );

							if( pRecord1 && pRecord2 )
							{
								pRecord1->m_MousePosX = pRecord2->m_MousePosX;
								pRecord1->m_MousePosY = pRecord2->m_MousePosY;
								pRecord1->m_SpriteState = pRecord2->m_SpriteState;
							}
						}
					}
				}

				delete [] pSelectionList;

				SetMouseMode( MOUSEMODE_POSITION );

				OnMouseMove( Flags, p );
			}
		}
	}

	Invalidate();
}


void GameView::OnMouseRotateTextureBegin( void )
{
	Sprite* pSprite = SelectedSprite();

	if( pSprite )
	{
		SetMouseMode( MOUSEMODE_ROTATETEXTURE );

		float OldX = pSprite->m_state.origin.x;
		float OldY = pSprite->m_state.origin.y;

		pSprite->SetOrigin( 0.5f, 0.5f );
		pSprite->GetScreenPosition( m_ox, m_oy );
		pSprite->SetOrigin( OldX, OldY );

		m_x1 = m_x;
		m_y1 = m_y;

		float x, y;

		for( int i = 0; i < 4; i++ )
		{
			x = pSprite->m_state.texcoords[i].x - 0.5f;
			y = pSprite->m_state.texcoords[i].y - 0.5f;

			m_Rot[i] = Math::ArcTan2( y, x );
		}
	}

	return;
}
//=============================================================================
// GameView::OnMouseRotateTexture
//=============================================================================

void GameView::OnMouseRotateTexture( UINT Flags, CPoint p )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		if( Flags & MK_LBUTTON )
		{
			MouseToWorld( p, m_x2, m_y2 );

			float Dx = m_x2 - m_ox;
			float Dy = m_y2 - m_oy;

			float T = Math::ArcTan2( Dy, Dx );

			float x, y;
			float R, N;
			int i;

			for( i = 0; i < 4; i++ )
			{
				x = pSprite->m_state.texcoords[i].x - 0.5f;
				y = pSprite->m_state.texcoords[i].y - 0.5f;

				N = m_Rot[i];
				R = Math::Sqrt( x*x + y*y );

				x = Math::Cos(N-T) * R;
				y = Math::Sin(N-T) * R;

				pSprite->SetTextureCoordinates( i, x+0.5f, y+0.5f );
			}

			if( m_TextureWnd.m_hWnd )
			{
				for( i = 0; i < 4; i++ )
					m_TextureWnd.SetTextureCoordinates( i, pSprite->m_state.texcoords[i].x, pSprite->m_state.texcoords[i].y );

				m_TextureWnd.Invalidate( FALSE );
			}

			Invalidate();
		}
	}

	return;
}

//=============================================================================
// GameView::OnMouseModeBegin
//=============================================================================

void GameView::OnMouseModeBegin( MOUSEMODE MouseMode )
{
}

//=============================================================================
// GameView::OnMouseModeEnd
//=============================================================================

void GameView::OnMouseModeEnd( MOUSEMODE MouseMode )
{
}

//=============================================================================
// GameView::OnMouseToolBegin
//=============================================================================

void GameView::OnMouseToolBegin( MOUSETOOL MouseTool )
{
	switch( MouseTool )
	{
	case MOUSETOOL_BUTTONORDER:
		DeselectAll();
		m_PrevButtonId = 0;
		m_NextButtonId = 0;
		break;
	}

	return;
}

//=============================================================================
// GameView::OnMouseToolEnd
//=============================================================================

void GameView::OnMouseToolEnd( MOUSETOOL MouseTool )
{
	switch( MouseTool )
	{
	case MOUSETOOL_STAMP:
		{
			SpriteItem* pCopyItem;
			Sprite* pSprite;

			pCopyItem = m_CopyList.Child();

			while( pCopyItem )
			{
				pSprite = pCopyItem->m_pSprite;

				if( pSprite )
					pSprite->ReleaseHandlesRecursively();

				pCopyItem = pCopyItem->Next();
			}
		}
		break;
	}

	return;
}


//=============================================================================
// GameView::OnChar
//=============================================================================

void GameView::OnChar( UINT Char, UINT RepeatCount, UINT Flags )
{
	SevenupDoc *pDocument = GetDocument();
	Sprite* pSelSprite = SelectedSprite();

	if( pDocument )
	{
		switch( Char )
		{
		case '1':
			m_ObjectType = HUDOBJECT_SPRITE;
			break;
		case '2':
			m_ObjectType = HUDOBJECT_TEXT;
			break;
		case '3':
		case '[':
			PickPrevSprite();
			break;
		case ']':
			PickNextSprite();
			break;
		case '{':
			{
			}
			break;
		case '}':
			{
			}
			break;
		case 't':
		case 'T':
			{
				if( pSelSprite )
					pSelSprite->SetTextureSet( pDocument->GetTextureHandle(), pDocument->GetTextureIndex() );

				Invalidate();
			}
			break;
		case ',':
		case '<':
			{
				if( pSelSprite )
					pSelSprite->Rotate( -5.0f );

				Invalidate();
			}
			break;
		case '.':
		case '>':
			{
				if( pSelSprite )
					pSelSprite->Rotate( 5.0f );

				Invalidate();
			}
			break;
		
		case '+':
		case '=':
			ZoomIn();
			break;		
		case '-':
		case '_':
			ZoomOut();
			break;
		case '`':
		case '~':
			{
				if( m_ViewMode & VIEWMODE_SHOWNAMES )
					m_ViewMode &= ~VIEWMODE_SHOWNAMES;
				else
				if( m_ViewMode & VIEWMODE_SHOWBOXES )
					m_ViewMode &= ~VIEWMODE_SHOWBOXES;
				else
					m_ViewMode = VIEWMODE_SHOWNAMES|VIEWMODE_SHOWBOXES;

				Invalidate();
			}
			break;
		}
	}

	return CView::OnChar( Char, RepeatCount, Flags );
}

//=============================================================================
// GameView::OnKeyDown
//=============================================================================

void GameView::OnKeyDown( UINT Char, UINT RepeatCount, UINT Flags )
{
	Sprite* pSelSprite = SelectedSprite();

	switch( Char )
	{
	case VK_MENU:
		OnAltKey( TRUE );
		break;

	case VK_RETURN:
		OnEnterKey( TRUE );
		break;

	case VK_DELETE:
		OnDeleteKey( TRUE );
		break;

	case VK_SHIFT:
		OnShiftKey( TRUE );
		break;

	case VK_CONTROL:
		OnControlKey( TRUE );
		break;

	case VK_ESCAPE:
		if( !m_EscapeKeyHandled )
			OnEscapeKey( TRUE );
		break;

	case VK_PRIOR:
		break;

	case VK_UP:
		MoveSelection( 0.0f,-1.0f );
		break;
	case VK_DOWN:
		MoveSelection( 0.0f, 1.0f );
		break;
	case VK_LEFT:
		MoveSelection(-1.0f, 0.0f );
		break;
	case VK_RIGHT:
		MoveSelection( 1.0f, 0.0f );
		break;
	}

	return;
}

//=============================================================================
// GameView::OnKeyUp
//=============================================================================

void GameView::OnKeyUp( UINT Char, UINT RepeatCount, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( FALSE );
		break;

	case VK_RETURN:
		OnEnterKey( FALSE );
		break;
		
	case VK_DELETE:
		OnDeleteKey( FALSE );
		break;

	case VK_SHIFT:
		OnShiftKey( FALSE );
		break;
		
	case VK_CONTROL:
		OnControlKey( FALSE );
		break;

	case VK_ESCAPE:
		OnEscapeKey( FALSE );
		break;
	}

	return;
}

//=============================================================================
// GameView::OnSysKeyDown
//=============================================================================

void GameView::OnSysKeyDown( UINT Char, UINT RepeatCount, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( TRUE );
		break;

	case VK_SHIFT:
		OnShiftKey( TRUE );
		break;

	default:
		CView::OnSysKeyDown( Char, RepeatCount, Flags );
		break;
	}

	return;
}

//=============================================================================
// GameView::OnSysKeyUp
//=============================================================================

void GameView::OnSysKeyUp( UINT Char, UINT RepeatCount, UINT Flags )
{
	switch( Char )
	{
	case VK_MENU:
		OnAltKey( FALSE );
		break;

	case VK_SHIFT:
		OnShiftKey( FALSE );
		break;

	default:
		CView::OnSysKeyUp( Char, RepeatCount, Flags );
		break;
	}

	return;
}

//=============================================================================
// GameView::OnCreate
//=============================================================================

int GameView::OnCreate( LPCREATESTRUCT pcs )
{
	if( -1 == CView::OnCreate( pcs ) )
		return -1;

	SetClassLong( m_hWnd, GCL_HCURSOR, 0 );

	SCROLLINFO si;
	
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
	si.nMin = 0;
	si.nMax = 3000;
	si.nPage = 1000;
	si.nPos = 1000;
	si.nTrackPos = 0;

	SetScrollInfo( SB_VERT, &si, TRUE );
	SetScrollInfo( SB_HORZ, &si, TRUE );

	Init();

	return 0;
}

//=============================================================================
// GameView::OnDestroy
//=============================================================================

void GameView::OnDestroy()
{
}

//=============================================================================
// GameView::OnMouseActivate
//=============================================================================

int GameView::OnMouseActivate( CWnd *pDesktopWnd, UINT HitTest, UINT message )
{
	int Result = CView::OnMouseActivate( pDesktopWnd, HitTest, message );

	return Result;
}

//=============================================================================
// GameView::OnMouseMove
//=============================================================================

void GameView::OnMouseMove( UINT Flags, CPoint p )
{
	CFrameWnd *pFrameWnd = GetParentFrame();

	if( pFrameWnd )
	{
		pFrameWnd->SetActiveView( this );

		if( m_PropertyWnd.m_hWnd == NULL )
			pFrameWnd->SetFocus();
	}

	MouseToWorld( p.x, p.y, m_x, m_y );

	if( !SelectedSprite() )
		SetMouseMode( MOUSEMODE_HOVER );

	switch( m_MouseMode )
	{
	case MOUSEMODE_HOVER:
		OnMouseHover( Flags, p );
		break;
	case MOUSEMODE_POSITION:
		OnMousePosition( Flags, p );
		break;
	case MOUSEMODE_SIZE:
		OnMouseSize( Flags, p );
		break;
	case MOUSEMODE_SCALE:
		break;
	case MOUSEMODE_ROTATE:
		OnMouseRotate( Flags, p );
		break;
	case MOUSEMODE_DUPLICATE:
		OnMouseDuplicate( Flags, p );
		break;
	case MOUSEMODE_ROTATETEXTURE:
		OnMouseRotateTexture( Flags, p );
		break;
	}

	UpdateOemCursor();
}

//=============================================================================
// GameView::OnMouseWheel
//=============================================================================

BOOL GameView::OnMouseWheel( UINT Flags, short Delta, CPoint p )
{
	if( Delta < 0 )
		PickNextSprite();
	else
		PickPrevSprite();

	return CView::OnMouseWheel( Flags, Delta, p );
}

//=============================================================================
// GameView::OnLButtonDown
//=============================================================================

void GameView::OnLButtonDown( UINT Flags, CPoint p )
{
	CView::OnLButtonDown( Flags, p );

	SevenupDoc *pDocument = GetDocument();

	if( pDocument )
	{
		if( m_MouseTool == MOUSETOOL_NEW )
		{
			if( m_pNewSprite )
			{
				MouseToWorld( p.x, p.y, m_x1, m_y1 );

				DeselectAll();

				SelectSprite( m_pNewSprite );

				SetMouseMode( MOUSEMODE_HOVER );

				pDocument->Insert( m_pNewSprite, TopSprite() );

				m_pNewSprite->SetScreenPosition( m_x1, m_y1 );
				m_pNewSprite->SetPriority( 0 );
				m_pNewSprite = 0;

				Invalidate();
			}
		}
		else
		if( m_MouseTool == MOUSETOOL_DRAG )
		{
			if( m_MouseMode == MOUSEMODE_HOVER )
			{
				if( !HoveredSprite() )
				{
					ClearPickList();

					if( !(Flags & MK_CONTROL) && SelectedSprite() )
						DeselectAll();

					m_x1 = m_x2 = m_x;
					m_y1 = m_y2 = m_y;

					SetMouseTool( MOUSETOOL_SELECT );

					Invalidate();
				}
				else
				{
					HoverListToPickList();
				
					Sprite* pSelSprite = SelectedSprite();

					int SizeHandles = 0;

					if( m_AllowSizing )
					{
						if( IsHoverSelected() && m_SelectionList.GetNumChildren() == 1 )
							SizeHandles = GetSizeHandles( p.x, p.y );
					}

					BOOL CancelDrag = FALSE;

					if( !IsSelected( HoveredSprite() ) )
					{
						if( Flags & MK_CONTROL )
							CancelDrag = TRUE;

						if( !IsHoverSelected() )
						{
							if( !CancelDrag )
								DeselectAll();

							SelectSprite( HoveredSprite() );
						}
						else
						if( CancelDrag )
							SelectSprite( HoveredSprite() );
					}
					else
					if( Flags & MK_CONTROL )
					{
						DeselectSprite( HoveredSprite() );
						CancelDrag = TRUE;
					}
					else
					{
						DeselectSprite( HoveredSprite() );
						SelectSprite( HoveredSprite() );
					}
					
					if( !CancelDrag )
					{
						if( m_AltKey )
							OnMouseDuplicateBegin( Flags, p );
						else
						{
							if( SizeHandles != 0 )
								OnMouseSizeBegin( SizeHandles, p );
							else
								OnMousePositionBegin( Flags, p );
						}
					}

					Invalidate();
				}
			}
			else
			if( m_MouseMode == MOUSEMODE_ROTATETEXTURE )
			{
				OnMouseRotateTextureBegin();
			}
		}
		else
		if( m_MouseTool == MOUSETOOL_GROUP )
		{
			Sprite* pSprite = SelectedSprite();
			Sprite* pChild = HoveredSprite();

			if( pSprite == pChild )
			{
				OnButtonCreateFamily();

				DeselectAll();

				SetMouseMode( MOUSEMODE_HOVER );
				SetMouseTool( MOUSETOOL_DRAG );
			}
			else
			if( pChild )
			{
				DeselectSprite( pSprite );

				if( !IsSelected( pChild ) )
					SelectSprite( pChild );
				else
					DeselectSprite( pChild );

				SelectSprite( pSprite );

				Invalidate();
			}
		}
		else
		if( m_MouseTool == MOUSETOOL_ZOOM )
		{
			ZoomIn();

			RECT rc;
			GetClientRect( &rc );
			ClientToScreen( &rc );

			int x = rc.left + (rc.right - rc.left) / 2;
			int y = rc.top + (rc.bottom - rc.top) / 2;

			SetCursorPos( x, y );
			
			Invalidate();
		}
		else
		if( m_MouseTool == MOUSETOOL_STAMP )
		{
			SevenupDoc* pDocument = GetDocument();
			SpriteItem* pCopyItem = m_CopyList.Child();
			Sprite* pSprite;

			if( pDocument )
			{
				while( pCopyItem )
				{
					pSprite = pCopyItem->m_pSprite;

					if( pSprite )
						pDocument->DuplicateSpriteFamily( pSprite );

					pCopyItem = pCopyItem->Next();
				}
			}
			Invalidate();
		}
		else
		if( m_MouseTool == MOUSETOOL_BUTTONORDER )
		{
			/*
			Screen* pScreen;
			Sprite* pSprite;
			Sprite* pPrevSprite;
			Sprite* pSprite1;
			Sprite* pSprite2;
			Button* pButton;
			ProjectItem* pProjectItem;
			int ButtonId;

			pScreen = TopScreen();
			pSprite = HoveredSprite();
			pProjectItem = Project.GetProjectItem( pSprite );

			if( pScreen && pSprite && pProjectItem )
			{
				ButtonId = pSprite->ObjectId();

				if( m_PrevButtonId == 0 )
					pScreen->m_Param.InitialButton = ButtonId;
				else
				{
					pSprite1 = pScreen->GetSprite( pScreen->m_Param.InitialButton );
					pSprite2 = 0;

					while( pSprite1 )
					{
						pButton = (Button*) pSprite1->GetInterface( Button::ClassnameStatic() );

						if( pButton )
							pSprite2 = pScreen->GetSprite( pButton->m_Param.NextButton );

						pSprite1 = pSprite2;
						pSprite2 = 0;
					}

					pPrevSprite = pScreen->GetSprite( m_PrevButtonId );

					if( pPrevSprite )
					{
						Button* pButton = (Button*) pPrevSprite->GetInterface( Button::ClassnameStatic() );

						if( pButton )
							pButton->m_Param.NextButton = ButtonId;

						pButton = (Button*) pSprite->GetInterface( Button::ClassnameStatic() );

						if( pButton )
							pButton->m_Param.PrevButton = m_PrevButtonId;
					}
				}

				m_PrevButtonId = ButtonId;
			}
			*/
		}
	}

	return;
}

//=============================================================================
// GameView::OnLButtonDblClk
//=============================================================================

void GameView::OnLButtonDblClk( UINT Flags, CPoint p )
{
	OnLButtonDown( Flags, p );
}

//=============================================================================
// GameView::OnLButtonUp
//=============================================================================

void GameView::OnLButtonUp( UINT Flags, CPoint p )
{
	CView::OnLButtonUp( Flags, p );

	OnMouseMove( Flags, p );

	if( m_MouseTool == MOUSETOOL_SELECT )
		SetMouseTool( MOUSETOOL_DRAG );

	if( m_PositionLock != 0 )
		m_PositionLock = 1;

	return;
}

//=============================================================================
// GameView::OnRButtonDown
//=============================================================================

void GameView::OnRButtonDown(	UINT Flags, CPoint p )
{
	CView::OnRButtonDown( Flags, p );

	MouseToWorld( p.x, p.y, m_x1, m_y1 );

	if( m_MouseTool == MOUSETOOL_ZOOM )
	{
		ZoomOut();

		RECT rc;
		GetClientRect( &rc );
		ClientToScreen( &rc );

		int x = rc.left + (rc.right - rc.left) / 2;
		int y = rc.top + (rc.bottom - rc.top) / 2;

		SetCursorPos( x, y );

		Invalidate();
	}
	else
	{
		BOOL HoverOK = IsHoverSelected();

		if( !HoverOK )
		{
			if( !(Flags & MK_CONTROL) )
				DeselectAll();

			Sprite* pSprite = HoveredSprite();

			if( pSprite == 0 && (Flags & MK_CONTROL) )
			{
				SpriteItem* pSpriteList = m_LockedList.Child();

				if( pSpriteList )
					pSprite = pSpriteList->m_pSprite;
			}

			SelectSprite( pSprite );
		}
		else
		if( IsSelected( HoveredSprite() ) )
		{
			DeselectSprite( HoveredSprite() );
			SelectSprite( HoveredSprite() );
		}

		SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	}

	return;
}

//=============================================================================
// GameView::OnRButtonDblClk
//=============================================================================

void GameView::OnRButtonDblClk( UINT Flags, CPoint p )
{
	OnRButtonDown( Flags, p );
}

//=============================================================================
// GameView::OnRButtonUp
//=============================================================================

void GameView::OnRButtonUp( UINT Flags, CPoint p )
{
	return;
}

//=============================================================================
// GameView::OnContextMenu
//=============================================================================

void GameView::OnContextMenu( CWnd *pWnd, CPoint p )
{
	CView::OnContextMenu( pWnd, p );

	if( SelectedSprite() )
	{
		HWND hWnd = m_hWnd;

		if( AfxGetMainWnd() )
			hWnd = AfxGetMainWnd()->m_hWnd;

		TrackPopupMenuEx( GuiGetContextMenu(), TPM_LEFTALIGN|TPM_TOPALIGN, p.x, p.y, hWnd, NULL );

		SetMouseMode( MOUSEMODE_HOVER );
	}

	return;
}

//=============================================================================
// GameView::OnHScroll
//=============================================================================

void GameView::OnHScroll( UINT Code, UINT Pos, CScrollBar *pScrollBar )
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);

	GetScrollInfo( SB_HORZ, &si );

	int OldPos = si.nPos;

	Win32_ScrollBar_OnUpdate( Code, 100, si );

	si.fMask = SIF_POS;
	SetScrollInfo( SB_HORZ, &si, TRUE );
	GetScrollInfo( SB_HORZ, &si, SIF_POS );

	int NewPos = si.nPos;

	float n = Sprite::SimulatedScreenWidth();
	float Width = DocumentGetScreenWidth();

	m_ViewPortX = Width * (NewPos / 1000.0f - 1.0f);

	Invalidate();
}

//=============================================================================
// GameView::OnVScroll
//=============================================================================

void GameView::OnVScroll( UINT Code, UINT Pos, CScrollBar *pScrollBar )
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);

	GetScrollInfo( SB_VERT, &si );

	int OldPos = si.nPos;

	Win32_ScrollBar_OnUpdate( Code, 100, si );

	si.fMask = SIF_POS;
	SetScrollInfo( SB_VERT, &si, TRUE );
	GetScrollInfo( SB_VERT, &si, SIF_POS );

	int NewPos = si.nPos;

	float n = Sprite::SimulatedScreenHeight();
	float Height = DocumentGetScreenHeight();

	m_ViewPortY = Height * (NewPos / 1000.0f - 1.0f);

	Invalidate();
}


//=============================================================================
// GameView::OnTimer
//=============================================================================

void GameView::OnTimer( UINT_PTR TimerEvent )
{
	if( TimerEvent == TIMEREVENT_SPRITECHANGES )
	{
		if( m_PropertyWnd.m_hWnd )
		{
			Sprite* pSprite = (Sprite*) m_PropertyWnd.GetUserParam();

			if( pSprite != SelectedSprite() )
				GuiShowProperties( SelectedSprite() );
		}

		KillTimer( TIMEREVENT_SPRITECHANGES );
	}

	return;
}

//=============================================================================
// GameView::OnSetFocus
//=============================================================================

void GameView::OnSetFocus( CWnd* pOldWnd )
{
	CView::OnSetFocus( pOldWnd );
}

//=============================================================================
// GameView::OnKillFocus
//=============================================================================

void GameView::OnKillFocus( CWnd* pNewWnd )
{
	CView::OnKillFocus( pNewWnd );
}

//=============================================================================
// GameView::OnEraseBkgnd
//=============================================================================

BOOL GameView::OnEraseBkgnd( CDC *pdc )
{
	return TRUE;
}

//=============================================================================
// GameView::OnEnterKey
//=============================================================================

void GameView::OnEnterKey( BOOL KeyDown )
{
	if( KeyDown )
	{
		if( m_MouseTool == MOUSETOOL_GROUP )
		{
			OnButtonCreateFamily();

			DeselectAll();

			SetMouseMode( MOUSEMODE_HOVER );
			SetMouseTool( MOUSETOOL_DRAG );
		}
		if( m_MouseMode == MOUSEMODE_ROTATETEXTURE )
		{
			SetMouseMode( MOUSEMODE_HOVER );
			Invalidate();
		}
		else
		{
			if( SelectedSprite() )
				GuiShowProperties( SelectedSprite() );
		}
	}
	
	return;
}

//=============================================================================
// GameView::OnDeleteKey
//=============================================================================

void GameView::OnDeleteKey( BOOL KeyDown )
{
	if( KeyDown )
		OnEditDelete();

	return;
}

//=============================================================================
// GameView::OnShiftKey
//=============================================================================

void GameView::OnShiftKey( BOOL KeyDown )
{
	m_ShiftKey = KeyDown;

	if( KeyDown )
	{
		if( m_PositionLock == 0 )
			m_PositionLock = 1;
	}
	else
	{
		m_PositionLock = 0;
	}

	Invalidate();
}

//=============================================================================
// GameView::OnControlKey
//=============================================================================

void GameView::OnControlKey( BOOL KeyDown )
{
	m_ControlKey = KeyDown;

	Invalidate();
}

//=============================================================================
// GameView::OnAltKey
//=============================================================================

void GameView::OnAltKey( BOOL KeyDown )
{
	m_AltKey = KeyDown;

	Invalidate();
}

//=============================================================================
// GameView::OnEscapeKey
//=============================================================================

void GameView::OnEscapeKey( BOOL KeyDown )
{
	if( KeyDown )
	{
		if( m_PropertyWnd.m_hWnd )
		{
			GuiHideProperties();
		}
		else
		{
			switch( m_MouseTool )
			{
			case MOUSETOOL_DRAG:
				if( m_MouseMode == MOUSEMODE_POSITION )
				{
					SpriteItem* pSelection = m_SelectionList.Child();
					SelectRecord* pRecord;
					Sprite* pSprite;

					while( pSelection )
					{
						pSprite = pSelection->m_pSprite;

						if( pSprite && !IsSelected( pSprite->Parent() ) )
						{
							pRecord = (SelectRecord*) pSelection->GetRecord( SelectRecord::ClassnameStatic() );

							if( pRecord )
								pSprite->SetPosition( pRecord->m_SpriteState.position.x,
															 pRecord->m_SpriteState.position.y );
						//	pSprite->SetScreenPosition( pSelection->m_position_x, pSelection->m_position_y );
						}

						pSelection = pSelection->Next();
					}
				}

				if( m_MouseMode == MOUSEMODE_HOVER )
					DeselectAll();

				SetMouseMode( MOUSEMODE_HOVER );
				SetMouseTool( MOUSETOOL_DRAG );
				break;

			case MOUSETOOL_SELECT:
				SetMouseMode( MOUSEMODE_HOVER );
				SetMouseTool( MOUSETOOL_DRAG );
				break;
			case MOUSETOOL_ZOOM:
				if( m_SelectionList.GetNumChildren() > 0 )
					DeselectAll();
				else
				{
					OnButtonCenterView();
					SetMouseMode( MOUSEMODE_HOVER );
					SetMouseTool( MOUSETOOL_DRAG );
				}
				break;
			case MOUSETOOL_NEW:
				SetMouseMode( MOUSEMODE_HOVER );
				SetMouseTool( MOUSETOOL_DRAG );
				break;
			case MOUSETOOL_STAMP:
				SetMouseMode( MOUSEMODE_HOVER );
				SetMouseTool( MOUSETOOL_DRAG );
				break;
			default:
				DeselectAll();
				break;
			}

			Invalidate();
		}
	}

	return;
}

//=============================================================================
// GameView::OnEditUndo
//=============================================================================

void GameView::OnEditUndo( void )
{
}

//=============================================================================
// GameView::OnEditRedo
//=============================================================================

void GameView::OnEditRedo( void )
{
}

//=============================================================================
// GameView::OnEditCut
//=============================================================================

void GameView::OnEditCut( void )
{
	CutSelection();
}

//=============================================================================
// GameView::OnEditCopy
//=============================================================================

void GameView::OnEditCopy( void )
{
	CopySelection();
}

//=============================================================================
// GameView::OnEditPaste
//=============================================================================

void GameView::OnEditPaste( void )
{
	PasteSelection( TRUE );
}

//=============================================================================
// GameView::OnEditDelete
//=============================================================================

void GameView::OnEditDelete( void )
{
	DeleteSelection();
}

//=============================================================================
// GameView::OnEditSelectAll
//=============================================================================

void GameView::OnEditSelectAll( void )
{
}

//=============================================================================
// GameView::OnEditFind
//=============================================================================

void GameView::OnEditFind( void )
{
}

//=============================================================================
// GameView::OnEditFindReplace
//=============================================================================

void GameView::OnEditFindReplace( void )
{
	CWnd* pWnd = AfxGetMainWnd();

	if( pWnd )
	{
		RECT rc;
		pWnd->GetWindowRect( &rc );

		if( !m_FindDialog.m_hWnd )
			m_FindDialog.Create( IDD_FINDDIALOG, AfxGetMainWnd() );

		Win32_CenterWindow( m_FindDialog.m_hWnd, rc );

		m_FindDialog.ShowWindow( SW_SHOW );
		m_FindDialog.UpdateWindow();
	}

	return;
}

//=============================================================================
// GameView::OnNewScreen
//=============================================================================

void GameView::OnNewScreen( void )
{
	NewScreenDialog Dialog;

	INT_PTR Result = Dialog.DoModal();

	if( Result == IDOK )
	{
		Sprite* pSprite = CreateSprite( HUDOBJECT_SPRITE );

		if( pSprite )
		{
			float ScreenWidth = Sprite::SimulatedScreenWidth();
			float ScreenHeight = Sprite::SimulatedScreenHeight();

			pSprite->AddInterface( "Screen", 0 );
			pSprite->ScreenPosition( 0.5f, 0.5f );
			pSprite->SetSize( ScreenWidth, ScreenHeight );
			pSprite->SetTextureSet( INVALID_HANDLE, 0 );

			Project.SetName( pSprite, Dialog.m_ScreenName.get() );
			Project.Lock( pSprite );

			SevenupDoc* pDocument = GetDocument();

			if( pDocument )
			{
				pDocument->Insert( pSprite, 0 );
				pDocument->SyncProjectViews();
			}

			SetTopSprite( pSprite );
		}
	}

	return;
}

//=============================================================================
// GameView::OnNewSprite
//=============================================================================

void GameView::OnNewSprite( void )
{
}

//=============================================================================
// GameView::OnSpriteLock
//=============================================================================

void GameView::OnSpriteLock( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	int c = m_SelectionList.GetNumChildren();

	Sprite** ppSprites = new Sprite*[c];

	if( !ppSprites )
		return;

	for( c = 0; pSelection; c++ )
	{
		pSprite = pSelection->m_pSprite;
		ppSprites[c] = pSprite;

		if( Project.Locked( pSprite ) )
			Project.Unlock( pSprite );
		else
			Project.Lock( pSprite );

		pSelection = pSelection->Next();
	}

	for( int i = 0; i < c; i++ )
	{
		pSprite = ppSprites[i];

		if( Project.Locked( pSprite ) )
			DeselectSprite( pSprite );
	}

	delete [] ppSprites;
}

//=============================================================================
// GameView::OnSpriteDelete
//=============================================================================

void GameView::OnSpriteDelete( void )
{
	DeleteSelection();
}

//=============================================================================
// GameView::OnSpriteProperties
//=============================================================================

void GameView::OnSpriteProperties( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
		GuiShowProperties( pSprite );

	return;
}

//=============================================================================
// GameView::OnSpriteSaveAs
//=============================================================================

void GameView::OnSpriteSaveAs( void )
{
	SevenupDoc* pDocument = GetDocument();

	if( pDocument )
	{
		CString FileName;
		BOOL SaveOK = App.AskDocSave( FileName );

		if( SaveOK )
		{
			Sprite* pSprite = SelectedSprite();

			if( pSprite )
			{
				float x1, y1;
				float x2, y2;
				int bmWidth;
				int bmHeight;
				int bmMax;

				pSprite->GetFamilyRect( x1, y1, x2, y2 );

				bmWidth = (int) (x2 - x1);
				bmHeight = (int) (y2 - y1);
				
				if( bmWidth > bmHeight )
					bmMax = bmWidth;
				else
					bmMax = bmHeight;

				bmWidth = (bmWidth * 128) / bmMax;
				bmHeight = (bmHeight * 128) / bmMax;

				HBITMAP hbm = pDocument->CreateThumbnail( SelectedSprite(), bmWidth, bmHeight, TRUE );

			//	SaveDIB( FileName, hbm, 0 );

				pDocument->SaveDocument( FileName, SelectedSprite() );
			}
		}
	}

	return;
}

//=============================================================================
// GameView::OnSpriteAddFiles
//=============================================================================

void GameView::OnSpriteAddFiles( void )
{
	CString FileName;
	BOOL FileOK = App.AskDocOpen( FileName );

	if( FileOK )
	{
		SevenupDoc* pDocument = GetDocument();
		Sprite* pSceneList = SelectedSprite();
		Sprite* pLastChild;
		Sprite* pNewChild;

		if( pDocument && pSceneList )
		{
			pLastChild = (Sprite*) pSceneList->GetLastChild();

			FileOK = pDocument->OpenDocument( FileName, SelectedSprite() );

			if( !FileOK )
				MessageBox( "Encounted a problem with the file..." );
			else
			{
				if( pLastChild )
					pNewChild = pLastChild->Next();
				else
					pNewChild = pSceneList->Child();

				while( pNewChild )
				{
					Project.Unlock( pNewChild, TRUE );

					pNewChild = pNewChild->Next();
				}
			}
		}
	}

	return;
}

//=============================================================================
// GameView::OnTransformPosition
//=============================================================================

void GameView::OnTransformPosition( void )
{
	SetMouseTool( MOUSETOOL_DRAG );
	SetMouseMode( MOUSEMODE_POSITION );
}

//=============================================================================
// GameView::OnTransformSize
//=============================================================================

void GameView::OnTransformSize( void )
{
	SetMouseTool( MOUSETOOL_DRAG );
	SetMouseMode( MOUSEMODE_SIZE );
}

//=============================================================================
// GameView::OnTransformScale
//=============================================================================

void GameView::OnTransformScale( void )
{
	SetMouseTool( MOUSETOOL_DRAG );
	SetMouseMode( MOUSEMODE_SCALE );
}

//=============================================================================
// GameView::OnTransformFlipHorizontal
//=============================================================================

void GameView::OnTransformFlipHorizontal( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		pSprite->Scale( -1.0f, 1.0f );

		float Ox = pSprite->m_state.origin.x;
		float Oy = pSprite->m_state.origin.y;

		pSprite->SetOrigin( 1.0f - Ox, Oy );
	}

	return;

}

//=============================================================================
// GameView::OnTransformFlipVertical
//=============================================================================

void GameView::OnTransformFlipVertical( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		pSprite->Scale( 1.0f, -1.0f );

		float Ox = pSprite->m_state.origin.x;
		float Oy = pSprite->m_state.origin.y;

		pSprite->SetOrigin( Ox, 1.0f - Oy );
	}
}

//=============================================================================
// GameView::OnTransformRotateCW
//=============================================================================

void GameView::OnTransformRotateCW( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
		pSprite->Rotate( 90.0f );

	return;
}

//=============================================================================
// GameView::OnTransformRotateCCW
//=============================================================================

void GameView::OnTransformRotateCCW( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
		pSprite->Rotate( -90.0f );

	return;
}

//=============================================================================
// GameView::OnTransformRotate
//=============================================================================

void GameView::OnTransformRotate( void )
{
	SetMouseTool( MOUSETOOL_DRAG );
	SetMouseMode( MOUSEMODE_ROTATE );
}

//=============================================================================
// GameView::OnTextureApply
//=============================================================================

void GameView::OnTextureApply( void )
{
	SevenupDoc *pDocument = GetDocument();

	if( pDocument )
	{
		SpriteItem* pSelection = m_SelectionList.Child();
		Sprite* pSprite;

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
				pSprite->SetTextureSet( pDocument->GetTextureHandle(), pDocument->GetTextureIndex() );

			pSelection = pSelection->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::OnTextureRemove
//=============================================================================

void GameView::OnTextureRemove( void )
{
	SevenupDoc *pDocument = GetDocument();

	if( pDocument )
	{
		SpriteItem* pSelection = m_SelectionList.Child();
		Sprite* pSprite;

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
				pSprite->SetTextureSet( INVALID_HANDLE, 0 );

			pSelection = pSelection->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::OnTextureSelect
//=============================================================================

void GameView::OnTextureSelect( void )
{
	SevenupDoc *pDocument = GetDocument();

	if( pDocument )
	{
		Sprite *pSprite = SelectedSprite();

		if( pSprite )
			pDocument->SetTexture( pSprite->GetTextureSet(), pSprite->GetTexture() );
	}

	return;
}

//=============================================================================
// GameView::OnTextureCustom
//=============================================================================

void GameView::OnTextureCustomize( void )
{
	if( !m_TextureWnd.m_hWnd )
	{
		DWORD dwStyle =
			WS_CAPTION|WS_CHILD|WS_CLIPCHILDREN|WS_POPUP|WS_SYSMENU|WS_THICKFRAME;

		RECT rc = GdiMakeRect( 0, 0, 256, 256 );

		m_TextureWnd.CreateEx( 0, 0, "Custom Texture", dwStyle, rc, this, 0 );
	}

	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		int Texture = pSprite->GetTexture();
		t_Handle Handle = pSprite->GetTextureSet();

		m_TextureWnd.CreateTexture( TextureMgr__GetTextureDirect3D( Handle, Texture ) );

		for( int i = 0; i < 4; i++ )
			m_TextureWnd.SetTextureCoordinates( i, pSprite->m_state.texcoords[i].x, pSprite->m_state.texcoords[i].y );

		m_TextureWnd.Invalidate( FALSE );
	}

	m_TextureWnd.ShowWindow( SW_SHOW );
	m_TextureWnd.UpdateWindow();

	return;
}

//=============================================================================
// GameView::OnTextureFlipHorizonal
//=============================================================================

void GameView::OnTextureFlipHorizontal( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			float x0, y0;
			float x1, y1;
			float x2, y2;
			float x3, y3;

			x0 = pSprite->m_state.texcoords[0].x;
			y0 = pSprite->m_state.texcoords[0].y;
			x1 = pSprite->m_state.texcoords[1].x;
			y1 = pSprite->m_state.texcoords[1].y;
			x2 = pSprite->m_state.texcoords[2].x;
			y2 = pSprite->m_state.texcoords[2].y;
			x3 = pSprite->m_state.texcoords[3].x;
			y3 = pSprite->m_state.texcoords[3].y;

			pSprite->SetTextureCoordinates( 0, x1, y1 );
			pSprite->SetTextureCoordinates( 1, x0, y0 );
			pSprite->SetTextureCoordinates( 2, x3, y3 );
			pSprite->SetTextureCoordinates( 3, x2, y2 );
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnTextureFlipVertical
//=============================================================================

void GameView::OnTextureFlipVertical( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			float x0, y0;
			float x1, y1;
			float x2, y2;
			float x3, y3;

			x0 = pSprite->m_state.texcoords[0].x;
			y0 = pSprite->m_state.texcoords[0].y;
			x1 = pSprite->m_state.texcoords[1].x;
			y1 = pSprite->m_state.texcoords[1].y;
			x2 = pSprite->m_state.texcoords[2].x;
			y2 = pSprite->m_state.texcoords[2].y;
			x3 = pSprite->m_state.texcoords[3].x;
			y3 = pSprite->m_state.texcoords[3].y;

			pSprite->SetTextureCoordinates( 0, x3, y3 );
			pSprite->SetTextureCoordinates( 1, x2, y2 );
			pSprite->SetTextureCoordinates( 2, x1, y1 );
			pSprite->SetTextureCoordinates( 3, x0, y0 );
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnTextureRotateCW
//=============================================================================

void GameView::OnTextureRotateCW( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			float x0, y0;
			float x1, y1;
			float x2, y2;
			float x3, y3;

			x0 = pSprite->m_state.texcoords[0].x;
			y0 = pSprite->m_state.texcoords[0].y;
			x1 = pSprite->m_state.texcoords[1].x;
			y1 = pSprite->m_state.texcoords[1].y;
			x2 = pSprite->m_state.texcoords[2].x;
			y2 = pSprite->m_state.texcoords[2].y;
			x3 = pSprite->m_state.texcoords[3].x;
			y3 = pSprite->m_state.texcoords[3].y;

			pSprite->SetTextureCoordinates( 0, x3, y3 );
			pSprite->SetTextureCoordinates( 1, x0, y0 );
			pSprite->SetTextureCoordinates( 2, x1, y1 );
			pSprite->SetTextureCoordinates( 3, x2, y2 );
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnTextureRotateCCW
//=============================================================================

void GameView::OnTextureRotateCCW( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			float x0, y0;
			float x1, y1;
			float x2, y2;
			float x3, y3;

			x0 = pSprite->m_state.texcoords[0].x;
			y0 = pSprite->m_state.texcoords[0].y;
			x1 = pSprite->m_state.texcoords[1].x;
			y1 = pSprite->m_state.texcoords[1].y;
			x2 = pSprite->m_state.texcoords[2].x;
			y2 = pSprite->m_state.texcoords[2].y;
			x3 = pSprite->m_state.texcoords[3].x;
			y3 = pSprite->m_state.texcoords[3].y;

			pSprite->SetTextureCoordinates( 0, x1, y1 );
			pSprite->SetTextureCoordinates( 1, x2, y2 );
			pSprite->SetTextureCoordinates( 2, x3, y3 );
			pSprite->SetTextureCoordinates( 3, x0, y0 );
		}

		pSelection = pSelection->Next();
	}
	
	Invalidate();
}

//=============================================================================
// GameView::OnTextureRotate
//=============================================================================

void GameView::OnTextureRotate( void )
{
	SetMouseMode( MOUSEMODE_ROTATETEXTURE );
}

//=============================================================================
// GameView::OnTextureAutoFit
//=============================================================================

void GameView::OnTextureAutoFit( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	float w, h;
	t_Handle TextureSet;

	D3DSURFACE_DESC SurfaceDesc;
	LPDIRECT3DTEXTURE9 pTexture;
	D3DLOCKED_RECT Lock;
	BITMAP2 bm2;
	int x, y;
	int x1, y1;
	int x2, y2;
	float u1, v1;
	float u2, v2;
	float tw, th;
	float sw, sh;
	COLOR32 c;
	bool found;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			pSprite->GetSize( w, h );
			
			pTexture = TextureMgr__GetTextureDirect3D( pSprite->GetTextureSet(), pSprite->GetTexture() );

			if( pTexture )
			{
				pTexture->GetLevelDesc( 0, &SurfaceDesc );
				pTexture->LockRect( 0, &Lock, 0, D3DLOCK_READONLY );

				tw = float( SurfaceDesc.Width );
				th = float( SurfaceDesc.Height );

				bm2.Width = SurfaceDesc.Width;
				bm2.Height = SurfaceDesc.Height;
				bm2.Format = PixelFormatFromDirectX( SurfaceDesc.Format );
				bm2.Stride = Lock.Pitch;
				bm2.pBits = (BYTE *) Lock.pBits;

				BeginBitmap( 0, &bm2 );

				x1 = bm2.Width;
				y1 = bm2.Height;
				x2 = 0; y2 = 0;

				found = false;

				for( y = 0; y < bm2.Height; y++ )
				{
					for( x = 0; x < bm2.Width; x++ )
					{
						c = GetPixelU( 0, x, y );

						if( c & 0xff000000 )
						{
							found = true;
							y1 = y;
							break;
						}
					}
					
					if( found )
						break;
				}

				found = false;

				for( y = bm2.Height - 1; y >= y1; y-- )
				{
					for( x = 0; x < bm2.Width; x++ )
					{
						c = GetPixelU( 0, x, y );

						if( c & 0xff000000 )
						{
							found = true;
							y2 = y;
							break;
						}
					}

					if( found )
						break;
				}

				found = false;

				for( x = 0; x < bm2.Width; x++ )
				{
					for( y = y1; y < y2; y++ )
					{
						c = GetPixelU( 0, x, y );

						if( c & 0xff000000 )
						{
							found = true;
							x1 = x;
							break;
						}
					}

					if( found )
						break;
				}

				found = false;

				for( x = bm2.Width - 1; x >= x1; x-- )
				{
					for( y = y1; y < y2; y++ )
					{
						c = GetPixelU( 0, x, y );

						if( c & 0xff000000 )
						{
							found = true;
							x2 = x;
							break;
						}
					}

					if( found )
						break;
				}

				FinishBitmap(0);

				pTexture->UnlockRect( 0 );

				u1 = float(x1) / tw;
				v1 = float(y1) / th;
				u2 = float(x2) / tw;
				v2 = float(y2) / th;

				sw = float( x2 - x1 ) * w / tw;
				sh = float( y2 - y1 ) * h / th;

				pSprite->SetTextureRect( u1, v1, u2, v2 );
				pSprite->SetSize( sw, sh );
			}
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnTextureDefault
//=============================================================================

void GameView::OnTextureDefault( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		pSprite->SetTextureRect( 0.0f, 0.0f, 1.0f, 1.0f );

		Invalidate();
	}

	return;
}

//=============================================================================
// GameView::OnGroupRemove
//=============================================================================

void GameView::OnGroupRemove( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		float x, y;

		SevenupDoc *pDocument = GetDocument();

		pSprite->GetScreenPosition( x, y );

		if( pDocument )
			pDocument->m_SceneList.AddChild( pSprite );

		pSprite->SetScreenPosition( x, y );
	}

	return;
}

//=============================================================================
// GameView::OnGroupSelectParent
//=============================================================================

void GameView::OnGroupSelectParent( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		Sprite *pParent = pSprite->Parent();

		if( pParent )
		{
			DeselectSprite( pSprite );
			SelectSprite( pParent );
			Invalidate();
		}
	}

	return;
}

//=============================================================================
// GameView::OnGroupSelectFamily
//=============================================================================

void GameView::OnGroupSelectFamily( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
	}

	return;
}

//=============================================================================
// GameView::OnInterfaceScreen
//=============================================================================

void GameView::OnInterfaceScreen( void )
{
	Sprite *pSprite = SelectedSprite();

	if( pSprite )
	{
		SpriteInterface* pInterface = pSprite->GetInterface( "Screen" );

		if( pInterface )
			delete pInterface;
		else
		{
			pSprite->AddInterface( "Screen", 0 );
		}
	}

	return;
}

//=============================================================================
// GameView::OnInterfaceButton
//=============================================================================

void GameView::OnInterfaceButton( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	SpriteInterface* pInterface;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			pInterface = pSprite->GetInterface( "Button" );

			if( pInterface )
				delete pInterface;
			else
			{
				pSprite->AddInterface( "Button", 0 );
			}
		}

		pSelection = pSelection->Next();
	}

	return;
}

//=============================================================================
// GameView::OnTextApplyFont
//=============================================================================

void GameView::OnTextApplyFont( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	Text* pText;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			if( 0 == strcmp( pSprite->Classname(), Text::ClassnameStatic() ) )
			{
				pText = (Text*) pSprite;
				pText->SetFont2( App.GetGameFont() );
			}
		}

		pSelection = pSelection->Next();
	}

	return;
}

//=============================================================================
// GameView::OnButtonZoom
//=============================================================================

void GameView::OnButtonZoom( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_ZOOM );
}

//=============================================================================
// GameView::OnButtonCenterView
//=============================================================================

void GameView::OnButtonCenterView( void )
{
	m_ViewPortX = 0.0f;
	m_ViewPortY = 0.0f;
	m_Zoom = 1.0f;

	Invalidate();
}

//=============================================================================
// GameView::OnButtonArrow
//=============================================================================

void GameView::OnButtonArrow( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_DRAG );
}

//=============================================================================
// GameView::OnButtonArrowSelect
//=============================================================================

void GameView::OnButtonArrowSelect( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_SELECT );
}

//=============================================================================
// GameView::OnButtonNewSprite
//=============================================================================

void GameView::OnButtonNewSprite( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_NEW );
	m_ObjectType = HUDOBJECT_SPRITE;
}

//=============================================================================
// GameView::OnButtonNewText
//=============================================================================

void GameView::OnButtonNewText( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_NEW );
	m_ObjectType = HUDOBJECT_TEXT;
}

//=============================================================================
// GameView::OnButtonStamper
//=============================================================================

void GameView::OnButtonStamper( void )
{
	DeselectAll();
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_STAMP );
}

//=============================================================================
// GameView::OnButtonProperties
//=============================================================================

void GameView::OnButtonProperties( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_PROPERTIES );
}

//=============================================================================
// GameView::OnButtonNewFamily
//=============================================================================

void GameView::OnButtonNewFamily( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_GROUP );
}

//=============================================================================
// GameView::OnButtonButtonOrder
//=============================================================================

void GameView::OnButtonButtonOrder( void )
{
	SetMouseMode( MOUSEMODE_HOVER );
	SetMouseTool( MOUSETOOL_BUTTONORDER );
}

//=============================================================================
// GameView::OnButtonGridLock
//=============================================================================

void GameView::OnButtonGridLock( void )
{
	m_GridLock = !m_GridLock;
}

void GameView::OnButtonSelectFamily( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;
	Sprite* pChild;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			pChild = pSprite->Child();

			while( pChild )
			{
				SelectSprite( pChild );

				pChild = pChild->Next();
			}
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

void GameView::OnButtonSelectParent( void )
{
	int c = m_SelectionList.GetNumChildren();

	SpriteItem* pSelection;
	Sprite* pSprite;
	Sprite** ppSelectionList = new Sprite*[c];

	int i;

	if( ppSelectionList )
	{
		pSelection = m_SelectionList.Child();

		for( i = 0; i < c; i++ )
		{
			if( !pSelection )
				ppSelectionList[i] = 0;
			else
				ppSelectionList[i] = pSelection->m_pSprite;

			pSelection = pSelection->Next();
		}

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSelectionList[i];

			if( pSprite && pSprite->Parent() && Project.GetProjectItem( pSprite->Parent() ) )
				DeselectSprite( pSprite );
		}

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSelectionList[i];

			if( pSprite && pSprite->Parent() && Project.GetProjectItem( pSprite->Parent() ) )
				SelectSprite( pSprite->Parent() );
		}

		delete[] ppSelectionList;
	}

	Invalidate();
}

void GameView::OnButtonSelectChildren( void )
{
	int c = m_SelectionList.GetNumChildren();

	SpriteItem* pSelection;
	Sprite* pSprite;
	Sprite* pChild;
	Sprite** ppSelectionList = new Sprite*[c];

	int i;

	if( ppSelectionList )
	{
		pSelection = m_SelectionList.Child();

		for( i = 0; i < c; i++ )
		{
			if( !pSelection )
				ppSelectionList[i] = 0;
			else
				ppSelectionList[i] = pSelection->m_pSprite;

			pSelection = pSelection->Next();
		}

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSelectionList[i];

			if( pSprite )
			{
				pChild = pSprite->Child();

				while( pChild )
				{
					if( Project.GetProjectItem( pChild ) )
						DeselectSprite( pSprite );

					pChild = pChild->Next();
				}
			}
		}

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSelectionList[i];

			if( pSprite )
			{
				pChild = pSprite->Child();

				while( pChild )
				{
					if( Project.GetProjectItem( pChild ) )
						SelectSprite( pChild );

					pChild = pChild->Next();
				}
			}
		}

		delete[] ppSelectionList;
	}

	Invalidate();
}

void GameView::OnButtonSelectNextChild( void )
{
	int c = m_SelectionList.GetNumChildren();
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite** ppSprites = new Sprite*[c];
	Sprite* pSprite;
	int i;

	if( ppSprites )
	{
		for( i = 0; i < c; i++ )
		{
			ppSprites[i] = pSelection->m_pSprite;
			pSelection = pSelection->Next();
		}

		DeselectAll();

		for( i = 0; i < c; i++ )
		{
			pSprite = ppSprites[i];

			if( pSprite->Next() )
				SelectSprite( pSprite->Next() );
			else
			{
				while( pSprite->Prev() )
					pSprite = pSprite->Prev();

				SelectSprite( pSprite );
			}
		}

		delete [] ppSprites;
	}

	return;
}

//=============================================================================
// GameView::OnButtonChildOrder
//=============================================================================

void GameView::OnButtonChildOrder( void )
{
}

//=============================================================================
// GameView::OnButtonGroup
//=============================================================================

void GameView::OnButtonGroup( void )
{
	SevenupDoc* pDocument = GetDocument();
	SpriteItem* pSelection;
	Sprite* pNewSprite;
	Sprite* pSprite;
	Sprite* pParent;
	float x, y;
	float x1, y1;
	float x2, y2;
	float ox, oy;
	float width;
	float height;

	pNewSprite = CreateSprite( HUDOBJECT_SPRITE );

	if( pNewSprite )
	{
		GetSelectionRect( x1, y1, x2, y2 );

		width = x2 - x1;
		height = y2 - y1;

		ox = oy = 0.5f;

		pNewSprite->SetTextureSet( INVALID_HANDLE, 0 );
		pNewSprite->SetSize( width, height );
		pNewSprite->SetOrigin( ox, oy );

		pSelection = m_SelectionList.Child();
		pParent = 0;

		while( pSelection && pParent == 0 )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite && !IsSelected( pSprite->Parent() ) )
				pParent = pSprite->Parent();

			pSelection = pSelection->Next();
		}

		if( pParent )
			pParent->AddChild( pNewSprite );

		pNewSprite->SetScreenPosition( x1 + width * ox, y1 + height * oy );

		pSelection = m_SelectionList.Child();

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite && !IsSelected( pSprite->Parent() ) )
			{
				pSprite->GetScreenPosition( x, y );
				pSprite->SetParent( pNewSprite );
				pSprite->SetScreenPosition( x, y );
			}

			pSelection = pSelection->Next();
		}

		if( pParent )
			pParent->AddChild( pNewSprite );

		DocumentSyncProjectViews();
	}

	return;
}

//=============================================================================
// GameView::OnButtonCreateFamily
//=============================================================================

void GameView::OnButtonCreateFamily( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pParent = 0;
	Sprite* pChild = 0;
	float x, y;

	while( pSelection )
	{
		pChild = pSelection->m_pSprite;

		if( pChild )
		{
			if( !pParent )
				pParent = pChild;
			else
			{
				pChild->GetScreenPosition( x, y );
				pChild->SetParent( pParent );
				pChild->SetScreenPosition( x, y );
			}
		}

		pSelection = pSelection->Next();
	}

	DocumentSyncProjectViews();
}

//=============================================================================
// GameView::OnButtonSeparateFamily
//=============================================================================

void GameView::OnButtonSeparateFamily( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSceneList = DocumentGetSceneList();
	Sprite* pSprite;
	float x, y;

	if( pSceneList )
	{
		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite )
			{
				pSprite->GetScreenPosition( x, y );
				pSprite->SetParent( pSceneList );
				pSprite->SetScreenPosition( x, y );
			}

			pSelection = pSelection->Next();
		}

		DocumentSyncProjectViews();
	}

	return;
}

//=============================================================================
// GameView::OnButtonSameSize
//=============================================================================

void GameView::OnButtonSameSize( void )
{
	OnButtonSameWidth();
	OnButtonSameHeight();
}

//=============================================================================
// GameView::OnButtonSameWidth
//=============================================================================

void GameView::OnButtonSameWidth( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSelSprite = SelectedSprite();
	Sprite* pSprite;

	if( pSelSprite )
	{
		float Width = pSelSprite->GetWidth();

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite != pSelSprite )
				pSprite->SetWidth( Width );

			pSelection = pSelection->Next();
		}
	}

	Invalidate();
}

//=============================================================================
// GameView::OnButtonSameHeight
//=============================================================================

void GameView::OnButtonSameHeight( void )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSelSprite = SelectedSprite();
	Sprite* pSprite;

	if( pSelSprite )
	{
		float Height = pSelSprite->GetHeight();

		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( pSprite != pSelSprite )
				pSprite->SetHeight( Height );

			pSelection = pSelection->Next();
		}
	}

	Invalidate();
}

//=============================================================================
// GameView::OnButtonSpaceVertical
//=============================================================================

void GameView::OnButtonSpaceVertical( void )
{
	SpaceSelection( FALSE, TRUE );
}

//=============================================================================
// GameView::OnButtonSpaceHorizontal
//=============================================================================

void GameView::OnButtonSpaceHorizontal( void )
{
	SpaceSelection( TRUE, FALSE );
}

//=============================================================================
// GameView::OnButtonAlignLeft
//=============================================================================

void GameView::OnButtonAlignLeft( void )
{
	AlignSelection(2);
}

//=============================================================================
// GameView::OnButtonAlignRight
//=============================================================================

void GameView::OnButtonAlignRight( void )
{
	AlignSelection(3);
}

//=============================================================================
// GameView::OnButtonAlignTop
//=============================================================================

void GameView::OnButtonAlignTop( void )
{
	AlignSelection(0);
}

//=============================================================================
// GameView::OnButtonAlignBottom
//=============================================================================

void GameView::OnButtonAlignBottom( void )
{
	AlignSelection(1);
}

//=============================================================================
// GameView::OnKeyEscape
//=============================================================================

void GameView::OnKeyEscape( void )
{
	m_EscapeKeyHandled = TRUE;
	OnEscapeKey( TRUE );
}

//=============================================================================
// GameView::OnPropertyChange
//=============================================================================

void GameView::OnPropertyChange( UINT Id, NMHDR *pNotify, LPARAM *pResult )
{
	PROPERTYNOTIFYSTRUCT *n = (PROPERTYNOTIFYSTRUCT *) pNotify;

	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite* pSprite;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;

		if( pSprite )
		{
			ProjectItem *p = (ProjectItem *) pSprite->m_pToolData;

			if( p )
				p->SetProperty( n->PropertyCode, n->pPropertyString );

		//	Project.SetProperty( p, n->pPropertyKey, n->pPropertyString, n->PropertyCode );

			if( n->PropertyCode == PROPERTYCODE_NAME )
			{
				SevenupDoc* pDocument = GetDocument();

				if( pDocument )
					pDocument->SyncProjectViews();
			}
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnCustomTextureUpdate
//=============================================================================

void GameView::OnCustomTextureUpdate( UINT Id, NMHDR *pNotify, LPARAM *pResult )
{
	SpriteItem* pSelection = m_SelectionList.Child();
	Sprite *pSprite;
	float x, y;
	int i;

	while( pSelection )
	{
		pSprite = pSelection->m_pSprite;
	
		if( pSprite )
		{
			for( i = 0; i < 4; i++ )
			{
				m_TextureWnd.GetTextureCoordinates( i, x, y );
				pSprite->SetTextureCoordinates( i, x, y );
			}
		}

		pSelection = pSelection->Next();
	}

	Invalidate();
}

//=============================================================================
// GameView::OnUpdateCommandUI
//=============================================================================)

void GameView::OnUpdateCommandUI( CCmdUI *pCmdUI )
{
	SevenupDoc *pDocument = GetDocument();
	Sprite* pSprite = SelectedSprite();
	ProjectItem* pProjectItem = Project.GetProjectItem(pSprite);
	SpriteItem* pSelection = m_SelectionList.Child();
	BOOL Enabled;

	switch( pCmdUI->m_nID )
	{
	case ID_SPRITE_LOCK:
		{
			if( pProjectItem )
			{
				if( pProjectItem->m_Item.Locked )
					pCmdUI->SetCheck(1);
				else
					pCmdUI->SetCheck(0);
			}
			break;
		}
	case ID_TEXTURE_APPLY:
		{
			if( pDocument )
			{
				if( pDocument->GetTextureHandle() == INVALID_HANDLE )
					pCmdUI->Enable( FALSE );
				else
					pCmdUI->Enable( TRUE );
			}
			break;
		}
	case ID_TEXTURE_REMOVE:
		{
			if( pDocument )
			{
				if( pSprite->GetTextureSet() == INVALID_HANDLE )
					pCmdUI->Enable( FALSE );
				else
					pCmdUI->Enable( TRUE );
			}
			break;
		}
	case ID_INTERFACES_SCREEN:
		{
			if( pSprite )
			{
				if( pSprite->GetInterface( "Screen" ) )
					pCmdUI->SetCheck(1);
				else
					pCmdUI->SetCheck(0);
			}
		}
		break;
	case ID_INTERFACES_BUTTON:
		{
			if( pSprite )
			{
				if( pSprite->GetInterface( "Button" ) )
					pCmdUI->SetCheck(1);
				else
					pCmdUI->SetCheck(0);
			}
		}
		break;
	case ID_TEXT_APPLYFONT:
		{
			Enabled = FALSE;

			while( pSelection && !Enabled )
			{
				pSprite = pSelection->m_pSprite;

				if( pSprite && 0 == strcmp( pSprite->Classname(), Text::ClassnameStatic() ) )
					Enabled = TRUE;

				pSelection = pSelection->Next();
			}

			pCmdUI->Enable( Enabled );
		}
		break;
	case ID_BUTTON_ARROW:
		{
			if( m_MouseTool == MOUSETOOL_DRAG )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_EDIT_CUT:
	case ID_EDIT_COPY:
	case ID_EDIT_DELETE:
		{
			if( m_SelectionList.Child() )
				pCmdUI->Enable( TRUE );
			else
				pCmdUI->Enable( FALSE );
		}
		break;
	case ID_EDIT_PASTE:
		{
			if( m_CopyList.Child() )
				pCmdUI->Enable( TRUE );
			else
				pCmdUI->Enable( FALSE );
		}
		break;
	case ID_BUTTON_ARROWSELECT:
		{
			if( m_MouseTool == MOUSETOOL_SELECT )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_NEWSPRITE:
		{
			if( m_MouseTool == MOUSETOOL_NEW && m_ObjectType == HUDOBJECT_SPRITE )
				pCmdUI->SetRadio(	TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_NEWTEXT:
		{
			if( m_MouseTool == MOUSETOOL_NEW && m_ObjectType == HUDOBJECT_TEXT )
				pCmdUI->SetRadio(	TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_STAMPER:
		{
			if( m_CopyList.Child() )
				pCmdUI->Enable( TRUE );
			else
				pCmdUI->Enable( FALSE );
			
			if( m_MouseTool == MOUSETOOL_STAMP )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_GRIDLOCK:
		{
			if( m_GridLock )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_NEWFAMILY:
		{
			if( m_SelectionList.GetNumChildren() == 1 )
				pCmdUI->Enable( TRUE );
			else
				pCmdUI->Enable( FALSE );

			if( m_MouseTool == MOUSETOOL_GROUP )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_PROPERTIES:
		{
			if( m_MouseTool == MOUSETOOL_PROPERTIES )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_ZOOM:
		{
			if( m_MouseTool == MOUSETOOL_ZOOM )
				pCmdUI->SetRadio( TRUE );
			else
				pCmdUI->SetRadio( FALSE );
		}
		break;
	case ID_BUTTON_SELECTFAMILY:
	case ID_BUTTON_SELECTCHILDREN:
		{
			Enabled = FALSE;

			while( pSelection && !Enabled )
			{
				pSprite = pSelection->m_pSprite;

				if( pSprite && pSprite->Child() )
					Enabled = TRUE;

				pSelection = pSelection->Next();
			}

			pCmdUI->Enable( Enabled );
		}
		break;
	case ID_BUTTON_SELECTPARENT:
		{
			Enabled = FALSE;

			while( pSelection && !Enabled )
			{
				pSprite = pSelection->m_pSprite;

				if( pSprite && pSprite->Parent() )
					Enabled = TRUE;

				pSelection = pSelection->Next();
			}

			pCmdUI->Enable( Enabled );
		}
		break;
	case ID_BUTTON_SELECTNEXTCHILD:
		{
			Enabled = FALSE;

			while( pSelection && !Enabled )
			{
				pSprite = pSelection->m_pSprite;

				if( pSprite )
				{
					if( pSprite->Next() || pSprite->Prev() )
						Enabled = TRUE;
				}

				pSelection = pSelection->Next();
			}

			pCmdUI->Enable( Enabled );
		}
		break;
	case ID_BUTTON_GROUP:
	case ID_BUTTON_CREATEFAMILY:
		if( m_SelectionList.GetNumChildren() > 1)
			pCmdUI->Enable( TRUE );
		else
			pCmdUI->Enable( FALSE );
		break;
	case ID_BUTTON_SEPARATEFAMILY:
		{
			Enabled = FALSE;

			while( pSelection && !Enabled )
			{
				pSprite = pSelection->m_pSprite;

				if( pSprite )
				{
					if( pSprite->Parent() || pSprite->Child() )
						Enabled = TRUE;
				}

				pSelection = pSelection->Next();
			}

			pCmdUI->Enable( Enabled );
		}
		break;
	case ID_BUTTON_SAMESIZE:
	case ID_BUTTON_SAMEWIDTH:
	case ID_BUTTON_SAMEHEIGHT:
	case ID_BUTTON_ALIGNLEFT:
	case ID_BUTTON_ALIGNRIGHT:
	case ID_BUTTON_ALIGNTOP:
	case ID_BUTTON_ALIGNBOTTOM:
		if( m_SelectionList.GetNumChildren() > 1)
			pCmdUI->Enable( TRUE );
		else
			pCmdUI->Enable( FALSE );
		break;

	case ID_BUTTON_SPACEVERTICAL:
	case ID_BUTTON_SPACEHORIZONTAL:
		if( m_SelectionList.GetNumChildren() > 2)
			pCmdUI->Enable( TRUE );
		else
			pCmdUI->Enable( FALSE );
	}

	return;
}


//=============================================================================
// GameView::Search
//=============================================================================

void GameView::Search( SpriteItem& SearchList, SpriteItem& FoundList, const char* pFindWhat, BOOL MatchWholeWord, BOOL MatchCase )
{
	SpriteItem* pSearch = SearchList.Child();
	Sprite* pSprite;
	ProjectItem* pProjectItem;
	const char* pName;
	const char* pFound;

	if( MatchWholeWord )
	{
		char* pWholeWord = new char[tstrlen(pFindWhat)+3];

		if( pWholeWord )
		{
			sprintf( pWholeWord, " %s ", pFindWhat );

			Search( SearchList, FoundList, pWholeWord, FALSE, MatchCase );

			delete [] pWholeWord;
		}
	}
	else
	{
		while( pSearch )
		{
			pSprite = pSearch->m_pSprite;
			pProjectItem = Project.GetProjectItem( pSprite );

			if( pSprite && pProjectItem )
			{
				pName = pProjectItem->m_Name.get();

				if( MatchCase )
					pFound = tstrstr( pName, pFindWhat );
				else
					pFound = tstristr( pName, pFindWhat );

				if( pFound )
					AddToSpriteList( FoundList, pSprite );
			}

			pSearch = pSearch->Next();
		}
	}

	return;
}

//=============================================================================
// GameView::OnFindDialogFind
//=============================================================================

void GameView::OnFindDialogFind( void )
{
	m_FindDialog.ClearSearchResults();

	FindDialog::SearchPath SearchPath = m_FindDialog.m_SearchPath;
	const char* pFindWhat = m_FindDialog.m_szFindWhat;
	const char* pReplaceWith = m_FindDialog.m_szReplaceWith;

	SpriteItem* pSelection = m_SelectionList.Child();
	SpriteItem SearchList;
	SpriteItem FoundList;
	Sprite* pSprite;
	Sprite* pChild;
	Sprite* pSibling;
	Sprite* pParent;
	Sprite* pSceneList;
	BOOL SearchChildren;
	BOOL SearchParents;
	BOOL SearchSiblings;

	pSceneList = DocumentGetSceneList();

	SearchParents  = m_FindDialog.m_bSearchParents;
	SearchChildren = m_FindDialog.m_bSearchChildren;
	SearchSiblings = m_FindDialog.m_bSearchSiblings;

	if( SearchPath == FindDialog::SearchPath_EntireProject )
	{
		if( pSceneList )
		{
			pChild = pSceneList->Child();

			while( pChild )
			{
				AddFamilyToSpriteList( SearchList, pChild );

				pChild = pChild->Next();
			}
		}
	}
	else
	if( SearchPath == FindDialog::SearchPath_CurrentScreen )
	{
		AddFamilyToSpriteList( SearchList, TopSprite() );
	}
	else
	if( SearchPath == FindDialog::SearchPath_SelectionOnly )
	{
		while( pSelection )
		{
			pSprite = pSelection->m_pSprite;

			if( SearchChildren )
				AddFamilyToSpriteList( SearchList, pSprite );
			else
				AddToSpriteList( SearchList, pSprite );

			if( pSprite )
			{
				if( SearchSiblings )
				{
					pSibling = pSprite->Prev();

					if( pSibling )
					{
						while( pSibling->Prev() )
							pSibling = pSibling->Prev();

						while( pSibling )
						{
							if( pSibling != pSprite )
								AddToSpriteList( SearchList, pSibling );

							pSibling = pSibling->Next();
						}
					}
				}

				if( SearchParents )
				{
					pParent = pSprite->Parent();

					while( pParent )
					{
						if( pParent != pSceneList )
							AddToSpriteList( SearchList, pParent );

						pParent = pParent->Parent();

						if( pParent == pSceneList )
							pParent = 0;
					}
				}
			}

			pSelection = pSelection->Next();
		}
	}
	
	Search( SearchList, FoundList, pFindWhat, m_FindDialog.m_bMatchWholeWord, m_FindDialog.m_bMatchCase );

	SpriteItem* pFound = FoundList.Child();
	ProjectItem* pProjectItem;
	int i;
	char szParentName[512];

	while( pFound )
	{
		pSprite = pFound->m_pSprite;

		if( pSprite )
		{
			pProjectItem = Project.GetProjectItem( pSprite );

			if( pProjectItem )
			{
				int i = m_FindDialog.AddSearchResult( pProjectItem->m_Name.get(), pFound->m_pSprite );

				pParent = pSprite->Parent();

				strcpy( szParentName, "" );

				while( pParent )
				{
					pProjectItem = Project.GetProjectItem( pParent );

					if( pProjectItem )
						strcat( szParentName, Printf( "%s / ", pProjectItem->m_Name.get() ) );

					pParent = pParent->Parent();
				}

				m_FindDialog.SetColumn( i, 1, Printf( "%04d", i ) );
				m_FindDialog.SetColumn( i, 2, szParentName );
			}
		}

		pFound = pFound->Next();
	}

	if( m_FindDialog.m_bAutoSelect )
	{
		DeselectAll();

		SpriteItem* pFoundItem = FoundList.Child();

		while( pFoundItem )
		{
			if( pFoundItem->m_pSprite )
				SelectSprite( pFoundItem->m_pSprite );

			pFoundItem = pFoundItem->Next();
		}
	}

	ClearSpriteList( SearchList );
	ClearSpriteList( FoundList );

	Invalidate();
}

//=============================================================================
// GameView::OnFindDialogReplace
//=============================================================================

void GameView::OnFindDialogReplace( void )
{
	OnFindDialogFind();
}


//=============================================================================
// GameView::OnFindDialogSelect
//=============================================================================

void GameView::OnFindDialogSelect( void )
{
	CListCtrl* pListBox = m_FindDialog.GetListBox();

	if( pListBox )
	{
		POSITION pos = pListBox->GetFirstSelectedItemPosition();
		Sprite* pSprite;
		int i, n;

		if( !pos )
		{
			n = pListBox->GetItemCount();

			for( i = 0; i < n; i++ )
			{
				pSprite = (Sprite*) pListBox->GetItemData(i);

				if( pSprite )
					SelectSprite( pSprite );
			}
		}

		while( pos )
		{
			i = pListBox->GetNextSelectedItem( pos );

			pSprite = (Sprite*) pListBox->GetItemData(i);

			if( pSprite )
				SelectSprite( pSprite );
		}
	}

	return;
}
