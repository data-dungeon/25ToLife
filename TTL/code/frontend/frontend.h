#ifndef _FRONTEND_H_
#define _FRONTEND_H_

#include "hud/hudlib.h"
#include "frontend/inputdriver.h"
#include "frontend/screens/screen_BasePopup.h"
#include "frontend/screens/screen_Invitations.h"
#include "frontend/screens/screen_Keyboard.h"
#include "frontend/screens/screen_MemCard.h"
#include "frontend/screens/screen_Popups.h"
#include "frontend/screens/screen_ScoreBoard.h"
#include "GameHelper/DebugOutput.h"

enum {
STARTUPCHECK_IDLE,
STARTUPCHECK_CHECK_CONTROLLER,
STARTUPCHECK_CHECK_CONTROLLER_RESULT,
STARTUPCHECK_GO,
STARTUPCHECK_RESULT,
};

#if defined(DIRECTX_PC)
#define SCREEN_PAUSEMENU "SCREEN_ID_PAUSE_PC"
#define SCREEN_MAINMENU  "SCREEN_ID_MAINMENU"
#elif defined(_XBOX)
#define SCREEN_PAUSEMENU "SCREEN_ID_PAUSE_XBOX"
#define SCREEN_MAINMENU  "SCREEN_ID_MAINMENU_XBOX"
#else
#define SCREEN_PAUSEMENU "SCREEN_ID_PAUSE"
#define SCREEN_MAINMENU  "SCREEN_ID_MAINMENU"
#endif

class FrontEndUIHandler : public UIHandler
{
	public:
		virtual void RefreshInput( void );
		virtual bool ButtonPressed( DIGITALBUTTON Button );
		virtual bool ButtonDown( DIGITALBUTTON Button );
		virtual bool DeviceButtonPressed( int Button );
		virtual bool DeviceButtonReleased( int Button );
		virtual bool DeviceButtonDown( int Button );
		virtual void PlaySound( const char* pSound );

		FluffControlMapper* GetMapper( void );
		int GetDeviceButton( DIGITALBUTTON Button );
};

class FrontEnd : public ScreenManager
{
	public:
		enum
		{
			GAMETYPE_UNDEFINED			= 1<<0,
			GAMETYPE_MAINMENU				= 1<<1,
			GAMETYPE_SINGLEPLAYERGAME  = 1<<2,
			GAMETYPE_NETWORKGAME			= 1<<3,
			GAMETYPE_ONLINEGAME			= 1<<4,
			GAMETYPE_CREDITS				= 1<<5,
			GAMETYPE_ANY					= 0xffffffff,
		};
		typedef int GAMETYPE;

		class ScreenFile : public TreeNode
		{
			DECLARE_TREENODE( ScreenFile );

			ScreenFile( void )
				{ m_GameType = GAMETYPE_UNDEFINED; }
				
			ScreenFile( const char* pFileName, GAMETYPE GameType )
				{ m_FileName.set(pFileName); m_GameType = GameType; }

		public:
			xstring m_FileName;
			GAMETYPE m_GameType;
		};

		typedef enum
		{
			SCREENTYPE_MAINMENU,
			SCREENTYPE_CHAREDITOR,
			SCREENTYPE_TAGEDITOR,
			SCREENTYPE_SINGLEPLAYER,
			SCREENTYPE_ONLINEPLAY,
			SCREENTYPE_ONLINEUI,
			SCREENTYPE_PAUSE,
			SCREENTYPE_POPUPS,
			SCREENTYPE_ALL,
		}
		SCREENTYPE;

	public:
		FrontEnd( void );
		virtual ~FrontEnd( void );
		void DeclareUberFile( const char* pFileName, GAMETYPE GameType );
		void DeclareScreenFile( const char* pFileName, GAMETYPE GameType );
		
		void SetGameType( GAMETYPE GameType );
		GAMETYPE GetGameType( void );
		virtual bool Init( void );
		virtual void Update( void );
		virtual void Shutdown( void );

		virtual bool GotoScreen( const char* pScreenName );
		virtual void SetInputFocus( void );
		virtual void ReleaseInputFocus( void );

		void LoadScreens( SCREENTYPE ScreenType );

		ScreenScoreBoard* GetScoreBoard( void );
		ScreenBasePopup* GetPopupScreen( const char* pScreenName );
		ScreenMemCard* GetMemCardScreen( void );
		ScreenPopupDialog* GetPopupDialog( void );
		ScreenErrorDialog* GetErrorDialog( void );
		void ShowPopupDialog( bool Show = true );
		void ShowErrorDialog( bool Show = true );

		void SetPopupTitle( const char* pTitle );
		void SetPopupMessage( const char* pMessage );

		void GotoScoreBoard( void );
		void ShowScoreBoard( bool Show );
		bool ScoreBoardUp();
		void ShowScoreBoardBk( bool Show );
		void ShowKeyboard( void );
		ScreenKeyboard* GetKeyboard( void );
		void ShowObjectives( bool Show );
		void ToggleObjectives( void );
		void ShowMissionBriefing( void );

		ScreenInvitations* GetInvite( void );
		void SetInvite( Invitation::Icon Icon, const char* pMessage );
		void ShowInviteIcon( unsigned char Color );
		void ShowInviteText( unsigned char Color );
		void UpdateInvite( void );

		void SetErrorMessage( const char* pMessage );
		void ShowErrorMessage( bool Show );

		void InitDebugOutput( void );
		void ShowDebugOutput( bool Show );
		void ToggleDebugOutput( void );
		void UpdateDebugOutput( void );
		int GetDebugOutputRows( void );
		bool IsDebugOutputShown( void ) {return m_ShowDebugOutput; }
		CDebugDisplay& GetDebugOutput( void ) {return m_DebugDisplay; }
		void	SetCinematicInProgress(bool Flag);
		bool	IsCinematicInProgress() { return m_CinematicInProgress; }

	public:
		FrontEndUIHandler m_UIHandler;
		Text m_IconText;
		t_Handle m_IconImages;

		float m_ScoreBoardTime;
		bool m_ControlsActive;
		bool m_ShowObjectives;
		bool m_ShowDebugOutput;
		bool m_ShowScoreBoard;
		Sprite m_DebugRect;
		Text m_DebugText[12];
		CDebugDisplay m_DebugDisplay;
		bool m_ControllerDisconnected;
		bool m_FrontEndWorld;
		GAMETYPE m_GameType;
		ScreenFile m_UberFiles;
		ScreenFile m_ScreenFiles;

	protected:
		bool InitScreenManager( ScreenManager* pScreenManager );
		void InitDeviceDrivers( void );
		void RegisterScreenHandlers( bool TrueOrFalse = true );
		void UnregisterScreenHandlers( void );
		void PreloadImages( void );
		bool InitScreens( void );
		void MemCardState();

	private:
		static FrontEnd* m_pPopupHandler;
		static void WaitUntilIdleCallBack( void );
	public:
		static void ShowPopupCallBack( void );
		static void HidePopupCallBack( void );
		static int  PopupResultCallBack( void );
		static void PopupClearResultCallback( void );

		int	m_MemCardState;
		bool	m_CinematicInProgress;
};

extern FrontEnd g_FrontEnd;

class GangstaButton : public SpriteInterface
{
	DECLARE_CLASSOBJECT( GangstaButton );

	public:
		virtual bool Init( void* Param );
};

extern	int	g_ControllerRemoved;
extern	bool	g_DisableControllerRemoved;

#endif // _FRONTEND_H_
