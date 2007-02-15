/******************* includes ****************************/

#include "hud/HudPCH.h"

//////////////////////////////////////////////////////////////////////////

#include "hud/newmenu.h"

BaseMenu *BaseMenu::m_pMenuWhoCalledBack = 0;
BaseMenu *BaseMenu::GetMenuWhoCalledBack( void ) { return m_pMenuWhoCalledBack; }
BaseMenu *GetMenuWhoCalledBack( void ) { return BaseMenu::GetMenuWhoCalledBack(); }

//=============================================================================
// BaseMenu::BaseMenu()
//=============================================================================

BaseMenu::BaseMenu()
{
	m_Clicks	= 0;

	m_pAddMenuTarget	= this;
	m_pNewestMenu		= this;
	m_pActiveMenu		= this;
	m_pSelection		= 0;
	m_pPrevSelection	= 0;

	ClearCallBack();
}

//=============================================================================
// BaseMenu::~BaseMenu()
//=============================================================================

BaseMenu::~BaseMenu()
{
	Release();
}

//=============================================================================
// BaseMenu::Release()
//=============================================================================

void BaseMenu::Release()
{
	while( m_pChild )
		delete m_pChild;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName )
{
	BaseMenu *pNewMenu = (BaseMenu*) New();

	if( pNewMenu )
		pNewMenu->SetText( pName );

	m_pAddMenuTarget->AddChild( pNewMenu );

	if( pNewMenu )
		m_pNewestMenu = pNewMenu;

	return pNewMenu;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName, MENUHANDLER Handler, int Command, PARAM Param )
{
	BaseMenu *pMenu = AddMenuItem( pName );

	if( pMenu )
		pMenu->SetCommand( Handler, Command, Param );

	return pMenu;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName, char *pCommand )
{
	BaseMenu *pMenu = AddMenuItem( pName );

	if( pMenu )
		pMenu->SetCommand( pCommand );

	return pMenu;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName, STATICCALLBACKVOID CallBack )
{
	BaseMenu *pMenu = AddMenuItem( pName );

	if( pMenu )
		pMenu->SetCallBack( CallBack );

	return pMenu;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName, STATICCALLBACK CallBack, PARAM Param )
{
	BaseMenu *pMenu = AddMenuItem( pName );

	if( pMenu )
		pMenu->SetCallBack( CallBack, Param );

	return pMenu;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName, void *pObject, MEMBERCALLBACKVOID CallBack )
{
	BaseMenu *pMenu = AddMenuItem( pName );

	if( pMenu )
		pMenu->SetCallBack( pObject, CallBack );

	return pMenu;
}

//=============================================================================
// BaseMenu::AddMenuItem()
//=============================================================================

BaseMenu *BaseMenu::AddMenuItem( char *pName, void *pObject, MEMBERCALLBACK CallBack, PARAM Param )
{
	BaseMenu *pMenu = AddMenuItem( pName );

	if( pMenu )
		pMenu->SetCallBack( pObject, CallBack, Param );

	return pMenu;
}

//=============================================================================
// BaseMenu::SetCommand()
//=============================================================================

void BaseMenu::SetCommand( char *pCommand )
{
	m_szCommand.set( pCommand );
}

//=============================================================================
// BaseMenu::SetCommand()
//=============================================================================

void BaseMenu::SetCommand( MENUHANDLER Handler, int Command, PARAM Param )
{
	m_CallBackParam = Param;
	
	if( Handler )
		m_CallBackHandler = Handler;
	
	m_CallBackCommand = Command;
}

//=============================================================================
// BaseMenu::SetCallBack()
//=============================================================================

void BaseMenu::SetCallBack( STATICCALLBACKVOID CallBack )
{
	ClearCallBack();

	m_StaticCallBackVoid = CallBack;
}

//=============================================================================
// BaseMenu::SetCallBack()
//=============================================================================

void BaseMenu::SetCallBack( STATICCALLBACK CallBack, PARAM Param )
{
	ClearCallBack();

	m_StaticCallBack = CallBack;
	m_CallBackParam = Param;
}

//=============================================================================
// BaseMenu::SetCallBack()
//=============================================================================

void BaseMenu::SetCallBack( void *pObject, MEMBERCALLBACKVOID CallBack )
{
	ClearCallBack();

	m_pCallBackThisPointer = (UnknownClass *) pObject;
	m_MemberCallBackVoid = CallBack;
}

//=============================================================================
// BaseMenu::SetCallBack()
//=============================================================================

void BaseMenu::SetCallBack( void *pObject, MEMBERCALLBACK CallBack, PARAM Param )
{
	ClearCallBack();

	m_pCallBackThisPointer = (UnknownClass *) pObject;
	m_MemberCallBack = CallBack;
}

//=============================================================================
// BaseMenu::ClearCallBack()
//=============================================================================

void BaseMenu::ClearCallBack( void )
{
	m_pCallBackThisPointer = 0;
	m_CallBackParam = 0;
	m_StaticCallBack = 0;
	m_StaticCallBackVoid = 0;
	m_MemberCallBack = 0;
	m_MemberCallBackVoid = 0;
	m_CallBackHandler = 0;
	m_CallBackCommand = 0;
}

//=============================================================================
// BaseMenu::ExecuteCallBack()
//=============================================================================

void BaseMenu::ExecuteCallBack( void )
{
	m_pMenuWhoCalledBack = this;

	if( m_pCallBackThisPointer )
	{
		if( m_MemberCallBackVoid )
			MemberCallBackVoid( m_pCallBackThisPointer, m_MemberCallBackVoid );
	
		if( m_MemberCallBack )
			MemberCallBack( m_pCallBackThisPointer, m_MemberCallBack, m_CallBackParam );
	}
	else
	{
		if( m_StaticCallBackVoid )
			StaticCallBackVoid( m_StaticCallBackVoid );
		
		if( m_StaticCallBack )
			StaticCallBack( m_StaticCallBack, m_CallBackParam );

		if( m_CallBackHandler )
			(*m_CallBackHandler)( m_CallBackCommand, m_CallBackParam );
	}

	if( m_szCommand.get() )
	{
		// TODO: Execute a console command!
	}

	if( m_pMenuWhoCalledBack == this )
		m_pMenuWhoCalledBack = 0;

	return;
}

//=============================================================================
// BaseMenu::Branch()
//=============================================================================

void BaseMenu::Branch( void )
{
	m_pAddMenuTarget = m_pNewestMenu;
}

//=============================================================================
// BaseMenu::Return()
//=============================================================================

void BaseMenu::Return( void )
{
	m_pNewestMenu = m_pAddMenuTarget;

	if( m_pAddMenuTarget && m_pAddMenuTarget->m_pParent )
		m_pAddMenuTarget = (BaseMenu *) m_pAddMenuTarget->m_pParent;

	ASSERTS( m_pNewestMenu->GetNumChildren() <= MAX_MENU_BARS, "ERROR: Too many menu items" );
	return;
}

//=============================================================================
// BaseMenu::SetText()
//=============================================================================

void BaseMenu::SetText( char *pText )
{
	SetText( 0, pText );
}

//=============================================================================
// BaseMenu::SetText()
//=============================================================================

void BaseMenu::SetText( int Column, char *pText )
{
	m_szText[Column].set( pText );
}

//=============================================================================
// BaseMenu::GetText()
//=============================================================================

char *BaseMenu::GetText( int Column )
{
	return m_szText[Column].get();
}

//=============================================================================
// BaseMenu::ClearText()
//=============================================================================

void BaseMenu::ClearText( int Column )
{
	m_szText[Column].release();
}

//=============================================================================
// BaseMenu::ClearAllText()
//=============================================================================

void BaseMenu::ClearAllText( void )
{
	for( int i = 0; i < 8; i++ )
		ClearText(i);

	return;
}

//=============================================================================
// BaseMenu::Sort()
//=============================================================================

void BaseMenu::Sort( int Column, bool Reversed )
{
	BaseMenu *pMenu1 = (BaseMenu *) GetNthChild(0);
	BaseMenu *pMenu2 = (BaseMenu *) GetNthChild(1);

	while( pMenu1 && pMenu2 )
	{
		if( xstrsort( pMenu1->GetText( Column ), pMenu2->GetText( Column ) ) )
		{
			pMenu2->InsertAfter( pMenu1 );

			if( pMenu2->Prev() )
				pMenu1 = (BaseMenu * ) pMenu2->Prev();
			else
			{
				pMenu2 = (BaseMenu *) pMenu1->Next();
			}
		}
		else
		{
			pMenu1 = pMenu2;
			pMenu2 = (BaseMenu *) pMenu2->Next();
		}
	}

	return;
}

//=============================================================================
// BaseMenu::Click()
//=============================================================================

void BaseMenu::Click( void )
{
	m_Clicks++;

	ExecuteCallBack();

	return;
}

//=============================================================================
// BaseMenu::Clicked()
//=============================================================================

bool BaseMenu::Clicked( void )
{
	if( m_Clicks < 1 )
		return false;

	m_Clicks--;

	return true;
}

//=============================================================================
// BaseMenu::CursorUp()
//=============================================================================

void BaseMenu::CursorUp( void )
{
	BaseMenu *s = Selection();

	if( m_pSelection )
	{
		if( m_pSelection->m_pPrev )
			Select( (BaseMenu *) m_pSelection->m_pPrev );
		else
		{
			while( m_pSelection->m_pNext )
				Select( (BaseMenu *) m_pSelection->m_pNext );
		}
	}

	return;
}

//=============================================================================
// BaseMenu::CursorDown()
//=============================================================================

void BaseMenu::CursorDown( void )
{
	BaseMenu *s = Selection();

	if( m_pSelection )
	{
		if( m_pSelection->m_pNext )
				Select( (BaseMenu *) m_pSelection->m_pNext );
		else
		{
			while( m_pSelection->m_pPrev )
				Select( (BaseMenu *) m_pSelection->m_pPrev );
		}
	}

	return;
}

//=============================================================================
// BaseMenu::CursorLeft()
//=============================================================================

void BaseMenu::CursorLeft( void )
{
	if( m_pActiveMenu && m_pActiveMenu->m_pParent )
		Select( m_pActiveMenu );

	return;
}

//=============================================================================
// BaseMenu::CursorRight()
//=============================================================================

void BaseMenu::CursorRight( void )
{
	if( m_pSelection && m_pSelection->m_pChild )
		Select( (BaseMenu *) m_pSelection->m_pChild );

	return;
}

//=============================================================================
// BaseMenu::CursorEnter()
//=============================================================================

void BaseMenu::CursorEnter( void )
{
	BaseMenu *s = Selection();

	if( s )
		s->Click();
	
	CursorRight();
}

//=============================================================================
// BaseMenu::CursorExit()
//=============================================================================

void BaseMenu::CursorExit( void )
{
	CursorLeft();
}

//=============================================================================
// BaseMenu::KeyboardInput()
//=============================================================================

void BaseMenu::KeyboardInput( char Ch )
{
}

//=============================================================================
// BaseMenu::ReturnToTop()
//=============================================================================

void BaseMenu::ReturnToTop( void )
{
	while( m_pActiveMenu && m_pActiveMenu->m_pParent )
		Select( m_pActiveMenu );

	return;
}

//=============================================================================
// BaseMenu::Select()
//=============================================================================

void BaseMenu::Select( BaseMenu *pMenu )
{
	m_pPrevSelection = m_pSelection;
	m_pSelection = pMenu;

	if( m_pSelection )
		m_pActiveMenu = (BaseMenu *) m_pSelection->m_pParent;

	return;
}

//=============================================================================
// BaseMenu::ActiveMenu()
//=============================================================================

BaseMenu *BaseMenu::ActiveMenu( void )
{
	return m_pActiveMenu;
}

//=============================================================================
// BaseMenu::Selection()
//=============================================================================

BaseMenu *BaseMenu::Selection()
{
	if( !m_pSelection && m_pActiveMenu )
		Select( (BaseMenu *) m_pActiveMenu->m_pChild );

	return m_pSelection;
}

//=============================================================================
// BaseMenu::PrevSelection()
//=============================================================================

BaseMenu *BaseMenu::PrevSelection()
{
	return m_pPrevSelection;
}

bool BaseMenu::IsCommandMenu( void )
{
	if(m_StaticCallBack || m_StaticCallBackVoid ||
		m_MemberCallBack || m_MemberCallBackVoid ||
		m_CallBackHandler || m_szCommand.get() )
		return true;

	return false;
}

//=============================================================================
// GameMenu::GameMenu()
//=============================================================================

int GameMenuSize = 0;

GameMenu::GameMenu()
{
	m_Active = false;
	m_TextureSet = INVALID_HANDLE;
	m_LoadedTextureSet = INVALID_HANDLE;

	for( int i = 0; i < MAX_MENU_BARS; i++ )
	{
		m_MenuBar1[i] = 0;
		m_MenuBar2[i] = 0;
		m_MenuBar3[i] = 0;
		m_MenuText[i] = 0;
		m_MenuAnim[i] = 0;
		m_FlashAnim[i] = 0;
	}

	GameMenuSize += sizeof( GameMenu );
}

//=============================================================================
// GameMenu::~GameMenu()
//=============================================================================

GameMenu::~GameMenu()
{
	FreeTextureSet();

	for( int i = 0; i < MAX_MENU_BARS; i++ )
	{
		delete m_MenuText[i];
		delete m_MenuBar1[i];
		delete m_MenuBar2[i];
		delete m_MenuBar3[i];
		delete m_MenuAnim[i];
		delete m_FlashAnim[i];
	}

	GameMenuSize -= sizeof( GameMenu );
}

//=============================================================================
// GameMenu::LoadTextureSet()
//=============================================================================

t_Handle GameMenu::LoadTextureSet( char *pFileName )
{
	FreeTextureSet();

	m_LoadedTextureSet = dblLoad( pFileName );

	if( m_LoadedTextureSet != INVALID_HANDLE )
		m_TextureSet = m_LoadedTextureSet;

	return m_LoadedTextureSet;
}

//=============================================================================
// GameMenu::FreeTextureSet()
//=============================================================================

t_Handle GameMenu::FreeTextureSet( void )
{
	if( m_TextureSet == m_LoadedTextureSet )
		m_TextureSet = INVALID_HANDLE;

	if( m_LoadedTextureSet != INVALID_HANDLE )
		dblRemove( m_LoadedTextureSet );

	m_LoadedTextureSet = INVALID_HANDLE;

	return m_LoadedTextureSet;
}

//=============================================================================
// GameMenu::SetTextureSet()
//=============================================================================

void GameMenu::SetTextureSet( t_Handle TextureSet )
{
	m_TextureSet = TextureSet;
}

//=============================================================================
// GameMenu::TextureSet()
//=============================================================================

t_Handle GameMenu::TextureSet( void )
{
	return m_TextureSet;
}

//=============================================================================
// GameMenu::InitMenu()
//=============================================================================

void GameMenu::InitMenu()
{
	int i;
	float space = 2.0f;
	float barheight = 22.0f;
	float allheight = (space + barheight) * 16.0f;
	float y1 = 0.0f;
	float y2;
	float x1;
	float x2;
	float x3;
	float base = 0.0f;

	m_MenuDock.SetPriority( 0xc000 );
	m_MenuDock.SetPosition( 30.0f, 30.0f );

	for( i = 0; i < MAX_MENU_BARS; i++ )
	{
		y2 = y1 + barheight;

		x1 = Math::SinDeg( y1 * 135.0f / allheight + 180.0f ) * 35.0f;
		x2 = Math::SinDeg( y2 * 135.0f / allheight + 180.0f ) * 35.0f;

		x3 = x1;

		if( x3 < x2 )
			x3 = x2;

		if( !m_MenuBar1[i] )
			m_MenuBar1[i] = new Sprite;

		m_MenuBar1[i]->Create( m_TextureSet, 0 );
		m_MenuBar1[i]->SetSize( 47.0f, barheight );
		m_MenuBar1[i]->SetPosition( 0.0f, y1 );
		m_MenuBar1[i]->SetVertexPosition( 1, x1, 0.0f );
		m_MenuBar1[i]->SetVertexPosition( 2, x2, 0.0f );
		m_MenuBar1[i]->SetColor( 0, 0, 0, 0 );
		m_MenuBar1[i]->SetParent( &m_MenuDock );

		if( !m_MenuBar2[i] )
			m_MenuBar2[i] = new Sprite;

		m_MenuBar2[i]->Create( m_TextureSet, 0 );
		m_MenuBar2[i]->SetSize( 192.0f, m_MenuBar1[i]->m_state.size.y );
		m_MenuBar2[i]->SetPosition( m_MenuBar1[i]->m_state.size.x + 2.0f, 0.0f );
		m_MenuBar2[i]->SetVertexPosition( 0, x1 * 1.0f, 0.0f );
		m_MenuBar2[i]->SetVertexPosition( 1, x1 * 3.0f, 0.0f );
		m_MenuBar2[i]->SetVertexPosition( 2, x2 * 3.0f, 0.0f );
		m_MenuBar2[i]->SetVertexPosition( 3, x2 * 1.0f, 0.0f );
		m_MenuBar2[i]->SetColor( 0, 0, 0, 64 );
		m_MenuBar2[i]->SetParent( m_MenuBar1[i] );

		if( !m_MenuBar3[i] )
			m_MenuBar3[i] = new Sprite;

		m_MenuBar3[i]->Create( m_TextureSet, 0 );
		m_MenuBar3[i]->SetSize( 8.0f, barheight );
		m_MenuBar3[i]->SetPosition( m_MenuBar2[i]->m_state.size.x + 2.0f, 0.0f );
		m_MenuBar3[i]->SetVertexPosition( 0, x1 * 3.0f, 0.0f );
		m_MenuBar3[i]->SetVertexPosition( 1, x1 * 3.0f, 0.0f );
		m_MenuBar3[i]->SetVertexPosition( 2, x2 * 3.0f, 0.0f );
		m_MenuBar3[i]->SetVertexPosition( 3, x2 * 3.0f, 0.0f );
		m_MenuBar3[i]->SetColor( 0.0f, 0.0f, 0.0f, 1.0f );
		m_MenuBar3[i]->SetParent( m_MenuBar2[i] );

		if( !m_MenuText[i] )
			m_MenuText[i] = new Text;
		
		m_MenuText[i]->SetPosition( x3 + 2.0f, 0.0f );
		m_MenuText[i]->SetTextHeight( m_MenuBar2[i]->m_state.size.y );
		m_MenuText[i]->SetParent( m_MenuBar2[i] );

		y1 = y1 + m_MenuBar1[i]->m_state.size.y + 2.0f;

		if( !m_MenuAnim[i] )
			m_MenuAnim[i] = new Animator;

		m_MenuAnim[i]->SetTarget( &m_MenuBar1[i]->m_state.scale.y );
		m_MenuAnim[i]->SetFrame( base + 0.0f, 0.0f );
		m_MenuAnim[i]->SetFrame( base + 0.1f, 1.0f );

		if( !m_FlashAnim[i] )
			m_FlashAnim[i] = new Animator;

		m_FlashAnim[i]->SetRange( 0.000f, 0.200f, 1.0f );
		m_FlashAnim[i]->SetFrame( 0.400f, 0.0f );

		base = base + 0.025f;
	}

	m_Blinker.SetRange( 0.000f, 0.300f, 128.0f );
	m_Blinker.SetRange( 0.400f, 0.700f, 255.0f );
	m_Blinker.SetFrame( 0.800f, 128.0f );
	m_Blinker.Play( true );


	DeactivateMenu();

	return;
}

//=============================================================================
// GameMenu::DrawMenu()
//=============================================================================

void GameMenu::DrawMenu( void )
{
	if( m_Active )
	{
		GameMenu *pActiveMenu = (GameMenu *) ActiveMenu();

		if( pActiveMenu )
		{
			GameMenu *pMenu = (GameMenu *) pActiveMenu->Child();

			int i;
			int items = pActiveMenu->GetNumChildren();

			ASSERTS( items <= MAX_MENU_BARS, "ERROR: Attempting to draw more than 16 menu items" );	// attempting to draw more than 16 items

			for( i = 0; i < items; i++ )
			{
				if( m_MenuBar1[i] && m_MenuBar2[i] && m_MenuBar3[i] && m_MenuText[i] )
				{
					m_MenuBar1[i]->Show();
					m_MenuBar2[i]->Show();
					m_MenuText[i]->Show();

					if( pMenu->GetText(1) )
						m_MenuText[i]->SetText( Printf( "%s (%s)", pMenu->GetText(), pMenu->GetText(1) ) );
					else
						m_MenuText[i]->SetText( pMenu->GetText() );

					m_MenuBar3[i]->SetColor( 0, 0, 0, 128 );

					if( pMenu == Selection() )
					{
						if( pMenu->Clicked() )
							m_FlashAnim[i]->Play( false );

						m_MenuBar1[i]->SetColor( 96, 0, 0, 192 );//(int) m_Blinker.m_Value );
						m_MenuBar2[i]->SetColor( 96, 0, 0, 192 );//(int) m_Blinker.m_Value );

						if( m_DebugMode )
						{
							m_MenuBar1[i]->SetColor( 0, 0, 0, 255 );
							m_MenuBar2[i]->SetColor( 0, 0, 0, 255 );
							m_MenuBar3[i]->SetColor( 0, 0, 0, 255 );
						}
					}
					else
					{
						m_MenuText[i]->SetColor( 255, 255, 255, 255 );
						m_MenuBar1[i]->SetColor( 0, 0, 192, 0 );
						m_MenuBar2[i]->SetColor( 0, 0, 192, 128 );
					}

				//	m_MenuBar2[i]->MixColor( 1.0f, 1.0f, 0.0f, 1.0f, m_FlashAnim[i]->Value() );

					if( pMenu->GetText(1) )
					{
						// FIXME: This is really bad, but there isn't any alternative!

						if( 0 == xstrcmp( "DebugMenu", pMenu->Classname() ) )
						{
							DebugMenu *pDebugMenu = (DebugMenu *) pMenu;

							float Lo = pDebugMenu->m_Range.lo;
							float Hi = pDebugMenu->m_Range.hi;
							float Range = Hi - Lo;

							m_MenuBar3[i]->SetSize( 8.0f + 100.0f * ((float) atof( pMenu->GetText(1) ) - Lo ) / Range, m_MenuBar3[i]->GetHeight() );
						}
						else
						{
							m_MenuBar3[i]->SetSize( 8.0f + 100.0f * ( float )atof( pMenu->GetText(1) ), m_MenuBar3[i]->GetHeight() );
						}

						m_MenuBar3[i]->Show();
					}
					else
						m_MenuBar3[i]->Hide();
				}

				pMenu = (GameMenu *) pMenu->Next();
			}

			for( i = items; i < MAX_MENU_BARS; i++ )
			{
				if( m_MenuBar1[i] && m_MenuBar2[i] && m_MenuBar3[i] && m_MenuText[i] )
				{
					m_MenuBar1[i]->Hide();
					m_MenuBar2[i]->Show();
					m_MenuBar3[i]->Hide();
					m_MenuText[i]->Hide();

					m_MenuBar2[i]->SetColor( 0, 0, 0, 64 );
				}
			}
		}

		m_MenuDock.Update();
	}

	return;
}

//=============================================================================
// GameMenu::ActivateMenu()
//=============================================================================

void GameMenu::ActivateMenu( void )
{
	for( int i = 0; i < MAX_MENU_BARS; i++ )
	{
		if( m_MenuBar1[i] && m_MenuBar2[i] && m_MenuBar3[i] && m_MenuText[i] )
		{
			m_MenuBar1[i]->Show();
			m_MenuBar2[i]->Show();
			m_MenuBar3[i]->Show();
			m_MenuText[i]->Show();

			if( m_MenuAnim[i] )
				m_MenuAnim[i]->Play( false );
		}
	}

	m_Active = true;
}

//=============================================================================
// GameMenu::DeactivateMenu()
//=============================================================================

void GameMenu::DeactivateMenu( void )
{
	for( int i = 0; i < MAX_MENU_BARS; i++ )
	{
		m_MenuBar1[i]->Hide();
		m_MenuBar2[i]->Hide();
		m_MenuBar3[i]->Hide();
		m_MenuText[i]->Hide();
	}

	m_MenuDock.Update();

	m_Active = false;
}

//=============================================================================
// GameMenu::CursorLeft()
//=============================================================================

void GameMenu::Select( BaseMenu *pMenu )
{
	if( m_pActiveMenu != (BaseMenu *) pMenu->m_pParent )
	{
		for( int i = 0; i < MAX_MENU_BARS; i++ )
			m_MenuAnim[i]->Play( false );
	}

	BaseMenu::Select( pMenu );
}

void GameMenu::Click( void )
{
	BaseMenu::Click();

	// If the user clicks this menu, and there are no children, and there are callbacks
	// defined, then hide the menu.

//	if( !Child() && IsCommandMenu() )
//		DeactivateMenu();

	return;
}

//=============================================================================
// DebugMenu::DebugMenu()
//=============================================================================

DebugMenu *DebugMenu::m_pDebugMenu = 0;

DebugMenu::DebugMenu()
{
	if( m_pDebugMenu == 0 )
		m_pDebugMenu = this;

	m_DebugMode = false;

	m_Range.lo = 0.0f;
	m_Range.hi = 1.0f;
	m_Range.step = 0.0f;

	m_str.pWatch = 0;
	m_ubyte.pWatch = 0;
	m_sbyte.pWatch = 0;
	m_uword.pWatch = 0;
	m_sword.pWatch = 0;
	m_ulong.pWatch = 0;
	m_slong.pWatch = 0;
	m_real.pWatch = 0;

	m_str.Column = -1;
	m_ubyte.Column = -1;
	m_sbyte.Column = -1;
	m_uword.Column = -1;
	m_sword.Column = -1;
	m_ulong.Column = -1;
	m_slong.Column = -1;
	m_real.Column = -1;
}

//=============================================================================
// DebugMenu::~DebugMenu()
//=============================================================================

DebugMenu::~DebugMenu()
{
	if( m_pDebugMenu == this )
		m_pDebugMenu = 0;

	return;
}

//=============================================================================
// DebugMenu::SetRange()
//=============================================================================

void DebugMenu::SetRange( float RangeLo, float RangeHi )
{
	if( m_pNewestMenu )
	{
		((DebugMenu *)m_pNewestMenu)->m_Range.lo = RangeLo;
		((DebugMenu *)m_pNewestMenu)->m_Range.hi = RangeHi;
	}

	return;
}

//=============================================================================
// DebugMenu::SetStep()
//=============================================================================

void DebugMenu::SetStep( float Step )
{
	if( m_pNewestMenu )
		((DebugMenu *)m_pNewestMenu)->m_Range.step = Step;

	return;
}

//=============================================================================
// DebugMenu::CursorUp() - TODO: This is just a stub for EDIT mode
//=============================================================================

void DebugMenu::CursorUp( void )
{
	if( !m_DebugMode )
		GameMenu::CursorUp();

	return;
}

//=============================================================================
// DebugMenu::CursorDown() - TODO: This is just a stub for EDIT mode
//=============================================================================

void DebugMenu::CursorDown( void )
{
	if( !m_DebugMode )
		GameMenu::CursorDown();

	return;
}

//=============================================================================
// DebugMenu::CursorLeft() - TODO: This is just a stub for EDIT mode
//=============================================================================

void DebugMenu::CursorLeft( void )
{
	GameMenu *pMenu = (GameMenu *) Selection();

	if( !pMenu || !m_DebugMode )
		GameMenu::CursorLeft();
	else
	{
		DebugMenu* pDebug = (DebugMenu *)pMenu;

		float Step = pDebug->m_Range.step;

		if( Step == 0.0f )
			Step = (pDebug->m_Range.hi - pDebug->m_Range.lo) / 20.0f;

		float Value = ( float )atof( pMenu->GetText(1) ) - Step;

		if( Value < pDebug->m_Range.lo )
			Value = pDebug->m_Range.lo;

		pMenu->SetText( 1, Printf( "%.4f", Value ) );
	}

	return;
}

//=============================================================================
// DebugMenu::CursorRight() - TODO: This is just a stub for EDIT mode
//=============================================================================

void DebugMenu::CursorRight( void )
{
	GameMenu *pMenu = (GameMenu *) Selection();

	if( !pMenu || !m_DebugMode )
		GameMenu::CursorRight();
	else
	{
		DebugMenu* pDebug = (DebugMenu *)pMenu;

		float Step = pDebug->m_Range.step;

		if( Step == 0.0f )
			Step = (pDebug->m_Range.hi - pDebug->m_Range.lo) / 20.0f;

		float Value = ( float )atof( pMenu->GetText(1) ) + Step;

		if( Value > pDebug->m_Range.hi )
			Value = pDebug->m_Range.hi;

		pMenu->SetText( 1, Printf( "%.4f", Value ) );

		BaseMenu::Click();
	}

	return;
}

//=============================================================================
// DebugMenu::CursorEnter() - TODO: This is just a stub for EDIT mode
//=============================================================================

void DebugMenu::CursorEnter( void )
{
	GameMenu *pMenu = (GameMenu *) Selection();

	if( pMenu && pMenu->GetText(1) )
		m_DebugMode = !m_DebugMode;
	else
	{
		GameMenu::CursorEnter();
	}

	return;
}

//=============================================================================
// DebugMenu::CursorExit() - TODO: This is just a stub for EDIT mode
//=============================================================================

void DebugMenu::CursorExit( void )
{
	GameMenu::CursorExit();

	m_DebugMode = false;
}

void DebugMenu::DeactivateMenu( void )
{
	GameMenu::DeactivateMenu();

	m_DebugMode = false;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName )
{
	return GameMenu::AddMenuItem( pName ); 
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName, char *pCommand )
{
	return GameMenu::AddMenuItem( pName, pCommand ); 
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName, int Command, MENUHANDLER Handler, PARAM Param )
{
	return GameMenu::AddMenuItem( pName, Handler, Command, Param );
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName, STATICCALLBACKVOID CallBack )
{
	return GameMenu::AddMenuItem( pName, CallBack ); 
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName, STATICCALLBACK CallBack, PARAM Param )
{
	return GameMenu::AddMenuItem( pName, CallBack, Param ); 
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName, void *pObject, MEMBERCALLBACKVOID CallBack )
{
	return GameMenu::AddMenuItem( pName, pObject, CallBack ); 
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

BaseMenu *DebugMenu::AddMenuItem( char *pName, void *pObject, MEMBERCALLBACK CallBack, PARAM Param )
{
	return GameMenu::AddMenuItem( pName, pObject, CallBack, Param ); 
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, char **pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, unsigned char *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, signed char *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, unsigned short *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, signed short *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, unsigned int *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, signed int *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::AddMenuItem()
//=============================================================================

DebugMenu *DebugMenu::AddMenuItem( char *pName, float *pWatch )
{
	DebugMenu *pMenu = (DebugMenu *) GameMenu::AddMenuItem( pName );

	if( pMenu )
		pMenu->Watch( 1, pWatch );

	return pMenu;
}

//=============================================================================
// DebugMenu::Watch() - pWatch a string
//=============================================================================

void DebugMenu::Watch( int Column, char **str )
{
	m_str.pWatch = str;
	m_str.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch an unsigned byte (8-bits)
//=============================================================================

void DebugMenu::Watch( int Column, unsigned char *pWatch )
{
	m_ubyte.pWatch = pWatch;
	m_ubyte.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch a signed byte (8-bits)
//=============================================================================

void DebugMenu::Watch( int Column, signed char *pWatch )
{
	m_sbyte.pWatch = pWatch;
	m_sbyte.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch an unsigned word (16-bits)
//=============================================================================

void DebugMenu::Watch( int Column, unsigned short *pWatch )
{
	m_uword.pWatch = pWatch;
	m_uword.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch a signed word (16-bits)
//=============================================================================

void DebugMenu::Watch( int Column, signed short *pWatch )
{
	m_sword.pWatch = pWatch;
	m_sword.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch an unsigned long (32-bits)
//=============================================================================

void DebugMenu::Watch( int Column, unsigned int *pWatch )
{
	m_ulong.pWatch = pWatch;
	m_ulong.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch a signed long (32-bits)
//=============================================================================

void DebugMenu::Watch( int Column, signed int *pWatch )
{
	m_slong.pWatch = pWatch;
	m_slong.Column = Column;
}

//=============================================================================
// DebugMenu::Watch() - pWatch a floating point number (32-bits)
//=============================================================================

void DebugMenu::Watch( int Column, float *pWatch )
{
	m_real.pWatch = pWatch;
	m_real.Column = Column;
}

//=============================================================================
// DebugMenu::SetMenuText()
//=============================================================================

void DebugMenu::SetText( int Column, char *pText )
{
	GameMenu::SetText( Column, pText );

	if( m_ubyte.Column == Column )
		*m_ubyte.pWatch = (unsigned char) atoi( pText );

	if( m_sbyte.Column == Column )
		*m_sbyte.pWatch = (signed char) atoi( pText );

	if( m_uword.Column == Column )
		*m_uword.pWatch = (unsigned short) atoi( pText );

	if( m_sword.Column == Column )
		*m_sword.pWatch = (signed short) atoi( pText );

	if( m_ulong.Column == Column )
		*m_ulong.pWatch = (unsigned int) atoi( pText );

	if( m_slong.Column == Column )
		*m_slong.pWatch = (signed int) atoi( pText );

	if( m_real.Column == Column )
		*m_real.pWatch = (float) atof( pText );

	return;
}

//=============================================================================
// DebugMenu::GetText()
//=============================================================================

char *DebugMenu::GetText( int Column )
{
	if( m_ubyte.Column == Column && m_ubyte.pWatch )
		return Printf( "%d", (int) *m_ubyte.pWatch );

	if( m_sbyte.Column == Column && m_sbyte.pWatch )
		return Printf( "%d", (int) *m_sbyte.pWatch );

	if( m_uword.Column == Column && m_uword.pWatch  )
		return Printf( "%d", (int) *m_uword.pWatch );

	if( m_sword.Column == Column && m_sword.pWatch  )
		return Printf( "%d", (int) *m_sword.pWatch );

	if( m_ulong.Column == Column && m_ulong.pWatch  )
		return Printf( "%d", (int) *m_ulong.pWatch );

	if( m_slong.Column == Column && m_slong.pWatch  )
		return Printf( "%d", (int) *m_slong.pWatch );

	if( m_real.Column == Column && m_real.pWatch  )
		return Printf( "%0.4f", (float) *m_real.pWatch );

	return GameMenu::GetText( Column );
}

DebugMenu *GetDebugMenu( void )
{
	return DebugMenu::m_pDebugMenu;
}