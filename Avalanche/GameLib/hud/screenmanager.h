#ifndef _SCREENMANAGER_H_
#define _SCREENMANAGER_H_

#include "hud/sprite.h"
#include "hud/text.h"
#include "hud/screencontrols.h"
#ifdef DIRECTX_PC
#include "platform/Win32/wininput.h"
#endif
class Screen;
class Button;
class ScreenManager;
class ScreenHandler;
class ScreenControl;

//=============================================================================
// UIHandler
//=============================================================================

class UIHandler
{
	public:

		enum DIGITALBUTTON
		{
			CURSOR_UP,
			CURSOR_DOWN,
			CURSOR_LEFT,
			CURSOR_RIGHT,
			BUTTON_OK,
			BUTTON_CANCEL,
			BUTTON_START,
			BUTTON_SELECT,
		};

		virtual void RefreshInput( void ) = 0;
		virtual bool ButtonPressed( DIGITALBUTTON Button )	= 0;
		virtual bool ButtonDown( DIGITALBUTTON Button ) = 0;
		virtual bool DeviceButtonPressed( int Button ) = 0;
		virtual bool DeviceButtonReleased( int Button ) = 0;
		virtual bool DeviceButtonDown( int Button ) = 0;
		virtual void PlaySound( const char* pSound ) = 0;
};

//=============================================================================
// ScreenStack
//
//		This class is used to track screen flow as the user navigates through the
//		front end.
//=============================================================================

class ScreenStack : public TreeNode
{
	DECLARE_TREENODE( ScreenStack );

	public:
		
		// The default constructor just sets the screen param to 0
		ScreenStack( void )
			{ SetScreen(0); }

		// This constructor initializes the screen param
		ScreenStack( Screen* pScreen )
			{ SetScreen(pScreen); }

		// Set the screen
		void SetScreen( Screen* pScreen )
			{ m_pScreen = pScreen; }

		// Get the screen
		Screen*	GetScreen( void )
			{ return m_pScreen; }

		// Get the screen handler
		ScreenHandler* GetScreenHandler( void );

	protected:
		Screen* m_pScreen;
};

//=============================================================================
// ScreenManager
//
//		This is the heart of the front end.  Game-specific front end is usually
//		derived from this class.
//=============================================================================

class ScreenManager : public TreeNode
{
	public:
		ScreenManager( void );
		virtual ~ScreenManager();

		//=======================================================================
		// Initialization
		//
		//		Initialization and shutdown functions.
		//=======================================================================
		
		bool Init( void );		
		void Shutdown( void );

		//=======================================================================
		// UIHandler
		//=======================================================================

		void SetUIHandler( UIHandler* pUIHandler );
		UIHandler* GetUIHandler( void );
		void RefreshInput( void );
		void PlaySound( const char* pSound );

		//=======================================================================
		// Screen interface
		//
		//		Screens are created using the Windows front-end tool and saved as
		//		files to the hard drive (or stored on the cd-rom/dvd).
		//=======================================================================

		bool LoadScreen( const char* pFileName );
		bool LoadScreen( MemoryBank &m );
		bool LoadSprites( MemoryBank &m, Sprite *pParent );
		void UpdateScreens( void );
		void UpdateScreen( const char* pScreenName );
		void UpdateScreen( Screen* pScreen );
		void RenderScreen( const char* pScreenName );
		void RenderScreen( Screen* pScreen );
		void SetTime( float Time );
	
		//////////////////////////////////////////////////////////////////////////

		ScreenHandler* GetScreenHandler( const char* pScreenName, const char* pClassName );
		ScreenHandler* GetScreenHandler( const char* pScreenName );
		ScreenHandler* GetScreenHandler( Screen* pScreen );
		void SetScreenHandler( const char* pScreenName, const char* pScreenHandler );
		Screen* GetScreen( const char* pScreenName );
		Screen* GetCurrentScreen( void );
		ScreenHandler* GetCurrentScreenHandler( void );
		Screen* GetActiveScreen( void );
		ScreenHandler* GetActiveScreenHandler( void );
		bool IsScreenAlreadyActive( const char* pScreenName );
		bool IsScreenAlreadyActive( Screen* pScreen );
		void SetCurrentScreen( const char* pScreenName );
		void SetCurrentScreen( Screen* pScreen );
		void SetActivePopup( const char* pScreenName );
		void SetActivePopup( Screen* pScreen );

		//=======================================================================
		// Screen navigation
		//
		//		These functions control screen flow and navigation.
		//=======================================================================

		bool ShowScreen( const char* pScreenName );
		bool ShowScreen( Screen* pScreen );
		bool HideScreen( const char* pScreenName );
		bool HideScreen( Screen* pScreen );

		Screen* PopScreenHistory( void );
		Screen* PopScreenHistory( Screen* pScreen );
		Screen* PopScreenHistory( const char* pScreenName );
		ScreenStack* GetScreenStack( void );
		virtual bool GotoScreen( const char* pScreenName );
		virtual bool GotoScreenPtr( Screen* pScreen );
		bool ShowPopup( const char* pScreenName );
		bool ShowPopup( Screen* pScreen );
		void RemoveActivePopup( void );
		void RemoveAllPopups( void );
		void ClosePopup( const char* pScreenName );
		void ClosePopup( Screen* pPopupScreen );
		void EndScreen( int Result );
		void EndPopup( int Result );

		void GotoPrevScreen( void );
		void ClearScreenHistory( void );

		bool IsActiveScreenReadyToExit( void );
		bool IsScreenReadyForInput( void );

	protected:
		void AddScreen( Screen* pScreen, const char* pScreenName );
		void DestroyScene( Sprite *pSprite );

	public:
		Screen* m_pScreens;
		UIHandler* m_pUIHandler;
		ScreenStack m_ScreenStack;
		Screen* m_pCurrentScreen;
		Screen* m_pNextScreen;
		Sprite m_SpriteTree;
		float m_NextScreenTime;
		float m_AllowInputTime;
		float m_CurrentTime;
		float m_PastTime;
		bool m_NextScreenQueued;
		int m_ScreenKey;
};

class Screen : public SpriteInterface
{
	DECLARE_CLASSOBJECT( Screen );

	typedef enum
	{
		SOUNDEVENT_NONE,
		SOUNDEVENT_INVALID,
		SOUNDEVENT_DISABLED,
		SOUNDEVENT_SELECT,
		SOUNDEVENT_NEXTBUTTON,
		SOUNDEVENT_PREVBUTTON,
		SOUNDEVENT_SHOWSCREEN,
		SOUNDEVENT_PREVSCREEN,
		SOUNDEVENT_SHOWPOPUP,
		SOUNDEVENT_HIDEPOPUP,
		SOUNDEVENT_MAX,
	}
	SOUNDEVENT;

	typedef struct
	{
		u32 Version;
	}
	SaveStruct;

	public:
		Screen( void );
		virtual ~Screen( void );

		//////////////////////////////////////////////////////////////////////////
		
		void SetScreenName( const char* pScreenName );
		void SetScreenManager( ScreenManager* pScreenManager );
		ScreenManager* GetScreenManager( void );
		ScreenHandler* GetScreenHandler( void );
		void InitScreenHandler( void );
		void SetScreenHandlerClass( const char* pScreenHandlerClass );
		const char* GetScreenHandlerClass( void );
		void RemoveScreenHandler( void );
		bool InitScreen( void );
		void PlaySound( const char* pSound );

		//////////////////////////////////////////////////////////////////////////
		
		Sprite* GetSprite( int SpriteId );
		Sprite* GetSprite( int SpriteId, Sprite* pParent );
		Text* GetTextSprite( int SpriteId );
		void SetText( int TextId, const char* pText );
		void Printf( int TextId, const char *fmt, ... );
		SpriteInterface* GetInterface( int SpriteId, const char* pInterfaceName );
		SpriteInterface* AddInterface( int SpriteId, const char* pInterfaceName );
		SpriteInterface* AddInterface( Sprite* pSprite, const char* pInterfaceName );
		void ShowSprite( int SpriteId, bool Toggle );

		//////////////////////////////////////////////////////////////////////////

		void Show( void );
		void Hide( void );
		bool IsVisible( void );
		bool IsHidden( void );

		//////////////////////////////////////////////////////////////////////////

		virtual void HandleInput( UIHandler* pUIHandler );
		virtual bool Init( void* Param );

		//////////////////////////////////////////////////////////////////////////

		Screen* GetScreen( const char* pScreenName );
		bool GotoScreen( const char* pScreenName );
		void GotoPrevScreen( void );
		void ShowPopup( Screen* pScreen );
		void RemoveActivePopup( void );
		void RemoveAllPopups( void );
		void RemoveCurrentPopup( void );
		Screen* GetActivePopup( void );
		Screen* GetCurrentPopup( void );
		Button* GetButton( int ButtonId );
		void SetInitialButton( int ButtonId );
		void SetInitialButton( Button* pButton );
		Button* GetInitialButton( void );
		int GetInitialButtonId( void );
		void SetNextButton( int ButtonId );
		Button* GetLastButton( void );
		Button* GetCurrentButton( void );
		int GetCurrentButtonId( void );
		int GetCurrentButtonIndex( void );
		Button* GetNthButton( int n );
		int GetNthButtonId( int n );
		void SelectButton( int ButtonId );
		void SelectButton( Button* pNewButton );
		void SelectInitialButton( void );
		void SelectNextButton( void );
		void SelectPrevButton( void );
		void SelectNthButton( int n );
		void UnselectButton( void );
		Button* GetDefaultButton( void );
		void SetDefaultButton( int ButtonId );
		void SetDefaultButton( Button* pButton );
		void SelectDefaultButton( void );
		void CycleButton( int N );
		void CyclePrevButton( void );
		void CycleNextButton( void );
		void EnableButton( int ButtonId, bool Enabled );
		bool ButtonEnabled( int ButtonId );
		void PressButton( void );
		void PressCancel( void );

		void SetNextScreen( int ButtonId, const char* pScreenName );
		void SetNextScreen( Button* pButton, const char* pScreenName );
		const char* GetNextScreen( int ButtonId );
		const char* GetNextScreen( Button* pButton );

		void SetCursor( int SpriteId );
		void SetCursor( Sprite* pSprite );
		void SetCursorPos( float x, float y );
		void SetCursorScreenPos( float x, float y );
		Sprite* GetCursor( void );
		void SetCursorOffset( float x, float y );
		void GetCursorOffset( float &x, float &y );

		void SetSoundEvent( SOUNDEVENT SoundEvent, const char* pSound );
		const char* GetSoundEvent( SOUNDEVENT SoundEvent );
		void PlaySoundEvent( SOUNDEVENT SoundEvent );
		bool ValidSoundEvent( SOUNDEVENT SoundEvent );

	protected:
		virtual bool Save( MemoryBank& m );
		virtual bool Load( MemoryBank& m );
		virtual void OnUpdate( void );
		
	public:
		xstring m_ScreenName;
		xstring m_ScreenHandlerClass;
		ScreenManager* m_pScreenManager;
		ScreenHandler* m_pScreenHandler;
		ScreenControl* m_pScreenControls;
		Screen* m_pNextScreen;
		Screen* m_pPrevScreen;
		Screen* m_pPopupOwner;
		Screen* m_pCurrentPopup;
		bool m_visible;

		Button* m_pInitialButton;
		Button* m_pDefaultButton;
		Button* m_pCurrentButton;
		Sprite* m_pCursor;
		Vector2 m_CursorVec;
		Vector2 m_CursorPos;
		int m_ScreenPriority;
		int m_ScreenKey;
		char m_SoundEvent[SOUNDEVENT_MAX][64];
};

typedef struct
{
	char szNextScreen[64];
	char szActivateSound[64];
}
ButtonParamStruct;

class Button : public SpriteInterface
{
	DECLARE_CLASSOBJECT( Button );

	public:
		Button( void );
		int GetButtonId( void );
		void Select( void );
		void Unselect( void );
		void Enable( bool Enabled );
		bool Enabled( void );

		void SetNextScreen( const char* pScreenName );
		const char* GetNextScreen( void );
		void SetNextButton( Button* pNextButton );
		void SetPrevButton( Button* pPrevButton );
		Button* GetNextButton( void );
		Button* GetPrevButton( void );

		void SetParam( void* pParam );
		void* GetParam( void );
	
	public:
		virtual bool Init( void* Param );
		virtual void OnUpdate( void );
		
	public:
		ButtonParamStruct m_Param;
		SPRITESTATESTRUCT m_InitialState;
		Button* m_pNextButton;
		Button* m_pPrevButton;
		void* m_pParam;
		bool m_Selected;
		bool m_Enabled;
};

class ScreenHandler : public TreeNode
{
	DECLARE_CLASSOBJECT( ScreenHandler );
	DECLARE_CLASSFACTORY( ScreenHandler );

	public:
		ScreenHandler( void );

		virtual void PlaySound( const char* pSound );
		virtual void Show( void );
		virtual void Hide( void );
		virtual void HandleInput( UIHandler* pUIHandler );

		//=======================================================================
		// Buttons
		//=======================================================================
		// These functions are provided to help working with buttons.
		//=======================================================================

		Button* GetButton( int ButtonId );
		void SetInitialButton( int ButtonId );
		void SetInitialButton( Button* pButton );
		Button* GetInitialButton( void );
		int GetInitialButtonId( void );
		void SetNextButton( int ButtonId );
		Button* GetLastButton( void );
		Button* GetCurrentButton( void );
		int GetCurrentButtonId( void );
		int GetCurrentButtonIndex( void );
		Button* GetNthButton( int n );
		int GetNthButtonId( int n );
		void SelectButton( int ButtonId );
		void SelectButton( Button* pButton );
		void SelectInitialButton( void );
		void SelectNextButton( void );
		void SelectPrevButton( void );
		void SelectNthButton( int n );
		void CycleButton( int N );
		void CyclePrevButton( void );
		void CycleNextButton( void );
		void UnselectButton( void );
		Button* GetDefaultButton( void );
		void SetDefaultButton( int ButtonId );
		void SetDefaultButton( Button* pButton );
		void SelectDefaultButton( void );
		void EnableButton( int ButtonId, bool Enabled );
		bool ButtonEnabled( int ButtonId );
		void PressButton( void );
		void PressCancel( void );
		void SetButtonParam( int ButtonId, void* pParam );
		void* GetButtonParam( int ButtonId );

		void SetNextScreen( int ButtonId, const char* pScreenName );
		void SetNextScreen( Button* pButton, const char* pScreenName );
		const char* GetNextScreen( int ButtonId );
		const char* GetNextScreen( Button* pButton );

		void SetActive( bool Active );
		bool GetActive( void );
		bool GetVisible( void );

		void AllowInput( bool Allowed );
		bool IsInputAllowed( void );

#ifdef DIRECTX_PC
		// virtual accessors
		virtual GuiListBox* GetListBox() { return NULL; }
#endif
		//=======================================================================
		// Cursor functions
		//=======================================================================
		// These functions are provided to help working with cursors.
		//=======================================================================
		
		void SetCursor( int SpriteId );
		void SetCursor( Sprite* pSprite );
		void SetCursorPos( float x, float y );
		void SetCursorScreenPos( float x, float y );
		Sprite* GetCursor( void );
		void SetCursorOffset( float x, float y );
		void GetCursorOffset( float &x, float &y );
#ifdef DIRECTX_PC
		void SelectMenuItem();
#endif

		void SetSoundEvent( Screen::SOUNDEVENT SoundEvent, const char* pSound );
		const char* GetSoundEvent( Screen::SOUNDEVENT SoundEvent );
		void PlaySoundEvent( Screen::SOUNDEVENT SoundEvent );

		//=======================================================================
		// Event Handlers
		//=======================================================================
		// These functions are called by the framework when the corresponding
		// events occur.  Override them to implement custom behavior.
		//=======================================================================

		virtual void OnInitialize( void );
		virtual void OnTerminate( void );
		virtual void OnCommand( int Command );
		virtual void OnInvalidAction( void );
		virtual void OnCursorUp( void );
		virtual void OnCursorDown( void );
		virtual void OnCursorLeft( void );
		virtual void OnCursorRight( void );
		virtual void OnButtonOK( void );
		virtual void OnButtonCancel( void );
		virtual void OnButtonStart( void );
		virtual void OnButtonSelect( void );
		virtual void OnNextButton( void );
		virtual void OnPrevButton( void );
		virtual void OnNextScreen( Screen* pNextScreen );
		virtual void OnPrevScreen( Screen* pPrevScreen );
		virtual void OnScreenIntro( void );
		virtual void OnScreenExit( void );
		virtual void OnScreenUpdate( void );
		virtual void OnScreenRefresh( void );
		virtual void OnPopupShow( void );
		virtual void OnPopupHide( void );
		virtual void OnButtonUnselect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonSelect( Button* pButton, Sprite* pSprite );
		virtual void OnButtonEnabled( Button* pButton, Sprite* pSprite );
		virtual void OnButtonDisabled( Button* pButton, Sprite* pSprite );
		virtual void OnButtonCycle( int ButtonId, GuiCycleButton* pCycleButton, Sprite *pSprite );
		virtual void OnScreenMessage( ScreenHandler* pScreenHandler, int message, void* pParam );
		virtual void OnGuiMessage( GuiControl* pControl, int message, void* pParam );

		// These functions return information about the screen handler

		virtual float GetScreenIntroTime( void ) { return 0.0f; }
		virtual float GetScreenExitTime( void )  { return 0.0f; }
		virtual bool IsScreenReadyToExit( void )  { return true; }

		// When working with sprites, these functions will help you find them.
	
		Sprite* GetScreenSprite( void );
		Sprite* GetSprite( int SpriteId );
		Sprite* GetSprite( int SpriteId, Sprite* pParent );
		Text* GetTextSprite( int SpriteId );
		Text* GetTextSprite( int SpriteId, Sprite* pParent );

		// Interfaces

		SpriteInterface* GetInterface( int SpriteId, const char* pInterfaceName );
		SpriteInterface* AddInterface( int SpriteId, const char* pInterfaceName );
		SpriteInterface* AddInterface( Sprite* pSprite, const char* pInterfaceName );

		// Screens

		Screen* GetScreen( const char* pScreenName );
		ScreenHandler* GetScreenHandler( const char* pScreenName, const char* pClassName );
		ScreenHandler* GetScreenHandler( const char* pScreenName );
		ScreenHandler* GetScreenHandler( Screen* pScreen );
		ScreenManager* GetScreenManager( void );
		UIHandler* GetUIHandler( void );
		int ShowPopup( const char* pScreenName );
		bool GotoScreen( const char* pScreenName );
		void GotoPrevScreen( void );
		void EndScreen( int Result );

		// Helper functions

		void SetTexture( int SpriteId, t_Handle Handle, int Texture );
		void SetText( int TextId, const char* pText );
		void SetText( int TextId, int ParentId, const char* pText );
		void SetText( int TextId, Sprite* pParent, const char* pText );
		void Printf( int TextId, const char* fmt, ... );

		void ShowSprite( int SpriteId, bool Show );

	public:
		Screen* m_pScreen;
		bool m_Active;
		bool m_InputAllowed;
};

// a PC mouse based cursor for menu navigation
// 
#ifdef DIRECTX_PC
#define NUM_MOUSE_BUTTONS 2
class MenuCursor 
{
	public:
		MenuCursor() : xPos(0), yPos(0), pSprite(NULL), pInput(NULL),
					   maxWindowX(10000), maxWindowY(10000) {} 
		~MenuCursor();

		bool Initialize(Sprite* inSpritePtr, SystemInputDriver* inInputPtr);
		bool GetRect(float &tl, float &tr, float &bl, float &br);
		bool Update();
		bool IsActive() { return active; }
		bool CursorButtonPressed(int button) {  ASSERT(button < NUM_MOUSE_BUTTONS); 
												return (buttonDownLast[button] && !buttonDownNow[button]); }
		float GetX() { return xPos; }
		float GetY() { return yPos; }
		Sprite* GetSpritePtr() { return pSprite; }
		void SetMaxXandY(float x, float y);
		void SetHidden( bool hide ) { hidden = hide; hide ? pSprite->Hide() : pSprite->Show(); }
		void ForceButtonUp(int button) { ASSERT(button < NUM_MOUSE_BUTTONS); 
										 buttonDownNow[button] = buttonDownLast[button] = false; }
		int GetMouseWheelState() { return mouseWheelState; }

	private:
		Sprite* pSprite;
		WindowsSystemInputDriver* pInput;
		float xPos, yPos;
		float maxWindowX, maxWindowY;
		bool active;
		bool isInitialized();
		bool buttonDownNow[NUM_MOUSE_BUTTONS];
		bool buttonDownLast[NUM_MOUSE_BUTTONS];
		bool hidden;
		int	 mouseWheelState;

};

extern MenuCursor g_MenuCursor;
#endif

#endif // _SCREENMANAGER_H_
