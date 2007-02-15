//=============================================================================
// Every game will have a highly specialized menu, therefor it is impossible to
// design a class that can anticipate every need.  The purpose of these classes
// is to give programmers the tools they need to easily create and modify their
// own menus quickly without limiting creative freedom.
//=============================================================================

#ifndef __NEWMENU_H__
#define __NEWMENU_H__

#include "hud/stringex.h"
#include "hud/sprite.h"
#include "hud/text.h"
#include "hud/callback.h"

//=============================================================================
// BaseMenu
//=============================================================================
//
// BaseMenu is the starting point for all menus.  It is designed to shield the
// programmer from the details of menu management and navigation, allowing him
// to focus on appearance and functionality of the menu.
//
//=============================================================================
//
// To create your own menu, derive your class from BaseMenu and add a function
// to draw it:
//
//		class MyMenu : public BaseMenu
//		{
//			public:
//				void DrawMenu( void );
//
//			private:
//				Sprite m_Cursor;				// In this example, the menu has a cursor next to the selected item
//				Text m_MenuTitle;
//				Text m_MenuText[8];			// and can display up to 8 menu items at a time.
//		};
//
// Next, implement the drawing code.  This could be anything from Windows GDI
// calls to 3D API calls or whatever you are required to use.  This example uses
// 3D API calls wrapped by 2D classes, Sprite and Text (part of the Hud library).
//
//		void MyMenu::DrawMenu( void )
//		{
//			MyMenu *pMenu = (MyMenu *) ActiveMenu();
//
//			if( pMenu )
//			{
//				int Items = pMenu->GetNumChildren();
//
//				m_MenuTitle.SetText( pMenu->GetText() );
//				m_MenuTitle.SetPosition( 320.0f, 100.0f );
//				m_MenuTitle.SetOrigin( 0.5f, 0.0f );
//
//				pMenu = (MyMenu *) pMenu->GetChild();
//
//				for( int i = 0; i < 8; i++ )
//				{
//					if( i < Items )
//					{
//						m_MenuText[i].Show();
//						m_MenuText[i].SetText( pMenu->GetText() );
//						m_MenuText[i].SetTextHeight( 24.0f );
//						m_MenuText[i].SetColor( 192, 192, 255, 255 );		
//						m_MenuText[i].SetParent( &m_MenuTitle );
//
//						if( pMenu == (MyMenu *) Selection() )
//						{
//							m_MenuText[i].SetScale( 1.2f, 1.2f );
//							m_MenuText[i].SetColor( 255, 255, 255, 255 );
//							m_Cursor.SetPosition( -32.0f, 0.0f );
//							m_Cursor.SetParent( &m_MenuText[i] );
//							m_Cursor.Show();
//						}
//	
//						m_MenuText[i].SetParent( &m_MenuTitle );
//					}
//					else
//					{
//						m_MenuText[i].Hide();
//					}
//				}
//			
//				m_MenuTitle.Draw();
//			}
//
//			return;
//		}
//
//	Finally, adding items to the menu couldn't be easier.
//
//		MyMenu Menu;
//		
//		Menu.AddMenuItem( "Single Player" );
//		Menu.Branch();
//		Menu.AddMenuItem( "Single Race" );
//		Menu.AddMenuItem( "Championship" );
//		Menu.AddMenuItem( "Time Trial" );
//		Menu.AddMenuItem( "Practice" );
//		Menu.Return();
//		Menu.AddMenuItem( "Multiplayer" );
//		Menu.Branch();
//		Menu.AddMenuItem( "Battle" );
//		Menu.AddMenuItem( "Team Game" );
//		Menu.AddMenuItem( "High Stakes" );
//		Menu.Return();
//		Menu.AddMenuItem( "Options" );
//		Menu.AddMenuItem( "Quit" );
//
//=============================================================================
// !!! PLEASE DO NOT ADD GAME-SPECIFIC FUNCTIONALITY TO BASE MENU !!!
// !!! PLEASE DO NOT ADD GAME-SPECIFIC FUNCTIONALITY TO BASE MENU !!!
// !!! PLEASE DO NOT ADD GAME-SPECIFIC FUNCTIONALITY TO BASE MENU !!!
//=============================================================================
#define MAX_MENU_BARS				18

typedef void (*MENUHANDLER)( int Command, PARAM Param );

inline void CallMenuHandler( MENUHANDLER CallBack, int Command, PARAM Param )
{
	if( CallBack )
		CallBack( Command, Param );

	return;
}

class BaseMenu : public HudObject
{
	DECLARE_CLASSOBJECT( BaseMenu );

	public:
		BaseMenu();
		virtual ~BaseMenu();

		static BaseMenu *GetMenuWhoCalledBack( void );

		virtual void Release( void );

		virtual BaseMenu *AddMenuItem( char *pName );
		virtual BaseMenu *AddMenuItem( char *pName, char *pCommand );
		virtual BaseMenu *AddMenuItem( char *pName, MENUHANDLER Handler, int Command, PARAM Param );
		virtual BaseMenu *AddMenuItem( char *pName, STATICCALLBACKVOID CallBack );
		virtual BaseMenu *AddMenuItem( char *pName, STATICCALLBACK CallBack, PARAM Param );
		virtual BaseMenu *AddMenuItem( char *pName, void *pObject, MEMBERCALLBACKVOID CallBack );
		virtual BaseMenu *AddMenuItem( char *pName, void *pObject, MEMBERCALLBACK CallBack, PARAM Param );

		virtual void SetCommand( char *pCommand );
		virtual void SetCommand( MENUHANDLER Handler, int Command, PARAM Param);

		virtual void SetCallBack( STATICCALLBACKVOID CallBack );
		virtual void SetCallBack( STATICCALLBACK CallBack, PARAM Param );
		virtual void SetCallBack( void *pObject, MEMBERCALLBACKVOID CallBack );
		virtual void SetCallBack( void *pObject, MEMBERCALLBACK CallBack, PARAM Param );
		virtual void ClearCallBack( void );

		virtual void ExecuteCallBack( void );

		virtual void Branch( void );
		virtual void Return( void );

		virtual void SetText( char *pText );
		virtual void SetText( int Column, char *pText );
		virtual char *GetText( int Column = 0 );
		virtual void ClearText( int Column = 0 );
		virtual void ClearAllText( void );
		virtual void Sort( int Column, bool Reversed );

		virtual void CursorUp( void );			// Called when the user pushes UP on the D-PAD
		virtual void CursorDown( void );			// Called when the user pushes DOWN on the D-PAD
		virtual void CursorLeft( void );			// Called when the user pushes LEFT on the D-PAD
		virtual void CursorRight( void );		// Called when the user pushes RIGHT on the D-PAD
		virtual void CursorEnter( void );		// Called when the user pushes the X button
		virtual void CursorExit( void );			// Called when the user pushes the Y button
		virtual void KeyboardInput( char Ch );	// Called when the user enters characters on the keyboard

		virtual void ReturnToTop( void );

		virtual void Click( void );				// Called when the user has clicked on the menu item
		virtual bool Clicked( void );				// Returns true if the user has clicked on this menu item, or false otherwise.
		
		virtual void Select( BaseMenu *pMenu );
		virtual BaseMenu *ActiveMenu( void );	// Get the active menu
		virtual BaseMenu *Selection( void );	// Get the current selection
		virtual BaseMenu *PrevSelection( void );
		virtual bool IsCommandMenu( void );		// Does this menu activate a function?

	public:
		int m_Clicks;
		xstring m_szText[8];
		xstring m_szCommand;

	protected:
		BaseMenu *m_pAddMenuTarget;
		BaseMenu *m_pNewestMenu;
		BaseMenu *m_pActiveMenu;					// The active menu
		BaseMenu *m_pSelection;						// The current selection
		BaseMenu *m_pPrevSelection;				// The previous selection

	private:
		static BaseMenu *m_pMenuWhoCalledBack;
		UnknownClass *m_pCallBackThisPointer;
		PARAM m_CallBackParam;
		STATICCALLBACK m_StaticCallBack;
		STATICCALLBACKVOID m_StaticCallBackVoid;
		MEMBERCALLBACK m_MemberCallBack;
		MEMBERCALLBACKVOID m_MemberCallBackVoid;
		MENUHANDLER m_CallBackHandler;
		int m_CallBackCommand;
};

BaseMenu *GetMenuWhoCalledBack( void );

class GameMenu : public BaseMenu
{
	DECLARE_CLASSOBJECT( GameMenu );

	public:
		GameMenu();
		virtual ~GameMenu();

		virtual t_Handle LoadTextureSet( char *pName );
		virtual t_Handle FreeTextureSet( void );						// returns INVALID_HANDLE for convienience
		virtual void SetTextureSet( t_Handle TextureSet );
		virtual t_Handle TextureSet( void );
		virtual void InitMenu( void );
		virtual void DrawMenu( void );
		virtual void ActivateMenu( void );
		virtual void DeactivateMenu( void );
		virtual void Select( BaseMenu *pMenu );
		virtual void Click( void );

	public:
		bool		m_DebugMode;
		Sprite	m_MenuDock;
		Sprite	*m_MenuBar1[MAX_MENU_BARS];
		Sprite	*m_MenuBar2[MAX_MENU_BARS];
		Sprite	*m_MenuBar3[MAX_MENU_BARS];
		Text		*m_MenuText[MAX_MENU_BARS];
		Animator	m_Blinker;
		Animator	*m_MenuAnim[MAX_MENU_BARS];
		Animator *m_FlashAnim[MAX_MENU_BARS];
		int		m_FlashTarget;
		bool		m_Active;
		GameMenu *m_pSelection;
		GameMenu *m_pPrevSelection;

	private:
		t_Handle m_TextureSet;
		t_Handle m_LoadedTextureSet;
};

//=============================================================================
// DebugMenu - TODO: Documentation!
//=============================================================================

class DebugMenu : public GameMenu
{
	DECLARE_CLASSOBJECT( DebugMenu );

	public:
		DebugMenu();
		virtual ~DebugMenu();

		void SetRange( float RangeLow, float RangeHi );
		void SetStep( float Step );
		
		virtual void CursorUp( void );
		virtual void CursorDown( void );
		virtual void CursorLeft( void );
		virtual void CursorRight( void );
		virtual void CursorEnter( void );
		virtual void CursorExit( void );

		virtual void DeactivateMenu( void );

		virtual BaseMenu *AddMenuItem( char *pName );
		virtual BaseMenu *AddMenuItem( char *pName, char *pCommand );
		virtual BaseMenu *AddMenuItem( char *pName, int Command, MENUHANDLER Hander, PARAM Param );
		virtual BaseMenu *AddMenuItem( char *pName, STATICCALLBACKVOID CallBack );
		virtual BaseMenu *AddMenuItem( char *pName, STATICCALLBACK CallBack, PARAM Param );
		virtual BaseMenu *AddMenuItem( char *pName, void *pObject, MEMBERCALLBACKVOID CallBack );
		virtual BaseMenu *AddMenuItem( char *pName, void *pObject, MEMBERCALLBACK CallBack, PARAM Param );

		virtual DebugMenu *AddMenuItem( char *pName, char **pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, unsigned char *pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, signed char *pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, unsigned short *pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, signed short *pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, unsigned int *pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, signed int *pWatch );
		virtual DebugMenu *AddMenuItem( char *pName, float *pWatch );

		virtual void Watch( int Column, char **str );
		virtual void Watch( int Column, unsigned char *pWatch );
		virtual void Watch( int Column, signed char *pWatch );
		virtual void Watch( int Column, unsigned short *pWatch );
		virtual void Watch( int Column, signed short *pWatch );
		virtual void Watch( int Column, unsigned int *pWatch );
		virtual void Watch( int Column, signed int *pWatch );
		virtual void Watch( int Column, float *pWatch );

		virtual void SetText( int Column, char *pText );
		virtual char *GetText( int Column );

	public:
		static DebugMenu *m_pDebugMenu;
		
		struct {
			float lo;
			float hi;
			float step;
		} m_Range;

		struct {
			char **pWatch;
			int Column;
		} m_str;
		struct {
			unsigned char *pWatch;
			int Column;
		} m_ubyte;
		struct {
			signed char *pWatch;
			int Column;
		} m_sbyte;
		struct {
			unsigned short *pWatch;
			int Column;
		} m_uword;
		struct {
			signed short *pWatch;
			int Column;
		} m_sword;
		struct {
			unsigned int *pWatch;
			int Column;
		} m_ulong;
		struct {
			signed int *pWatch;
			int Column;
		} m_slong;
		struct {
			float *pWatch;
			int Column;
		} m_real;
};

DebugMenu *GetDebugMenu( void );

#endif // __NEWMENU_H__